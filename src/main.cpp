#include <Arduino.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;

#define RELAY_PIN D1
#define BUTTON_PIN D5
#define BUZZER_PIN D2

#define RELAY_TIMER_TIME 5 * 1 * 1000
#define SOUND_NOTIFICATION_TIME 2 * 1000

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

bool connectToWiFi() {
  int failedAttemptsNum = 0;
  while (WiFiMulti.run() != WL_CONNECTED && ++failedAttemptsNum <= 10) {
    delay(1000);
    Serial.print(".");
  }

  if (failedAttemptsNum >= 10) {
    Serial.println("Failed to connect to WiFi.");
    return false;
  }

  return true;
}

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

  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(SSID, PASSWORD);

  if (connectToWiFi()) {
    setClock();
    WiFi.disconnect();
  }
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

void sendHttpRequest() {
  if (!connectToWiFi()) {
    return;
  }

  setClock();

  BearSSL::WiFiClientSecure client;
  BearSSL::X509List list(cert);
  client.setTrustAnchors(&list);

  HTTPClient http;

  Serial.print("[HTTP] begin...\n");
  // Establish the connection
  if (http.begin(
          client,
          "https://5dsx4lgd5l.execute-api.us-east-1.amazonaws.com/dev/hello")) {

    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header, set the HTTP method and request
    // body
    int httpCode = http.POST("{\"message\":\"hello from ESP8266\"}");

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        // read response body as a string
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      // print out the error message
      Serial.printf("[HTTP] POST... failed, error: %s\n",
                    http.errorToString(httpCode).c_str());
    }

    // finish the exchange
    http.end();
  } else {
    Serial.printf("[HTTP] Unable to connect\n");
  }

  WiFi.disconnect();
}

void loop() {
  if (isRelayOff() && isButtonPressed()) {
    switchOnTheRelay();
  }

  if (isRelayOn() && relayTime < millis()) {
    switchOffTheRelay();
    switchOnTheBuzzer();
    sendHttpRequest();
  }

  if (isBuzzerOn() && buzzerOffTime < millis()) {
    switchOffTheBuzzer();
  }
}