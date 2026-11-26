/* the SCIF's transmit, error and break interrupts. each handler masks every
 * interrupt and never unmasks them. the RTE that ends the interrupt puts back
 * the SR it interrupted, mask and all */
#include <machine.h>
#include <sg_xpt.h>
#include "scif_hardware.h"
#include "scif_receive.h"
#include "scif_serial.h"
#include "scif_ring.h"

int scif_receive_count, scif_transmit_count, scif_error_count, scif_break_count;
int scif_line_breaks, scif_overruns;
/* where a read of a SCIF register just written lands. the read holds the CPU
 * until the write has reached the register. nothing reads scif_readback */
volatile char scif_readback;

void scif_transmit_interrupt(void)
{
    char bytes[SCIF_FIFO_BYTES];
    char *cursor;
    int count;
    int pending;

    set_imask(IMASK_ALL);
    scif_transmit_count++;
    if (SCFSR2 & SCIF_TX_EMPTY) {
        count = (SCIF_FIFO_BYTES - 1) - SCIF_TX_COUNT;
        count = scif_take_tx(bytes, count, &pending);
        if (count != 0) {
            cursor = bytes;
            while (count-- > 0) {
                SCFTDR2 = *cursor++;
                scif_readback = SCFTDR2;
            }
        }
        if (pending == 0) {
            scif_tx_paused = 1;
            SCSCR2 &= ~SCIF_TX_IRQ;
            scif_readback = SCSCR2;
        }
        SCFSR2 &= ~SCIF_TX_EMPTY;
        scif_readback = SCFSR2;
        if (SCFSR2 & SCIF_TX_END) {
            SCFSR2 &= ~SCIF_TX_END;
            scif_readback = SCFSR2;
        }
    }
}

void scif_error_interrupt(void)
{
    Sint16 status;

    scif_error_count++;
    set_imask(IMASK_ALL);
    status = SCFSR2;
    SCLSR2;
    if (status & SCIF_RX_ERROR) {
        SCFSR2 &= ~SCIF_RX_ERROR;
    }
    if (status & SCIF_RX_READY) {
        scif_drain_receive_fifo();
        SCFSR2 &= ~SCIF_RX_READY;
        scif_readback = SCFSR2;
    }
}

void scif_break_interrupt(void)
{
    Sint16 line_status;
    Sint16 status;

    scif_break_count++;
    set_imask(IMASK_ALL);
    status = SCFSR2;
    line_status = SCLSR2;
    if (line_status & SCIF_OVERRUN) {
        scif_overruns++;
        SCLSR2 &= ~SCIF_OVERRUN;
        scif_readback = SCLSR2;
    }
    if (status & SCIF_BREAK) {
        scif_line_breaks++;
        SCFSR2 &= ~SCIF_BREAK;
        scif_readback = SCFSR2;
        SCFRDR2;
    }
}
