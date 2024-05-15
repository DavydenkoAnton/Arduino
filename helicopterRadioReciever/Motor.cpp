#include "Arduino.h"
#include "Motor.h"

Motor::Motor () {
  this->speed = 0;
}

void Motor::init(int speedl) {
  this->speed = speed;
}

int Motor::getSpeed() {
  return this->speed;
}

void Motor::setSpeed(int speed) {
  this->speed = speed;
}
