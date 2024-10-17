#include "LocalServer.h"
#include "WiFiS3.h"
#include "AocClient.h"

LocalServer::LocalServer(AocClient *aocClient) : _server(80), _aocClient(aocClient)
{
}

void LocalServer::setup()
{
    _server.begin();
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
                        client.print("<html><head></head><body><p style=\"font-size:24px;\">Click <a href=\"/H\">here</a> turn the LED on<br></p>");
                        client.print("<p style=\"font-size:24px;\">Click <a href=\"/L\">here</a> turn the LED off<br></p></body></html>");

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

                if (currentLine.endsWith("GET /update"))
                {
                    _aocClient->requestUpdate();
                }
                // Check to see if the client request was "GET /H" or "GET /L":
                if (currentLine.endsWith("GET /H"))
                {
                    // digitalWrite(LED_BUILTIN, HIGH); // GET /H turns the LED on
                    // isAnimating = true; // TODO
                }
                if (currentLine.endsWith("GET /L"))
                {
                    // digitalWrite(LED_BUILTIN, LOW); // GET /L turns the LED off
                    // isAnimating = false; // TODO
                }
            }
        }
        // close the connection:
        client.stop();
        Serial.println("client disconnected");
    }
}
