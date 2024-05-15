#include "Arduino.h"
#include "Helicopter.h"
#include "Motor.h"

Helicopter::Helicopter () {
  this->frontGear = 0;
  this->backGear = 0;
  this->frontThrust = 0;
  this->backThrust = 0;
}

void Helicopter::setTopMotor(int speed) {
  setTopGear(speed);
}

void Helicopter::setBottomMotor(int speed) {
  setBottomGear(speed);
}

void Helicopter::setBackMotor(int speed, int roll) {
  setRoll(roll);
  setBackGear(speed);
}

////// gear ///////////

int Helicopter::getTopGear() {
  return this->topGear;
}

void Helicopter::setTopGear(int speed) {
  setFrontGear(speed);
  topGear = this->frontGear - this->leftRoll;
}

int Helicopter::getBottomGear() {
  return this->bottomGear;
}

void Helicopter::setBottomGear(int speed) {
  setFrontGear(speed);
  this->bottomGear = frontGear  - this->rightRoll;
}

int Helicopter::getFrontGear() {
  return this->frontGear;
}

void Helicopter::setFrontGear(int speed) {

}

void Helicopter::gearUp() {
  if (this->frontGear < 8) {
    frontGear++;
  }
}

void Helicopter::gearDown() {
  if (this->frontGear > 0) {
    frontGear--;
  }
}

int Helicopter::getBackGear() {
  return this->backGear;
}

void Helicopter::setBackGear(int thrust) {
}

void Helicopter::setFrontThrust(int speed) {
  if (speed <= 61 ) {
    this->frontThrust = 8;
  } else if (speed <= 122) {
    this->frontThrust = 7;
  } else if (speed <= 183) {
    this->frontThrust = 6;
  } else if (speed <= 244) {
    this->frontThrust = 5;
  } else if (speed <= 305) {
    this->frontThrust = 4;
  } else if (speed <= 366) {
    this->frontThrust = 3;
  } else if (speed <= 427) {
    this->frontThrust = 2;
  } else if (speed <= 488) {
    this->frontThrust = 1;
  } else if (speed <= 535) {
    this->frontThrust = 0;
  } else if (speed <= 596) {
    this->frontThrust = -1;
  } else if (speed <= 657) {
    this->frontThrust = -2;
  } else if (speed <= 718) {
    this->frontThrust = -3;
  } else if (speed <= 779) {
    this->frontThrust = -4;
  } else if (speed <= 840) {
    this->frontThrust = -5;
  } else if (speed <= 901) {
    this->frontThrust = -6;
  } else if (speed <= 966) {
    this->frontThrust = -7;
  } else if (speed <= 1023) {
    this->frontThrust = -8;
  }
}

int Helicopter::getFrontThrust() {
  return this->frontThrust;
}

void Helicopter::setBackThrust(int speed) {
  if (speed <= 100) {
    this->backThrust = 4;
    this->backGear = 4;
  } else if (speed <= 200) {
    this->backThrust = 3;
    this->backGear = 3;
  } else if (speed <= 300) {
    this->backThrust = 2 ;
    this->backGear = 2;
  } else if (speed <= 400) {
    this->backThrust = 1;
    this->backGear = 1;
  } else if (speed <= 600) {
    this->backThrust = 0;
    this->backGear = 0;
  } else if (speed <= 700) {
    this->backThrust = -1;
    this->backGear = -1;
  } else if (speed <= 800) {
    this->backThrust = -2;
    this->backGear = -2;
  } else if (speed <= 900) {
    this->backThrust = -3;
    this->backGear = -3;
  } else if (speed <= 1023) {
    this->backThrust = -4;
    this->backGear = -4;
  }
}

int Helicopter::getBackThrust() {
  return this->backThrust;
}

int Helicopter::getTopSpeed() {
  if ((this->frontGear - this->leftRoll) > 0) {
    switch (this->frontGear - this->leftRoll) {
      case 1:
        return 129;
        break;
      case 2:
        return 147;
        break;
      case 3:
        return 165;
        break;
      case 4:
        return 183;
        break;
      case 5:
        return 201;
        break;
      case 6:
        return 219;
        break;
      case 7:
        return 237;
        break;
      case 8:
        return 255;
        break;
    }
  }
  return 0;
}

int Helicopter::getBottomSpeed() {
  if ((this->frontGear - this->rightRoll) > 0) {
    switch (this->frontGear - this->rightRoll) {
      case 1:
        return 129;
        break;
      case 2:
        return 147;
        break;
      case 3:
        return 165;
        break;
      case 4:
        return 183;
        break;
      case 5:
        return 201;
        break;
      case 6:
        return 219;
        break;
      case 7:
        return 237;
        break;
      case 8:
        return 255;
        break;
    }
  }
  return 0;
}

int Helicopter::getBackSpeed() {
  if (backGear == 1 || backGear == -1) {
    return 130;
  } else if (backGear == 2 || backGear == -2) {
    return 170;
  } else if (backGear == 3 || backGear == -3) {
    return 210;
  } else if (backGear == 4 || backGear == -4) {
    return 255;
  }
  return 0;
}



//////// roll  //////////////

int Helicopter::getRightRoll() {
  return this->rightRoll;
}

int Helicopter::getLeftRoll() {
  return this->leftRoll;
}

void Helicopter::setRoll(int roll) {

  if (roll >= 384 && roll <= 640) {
    this->rightRoll = 0;
    this->leftRoll = 0;
  }

  if (roll > 640 && roll <= 1023) {
    if (roll <= 768) {
      this->leftRoll = 1;
    } else if (roll <= 896) {
      this->leftRoll = 2;
    } else if (roll <= 1023) {
      this->leftRoll = 3;
    }
  }

  if (roll >= 0 && roll < 384) {
    if (roll < 128) {
      this->rightRoll = 3;
    } else if (roll < 256) {
      this->rightRoll = 2;
    } else if (roll < 384) {
      this->rightRoll = 1;
    }
  }
}
