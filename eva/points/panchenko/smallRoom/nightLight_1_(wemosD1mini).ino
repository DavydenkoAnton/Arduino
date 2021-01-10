#include <ESP8266WiFi.h>        // wifi
#include <FirebaseESP8266.h>    // firebase

const char* WIFI_SSID = "Salva";
const char* WIFI_PASSWORD = "!Panchenko!60";
#define FIREBASE_HOST "panchenko-db04d.firebaseio.com"
#define FIREBASE_AUTH "Sj4vGGLFRnMq63hYPS2kVCSAnTlk7dZ50NxwP4IH"

FirebaseData firebaseData;

void setup() {
  Serial.begin(115200); //enable Serial Monitor
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  for (int i = 0; i < 5; i++) {
    if (WiFi.status()) {
      Serial.println("wifi connected");
      break;
    }
  }
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  //pinMode(kitchenController.kitchenWindowPirPin, INPUT);  // pir pin
}

void loop() {
  if (Firebase.setString(firebaseData, "panchenko/smallRoom/light/nightLight_1/status", "on")) {
    Serial.println("failed");
  } else {
    Serial.println("sended");
  }
  delay(5000);
}
