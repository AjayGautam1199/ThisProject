#include "esp_camera.h"
#include "Arduino.h"
#include "SD_MMC.h"
#include <WiFi.h>
#include <FirebaseESP32.h>
#include "FirebaseStorege.h"

#define FIREBASE_HOST "womansecuritydev.appspot.com"
#define FIREBASE_AUTH "AIzaSyDaE1VhPmbiNY0FQEih9U-W7sbo-6uykIA"
#define BUTTON_PIN 4

// WiFi credentials
const char* ssid = "GHRCE";
const char* password = "C89C6D26D3";

const int captureInterval = 10000; //capture interval in ms
const int imageCountMax = 6; //maximum number of images to capture
int imageCount = 1; //current image count
String imageName; //name of the current image
bool uploadImages = false; //flag to indicate if images should be uploaded

FirebaseStorage storage;

void setup() {
  Serial.begin(115200);

  //initialize WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");

  //initialize camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  //initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  Serial.println("Camera init successful");

  //initialize SD card
  if (!SD_MMC.begin()) {
    Serial.println("SD card initialization failed");
    return;
  }

  Serial.println("Sd card init successful");

  //initialize Firebase Storage
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  storage = FirebaseStorage(&Firebase);

  //initialize button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  //capture image
  if (imageCount <= imageCountMax) {
    if (millis() % captureInterval == 0) {
      camera_fb_t * fb = esp_camera_fb_get();
      if (!fb) {
        Serial.println("Camera capture failed");
        return;
      }

      //rename image
      imageName = String(imageCount) + ".jpg";
      Serial.println("Image name: " + imageName);

      //write image to file
      File file = SD_MMC.open(imageName, FILE_WRITE);
      if (!file) {
        Serial.println("Failed to open file");
        return;
      } 

      file.write(fb->buf, fb->len); //write image data to file
      file.close();

      //free image buffer
      esp_camera_fb_return(fb);

      //increment image count
      imageCount++;
    }
  } else {
    if (digitalRead(BUTTON_PIN) == LOW) {
      //upload images
      uploadImages = true;
    }
  }

  //upload images if flag is set
  if (uploadImages) {
    for (int i = 1; i <= imageCountMax; i++) {
//open image file
imageName = String(i) + ".jpg";
Serial.println("Uploading file " + imageName);
File file = SD_MMC.open(imageName, FILE_READ);
if (!file) {
Serial.println("Failed to open file " + imageName);
continue;
}
  //upload image to Firebase Storage
  FirebaseStorageFile storageFile = storage.createFile("/" + imageName, FirebaseStorage::StorageType::JPEG);
  if (!storageFile.begin()) {
    Serial.println("Failed to begin upload of file " + imageName);
    file.close();
    continue;
  }

  int chunkSize = 1024;
  uint8_t *chunk = new uint8_t[chunkSize];
  size_t bytesRead;
  while ((bytesRead = file.read(chunk, chunkSize)) > 0) {
    if (!storageFile.write(chunk, bytesRead)) {
      Serial.println("Failed to write chunk to file " + imageName);
      delete[] chunk;
      file.close();
      storageFile.close();
      continue;
    }
  }
  delete[] chunk;
  file.close();
  if (!storageFile.end()) {
    Serial.println("Failed to end upload of file " + imageName);
    storageFile.close();
    continue;
  }

  //delete image file from SD card
  SD_MMC.remove(imageName);

  Serial.println("File " + imageName + " uploaded successfully");

  //reset upload flag if all images have been uploaded
  if (i == imageCountMax) {
    uploadImages = false;
    imageCount = 1;
  }
}
}
}