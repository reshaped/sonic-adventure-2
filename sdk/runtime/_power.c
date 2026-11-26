#include <multiprec.h>
#include <string.h>

/* the power of 10 the tables start from, which is also the most _pow10
 * takes */
#define FIRST_POWER 27
/* the bits of an 8-byte fraction, which _rnd rounds each product to */
#define FRACTION_BITS 64

/* fractions[0][i] is the fraction of 10^(FIRST_POWER << i), and
 * fractions[1][i] that of its reciprocal. each is 8 bytes, most significant
 * first, with the top bit set */
static const unsigned char fractions[2][8][8] = {
    {
        {0xce, 0xcb, 0x8f, 0x27, 0xf4, 0x20, 0x0f, 0x3a},
        {0xa7, 0x0c, 0x3c, 0x40, 0xa6, 0x4e, 0x6c, 0x52},
        {0xda, 0x01, 0xee, 0x64, 0x1a, 0x70, 0x8d, 0xeb},
        {0xb9, 0xa7, 0x4a, 0x06, 0x37, 0xce, 0x2e, 0xe3},
        {0x86, 0xa3, 0x36, 0x4e, 0xa6, 0x2c, 0x67, 0x2f},
        {0x8d, 0x9e, 0x89, 0xd1, 0x13, 0x46, 0xbd, 0xaa},
        {0x9c, 0xb0, 0x0b, 0xfd, 0x6f, 0x02, 0x53, 0x43},
        {0xbf, 0xce, 0x0f, 0x5a, 0xb8, 0xa6, 0x76, 0x36},
    },
    {
        {0x9e, 0x74, 0xd1, 0xb7, 0x91, 0xe0, 0x7e, 0x48},
        {0xc4, 0x28, 0xd0, 0x5a, 0xa4, 0x75, 0x1e, 0x4c},
        {0x96, 0x4e, 0x85, 0x8c, 0x91, 0xba, 0x26, 0x54},
        {0xb0, 0x80, 0x39, 0x2c, 0xc4, 0x34, 0x9d, 0xea},
        {0xf3, 0x61, 0x1d, 0xad, 0x8e, 0xa3, 0x09, 0xe6},
        {0xe7, 0x61, 0x83, 0x2e, 0xfd, 0xc0, 0x64, 0x53},
        {0xd1, 0x21, 0x1f, 0xe3, 0x7a, 0xc6, 0xa1, 0x2d},
        {0xaa, 0xd7, 0x1a, 0x5a, 0xab, 0x16, 0xdc, 0x40},
    },
};
/* the power of 2 each fraction's top bit stands for, in the same order */
static const short exponents[2][8] = {
    {89, 179, 358, 717, 1435, 2870, 5740, 11480},
    {-90, -180, -359, -718, -1436, -2871, -5741, -11481},
};

/* 10^n is a product. each bit set in |n| / FIRST_POWER gives it the table
 * entry for that bit. |n| % FIRST_POWER gives it a power of 10 from _pow10 */
void _power(int n, short *exponent, unsigned char *fraction)
{
    int negative;
    int whole;
    int rest;
    int i;
    short scale;
    unsigned char product[STICKY_BYTES];
    /* @hack redundant: 12 bytes of frame that nothing reads or writes */
    unsigned char unused[12];

    if (n >= 0) {
        negative = 0;
    } else {
        negative = 1;
        n = -n;
    }
    whole = n / FIRST_POWER;
    rest = n % FIRST_POWER;
    if (negative == 1 && rest != 0) {
        /* _pow10 takes no negative power. 10^-rest is 10^-FIRST_POWER times
         * 10^(FIRST_POWER - rest), so start from 10^-FIRST_POWER */
        rest = FIRST_POWER - rest;
        scale = exponents[1][0];
        memcpy(product, fractions[1][0], 8);
        product[8] = 0;
    } else {
        memset(product, 0, STICKY_BYTES);
        product[0] |= TOP_BIT;
        scale = 0;
    }
    for (i = 0; i < 8; i++) {
        if (whole & 1) {
            _mult64(&scale, product, exponents[negative][i],
                    fractions[negative][i]);
            if (_rnd(product, FRACTION_BITS)) {
                scale++;
                product[0] |= TOP_BIT;
            }
        }
        whole >>= 1;
    }
    if (rest != 0) {
        _pow10(rest, exponent, fraction);
        _mult64(&scale, product, *exponent, fraction);
        /* @hack spelling: _rnd's carry compared with 1 here, and tested as it
         * is above, where one form would do */
        if (_rnd(product, FRACTION_BITS) == 1) {
            scale++;
            product[0] |= TOP_BIT;
        }
    }
    *exponent = scale;
    memcpy(fraction, product, 8);
}
