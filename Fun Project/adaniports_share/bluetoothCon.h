#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLESecurity.h>
#include <U8g2lib.h>
#include "BLESecurityManager.h"
static const unsigned char image_bluetooth_bits[] U8X8_PROGMEM = {
0x80,0x00,0x40,0x01,0x40,0x02,0x44,0x04,0x48,0x04,0x50,0x02,0x60,0x01,
0xc0,0x00,0x60,0x01,0x50,0x02,0x48,0x04,0x44,0x04,0x40,0x02,0x40,0x01,
0x80,0x00,0x00,0x00};
static const unsigned char image_bluetooth_not_connected_bits[] U8X8_PROGMEM = {
0x40,0x00,0xc1,0x00,0x42,0x01,0x44,0x02,0x48,0x04,0x10,0x04,0x20,0x02,
0x40,0x00,0xa0,0x00,0x50,0x01,0x48,0x02,0x44,0x04,0x40,0x0a,0x40,0x11,
0x80,0x20,0x00,0x00};


// BLE variables
BLEServer* pServer;
BLECharacteristic* pCharacteristic;
bool isConnected = false; // Connection status flag

// OLED initialization (update based on your display configuration)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Function to set up and start advertising
void advertising() {
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
    pAdvertising->setScanResponse(true); // Optional: Set scan response
    pAdvertising->setMinPreferred(0x06); // Optional: Set minimum preferred connection interval
    pAdvertising->setMaxPreferred(0x12); // Optional: Set maximum preferred connection interval
    pAdvertising->start();
    Serial.println("Advertising started...");
}

// Callback class to handle connection events
class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) override {
        isConnected = true; // Device connected
        Serial.println("Device Connected!");
    }

    void onDisconnect(BLEServer* pServer) override {
        isConnected = false; // Device disconnected
        Serial.println("Device Disconnected!");
        advertising(); // Restart advertising when disconnected
    }
};

// Security callback class for pairing
class MySecurityCallbacks : public BLESecurityCallbacks {
    uint32_t onPassKeyRequest() override {
        Serial.println("Passkey Request");
        return 123456; // Example 6-digit passkey
    }

    void onPassKeyNotify(uint32_t passkey) override {
        Serial.printf("Passkey Notify: %d\n", passkey);
    }

    bool onConfirmPIN(uint32_t passkey) override {
        Serial.printf("Confirm Passkey: %d\n", passkey);
        return true; // Automatically confirm
    }

    void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl) override {
        if (cmpl.success) {
            Serial.println("Authentication Success!");
        } else {
            Serial.println("Authentication Failed!");
            // To help debug the issue, print details of the failure
            Serial.printf("Reason: %d\n", cmpl.fail_reason);
        }
    }

    bool onSecurityRequest() override {
        Serial.println("Security Request");
        return true; // Accept security requests
    }
};

// Function to display Bluetooth status on OLED
void displayBluetoothStatus() {
    u8g2.clearBuffer();
    if (isConnected) {
        u8g2.drawXBMP(89, 0, 14, 16, image_bluetooth_bits); // Connected bitmap
    } else {
        u8g2.drawXBMP(89, 0, 14, 16, image_bluetooth_not_connected_bits); // Not connected bitmap
    }
    u8g2.sendBuffer();
}
// BLE and security manager setup
BLESecurityManager securityManager;
void setup() {
    // Initialize serial monitor
    Serial.begin(115200);

    // Initialize OLED
    u8g2.begin();

    // Initialize BLE
    BLEDevice::init("BTAG");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks()); // Use MyServerCallbacks for connection handling

    // Set up security and pairing
    BLESecurity* pSecurity = new BLESecurity();
    // securityManager.setSecurityCallbacks(new MySecurityCallbacks());
    // securityManager.initializeSecurity(pSecurity, pServer);
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND); // Enable bonding
    pSecurity->setCapability(ESP_IO_CAP_OUT); // Display passkey
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    // pSecurity->setCallbacks(new MySecurityCallbacks()); // Use MySecurityCallbacks for handling security requests

    // Create service and characteristic
    BLEService* pService = pServer->createService("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
    pCharacteristic = pService->createCharacteristic(
        "beb5483e-36e1-4688-b7f5-ea07361b26a8",
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    pService->start();
    advertising();

}

void loop() {
    // Update Bluetooth connection status on the OLED display
    displayBluetoothStatus();

    delay(1000); // Refresh every second
}



