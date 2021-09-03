/********** libraries **********************************/
#include "ITimer.h"

#include <MQ135.h>              //PPM
#include <SPI.h>                // Подключаем библиотеку для работы с SPI-интерфейсом
#include <nRF24L01.h>           // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h>               // Подключаем библиотеку для работа для работы с модулем NRF24L01
#include <ESP8266WiFi.h>        // wifi
#include <FirebaseESP8266.h>    // firebase
#include "DHT.h"                // temperature, humidity
#include <NTPClient.h>
#include <WiFiUdp.h>

/***********  local variables **********************************/
#define WIFI_SSID "Salva" /* Define the WiFi credentials */
#define WIFI_PASSWORD "!Panchenko!60" /* Define the WiFi credentials */
#define FIREBASE_HOST  "panchenko-db04d.firebaseio.com"
#define FIREBASE_AUTH  "Sj4vGGLFRnMq63hYPS2kVCSAnTlk7dZ50NxwP4IH"
#define DHTPIN  D1
#define DHTTYPE DHT22           // DHT 22 (AM2302)


ITimer timer(2); /* seconds */

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
DHT dht(DHTPIN, DHTTYPE);
FirebaseData firebaseData;
MQ135 gasSensor = MQ135(A0);
RF24 radio(D4, D2); // Создаём объект radio с указанием выводов CE и CSN
int potValue[1]; // Создаём массив для передачи значений nrf24l01

float humidity, temperature;
int pinCooler = D3;
byte addresses[][6] = {"1Node", "2Node"}; /*адреса каналов приема и передачи*/
int msg[1];
int count = 0;


void setup() {

  Serial.begin(115200);
  delay(500);

  pinMode(A0, INPUT);
  pinMode(pinCooler, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)    {
    Serial.print(".");
    delay(300);
  }
  Serial.print("Connected to Wi-Fi with IP: ");
  Serial.println(WiFi.localIP());

  /*firebase init*/
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  /*dht init*/
  dht.begin();

  /*nrf24 init*/
  radio.begin();  // Инициализация модуля NRF24L01
  radio.setChannel(5); // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setDataRate (RF24_1MBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openWritingPipe(0x7878787878LL); // Открываем трубу с уникальным ID

  /*time client init*/
  timeClient.begin();
  timeClient.setTimeOffset(10800);    // GMT+3 -> GMT + 3600(1 hour in seconds) * 3 = 10800
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  Firebase.setString(firebaseData, "panchenko/bathroom/realod/date", getDate());

  pinMode(LED_BUILTIN, OUTPUT);
}


void loop() {
  
  if (timer.endOfPeriod()) {
    /*Firebase.getString(firebaseData, "panchenko/mode");
      String s = firebaseData.stringData(); */

    /*read and upload zero value*/
    //http://digitrode.ru/computing-devices/mcu_cpu/3142-arduino-i-datchik-mq-135-izmerenie-koncentracii-uglekislogo-gaza-v-vozduhe.html
    float ppm = gasSensor.getPPM();
    Firebase.setFloat(firebaseData, "panchenko/bathroom/values/MQ135_zero", ppm );

    /*read and upload humidity, temperature value*/
    Firebase.setFloat(firebaseData, "panchenko/bathroom/values/humidity", getHumidity());
    Firebase.setFloat(firebaseData, "panchenko/bathroom/values/temperature", getTemperature());

    /*nrf send*/
    String ht = String(getHumidity()) + String(getTemperature());
    uint8_t dataArray[ht.length()];
    ht.getBytes(dataArray, ht.length());

    potValue[0] = count; // Считываем показания потенциометра
    radio.write(potValue, 1); // Отправляем считанные показания по радиоканалу
    count++;

  }
}

String getDate() {
  /*example "2021-09-02T19:09:16Z" */
  String fullDate = timeClient.getFormattedDate();
  String d = fullDate.substring(0, 10);
  String t = fullDate.substring(11, 19);
  return fullDate = d + " " + t;
}

float getHumidity() {
  float h = dht.readHumidity() > 0 ? dht.readHumidity() : 0;
  return h;
}

float getTemperature() {
  float t = dht.readTemperature() > 0 ? dht.readTemperature() : 0;
  return t;
}
