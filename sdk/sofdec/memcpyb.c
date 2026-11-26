/* Sofdec's byte copy (sofdec_uty.h). it copies the bytes past a whole
 * number of passes one at a time, then the passes */
#include <sg_xpt.h>
#include <sofdec_uty.h>
#include "../sofdec/copy16.h"

/* the bytes each pass of the second loop copies */
#define PASS_BYTES 32

void UTY_MemcpyByte(Uint8 *dst, const Uint8 *src, Uint32 count)
{
    Uint32 remainder;
    Uint32 passes;
    /* @hack redundant: first to fourth declared register, where plain locals
     * would do */
    register Uint8 first;
    register Uint8 second;
    register Uint8 third;
    register Uint8 fourth;

    /* @hack spelling: remainder counted from one more and decremented before
     * its test, where a count down from count % PASS_BYTES would do */
    remainder = (count % PASS_BYTES) + 1;
    while (--remainder != 0) {
        *dst++ = *src++;
    }
    /* @hack spelling: passes counted from one more and decremented before its
     * test, where a count down from count / PASS_BYTES would do */
    passes = (count / PASS_BYTES) + 1;
    while (--passes != 0) {
        COPY_SIXTEEN(dst, src, first, second, third, fourth);
        COPY_SIXTEEN(dst, src, first, second, third, fourth);
    }
}
