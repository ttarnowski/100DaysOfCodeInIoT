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

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(SSID, PASSWORD);
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
  int failedAttemptsNum = 0;
  while (WiFiMulti.run() != WL_CONNECTED && ++failedAttemptsNum <= 10) {
    delay(1000);
    Serial.print(".");
  }

  if (failedAttemptsNum >= 10) {
    Serial.println("Failed to connect to WiFi.");
    return;
  }

  WiFiClient client;

  HTTPClient http;

  Serial.print("[HTTP] begin...\n");
  // Establish the connection
  if (http.begin(client, "http://192.168.0.10:8080")) {

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