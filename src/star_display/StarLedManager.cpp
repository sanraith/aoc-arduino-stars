#include "StarLedManager.h"
#include <FastLED.h>
#include <cmath>
#include <set>

const CRGB COLOR_GOLD = CRGB(255, 255, 0); // CHSV(64, 255, 255);
const CHSV COLOR_SILVER = CHSV(64, 0, 200);

StarLedManager::StarLedManager() : _idx(0), _currentState(STAR_LOADING), _progress(0.0f)
{
    //       42
    //     41 39
    //    34 36 38
    //   33 31 29 27
    //   20 22 24 26
    //  19 17 15 13 11
    // 0  2  4  6  8  10

    // Row 1
    _dayToLedMap[0] = 0;
    _dayToLedMap[1] = 2;
    _dayToLedMap[2] = 4;
    _dayToLedMap[3] = 6;
    _dayToLedMap[4] = 8;
    _dayToLedMap[5] = 10;

    // Row 2
    _dayToLedMap[6] = 11;
    _dayToLedMap[7] = 13;
    _dayToLedMap[8] = 15;
    _dayToLedMap[9] = 17;
    _dayToLedMap[10] = 19;

    // Row 3
    _dayToLedMap[11] = 20;
    _dayToLedMap[12] = 22;
    _dayToLedMap[13] = 24;
    _dayToLedMap[14] = 26;

    // Row 4
    _dayToLedMap[15] = 27;
    _dayToLedMap[16] = 29;
    _dayToLedMap[17] = 31;
    _dayToLedMap[18] = 33;

    // Row 5
    _dayToLedMap[19] = 34;
    _dayToLedMap[20] = 36;
    _dayToLedMap[21] = 38;

    // Row 6
    _dayToLedMap[22] = 39;
    _dayToLedMap[23] = 41;

    // Row 7
    _dayToLedMap[24] = 42;

    // Initialize the 2D grid with -1 (indicating no LED)
    _ledGrid.resize(GRID_HEIGHT);
    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        _ledGrid[i].resize(GRID_WIDTH, -1);
    }

    int colorTypes = 4;
    int maxState = 65535;
    int maxDelta = maxState / 25;
    // Randomly assign LEDs to the grid for now
    for (int i = 0; i < NUM_DAYS; i++)
    {
        int x = random(GRID_WIDTH);
        int y = random(GRID_HEIGHT);
        _ledGrid[y][x] = i;

        _ledAnimationState[i] = (random(colorTypes) * (maxState / colorTypes) + random(maxDelta)) % maxState;
    }
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
    }
}

void StarLedManager::updateCompletionState(const uint8_t newState[NUM_DAYS])
{
    for (int i = 0; i < NUM_DAYS; i++)
    {
        _completionState[i] = newState[i];
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
            if (_ledGrid[i][j] != -1)
            {
                double distance = sqrt(pow(x - j, 2) + pow(y - i, 2));
                if (distance <= maxDistance)
                {
                    nearbyDayIds.push_back(_ledGrid[i][j]);
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
    for (int i = 0; i < numLedsToLight; i++)
    {
        _leds[_dayToLedMap[i]] = CRGB(0, 255, 0); // Green color
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
        if (_completionState[i] == 2)
        {
            ledColor = COLOR_GOLD;
        }
        else if (_completionState[i] == 1)
        {
            ledColor = COLOR_SILVER;
        }
        else
        {
            // Color by 45° columns
            int ledId = _dayToLedMap[i];
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

        _leds[_dayToLedMap[i]] = ledColor;
        // _ledAnimationState[i] = (_ledAnimationState[i] + 100) % 65536;
    }
    FastLED.setBrightness(50);
    FastLED.show();
}
