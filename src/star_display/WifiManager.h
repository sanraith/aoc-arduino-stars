#pragma once

#include "WiFiS3.h"
#include "StarLedManager.h"

class WifiManager
{
public:
    WifiManager(char ssid[], char pass[], StarLedManager *starLedManager);
    void setIpAddress(char ipAddress[]);
    void setup();

private:
    char *_ssid;
    char *_pass;
    int wifiStatus = WL_IDLE_STATUS;
    StarLedManager *_starLedManager; // Reference to StarLedManager
};