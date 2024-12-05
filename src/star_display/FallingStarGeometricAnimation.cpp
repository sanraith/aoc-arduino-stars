#include <cmath>
#include "Common.h"
#include "StarAnimation.h"
#include "FallingStarGeometricAnimation.h"

int findIndex(const uint8_t arr[], int size, uint8_t value)
{
    for (int i = 0; i < size; ++i)
    {
        if (arr[i] == value)
        {
            return i;
        }
    }
    return -1; // Return -1 if the value is not found
}

// Define the static member variable
Point FallingStarGeometricAnimation::_dayToCoordMap[NUM_DAYS];

// Static function to initialize the coordinate map
void FallingStarGeometricAnimation::initializeDayToCoordMap()
{
    for (int y = 0; y < LED_GRID_HEIGHT; y++)
    {
        for (int x = 0; x < LED_GRID_WIDTH; x++)
        {
            auto ledId = LED_GRID[y][x];
            if (ledId >= 0)
            {
                auto day = findIndex(DAY_TO_LED_MAP, NUM_DAYS, ledId);
                _dayToCoordMap[day] = Point(x, y);
            }
        }
    }
}

FallingStarGeometricAnimation::FallingStarGeometricAnimation(CRGB *leds, long animationLengthMs, long elapsedMs, uint8_t day, CRGB starColor)
    : FallingStarZigZagAnimation(leds, animationLengthMs, elapsedMs, day, starColor), day(day), starColor(starColor)
{
    static bool initialized = (initializeDayToCoordMap(), true);
    randomDelta = random16(10000) / 1000.0;
}

void highlightLedsAround(Point pos, float maxDistance, CRGB *_leds, CRGB color)
{
    int startY = max(0, (int)(pos.y - maxDistance));
    int endY = min(LED_GRID_HEIGHT - 1, ceil(pos.y + maxDistance));
    int startX = max(0, (int)(pos.x - maxDistance));
    int endX = min(LED_GRID_WIDTH - 1, ceil(pos.x + maxDistance));
    for (int y = startY; y <= endY; y++)
    {
        for (int x = startX; x <= endX; x++)
        {
            auto distance = pos.distance(Point(x, y));
            uint8_t darkness = easeOutCubic(min(1.0, distance / maxDistance)) * 255;
            uint8_t invDarkness = 255 - darkness;
            uint8_t ledIdx = LED_GRID[y][x];
            CRGB ledColor = color;
            _leds[ledIdx] = ledColor.fadeToBlackBy(darkness) + _leds[ledIdx].fadeToBlackBy(invDarkness);
        }
    }
}

StarAnimationState FallingStarGeometricAnimation::draw(unsigned long applicationTimeMs, unsigned long lastFrameMs)
{
    StarAnimationState state = StarAnimation::draw(applicationTimeMs, lastFrameMs);
    if (_elapsedMs < 0)
    {
        return state;
    }

    float progress = min(1.0, _elapsedMs * 1.0 / animationLengthMs);
    // float posProgress = easeOutCubic(progress);
    float posProgress = progress;
    float highlightProgress = 1; // 1 - easeInCubic(easeInCubic(progress));

    float startDistance = 8;
    Point target = _dayToCoordMap[day];
    Point direction = Point(sin(posProgress * 5 + randomDelta * 2) * 0.65 + ((target.x - LED_GRID_WIDTH / 2) / LED_GRID_WIDTH * 2), 1);
    Point pos = target + direction * -1 * startDistance * (1.0 - posProgress);

    float highlightDistance = 1 + 1.5 * highlightProgress;

    highlightLedsAround(pos, highlightDistance, _leds, starColor);

    return state;
}
