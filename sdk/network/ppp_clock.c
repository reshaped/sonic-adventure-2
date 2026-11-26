#include "ppp_system.h"

static unsigned long clock_seed = 0x12345678;
static unsigned short random_seed = 0x1234;

long PP_Clock(void)
{
    /* @hack spelling: the time held in now, where a return of the call would
     * do */
    long now = PP_GetTimeLow();
    return now;
}
long PP_secclock(void)
{
    return PP_GetTimeLow();
}
void _PP_srandom(void)
{
    clock_seed = PP_GetTimeLow();
    clock_seed =
        (clock_seed << 8) + (clock_seed << 16) + (clock_seed >> 4) + clock_seed;
    random_seed = (unsigned short)clock_seed;
    random_seed += 0x350b;
}
short _PP_random(void)
{
    random_seed = (unsigned short)((random_seed << 4) + (random_seed << 8) +
                                   (random_seed >> 5) + random_seed + 0xd499);
    return (short)random_seed;
}
int PP_strnicmp(const unsigned char *left, const unsigned char *right,
                short count)
{
    unsigned char a;
    unsigned char b;
    while (count && *left && *right) {
        a = *left;
        b = *right;
        if (a >= 'a' && a <= 'z') {
            a -= 'a' - 'A';
        }
        if (b >= 'a' && b <= 'z') {
            b -= 'a' - 'A';
        }
        if (a != b) {
            return a - b;
        }
        left++;
        right++;
        count--;
    }
    if (count) {
        return 1;
    }
    return 0;
}
void _PP_delay(unsigned short milliseconds)
{
    PP_OS_WaitMs(milliseconds);
}
