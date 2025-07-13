#include <Arduino.h>
#include "GY-521.h"

#define BAUD_RATE 9600
int gyroResult;

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  gyroResult = setupGyro();

  Serial.begin(BAUD_RATE);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(gyroResult);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(300);
  digitalWrite(LED_BUILTIN, LOW);
  delay(300);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}