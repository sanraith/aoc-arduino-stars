#include "StarLedManager.h"
#include <FastLED.h>

StarLedManager::StarLedManager() : _idx(0), _isAnimating(false)
{
    for (int i = 0; i < 25; i++)
    {
        _dayToLedMap[i] = i * 2;
    }
}

void StarLedManager::setup()
{
    FastLED.addLeds<WS2813, LED_STRIP_DATA_PIN, GRB>(_leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 1000);
    FastLED.clear(true);
    FastLED.show();
}

void StarLedManager::loop()
{
    int led1 = 0;
    int led2 = 2;
    delay(1000);
    switch (_idx)
    {
    case 0:
        _leds[led1] = CRGB(0, 0, 0);
        _leds[led2] = CRGB(0, 0, 0);
        break;
    case 1:
        _leds[led1] = CRGB(255, 255, 0); // Yellow color
        _leds[led2] = CRGB(0, 0, 0);
        break;
    case 2:
        _leds[led1] = CRGB(0, 0, 255);   // Yellow color
        _leds[led2] = CRGB(255, 128, 0); // Yellow color
        break;
    case 3:
        _leds[led1] = CRGB(0, 0, 0);
        _leds[led2] = CRGB(255, 0, 0); // Yellow color
        break;
    }

    _idx = (_idx + 1) % 4; // Cycle through 0, 1, 2, 3
    FastLED.setBrightness(127);

    // delay(75);
    // if (_isAnimating)
    // {
    //     if (_idx >= NUM_LEDS)
    //     {
    //         _idx = 1;
    //     }
    //     if (_leds[_idx].r > 0)
    //     {
    //         _leds[_idx] = CRGB(0, 0, 0);
    //     }
    //     else
    //     {
    //         _leds[_idx] = CRGB(255, 255, 0);
    //     }
    //     _idx = _idx + 2;
    // }
    // else
    // {
    //     for (int i = 0; i < NUM_LEDS; i++)
    //     {
    //         _leds[_idx] = CRGB(0, 0, 0);
    //     }
    //     FastLED.clear(true);
    // }
    // FastLED.setBrightness(25);

    FastLED.show();
}
