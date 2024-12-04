/******************  LIBRARY SECTION *************************************/
#include "arduino_secrets.h"
#include "AocClient.h"
#include "EEPROMManager.h"
#include "LocalServer.h"
#include "StarLedManager.h"
#include "WifiManager.h"

/*****************  GLOBAL VARIABLES  ****************************************/

const int targetFrameTimeMillis = 1000 / 1;
unsigned long prevFrameStartTime = 0;

/* Wifi */
char ssid[] = SECRET_SSID; // from "arduino_secrets.h"
char pass[] = SECRET_PASS; // from "arduino_secrets.h"

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
StarLedManager *starLedManager;
WifiManager *wifiManager;

/*****************  SETUP FUNCTIONS  ****************************************/
void setup()
{
  Serial.begin(115200);

  starLedManager = new StarLedManager();
  wifiManager = new WifiManager(ssid, pass, starLedManager);
  memoryManager = new EEPROMManager(/* version */ 3, /* startAddress */ 0);
#ifdef SECRET_AOC_USER_ID // from "arduino_secrets.h"
  aocClient = new AocClient(memoryManager, SECRET_AOC_SESSION_KEY, aocYear,
                            leaderboardHost, leaderboardPort, SECRET_AOC_LEADERBOARD_ID, SECRET_AOC_USER_ID, starLedManager);
#else
  aocClient = new AocClient(memoryManager, "", aocYear,
                            leaderboardHost, leaderboardPort, "0", "0", starLedManager);
#endif
  webServer = new LocalServer(aocClient, starLedManager);
  starLedManager->setup();
  starLedManager->updateProgress(0.10);

// Set IP from arduino_secrets.h otherwise fall back to DHCP
#ifdef SECRET_ARDUINO_IP_ADDRESS // from "arduino_secrets.h"
  wifiManager->setIpAddress(SECRET_ARDUINO_IP_ADDRESS);
#endif

  wifiManager->setup();
  memoryManager->setup();
  aocClient->setup();
  webServer->setup();
}

/*****************  MAIN LOOP  ****************************************/
void loop()
{
  unsigned long frameStartTime = millis();
  unsigned long prevFrameMs = frameStartTime - prevFrameStartTime;
  unsigned int remainingFrameMs = max(0, targetFrameTimeMillis - min(targetFrameTimeMillis, prevFrameMs));
  if (remainingFrameMs > 0)
  {
    delay(remainingFrameMs);
    frameStartTime = millis();
    prevFrameMs = frameStartTime - prevFrameStartTime;
  }
  prevFrameStartTime = frameStartTime;

  starLedManager->loop(frameStartTime, prevFrameMs);
  aocClient->loop();
  webServer->loop();
}
