#include "StarAnimation.h"

StarAnimation::StarAnimation(CRGB *leds, long animationLengthMs, long elapsedMs)
    : _leds(leds), animationLengthMs(animationLengthMs), _elapsedMs(elapsedMs)
{
}

StarAnimationState StarAnimation::draw(unsigned long applicationTimeMs, unsigned long lastFrameMs)
{
    if (_elapsedMs < animationLengthMs)
    {
        _elapsedMs += lastFrameMs;
    }
    return _elapsedMs >= animationLengthMs ? StarAnimationState::ANIMATION_IDLE : StarAnimationState::ANIMATION_IN_PROGRESS;
}