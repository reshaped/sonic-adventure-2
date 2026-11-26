#include <access/vsyncpro.h>
#include "ppp_system.h"

PPP_SHARED int PP_TaskCount = 0;
PPP_SHARED int PP_TaskTerminateReq = 0;
static int saved_interrupt_level;

void PP_OS_Initialize(void)
{
    PP_TaskCount = 0;
    PP_TaskTerminateReq = 0;
    vsStartPppDaemon();
    while (PP_TaskCount != 1) {
        vsWaitVSync(1);
    }
}
void PP_OS_Terminate(void)
{
    PP_TaskTerminateReq = 1;
    while (PP_TaskCount) {
        vsWaitVSync(1);
    }
    PP_ComTerminate();
    PP_TaskTerminateReq = 0;
}
void PP_OS_CriticalLock(void)
{
    saved_interrupt_level = vsEnterCriticalSection();
}
void PP_OS_CriticalUnLock(void)
{
    vsLeaveCriticalSection(saved_interrupt_level);
}
void PP_OS_Yield(void)
{
    vsPausePppDaemon();
}
void PP_OS_WaitMs(int milliseconds)
{
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: it pauses a frame at a time until
     * milliseconds have passed */
    while (milliseconds > 0) {
        vsPausePppDaemon();
        milliseconds -= VS_TICK_MILLISECONDS;
    }
#else
    int elapsed;
    /* @bug logic: a wait of more than 0 ms returns at once, as ppp_session.c's
     * waits of 2000 and 500 ms do. the loop runs while elapsed exceeds
     * milliseconds, where the reverse is meant (0x0c9e04a6) */
    for (elapsed = 0; elapsed > milliseconds; elapsed += VS_TICK_MILLISECONDS) {
        vsPausePppDaemon();
    }
#endif
}
long PP_OS_GetTime(void)
{
    return (long)((unsigned long)vsGetTickCount() * VS_TICK_MILLISECONDS);
}
void PP_OS_IoWaitSub(void)
{
}
