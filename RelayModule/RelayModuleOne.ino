
class RelayModuleOne {
  public:
    int pinReleLightKitchenSmall = 2;
    int pinSensMoveKitchenSmall = 3;
    int pinReleLightKitchenBig = 4;
    int pinSensMoveKitchenBig = 5;
};


class Timer {
  public:
    long secondCount = 0;
    long previousMillis = 0;
    unsigned long currentMillis = 0;
    long period = 100;
    long interval = 20000;
};


class MotionController {
  public:
    RelayModuleOne rmOne;
    Timer smallKitchenTimer;
    Timer bigKitchenTimer;

    bool isMotionSmallKitchen() {
      if (digitalRead(rmOne.pinSensMoveKitchenSmall) == HIGH) {
        return true;
      }
      if (digitalRead(rmOne.pinSensMoveKitchenSmall) == LOW) {
        return false;
      }
    };

    bool isMotionBigKitchen() {
      if (digitalRead(rmOne.pinSensMoveKitchenBig) == HIGH) {
        return true;
      }
      if (digitalRead(rmOne.pinSensMoveKitchenBig) == LOW) {
        return false;
      }
    };

    void lightUpSmallKitchen() {
      digitalWrite(rmOne.pinReleLightKitchenSmall, HIGH);
    };

    void lightUpBigKitchen() {
      digitalWrite(rmOne.pinReleLightKitchenBig, HIGH);
    };

    void lightOffSmallKitchen() {
      digitalWrite(rmOne.pinReleLightKitchenSmall, LOW);
    };

    void lightOffBigKitchen() {
      digitalWrite(rmOne.pinReleLightKitchenBig, LOW);
    };
};

MotionController mCntrl;

void setup() {
  Serial.begin(9600);
  pinMode(mCntrl.rmOne.pinReleLightKitchenSmall, OUTPUT);
  pinMode(mCntrl.rmOne.pinSensMoveKitchenSmall, INPUT);
  pinMode(mCntrl.rmOne.pinReleLightKitchenBig, OUTPUT);
  pinMode(mCntrl.rmOne.pinSensMoveKitchenBig, INPUT);
  digitalWrite(mCntrl.rmOne.pinReleLightKitchenSmall, LOW);
  digitalWrite(mCntrl.rmOne.pinReleLightKitchenSmall, LOW);
}




void loop() {

  mCntrl.smallKitchenTimer.currentMillis = millis();
  mCntrl.bigKitchenTimer.currentMillis = millis();


  //////////////////////////////////
  // small kitchen logic
  //////////////////////////////////

  if (mCntrl.isMotionSmallKitchen()) {
    mCntrl.lightUpSmallKitchen();
    mCntrl.smallKitchenTimer.secondCount = 0;
  }

  if (mCntrl.smallKitchenTimer.secondCount > mCntrl.smallKitchenTimer.interval) {
    mCntrl.lightOffSmallKitchen();
  }

  if (mCntrl.smallKitchenTimer.currentMillis - mCntrl.smallKitchenTimer.previousMillis > mCntrl.smallKitchenTimer.period) {
    mCntrl.smallKitchenTimer.previousMillis = mCntrl.smallKitchenTimer.currentMillis;
    mCntrl.smallKitchenTimer.secondCount += mCntrl.smallKitchenTimer.period;
  }


  //////////////////////////////////
  // big kitchen logic
  //////////////////////////////////

  if (mCntrl.isMotionBigKitchen()) {
    mCntrl.lightUpBigKitchen();
    mCntrl.bigKitchenTimer.secondCount = 0;
  }

  if (mCntrl.bigKitchenTimer.secondCount > mCntrl.bigKitchenTimer.interval) {
    mCntrl.lightOffBigKitchen();
  }

  if (mCntrl.bigKitchenTimer.currentMillis - mCntrl.bigKitchenTimer.previousMillis > mCntrl.bigKitchenTimer.period) {
    mCntrl.bigKitchenTimer.previousMillis = mCntrl.bigKitchenTimer.currentMillis;
    mCntrl.bigKitchenTimer.secondCount += mCntrl.bigKitchenTimer.period;
  }

  
}
