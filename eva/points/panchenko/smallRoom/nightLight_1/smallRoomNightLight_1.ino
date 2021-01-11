#include <ESP8266WiFi.h>        // wifi
#include <FirebaseESP8266.h>    // firebase
#include <Adafruit_NeoPixel.h>  // WS212B

const char* WIFI_SSID = "Salva";
const char* WIFI_PASSWORD = "!Panchenko!60";
#define FIREBASE_HOST "panchenko-db04d.firebaseio.com"
#define FIREBASE_AUTH "Sj4vGGLFRnMq63hYPS2kVCSAnTlk7dZ50NxwP4IH"
#define NUM_LEDS    5           // Number of LEDs connected.
int pirPin = D1;
int ledPin = D4;
String lightMode = "static";
String color = "";
int brightness = 100;
bool connection = false;
FirebaseData firebaseData;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, ledPin, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200); //enable Serial Monitor
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  for (int i = 0; i < 5; i++) {
    if (WiFi.status()) {
      connection = true;
      break;
    }
    delay(100);
  }
  if (!connection) {
    delay(5000);
    ESP.restart();
  }
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  pinMode(pirPin, INPUT);   // pir pin
  pinMode(ledPin, OUTPUT);  // led pin

  pixels.begin();
}

void loop() {
  getLightMode();
  getBrightness();

  if (lightMode.equals("static")) {
    for (int i = 0; i < NUM_LEDS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 255, 0));
      pixels.show();
      delay(1000);
    }
  } else if (lightMode.equals("auto")) {
    for (int i = 0; i < NUM_LEDS; i++) {
      pixels.setPixelColor(i, pixels.Color(255, 0, 0));
      pixels.show();
      delay(1000);
    }
  }

  runLed();
  delay(3000);
}

void runLed() {

}

void getLightMode() {
  if (Firebase.getString(firebaseData, "panchenko/smallRoom/light/nightLight_1/lightMode")) {
    lightMode = firebaseData.stringData();
  }
}

void getColor() {
  if (Firebase.getString(firebaseData, "panchenko/smallRoom/light/nightLight_1/color")) {
    color = firebaseData.stringData();
  }
}

void getBrightness() {
  if (Firebase.getInt(firebaseData, "panchenko/smallRoom/light/nightLight_1/brightness")) {
    brightness = firebaseData.intData();
  }
}

