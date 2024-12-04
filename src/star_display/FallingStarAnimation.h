#pragma once

#include <FastLED.h>
#include "Common.h"
#include "StarAnimation.h"

class FallingStarAnimation : public StarAnimation
{
public:
    FallingStarAnimation(CRGB *leds, long animationLengthMs, long elapsedMs, uint8_t day, CRGB starColor);
    virtual StarAnimationState draw(unsigned long applicationTimeMs, unsigned long lastFrameMs) override;
    u_int8_t day;
    CRGB starColor;
};
