#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>;                 // Библиотека для работы с шиной SPI
#include <nRF24L01.h>;;           // Файл конфигурации для библиотеки RF24
#include <RF24.h>;                // Библиотека для работы с модулем NRF24L01
#include "ITimer.h"

#define MAIN_LEVEL_PIN 2
#define RESERVE_LEVEL_PIN 3
#define COOLER_VAPORIZER_PIN 4
#define SENSOR_COOLER_PIN 7

int humid, temp;
int count = 0;
int potValue[1]; // Создаём массив для приёма значений
byte pageIndex = 1 ;
String humidityData;
String temperatureData;
String ppmData;
String coolerData;
bool coolerBtnTouch = false;

LiquidCrystal_I2C lcd(0x27, 16, 2);
RF24 radio(9, 10);  //CE,CSN
ITimer pageTimer(5); // seconds
ITimer sensCoolerTimer(5); // seconds

void setup() {
  Serial.begin(115200);

  pinMode(SENSOR_COOLER_PIN, INPUT);

  lcd.init();
  lcd.backlight();// Включаем подсветку дисплея

  radio.begin();  // Инициализация модуля NRF24L01
  radio.setChannel(5); // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setDataRate (RF24_1MBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openReadingPipe (1, 0x7878787878LL); // Открываем трубу ID передатчика
  radio.startListening(); // Начинаем прослушивать открываемую трубу

  gtreetingPage();
}

/************** loop  *****************/
void loop() {  
  pageSwitcher();
  getNrf24Data();
  buttonListener();
}

/************** page adapter  *****************/
void buttonListener(){
  if(sensCoolerTimer.endOfPeriod()){
      coolerBtnTouch = true;
    }
  if(digitalRead(SENSOR_COOLER_PIN)==HIGH && coolerBtnTouch){
      coolerBtnTouch = false;
    }
}
/************** page adapter  *****************/
void pageSwitcher(){
  if (pageTimer.endOfPeriod()) {
    switch (pageIndex) {
      case 1:
        humidityPage();
        pageIndex++;
        break;
      case 2:
        temperaturePage();
        pageIndex++;
        break;
      case 3:
        ppmPage();
        pageIndex++;
        break;
      case 4:
        coolerPage();
        pageIndex = 1;
        break;
    }
  }
}

void gtreetingPage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    welcome!");
}

void humidityPage() {
  lcd.clear();
  printLcdHumidity();
  lcd.setCursor(5, 0);
  lcd.print(humidityData);
}

void temperaturePage() {
  lcd.clear();
  printLcdTemperature();
  lcd.setCursor(5, 0);
  lcd.print(temperatureData);
}

void ppmPage() {
  lcd.clear();
  printLcdPPM();
  lcd.setCursor(5, 0);
  lcd.print(ppmData);
}

void coolerPage() {
  lcd.clear();
  printLcdCooler();
  lcd.setCursor(5, 0);
  lcd.print(coolerData);
}
/************** nrf adapter  *****************/
void getNrf24Data() {
  if (radio.available()) { // Если в буфер приёмника поступили данные
    char msg[32];
    String s = "";
    radio.read( &msg, sizeof(msg) );
    for (int i = 0; i < 32; i++) {
      s += msg[i];
    }
    if (s.startsWith("hum")) {
      humidityData = s.substring(3, 7);
    } else if (s.startsWith("temp")) {
      temperatureData = s.substring(4, 8);
    } else if (s.startsWith("ppm")) {
      Serial.print(s);
      ppmData = s.substring(3, 10);
    } else if (s.startsWith("cooler")) {
      coolerData = "no data";
    }
  }
}
/************** print lcd adapter  *****************/
void printLcdHumidity() {
  byte humidity[8] = {B00100,  B00100, B01110,  B11111,  B11111,  B11111,  B01110,  B00000};
  lcd.createChar(1, humidity);
  lcd.setCursor(1, 0);
  lcd.write(1);
  lcd.setCursor(3, 0);
  lcd.write(1);
  lcd.setCursor(2, 1);
  lcd.write(1);
}

void printLcdTemperature() {
  byte capsule[8] = { B01010,  B01010,  B01010,  B11011,  B10001,  B11011,  B01110,  B00000};
  byte stick[8] = {B00000,  B01110,  B01110,  B01110,  B01110,  B01010,  B01010,  B01010};
  lcd.createChar(1, capsule);
  lcd.createChar(2, stick);
  lcd.setCursor(2, 1);
  lcd.write(1);
  lcd.setCursor(2, 0);
  lcd.write(2);
}

void printLcdPPM() {
  byte p1[8] = { B00000,  B00001,  B00000,  B00000,  B00100,  B00000,  B00100,  B00000};
  byte p2[8] = { B00000,  B00000,  B00000,  B00100,  B00001,  B00000,  B00000,  B00001};
  byte p3[8] = { B00100,  B00000,  B01000,  B00000,  B00000,  B00000,  B00100,  B00000};
  byte p4[8] = { B00000,  B00001,  B00000,  B00000,  B00000,  B00000,  B00000,  B00000};
  byte p5[8] = { B00100,  B00001,  B01000,  B00000,  B10100,  B00000,  B00100,  B00000};
  byte p6[8] = { B00100,  B00001,  B00000,  B00000,  B00000,  B00000,  B00000,  B00000};

  lcd.createChar(1, p1);
  lcd.createChar(2, p2);
  lcd.createChar(3, p3);
  lcd.createChar(4, p4);
  lcd.createChar(5, p5);
  lcd.createChar(6, p6);

  lcd.setCursor(1, 0);
  lcd.write(1);
  lcd.setCursor(2, 0);
  lcd.write(2);
  lcd.setCursor(3, 0);
  lcd.write(3);
  lcd.setCursor(1, 1);
  lcd.write(4);
  lcd.setCursor(2, 1);
  lcd.write(5);
  lcd.setCursor(3, 1);
  lcd.write(6);
}

void printLcdCooler() {
  byte p1[8] = { B00000,  B00000,  B00000,  B00011,  B00111,  B00111,  B00011,  B00001};
  byte p2[8] = { B00000,  B00000,  B00000,  B00000,  B00000,  B00110,  B01111,  B11111};
  byte p3[8] = { B10000,  B11000,  B11100,  B11100,  B11000,  B00000,  B00000,  B00000};
  byte p4[8] = { B11111,  B11110,  B01100,  B00000,  B00000,  B00000,  B00000,  B00000};

  lcd.createChar(1, p1);
  lcd.createChar(2, p2);
  lcd.createChar(3, p3);
  lcd.createChar(4, p4);

  lcd.setCursor(1, 0);
  lcd.write(1);
  lcd.setCursor(2, 0);
  lcd.write(2);
  lcd.setCursor(2, 1);
  lcd.write(3);
  lcd.setCursor(1, 1);
  lcd.write(4);
}
