#include <NTPClient.h>
#include "WiFiS3.h"

class AocClient
{
public:
    AocClient(char sessionKey[], char leaderboardHost[], char leaderboardPath[], int leaderboardPort, char aocUserId[]);
    void setup();
    void loop();
    void update(bool isForced);

private:
    char *_sessionKey;
    char *_leaderboardHost;
    char *_leaderboardPath;
    char *_aocUserId;
    int _leaderboardPort;
    int _lastUpdate;
    int _completionState[25];
    WiFiClient *_client;

    WiFiUDP _ntpUDP;
    NTPClient *_ntpClient;
};