#include "DHT.h"
#include "MQ135.h"                 // подключаем библиотеку MQ135
#include "LiquidCrystal_I2C.h"     // подключаем библиотеку дисплея 

#define DHTPIN 3  // Вывод датчика DATA подключен ко второму пину
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define mq135PIN A1; // аналоговый выход MQ135 подключен к пину A0 Arduino

DHT dht(DHTPIN, DHTTYPE);
MQ135 gasSensor = MQ135(A1);
LiquidCrystal_I2C lcd(0x27,16,2);  // Устанавливаем какой дисплей
const int minCo2=600;
const int maxCo2=800;
int hum = 0;
int temp = 0;


void setup(){
  Serial.begin(9600);
  lcd.init();                      // инициализируем дисплей                
  lcd.backlight();
  dht.begin();  
  pinMode(A1, INPUT);// Включаем подсветку дисплея
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
}

void loop(){    
digitalWrite(2, LOW);
delay(1000);
digitalWrite(2, HIGH);
delay(1000);
  
//  int co2 = gasSensor.getPPM();
//  
//  Serial.println(co2);
//  lcd.setCursor(0, 0);
//  delay(10);
//  lcd.clear();
//  delay(10);
//  lcd.print("co2: ");
//  delay(10);
//  lcd.print(co2);
//  delay(10);
//  lcd.setCursor(0, 1);
//  delay(10);
//  hum = dht.readHumidity();
//  delay(10);
//  temp = dht.readTemperature();
//  delay(10);
//  lcd.print("hum: ");
//  delay(10);
//  lcd.print(hum);
//  delay(10);
//  lcd.print(" temp: ");
//  delay(10);
//  lcd.print(temp);
//  
//  if(co2 > maxCo2){
//    digitalWrite(2, HIGH);
//  }
//  if(co2 < minCo2){
//     digitalWrite(2, LOW);
//  }
  
//  delay(10000);
}
