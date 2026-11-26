#include <multiprec.h>
#include <string.h>

/* the bits of the integer _calcnpw stores */
#define INTEGER_BITS 64

void _calcnpw(short exponent, const unsigned char *fraction, int n,
              unsigned char *integer)
{
    short scaled_exponent;
    unsigned char scaled[STICKY_BYTES];
    short shift;
    short bytes;
    short i;
    /* @hack spelling: the 8 as a variable, where the constant 8 would do */
    short byte_bits = 8;

    if (n > 0) {
        _power(n, &scaled_exponent, scaled);
        _div64(&scaled_exponent, scaled, exponent, fraction);
    } else {
        _power(-n, &scaled_exponent, scaled);
        _mult64(&scaled_exponent, scaled, exponent, fraction);
    }
    /* shift scaled right until its units bit is bit INTEGER_BITS - 1. its
     * whole part then fills its first 8 bytes. _setsbit first folds every bit
     * the shift would push past the sticky bit into bit STICKY_BIT - shift. the
     * shift then moves that bit to the sticky bit */
    shift = (INTEGER_BITS - 1) - scaled_exponent;
    bytes = shift / byte_bits;
    _setsbit(scaled, STICKY_BYTES, STICKY_BIT - shift);
    for (i = STICKY_BYTES - 1; i >= bytes; --i) {
        scaled[i] = scaled[i - bytes];
    }
    for (; i >= 0; --i) {
        scaled[i] = 0;
    }
    _rsfts(scaled, STICKY_BYTES, shift - (bytes * 8));
    /* @hack redundant: the store to scaled_exponent, which nothing reads */
    scaled_exponent = INTEGER_BITS - 1;
    _rnd(scaled, INTEGER_BITS);
    memcpy(integer, scaled, 8);
}
