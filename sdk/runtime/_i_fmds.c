#include <float_bits.h>
#include <mathf.h>

float _in_fmods(float x, float y, unsigned int *quotient)
{
    union {
        float value;
        int bits;
    } dividend, divisor, remainder;
    float whole_quotient;
    int sign;

    dividend.value = x;
    divisor.value = y;
    if ((dividend.bits & FLOAT_EXPONENT) == FLOAT_EXPONENT ||
        ((divisor.bits & FLOAT_EXPONENT) == FLOAT_EXPONENT &&
         (divisor.bits & FLOAT_FRACTION) != 0) ||
        divisor.value == 0.0F) {
        *quotient = 0;
        remainder.value = _IFNANS;
    } else if ((divisor.bits & FLOAT_EXPONENT) == FLOAT_EXPONENT) {
        *quotient = 0;
        remainder.value = x;
    } else {
        sign = dividend.bits & FLOAT_SIGN;
        dividend.bits &= ~FLOAT_SIGN;
        divisor.bits &= ~FLOAT_SIGN;
        whole_quotient = truncated(dividend.value / divisor.value);
        *quotient = (unsigned int)whole_quotient;
        remainder.value = dividend.value - (whole_quotient * divisor.value);
        remainder.bits |= sign;
    }
    return remainder.value;
}
