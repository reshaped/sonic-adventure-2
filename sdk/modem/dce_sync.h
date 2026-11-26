/* dce_sync: HDLC framing, done in software on the data pump's bit stream,
 * and synchronous blocks without it */
#ifndef MODEM_DCE_SYNC_H
#define MODEM_DCE_SYNC_H
#include <sg_xpt.h>

/* the phases of the HDLC coder's receiver and transmitter. the synchronous
 * blocks' rx_phase and tx_phase take HDLC_DATA and HDLC_IDLE too */
#define HDLC_DATA 2
#define HDLC_ABORT 4
#define HDLC_FLAG 5
#define HDLC_FCS 6
#define HDLC_OPEN 7
#define HDLC_IDLE 8

/* the values of crc_length, the bytes in a frame's FCS */
#define FCS16_BYTES 2
#define FCS32_BYTES 4

/* the next count bits to send, at most 32, in the result's top count bits.
 * the first to send is the lowest of them */
Uint32 soft_hdlc_transmit(Uint8 count);
/* take the first length bits of data, the first in bit 0 */
void soft_hdlc_receive(Uint32 data, Uint8 length);
void init_dce_hdlc_rx(void);
void init_dce_hdlc_tx(void);
/* empty both rings, and start the coder: its receiver waits for a flag, and
 * its transmitter sends flags */
void init_dce_hdlc(void);
/* empty both rings, and size the receive blocks for speed, a TCI speed code */
void init_dce_sync(Uint8 speed);
void sync_receive(void);
void async_sync_transmit(void);
/* set both transmitters' phase. HDLC_ABORT ends the frame being sent */
void dceSetTxPhase(Uint8 phase);

#endif
