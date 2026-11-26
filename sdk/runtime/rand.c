#include <rnext.h>
#include <stdlib.h>

/* the C standard's sample generator. each call steps _rnext by MULTIPLIER
 * and INCREMENT, and returns the bits from RESULT_SHIFT up that RAND_MAX
 * keeps */
#define MULTIPLIER 1103515245
#define INCREMENT 12345
#define RESULT_SHIFT 16

int rand(void)
{
    _rnext = (_rnext * MULTIPLIER) + INCREMENT;
    return (_rnext >> RESULT_SHIFT) & RAND_MAX;
}
