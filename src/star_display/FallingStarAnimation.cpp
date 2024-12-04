#include "StarAnimation.h"
#include "FallingStarAnimation.h"

FallingStarAnimation::FallingStarAnimation(CRGB *leds, long animationLengthMs, long elapsedMs, uint8_t day, CRGB starColor)
    : StarAnimation(leds, animationLengthMs * (NUM_DAYS - day), elapsedMs), day(day), starColor(starColor)
{
}

double easeOutBounce(double x)
{
    const double n1 = 7.5625;
    const double d1 = 2.75;

    if (x < 1 / d1)
    {
        return n1 * x * x;
    }
    else if (x < 2 / d1)
    {
        return n1 * (x -= 1.5 / d1) * x + 0.75;
    }
    else if (x < 2.5 / d1)
    {
        return n1 * (x -= 2.25 / d1) * x + 0.9375;
    }
    else
    {
        return n1 * (x -= 2.625 / d1) * x + 0.984375;
    }
}

StarAnimationState FallingStarAnimation::draw(unsigned long applicationTimeMs, unsigned long lastFrameMs)
{
    StarAnimationState state = StarAnimation::draw(applicationTimeMs, lastFrameMs);
    if (_elapsedMs < 0)
    {
        return state;
    }

    double span = NUM_DAYS - day;
    double progress = min(1.0, easeOutBounce(_elapsedMs * 1.0 / animationLengthMs));
    double litStarPos = NUM_DAYS * 1.0 - progress * span;

    uint8_t trailBrightness = (litStarPos - (int)litStarPos) * 255;
    uint8_t frontBrightness = 255 - trailBrightness;
    uint8_t frontIdx = DAY_TO_LED_MAP[(uint8_t)litStarPos];
    uint8_t trailIdx = DAY_TO_LED_MAP[(uint8_t)std::ceil(litStarPos)];

    CRGB frontColor = starColor;
    _leds[frontIdx] = frontColor.fadeToBlackBy(trailBrightness) + _leds[frontIdx].fadeToBlackBy(frontBrightness);
    if (frontIdx != trailIdx)
    {
        CRGB trailColor = starColor;
        _leds[trailIdx] = trailColor.fadeToBlackBy(frontBrightness) + _leds[trailIdx].fadeToBlackBy(trailBrightness);
    }

    return state;
}
