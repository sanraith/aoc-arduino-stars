#pragma once

#include "WiFiS3.h"
#include "AocClient.h"
#include "StarLedManager.h"

class LocalServer
{
public:
    LocalServer(AocClient *aocClient, StarLedManager *starLedManager);
    void setup();
    void loop();

private:
    WiFiServer _server;
    AocClient *_aocClient;
    StarLedManager *_starLedManager;
};