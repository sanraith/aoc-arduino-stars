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
    StarAnimation(CRGB *leds, unsigned long animationLengthMs);
    virtual StarAnimationState draw(unsigned long applicationTimeMs, unsigned long lastFrameMs);
    virtual ~StarAnimation() {}

protected:
    CRGB *_leds;
    unsigned long const _animationLengthMs;
    unsigned long _elapsedMs = 0;
};
