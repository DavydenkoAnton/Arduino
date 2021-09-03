#include "ITimer.h"
#include "Arduino.h"


ITimer::ITimer() {}

ITimer::ITimer(uint32_t period) {
  //  this->divider = 1000;
  this->period = period * 1000;
  this->startTime = millis();
}

bool ITimer::endOfPeriod() {
  if ( millis() - this->startTime  >= this->period) {
    this->startTime = millis();
    return true;
  }
  return false;
}
