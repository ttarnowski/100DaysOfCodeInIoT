#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  pinMode(D1, OUTPUT);
  pinMode(D5, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(D5) == HIGH) {
    Serial.println("High");
    digitalWrite(D1, LOW);
  } else {
    Serial.println("Low");
    digitalWrite(D1, HIGH);
  }
}