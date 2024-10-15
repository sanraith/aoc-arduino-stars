/******************  LIBRARY SECTION *************************************/
#include <ArduinoJson.h>
#include <FastLED.h>
#include "WiFiS3.h"
#include "arduino_secrets.h"
#include "WiFiSSLClient.h"

/*****************  LED LAYOUT AND SETUP *********************************/
#define NUM_LEDS 60
#define LED_STRIP_DATA_PIN 4

/*****************  GLOBAL VARIABLES  ****************************************/
/* Led strip setup */
CRGB leds[NUM_LEDS];
int idx = 1;
bool isAnimating = false;

/* Wifi */
char ssid[] = SECRET_SSID; // from "arduino_secrets.h"
char pass[] = SECRET_PASS; // from "arduino_secrets.h"
int led = LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiServer server(80);

/* Web Client */
// #define LEADERBOARD_HTTPS 1
char aocUserId[] = SECRET_AOC_USER_ID;
char leaderboardHost[] = SECRET_LEADERBOARD_HOST;
char leaderboardUrl[] = "/temp/board.json";
int leaderboardPort = 5500;
bool leaderBoardSSL = false;
WiFiSSLClient httpsClient;
WiFiClient httpClient;
WiFiClient *client;

/*****************  SETUP FUNCTIONS  ****************************************/
void setup()
{
  Serial.begin(115200);
  wifiSetup();
  ledStripSetup();
  webClientSetup();
}

void webClientSetup()
{
  Serial.println("\nStarting connection to server...");
  client = leaderBoardSSL ? &httpsClient : &httpClient;
  int connectResult = client->connect(leaderboardHost, leaderboardPort);
  if (connectResult)
  {
    Serial.println("connected to server");
    // Make a HTTP request:
    client->print("GET ");
    client->print(leaderboardUrl);
    client->println(" HTTP/1.1");
    client->print("Host: ");
    client->println(leaderboardHost);
    client->println("Connection: close");
    client->println();
  }
  else
  {
    Serial.println("Could not connect to server: " + connectResult);
    return;
  }

  String jsonString;
  if (client->connected())
  {
    // Jump after the HTTP headers
    client->find("\r\n\r\n"); // TODO handle possible errors, check that state is 200, etc

    // Filter to keep completion data only for our own user to save memory
    JsonDocument filter;
    JsonObject filter_member = filter["members"][aocUserId].to<JsonObject>();
    filter_member["name"] = true;
    filter_member["last_star_ts"] = true;
    JsonObject filter_memberCompletionStarOnly = filter_member["completion_day_level"]["*"]["*"].to<JsonObject>();

    // Deserialize json from stream
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, *client, DeserializationOption::Filter(filter));

    if (error)
    {
      Serial.print("deserializeJson() returned ");
      Serial.println(error.c_str());
      return;
    }

    const char *userName = doc["members"][aocUserId]["name"];
    Serial.print("Name: ");
    Serial.println(userName);
    for (int day = 1; day <= 25; day++)
    {
      String dayStr = String(day);
      const int completionState = doc["members"][aocUserId]["completion_day_level"][dayStr].containsKey("1") +
                                  doc["members"][aocUserId]["completion_day_level"][dayStr].containsKey("2");
      Serial.print("Day ");
      Serial.print(day);
      Serial.print(" state: ");
      Serial.println(completionState);
    }
  }

  Serial.println();
  Serial.println("disconnecting from server.");
  client->stop();
}

void wifiSetup()
{
  pinMode(led, OUTPUT); // set the LED pin mode

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    while (true)
      ; // don't continue
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
  {
    Serial.println("Please upgrade the firmware");
  }

// Set IP from arduino_secrets.h otherwise fall back to DHCP
#ifdef IP_ADDRESS
  WiFi.config(IPAddress(IP_ADDRESS));
#endif

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid); // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(1000);
  }
  server.begin();    // start the web server on port 80
  printWifiStatus(); // you're connected now, so print out the status
}

void ledStripSetup()
{
  FastLED.addLeds<WS2813, LED_STRIP_DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.show();
}

/*****************  MAIN LOOP  ****************************************/
void loop()
{
  wifiLoop();
  ledStripLoop();
  // webClientLoop();
}

void wifiLoop()
{
  WiFiClient client = server.available(); // listen for incoming clients

  // TODO handle the client without the while true, I get it stuck if I spam click multiple times
  // maybe add a timeout check in the while loop

  if (client)
  {                               // if you get a client,
    Serial.println("new client"); // print a message out the serial port
    String currentLine = "";      // make a String to hold incoming data from the client
    while (client.connected())
    { // loop while the client's connected
      if (client.available())
      {                         // if there's bytes to read from the client,
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
        {                   // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }

        if (currentLine.endsWith("GET /update"))
        {
          for (int i = 0; i < 10; i++)
          {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(25);
            digitalWrite(LED_BUILTIN, LOW);
            delay(75);
          }
        }
        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H"))
        {
          // digitalWrite(LED_BUILTIN, HIGH); // GET /H turns the LED on
          isAnimating = true;
        }
        if (currentLine.endsWith("GET /L"))
        {
          // digitalWrite(LED_BUILTIN, LOW); // GET /L turns the LED off
          isAnimating = false;
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void ledStripLoop()
{
  delay(75);
  if (isAnimating)
  {
    if (idx >= NUM_LEDS)
    {
      idx = 1;
    }
    if (leds[idx].r > 0)
    {
      leds[idx] = CRGB(0, 0, 0);
    }
    else
    {
      leds[idx] = CRGB(255, 255, 0);
    }
    idx = idx + 2;
  }
  else
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[idx] = CRGB(0, 0, 0);
    }
    FastLED.clear(true);
  }

  FastLED.setBrightness(25);
  FastLED.show();
}