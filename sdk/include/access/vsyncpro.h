/* the PPP and TCP/IP daemons, which the vertical-blank interrupt runs, each
 * in a context of its own */
#ifndef ACCESS_VSYNCPRO_H
#define ACCESS_VSYNCPRO_H
/* vsWaitVSync waits a frame by calling callback, or njWaitVSync where
 * callback is NULL */
void vsInitVSyncProc(void (*callback)(void));
void vsFinishVSyncProc(void);
void vsWaitVSync(int count);
void vsStartPppDaemon(void);
void vsStopPppDaemon(void);
void vsStartTcpipDaemon(void);
void vsStopTcpipDaemon(void);
/* raise the interrupt mask to 6, the vertical blank's level, where it is lower.
 * neither daemon runs while it stays there. it returns the mask as it was, for
 * vsLeaveCriticalSection to restore */
int vsEnterCriticalSection(void);
void vsLeaveCriticalSection(int savedLevel);
void vsPausePppDaemon(void);
void vsPauseTcpipDaemon(void);
/* vsGetTickCount counts vertical blanks. its callers take each as
 * VS_TICK_MILLISECONDS */
#define VS_TICK_MILLISECONDS 16
int vsGetTickCount(void);
#endif
