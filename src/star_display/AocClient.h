#pragma once

#include <ArduinoHttpClient.h>
#include <NTPClient.h>
#include "WiFiS3.h"
#include "EEPROMManager.h"
#include <map>
#include "StarLedManager.h"

class AocClient
{
public:
    AocClient(EEPROMManager *memoryManager,
              const char sessionKey[], int aocYear,
              char leaderboardHost[], int leaderboardPort, char leaderboardId[],
              char aocUserId[], StarLedManager *starLedManager);
    void setup();
    void loop();
    void requestUpdate();

    char *getSessionKey() { return _sessionKey; };
    void setSessionKey(const char sessionKey[]);

    int getAocYear() { return _aocYear; };
    void setAocYear(int aocYear);

    char *getUserId() { return _userId; };
    void setUserId(const char userId[]);

    char *getLeaderboardId() { return _leaderboardId; };
    void setLeaderboardId(const char leaderboardId[]);

    char *getLeaderboardHost() { return _leaderboardHost; };
    void setLeaderboardHost(const char leaderboardHost[]);

    int getLeaderboardPort() { return _leaderboardPort; };
    void setLeaderboardPort(int leaderboardPort);

    uint8_t _completionState[25] = {0};

private:
    void _update();
    void _printTime(time_t t);
    void _retryLater();
    int _getLatestAocDay(time_t currentTime, int aocYear);
    void _updateWebClientConfig();
    void _updateLeaderboardPath();

    char _sessionKey[260];
    char _leaderboardHost[128];
    char _leaderboardPath[128];
    char _leaderboardId[16];
    char _userId[16];
    int _aocYear;
    int _leaderboardPort;

    bool _updateRequested = false;
    unsigned long _lastUpdateEpoch = 0;

    WiFiClient *_wifiClient;
    HttpClient *_httpClient;
    WiFiUDP _ntpUDP;
    NTPClient _ntpClient;
    StarLedManager *_starLedManager;

    std::map<int, int> _memoryMap;
};