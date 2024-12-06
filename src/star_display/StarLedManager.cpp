#include "StarLedManager.h"
#include "FallingStarZigZagAnimation.h"
#include "FallingStarGeometricAnimation.h"
#include <FastLED.h>
#include <cmath>
#include <set>
#include <memory>
#include "BackgroundAnimation.h"

const CRGB COLOR_GOLD = CRGB(255, 255, 0); // CHSV(64, 255, 255);
const CHSV COLOR_SILVER = CHSV(64, 0, 200);

StarLedManager::StarLedManager() : _currentState(STAR_LOADING), _progress(0.0f)
{
}

void StarLedManager::setup()
{
    FastLED.addLeds<WS2812B, LED_STRIP_DATA_PIN, GRB>(_leds, NUM_LEDS);
    // FastLED.setMaxPowerInVoltsAndMilliamps(5, 1000);
    FastLED.clear(true);
    FastLED.show();
}

void StarLedManager::loop(unsigned long totalTime, unsigned long frameTime)
{
    switch (_currentState)
    {
    case STAR_LOADING:
        handleLoadingState();
        break;
    case STAR_IDLE:
        FastLED.clear();
        for (auto it = _continuousAnimations.begin(); it != _continuousAnimations.end(); it++)
        {
            // Serial.println("starting star bg ");
            (*it)->draw(totalTime, frameTime);
        }
        handleIdleState();
        handleAnimations(totalTime, frameTime);
        FastLED.setBrightness(50);
        FastLED.show();
        break;
    }
}

void StarLedManager::updateProgress(float percentage)
{
    if (_currentState != STAR_LOADING)
    {
        return;
    }
    _progress = percentage;

    Serial.println(_progress);
    handleLoadingState();
    if (_progress >= 1)
    {
        _currentState = STAR_IDLE;
        delay(250);
    }
}

void makeAnimation(
    int animationType, CRGB *_leds, long animationLengthPerStarDistanceMs, long *delayBetweenAnimations,
    uint8_t dayIdx, long animationOverlapMs, std::vector<std::unique_ptr<FallingStarZigZagAnimation>> *_queuedAnimations, CRGB color)
{
    std::unique_ptr<FallingStarZigZagAnimation> animation;
    if (animationType == 0)
    {
        animationOverlapMs = 500;
        animation = std::make_unique<FallingStarZigZagAnimation>(
            _leds, animationLengthPerStarDistanceMs * (NUM_DAYS - dayIdx), -1 * *delayBetweenAnimations, dayIdx, color);
    }
    else
    {
        animationOverlapMs = 500 + random16(500);
        animation = std::make_unique<FallingStarGeometricAnimation>(
            _leds, animationLengthPerStarDistanceMs * 15, -1 * *delayBetweenAnimations, dayIdx, color);
        // TODO make length proportional to distance
    }
    (*delayBetweenAnimations) += (*animation).animationLengthMs - animationOverlapMs;
    _queuedAnimations->push_back(std::move(animation));
}

void StarLedManager::updateCompletionState(const uint8_t newState[NUM_DAYS])
{
    long animationLengthPerStarDistanceMs = 100;
    long delayBetweenAnimations = 0;
    long animationOverlapMs = 500;

    int knownStarsCount = 0;
    for (int i = 0; i < NUM_DAYS; i++)
    {
        knownStarsCount += _knownCompletionState[i];
    }

    int animationType = knownStarsCount == 0 ? 1 : 0;
    boolean areSilversSeparate = animationType == 1;

    // TODO group initial animations by line, by color
    if (areSilversSeparate)
    {
        // Queue silver star animations first if requested
        for (int dayIdx = 0; dayIdx < NUM_DAYS; dayIdx++)
        {
            uint8_t starState = newState[dayIdx];
            if (_knownCompletionState[dayIdx] < starState)
            {
                // There is a new star where it was not before, animate it.
                makeAnimation(animationType, _leds, animationLengthPerStarDistanceMs, &delayBetweenAnimations, dayIdx, animationOverlapMs, &_queuedAnimations, COLOR_SILVER);
            }
            else
            {
                // There is no star when there was before, just update it's state.
                _displayedCompletionState[dayIdx] = starState;
            }
            _knownCompletionState[dayIdx] = min(1, starState);
        }
    }

    for (int dayIdx = 0; dayIdx < NUM_DAYS; dayIdx++)
    {
        uint8_t starState = newState[dayIdx];
        if (_knownCompletionState[dayIdx] < starState)
        {
            // There is a new star where it was not before, animate it.
            makeAnimation(animationType, _leds, animationLengthPerStarDistanceMs, &delayBetweenAnimations, dayIdx, animationOverlapMs, &_queuedAnimations, starState == 2 ? COLOR_GOLD : COLOR_SILVER);
        }
        else
        {
            // There is no star when there was before, just update it's state.
            _displayedCompletionState[dayIdx] = starState;
        }
        _knownCompletionState[dayIdx] = starState;
    }

    // Start the background animation after the initial stars animations are completed
    if (_continuousAnimations.empty())
    {
        _continuousAnimations.push_back(std::make_unique<BackgroundAnimation>(_leds, 7500, -delayBetweenAnimations - 1000));
    }
}

void StarLedManager::handleLoadingState()
{
    FastLED.clear();
    int numLedsToLight = static_cast<int>(_progress * NUM_DAYS);
    for (int i = 0; i < NUM_DAYS; i++)
    {
        _leds[DAY_TO_LED_MAP[i]] = i < numLedsToLight ? CRGB(0, 255, 0) : CRGB(128, 0, 0);
    }
    FastLED.setBrightness(25); // Set brightness to faint
    FastLED.show();
}

void StarLedManager::handleIdleState()
{
    for (int i = 0; i < NUM_DAYS; i++)
    {
        CRGB ledColor;
        if (_displayedCompletionState[i] == 2)
        {
            ledColor = COLOR_GOLD;
        }
        else if (_displayedCompletionState[i] == 1)
        {
            ledColor = COLOR_SILVER;
        }
        else
        {
            continue;
        }
        _leds[DAY_TO_LED_MAP[i]] = ledColor;
    }
}

void StarLedManager::handleAnimations(unsigned long totalTime, unsigned long frameTime)
{
    for (auto it = _queuedAnimations.begin(); it != _queuedAnimations.end();)
    {
        StarAnimationState state = (*it)->draw(totalTime, frameTime);
        if (state == StarAnimationState::ANIMATION_IDLE)
        {
            uint8_t animatedDay = (*it)->day;
            _displayedCompletionState[animatedDay] = (*it)->starColor == COLOR_GOLD ? 2 : 1;
            it = _queuedAnimations.erase(it);
        }
        else
        {
            ++it;
        }
    }
}