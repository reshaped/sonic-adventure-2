/* Sofdec's scaling of one Sint32 by the ratio of two (sofdec_uty.h) */
#include <sg_xpt.h>
#include <sofdec_uty.h>

/* the results past Sint32's range, toward each sign */
#define POSITIVE_LIMIT 0x7fffffffL
#define NEGATIVE_LIMIT ((Sint32)0x80000000UL)

Sint32 UTY_MulAbDivC(Sint32 left, Sint32 right, Sint32 divisor)
{
    Sint32 sign;
    Sint32 limit;
    struct uty_u64 product;
    Sint32 quotient;
    Uint32 high;
    Uint32 low;

    sign = 1;
    if (left < 0) {
        left = -(Uint32)left;
        sign = -1;
    }
    if (right < 0) {
        right = -(Uint32)right;
        sign = -sign;
    }
    if (divisor < 0) {
        divisor = -(Uint32)divisor;
        sign = -sign;
    }
    if (sign > 0) {
        limit = POSITIVE_LIMIT;
    } else {
        limit = NEGATIVE_LIMIT;
    }
    if (divisor == 0) {
        return limit;
    }
    UTY_Dmulu(left, right, &product);
    high = product.high;
    low = product.low;
    if (high >= (Uint32)divisor) {
        return limit;
    }
    quotient = UTY_Div64u(high, low, divisor);
    if (sign < 0) {
        quotient = -(Uint32)quotient;
    }
    return quotient;
}
