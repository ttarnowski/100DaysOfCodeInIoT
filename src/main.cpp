#include <Arduino.h>

#include "DHTesp.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>
#include <EventDispatcher.hpp>
#include <SingleHostHTTPSClient.hpp>
#include <Timer.hpp>
#include <WiFiManager.hpp>
#include <getTemperature.hpp>

#define RELAY_PIN D1
#define TEMP_SENSOR_PIN D6

#define TEMP_RANGE 0.25

// Local WiFi Network credentials
#define SSID "myssid"
#define PASSWORD "mypassword"

const char *cert PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

Timer timer;
EventDispatcher dispatcher;
ESP8266WiFiMulti wifiMulti;
WiFiManager wifiManager(&wifiMulti, &dispatcher, &timer, SSID, PASSWORD);
DHTesp dht;
SingleHostHTTPSClient
    httpsClient("5dsx4lgd5l.execute-api.us-east-1.amazonaws.com", cert,
                &wifiManager, &timer);

bool isRelayOn() { return digitalRead(RELAY_PIN) == HIGH; }

bool isRelayOff() { return !isRelayOn(); }

void switchOnTheRelay() {
  Serial.println("Switching on the relay");
  digitalWrite(RELAY_PIN, HIGH);
}

void switchOffTheRelay() {
  Serial.println("Switching relay off");
  digitalWrite(RELAY_PIN, LOW);
}

void loadTemperature() {
  timer.setTimeout(
      []() {
        getTemperature(&httpsClient, [](const char *error,
                                        float desiredTemperature) {
          if (error != nullptr) {
            Serial.printf("Loading temperature failed due to '%s'\n", error);
          } else {
            delay(dht.getMinimumSamplingPeriod());
            float sensorTemperature = dht.getTemperature();

            Serial.printf("Desired temp.: %.2f Sensor temp.: %.2f \n",
                          desiredTemperature, sensorTemperature);

            if (sensorTemperature < desiredTemperature - TEMP_RANGE) {
              switchOnTheRelay();
            }

            if (sensorTemperature > desiredTemperature + TEMP_RANGE) {
              switchOffTheRelay();
            }
          }

          wifiManager.disconnect();
          loadTemperature();
        });
      },
      30 * 1000);
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  dht.setup(TEMP_SENSOR_PIN, DHTesp::DHT11);

  delay(1000);

  dispatcher.on<WiFiManager::WiFiConnectedEvent>(
      [](WiFiManager::WiFiConnectedEvent *) {
        digitalWrite(LED_BUILTIN, HIGH);
      });

  dispatcher.on<WiFiManager::WiFiDisconnectedEvent>(
      [](WiFiManager::WiFiDisconnectedEvent *) {
        digitalWrite(LED_BUILTIN, LOW);
      });

  loadTemperature();
}

void loop() { timer.tick(); }
