#pragma once

#include <cmath>
#include <cstdint>

#define NUM_DAYS 25

//       42
//
//     41 39
//
//    34 36 38
//   33 31 29 27
//
//   20 22 24 26
//  19 17 15 13 11
// 0  2  4  6  8  10

/** Led ids arranged into a grid that estimates their real-world position. */
const int8_t LED_GRID[10][11] = {
    {-1, -1, -1, -1, -1, 42, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, 41, -1, 39, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, 34, -1, 36, -1, 38, -1, -1, -1},
    {-1, -1, 33, -1, 31, -1, 29, -1, 27, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, 20, -1, 22, -1, 24, -1, 26, -1, -1},
    {-1, 19, -1, 17, -1, 15, -1, 13, -1, 11, -1},
    {0., -1, 2., -1, 4., -1, 6., -1, 8., -1, 10}};

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

double easeInOutExpo(double x);
double easeOutBounce(double x);
double easeInOutBounce(double x);
double easeInOutQuart(double x);