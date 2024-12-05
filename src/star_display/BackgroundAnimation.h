#pragma once

#include <FastLED.h>
#include "Common.h"
#include "StarAnimation.h"

class BackgroundAnimation : public StarAnimation
{
public:
    BackgroundAnimation(CRGB *leds, uint32_t animationLengthMs, long elapsedMs);
    virtual StarAnimationState draw(unsigned long applicationTimeMs, unsigned long lastFrameMs) override;

private:
    double _ledStates[NUM_DAYS];
};
