#include <errno.h>
#include <float_bits.h>
#include <math.h>
#include <mathf.h>

double cos(double x)
{
    double result;

    result = _in_coss(x);
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
