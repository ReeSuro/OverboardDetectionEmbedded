#include <Arduino.h>

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
}

void loop() {
  digitalWrite(BUILTIN_LED, HIGH);
  delay(2000);
  digitalWrite(BUILTIN_LED, LOW);
  delay(2000);
}