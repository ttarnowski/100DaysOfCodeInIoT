#include <Arduino.h>

#define RELAY_PIN D1
#define BUTTON_PIN D5

void setup() {
  Serial.begin(115200);
  pinMode(D1, OUTPUT);
  pinMode(D5, INPUT_PULLUP);
}

unsigned long coffeeReadyTime = 0;
unsigned long buzzOffTime = 0;

void makeCoffee(unsigned long time = 10 * 60 * 1000) {
  Serial.println("Making coffee...");

  digitalWrite(D1, HIGH);

  coffeeReadyTime = millis() + time;
}

void notify(unsigned long time = 5 * 60 * 1000) {
  digitalWrite(D2, HIGH);

  buzzOffTime = millis() + time;
}

void isButtonPressed() {
  return digitalRead(D5) === LOW;
}

Timer timer;

void loop() {

  timer.tick();

  if (digitalRead(D1) == LOW && isButtonPressed()) {
    makeCoffee(5 * 1000);
    timer.setTimeout([]() {
    Serial.println("Coffee ready!");
    digitalWrite(D1, LOW);

    }, 5 * 1000);
  }

  if (digitalRead(D1) == HIGH && millis() > coffeeReadyTime) {
  }

  if (digitalRead(D2) == HIGH && millis() > buzzOffTime) {
    digitalWrite(D2, LOW);
  }
}
