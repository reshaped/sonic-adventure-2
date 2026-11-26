#include <float_bits.h>
#include <mathf.h>

#define SIGN_AND_FRACTION (FLOAT_SIGN | FLOAT_FRACTION)
#define ONE_EXPONENT (FLOAT_BIAS << FLOAT_FRACTION_BITS)

/* root_two lies an ulp above the float nearest root 2, and minus_root_two
 * is its negative */
static const float root_two = 1.4142137F;
static const float minus_root_two = -1.4142137F;
/* 2 atanh t's series, by its coefficients of t^15, t^13 and on down to t.
 * each is the float nearest the term beside it, or as far off as its row
 * says */
static const float coefficients[8] = {
    0.13237529F, /* 2/15, 64294 ulps below */
    0.1538774F, /* 2/13, 2097 ulps above */
    0.18181759F, /* 2/11, 40 ulps below */
    0.22222222F, /* 2/9 */
    0.2857143F, /* 2/7 */
    0.4F, /* 2/5 */
    0.6666667F, /* 2/3 */
    2.0F,
};

/* x is m 2^e, with m in [1, 2). log x is then (e + 1/2) log 2 +
 * log(m / root 2). t = (m - root 2) / (m + root 2) lies within 0.172 of 0.
 * log(m / root 2) is 2 atanh t */
float _in_logs(float x)
{
    float fraction;
    float power;
    float reduced;
    float result;
    int kind = classify(x);

    /* @hack redundant: an else after each return of these tests */
    if (kind & CLASS_NAN) {
        return _IFNANS;
    } else if (kind & CLASS_ZERO) {
        return _NIFINFS;
    } else if (kind & CLASS_NEGATIVE) {
        return _IFNANS;
    } else if (kind & CLASS_INFINITE) {
        return _IFINFS;
    }
    if (x == _IFONES) {
        return _IFZEROS;
    }
    fraction = x;
    /* @hack type: fraction's bits rewritten through its address, where
     * fraction as a union with an int would do */
    *(int *)&fraction &= SIGN_AND_FRACTION;
    *(int *)&fraction |= ONE_EXPONENT;
    /* @hack type: x's exponent read through its address, where x as a union
     * with an int would do */
    power =
        ((((*(int *)&x & FLOAT_EXPONENT) >> FLOAT_FRACTION_BITS) - FLOAT_BIAS) +
         _HALFS) *
        _LOGE2S;
    reduced = (fraction + minus_root_two) / (fraction + root_two);
    result = reduced * polynomial(reduced * reduced, coefficients, 7);
    return power + result;
}
