#include <errno.h>
#include <float_bits.h>
#include <mathf.h>

float atan2f(float y, float x)
{
    float result;

    result = _in_atan2s(y, x);
    /* @hack type: result's bits read through its address, where result as a
     * union with an unsigned long would do */
    if ((*(unsigned long *)&result & FLOAT_EXPONENT) == FLOAT_EXPONENT) {
        if (*(unsigned long *)&result & FLOAT_FRACTION) {
            errno = EDOM;
        } else {
            errno = ERANGE;
        }
    }
    return result;
}
