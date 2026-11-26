/* osif: the system's services to the modem, which are interrupt hooks, a
 * clock and the date */
#ifndef MODEM_OSIF_H
#define MODEM_OSIF_H
#include <sg_xpt.h>

/* a date and time, each field as in sg_syrtc.h's SYS_RTC_DATE */
struct Sys_Time {
    Uint16 year;
    Uint8 month;
    Uint8 day;
    Uint8 hour;
    Uint8 minute;
    Uint8 second;
};

/* where hook isn't 0, add func to the chain of the modem's interrupt. where
 * hook is 0, take off the handler the last such call added, if it is still
 * on. a handler added by an earlier call stays on, since only the last
 * handler's number is kept */
void osifHookDataPumpIntr(char hook, void (*func)(void *));
/* do as osifHookDataPumpIntr does, on the vertical blank's chain */
void osifHookTimerIntr(char hook, void (*func)(void *));
/* return the time since syTmrInit, in 10 ms ticks. syTmrGetCount's count
 * wraps every 92 minutes or so. at each wrap, the result falls back about 343
 * seconds. a call that interrupts another returns the result of the last
 * call to finish */
Uint32 osifGet10msCount(void);
/* store in *time the date and time syRtcGetDate gives. where syRtcGetDate
 * returns other than 0, every field of *time is 0 */
void osifGetSystemTime(struct Sys_Time *time);

#endif
