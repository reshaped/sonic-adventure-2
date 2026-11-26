#include <errno.h>
#include <float_bits.h>
#include <math.h>

/* the exponent field, shifted down, of a float from 0.5 up to 1 */
#define HALF_FIELD (FLOAT_BIAS - 1)

double frexp(double value, int *exp)
{
    union {
        double value;
        unsigned long bits;
    } word;
    long exponent;

    word.value = value;
    exponent = word.bits & FLOAT_EXPONENT;
    if (value == 0.0) {
        *exp = 0;
        return value;
    }
    if (exponent == FLOAT_EXPONENT) {
        if ((word.bits & FLOAT_FRACTION) == 0) {
            errno = ERANGE;
        } else {
            errno = EDOM;
        }
        return value;
    }
    *exp = (exponent >> FLOAT_FRACTION_BITS) - HALF_FIELD;
    word.bits = (word.bits & FLOAT_SIGN) | (word.bits & FLOAT_FRACTION) |
                (HALF_FIELD << FLOAT_FRACTION_BITS);
    return word.value;
}
