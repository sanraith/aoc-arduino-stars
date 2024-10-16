#include "AocClient.h"
#include <ArduinoJson.h>
#include <NTPClient.h>
#include "WiFiSSLClient.h"
#include "WiFiS3.h"

AocClient::AocClient(char sessionKey[], char leaderboardHost[], char leaderboardPath[], int leaderboardPort, char aocUserId[])
{
    _sessionKey = sessionKey;
    _leaderboardHost = leaderboardHost;
    _leaderboardPath = leaderboardPath;
    _leaderboardPort = leaderboardPort;
    _aocUserId = aocUserId;
    _client = leaderboardPort == 430 ? new WiFiSSLClient() : new WiFiClient();
}

void AocClient::setup()
{
    _ntpClient = new NTPClient(_ntpUDP, "pool.ntp.org", 3600, 60000);
    Serial.println("Setting up aoc");
    this->update(false);
}

void AocClient::loop()
{
    _ntpClient->update();
    Serial.println(_ntpClient->getFormattedTime());
    delay(1000);
}

void AocClient::update(bool isForced)
{
    Serial.println("\nStarting connection to server...");
    int connectResult = _client->connect(_leaderboardHost, _leaderboardPort);
    if (connectResult)
    {
        Serial.println("connected to server");
        // Make a HTTP request:
        _client->print("GET ");
        _client->print(_leaderboardPath);
        _client->println(" HTTP/1.1");
        _client->print("Host: ");
        _client->println(_leaderboardHost);
        _client->println("Connection: close");
        _client->println();
    }
    else
    {
        Serial.println("Could not connect to server: " + connectResult);
        return;
    }

    String jsonString;
    if (_client->connected())
    {
        // Jump after the HTTP headers
        _client->find("\r\n\r\n"); // TODO handle possible errors, check that state is 200, etc

        // Filter to keep completion data only for our own user to save memory
        JsonDocument filter;
        JsonObject filter_member = filter["members"][_aocUserId].to<JsonObject>();
        filter_member["name"] = true;
        filter_member["last_star_ts"] = true;
        JsonObject filter_memberCompletionStarOnly = filter_member["completion_day_level"]["*"]["*"].to<JsonObject>();

        // Deserialize json from stream
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, *_client, DeserializationOption::Filter(filter));

        if (error)
        {
            Serial.print("deserializeJson() returned ");
            Serial.println(error.c_str());
            return;
        }

        const char *userName = doc["members"][_aocUserId]["name"];
        Serial.print("Name: ");
        Serial.println(userName);
        for (int day = 1; day <= 25; day++)
        {
            String dayStr = String(day);
            const int completionState = doc["members"][_aocUserId]["completion_day_level"][dayStr].containsKey("1") +
                                        doc["members"][_aocUserId]["completion_day_level"][dayStr].containsKey("2");
            Serial.print("Day ");
            Serial.print(day);
            Serial.print(" state: ");
            Serial.println(completionState);
        }
    }

    Serial.println();
    Serial.println("disconnecting from server.");
    _client->stop();
}