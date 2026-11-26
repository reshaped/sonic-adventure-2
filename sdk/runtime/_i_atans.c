#include <float_bits.h>
#include <mathf.h>

/* the unbiased exponent of 2^32. from that magnitude up, atan x is pi/2,
 * _HLFPAIS, with x's sign */
#define HUGE_EXPONENT 32
/* 1/4 is 2^QUARTER_EXPONENT, and 1/2 is 2^HALF_EXPONENT */
#define QUARTER_EXPONENT (-2)
#define HALF_EXPONENT (-1)

/* atan |x| is offset + atan reduced. below 1, offset is 0 and reduced is |x|.
 * above 1, offset is pi/4 and reduced is (|x| - 1) / (|x| + 1) */
float _in_atans(float x)
{
    float offset;
    float reduced;
    float result;
    float magnitude;
    unsigned int eighth;
    /* @hack type: kind as unsigned int, where int would do */
    unsigned int kind;
    /* atan_half and atan_three_quarters are the floats nearest atan 1/2 and
     * atan 3/4. atan_quarter lies an ulp above the float nearest atan 1/4 */
    /* @hack spelling: the constants declared in the function after its
     * locals, where file scope would do */
    static const float atan_quarter = 0.24497868F;
    static const float atan_half = 0.4636476F;
    static const float atan_three_quarters = 0.6435011F;
    static const float three_quarters = 0.75F;
    static const float minus_three_quarters = -0.75F;
    static const float an_eighth = 0.125F;

    kind = classify(x);
    magnitude = x;
    /* @hack type: magnitude's sign bit, and its exponent below, read through
     * its address, where magnitude as a union with an int would do */
    if (*(int *)&magnitude & FLOAT_SIGN) {
        magnitude = -magnitude;
    }
    if (kind & CLASS_NAN) {
        result = _IFNANS;
    } else if ((((*(int *)&magnitude & FLOAT_EXPONENT) >> FLOAT_FRACTION_BITS) -
                FLOAT_BIAS) >= HUGE_EXPONENT) {
        result = _HLFPAIS;
        if (kind & CLASS_NEGATIVE) {
            result = -result;
        }
    } else if (kind & CLASS_ZERO) {
        result = x;
    } else {
        if (magnitude < _IFONES) {
            offset = _IFZEROS;
            reduced = magnitude;
        } else if (_IFONES == magnitude) {
            result = _QUOPAIS;
            if (kind & CLASS_NEGATIVE) {
                result = -result;
            }
            return result;
        } else {
            offset = _QUOPAIS;
            reduced = (_NIFONES + magnitude) / (_IFONES + magnitude);
        }
        /* reduced lies in an eighth of [0, 1]. that eighth's case takes b, the
         * multiple of 1/4 nearest reduced. atan reduced is then atan b +
         * atan((reduced - b) / (1 + reduced b)). eighth is 8 only for a
         * reduced of 1. for a magnitude of 2^27, rounding to nearest gives
         * 1, and rounding toward zero gives the float just below 1 */
        _in_fmods(reduced, an_eighth, &eighth);
#ifdef NONMATCHING
        /* this side keeps the shipped result. GCC can't see that the cases take
         * every eighth, 0 to 8 */
        result = offset;
#endif
        switch (eighth) {
        case 0:
            result = offset + _app_atans(reduced);
            break;
        case 1:
        case 2:
            result =
                offset + atan_quarter +
                _app_atans((_NQUOTS + reduced) /
                           (_IFONES + _in_ldexps(reduced, QUARTER_EXPONENT)));
            break;
        case 3:
        case 4:
            result = offset + atan_half +
                     _app_atans((_NHALFS + reduced) /
                                (_IFONES + _in_ldexps(reduced, HALF_EXPONENT)));
            break;
        case 5:
        case 6:
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code forms 1 +
             * reduced * 3/4 by one FMAC, which rounds once */
            result =
                offset + atan_three_quarters +
                _app_atans((reduced + minus_three_quarters) /
                           __builtin_fmaf(reduced, three_quarters, _IFONES));
#else
            result = offset + atan_three_quarters +
                     _app_atans((reduced + minus_three_quarters) /
                                (_IFONES + (reduced * three_quarters)));
#endif
            break;
        case 7:
        case 8:
            result = offset + _QUOPAIS +
                     _app_atans((_NIFONES + reduced) / (_IFONES + reduced));
            break;
        }
        if (kind & CLASS_NEGATIVE) {
            result = -result;
        }
    }
    return result;
}
