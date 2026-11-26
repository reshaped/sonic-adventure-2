#include <multiprec.h>
#include <string.h>

void _mult64(short *a_exponent, unsigned char *a, short b_exponent,
             const unsigned char *b)
{
    unsigned char product[16];

    /* @hack spelling: *a_exponent = *a_exponent + b_exponent, where += would
     * do */
    *a_exponent = *a_exponent + b_exponent;
    _mult(a, b, product);
    if (product[0] & TOP_BIT) {
        ++*a_exponent;
    } else {
        _lsft(product, 16);
    }
    /* fold every bit past the sticky bit into it, and keep none of them */
    _setsbit(product, 16, STICKY_BIT);
    product[8] &= ROUNDING_MASK;
    memcpy(a, product, STICKY_BYTES);
}
