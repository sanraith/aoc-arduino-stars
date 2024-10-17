#pragma once

#include <ArduinoHttpClient.h>
#include <NTPClient.h>
#include "WiFiS3.h"
#include "EEPROMManager.h"
#include <map>

class AocClient
{
public:
    AocClient(EEPROMManager *memoryManager, char sessionKey[], char leaderboardHost[], char leaderboardPath[], int leaderboardPort, char aocUserId[]);
    void setup();
    void loop();
    void requestUpdate();

private:
    void _update();
    void _printTime(time_t t);
    void _retryLater();

    char *_sessionKey;
    char *_leaderboardHost;
    char *_leaderboardPath;
    char *_aocUserId;
    int _leaderboardPort;

    bool _updateRequested;
    uint8_t _completionState[25];
    unsigned long _lastUpdateEpoch;

    WiFiClient *_wifiClient;
    WiFiUDP _ntpUDP;
    NTPClient *_ntpClient;
    HttpClient *_httpClient;

    std::map<int, int> _memoryMap;
};