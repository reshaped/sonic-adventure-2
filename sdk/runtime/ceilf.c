#include <errno.h>
#include <float_bits.h>
#include <mathf.h>

#ifdef DREAMCAST
#ifdef NONMATCHING
/* each C call runs mathf.h's zam_ceilf inline. this definition serves the
 * function's address and the calls from assembly */
#undef ceilf
#endif
#endif
float ceilf(float x)
{
    float result;

    result = _in_ceils(x);
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
