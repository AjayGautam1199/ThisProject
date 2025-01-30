#include <WebSocketsServer_Generic.h>
#include <WiFi.h>
#include <esp_camera.h>

// Wi-Fi credentials
const char* ssid = "Google_pixel";
const char* password = "11889900";

// WebSocket server
WebSocketsServer webSocket = WebSocketsServer(81);

// Camera configuration for Xiao ESP32S3
void setupCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 15;  // Y2_GPIO_NUM
  config.pin_d1 = 17;  // Y3_GPIO_NUM
  config.pin_d2 = 18;  // Y4_GPIO_NUM
  config.pin_d3 = 16;  // Y5_GPIO_NUM
  config.pin_d4 = 14;  // Y6_GPIO_NUM
  config.pin_d5 = 12;  // Y7_GPIO_NUM
  config.pin_d6 = 11;  // Y8_GPIO_NUM
  config.pin_d7 = 48;  // Y9_GPIO_NUM
  config.pin_xclk = 10; // XCLK_GPIO_NUM
  config.pin_pclk = 13; // PCLK_GPIO_NUM
  config.pin_vsync = 38; // VSYNC_GPIO_NUM
  config.pin_href = 47;  // HREF_GPIO_NUM
  config.pin_sscb_sda = 40; // SIOD_GPIO_NUM
  config.pin_sscb_scl = 39; // SIOC_GPIO_NUM
  config.pin_pwdn = -1;  // PWDN_GPIO_NUM (not used)
  config.pin_reset = -1; // RESET_GPIO_NUM (not used)
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }
}

// WebSocket event handler
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      Serial.printf("[%u] Connected!\n", num);
      break;
    case WStype_TEXT:
      // Handle text data
      break;
    case WStype_BIN:
      // Handle binary data
      break;
  }
}

// Initialize WebSocket server
void setupWebSocket() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

// Capture and send frames
void sendFrame() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  webSocket.broadcastBIN(fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Print ESP32 IP address
  Serial.println(WiFi.localIP());

  // Initialize the camera
  setupCamera();

  // Start WebSocket server
  setupWebSocket();
}

void loop() {
  webSocket.loop();
  sendFrame();
  delay(100); // Adjust delay based on your frame rate requirements
}