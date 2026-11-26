#include <multiprec.h>

int _rnd(unsigned char *bytes, int bits)
{
    /* @hack spelling: the 8 as a variable, where the constant 8 would do */
    int byte_bits = 8;
    int round_up = 0;
    unsigned char *rest;
    unsigned char *last;
    unsigned char first_mask;
    unsigned char second_mask;
    unsigned char third_mask;
    unsigned char last_mask;

    rest = bytes + (bits / byte_bits);
    first_mask = TOP_BIT >> (bits % byte_bits);
    second_mask = first_mask >> 1;
    third_mask = second_mask >> 1;
    /* last and last_mask address the last bit kept, where a round up adds 1 */
    last = bytes + ((bits - 1) / byte_bits);
    last_mask = TOP_BIT >> ((bits - 1) % byte_bits);
    /* @hack spelling: the empty first arm, where if (*rest & first_mask)
     * around the other tests would do */
    if (!(*rest & first_mask)) {
    } else if ((*rest & second_mask) || (*rest & third_mask)) {
        round_up = 1;
    } else if (*last & last_mask) {
        round_up = 1;
    } else {
        /* @hack redundant: the empty last arm */
    }
    if (round_up == 1) {
        /* @bug memory: a carry out of the first byte steps last to the
         * byte before bytes, which the loop's test reads (MOV.B @R5,R2 at
         * 0x8c0cd526) before it tests last >= bytes (CMP/HS R4,R5 at
         * 0x8c0cd530) */
#ifdef NONMATCHING
        /* this side repairs the tag above: a carry out of the first byte
         * returns 1 before last steps below bytes */
        while (*last & last_mask) {
            *last &= ~last_mask;
            last_mask <<= 1;
            if (last_mask == 0) {
                if (last == bytes) {
                    return 1;
                }
                last--;
                last_mask = 1;
            }
        }
        *last |= last_mask;
#else
        while ((*last & last_mask) && last >= bytes) {
            *last &= ~last_mask;
            last_mask <<= 1;
            if (last_mask == 0) {
                last--;
                last_mask = 1;
            }
        }
        if (last >= bytes) {
            *last |= last_mask;
        } else {
            return 1;
        }
#endif
    }
    return 0;
}
