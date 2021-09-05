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


int pinCooler = D3;
byte addresses[][6] = {"1Node", "2Node"}; /*адреса каналов приема и передачи*/
String coolerData = "coolerOFF";

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
  radio.openReadingPipe(1, 0x6969696969AA);
  radio.startListening();

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
  getRadioData();
  sendRadioData();
}

void getRadioData() {
  if (radio.available()) { // Если в буфер приёмника поступили данные
    char msg[32];
    String s = "";
    radio.read( &msg, sizeof(msg) );
    for (int i = 0; i < 32; i++) {
      s += msg[i];
    }
    if (s.startsWith("coolerON")) {
      coolerData = "coolerON";
      //todo coolerOn();
    } else if (s.startsWith("coolerOFF")) {
      coolerData = "coolerOFF";
      //todo coolerOff();
    }
    //Firebase.setString(firebaseData, "panchenko/bathroom/values/cooler", coolerData);
  }
}

void sendRadioData() {
  if (timer.endOfPeriod()) {
    /*Firebase.getString(firebaseData, "panchenko/mode");
      String s = firebaseData.stringData(); */

    /*read and upload zero value*/
    //http://digitrode.ru/computing-devices/mcu_cpu/3142-arduino-i-datchik-mq-135-izmerenie-koncentracii-uglekislogo-gaza-v-vozduhe.html
    //float ppm = gasSensor.getPPM();
    //Firebase.setFloat(firebaseData, "panchenko/bathroom/values/ppm", ppm );

    /*read and upload humidity, temperature value*/
    //Firebase.setFloat(firebaseData, "panchenko/bathroom/values/humidity", getHumidity());
    //Firebase.setFloat(firebaseData, "panchenko/bathroom/values/temperature", getTemperature());

    /*nrf send*/
    sendPPM_NRF24();
    sendHumidity_NRF24();
    sendTemperature_NRF24();
    sendCooler_NRF4();
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void sendHumidity_NRF24() {
  char message[32];
  char humArr[5];
  float hfloat = getHumidity();
  dtostrf(hfloat, 4, 1, humArr);
  message[0] = 'h';
  message[1] = 'u';
  message[2] = 'm';
  for (int i = 0; i < 4; i++) {
    message[i + 3] = humArr[i];
  }
  radio.stopListening();
  radio.write(&message, sizeof(message) );
  radio.startListening();
}

void sendTemperature_NRF24() {//
  char message[32];
  char tempArr[5];
  float tfloat = getTemperature();
  dtostrf(tfloat, 4, 1, tempArr);
  String s = "temp";
  for (int i = 0; i < s.length(); i++) {
    message[i] = s[i];
  }
  for (int i = 0; i < s.length(); i++) {
    message[i + s.length()] = tempArr[i];
  }
  radio.stopListening();
  radio.write(&message, sizeof(message) );
  radio.startListening();
}

void sendPPM_NRF24() {
  char message[32];
  char ppmArr[7];
  int divider = 100000;
  int length = 6;
  int ppm = getPPM();
  itoa(ppm, ppmArr, 10);
  String s = "ppm";
  for (int i = 0; i < s.length(); i++) {
    message[i] = s[i];
  }
  for (int i = length; i > 0 ; i--) {
    if (ppm / divider > 0) {
      break;
    } else {
      divider /= 10;
      length--;
    }
  }
  for (int i = 0; i < length; i++) {
    message[i + s.length()] = ppmArr[i];
  }
  radio.stopListening();
  radio.write(&message, sizeof(message) );
  radio.startListening();
}

void sendCooler_NRF4() {
  char message[32];
  for (int i = 0; i < coolerData.length(); i++) {
    message[i] = coolerData[i];
  }
  radio.stopListening();
  radio.write(&message, sizeof(message) );
  radio.startListening();
}

String getDate() {
  /*example "2021-09-02T19:09:16Z" */
  String fullDate = timeClient.getFormattedDate();
  String d = fullDate.substring(0, 10);
  String t = fullDate.substring(11, 19);
  return fullDate = d + " " + t;
}

float getHumidity() {
  float h = dht.readHumidity();
  return h > 0 ? h : 0.0;
}

float getTemperature() {
  float t = dht.readTemperature();
  return t > 0 ? t : 0.0;
}

float getPPM() {
  float offValue = 150000.0;
  float ppm = gasSensor.getPPM();
  return ppm < offValue ? ppm : 0.0;
}
