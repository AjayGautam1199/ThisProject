// BLESecurityManager.h

#ifndef BLESECURITYMANAGER_H
#define BLESECURITYMANAGER_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLESecurity.h>
#include <BLEServer.h>

// Forward declaration for BLEServerCallbacks and BLESecurityCallbacks
class BLEServerCallbacks;
class BLESecurityCallbacks;

class BLESecurityManager {
public:
    BLESecurityManager();
    ~BLESecurityManager();

    void setCallbacks(BLESecurityCallbacks* callbacks);
    void initializeSecurity(BLESecurity* pSecurity, BLEServer* pServer);

private:
    BLESecurityCallbacks* m_callbacks;
    BLESecurity* m_security;
};

#endif