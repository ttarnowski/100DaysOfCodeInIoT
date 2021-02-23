#include <Arduino.h>

#include "DHTesp.h"
#include <APIGatewayTemperatureClient.hpp>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>
#include <EventDispatcher.hpp>
#include <WiFiManager.hpp>

#define RELAY_PIN D1
#define BUTTON_PIN D5
#define BUZZER_PIN D2
#define TEMP_SENSOR_PIN D6

#define RELAY_TIMER_TIME 5 * 1 * 1000
#define SOUND_NOTIFICATION_TIME 2 * 1000

// Local WiFi Network credentials
#define SSID "myssid"
#define PASSWORD "mypassword"

EventDispatcher dispatcher;
ESP8266WiFiMulti wifiMulti;
WiFiManager wifiManager(&wifiMulti, &dispatcher, SSID, PASSWORD);
APIGatewayTemperatureClient temperatureClient(&dispatcher, &wifiManager);
DHTesp dht;

float desiredTemperature = 20;

void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  dht.setup(TEMP_SENSOR_PIN, DHTesp::DHT11);

  delay(1000);

  dispatcher.on<WiFiManager::WiFiConnectedEvent>(
      [](WiFiManager::WiFiConnectedEvent *) {
        setClock();
        digitalWrite(LED_BUILTIN, HIGH);
      });

  dispatcher.on<WiFiManager::WiFiDisconnectedEvent>(
      [](WiFiManager::WiFiDisconnectedEvent *) {
        digitalWrite(LED_BUILTIN, LOW);
      });

  dispatcher.once<WiFiManager::WiFiConnectedEvent>(
      [](WiFiManager::WiFiConnectedEvent *) { wifiManager.disconnect(); });

  wifiManager.connect();

  dispatcher.on<APIGatewayTemperatureClient::TemperatureReceivedEvent>(
      [](APIGatewayTemperatureClient::TemperatureReceivedEvent *e) {
        desiredTemperature = e->temperature;
      });
}

unsigned long relayTime;
unsigned long buzzerOffTime;

bool isButtonPressed() { return digitalRead(BUTTON_PIN) == LOW; }

bool isRelayOn() { return digitalRead(RELAY_PIN) == HIGH; }

bool isRelayOff() { return !isRelayOn(); }

bool isBuzzerOn() { return digitalRead(BUZZER_PIN) == HIGH; }

void switchOnTheRelay() {
  Serial.println("Switching on the relay");
  digitalWrite(RELAY_PIN, HIGH);
  relayTime = millis() + RELAY_TIMER_TIME;
}

void switchOffTheRelay() {
  Serial.println("Switching relay off");
  digitalWrite(RELAY_PIN, LOW);
}

void switchOnTheBuzzer() {
  Serial.println("Switching on the buzzer");
  digitalWrite(BUZZER_PIN, HIGH);
  buzzerOffTime = millis() + SOUND_NOTIFICATION_TIME;
}

void switchOffTheBuzzer() {
  Serial.println("Switching off the buzzer");
  digitalWrite(BUZZER_PIN, LOW);
}

float t = 0.25;

void loop() {
  wifiManager.loop();
  temperatureClient.loadTemperature();

  delay(dht.getMinimumSamplingPeriod());
  float sensorTemperature = dht.getTemperature();

  Serial.printf("Desired temp.: %.2f Sensor temp.: %.2f \n", desiredTemperature,
                sensorTemperature);

  if (sensorTemperature < desiredTemperature - t) {
    switchOnTheRelay();
  }

  if (sensorTemperature > desiredTemperature + t) {
    switchOffTheRelay();
  }
}
