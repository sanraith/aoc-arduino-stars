#include <vector>
#include <set>
#include "BackgroundAnimation.h"

const int COLOR_COUNT = 3;

BackgroundAnimation::BackgroundAnimation(CRGB *leds, uint32_t animationLengthMs, long elapsedMs) : StarAnimation(leds, animationLengthMs * COLOR_COUNT, elapsedMs)
{
    std::vector<std::set<int>> _columns = {
        {33, 34, 41, 42},
        {0, 19, 20, 31, 36, 39},
        {2, 17, 22, 29, 38},
        {4, 15, 24, 27},
        {6, 13, 26},
        {8, 11},
        {10}};

    for (int i = 0; i < NUM_DAYS; i++)
    {
        int ledId = DAY_TO_LED_MAP[i];
        int columnIdx;
        for (columnIdx = 0; columnIdx < _columns.size(); columnIdx++)
        {
            std::set<int> mySet = _columns.at(columnIdx);
            if (mySet.find(ledId) != mySet.end())
            {
                break;
            }
        }
        int colorIdx = COLOR_COUNT - (columnIdx % COLOR_COUNT) - 1;

        int fluctPct = 100;
        double fluctuation = (random16(fluctPct * 100) - fluctPct / 2 * 100) / 10000.0;
        _ledStates[i] = colorIdx + fluctuation + 1;
        _ledStates[i] -= (int)(_ledStates[i] / COLOR_COUNT) * COLOR_COUNT;
    }
}

StarAnimationState BackgroundAnimation::draw(unsigned long applicationTimeMs, unsigned long lastFrameMs)
{
    _elapsedMs += lastFrameMs;
    if (_elapsedMs < 0)
    {
        return StarAnimationState::ANIMATION_IN_PROGRESS;
    }

    const uint32_t fadeInTimeMs = 3000;
    uint8_t initialDarkness = 255 - min(1.0, max(0, _elapsedMs) * 1.0 / fadeInTimeMs) * 255;

    CRGB ledColors[COLOR_COUNT] = {CRGB::OrangeRed, CRGB::RoyalBlue, CRGB::ForestGreen};
    auto animPhase = 1.0 * COLOR_COUNT * (_elapsedMs % animationLengthMs) / animationLengthMs;
    for (int dayIdx = 0; dayIdx < NUM_DAYS; dayIdx++)
    {
        auto ledPhase = _ledStates[dayIdx] + animPhase;          // Step animation
        ledPhase -= (int)(ledPhase / COLOR_COUNT) * COLOR_COUNT; // Normalize to 0..COLOR_COUNT

        // double blacknessRatio = easeInOutBounce(ledPhase - (int)ledPhase);
        // double blacknessRatio = easeInOutExpo(ledPhase - (int)ledPhase);
        // double blacknessRatio = easeInOutExpo(easeInOutExpo(ledPhase - (int)ledPhase));
        double blacknessRatio = easeInOutQuart(easeInOutQuart(ledPhase - (int)ledPhase));

        uint8_t blackness = blacknessRatio * 255;
        uint8_t invBlackness = 255 - blackness;

        uint8_t colorIdx = (uint8_t)ledPhase;
        CRGB color1 = ledColors[colorIdx];
        CRGB color2 = ledColors[(colorIdx + 1) % COLOR_COUNT];
        uint8_t ledIdx = DAY_TO_LED_MAP[dayIdx];

        // Create a smooth transition by interpolating between 2 colors
        _leds[ledIdx] = color1.fadeToBlackBy(blackness) + color2.fadeToBlackBy(invBlackness);
        if (dayIdx != 24)
        {
            _leds[ledIdx].fadeToBlackBy(64); // the top star needs more power to get the same relative brightness
        }
        _leds[ledIdx].fadeToBlackBy(initialDarkness); // apply fade-in at the start of the animation
    }

    return StarAnimationState::ANIMATION_IN_PROGRESS;
}
