#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  Serial.println("Low Voltage");
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  Serial.println("High Voltage");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
}