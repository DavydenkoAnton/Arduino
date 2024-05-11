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

/*pins*/
#define CE_PIN 2  // NRF24L
#define CSN_PIN 4 // NRF24L
#define PIN_ENA 3 // Вывод управления скоростью вращения мотора №1
#define PIN_ENB 9 // Вывод управления скоростью вращения мотора №2
#define PIN_IN1 5 // Вывод управления направлением вращения мотора №1
#define PIN_IN2 6 // Вывод управления направлением вращения мотора №1
#define PIN_IN3 7 // Вывод управления направлением вращения мотора №2
#define PIN_IN4 8 // Вывод управления направлением вращения мотора №2

#define INTERVAL_MS_SIGNAL_LOST 1000
#define INTERVAL_MS_SIGNAL_RETRY 250

RF24 radio(CE_PIN, CSN_PIN);

const byte address[6] = "00001";

int rightTrackSpeed = 0;
int leftTrackSpeed = 0;
int rightTrackDirection = 0;
int leftTrackDirection  = 0;

//NRF24L01 buffer limit is 32 bytes (max struct size)
struct joystick {
  int x1;
  int y1;
  int x2;
  int y2;
};
joystick joystick;

unsigned long lastSignalMillis = 0;




/*  INIT  */
void setup() {
  Serial.begin(115200);

  radio.begin();

  //Append ACK packet from the receiving radio back to the transmitting radio
  radio.setAutoAck(false); //(true|false)
  //Set the transmission datarate
  radio.setDataRate(RF24_1MBPS); //(RF24_250KBPS|RF24_1MBPS|RF24_2MBPS)
  //Greater level = more consumption = longer distance
  radio.setPALevel(RF24_PA_MAX); //(RF24_PA_MIN|RF24_PA_LOW|RF24_PA_HIGH|RF24_PA_MAX)
  //Default value is the maximum 32 bytes
  radio.setPayloadSize(sizeof(joystick));
  //Act as receiver
  radio.openReadingPipe(0, address);
  radio.startListening();

  // Установка всех управляющих пинов в режим выхода
  pinMode(PIN_ENA, OUTPUT);
  pinMode(PIN_ENB, OUTPUT);
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);
  // Команда остановки двум моторам
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, LOW);
}




/*  MAIN  */
void loop() {

  unsigned long currentMillis = millis();

  if (radio.available() > 0) {
    radio.read(&joystick, sizeof(joystick));

    RightTrackMotor();
    //LeftTrackMotor();


    lastSignalMillis = currentMillis;
    printRadioValue();
  }
  /*
    if (currentMillis - lastSignalMillis > INTERVAL_MS_SIGNAL_LOST) {
      lostConnection();
    }
  */
}

void printRadioValue() {
  Serial.print("x1:");
  Serial.print(joystick.x1);
  Serial.print(" y1:");
  Serial.print(joystick.y1);
  Serial.print(" x2:");
  Serial.print(joystick.x2);
  Serial.print(" y2:");
  Serial.println(joystick.y2);
}


void RightTrackMotor() {

  // get
  getRightTrackSpeed();
  //getRightTrackDirection();
  // set
  //setRightTrackSpeed();
  //setRightTrackDirection();
}


int getSpeed_Y() {
  int tmpSpeed_Y = joystick.y2;
  if (tmpSpeed_Y >= 0 && tmpSpeed_Y < 137) {
    tmpSpeed_Y = 3 ;
  } else if ( tmpSpeed_Y < 274) {
    tmpSpeed_Y = 2 ;
  } else if ( tmpSpeed_Y < 411) {
    tmpSpeed_Y = 1 ;
  } else if ( tmpSpeed_Y < 612) {
    tmpSpeed_Y = 0 ;
  } else if ( tmpSpeed_Y < 749) {
    tmpSpeed_Y = -1 ;
  } else if ( tmpSpeed_Y < 886) {
    tmpSpeed_Y = -2 ;
  } else if ( tmpSpeed_Y <= 1023) {
    tmpSpeed_Y = -3 ;
  }
  return tmpSpeed_Y;
}


