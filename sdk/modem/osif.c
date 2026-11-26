/* the system's services to the modem, which are interrupt hooks, a clock
 * and the date */
#include <sg_chain.h>
#include <sg_syrtc.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "osif.h"

/* the chain priorities at which osifHookDataPumpIntr and osifHookTimerIntr
 * add their handlers */
#define PUMP_PRIORITY 16
#define TIMER_PRIORITY 72

/* the microseconds in a tick of osifGet10msCount */
#define TICK_MICROS 10000

/* the ticks osifGet10msCount adds for each wrap of syTmrGetCount's count */
#define WRAP_TICKS 515396

/* the wraps of syTmrGetCount's count that osifGet10msCount has seen */
static Uint32 osif_wraps = 0;
/* the handlers osifHookTimerIntr and osifHookDataPumpIntr last added, and
 * whether each is still on its chain */
static SYCHAIN osif_timer_chain = 0;
static SYCHAIN osif_pump_chain = 0;
static char osif_timer_on = 0;
static char osif_pump_on = 0;
/* nonzero while osifGet10msCount computes its result. a call from an
 * interrupt in that time returns osif_ticks, the last result */
static char osif_counting = 0;
/* osifGet10msCount's last reading of syTmrGetCount's count, and its last
 * result */
static Uint32 osif_last_count = 0;
static Uint32 osif_ticks = 0;

void osifHookDataPumpIntr(char hook, void (*func)(void *))
{
    if (hook) {
        osif_pump_chain = syChainAddHandler(SYD_CHAIN_EVENT_IML4_MODEM, func,
                                            PUMP_PRIORITY, NULL);
        osif_pump_on = 1;
    } else if (osif_pump_on) {
        syChainDeleteHandler(osif_pump_chain);
        osif_pump_on = 0;
    }
}

void osifHookTimerIntr(char hook, void (*func)(void *))
{
    if (hook) {
        osif_timer_chain = syChainAddHandler(SYD_CHAIN_EVENT_IML6_VBLANK, func,
                                             TIMER_PRIORITY, NULL);
        osif_timer_on = 1;
    } else if (osif_timer_on) {
        syChainDeleteHandler(osif_timer_chain);
        osif_timer_on = 0;
    }
}

Uint32 osifGet10msCount(void)
{
    Uint32 count;
    Uint32 half;
    Uint32 total;

    if (!osif_counting) {
        osif_counting = 1;
        count = syTmrGetCount();
        if (osif_last_count > count) {
            osif_wraps++;
        }
        osif_last_count = count;
        /* syTmrCountToMicro overflows for a count of 0xc8000000 or more.
         * each half of the count takes a call of its own */
        half = count / 2;
        count -= half;
        total = syTmrCountToMicro(count) / TICK_MICROS;
        /* @hack spelling: half's microseconds stored in count first, where
         * total += syTmrCountToMicro(half) / TICK_MICROS would do */
        count = syTmrCountToMicro(half);
        total += count / TICK_MICROS;
        /* @bug logic: the result steps back about 343 seconds at each wrap
         * of syTmrGetCount's count. WRAP_TICKS takes each of a wrap's 2^32
         * counts as 1.2 microseconds, where a count is 1.28 (the MOV.L at
         * 0x0cae3b32 loads WRAP_TICKS, H'0007DD44) */
        /* @hack spelling: the sum written out, where += would do */
        total = total + (osif_wraps * WRAP_TICKS);
        osif_counting = 0;
        osif_ticks = total;
    }
    return osif_ticks;
}

void osifGetSystemTime(struct Sys_Time *time)
{
    SYS_RTC_DATE date;

    if (syRtcGetDate(&date) == 0) {
        time->year = date.year;
        time->month = date.month;
        time->day = date.day;
        time->hour = date.hour;
        time->minute = date.minute;
        time->second = date.second;
    } else {
        time->year = 0;
        time->month = 0;
        time->day = 0;
        time->hour = 0;
        time->minute = 0;
        time->second = 0;
    }
}
