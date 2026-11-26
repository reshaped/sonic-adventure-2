#include <float_bits.h>
#include <mathf.h>

float _in_ldexps(float x, int n)
{
    union {
        float value;
        int bits;
    } word;
    int exponent;

    word.value = x;
    exponent = word.bits & FLOAT_EXPONENT;
    if (exponent == FLOAT_EXPONENT) {
        if ((word.bits & FLOAT_FRACTION) == 0) {
            if ((word.bits & FLOAT_SIGN) == 0) {
                return _IFINFS;
            }
            return _NIFINFS;
        }
        return _IFNANS;
    }
    if (exponent <= 0) {
        if ((word.bits & FLOAT_SIGN) == 0) {
            return _IFZEROS;
        }
        return _NIFZEROS;
    }
    exponent = (exponent >> FLOAT_FRACTION_BITS) + n;
    if (exponent <= 0 || exponent >= FLOAT_EXPONENT_MAX) {
        if (n >= 0) {
            if ((word.bits & FLOAT_SIGN) == 0) {
                return _IFINFS;
            }
            return _NIFINFS;
        }
        if ((word.bits & FLOAT_SIGN) == 0) {
            return _IFZEROS;
        }
        return _NIFZEROS;
    }
    word.bits = (word.bits & FLOAT_SIGN) | (exponent << FLOAT_FRACTION_BITS) |
                (word.bits & FLOAT_FRACTION);
    return word.value;
}
