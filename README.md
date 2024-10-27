# aoc-arduino-stars

Display Advent of Code stars via an arduino and some led strips.

## Dev environment

- Arduino
  - Board: ARDUINO-UNO-R4-WIFI
  - Led strip chip: WS2813B
  - Download Arduino IDE
  - Install libraries within Arduino IDE:
    - ArduinoHttpClient (0.6.1)
    - ArduinoJson (7.2.0)
    - FastLED (3.7.8)
    - NTPClient (3.2.1)
    - Time (1.6.1)

## Setup secrets

Create `src/star_display/arduino_secrets.h` with the following content:

```cpp
#include <avr/pgmspace.h>

// Required, wifi connection info
#define SECRET_SSID "YOUR WIFI NAME"
#define SECRET_PASS "YOUR WIFI PASSWORD"

// Optional, will fall back to DHCP if not provided
#define SECRET_ARDUINO_IP_ADDRESS "192.168.0.123" // Local IP address of the Arduino

// Optional, can be set on the web interface
const char SECRET_AOC_SESSION_KEY[] PROGMEM = "YOUR_SESSION_KEY"; // Your Advent of Code session key
#define SECRET_AOC_LEADERBOARD_ID "1234" // ID of the private leaderboard the app will query
#define SECRET_AOC_USER_ID "1234" // ID of the user the stars will be queried for
#define SECRET_LEADERBOARD_HOST "192.168.0.1" // Alternative IP address to get the leaderboard json from
#define SECRET_LEADERBOARD_PORT 5500 // Alternative port to get the leaderboard json from
```
