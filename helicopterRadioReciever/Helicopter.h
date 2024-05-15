#ifndef Helicopter_h
#define Helicopter_h
#include "Arduino.h"
#include "Motor.h"

class Helicopter {
  public:
    Helicopter();
    void setTopMotor(int speed);
    void setBottomMotor(int speed);
    void setFrontGear(int speed);
    void setBackMotor(int speed, int roll);
    int getFrontGear();
    int getTopGear();
    int getBottomGear();
    int getBackGear();
    void setTopGear(int speed);
    void setBottomGear(int speed);
    void setBackGear(int thrust);
    int getTopSpeed();
    int getBottomSpeed();
    int getBackSpeed();
    void setRoll(int roll);
    void setFrontThrust(int speed);
    void setBackThrust(int speed);
    int getFrontThrust();
    int getBackThrust();
    void gearUp();
    void gearDown();

  private:
    Motor topMotor;
    Motor bottomMotor;
    Motor backMotor;
    int topGear;
    int bottomGear;
    int frontGear;
    int backGear;
    int topSpeed;
    int bottomSpeed;
    int backSpeed;
    int rightRoll;
    int leftRoll;

    int frontThrust;
    int backThrust;


    int getRightRoll();
    int getLeftRoll();
    int convertSpeedToGear(int speed);
};

#endif
