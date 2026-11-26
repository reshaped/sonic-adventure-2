/* the PPP and TCP/IP daemons' bodies, which the vertical-blank interrupt
 * runs, each in a context of its own */
#ifndef NETWORK_DAEMONS_H
#define NETWORK_DAEMONS_H

void PP_TimerTask(void);
void TcpIntervalTimerTaskFunc(void);
#endif
