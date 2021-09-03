#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>;                 // Библиотека для работы с шиной SPI
#include <nRF24L01.h>;;           // Файл конфигурации для библиотеки RF24
#include <RF24.h>;                // Библиотека для работы с модулем NRF24L01



#define MAIN_LEVEL_PIN 2
#define RESERVE_LEVEL_PIN 3
#define COOLER_VAPORIZER_PIN 4
#define PUMP_PIN 7


int humid, temp;
int count = 0;
int potValue[1]; // Создаём массив для приёма значений

LiquidCrystal_I2C lcd(0x27, 16, 2);
RF24 radio(9, 10);  //CE,CSN


void setup() {
  Serial.begin(115200);

  pinMode(COOLER_VAPORIZER_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(RESERVE_LEVEL_PIN, INPUT);
  pinMode(MAIN_LEVEL_PIN, INPUT);

  lcd.init();
  lcd.backlight();// Включаем подсветку дисплея
  lcd.setCursor(0, 0);

  radio.begin();  // Инициализация модуля NRF24L01
  radio.setChannel(5); // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setDataRate (RF24_1MBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openReadingPipe (1, 0x7878787878LL); // Открываем трубу ID передатчика
  radio.startListening(); // Начинаем прослушивать открываемую трубу
}


void loop() {
  firstLaunch();
  getHumidTempData();

  /*
    if (humid < 40 && !isCooler) {
      isCooler = true;
      digitalWrite(COOLER_VAPORIZER_PIN, LOW);
      digitalWrite(PUMP_PIN, LOW);//on
      Serial.print(humid);
      Serial.print(" ");
      Serial.println(temp);
    }
    if (humid > 45 && isCooler) {
      isCooler = false;
      digitalWrite(COOLER_VAPORIZER_PIN, HIGH);
      digitalWrite(PUMP_PIN, HIGH);//off
      Serial.print(humid);
      Serial.print(" ");
      Serial.println(temp);
    }

    if (digitalRead(RESERVE_LEVEL_PIN) == HIGH) {
      digitalWrite(PUMP_PIN, HIGH);//off
      digitalWrite(COOLER_VAPORIZER_PIN, HIGH);//off
    } else {
      digitalWrite(PUMP_PIN, LOW);//on
      digitalWrite(COOLER_VAPORIZER_PIN, LOW);//on
    }
  */
}


void firstLaunch() {

}

void getHumidTempData() {
  
  if(radio.available()){ // Если в буфер приёмника поступили данные
    radio.read(&potValue, sizeof(potValue)); // Читаем показания потенциометра
    String s = (String)potValue[0];
    Serial.print(s);Serial.print(" ");
    lcd.setCursor(0, 0);
    lcd.print("humid: " + s + " %");
    lcd.setCursor(0, 1);
    lcd.print("temp:  " + s + " C");

  }
 
}
