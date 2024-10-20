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
    FastLED.show();
}

void StarLedManager::loop()
{
    delay(75);
    if (_isAnimating)
    {
        if (_idx >= NUM_LEDS)
        {
            _idx = 1;
        }
        if (_leds[_idx].r > 0)
        {
            _leds[_idx] = CRGB(0, 0, 0);
        }
        else
        {
            _leds[_idx] = CRGB(255, 255, 0);
        }
        _idx = _idx + 2;
    }
    else
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            _leds[_idx] = CRGB(0, 0, 0);
        }
        FastLED.clear(true);
    }

    FastLED.setBrightness(25);
    FastLED.show();
}
