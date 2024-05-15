#ifndef Motor_h
#define Motor_h
#include "Arduino.h"

class Motor {
  public:

    Motor();

    void init(int speed) ;
    int getSpeed();
    void setSpeed(int speed);


  private:
    int speed;
};

#endif
