#include <errno.h>
#include <multiprec.h>

/* the exponent field, shifted down, of a double from 0.5 up to 1 */
#define HALF_FIELD (DOUBLE_BIAS - 1)
/* the bit of a double's second byte from the top where its implicit 1 would
 * lie */
#define IMPLICIT_BIT 0x10
/* the bits of a word, half a double */
#define WORD_BITS 32
/* log10 2, to three places */
#define LOG10_TWO 0.301L

static long double split_exponent(long double value, int *exp);
static long double split_whole(long double value, long double *whole);

int _log10(long double value)
{
    int exp;
    long double whole;

#ifdef NONMATCHING
    /* this side keeps the shipped result. value is finite, and so is exp's
     * product, so the splits set exp and whole. GCC can't see that */
    exp = 0;
    whole = 0.0L;
#endif
    split_exponent(value, &exp);
    split_whole(exp * LOG10_TWO, &whole);
    return (int)whole;
}

/* frexp for a double. store value's binary exponent in *exp, and return its
 * fraction. a nonzero fraction's magnitude lies in [0.5, 1). a zero returns
 * 0, with *exp 0. a NaN sets errno to ENUM and an infinity sets it to ERANGE.
 * either returns all ones and leaves *exp unset */
static long double split_exponent(long double value, int *exp)
{
    unsigned char bytes[8];
    unsigned int keep = DOUBLE_TOP_SIGN | DOUBLE_TOP_FRACTION;
    unsigned char *at;
    int kind;
    int top;

    /* @hack spelling: kind assigned in the test, and tested by truth, where a
     * statement of its own ahead of it and != DUCHEK_FINITE would do */
    if ((kind = _duchek(value))) {
        if (kind == DUCHEK_NAN) {
            errno = ENUM;
        }
        if (kind == DUCHEK_INFINITE) {
            errno = ERANGE;
        }
        /* @hack type: the all-ones result stored as value's two words
         * through unsigned long lvalues, where a union's two words would
         * do */
        ((unsigned long *)&value)[0] = -1;
        ((unsigned long *)&value)[1] = -1;
        return value;
    }
    if (value == 0.0) {
        *exp = 0;
        return 0.0;
    }
    at = (unsigned char *)&value;
    /* @hack type: value's exponent halfword read through an unsigned short
     * lvalue at value's own address, where a union copy's fourth halfword
     * would do */
    top = *((unsigned short *)at + DOUBLE_TOP_HALF);
    *exp = (((unsigned int)top >> DOUBLE_TOP_FRACTION_BITS) &
            DOUBLE_EXPONENT_MAX) -
           HALF_FIELD;
    if (!(*exp + HALF_FIELD)) {
        /* a subnormal. shift its fraction left until the leading 1 stands at
         * the implicit bit */
        ++*exp;
        /* @hack type: the exponent halfword masked in place through an
         * unsigned short lvalue, where a union copy masked and written back
         * would do */
        *((unsigned short *)at + DOUBLE_TOP_HALF) &= keep;
        /* @hack type: value's words passed to _mvito4b at their own
         * addresses, where a union copy's two words would do */
        _mvito4b(bytes, (unsigned long *)at + 1);
        _mvito4b(bytes + 4, (unsigned long *)at);
        while (!(bytes[1] & IMPLICIT_BIT)) {
            _lsft(bytes + 1, 7);
            --*exp;
        }
        /* @hack type: the byte vector's words passed to _mvito4b at their
         * own addresses, where the bytes in a union with two words would
         * do */
        _mvito4b(at + 4, (unsigned long *)bytes);
        _mvito4b(at, (unsigned long *)bytes + 1);
    }
    /* @hack spelling: the exponent field stored in kind and ORed from there,
     * where the constant in the OR would do */
    kind = HALF_FIELD << DOUBLE_TOP_FRACTION_BITS;
    /* @hack type: the exponent field set in place through unsigned short
     * lvalues, where a union copy written back would do */
    *((unsigned short *)at + DOUBLE_TOP_HALF) &= keep;
    *((unsigned short *)at + DOUBLE_TOP_HALF) |= kind;
    return value;
}

/* modf for a double, as far as _log10 needs it. *whole gets 0 where value's
 * magnitude is below 1, and value itself otherwise, for _log10 to truncate.
 * the double returned isn't modf's fraction, and _log10 doesn't read it. a
 * NaN sets errno to ENUM and an infinity sets it to ERANGE. either returns all
 * ones and leaves *whole unset */
static long double split_whole(long double value, long double *whole)
{
    int kind;
    int exp;
    unsigned long mask;

    /* @hack spelling: kind assigned in the test, and tested by truth, where a
     * statement of its own ahead of it and != DUCHEK_FINITE would do */
    if ((kind = _duchek(value))) {
        if (kind == DUCHEK_NAN) {
            errno = ENUM;
        }
        if (kind == DUCHEK_INFINITE) {
            errno = ERANGE;
        }
        /* @hack type: the all-ones result stored as value's two words
         * through unsigned long lvalues, where a union's two words would
         * do */
        ((unsigned long *)&value)[0] = -1;
        ((unsigned long *)&value)[1] = -1;
        return value;
    }
    /* @hack type: value's exponent halfword read through an unsigned short
     * lvalue at byte 6 of value, where a union copy's fourth halfword would
     * do */
    exp = ((unsigned short *)((unsigned char *)&value +
                              (DOUBLE_TOP_HALF * sizeof(unsigned short))))[0];
    exp =
        ((unsigned int)exp >> DOUBLE_TOP_FRACTION_BITS & DOUBLE_EXPONENT_MAX) -
        DOUBLE_BIAS;
    if (exp < 0) {
        *whole = 0.0;
    } else {
        *whole = value;
        if (exp < DOUBLE_FRACTION_BITS) {
            /* @hack type: value's fraction words cleared in place through
             * unsigned long lvalues, where a union copy written back would
             * do */
            if ((DOUBLE_FRACTION_BITS - exp) / WORD_BITS > 0) {
                ((unsigned long *)&value)[0] = 0;
            }
            /* for an exponent from 21 to 51, every fraction bit left to clear
             * lies in the low word. this clears as many low bits of the high
             * word instead. only the double returned changes, and _log10
             * doesn't read it */
            mask = -1UL << ((DOUBLE_FRACTION_BITS - exp) % WORD_BITS);
            ((unsigned long *)&value)[1] &= mask;
        }
    }
    value -= *whole;
    if (*whole < 0.0) {
        ((unsigned char *)&value)[7] |= DOUBLE_TOP_SIGN >> 8;
    }
    return value;
}
