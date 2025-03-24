#include <U8g2lib.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <ArduinoJson.h>

// Bitmap Data
#include "bitmaps.h" // Create a header file for all image definitions to keep the main file clean

// OLED Display (SH1106 128x64)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 22, /* data=*/ 21);

// Wi-Fi credentials
const char* ssid = "Google pixel";
const char* password = "11889900";

// Alpha Vantage API
const char* api_url = "https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=ADANIPORTS.BSE&apikey=GU7VQTNNWGQPCGA2";

String stockPrice = "000.00"; // Default stock price

void setup() {
    // Serial Communication
    Serial.begin(115200);

    // Initialize I2C and OLED
    Wire.begin();
    Wire.setClock(100000);
    u8g2.begin();

    // Connect to Wi-Fi
    connectToWiFi();
}

void connectToWiFi() {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to Wi-Fi");
}

void fetchStockPrice() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Wi-Fi not connected");
        return;
    }

    HTTPClient http;
    http.begin(api_url);

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("HTTP Response: " + response);

        // Parse JSON response
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, response);

        if (error) {
            Serial.println("JSON Error: " + String(error.c_str()));
            stockPrice = "N/A";
        } else {
            const char* price = doc["Global Quote"]["05. price"];
            stockPrice = (price) ? String(price) : "N/A";
        }
    } else {
        Serial.println("HTTP Error: " + String(httpResponseCode));
        stockPrice = "Error";
    }
    http.end();
}

void drawOLED() {
    u8g2.clearBuffer();

    // Draw static components
    u8g2.drawXBM(102, 1, 24, 16, image_battery_full_bits);
    // u8g2.drawXBM(64, 0, 19, 16, image_wifi_bits);
    u8g2.drawXBM(0, 0, 60, 64, image_Cyre_bits);
    u8g2.drawLine(61, 18, 128, 18);
    u8g2.drawXBM(89, 0, 14, 16, image_bluetooth_bits);

    // Draw text and data
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(2, 9, "it'z A!G");
    u8g2.drawStr(82, 36, "Share");

    // Stock price section
    u8g2.drawXBM(63, 21, 65, 7, image_text1_bits);
    u8g2.setFont(u8g2_font_6x13_tr);
    u8g2.drawStr(68, 55, stockPrice.c_str());
    u8g2.drawXBM(67, 32, 5, 8, image_arrow_curved_down_right_bits);
    // Check Wi-Fi status
    if (WiFi.status() == WL_CONNECTED) {
        // Draw Wi-Fi connected icon
        u8g2.drawXBMP(64, 0, 19, 16, image_wifi_bits);
    } else {
        // Draw Wi-Fi not connected icon
        u8g2.drawXBMP(64, 0, 19, 16, image_wifi_not_connected_bits);
    }
    u8g2.sendBuffer();
}

void loop() {
    fetchStockPrice(); // Update stock price
    drawOLED();        // Update OLED display
    delay(60000);      // Fetch and update every 60 seconds
}


