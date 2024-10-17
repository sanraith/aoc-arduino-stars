#pragma once

#include "WiFiS3.h"
#include "AocClient.h"

class LocalServer
{
public:
    LocalServer(AocClient *aocClient);
    void setup();
    void loop();

private:
    WiFiServer _server;
    AocClient *_aocClient;
};