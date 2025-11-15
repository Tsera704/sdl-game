#include "defs.h"
#include <math.h> // <-- ADD THIS INCLUDE
#include <stdlib.h> // <-- and this for abs()

int collision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
    return (MAX(x1, x2) < MIN(x1 + w1, x2 + w2)) && (MAX(y1, y2) < MIN(y1 + h1, y2 + h2));
}

void calcSlope(float x1, float y1, float x2, float y2, float *dx, float *dy)
{
    // --- FIXED ---
    // Use fabs() for floats and make sure steps is an int
    int steps = (int)MAX(fabs(x1 - x2), fabs(y1 - y2));

    if (steps == 0)
    {
        *dy = *dx = 0;
        return;
    }

    *dx = (x1 - x2);
    *dx /= steps;

    *dy = (y1 - y2);
    *dy /= steps;
}