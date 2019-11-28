#include "Arduino.h"
#include "Wire.h"                  // подключаем библиотеку I2C
#include "MQ135.h"                 // подключаем библиотеку MQ135
#include "LiquidCrystal_I2C.h"     // подключаем библиотеку дисплея 

#define mq135PIN A1; // аналоговый выход MQ135 подключен к пину A0 Arduino


MQ135 gasSensor = MQ135(A1);
LiquidCrystal_I2C lcd(0x27,16,2);  // Устанавливаем какой дисплей
float minimum=10000;
float maximum=0;
float average=0;

void setup(){
  Serial.begin(9600);
  lcd.init();                      // инициализируем дисплей                
  lcd.backlight();  
  pinMode(A1, INPUT);// Включаем подсветку дисплея
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
}

void loop(){  

  lcd.setCursor(0, 0);  
  float rzero = gasSensor.getPPM();
  int minCO=400;
  Serial.println(rzero);
  lcd.print(rzero);
  if(rzero > minCO){
    digitalWrite(2, HIGH);
    Serial.println("true");
  }else{
     digitalWrite(2, LOW);
      Serial.println("false");
      }
  delay(20000);
}

void getMin(float v){
  if(v<minimum){
    minimum=v;
  }  
}

void getMax(float v){
  if(v>maximum){
    maximum=v;
  }  
}

void getAverage(){
  average=(minimum+maximum)/2;  
}
