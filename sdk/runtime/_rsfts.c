#include <multiprec.h>

int _rsfts(unsigned char *bytes, int n, short bits)
{
    int i;
    int mask;
    short kept;
    unsigned char carry;
    unsigned char low;

    kept = 8 - bits;
    if (kept != 0) {
        mask = 0xff >> kept;
        carry = 0;
        for (i = 0; i < n; ++i) {
            low = *bytes & mask;
            *bytes >>= bits;
            *bytes |= carry;
            carry = low << kept;
            /* @hack spelling: bytes = bytes + 1, where ++bytes, bytes++ or
             * bytes += 1 would do */
            bytes = bytes + 1;
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
