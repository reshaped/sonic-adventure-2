/* scif_drain_receive_fifo moves what the receive FIFO holds into the receive
 * ring, as far as the ring has room. the receive and error interrupts call
 * it. a ring full on entry disables the receive interrupt and sets
 * scif_rx_paused. scif_install gives the error and break interrupts no
 * enable but SCIF_RX_IRQ, so the pause silences them too */
#ifndef NETWORK_SCIF_RECEIVE_H
#define NETWORK_SCIF_RECEIVE_H

#include "scif_hardware.h"
#include "scif_ring.h"
#include "scif_serial.h"

#pragma inline(scif_drain_receive_fifo)
static void scif_drain_receive_fifo(void)
{
    int count;
    int room;
    char byte;
    count = SCIF_RX_COUNT;
    room = scif_rx_space;
    if (room == 0) {
        scif_rx_paused = 1;
        SCSCR2 &= ~SCIF_RX_IRQ;
        scif_readback = SCSCR2;
    }
    while (scif_rx_space > 0) {
        /* @hack spelling: the test of count inside the loop, where the loop's
         * own test would do */
        if (count-- > 0) {
            byte = SCFRDR2;
            scif_put_rx(&byte, 1, &room);
        } else {
            break;
        }
    }
}

#endif
