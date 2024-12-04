#include "StarLedManager.h"
#include "FallingStarAnimation.h"
#include <FastLED.h>
#include <cmath>
#include <set>
#include <memory>

const CRGB COLOR_GOLD = CRGB(255, 255, 0); // CHSV(64, 255, 255);
const CHSV COLOR_SILVER = CHSV(64, 0, 200);

StarLedManager::StarLedManager() : _idx(0), _currentState(STAR_LOADING), _progress(0.0f)
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

void StarLedManager::updateCompletionState(const uint8_t newState[NUM_DAYS])
{
    long animationLengthPerStarDistanceMs = 100;
    long animationOverlapMs = 500;
    long delayBetweenAnimations = 0;
    for (int dayIdx = 0; dayIdx < NUM_DAYS; dayIdx++)
    {
        uint8_t starState = newState[dayIdx];
        if (_knownCompletionState[dayIdx] < starState)
        {
            // There is a new star where it was not before, animate it.
            auto animation = std::make_unique<FallingStarAnimation>(
                _leds, animationLengthPerStarDistanceMs, -1 * delayBetweenAnimations, dayIdx, starState == 1 ? COLOR_SILVER : COLOR_GOLD);
            delayBetweenAnimations += (*animation).animationLengthMs - animationOverlapMs;
            _queuedAnimations.push_back(std::move(animation));
        }
        else
        {
            // There is no star when there was before, just update it's state.
            _displayedCompletionState[dayIdx] = starState;
        }
        _knownCompletionState[dayIdx] = starState;
    }
}

std::vector<int> StarLedManager::getNearbyDayIds(int x, int y)
{
    std::vector<int> nearbyDayIds;
    int maxDistance = 2;

    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            if (LED_GRID[i][j] != -1)
            {
                double distance = sqrt(pow(x - j, 2) + pow(y - i, 2));
                if (distance <= maxDistance)
                {
                    nearbyDayIds.push_back(LED_GRID[i][j]);
                }
            }
        }
    }

    return nearbyDayIds;
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

CHSV getColorForAnimationState(uint16_t stateIn)
{
    uint8_t state = stateIn >> 8;
    CHSV color = CHSV(state, 255, 255);
    return color;
}

void StarLedManager::handleIdleState()
{
    FastLED.clear();
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
            // Color by 45° columns
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
            int colorIdx = columnIdx % 3;
            switch (colorIdx)
            {
            case 0:
                ledColor = CRGB::OrangeRed;
                break;
            case 2:
                ledColor = CRGB::RoyalBlue;
                break;
            case 1:
                ledColor = CRGB::ForestGreen;
                break;
            }
            if (i != 24)
            {
                ledColor.fadeToBlackBy(160);
            }

            // Color equally by dark colors
            // int colorIdx = _dayToLedMap[i] % 3;
            // switch (colorIdx)
            // {
            // case 0:
            //     ledColor = CRGB::DarkOrange;
            //     break;
            // case 1:
            //     ledColor = CRGB::BlueViolet;
            //     break;
            // case 2:
            //     ledColor = CRGB::DarkCyan;
            //     break;
            // }
            // ledColor.fadeToBlackBy(128);

            // Color each row with red/blue
            // int colorIdx = _dayToLedMap[i] % 2;
            // switch (colorIdx)
            // {
            // case 0:
            //     ledColor = CRGB::DarkRed;
            //     break;
            // case 1:
            //     ledColor = CRGB::DarkBlue;
            //     break;
            //     // case 2:
            //     //     ledColor = CRGB::DarkCyan;
            //     //     break;
            // }
            // ledColor.fadeToBlackBy(64);

            // ledColor = getColor(_ledAnimationState[i]);
        }

        _leds[DAY_TO_LED_MAP[i]] = ledColor;
        // _ledAnimationState[i] = (_ledAnimationState[i] + 100) % 65536;
    }
    // FastLED.setBrightness(50);
    // FastLED.show();
}

void StarLedManager::handleAnimations(unsigned long totalTime, unsigned long frameTime)
{
    for (auto it = _queuedAnimations.begin(); it != _queuedAnimations.end();)
    {
        StarAnimationState state = (*it)->draw(totalTime, frameTime);
        if (state == StarAnimationState::ANIMATION_IDLE)
        {
            uint8_t animatedDay = (*it)->day;
            _displayedCompletionState[animatedDay] = _knownCompletionState[animatedDay];
            it = _queuedAnimations.erase(it);
        }
        else
        {
            ++it;
        }
    }
}