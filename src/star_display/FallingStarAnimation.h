#pragma once

#include <FastLED.h>
#include "Common.h"
#include "StarAnimation.h"

class FallingStarAnimation : public StarAnimation
{
public:
    FallingStarAnimation(CRGB *leds, unsigned long animationLengthMs, uint8_t day);
    virtual StarAnimationState draw(unsigned long applicationTimeMs, unsigned long lastFrameMs) override;
    u_int8_t day;
};
