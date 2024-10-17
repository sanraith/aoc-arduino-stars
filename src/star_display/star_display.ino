/******************  LIBRARY SECTION *************************************/
#include <ArduinoJson.h>
#include <FastLED.h>
#include "WiFiS3.h"
#include "arduino_secrets.h"
#include "WiFiSSLClient.h"
#include "AocClient.h"
#include "EEPROMManager.h"
#include "LocalServer.h"

/*****************  LED LAYOUT AND SETUP *********************************/
#define NUM_LEDS 60
#define LED_STRIP_DATA_PIN 4

/*****************  GLOBAL VARIABLES  ****************************************/
EEPROMManager memoryManager(1, 0);

/* Led strip setup */
CRGB leds[NUM_LEDS];
int idx = 1;
bool isAnimating = false;

/* Wifi */
char ssid[] = SECRET_SSID; // from "arduino_secrets.h"
char pass[] = SECRET_PASS; // from "arduino_secrets.h"
int led = LED_BUILTIN;
int status = WL_IDLE_STATUS;

/* Web Client */
char aocUserId[] = SECRET_AOC_USER_ID;
char leaderboardHost[] = SECRET_LEADERBOARD_HOST;
char leaderboardUrl[] = SECRET_LEADERBOARD_URL;
int leaderboardPort = SECRET_LEADERBOARD_PORT;
AocClient aocClient(&memoryManager, SECRET_AOC_SESSION_KEY, leaderboardHost, leaderboardUrl, leaderboardPort, SECRET_AOC_USER_ID);

/* WebServer */
LocalServer webServer(&aocClient);

/*****************  SETUP FUNCTIONS  ****************************************/
void setup()
{
  Serial.begin(115200);

  memoryManager.setup();

  wifiSetup();
  ledStripSetup();
  aocClient.setup();
  webServer.setup();
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
  ledStripLoop();
  aocClient.loop();
  webServer.loop();
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