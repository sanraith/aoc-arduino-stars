#include "WifiManager.h"
#include "WiFiS3.h"

WifiManager::WifiManager(char ssid[], char pass[], StarLedManager *starLedManager) : _ssid(ssid), _pass(pass), _starLedManager(starLedManager)
{
}

void WifiManager::setIpAddress(char ipAddress[])
{
    WiFi.config(IPAddress(ipAddress));
    WiFi.setTimeout(5000);
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

void WifiManager::setup()
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
    _starLedManager->updateProgress(0.20);

    // attempt to connect to WiFi network:
    while (wifiStatus != WL_CONNECTED)
    {
        _starLedManager->updateProgress(0.20);
        Serial.print("Attempting to connect to Network named: ");
        Serial.println(_ssid); // print the network name (SSID);

        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        wifiStatus = WiFi.begin(_ssid, _pass);

        // wait a bit, then retry again
        if (wifiStatus != WL_CONNECTED)
        {
            delay(500);
        }
    }
    _starLedManager->updateProgress(0.40);

    printWifiStatus(); // you're connected now, so print out the status
}