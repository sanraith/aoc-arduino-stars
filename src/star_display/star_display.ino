/******************  LIBRARY SECTION *************************************/
#include "WiFiS3.h"
#include "arduino_secrets.h"
#include "AocClient.h"
#include "EEPROMManager.h"
#include "LocalServer.h"
#include "StarLedManager.h"

/*****************  GLOBAL VARIABLES  ****************************************/

/* Wifi */
char ssid[] = SECRET_SSID; // from "arduino_secrets.h"
char pass[] = SECRET_PASS; // from "arduino_secrets.h"
int wifiStatus = WL_IDLE_STATUS;

/* AOC Web Client */
int aocYear = 2024;
#ifdef SECRET_LEADERBOARD_HOST
char leaderboardHost[] = SECRET_LEADERBOARD_HOST; // from "arduino_secrets.h"
int leaderboardPort = SECRET_LEADERBOARD_PORT;    // from "arduino_secrets.h"
#else
char leaderboardHost[] = "adventofcode.com";
int leaderboardPort = 443;
#endif

/* Modules */
EEPROMManager *memoryManager;
AocClient *aocClient;
LocalServer *webServer;
StarLedManager starLedManager;

/*****************  SETUP FUNCTIONS  ****************************************/
void setup()
{
  Serial.begin(115200);

  memoryManager = new EEPROMManager(/* version */ 3, /* startAddress */ 0);
#ifdef SECRET_AOC_USER_ID // from "arduino_secrets.h"
  aocClient = new AocClient(memoryManager, SECRET_AOC_SESSION_KEY, aocYear,
                            leaderboardHost, leaderboardPort, SECRET_AOC_LEADERBOARD_ID, SECRET_AOC_USER_ID);
#else
  aocClient = new AocClient(memoryManager, "", aocYear,
                            leaderboardHost, leaderboardPort, "0", "0");
#endif
  webServer = new LocalServer(aocClient);

  memoryManager->setup();
  wifiSetup();
  starLedManager.setup();
  aocClient->setup();
  webServer->setup();
}

/*****************  MAIN LOOP  ****************************************/
void loop()
{
  starLedManager.loop();
  aocClient->loop();
  webServer->loop();
}

void wifiSetup()
{
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
#ifdef SECRET_ARDUINO_IP_ADDRESS
  WiFi.config(IPAddress(SECRET_ARDUINO_IP_ADDRESS));
#endif

  // attempt to connect to WiFi network:
  while (wifiStatus != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid); // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    wifiStatus = WiFi.begin(ssid, pass);
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
