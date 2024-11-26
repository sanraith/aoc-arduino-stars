#include "StarLedManager.h"
#include <FastLED.h>
#include <cmath>

StarLedManager::StarLedManager() : _idx(0), _isAnimating(false), _currentState(STAR_LOADING), _progress(0.0f)
{
    for (int i = 0; i < NUM_DAYS; i++)
    {
        _dayToLedMap[i] = i * 2;
    }

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
    FastLED.addLeds<WS2813, LED_STRIP_DATA_PIN, GRB>(_leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 1000);
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
    delay(100);
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
    // FastLED.clear(true);
    FastLED.show();
}