int getSpeed_X() {
  int tmpSpeed_X = joystick.x2;
  if (tmpSpeed_X >= 0 && tmpSpeed_X < 137)  {
    tmpSpeed_X = -3 ;
  } else  if (tmpSpeed_X < 274)  {
    tmpSpeed_X = -2 ;
  } else  if (tmpSpeed_X < 411)  {
    tmpSpeed_X = -1 ;
  } else  if (tmpSpeed_X < 612)  {
    tmpSpeed_X = 0 ;
  } else  if (tmpSpeed_X < 749)  {
    tmpSpeed_X = 1 ;
  } else  if (tmpSpeed_X < 886)  {
    tmpSpeed_X = 2 ;
  } else  if (tmpSpeed_X <= 1023)  {
    tmpSpeed_X = 3 ;
  }
  return tmpSpeed_X;
}


void getRightTrackSpeed() {
  int speed_Y = getSpeed_Y();
  int speed_X = getSpeed_X();


  //  Serial.print("speed Y:");
  //  Serial.print(speed_Y);
  //  Serial.print("speed X:");
  //  Serial.println(speed_X);


  if (speed_Y == 0 ) {
    // Y0 +-X
    leftTrackSpeed = -speed_X;
    rightTrackSpeed = speed_X;
  }

  if (speed_X == 0 ) {
    // +-Y X0
    leftTrackSpeed = speed_Y;
    rightTrackSpeed = speed_Y;
  }







/*
  Serial.print(" speed LEFT:");
  Serial.print(leftTrackSpeed);
  Serial.print("speed RIGHT:");
  Serial.println(rightTrackSpeed);
  */

  //left track
  //speed
  if (leftTrackSpeed == -3 || leftTrackSpeed == 3) {
    analogWrite(PIN_ENB, 255);
  } else if (leftTrackSpeed == -2 || leftTrackSpeed == 2) {
    analogWrite(PIN_ENA, 180);
  } else if (leftTrackSpeed == -1 || leftTrackSpeed == 1) {
    analogWrite(PIN_ENA, 130);
  } else if (leftTrackSpeed == 0) {
    analogWrite(PIN_ENA, 0);
  }
  //direction
  if (leftTrackSpeed == 0) {
    // stop
    digitalWrite(PIN_IN1, LOW);
    digitalWrite(PIN_IN2, LOW);
  } else if (leftTrackSpeed > 0) {
    // forward
    digitalWrite(PIN_IN1, HIGH);
    digitalWrite(PIN_IN2, LOW);
  } else if (leftTrackSpeed < 0) {
    //  back
    digitalWrite(PIN_IN1, LOW);
    digitalWrite(PIN_IN2, HIGH);
  }


  //right track
  if (rightTrackSpeed == -3 || rightTrackSpeed == 3) {
    analogWrite(PIN_ENB, 255);
  } else if (rightTrackSpeed == -2 || rightTrackSpeed == 2) {
    analogWrite(PIN_ENB, 180);
  } else if (rightTrackSpeed == -1 || rightTrackSpeed == 1) {
    analogWrite(PIN_ENB, 130);
  } else if (rightTrackSpeed == 0) {
    analogWrite(PIN_ENB, 0);
  }
  //direction
  if (rightTrackSpeed == 0) {
    // stop
    digitalWrite(PIN_IN3, LOW);
    digitalWrite(PIN_IN4, LOW);
  } else if (rightTrackSpeed > 0) {
    // forward
    digitalWrite(PIN_IN3, HIGH);
    digitalWrite(PIN_IN4, LOW);
  } else if (rightTrackSpeed < 0) {
    //  back
    digitalWrite(PIN_IN3, LOW);
    digitalWrite(PIN_IN4, HIGH);
  }

}

void lostConnection() {
  Serial.println("We have lost connection, preventing unwanted behavior");
  delay(INTERVAL_MS_SIGNAL_RETRY);
}
