/*
  nrf24l  ->   Nano
  MOSI:   ->   Arduino Nano D11
  MISO:   ->   Arduino Nano D12
  SCK:    ->   Arduino Nano D13
  CE:     ->   Arduino Nano D2
  CSN:    ->   Arduino Nano D4
*/

/*libs*/
#include "SPI.h"
#include "RF24.h"
#include "nRF24L01.h"
#include "Helicopter.h"


/*pins*/
#define PIN_CE 2            // NRF24L
#define PIN_CSN 4           // NRF24L
#define PIN_PWM_BOTTOM 9    // Шим мотора №1
#define PIN_DGT_BOTTOM 7    // Логический мотора №1
#define PIN_PWM_TOP 10      // Шим мотора №2
#define PIN_DGT_TOP 8       // Логический мотора №2
#define PIN_PWM_BACK_1 3    // Шим мотора №3
#define PIN_PWM_BACK_2 5    // Логический мотора №3

#define INTERVAL_MS_SIGNAL_LOST 1000
#define INTERVAL_MS_SIGNAL_RETRY 250

RF24 radio(PIN_CE, PIN_CSN);

const byte address[6] = "00001";

//NRF24L01 buffer limit is 32 bytes (max struct size)
struct Joystick {
  int x1;
  int y1;
  int x2;
  int y2;
}; Joystick joystick;

Helicopter helicopter;

unsigned long lastSignalMillis = 0;
unsigned long currentMillis = 0;
unsigned long gearInterval = 0;
unsigned long interval = 0;


/*  INIT  */
void setup() {
  Serial.begin(115200);

  radio.begin();

  //NRF24 INIT
  //Append ACK packet from the receiving radio back to the transmitting radio
  radio.setAutoAck(false); //(true|false)
  //Set the transmission datarate
  radio.setDataRate(RF24_1MBPS); //(RF24_250KBPS|RF24_1MBPS|RF24_2MBPS)
  //Greater level = more consumption = longer distance
  radio.setPALevel(RF24_PA_MAX); //(RF24_PA_MIN|RF24_PA_LOW|RF24_PA_HIGH|RF24_PA_MAX)
  //Default value is the maximum 32 bytes
  radio.setPayloadSize(sizeof(Joystick));
  //Act as receiver
  radio.openReadingPipe(0, address);
  radio.startListening();

  // Установка всех управляющих пинов в режим выхода
  pinMode(PIN_PWM_BOTTOM, OUTPUT);
  pinMode(PIN_DGT_BOTTOM, OUTPUT);
  pinMode(PIN_PWM_TOP, OUTPUT);
  pinMode(PIN_DGT_TOP, OUTPUT);
  pinMode(PIN_PWM_BACK_1, OUTPUT);
  pinMode(PIN_PWM_BACK_2, OUTPUT);

  // Команда остановки 3 моторам
  digitalWrite(PIN_PWM_BOTTOM, LOW);
  digitalWrite(PIN_DGT_BOTTOM, LOW);
  digitalWrite(PIN_PWM_TOP, LOW);
  digitalWrite(PIN_DGT_TOP, LOW);
  digitalWrite(PIN_PWM_BACK_1, LOW);
  digitalWrite(PIN_PWM_BACK_2, LOW);

  currentMillis = millis();
}




/*  MAIN  */
void loop() {

  lastSignalMillis = currentMillis;

  if (radio.available() > 0) {

    radio.read(&joystick, sizeof(joystick));

    helicopter.setFrontThrust(joystick.y2);
    helicopter.setBackThrust(joystick.y1);
    helicopter.setRoll(joystick.x1);


    if (millis() - interval > gearInterval) {
      interval = millis();
      if (helicopter.getFrontThrust() == 0) {
        gearInterval = 1000;
      } else if (helicopter.getFrontThrust() > 0) {
        switch (helicopter.getFrontThrust()) {
          case 0:
            gearInterval = 0;
            break;
          case 1:
            gearInterval = 875;
            break;
          case 2:
            gearInterval = 750;
            break;
          case 3:
            gearInterval = 625;
            break;
          case 4:
            gearInterval = 500;
            break;
          case 5:
            gearInterval = 375;
            break;
          case 6:
            gearInterval = 250;
            break;
          case 7:
            gearInterval = 125;
            break;
          case 8:
            gearInterval = 0;
            break;
        }
        helicopter.gearUp();
      } else if (helicopter.getFrontThrust() < 0) {
        switch (helicopter.getFrontThrust()) {
          case 0:
            gearInterval = 0;
            break;
          case -1:
            gearInterval = 875;
            break;
          case -2:
            gearInterval = 750;
            break;
          case -3:
            gearInterval = 625;
            break;
          case -4:
            gearInterval = 500;
            break;
          case -5:
            gearInterval = 375;
            break;
          case -6:
            gearInterval = 250;
            break;
          case -7:
            gearInterval = 125;
            break;
          case -8:
            gearInterval = 0;
            break;
        }
        helicopter.gearDown();
      }
    }

    Serial.print("top:");
    Serial.print(helicopter.getTopSpeed());
    Serial.print(" bottom:");
    Serial.print(helicopter.getBottomSpeed());
    Serial.print(" back:");
    Serial.println(helicopter.getBackSpeed());


    if (helicopter.getFrontGear() > 0) {
      analogWrite(PIN_PWM_TOP, helicopter.getTopSpeed());
      digitalWrite(PIN_DGT_TOP, HIGH);
      analogWrite(PIN_PWM_BOTTOM, helicopter.getBottomSpeed());
      digitalWrite(PIN_DGT_BOTTOM, HIGH);
    } else {
      analogWrite(PIN_PWM_TOP, LOW);
      digitalWrite(PIN_DGT_TOP, LOW);
      analogWrite(PIN_PWM_BOTTOM, LOW);
      digitalWrite(PIN_DGT_BOTTOM, LOW);
    }

    if (helicopter.getBackGear() > 0) {
      analogWrite(PIN_PWM_BACK_1, helicopter.getBackSpeed());
      analogWrite(PIN_PWM_BACK_2, HIGH);
    } else if (helicopter.getBackGear() < 0) {
      analogWrite(PIN_PWM_BACK_1, HIGH);
      analogWrite(PIN_PWM_BACK_2, helicopter.getBackSpeed());
    } else {
      analogWrite(PIN_PWM_BACK_1, LOW);
      analogWrite(PIN_PWM_BACK_2, LOW);
    }


    //    Serial.print("top:");
    //    Serial.print(helicopter.getTopSpeed());
    //    Serial.print(" bottom:");
    //    Serial.print(helicopter.getBottomSpeed());
    //    Serial.print(" back:");
    //    Serial.println(helicopter.getBackSpeed());

  }

  /*
    if (currentMillis - lastSignalMillis > INTERVAL_MS_SIGNAL_LOST) {
      lostConnection();
    }
  */

  //printRadioData();
}




void printRadioData() {
  Serial.print("x1:");
  Serial.print(joystick.x1);
  Serial.print(" y1:");
  Serial.print(joystick.y1);
  Serial.print(" x2:");
  Serial.print(joystick.x2);
  Serial.print(" y2:");
  Serial.println(joystick.y2);
}


void lostConnection() {
  Serial.println("We have lost connection, preventing unwanted behavior");
  delay(INTERVAL_MS_SIGNAL_RETRY);
}
