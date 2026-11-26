#include <multiprec.h>

void _setsbit(unsigned char *bytes, int n, int bit)
{
    /* @hack spelling: the 8 as a variable, where the constant 8 would do */
    int byte_bits = 8;
    unsigned char mask;
    unsigned char *at;
    /* @hack redundant: register on cursor */
    register unsigned char *cursor;
    unsigned char below;
    unsigned char scan;

    at = bytes + (bit / byte_bits);
    cursor = at;
    mask = TOP_BIT >> (bit % byte_bits);
    if ((*cursor & mask) == 0) {
        scan = mask;
        for (below = 0; scan != 0; scan >>= 1) {
            below |= scan;
        }
        if ((*cursor & below) != 0) {
            *cursor |= mask;
        } else {
            for (bytes += n; ++cursor < bytes;) {
                if (*cursor != 0) {
                    *at |= mask;
                    break;
                }
            }
        }
    }
}
