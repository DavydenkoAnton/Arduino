/*
  MOSI:  Arduino Nano D11
  MISO:  Arduino Nano D12
  SCK:   Arduino Nano D13
  CE:    Arduino Nano D2
  CSN:   Arduino Nano D3
*/


const int pinX2 = A1;
const int pinY2 = A0;
const int pinX1 = A2;
const int pinY1 = A3;
const int buttonPin = 2;



#include "SPI.h"
#include "RF24.h"
#include "nRF24L01.h"

#define CE_PIN 2
#define CSN_PIN 3

#define INTERVAL_MS_TRANSMISSION 250

RF24 radio(CE_PIN, CSN_PIN);

const byte address[6] = "00001";

//NRF24L01 buffer limit is 32 bytes (max struct size)
struct joystick {
  uint16_t x1;
  uint16_t y1;
  uint16_t x2;
  uint16_t y2;
};

joystick joystick;

void setup() {
  Serial.begin(115200);

  pinMode(pinX1, INPUT);
  pinMode(pinY1, INPUT);
  pinMode(pinX2, INPUT);
  pinMode(pinY2, INPUT);

  radio.begin();

  //Append ACK packet from the receiving radio back to the transmitting radio
  radio.setAutoAck(false); //(true|false)
  //Set the transmission datarate
  radio.setDataRate(RF24_1MBPS); //(RF24_250KBPS|RF24_1MBPS|RF24_2MBPS)
  //Greater level = more consumption = longer distance
  radio.setPALevel(RF24_PA_MAX); //(RF24_PA_MIN|RF24_PA_LOW|RF24_PA_HIGH|RF24_PA_MAX)
  //Default value is the maximum 32 bytes
  radio.setPayloadSize(sizeof(joystick));
  //Act as transmitter
  radio.openWritingPipe(address);
  radio.stopListening();
}

void loop(){

  

  joystick.x1 = analogRead(pinX1);
  joystick.y1 = analogRead(pinY1);
  joystick.x2 = analogRead(pinX2);
  joystick.y2 = analogRead(pinY2);

  radio.write(&joystick, sizeof(joystick));

  Serial.print("X1-");
  Serial.print(joystick.x1);
  Serial.print(" Y1-");
  Serial.print(joystick.y1);
  Serial.print(" X2-");
  Serial.print(joystick.x2);
  Serial.print(" Y2-");
  Serial.println(joystick.y2);

  //delay(INTERVAL_MS_TRANSMISSION);
}
