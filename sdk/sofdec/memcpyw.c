/* Sofdec's halfword copy (sofdec_uty.h). it copies the halfwords past a
 * whole number of passes one at a time, then the passes */
#include <sg_xpt.h>
#include <sofdec_uty.h>
#include "../sofdec/copy16.h"

/* the halfwords each pass of the second loop copies */
#define PASS_HALFWORDS 16

void UTY_MemcpyWord(Uint16 *dst, const Uint16 *src, Uint32 count)
{
    Uint32 remainder;
    Uint32 passes;
    /* @hack redundant: first to fourth declared register, where plain locals
     * would do */
    register Uint16 first;
    register Uint16 second;
    register Uint16 third;
    register Uint16 fourth;

    /* @hack spelling: remainder counted from one more and decremented before
     * its test, where a count down from count % PASS_HALFWORDS would do */
    remainder = (count % PASS_HALFWORDS) + 1;
    while (--remainder != 0) {
        *dst++ = *src++;
    }
    /* @hack spelling: passes counted from one more and decremented before its
     * test, where a count down from count / PASS_HALFWORDS would do */
    passes = (count / PASS_HALFWORDS) + 1;
    while (--passes != 0) {
        COPY_SIXTEEN(dst, src, first, second, third, fourth);
    }
}
