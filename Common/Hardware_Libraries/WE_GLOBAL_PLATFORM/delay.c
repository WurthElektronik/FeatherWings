#include <ConfigPlatform.h>

// This is needed to avoid naming conflicts
static void (*delayFunc)(uint32_t) = delay;

/* Delay in milliseconds */
void WE_Delay(uint32_t delay)
{
    if (delay > 0)
    {
        delayFunc(delay);
    }
}