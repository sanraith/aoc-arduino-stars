#include "LocalServer.h"
#include "LocalServer_res.h"
#include "WiFiS3.h"
#include "AocClient.h"

LocalServer::LocalServer(AocClient *aocClient, StarLedManager *starLedManager)
    : _server(80), _aocClient(aocClient), _starLedManager(starLedManager)
{
}

void LocalServer::setup()
{
    _server.begin();
    _starLedManager->updateProgress(0.80);
}

String readGetParameter(WiFiClient &client, String &currentLine)
{
    while (client.read() != '=')
        ;

    char c;
    String value = "";
    while ((c = client.read()) != '\r' && c != ' ')
    {
        value += c;
    }
    currentLine = "";
    return value;
}

void LocalServer::loop()
{
    WiFiClient client = _server.available(); // listen for incoming clients

    // TODO handle the client without the while true, I get it stuck if I spam click multiple times
    // maybe add a timeout check in the while loop
    if (client)
    {                                 // if you get a client,
        Serial.println("new client"); // print a message out the serial port
        String currentLine = "";      // make a String to hold incoming data from the client
        while (client.connected())
        { // loop while the client's connected
            if (client.available())
            {                           // if there's bytes to read from the client,
                char c = client.read(); // read a byte, then
                Serial.write(c);        // print it out to the serial monitor
                if (c == '\n')
                { // if the byte is a newline character

                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0)
                    {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();

                        // the content of the HTTP response follows the header:
                        char websiteContent[sizeof(WEBSITE_SOURCE_TEMPLATE) + 1024];
                        snprintf(websiteContent, sizeof(websiteContent), WEBSITE_SOURCE_TEMPLATE, _aocClient->getAocYear(),
                                 _aocClient->getUserId(), _aocClient->getLeaderboardId(), _aocClient->getSessionKey(),
                                 _aocClient->getLeaderboardHost(), _aocClient->getLeaderboardPort());

                        // Send the formatted HTML content to the client
                        client.print(websiteContent);

                        // The HTTP response ends with another blank line:
                        client.println();
                        // break out of the while loop:
                        break;
                    }
                    else
                    { // if you got a newline, then clear currentLine:
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {                     // if you got anything else but a carriage return character,
                    currentLine += c; // add it to the end of the currentLine
                }

                if (currentLine.endsWith("GET /sessionKey"))
                {
                    String value = readGetParameter(client, currentLine);
                    Serial.println("\nSetting session key to '" + value + "'");
                    _aocClient->setSessionKey(value.c_str());
                }
                else if (currentLine.endsWith("GET /year"))
                {
                    String value = readGetParameter(client, currentLine);
                    Serial.println("\nSetting year to '" + value + "'");
                    _aocClient->setAocYear(value.toInt());
                }
                else if (currentLine.endsWith("GET /leaderboardId"))
                {
                    String value = readGetParameter(client, currentLine);
                    Serial.println("\nSetting leaderboard ID to '" + value + "'");
                    _aocClient->setLeaderboardId(value.c_str());
                }
                else if (currentLine.endsWith("GET /userId"))
                {
                    String value = readGetParameter(client, currentLine);
                    Serial.println("\nSetting user ID to '" + value + "'");
                    _aocClient->setUserId(value.c_str());
                }
                else if (currentLine.endsWith("GET /leaderboardHost"))
                {
                    String value = readGetParameter(client, currentLine);
                    Serial.println("\nSetting leaderboard host to '" + value + "'");
                    _aocClient->setLeaderboardHost(value.c_str());
                }
                else if (currentLine.endsWith("GET /leaderboardPort"))
                {
                    String value = readGetParameter(client, currentLine);
                    Serial.println("\nSetting leaderboard port to '" + value + "'");
                    _aocClient->setLeaderboardPort(value.toInt());
                }
                else if (currentLine.endsWith("GET /update"))
                {
                    _aocClient->requestUpdate();
                }
            }
        }
        // close the connection:
        client.stop();
        Serial.println("client disconnected");
    }
}
