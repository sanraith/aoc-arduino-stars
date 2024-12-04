#pragma once

#include <FastLED.h>
#include <map>
#include <vector>
#include <set>

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
    void loop(unsigned long totalTime, unsigned long frameTime);
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

    uint16_t _ledAnimationState[NUM_DAYS] = {0};

    void handleLoadingState();
    void handleIdleState();

    std::vector<std::set<int>> _columns = {
        {33, 34, 41, 42},
        {0, 19, 20, 31, 36, 39},
        {2, 17, 22, 29, 38},
        {4, 15, 24, 27},
        {6, 13, 26},
        {8, 11},
        {10}};
};