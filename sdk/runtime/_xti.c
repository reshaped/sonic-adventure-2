#include <multiprec.h>
#include <string.h>

/* a double's top byte, bytes[7], holds its sign and its exponent field's
 * high 7 bits. bytes[6] holds the field's low 4 bits over 4 fraction bits */
#define TOP_BYTE_SIGN 0x80
#define TOP_BYTE_EXPONENT 0x7f
#define NEXT_BYTE_EXPONENT 0xf0

int _xti(long double value, int digits, unsigned char *integer, int *scale,
         signed char *sign)
{
    unsigned char *bytes;
    unsigned char fraction[8];
    unsigned char upper[8];
    unsigned char lower[8];
    unsigned char copy[8];
    short scale_raised;
    int i;
    int shift;
    int byte_bits;
    short exponent;
    int decimal_exponent;

    /* @hack spelling: byte_bits, scale_raised and bytes set in that order,
     * not in the order they are declared */
    /* @hack spelling: the 8 as a variable, where the constant 8 would do */
    byte_bits = 8;
    scale_raised = 0;
    bytes = (unsigned char *)&value;

    /* zero of either sign, which _unpack does not take */
    if ((bytes[7] & TOP_BYTE_EXPONENT) == 0 && _allzero(bytes, 7)) {
        for (i = 0; i < 8; i++) {
            integer[i] = 0;
        }
        *scale = 0;
        if (bytes[7] & TOP_BYTE_SIGN) {
            *sign = -1;
        } else {
            *sign = 1;
        }
        return XTI_FINITE;
    }
    _unpack(value, sign, &exponent, fraction);
    /* @hack spelling: value's sign bit cleared through (unsigned char *)&value,
     * where bytes, which holds that address, would do */
    if (*sign == -1) {
        ((unsigned char *)&value)[7] &= TOP_BYTE_EXPONENT;
    }
    if (exponent == DOUBLE_EXPONENT_MAX) {
        /* an infinity's fraction bits are 0: the low half of bytes[6], and
         * bytes[0] to bytes[5] */
        if (bytes[6] == NEXT_BYTE_EXPONENT && _allzero(bytes, 6)) {
            /* @hack redundant: the test of *sign == -1, the only sign _unpack
             * leaves once *sign == 1 has failed */
            if (*sign == 1) {
                return XTI_PLUS_INFINITY;
            } else if (*sign == -1) {
                return XTI_MINUS_INFINITY;
            }
        } else {
            return XTI_NAN;
        }
    }
    exponent -= DOUBLE_BIAS;
    if (exponent >= 0) {
        decimal_exponent = _log10(value);
    } else {
        decimal_exponent = _log10(value) - 1;
    }
    /* _calcnpw takes an 8-byte fraction, and _unpack fills 7 */
    fraction[7] = 0;
    *scale = decimal_exponent - digits + 1;
    /* upper is 10^digits and lower 10^(digits - 1), the bounds of an integer
     * of digits digits. 10^n is 5^n shifted left n bits */
    _pow5(digits, upper);
    shift = digits / byte_bits;
    for (i = 0; i < 8 - shift; i++) {
        upper[i] = upper[i + shift];
    }
    for (; i < 8; i++) {
        upper[i] = 0;
    }
    _lsfts(upper, 8, digits - (shift * 8));
    _pow5(digits - 1, lower);
    shift = (digits - 1) / byte_bits;
    for (i = 0; i < 8 - shift; i++) {
        lower[i] = lower[i + shift];
    }
    for (; i < 8; i++) {
        lower[i] = 0;
    }
    _lsfts(lower, 8, digits - 1 - (shift * 8));
    /* step *scale until the integer is at least lower and below upper. once
     * *scale has risen, it never falls, so the steps can't cycle */
    /* @hack spelling: the label retry and its two gotos, where a for loop
     * with continue would do */
retry:
    _calcnpw(exponent, fraction, *scale, integer);
    /* @hack redundant: the copies of integer to copy, where memcmp of integer
     * itself would do */
    memcpy(copy, integer, 8);
    if (memcmp(copy, upper, 8) > 0) {
        scale_raised = 1;
        ++*scale;
        goto retry;
    }
    memcpy(copy, integer, 8);
    if (memcmp(copy, upper, 8) == 0) {
        ++*scale;
        _calcnpw(exponent, fraction, *scale, integer);
    } else {
        memcpy(copy, integer, 8);
        if (memcmp(copy, lower, 8) < 0 && scale_raised == 0) {
            --*scale;
            goto retry;
        }
    }
    return XTI_FINITE;
}
