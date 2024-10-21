#pragma once

#include <ArduinoHttpClient.h>
#include <NTPClient.h>
#include "WiFiS3.h"
#include "EEPROMManager.h"
#include <map>

class AocClient
{
public:
    AocClient(EEPROMManager *memoryManager,
              const char sessionKey[], int aocYear,
              char leaderboardHost[], int leaderboardPort, char leaderboardUserId[],
              char aocUserId[]);
    void setup();
    void loop();
    void requestUpdate();

private:
    void _update();
    void _printTime(time_t t);
    void _retryLater();
    int _getLatestAocDay(time_t currentTime, int aocYear);

    char _sessionKey[257];
    char _leaderboardHost[128];
    char _leaderboardPath[128];
    char _userId[16];
    int _aocYear;
    int _leaderboardPort;

    bool _updateRequested;
    uint8_t _completionState[25];
    unsigned long _lastUpdateEpoch;

    WiFiClient *_wifiClient;
    HttpClient *_httpClient;
    WiFiUDP _ntpUDP;
    NTPClient _ntpClient;

    std::map<int, int> _memoryMap;
};