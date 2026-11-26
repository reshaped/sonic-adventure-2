/* the AHX decoder's timer (mpeg_timer.h) */
#include <mpeg_timer.h>
#include <sg_xpt.h>
#include <shinobi.h>

Uint32 mpeg_timer_count(void)
{
    /* @hack spelling: syTmrGetCount's result returned through count, where
     * returning the call would do */
    Uint32 count = syTmrGetCount();

    return count;
}

Uint32 mpeg_timer_microseconds(Uint32 count)
{
    /* @hack spelling: syTmrCountToMicro's result returned through
     * microseconds, where returning the call would do */
    Uint32 microseconds = syTmrCountToMicro(count);

    return microseconds;
}

Uint32 mpeg_timer_elapsed(Uint32 start, Uint32 end)
{
    return end - start;
}
