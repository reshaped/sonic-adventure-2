#include <float_bits.h>
#include <mathf.h>

/* the unbiased exponent of 2^15. from that magnitude up, e^x is infinite, or
 * 0 for a negative x */
#define HUGE_EXPONENT 15
/* the unbiased exponent of a half */
#define HALF_EXPONENT (-1)

/* numerator holds P, a polynomial of degree 6, by its coefficients from t^6
 * down. denominator holds P(-t), whose odd powers' coefficients are
 * negated */
static const float numerator[7] = {
    1.2648366e-06F, 5.3142125e-05F, 0.0010630327F, 0.012757532F,
    0.095685475F,   0.4210225F,     0.84204495F,
};
static const float denominator[7] = {
    1.2648366e-06F, -5.3142125e-05F, 0.0010630327F, -0.012757532F,
    0.095685475F,   -0.4210225F,     0.84204495F,
};

/* @hack spelling: a polynomial of its own, whose degree is a local, where
 * float_bits.h's polynomial with a degree of 6 would do */
#pragma inline(series)
static float series(float x, const float *coefficient)
{
    float sum;
    int degree;

    sum = *coefficient++;
    degree = 6;
    do {
#ifdef NONMATCHING
        /* this side keeps the console's bits: SHC's code takes each step by
         * one FMAC, which rounds once */
        sum = __builtin_fmaf(sum, x, *coefficient++);
#else
        sum = (sum * x) + *coefficient++;
#endif
    } while (--degree > 0);
    return sum;
}

/* e^x is 2^n times 2^f. n is the float product of x and one_over_log_two,
 * about x / log 2, truncated toward zero. f is the product's rest.
 * where |f| is a half or more, 1 with f's sign moves from f to n. 2^f is
 * e^t, for t = f log 2, and P(t) / P(-t) approximates it */
float _in_exps(float x)
{
    /* one_over_log_two lies an ulp above the float nearest 1 / log 2 */
    /* @hack spelling: the constant declared in the function, where file
     * scope would do */
    static const float one_over_log_two = 1.4426951F;
    int kind = classify(x);
    int power;
    float scaled;
    float part;
    float fraction;

    /* @hack redundant: an else after a return, here and in the test of x's
     * sign below */
    if (kind & CLASS_NAN) {
        return _IFNANS;
    } else if (kind & CLASS_ZERO) {
        return _IFONES;
    }
    /* @hack type: x's exponent read through its address, where x as a union
     * with an int would do */
    if ((((*(int *)&x & FLOAT_EXPONENT) >> FLOAT_FRACTION_BITS) - FLOAT_BIAS) >=
        HUGE_EXPONENT) {
        if (kind & CLASS_NEGATIVE) {
            return _IFZEROS;
        } else {
            return _IFINFS;
        }
    }
    scaled = x * one_over_log_two;
    part = truncated(scaled);
    power = (int)part;
    fraction = scaled - part;
    /* @hack type: fraction's exponent and sign read through its address,
     * where fraction as a union with an int would do */
    if ((((*(int *)&fraction & FLOAT_EXPONENT) >> FLOAT_FRACTION_BITS) -
         FLOAT_BIAS) >= HALF_EXPONENT) {
        /* @hack spelling: power = power - 1 here and power + 1 below, where
         * power-- and power++ would do */
        if (*(int *)&fraction & FLOAT_SIGN) {
            fraction = _IFONES + fraction;
            power = power - 1;
        } else {
            fraction = _NIFONES + fraction;
            power = power + 1;
        }
    }
    scaled = _LOGE2S * fraction;
    part = series(scaled, numerator);
    part /= series(scaled, denominator);
    return _in_ldexps(part, power);
}
