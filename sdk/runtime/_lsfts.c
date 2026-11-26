#include <multiprec.h>

int _lsfts(unsigned char *bytes, int n, short bits)
{
    short kept;
    int mask;
    int i;
    unsigned char carry;
    unsigned char high;

    kept = 8 - bits;
    if (kept != 0) {
        mask = 0xff << kept;
        carry = 0;
        for (i = n - 1; i >= 0; i--) {
            high = bytes[i] & mask;
            bytes[i] <<= bits;
            bytes[i] |= carry;
            carry = high >> kept;
        }
        if (carry != 0) {
            return 1;
        }
        return 0;
    }
    /* @hack implicit: no return for a bits of 8, where return 0 would do: no
     * caller in the image reads the result */
#ifdef NONMATCHING
    /* this side returns 0, the tag's plain form */
    return 0;
#endif
} /* NOLINT(clang-diagnostic-return-type) */
