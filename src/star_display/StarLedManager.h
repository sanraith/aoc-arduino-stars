#pragma once

#include <FastLED.h>
#include <map>

#define NUM_LEDS 60
#define LED_STRIP_DATA_PIN 4

class StarLedManager
{
public:
    StarLedManager();
    void setup();
    void loop();

private:
    CRGB _leds[NUM_LEDS];
    int _idx;
    bool _isAnimating;
    int _dayToLedMap[25];
};