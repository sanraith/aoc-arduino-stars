#pragma once

#include <cmath>
#include <cstdint>
#include <tuple> // for std::tie

#define NUM_DAYS 25

//       42
//     41 39
//    34 36 38
//   33 31 29 27
//   20 22 24 26
//  19 17 15 13 11
// 0  2  4  6  8  10

/** Led ids arranged into a grid that estimates their real-world position. */
const uint8_t LED_GRID_HEIGHT = 7;
const uint8_t LED_GRID_WIDTH = 11;
const int8_t LED_GRID[LED_GRID_HEIGHT][LED_GRID_WIDTH] = {
    {-1, -1, -1, -1, -1, 42, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, 41, -1, 39, -1, -1, -1, -1},
    {-1, -1, -1, 34, -1, 36, -1, 38, -1, -1, -1},
    {-1, -1, 33, -1, 31, -1, 29, -1, 27, -1, -1},
    {-1, -1, 20, -1, 22, -1, 24, -1, 26, -1, -1},
    {-1, 19, -1, 17, -1, 15, -1, 13, -1, 11, -1},
    {0., -1, 2., -1, 4., -1, 6., -1, 8., -1, 10}};

const uint8_t DAYS_IN_ROW[LED_GRID_HEIGHT] = {6, 5, 4, 4, 3, 2, 1};
const uint8_t DAY_TO_ROW_MAP[NUM_DAYS] = {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 5, 5, 6};

/** Maps day of month (0-indexed, 0..24) to the corresponding led id. */
const uint8_t DAY_TO_LED_MAP[NUM_DAYS] = {
    0, 2, 4, 6, 8, 10,  // Row 1 (bottom)
    11, 13, 15, 17, 19, // Row 2
    20, 22, 24, 26,     // Row 3
    27, 29, 31, 33,     // Row 4
    34, 36, 38,         // Row 5
    39, 41,             // Row 6
    42                  // Row 7 (top)
};

struct Point
{
    float x;
    float y;

    // Constructor
    Point() : x(0), y(0) {}
    Point(float x, float y) : x(x), y(y) {}

    // Equality operator
    bool operator==(const Point &other) const
    {
        return std::tie(x, y) == std::tie(other.x, other.y);
    }

    // Inequality operator
    bool operator!=(const Point &other) const
    {
        return !(*this == other);
    }

    // Less than operator (for sorting)
    bool operator<(const Point &other) const
    {
        return std::tie(x, y) < std::tie(other.x, other.y);
    }

    Point operator+(Point other) const
    {
        return Point(x + other.x, y + other.y);
    }

    // Multiplication by scalar operator
    Point operator*(float scalar) const
    {
        return Point(x * scalar, y * scalar);
    }

    // Distance method
    float distance(const Point &other) const
    {
        float dx = x - other.x;
        float dy = y - other.y;
        return sqrt(dx * dx + dy * dy);
    }
};

float easeInCubic(float x);
float easeOutCubic(float x);
double easeOutBounce(double x);
double easeInOutExpo(double x);
double easeInOutBounce(double x);
double easeInOutQuart(double x);