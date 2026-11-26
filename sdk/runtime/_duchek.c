#include <multiprec.h>

int _duchek(long double value)
{
    /* @hack type: value's words read through an unsigned long cursor, where a
     * union copy's two unsigned longs would do */
    unsigned long *word = (unsigned long *)&value + DOUBLE_HIGH_WORD;

    if ((*word & DOUBLE_HIGH_EXPONENT) != DOUBLE_HIGH_EXPONENT) {
        return DUCHEK_FINITE;
    }
    if (*word & DOUBLE_HIGH_FRACTION) {
        return DUCHEK_NAN;
    }
    if (*--word) {
        return DUCHEK_NAN;
    }
    return DUCHEK_INFINITE;
}
