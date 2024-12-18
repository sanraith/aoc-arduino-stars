/******************  LIBRARY SECTION *************************************/
#include "arduino_secrets.h"
#include "AocClient.h"
#include "EEPROMManager.h"
#include "LocalServer.h"
#include "StarLedManager.h"
#include "WifiManager.h"
#include "ButtonManager.h"

/*****************  GLOBAL VARIABLES  ****************************************/

const int BUTTON_PIN = 5;
const int targetFrameTimeMs = 1000 / 40; // 40 fps
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
ButtonManager *buttonManager;

/*****************  SETUP FUNCTIONS  ****************************************/

void onButtonRelease()
{
  Serial.println(F("button pressed, restarting animation"));
  Serial.println(millis());
  if (starLedManager && aocClient)
  {
    starLedManager->resetAnimation();
    starLedManager->updateCompletionState(aocClient->_completionState);
  }
}

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
  buttonManager = new ButtonManager(BUTTON_PIN);

// Set IP from arduino_secrets.h otherwise fall back to DHCP
#ifdef SECRET_ARDUINO_IP_ADDRESS // from "arduino_secrets.h"
  wifiManager->setIpAddress(SECRET_ARDUINO_IP_ADDRESS);
#endif

  wifiManager->setup();
  memoryManager->setup();
  aocClient->setup();
  webServer->setup();
  buttonManager->setup(nullptr, onButtonRelease);
}

/*****************  MAIN LOOP  ****************************************/
void loop()
{
  unsigned long frameStartTime = millis();
  unsigned long prevFrameMs = frameStartTime - prevFrameStartTime;
  unsigned int remainingFrameMs = max(0, targetFrameTimeMs - min(targetFrameTimeMs, prevFrameMs));
  if (remainingFrameMs > 0)
  {
    delay(remainingFrameMs);
    frameStartTime = millis();
    prevFrameMs = frameStartTime - prevFrameStartTime;
  }
  prevFrameStartTime = frameStartTime;

  // Slow down animations in case of long frame times instead of skipping a large chunk of them
  prevFrameMs = min(targetFrameTimeMs * 2, prevFrameMs);

  starLedManager->loop(frameStartTime, prevFrameMs);
  aocClient->loop();
  webServer->loop();
  buttonManager->loop();
}
