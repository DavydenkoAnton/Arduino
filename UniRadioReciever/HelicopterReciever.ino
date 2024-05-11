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

  //NRF24 INIT
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

    BottomMotor();
    //UpperMotor();


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


void lostConnection() {
  Serial.println("We have lost connection, preventing unwanted behavior");
  delay(INTERVAL_MS_SIGNAL_RETRY);
}
