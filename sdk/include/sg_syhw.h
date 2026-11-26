/* Shinobi hardware initialization, as far as the sources use it */
#ifndef SG_SYHW_H
#define SG_SYHW_H

/* syHwInit sets the interrupt controller, the serial port and the system
 * bus to their starting values. it then starts the handling of exceptions
 * and interrupts, the store queues, the caches and the timer. syHwInit2
 * starts the interrupt chains, the G2 bus library and the expansion check.
 * syHwFinish ends the expansion check and the store queues, and turns both
 * caches off. every interrupt is masked while syHwFinish runs */
void syHwInit(void);
void syHwInit2(void);
void syHwFinish(void);

#endif
