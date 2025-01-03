#include <Arduino.h>
#include "physicalClock.cpp"

// put function declarations here:
int myFunction(int, int);

PhysicalClock clock = PhysicalClock(0, 10, 11);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  clock.update();
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}