/* pacing: the queues that move bytes between the host and the port, at the
 * port's speed */
#ifndef MODEM_PACING_H
#define MODEM_PACING_H

/* point the pacing at its two queues */
void OpenPacing(void);
/* move the host's bytes into the port's receive buffers, as many as the
 * port's speed allows. in command mode, take the host's command bytes
 * instead. glbScc_Pc must be set, and OpenPacing called */
void MainPacingFunc(void);
/* move the port's transmit buffers to the host, as many bytes as the port's
 * speed allows. in command mode, give the host the priority bytes instead.
 * glbScc_Pc must be set, and OpenPacing called */
void SecondPacingFunc(void);

#endif
