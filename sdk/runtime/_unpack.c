#include <multiprec.h>
#include <string.h>

void _unpack(long double value, signed char *sign, short *exponent,
             unsigned char *fraction)
{
    unsigned short *top_half;
    unsigned char word[4];

    /* @hack type: value's sign and exponent read as *top_half, at value's own
     * address, where a union copy's fourth unsigned short would do */
    top_half = (unsigned short *)&value + DOUBLE_TOP_HALF;
    /* @hack spelling: the masked sign compared with DOUBLE_TOP_SIGN, where
     * testing it as it is would do */
    *sign = (*top_half & DOUBLE_TOP_SIGN) == DOUBLE_TOP_SIGN ? -1 : 1;
    /* @hack spelling: *top_half stored in *exponent and masked there, where
     * one assignment that masks *top_half would do */
    *exponent = *top_half;
    *exponent = (*exponent & DOUBLE_TOP_EXPONENT) >> DOUBLE_TOP_FRACTION_BITS;
    /* @hack type: value's words passed to _mvito4b at their own addresses,
     * where a union copy's two unsigned longs would do */
    _mvito4b(word, (const unsigned long *)(top_half - 1));
    memcpy(fraction, word + 1, 3);
    _mvito4b(word, (const unsigned long *)(top_half - 3));
    memcpy(fraction + 3, word, 4);
    /* fraction holds the exponent's low 4 bits, then the 52 fraction bits.
     * shifting it left by 3 leaves one bit above them for the leading 1 */
    _lsfts(fraction, 7, 3);
    if (*exponent > 0) {
        fraction[0] |= TOP_BIT;
    } else {
        ++*exponent;
        fraction[0] &= 0x7f;
        while (!(fraction[0] & TOP_BIT)) {
            _lsfts(fraction, 7, 1);
            --*exponent;
        }
    }
}
