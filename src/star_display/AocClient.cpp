#include "AocClient.h"
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <EEPROM.h>
#include <TimeLib.h>
#include "EEPROMManager.h"
#include "WiFiSSLClient.h"
#include "WiFiS3.h"

#define REQUEST_TIMEOUT_MILLIS 10000  /* 10 sec */
#define RETRY_FREQUENCY_SECONDS 60    /* 60 sec */
#define UPDATE_FREQUENCY_SECONDS 3600 /* 1 hour */
#define TIMEZONE_OFFSET_HOURS 2       /* UTC + 2 */

// IDs to keep track of EEPROM memory slots
#define EEPROM_LAST_UPDATE_EPOCH_ID 0
#define EEPROM_COMPLETION_STATE_ID 1

// Misc constants
#define TWO_STARS_COMPLETED 2
#define AOC_TIMEZONE_OFFSET_HOURS -5

// Created to use WiFiClient and HttpClient together
class MyHttpClient : public HttpClient
{
public:
    MyHttpClient(Client &aClient, const String &aServerName, uint16_t aServerPort);
    void markRequestSent();
};
MyHttpClient::MyHttpClient(Client &aClient, const String &aServerName, uint16_t aServerPort) : HttpClient(aClient, aServerName, aServerPort)
{
}
void MyHttpClient::markRequestSent()
{
    iState = eRequestSent;
}

AocClient::AocClient(EEPROMManager *memoryManager,
                     const char sessionKey[], int aocYear,
                     char leaderboardHost[], int leaderboardPort, char leaderboardUserId[],
                     char userId[])
    : _leaderboardPort(leaderboardPort), _aocYear(aocYear),
      _ntpClient(_ntpUDP, "pool.ntp.org", TIMEZONE_OFFSET_HOURS * 3600, UPDATE_FREQUENCY_SECONDS * 1000)
{
    snprintf(_userId, sizeof(_userId), "%s", userId);
    snprintf(_sessionKey, sizeof(_sessionKey), "%s", sessionKey);
    snprintf(_leaderboardHost, sizeof(_leaderboardHost), "%s", leaderboardHost);
    snprintf(_leaderboardPath, sizeof(_leaderboardPath), "/%d/leaderboard/private/view/%s.json", aocYear, leaderboardUserId);

    _wifiClient = leaderboardPort == 443 ? new WiFiSSLClient() : new WiFiClient();
    _httpClient = new MyHttpClient(*_wifiClient, leaderboardHost, leaderboardPort);
    _lastUpdateEpoch = 0;
    _updateRequested = false;
    _memoryMap.insert({EEPROM_LAST_UPDATE_EPOCH_ID, memoryManager->registerSlot(sizeof(_lastUpdateEpoch))});
    _memoryMap.insert({EEPROM_COMPLETION_STATE_ID, memoryManager->registerSlot(sizeof(_completionState))});
}

void AocClient::setup()
{
    Serial.println(F("Setting up AocClient"));
    Serial.print(F("Using leaderboard: "));
    Serial.print(_leaderboardHost);
    Serial.print(":");
    Serial.print(_leaderboardPort);
    Serial.println(_leaderboardPath);

    _ntpClient.begin();
    _ntpClient.update();

    EEPROM.get(_memoryMap.at(EEPROM_LAST_UPDATE_EPOCH_ID), _lastUpdateEpoch);
    EEPROM.get(_memoryMap.at(EEPROM_COMPLETION_STATE_ID), _completionState);
    if (_lastUpdateEpoch == 0)
    {
        _updateRequested = true;
    }

    Serial.print(F("Last update from EEPROM: "));
    _printTime(_lastUpdateEpoch);

    Serial.print(F("Current time: "));
    _printTime(_ntpClient.getEpochTime());
}

void AocClient::loop()
{
    // Update current time
    _ntpClient.update();
    unsigned long currentEpochTime = _ntpClient.getEpochTime();

    // Do not fetch leaderboard automatically if there are no new days to complete
    bool isYearComplete = true;
    int latestAocDayOfYear = _getLatestAocDay(currentEpochTime, _aocYear);
    for (int i = 0; i < latestAocDayOfYear; i++)
    {
        if (_completionState[i] != TWO_STARS_COMPLETED)
        {
            isYearComplete = false;
            break;
        }
    }

    // Fetch leaderboard update if requested or data is outdated
    int isUpdateDue = _lastUpdateEpoch > currentEpochTime || currentEpochTime - _lastUpdateEpoch >= UPDATE_FREQUENCY_SECONDS;
    if (_updateRequested || (!isYearComplete && isUpdateDue))
    {
        Serial.println(_ntpClient.getFormattedTime());
        if (_updateRequested)
        {
            Serial.println(F("Update requested, udating..."));
        }
        else
        {
            Serial.println(F("Data is outdated, updating..."));
        }
        _updateRequested = false;

        _update();
    }
}

void AocClient::requestUpdate()
{
    _updateRequested = true;
}

void AocClient::_printTime(time_t t)
{
    char buffer[50];
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", year(t), month(t), day(t), hour(t), minute(t), second(t));
    Serial.println(buffer);
}

