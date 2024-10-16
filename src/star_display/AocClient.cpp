#include "AocClient.h"
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <EEPROM.h>
#include <TimeLib.h>
#include "WiFiSSLClient.h"
#include "WiFiS3.h"

#define UPDATE_FREQUENCY_SECONDS 3600 /* 1 hour */
#define EEPROM_LAST_UPDATE_EPOCH_ADDRESS 0

AocClient::AocClient(char sessionKey[], char leaderboardHost[], char leaderboardPath[], int leaderboardPort, char aocUserId[])
{
    _sessionKey = sessionKey;
    _leaderboardHost = leaderboardHost;
    _leaderboardPath = leaderboardPath;
    _leaderboardPort = leaderboardPort;
    _aocUserId = aocUserId;
    _wifiClient = leaderboardPort == 430 ? new WiFiSSLClient() : new WiFiClient();
    _lastUpdateEpoch = 0;
}

void AocClient::setup()
{
    Serial.println("Setting up AocClient");
    Serial.println("EEPROM length: " + String(EEPROM.length()));
    _lastUpdateEpoch = _getLastUpdateEpochFromEEPROM();
    Serial.print("Last update from EEPROM: ");
    _printTime(_lastUpdateEpoch);
    _ntpClient = new NTPClient(_ntpUDP, "pool.ntp.org", 2 * 3600, UPDATE_FREQUENCY_SECONDS * 1000);
    _ntpClient->begin();
    _ntpClient->update();

    Serial.print("Current time: ");
    _printTime(_ntpClient->getEpochTime());
}

void AocClient::_printTime(time_t t)
{
    char buffer[50];
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", year(t), month(t), day(t), hour(t), minute(t), second(t));
    Serial.println(buffer);
}

void AocClient::loop()
{
    _ntpClient->update();
    unsigned long currentEpochTime = _ntpClient->getEpochTime();
    if (_lastUpdateEpoch > currentEpochTime || currentEpochTime - _lastUpdateEpoch >= UPDATE_FREQUENCY_SECONDS)
    {
        Serial.println(_ntpClient->getFormattedTime());
        if (_lastUpdateEpoch == 0)
        {
            Serial.println("Update requested, udating...");
        }
        else
        {
            Serial.println(String(UPDATE_FREQUENCY_SECONDS) + " seconds have passed since last update, updating...");
        }
        _update();
    }
}

void AocClient::requestUpdate()
{
    _lastUpdateEpoch = 0;
}

void AocClient::_update()
{
    Serial.println("\nStarting connection to server...");
    int connectResult = _wifiClient->connect(_leaderboardHost, _leaderboardPort);
    if (connectResult)
    {
        Serial.println("connected to server");
        // Make a HTTP request:
        _wifiClient->print("GET ");
        _wifiClient->print(_leaderboardPath);
        _wifiClient->println(" HTTP/1.1");
        _wifiClient->print("Host: ");
        _wifiClient->println(_leaderboardHost);
        _wifiClient->println("Connection: close");
        _wifiClient->println();
    }
    else
    {
        Serial.println("Could not connect to server: " + connectResult);
        return;
    }

    String jsonString;
    if (_wifiClient->connected())
    {
        // Jump after the HTTP headers
        _wifiClient->find("\r\n\r\n"); // TODO handle possible errors, check that state is 200, etc

        // Filter to keep completion data only for our own user to save memory
        JsonDocument filter;
        JsonObject filter_member = filter["members"][_aocUserId].to<JsonObject>();
        filter_member["name"] = true;
        filter_member["last_star_ts"] = true;
        JsonObject filter_memberCompletionStarOnly = filter_member["completion_day_level"]["*"]["*"].to<JsonObject>();

        // Deserialize json from stream
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, *_wifiClient, DeserializationOption::Filter(filter));

        if (error)
        {
            Serial.print("deserializeJson() returned ");
            Serial.println(error.c_str());
            return;
        }

        const char *userName = doc["members"][_aocUserId]["name"];
        Serial.print("Name: ");
        Serial.println(userName);
        for (int day = 0; day < 25; day++)
        {
            String dayStr = String(day + 1);
            const int completionState = doc["members"][_aocUserId]["completion_day_level"][dayStr].containsKey("1") +
                                        doc["members"][_aocUserId]["completion_day_level"][dayStr].containsKey("2");
            _completionState[day] = completionState;
        }
    }

    Serial.println();
    Serial.println("disconnecting from server.");
    _wifiClient->stop();

    _lastUpdateEpoch = _ntpClient->getEpochTime();
    _saveLastUpdateEpochFromEEPROM(_lastUpdateEpoch);
}

unsigned long AocClient::_getLastUpdateEpochFromEEPROM()
{
    unsigned long epoch;
    EEPROM.get(EEPROM_LAST_UPDATE_EPOCH_ADDRESS, epoch);
    return epoch;
}

void AocClient::_saveLastUpdateEpochFromEEPROM(unsigned long epoch)
{
    EEPROM.put(EEPROM_LAST_UPDATE_EPOCH_ADDRESS, epoch);
}