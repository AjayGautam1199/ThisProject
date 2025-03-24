
// BLESecurityManager.cpp
#include "BLESecurityManager.h"

BLESecurityManager::BLESecurityManager() : m_callbacks(nullptr), m_security(nullptr) {}

BLESecurityManager::~BLESecurityManager() {}

void BLESecurityManager::setCallbacks(BLESecurityCallbacks* callbacks) {
    m_callbacks = callbacks;  // Set the user-defined callback handler
}

void BLESecurityManager::initializeSecurity(BLESecurity* pSecurity, BLEServer* pServer) {
    m_security = pSecurity;

    // Set up the security parameters
    m_security->setAuthenticationMode(ESP_LE_AUTH_BOND);
    m_security->setCapability(ESP_IO_CAP_OUT); // Display passkey capability
    m_security->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

    // You do not need to call setCallbacks() for BLESecurity, it's not needed
    // Handle security events directly using m_callbacks if needed
}