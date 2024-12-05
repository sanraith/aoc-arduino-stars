#pragma once

#include <FastLED.h>
#include "Common.h"
#include "StarAnimation.h"
#include "FallingStarZigZagAnimation.h"

class FallingStarGeometricAnimation : public FallingStarZigZagAnimation // TODO use a better base class
{
public:
    FallingStarGeometricAnimation(CRGB *leds, long animationLengthMs, long elapsedMs, uint8_t day, CRGB starColor);
    virtual StarAnimationState draw(unsigned long applicationTimeMs, unsigned long lastFrameMs) override;
    u_int8_t day;
    CRGB starColor;

private:
    float randomDelta;
    static Point _dayToCoordMap[NUM_DAYS];
    static void initializeDayToCoordMap();
};
