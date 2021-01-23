#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  pinMode(D1, OUTPUT);
  pinMode(D5, INPUT);
}

void loop() {
  if (digitalRead(D5) == HIGH) {
    Serial.println("high");
    digitalWrite(D1, HIGH);
  } else {
    Serial.println("low");
    digitalWrite(D1, LOW);
  }
}