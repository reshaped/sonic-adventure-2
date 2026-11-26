/* the call that splits a stream joint's chunk in two */
#include <sg_xpt.h>
#include <sj.h>

void SJ_SplitChunk(SJCK *ck, Sint32 nbyte, SJCK *ck1, SJCK *ck2)
{
    *ck1 = *ck;
    /* @hack spelling: ck2->len holds the chunk's whole len until ck1's
     * comes off it, where a local would do */
    ck2->len = ck1->len;
    if (ck1->len > nbyte) {
        ck1->len = nbyte;
    }
    ck2->len -= ck1->len;
    if (ck2->len == 0) {
        ck2->data = NULL;
    } else {
        ck2->data = ck1->data + ck1->len;
    }
}
