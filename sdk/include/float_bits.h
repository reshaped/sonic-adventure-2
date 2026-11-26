/* the fields of a float's bits, and the helpers the C runtime's float routines
 * share */
#ifndef FLOAT_BITS_H
#define FLOAT_BITS_H

/* a float's sign bit, exponent field and fraction field, and the fraction's
 * width in bits */
#define FLOAT_SIGN 0x80000000
#define FLOAT_EXPONENT 0x7f800000
#define FLOAT_FRACTION 0x007fffff
#define FLOAT_FRACTION_BITS 23
/* the exponent field of an infinity or a NaN, shifted down */
#define FLOAT_EXPONENT_MAX (FLOAT_EXPONENT >> FLOAT_FRACTION_BITS)
/* the exponent field, shifted down, of a float from 1 up to 2 */
#define FLOAT_BIAS 127

/* the exponent field of 2^FLOAT_FRACTION_BITS. a float with this exponent or
 * a greater one holds no fraction */
#define WHOLE_EXPONENT                                                         \
    ((FLOAT_BIAS + FLOAT_FRACTION_BITS) << FLOAT_FRACTION_BITS)

/* the flags classify(x) returns for a float x. at most one of CLASS_NAN,
 * CLASS_INFINITE and CLASS_ZERO is set. CLASS_NEGATIVE is x's sign bit, so -0
 * has it, and so may a NaN */
enum { CLASS_NAN = 1, CLASS_INFINITE = 2, CLASS_ZERO = 4, CLASS_NEGATIVE = 8 };
#pragma inline(classify)
static int classify(float x)
{
    int kind = 0;
    union {
        float value;
        int bits;
    } word;

    word.value = x;
    if (word.bits < 0) {
        kind = CLASS_NEGATIVE;
    }
    if (x == 0.0F) {
        kind |= CLASS_ZERO;
    } else if ((word.bits & ~FLOAT_SIGN) == FLOAT_EXPONENT) {
        kind |= CLASS_INFINITE;
    } else if (x != x) {
        kind |= CLASS_NAN;
    }
    return kind;
}

/* x truncated toward zero, with x's sign: -0.5 gives -0. an infinity and a NaN
 * come back as they are */
#pragma inline(truncated)
static float truncated(float x)
{
    union {
        float value;
        int bits;
    } word;
    int sign;

    word.value = x;
    sign = word.bits & FLOAT_SIGN;
    if ((word.bits & FLOAT_EXPONENT) < WHOLE_EXPONENT) {
        word.value = (float)(int)x;
    }
    word.bits |= sign;
    return word.value;
}

/* the polynomial of the given degree at x, from its degree + 1 coefficients,
 * highest power first. it sums by Horner's rule. degree must be 1 or more */
#pragma inline(polynomial)
static float polynomial(float x, const float *coefficient, int degree)
{
    float sum = *coefficient++;

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

#endif
