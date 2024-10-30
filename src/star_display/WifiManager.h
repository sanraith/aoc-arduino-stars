#pragma once

#include "WiFiS3.h"

class WifiManager
{
public:
    WifiManager(char ssid[], char pass[]);
    void setIpAddress(char ipAddress[]);
    void setup();

private:
    char *_ssid;
    char *_pass;
    int wifiStatus = WL_IDLE_STATUS;
};