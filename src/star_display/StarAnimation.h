#pragma once

#include <FastLED.h>
#include "Common.h"

enum StarAnimationState
{
    ANIMATION_IN_PROGRESS,
    ANIMATION_IDLE
};

class StarAnimation
{
public:
    StarAnimation(CRGB *leds, long animationLengthMs, long elapsedMs);
    virtual StarAnimationState draw(unsigned long applicationTimeMs, unsigned long lastFrameMs);
    virtual ~StarAnimation() {}
    long const animationLengthMs;

protected:
    CRGB *_leds;
    long _elapsedMs;
};
