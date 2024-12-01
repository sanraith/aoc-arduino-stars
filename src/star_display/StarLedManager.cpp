#include "StarLedManager.h"
#include <FastLED.h>
#include <cmath>

StarLedManager::StarLedManager() : _idx(0), _currentState(STAR_LOADING), _progress(0.0f)
{
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

    // Randomly assign LEDs to the grid for now
    for (int i = 0; i < NUM_DAYS; i++)
    {
        int x = random(GRID_WIDTH);
        int y = random(GRID_HEIGHT);
        _ledGrid[y][x] = i;
    }
}

void StarLedManager::setup()
{
    FastLED.addLeds<WS2812B, LED_STRIP_DATA_PIN, GRB>(_leds, NUM_LEDS);
    // FastLED.setMaxPowerInVoltsAndMilliamps(5, 1000);
    FastLED.clear(true);
    FastLED.show();
}

void StarLedManager::loop()
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
    delay(1000);
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

void StarLedManager::handleIdleState()
{
    // Implement the behavior for the idle state
    // For now, just clear the LEDs
    FastLED.clear(true);
    for (int i = 0; i < NUM_DAYS; i++)
    {
        CRGB ledColor;
        if (_completionState[i] == 2)
        {
            ledColor = CRGB(255, 255, 0);
        }
        else if (_completionState[i] == 1)
        {
            ledColor = CRGB(100, 100, 100);
        }
        else
        {
            ledColor = CRGB(0, 0, 100);
        }
        _leds[_dayToLedMap[i]] = ledColor;
    }
    FastLED.setBrightness(25);
    FastLED.show();
}
