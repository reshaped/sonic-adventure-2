/* the Katana serial console on the SH-4's SCIF. scif_init takes the caller's
 * receive and transmit rings. it sets the SCIF's four interrupt vectors to the
 * console's handlers */
#ifndef SH4SCIF_H
#define SH4SCIF_H

/* scif_open's brr, SCBRR2's divisor for each speed. BPS_AUTO takes SCBRR1's
 * divisor, or 38400 bps's where SCBRR1 holds 0xff */
enum SCI_SPEED {
    BPS_9600 = 0xa2,
    BPS_19200 = 0x50,
    BPS_38400 = 0x28,
    BPS_57600 = 0x1a,
    BPS_115200 = 0x0d,
    BPS_AUTO = -1
};
long scif_init(void *rx, long rx_size, void *tx, long tx_size);
long scif_open(long brr);
long scif_close(void);
void scif_putq(char c);
/* 1 where a received byte waits, or 0. where the receive ring is empty,
 * scif_isget first moves a byte from the SCIF's FIFO into it. scif_get takes
 * a byte off the ring. on an empty ring its result is unset, so a caller
 * tests scif_isget first */
long scif_isget(void);
short scif_get(void);
long scif_isbreak(void);

#endif
