#include <NTPClient.h>
#include "WiFiS3.h"

class AocClient
{
public:
    AocClient(char sessionKey[], char leaderboardHost[], char leaderboardPath[], int leaderboardPort, char aocUserId[]);
    void setup();
    void loop();
    void requestUpdate();

private:
    void _update();
    unsigned long _getLastUpdateEpochFromEEPROM();
    void _saveLastUpdateEpochFromEEPROM(unsigned long epoch);
    void _printTime(time_t t);

    char *_sessionKey;
    char *_leaderboardHost;
    char *_leaderboardPath;
    char *_aocUserId;
    int _leaderboardPort;
    int _lastUpdate;
    int _completionState[25];
    unsigned long _lastUpdateEpoch;
    WiFiClient *_wifiClient;

    WiFiUDP _ntpUDP;
    NTPClient *_ntpClient;
};