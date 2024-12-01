#pragma once

#include <FastLED.h>
#include <map>
#include <vector>

#define NUM_LEDS 43
#define LED_STRIP_DATA_PIN 4
#define GRID_WIDTH 11
#define GRID_HEIGHT 8
#define NUM_DAYS 25

enum StarDisplayState
{
    STAR_LOADING,
    STAR_IDLE
};

class StarLedManager
{
public:
    StarLedManager();
    void setup();
    void loop();
    std::vector<int> getNearbyDayIds(int x, int y);
    void updateProgress(float percentage);
    void updateCompletionState(const uint8_t newState[NUM_DAYS]);

private:
    CRGB _leds[NUM_LEDS];
    int _idx;
    int _dayToLedMap[NUM_DAYS];
    std::vector<std::vector<int>> _ledGrid; // 2D grid to store LED positions (rows)
    StarDisplayState _currentState;         // State variable to keep track of the current display type
    float _progress;                        // Field to store the progress value
    uint8_t _completionState[NUM_DAYS] = {0};

    void handleLoadingState();
    void handleIdleState();
};