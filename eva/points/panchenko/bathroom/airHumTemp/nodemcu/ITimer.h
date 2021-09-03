#ifndef ITIMER_H
#define ITIMER_H

#include <Arduino.h>

class ITimer {

  public:
    ITimer();
    ITimer(uint32_t period);
    bool endOfPeriod();

  private:
    uint32_t startTime;
    uint32_t period;
};

#endif