void AocClient::_retryLater()
{
    _lastUpdateEpoch = _ntpClient.getEpochTime() - UPDATE_FREQUENCY_SECONDS + RETRY_FREQUENCY_SECONDS;
    _wifiClient->stop();
    _httpClient->stop();
}

void AocClient::_update()
{
    Serial.println(F("\nConnecting to server..."));
    int connectResult = _wifiClient->connect(_leaderboardHost, _leaderboardPort);
    if (connectResult)
    {
        Serial.println(F("Connected to server. Sending HTTP request..."));
        // Make a HTTP request:
        _wifiClient->print(F("GET "));
        _wifiClient->print(_leaderboardPath);
        _wifiClient->println(F(" HTTP/1.1"));
        _wifiClient->print(F("Host: "));
        _wifiClient->println(_leaderboardHost);
        _wifiClient->println(F("User-Agent: https://github.com/sanraith/aoc-arduino-stars by sanraith@users.noreply.github.com"));
        _wifiClient->print(F("Cookie: session="));
        _wifiClient->println(_sessionKey);
        _wifiClient->println(F("Connection: close"));
        _wifiClient->println();
    }
    else
    {
        Serial.println(F("Could not connect to server"));
        _retryLater();
        return;
    }

    // Wait for the response
    Serial.println(F("Waiting for response..."));
    unsigned long timeout = millis() + REQUEST_TIMEOUT_MILLIS;
    while (_wifiClient->connected() && !_wifiClient->available() && millis() < timeout)
    {
        delay(10);
    }
    if (millis() >= timeout)
    {
        Serial.println(F("Request timed out."));
        _retryLater();
        return;
    }

    // Parse the request with HttpClient. This is needed to abstract away chuncked responses.
    // HTTPS request did not really work with HttpClient,
    // so I send it myself and trick HttpClient to allow parsing the response
    ((MyHttpClient *)_httpClient)->markRequestSent();
    int statusCode = _httpClient->responseStatusCode();
    if (statusCode != 200)
    {
        Serial.print(F("Status code: "));
        Serial.println(statusCode);
        Serial.print(F("Response: "));
        Serial.println(_httpClient->responseBody());
        _retryLater();
        return;
    }
    _httpClient->skipResponseHeaders();

    // Filter to keep completion data only for our own user to save memory
    String jsonString;
    JsonDocument filter;
    JsonObject filter_member = filter["members"][_userId].to<JsonObject>();
    filter_member["name"] = true;
    filter_member["last_star_ts"] = true;
    JsonObject filter_memberCompletionStarOnly = filter_member["completion_day_level"]["*"]["*"].to<JsonObject>();

    // Deserialize json from stream
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, *_httpClient, DeserializationOption::Filter(filter));
    if (error)
    {
        Serial.print(F("deserializeJson() returned "));
        Serial.println(error.c_str());
        _retryLater();
        return;
    }

    // Close the connection
    Serial.println(F("disconnecting from server."));
    _wifiClient->stop();
    _httpClient->stop();

    // Read name for user from json to double check
    const char *userName = doc["members"][_userId]["name"];
    Serial.print(F("Name: "));
    Serial.println(userName);

    // Read completion data from json
    char dayStr[3];
    bool updateHasChanges = false;
    for (int day = 0; day < 25; day++)
    {
        snprintf(dayStr, sizeof(dayStr), "%d", day + 1);
        const uint8_t dayCompletionState = doc["members"][_userId]["completion_day_level"][dayStr].containsKey("1") +
                                           doc["members"][_userId]["completion_day_level"][dayStr].containsKey("2");
        updateHasChanges |= dayCompletionState != _completionState[day];
        _completionState[day] = dayCompletionState;
        Serial.println(String("Completion day ") + dayStr + ": " + String(dayCompletionState));
    }

    // Save update data to EEPROM
    _lastUpdateEpoch = _ntpClient.getEpochTime();
    Serial.println(F("Saving results to EEPROM..."));
    EEPROM.put(_memoryMap.at(EEPROM_LAST_UPDATE_EPOCH_ID), _lastUpdateEpoch);
    EEPROM.put(_memoryMap.at(EEPROM_COMPLETION_STATE_ID), _completionState);
    Serial.println(F("Save completed."));
}

/** Return the latest available day (1 for the 1st) of Advent of Code in the given year based on the current time. */
int AocClient::_getLatestAocDay(time_t currentTime, int aocYear)
{
    int currentYear = year(currentTime);
    int currentMonth = month(currentTime);
    int currentDay = day(currentTime);
    int currentHour = hour(currentTime);

    // Check if AOC is completely in the past or future
    if (aocYear < currentYear)
    {
        return 25; // aoc in the past
    }
    else if (aocYear > currentYear || currentMonth < 12)
    {
        return 0; // aoc in the future
    }

    // Puzzles unlock midnight UTC-5
    if (currentHour - TIMEZONE_OFFSET_HOURS + AOC_TIMEZONE_OFFSET_HOURS < 0)
    {
        currentDay -= 1;
    }
    currentDay = max(0, min(currentDay, 25));

    return currentDay;
}
