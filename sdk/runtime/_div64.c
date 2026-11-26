#include <multiprec.h>
#include <string.h>

void _div64(short *divisor_exponent, unsigned char *divisor,
            short dividend_exponent, const unsigned char *dividend)
{
    unsigned char remainder[12];
    unsigned char wide_divisor[12];
    unsigned char quotient[16];
    int i;

    memset(remainder, 0, 4);
    memcpy(&remainder[4], dividend, 8);
    memset(wide_divisor, 0, 4);
    memcpy(&wide_divisor[4], divisor, 8);
    memset(quotient, 0, 16);

    /* long division, one quotient bit for each bit before the sticky bit.
     * each bit enters at the bottom of quotient's first 12 bytes */
    for (i = 0; i < STICKY_BIT; i++) {
        _lsft(quotient, 12);
        if (_sub(SUB_SIGNED, remainder, wide_divisor, 12) >= 0) {
            quotient[11]++;
        } else {
            _add(remainder, wide_divisor, 12);
        }
        _lsft(remainder, 12);
    }
    /* the STICKY_BIT quotient bits end at the bottom of quotient[11]. two
     * shifts right move the first to the top of quotient[4]. the 9-byte
     * fraction is then quotient[4] to quotient[12] */
    for (i = 0; i < 2; i++) {
        _rsft(quotient, 16);
    }

    if (remainder[0] || remainder[1] || remainder[2] || remainder[3] ||
        remainder[4] || remainder[5] || remainder[6] || remainder[7] ||
        remainder[8] || remainder[9] || remainder[10] || remainder[11]) {
        quotient[12] |= STICKY_MASK;
    }

    *divisor_exponent = dividend_exponent - *divisor_exponent;

    if (quotient[4] || quotient[5] || quotient[6] || quotient[7] ||
        quotient[8] || quotient[9] || quotient[10] || quotient[11] ||
        quotient[12]) {
        while (!(quotient[4] & TOP_BIT)) {
            _lsft(&quotient[4], STICKY_BYTES);
            --*divisor_exponent;
        }
    }
    memcpy(divisor, &quotient[4], STICKY_BYTES);
}
