#include <sg_xpt.h>
#include <shinobi.h>
#include "../nexgen/ngos_ETD_DLC.h"

void ETD_DlcWait(Uint32 delay)
{
    Uint32 start;
    Uint32 ticks;
    Uint32 elapsed;

    start = syTmrGetCount();
    /* @hack spelling: an endless loop that breaks, where do-while would do */
    for (;;) {
        /* @hack spelling: each call's result held in ticks or elapsed, where
         * the next call and the test could take them */
        ticks = syTmrDiffCount(start, syTmrGetCount());
        elapsed = syTmrCountToMicro(ticks);
        if (elapsed > delay) {
            break;
        }
    }
}
