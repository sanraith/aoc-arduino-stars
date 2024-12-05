#include "StarAnimation.h"
#include "FallingStarZigZagAnimation.h"

FallingStarZigZagAnimation::FallingStarZigZagAnimation(CRGB *leds, long animationLengthMs, long elapsedMs, uint8_t day, CRGB starColor)
    : StarAnimation(leds, animationLengthMs, elapsedMs), day(day), starColor(starColor)
{
}

StarAnimationState FallingStarZigZagAnimation::draw(unsigned long applicationTimeMs, unsigned long lastFrameMs)
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
