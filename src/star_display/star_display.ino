/******************  LIBRARY SECTION *************************************/
#include "WiFiS3.h"
#include "arduino_secrets.h"
#include "AocClient.h"
#include "EEPROMManager.h"
#include "LocalServer.h"
#include "StarLedManager.h"

/*****************  GLOBAL VARIABLES  ****************************************/
EEPROMManager memoryManager(1, 0);

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
AocClient aocClient(&memoryManager, SECRET_AOC_SESSION_KEY, leaderboardHost, leaderboardUrl, leaderboardPort, aocUserId);

/* WebServer */
LocalServer webServer(&aocClient);

/* Star leds */
StarLedManager starLedManager;

/*****************  SETUP FUNCTIONS  ****************************************/
void setup()
{
  Serial.begin(115200);

  memoryManager.setup();

  wifiSetup();
  starLedManager.setup();
  aocClient.setup();
  webServer.setup();
}

/*****************  MAIN LOOP  ****************************************/
void loop()
{
  starLedManager.loop();
  aocClient.loop();
  webServer.loop();
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
