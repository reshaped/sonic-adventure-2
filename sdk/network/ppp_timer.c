#include <sg_xpt.h>
#include "daemons.h"
#include "ppp_system.h"

enum { TIMER_STOPPED, TIMER_RUNNING, TIMER_EXPIRED };
static struct ppp_timer *timers;
static int arp_timer_state, arp_timeout;

void PP_start_timer(struct ppp_timer *timer)
{
    if (timer) {
        if (timer->state == TIMER_RUNNING) {
            PP_stop_timer(timer);
        }
        if (timer->interval) {
            timer->timeout = timer->interval;
            /* @hack redundant: a test of timeout, which the test of interval
             * above keeps from 0 */
            if (!timer->timeout) {
                timer->timeout = 1;
            }
            timer->started = PP_GetTimeLow();
            if (timer->callback) {
                timer->state = TIMER_RUNNING;
            }
        }
    }
}
void PP_stop_timer(struct ppp_timer *timer)
{
    if (timer && timer->state == TIMER_RUNNING) {
        timer->state = TIMER_STOPPED;
    }
}
void PP_set_timer(struct ppp_timer *timer, Uint32 interval)
{
    struct ppp_timer *entry;
    if (timer) {
        timer->interval = interval;
        if (timers) {
            entry = timers;
            while (entry->next) {
                if (entry == timer) {
                    timer->state = TIMER_STOPPED;
                    return;
                }
                entry = entry->next;
            }
            entry->next = timer;
            timer->next = NULL;
        } else {
            timers = timer;
            timer->next = NULL;
        }
        timer->state = TIMER_STOPPED;
    }
}
void PP_timerproc(void)
{
    struct ppp_timer *timer;
    Uint32 now;

    PP_OS_CriticalLock();
#ifdef NONMATCHING
    /* this side calls ppp_recv once, the plain form the hack tag below gives */
    ppp_recv();
#else
    /* @hack redundant: a loop no pass enters, of PP_OS_CriticalUnLock,
     * _PP_delay and PP_OS_CriticalLock, where the call alone would do */
    while (ppp_recv() && 0) {
        PP_OS_CriticalUnLock();
        _PP_delay(1);
        PP_OS_CriticalLock();
    }
#endif
    now = PP_GetTimeLow();
    for (timer = timers; timer; timer = timer->next) {
        if (timer->state == TIMER_RUNNING &&
            now - timer->started > timer->timeout) {
            PP_stop_timer(timer);
            timer->state = TIMER_EXPIRED;
            if (timer->callback) {
                timer->state = TIMER_STOPPED;
                timer->callback(timer->argument);
            }
        }
    }
    PP_OS_CriticalUnLock();
}
void PP_TimerTask(void)
{
    ++PP_TaskCount;
    while (!PP_TaskTerminateReq) {
        if (PPP_CloseRequest) {
            PP_OS_CriticalLock();
            ppp_close_sub();
            PP_OS_CriticalUnLock();
        } else if (PPP_OpenRequest) {
            PP_OS_CriticalLock();
            ppp_open_sub();
            PP_OS_CriticalUnLock();
        } else if (PPP_Executing) {
            PP_OS_Yield();
            PP_timerproc();
        } else {
            PP_OS_Yield();
        }
    }
    --PP_TaskCount;
    for (;;) {
        PP_OS_Yield();
    }
}
short PP_TimerDriverInitialize(void)
{
    timers = NULL;
    arp_timer_state = 1;
    arp_timeout = 0;
    return 0;
}
void PP_start_arp_timer(void)
{
    arp_timer_state = 0;
    arp_timeout = 100;
}
void PP_stop_arp_timer(void)
{
    arp_timer_state = 0;
}
long PP_GetTimeLow(void)
{
    return PP_OS_GetTime();
}
