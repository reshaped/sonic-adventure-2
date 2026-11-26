#include <errno.h>
#include <float_bits.h>
#include <mathf.h>

float modff(float value, float *iptr)
{
    float fraction;
    /* @hack spelling: _in_modfs's integral part stored in integral and then
     * copied to *iptr, where passing iptr to _in_modfs would do */
    float integral;

    fraction = _in_modfs(value, &integral);
    *iptr = integral;
    /* @hack type: fraction's bits read through its address, where fraction as a
     * union with an unsigned long would do */
    if ((*(unsigned long *)&fraction & FLOAT_EXPONENT) == FLOAT_EXPONENT) {
        if (*(unsigned long *)&fraction & FLOAT_FRACTION) {
            errno = EDOM;
        } else {
            errno = ERANGE;
        }
    }
    return fraction;
}
