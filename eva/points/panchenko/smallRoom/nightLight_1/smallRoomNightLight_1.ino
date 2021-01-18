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
int green = 150;
int blue = 0;
bool MOVEMENT = false;
bool connection = false;
FirebaseData firebaseData;
Timer modeRainbowTimer;
Timer movementTimer;
Timer colorCheckTimer;
Timer modeCheckTimer;
Timer alphaCheckTimer;
Timer rainbowTimer;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, ledPin, NEO_GRB + NEO_KHZ800);

bool m = true;


//////////////////////////////////
/////// SETUP  ///////////////////
//////////////////////////////////
void setup() {
  delay(2000);
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
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);

  strip.begin();
  offMode();

  movementTimer.interval = 30000;
  modeCheckTimer.interval = 10000;
  colorCheckTimer.interval = 10000;
  alphaCheckTimer.interval = 10000;
  rainbowTimer.interval = 100;

  //ledSignal(3, 200);
}

//////////////////////////////////
/////// LOOP  ////////////////////
//////////////////////////////////

void loop() {
  movementListener();
  runLedStrip();
}

//////////////////////////////////
/////// FUNCTIONS  ///////////////
//////////////////////////////////
void runLedStrip() {
  if (MOVEMENT) {
    if (ledStripMode.equals("color")) {
      colorMode();
    } else  if (ledStripMode.equals("rainbow")) {
      rainbowMode();
    } else  if (ledStripMode.equals("belarus")) {
      belarusMode();
    } else  if (ledStripMode.equals("auto")) {
      randomMode();
    } else  if (ledStripMode.equals("off")) {
      offMode();
    }
  } else {
    offMode();
  }
}

void colorMode() {
  colorValueListener();
  modeValueListener();
  //ledSignal(2, 300);
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }
  strip.show();
}

void rainbowMode() {
  uint16_t i, j;
  for (j = 0; j < 256; j++) {
    for (i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, Wheel((i * 1 + j) & 255));
    }
    strip.show();
    delay(rainbowTimer.interval);
  }
  colorValueListener();
  modeValueListener();
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }  else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }  else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void belarusMode() {
  brightnessValueListener();
  modeValueListener();
  //ledSignal(3, 300);
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(brightness, brightness, brightness));
  }
  for (int i = NUM_LEDS / 3; i < (NUM_LEDS / 3) * 2; i++) {
    strip.setPixelColor(i, strip.Color(brightness, 0, 0));
  }
  strip.show();
}

void randomMode(){
  
  }

void modeValueListener() {
  if (millis() - modeCheckTimer.prev >= modeCheckTimer.interval) {
    modeCheckTimer.prev = millis();
    if (Firebase.getString(firebaseData, "panchenko/smallRoom/light/nightLight_1/ledStripMode")) {
      ledStripMode = firebaseData.stringData();
    }
    do {
      modeCheckTimer.prev += modeCheckTimer.interval;
      if (modeCheckTimer.prev < modeCheckTimer.interval) break;  // переполнение uint32_t
    } while (modeCheckTimer.prev < millis() - modeCheckTimer.interval); // защита от пропуска шага
  }
}

void colorValueListener() {
  if (millis() - colorCheckTimer.prev >= colorCheckTimer.interval) {
    colorCheckTimer.prev = millis();
    getRedColorFireBase();
    getGreenColorFireBase();
    getBlueColorFireBase();
    do {
      colorCheckTimer.prev += colorCheckTimer.interval;
      if (colorCheckTimer.prev < colorCheckTimer.interval) break;  // переполнение uint32_t
    } while (colorCheckTimer.prev < millis() - colorCheckTimer.interval); // защита от пропуска шага
  }
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
  if (millis() - alphaCheckTimer.prev >= alphaCheckTimer.interval) {
    alphaCheckTimer.prev = millis();
    if (Firebase.getInt(firebaseData, "panchenko/smallRoom/light/nightLight_1/color/alpha")) {
      brightness = firebaseData.intData();
    }
    do {
      alphaCheckTimer.prev += alphaCheckTimer.interval;
      if (alphaCheckTimer.prev < alphaCheckTimer.interval) break;  // переполнение uint32_t
    } while (alphaCheckTimer.prev < millis() - alphaCheckTimer.interval); // защита от пропуска шага
  }
}

void movementListener() {
  if (isMovement(pirPin)) {
    Serial.println("move");
    MOVEMENT = true;
    movementTimer.prev = millis();
  } else {
    if (millis() - movementTimer.prev >= movementTimer.interval) {
      Serial.println("not move");
      MOVEMENT = false;
      do {
        movementTimer.prev += movementTimer.interval;
        if (movementTimer.prev < movementTimer.interval) break;  // переполнение uint32_t
      } while (movementTimer.prev < millis() - movementTimer.interval); // защита от пропуска шага
    }
  }
}

bool isMovement(int pin) {
  return digitalRead(pin) == HIGH ? true : false;
}

void offMode() {
  for (int ledNum = 0; ledNum < NUM_LEDS; ledNum++) {
    strip.setPixelColor(ledNum, 0, 0, 0);
  }
  strip.show();
  //digitalWrite(BUILTIN_LED, HIGH); // led off
  modeValueListener();
}

void ledSignal(int count, int milliseconds) {
  for (int i = 0; i < count; i++) {
    digitalWrite(BUILTIN_LED, LOW);
    delay(milliseconds);
    digitalWrite(BUILTIN_LED, HIGH);
    delay(milliseconds);
  }
  delay(milliseconds);
}
