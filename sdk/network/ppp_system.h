/* the services the ACCESS PPP library takes from its platform: timers,
 * locks, yields and the daemon's flags */
#ifndef NETWORK_PPP_SYSTEM_H
#define NETWORK_PPP_SYSTEM_H
#include <sg_xpt.h>

struct ppp_timer {
    struct ppp_timer *next;
    Uint32 interval, timeout, started;
    void (*callback)(void *);
    void *argument;
    Uint8 state;
};

#ifdef NONMATCHING
/* this side marks the five volatile, since the PPP daemon changes them while a
 * caller waits on them */
#define PPP_SHARED volatile
#else
#define PPP_SHARED
#endif
extern PPP_SHARED int PP_TaskCount, PP_TaskTerminateReq;
extern PPP_SHARED int PPP_OpenRequest, PPP_CloseRequest, PPP_Executing;

void PP_OS_Initialize(void);
void PP_OS_Terminate(void);
void PP_OS_CriticalLock(void);
void PP_OS_CriticalUnLock(void);
void PP_OS_Yield(void);
void PP_OS_WaitMs(int milliseconds);
long PP_OS_GetTime(void);
void PP_OS_IoWaitSub(void);
void PP_ComTerminate(void);
int PP_main(int argc, char **argv);
int ppp_restart(void);
short ppp_init(int port);
int ppp_recv(void);
int ppp_open_sub(void);
short ppp_close_sub(void);
void PP_start_timer(struct ppp_timer *timer);
void PP_stop_timer(struct ppp_timer *timer);
void PP_set_timer(struct ppp_timer *timer, Uint32 interval);
void PP_timerproc(void);
short PP_TimerDriverInitialize(void);
void PP_start_arp_timer(void);
void PP_stop_arp_timer(void);
long PP_GetTimeLow(void);
long PP_Clock(void);
long PP_secclock(void);
void _PP_srandom(void);
short _PP_random(void);
/* compare at most count bytes of left and right, ignoring the case of ASCII
 * letters. it returns the difference of the first bytes that differ, as
 * capitals, or 0. unlike strnicmp, it returns 1 where either string ends
 * within count bytes, even where both end alike */
int PP_strnicmp(const unsigned char *left, const unsigned char *right,
                short count);
void _PP_delay(unsigned short milliseconds);
#endif
