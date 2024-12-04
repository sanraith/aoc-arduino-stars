#include "StarAnimation.h"
#include "FallingStarAnimation.h"

FallingStarAnimation::FallingStarAnimation(CRGB *leds, unsigned long animationLengthMs, uint8_t day)
    : StarAnimation(leds, animationLengthMs * (NUM_DAYS - day)), day(day)
{
}

StarAnimationState FallingStarAnimation::draw(unsigned long applicationTimeMs, unsigned long lastFrameMs)
{
    StarAnimationState state = StarAnimation::draw(applicationTimeMs, lastFrameMs);

    double span = NUM_DAYS - day;
    double litStarPos = NUM_DAYS * 1.0 - min(1.0, _elapsedMs * 1.0 / _animationLengthMs) * span + 1;

    CRGB starColor = CRGB(255, 255, 0);
    uint8_t mainBrightness = (litStarPos - (double)(int)litStarPos) * 255;
    uint8_t remainderBrightness = 255 - mainBrightness;
    Serial.println(mainBrightness);
    Serial.println(remainderBrightness);
    Serial.println("");
    uint8_t mainIdx = DAY_TO_LED_MAP[(uint8_t)litStarPos];
    uint8_t remainderIdx = DAY_TO_LED_MAP[min(NUM_DAYS, (uint8_t)litStarPos - 1)];

    _leds[mainIdx] = starColor.fadeToBlackBy(mainBrightness) + _leds[mainIdx].fadeToBlackBy(remainderBrightness); // * (remainderBrightness / 4);
    if (remainderIdx != mainIdx)
    {
        _leds[remainderIdx] = starColor.fadeToBlackBy(remainderBrightness) + _leds[remainderIdx].fadeToBlackBy(mainBrightness); // * (mainBrightness / 4);
    }

    return state;
}