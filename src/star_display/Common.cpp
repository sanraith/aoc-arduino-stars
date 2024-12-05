#include "Common.h"

double easeInOutExpo(double x)
{
    return x == 0 ? 0 : x == 1 ? 1
                    : x < 0.5  ? pow(2, 20 * x - 10) / 2
                               : (2 - pow(2, -20 * x + 10)) / 2;
}

double easeOutBounce(double x)
{
    const double n1 = 7.5625;
    const double d1 = 2.75;

    if (x < 1 / d1)
    {
        return n1 * x * x;
    }
    else if (x < 2 / d1)
    {
        return n1 * (x -= 1.5 / d1) * x + 0.75;
    }
    else if (x < 2.5 / d1)
    {
        return n1 * (x -= 2.25 / d1) * x + 0.9375;
    }
    else
    {
        return n1 * (x -= 2.625 / d1) * x + 0.984375;
    }
}

double easeInOutBounce(double x)
{
    return x < 0.5
               ? (1 - easeOutBounce(1 - 2 * x)) / 2
               : (1 + easeOutBounce(2 * x - 1)) / 2;
}

double easeInOutQuart(double x)
{
    return x < 0.5 ? 8 * x * x * x * x : 1 - pow(-2 * x + 2, 4) / 2;
}