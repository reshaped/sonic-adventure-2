/* the AHX decoder's timer, in syTmrGetCount's counts */
#ifndef MPEG_TIMER_H
#define MPEG_TIMER_H
#include <sg_xpt.h>

Uint32 mpeg_timer_count(void);
Uint32 mpeg_timer_microseconds(Uint32 count);
/* end less start, modulo 2^32, which holds across a wrap of the count */
Uint32 mpeg_timer_elapsed(Uint32 start, Uint32 end);

#endif
