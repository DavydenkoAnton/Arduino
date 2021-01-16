#include <ESP8266WiFi.h>        // wifi
#include <FirebaseESP8266.h>    // firebase
#include <Adafruit_NeoPixel.h>  // WS212B
#include <Timer.h>

const char* WIFI_SSID = "Salva";
const char* WIFI_PASSWORD = "!Panchenko!60";
#define FIREBASE_HOST "panchenko-db04d.firebaseio.com"
#define FIREBASE_AUTH "Sj4vGGLFRnMq63hYPS2kVCSAnTlk7dZ50NxwP4IH"
#define NUM_LEDS    16
int pirPin = D1;
int ledPin = D4;
String ledStripMode = "static";
int brightness = 150;
int red = 0;
int green = 0;
int blue = 0;
bool MOVEMENT=false;
bool connection = false;
FirebaseData firebaseData;
Timer modeRainbowTimer;
Timer movementTimer;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, ledPin, NEO_GRB + NEO_KHZ800);

bool m = false;
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

  strip.begin();
  for (int ledNum = 0; ledNum < NUM_LEDS; ledNum++) {
    strip.setPixelColor(ledNum, 0, 0, 0);
  }
  strip.show();

  movementTimer.interval = 30000;
  modeRainbowTimer.interval = 50;
}

void loop() {
  movementListener();
  if (MOVEMENT) {
    runLedStrip();
  } else {
    offLedStrip();
  }
}

void runLedStrip() {
  modeValueListener();
  runLedStripMode();
}

void runLedStripMode() {
  if (ledStripMode.equals("static")) {
    staticMode();
  } else  if (ledStripMode.equals("rainbow")) {
    rainbowMode();
  } else  if (ledStripMode.equals("b4b")) {
    b4bMode();
  }
}

void offLedStrip() {
  for ( int ledNum = 0; ledNum < NUM_LEDS; ledNum++) {
    strip.setPixelColor(ledNum, strip.Color(0, 0 , 0));
  }
  strip.show();
}

void staticMode() {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }
  strip.show();
  colorValueListener();
  modeValueListener();
}

void rainbowMode() {
  for ( int i = 0; i < 255; i++) {
    for ( int ledNum = 0; ledNum < NUM_LEDS; ledNum++) {
      strip.setPixelColor(ledNum, strip.Color(red, i , blue));
    }
    strip.show();
    delay(30);
  }
  for ( int i = 255; i > 0; i--) {
    for ( int ledNum = 0; ledNum < NUM_LEDS; ledNum++) {
      strip.setPixelColor(ledNum, strip.Color(red, i , blue));
    }
    delay(30);
    strip.show();
  }
  colorValueListener();
  modeValueListener();
  /*
    if (millis() - modeRainbowTimer.prev >= modeRainbowTimer.interval) {
      modeRainbowTimer.prev = millis();   // "сбросить" таймер
      strip.setPixelColor(pixNum, strip.Color(, 0, 0));
      strip.show();
    }
  */
}

void b4bMode() {
  brightnessValueListener();
  modeValueListener();
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(brightness, brightness, brightness));
  }
  for (int i = NUM_LEDS / 3; i < (NUM_LEDS / 3) * 2; i++) {
    strip.setPixelColor(i, strip.Color(brightness, 0, 0));
  }
  strip.show();
}

void modeValueListener() {
  if (Firebase.getString(firebaseData, "panchenko/smallRoom/light/nightLight_1/ledStripMode")) {
    ledStripMode = firebaseData.stringData();
  }
}

void colorValueListener() {
  getRedColorFireBase();
  getGreenColorFireBase();
  getBlueColorFireBase();
}

void getRedColorFireBase() {
  if (Firebase.getInt(firebaseData, "panchenko/smallRoom/light/nightLight_1/color/red")) {
    red = firebaseData.intData();
  }
}

void  getGreenColorFireBase() {
  if ( Firebase.getInt(firebaseData, "panchenko/smallRoom/light/nightLight_1/color/green")) {
    green  = firebaseData.intData();
  }
}

void  getBlueColorFireBase() {
  if ( Firebase.getInt(firebaseData, "panchenko/smallRoom/light/nightLight_1/color/blue")) {
    blue = firebaseData.intData();
  }
}

void brightnessValueListener() {
  if (Firebase.getInt(firebaseData, "panchenko/smallRoom/light/nightLight_1/color/alpha")) {
    brightness = firebaseData.intData();
  }
}

void movementListener() {
  if (isMovement(pirPin)) {
    Serial.println("move");
    MOVEMENT = true;
    movementTimer.prev = millis();
  }
  if (millis() - movementTimer.prev >= movementTimer.interval) {
    Serial.println("not move");
    MOVEMENT = false;
  }
}

bool isMovement(int pin) {
  bool m;
  if (digitalRead(pin) == HIGH) {
    m = true;
  } else if (digitalRead(pin) == LOW) {
    m = false;
  }
  return m;
}
