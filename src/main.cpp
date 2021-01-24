#include <Arduino.h>

#define RELAY_PIN D1
#define BUTTON_PIN D5
#define BUZZER_PIN D2

#define COFFEE_MAKING_TIME 10 * 60 * 1000
#define SOUND_NOTIFICATION_TIME 5 * 1000

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

unsigned long coffeeReadyTime;
unsigned long buzzerOffTime;

bool isButtonPressed() {
  return digitalRead(BUTTON_PIN) == LOW;
}

bool isRelayOn() {
  return digitalRead(RELAY_PIN) == HIGH;
}

bool isRelayOff() {
  return !isRelayOn();
}

bool isBuzzerOn() {
  return digitalRead(BUZZER_PIN) == HIGH;
}

void switchOnTheRelay() {
  Serial.println("Switching on the relay");
  digitalWrite(RELAY_PIN, HIGH);
  coffeeReadyTime = millis() + COFFEE_MAKING_TIME;
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

void loop() {
  if (isRelayOff() && isButtonPressed()) {
    switchOnTheRelay();
  }

  if (isRelayOn() && coffeeReadyTime < millis()) {
    switchOffTheRelay();
    switchOnTheBuzzer();
  }

  if (isBuzzerOn() && buzzerOffTime < millis()) {
    switchOffTheBuzzer();
  }
}