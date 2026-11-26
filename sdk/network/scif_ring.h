/* the SCIF's transmit and receive rings. the modem API fills the transmit
 * ring and drains the receive ring. the SCIF's interrupts drain the transmit
 * ring and fill the receive ring */
#ifndef NETWORK_SCIF_RING_H
#define NETWORK_SCIF_RING_H

#include <sg_xpt.h>

extern Uint32 scif_tx_capacity, scif_rx_capacity;
extern char *scif_tx_buffer, *scif_tx_reader, *scif_tx_writer;
extern int scif_tx_space;
extern char *scif_rx_buffer, *scif_rx_reader, *scif_rx_writer;
extern int scif_rx_space;

/* scif_take_tx copies up to size queued bytes into buffer, and sets *pending
 * to the bytes still queued. scif_put_rx copies up to size bytes of buffer
 * into the receive ring, and sets *rejected to the rest. each masks every
 * interrupt while it copies, and returns how many bytes it copied.
 * scif_tx_complete and scif_rx_complete do nothing */
int scif_take_tx(char *buffer, int size, int *pending);
int scif_put_rx(char *buffer, int size, volatile int *rejected);
void scif_tx_complete(void);
void scif_rx_complete(void);

#endif
