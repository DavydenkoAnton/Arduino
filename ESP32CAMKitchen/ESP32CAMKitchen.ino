// SD operations https://esp32.com/viewtopic.php?t=14371
//https://github.com/fustyles/Arduino/blob/master/ESP32-CAM_Firebase/ESP32-CAM_Firebase.ino
//https://codebeautify.org/base64-to-image-converter

#include "esp_camera.h"
#include "Arduino.h"
#include "Base64.h"
#include "FirebaseESP32.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
//#include <EEPROM.h>            // read and write from flash memory
#include <WiFi.h>
#include <NTPClient.h>         // time lib
#include <WiFiUdp.h>           // time lib

// define the number of bytes you want to access
//#define EEPROM_SIZE 1

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

String FIREBASE_HOST = "panchenko-db04d.firebaseio.com";
String FIREBASE_AUTH = "Sj4vGGLFRnMq63hYPS2kVCSAnTlk7dZ50NxwP4IH";
const char* ssid = "Salva";
const char* password = "!Panchenko!60";
FirebaseData firebaseData;
String command = "";
//int pictureNumber = 0;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  //Serial.println();

  WiFi.begin(ssid, password);

  long int StartTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if ((StartTime + 10000) < millis()) break;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to wifi");
  } else {
    Serial.println("Connection failed");
    return;
  }

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_QVGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }



  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }



  //Serial.println("Starting SD Card");
  if (!SD_MMC.begin()) {
    Serial.println("SD Card Mount Failed");
    return;
  }

  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD Card attached");
    return;
  }

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setMaxRetry(firebaseData, 3);
  Firebase.setMaxErrorQueue(firebaseData, 30);
  Firebase.enableClassicRequest(firebaseData, true);

  // Initialize a NTPClient to get time
  timeClient.begin();
  timeClient.setTimeOffset(10800);// GMT +1 = 3600
}


void loop() {
  getCommand();
  executeCommand();
}

void getCommand() {
  if (Firebase.getString(firebaseData, "/panchenko/kitchen/command/cam1")) {
    command = firebaseData.stringData();
    if (command.length() > 0) {
      Serial.println(command);
    }
  }
}

void executeCommand() {
  if (command.equals("refresh")) {
    snapshot();
    eraseCommand();
  }
  if (command.equals("save")) {
    saveSnapshot();
    eraseCommand();
  }
}

void eraseCommand() {
  String firebaseCommandPath = "/panchenko/kitchen/command/cam1";
  if (Firebase.setString(firebaseData, firebaseCommandPath, "")) {
    Serial.println("send to firebase");
  } else {
    Serial.println(firebaseData.errorReason());
  }
}

void snapshot() {
  Serial.println("snapshot()");
  camera_fb_t * fb = NULL;

  // Take Picture with Camera
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  String imageFile = "";
  char *input = (char *)fb->buf;
  char output[base64_enc_len(3)];
  String firebasePhotoPath = "/panchenko/kitchen/photoBase64/cam1/320x240/";
  int count = 1;
  String firebasePhotoPathBuf = "";
  String time = getTime();
  String date = getDate();
  for (int i = 0; i < fb->len; i++) {
    base64_encode(output, (input++), 3);
    if (i % 3 == 0) {
      imageFile += urlencode(String(output));
    }
    if (imageFile.length() > 5000) {
      firebasePhotoPathBuf = firebasePhotoPath + (String)count;
      if (!Firebase.setString(firebaseData, firebasePhotoPathBuf, imageFile)) {
        Serial.println(firebaseData.errorReason());
      }
      imageFile = "";
      count++;
    }
  }
  firebasePhotoPathBuf = firebasePhotoPath + (String)count;
  if (!Firebase.setString(firebaseData, firebasePhotoPathBuf, imageFile)) {
    Serial.println(firebaseData.errorReason());
  }
  firebasePhotoPathBuf = firebasePhotoPath + "date";
  if (!Firebase.setString(firebaseData, firebasePhotoPathBuf, date)) {
    Serial.println(firebaseData.errorReason());
  }
  firebasePhotoPathBuf = firebasePhotoPath + "time";
  if (!Firebase.setString(firebaseData, firebasePhotoPathBuf, time)) {
    Serial.println(firebaseData.errorReason());
  }
  esp_camera_fb_return(fb);
}

void saveSnapshot() {
  Serial.println("saveSnapshot()");
  
  sensor_t * cam = esp_camera_sensor_get();
  cam->set_framesize(cam, FRAMESIZE_UXGA);

  camera_fb_t * fb = NULL;

  // Take Picture with Camera
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  // initialize EEPROM with predefined size
  //EEPROM.begin(EEPROM_SIZE);
  //pictureNumber = EEPROM.read(0) + 1;



  String dirPath = "/" + getDate();
  String filePath = "/" + getTime() + ".jpg";

  createDir(SD_MMC, dirPath.c_str());

  // Path where new picture will be saved in SD Card
  String path = dirPath + filePath;

  fs::FS &fs = SD_MMC;
  Serial.printf("Picture file name: %s\n", path.c_str());

  File file = fs.open(path.c_str(), FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file in writing mode");
  }  else {
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path.c_str());
    //EEPROM.write(0, pictureNumber);
    //EEPROM.commit();
  }
  file.close();
  cam->set_framesize(cam, FRAMESIZE_QVGA);
  esp_camera_fb_return(fb);
}


String Photo2Base64() {
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return "";
  }
  String imageFile = "data:image/jpeg;base64,";
  char *input = (char *)fb->buf;
  char output[base64_enc_len(3)];
  for (int i = 0; i < fb->len; i++) {
    base64_encode(output, (input++), 3);
    if (i % 3 == 0) imageFile += urlencode(String(output));
  }
  esp_camera_fb_return(fb);
  return imageFile;
}


//https://github.com/zenmanenergy/ESP8266-Arduino-Examples/
String urlencode(String str)
{
  String encodedString = "";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      code2 = '\0';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
      //encodedString+=code2;
    }
    yield();
  }
  return encodedString;
}

String getDate() {
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  String formattedDate = timeClient.getFormattedDate();
  // Extract date
  int splitT = formattedDate.indexOf("T");
  String date = formattedDate.substring(0, splitT);
  return date;
}

String getTime() {
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  String formattedDate = timeClient.getFormattedDate();
  // Extract time
  int splitT = formattedDate.indexOf("T");
  String time = formattedDate.substring(splitT + 1, formattedDate.length() - 1);
  time.replace(":", "-");
  return time;
}

void createDir(fs::FS &fs, const  char * path) {
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path)) {
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
}

//init with high specs to pre-allocate larger buffers
//    FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
//    FRAMESIZE_QQVGA,    // 160x120
//    FRAMESIZE_QCIF,     // 176x144
//    FRAMESIZE_HQVGA,    // 240x176
//    FRAMESIZE_240X240,  // 240x240
//    FRAMESIZE_QVGA,     // 320x240
//    FRAMESIZE_CIF,      // 400x296
//    FRAMESIZE_HVGA,     // 480x320
//    FRAMESIZE_VGA,      // 640x480
//    FRAMESIZE_SVGA,     // 800x600
//    FRAMESIZE_XGA,      // 1024x768
//    FRAMESIZE_HD,       // 1280x720
//    FRAMESIZE_SXGA,     // 1280x1024
//    FRAMESIZE_UXGA,     // 1600x1200
