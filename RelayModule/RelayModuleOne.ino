
class RelayModuleOne {
  public:
    int pinReleLightKitchenSmall_12 = 2;
    int pinReleLightKitchenSmall_220 = 3;
    int pinReleLightKitchenBig_220 = 5;
    int pinSensMoveKitchenSmall = 4;
    int pinSensMoveKitchenBig = 6;
};


class Timer {
  public:
    long secondCount = 0;
    long previousMillis = 0;
    unsigned long currentMillis = 0;
    long period = 100;
    long interval = 0;
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
      digitalWrite(rmOne.pinReleLightKitchenSmall_12, HIGH);
      digitalWrite(rmOne.pinReleLightKitchenSmall_220, HIGH);
    };

    void lightUpBigKitchen() {
      digitalWrite(rmOne.pinReleLightKitchenBig_220, HIGH);
    };

    void lightOffSmallKitchen() {
      digitalWrite(rmOne.pinReleLightKitchenSmall_12, LOW);
      digitalWrite(rmOne.pinReleLightKitchenSmall_220, LOW);
    };

    void lightOffBigKitchen() {
      digitalWrite(rmOne.pinReleLightKitchenBig_220, LOW);
    };
};

MotionController mCntrl;

void setup() {
  Serial.begin(9600);
  pinMode(mCntrl.rmOne.pinReleLightKitchenSmall_12, OUTPUT);
  pinMode(mCntrl.rmOne.pinReleLightKitchenSmall_220, OUTPUT);
  pinMode(mCntrl.rmOne.pinSensMoveKitchenSmall, INPUT);
  pinMode(mCntrl.rmOne.pinReleLightKitchenBig_220, OUTPUT);
  pinMode(mCntrl.rmOne.pinSensMoveKitchenBig, INPUT);
  digitalWrite(mCntrl.rmOne.pinReleLightKitchenSmall_12, LOW);
  digitalWrite(mCntrl.rmOne.pinReleLightKitchenSmall_220, LOW);
  digitalWrite(mCntrl.rmOne.pinReleLightKitchenBig_220, LOW);
  mCntrl.smallKitchenTimer.interval=30000;
  mCntrl.bigKitchenTimer.interval=60000;
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
