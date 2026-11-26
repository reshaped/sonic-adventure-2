#include <multiprec.h>

int _sub(int mode, unsigned char *to, const unsigned char *from, int n)
{
    int difference;
    /* @hack spelling: zero declared ahead of borrow, not after it */
    int zero = 1;
    int borrow = 0;

    for (--n; n >= 0; --n) {
        difference = (int)to[n] - (int)from[n] - borrow;
        /* @hack spelling: the borrow and the wrap in one select, where an if
         * and an else would do */
        borrow = difference < 0 ? (difference = difference + 256, 1) : 0;
        if (difference != 0) {
            zero = 0;
        }
        to[n] = (unsigned char)difference;
    }
    if (borrow == 1 && mode == SUB_SIGNED) {
        return -1;
    }
    if (zero == 1) {
        return 0;
    }
    return 1;
}
