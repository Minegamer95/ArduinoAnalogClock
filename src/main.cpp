#include <Arduino.h>
#include "physicalClock.cpp"

// put function declarations here:
int myFunction(int, int);

PhysicalClock clock = PhysicalClock(0, 10, 11);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  Serial.begin(9600);
}

void loop() {
  clock.update();
  clock.setTargetTimeSec(millis());
  Serial.println(clock.getTimeStr(TimeType::Target));
  Serial.println(clock.getTargetTimeSec());
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}