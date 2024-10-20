#include "AocClient.h"
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <EEPROM.h>
#include <TimeLib.h>
#include "EEPROMManager.h"
#include "WiFiSSLClient.h"
#include "WiFiS3.h"

#define REQUEST_TIMEOUT_MILLIS 10000  /* 10 sec*/
#define RETRY_FREQUENCY_SECONDS 10    /* 10 sec */
#define UPDATE_FREQUENCY_SECONDS 3600 /* 1 hour */
#define TIMEZONE_OFFSET_HOURS 2       /* UTC + 2 */
#define EEPROM_LAST_UPDATE_EPOCH_ID 0
#define EEPROM_COMPLETION_STATE_ID 1

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

AocClient::AocClient(EEPROMManager *memoryManager, char sessionKey[], char leaderboardHost[], char leaderboardPath[], int leaderboardPort, char aocUserId[])
    : _sessionKey(sessionKey), _leaderboardHost(leaderboardHost), _leaderboardPath(leaderboardPath), _leaderboardPort(leaderboardPort), _aocUserId(aocUserId)
{
    _wifiClient = leaderboardPort == 443 ? new WiFiSSLClient() : new WiFiClient();
    _httpClient = new MyHttpClient(*_wifiClient, leaderboardHost, leaderboardPort);
    _lastUpdateEpoch = 0;
    _updateRequested = false;
    _memoryMap.insert({EEPROM_LAST_UPDATE_EPOCH_ID, memoryManager->registerSlot(sizeof(_lastUpdateEpoch))});
    _memoryMap.insert({EEPROM_COMPLETION_STATE_ID, memoryManager->registerSlot(sizeof(_completionState))});
}

void AocClient::setup()
{
    Serial.println("Setting up AocClient");
    _ntpClient = new NTPClient(_ntpUDP, "pool.ntp.org", TIMEZONE_OFFSET_HOURS * 3600, UPDATE_FREQUENCY_SECONDS * 1000);
    _ntpClient->begin();
    _ntpClient->update();

    EEPROM.get(_memoryMap.at(EEPROM_LAST_UPDATE_EPOCH_ID), _lastUpdateEpoch);
    EEPROM.get(_memoryMap.at(EEPROM_COMPLETION_STATE_ID), _completionState);
    if (_lastUpdateEpoch == 0)
    {
        _updateRequested = true;
    }

    Serial.print("Last update from EEPROM: ");
    _printTime(_lastUpdateEpoch);

    Serial.print("Current time: ");
    _printTime(_ntpClient->getEpochTime());
}

void AocClient::loop()
{
    // Update current time
    _ntpClient->update();
    unsigned long currentEpochTime = _ntpClient->getEpochTime();

    // Do not fetch leaderboard automatically if the year is already completed
    bool isYearCompleted = true;
    for (int i = 0; i < 25; i++)
    {
        if (_completionState[i] != 2)
        {
            isYearCompleted = false;
            break;
        }
    }

    // Fetch leaderboard update if requested or data is outdated
    if (_updateRequested ||
        (!isYearCompleted && (_lastUpdateEpoch > currentEpochTime || currentEpochTime - _lastUpdateEpoch >= UPDATE_FREQUENCY_SECONDS)))
    {
        Serial.println(_ntpClient->getFormattedTime());
        if (_updateRequested)
        {
            Serial.println("Update requested, udating...");
        }
        else
        {
            Serial.println(String(UPDATE_FREQUENCY_SECONDS) + " seconds have passed since last update, updating...");
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
    _lastUpdateEpoch = _ntpClient->getEpochTime() - UPDATE_FREQUENCY_SECONDS + RETRY_FREQUENCY_SECONDS;
    _wifiClient->stop();
    _httpClient->stop();
}

void AocClient::_update()
{
    Serial.println("\nConnecting to server...");
    int connectResult = _wifiClient->connect(_leaderboardHost, _leaderboardPort);
    if (connectResult)
    {
        Serial.println("Connected to server. Sending HTTP request...");
        // Make a HTTP request:
        _wifiClient->print("GET ");
        _wifiClient->print(_leaderboardPath);
        _wifiClient->println(" HTTP/1.1");
        _wifiClient->print("Host: ");
        _wifiClient->println(_leaderboardHost);
        _wifiClient->println("User-Agent: https://github.com/sanraith/aoc-arduino-stars by sanraith@users.noreply.github.com");
        _wifiClient->print("Cookie: session=");
        _wifiClient->println(_sessionKey);
        _wifiClient->println("Connection: close");
        _wifiClient->println();
    }
    else
    {
        Serial.println("Could not connect to server: " + connectResult);
        _retryLater();
        return;
    }

    // Wait for the response
    Serial.println("Waiting for response...");
    unsigned long timeout = millis() + REQUEST_TIMEOUT_MILLIS;
    while (_wifiClient->connected() && !_wifiClient->available() && millis() < timeout)
    {
        delay(10);
    }
    if (millis() >= timeout)
    {
        Serial.println("Request timed out.");
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
        Serial.print("Status code: ");
        Serial.println(statusCode);
        Serial.print("Response: ");
        Serial.println(_httpClient->responseBody());
        _retryLater();
        return;
    }
    _httpClient->skipResponseHeaders();

    // Filter to keep completion data only for our own user to save memory
    String jsonString;
    JsonDocument filter;
    JsonObject filter_member = filter["members"][_aocUserId].to<JsonObject>();
    filter_member["name"] = true;
    filter_member["last_star_ts"] = true;
    JsonObject filter_memberCompletionStarOnly = filter_member["completion_day_level"]["*"]["*"].to<JsonObject>();

    // Deserialize json from stream
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, *_httpClient, DeserializationOption::Filter(filter));
    if (error)
    {
        Serial.print("deserializeJson() returned ");
        Serial.println(error.c_str());
        _retryLater();
        return;
    }

    // Close the connection
    Serial.println("disconnecting from server.");
    _wifiClient->stop();
    _httpClient->stop();

    // Read completion data from json
    const char *userName = doc["members"][_aocUserId]["name"];
    Serial.print("Name: ");
    Serial.println(userName);
    bool updateHasChanges = false;
    for (int day = 0; day < 25; day++)
    {
        String dayStr = String(day + 1);
        const uint8_t dayCompletionState = doc["members"][_aocUserId]["completion_day_level"][dayStr].containsKey("1") +
                                           doc["members"][_aocUserId]["completion_day_level"][dayStr].containsKey("2");
        updateHasChanges |= dayCompletionState != _completionState[day];
        _completionState[day] = dayCompletionState;
        Serial.println("Completion day " + dayStr + ": " + String(dayCompletionState));
    }

    // Save update data to EEPROM
    _lastUpdateEpoch = _ntpClient->getEpochTime();
    Serial.println("Saving results to EEPROM...");
    EEPROM.put(_memoryMap.at(EEPROM_LAST_UPDATE_EPOCH_ID), _lastUpdateEpoch);
    EEPROM.put(_memoryMap.at(EEPROM_COMPLETION_STATE_ID), _completionState);
    Serial.println("Save completed.");
}
