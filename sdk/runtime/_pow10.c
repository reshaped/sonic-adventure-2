#include <multiprec.h>
#include <string.h>

/* the power of 2 that the top bit of an 8-byte integer stands for */
#define TOP_EXPONENT 63

/* 10^n is 5^n times 2^n. the fraction is 5^n shifted left until its top bit
 * is set. before the shift, the top bit of the 8 bytes stands for
 * 2^(n + TOP_EXPONENT) */
void _pow10(int n, short *exponent, unsigned char *fraction)
{
    unsigned char five[8];
    unsigned char *top;
    unsigned char mask;
    int bytes;
    int bits;

    *exponent = n + TOP_EXPONENT;
    _pow5(n, five);
    memset(fraction, 0, 8);
    bytes = 0;
    bits = 0;
    top = five;
    while (*top == 0) {
        *exponent -= 8;
        top++;
        bytes++;
    }
    mask = TOP_BIT;
    while ((*top & mask) == 0) {
        mask >>= 1;
        bits++;
    }
    _lsfts(top, 8 - bytes, bits);
    *exponent -= bits;
    memcpy(fraction, top, 8 - bytes);
}
