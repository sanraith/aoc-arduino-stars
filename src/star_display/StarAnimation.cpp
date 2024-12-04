#include "StarAnimation.h"

StarAnimation::StarAnimation(CRGB *leds, unsigned long animationLengthMs) : _leds(leds), _animationLengthMs(animationLengthMs)
{
}

StarAnimationState StarAnimation::draw(unsigned long applicationTimeMs, unsigned long lastFrameMs)
{
    if (_elapsedMs < _animationLengthMs)
    {
        _elapsedMs += lastFrameMs;
    }
    return _elapsedMs >= _animationLengthMs ? StarAnimationState::ANIMATION_IDLE : StarAnimationState::ANIMATION_IN_PROGRESS;
}