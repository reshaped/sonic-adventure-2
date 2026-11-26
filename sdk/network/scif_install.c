#include <machine.h>
#include <mdmAPI.h>
#include <sg_xpt.h>
#include "scif_hardware.h"
#include "scif_receive.h"
#include "scif_serial.h"

unsigned int scif_speed;
static scif_handler saved_receive;
static scif_handler saved_transmit;
static scif_handler saved_error;
static scif_handler saved_break;
/* the clock SCBRR2 divides, the 50 MHz peripheral clock over 32. SCBRR2
 * takes it over the bit rate, less 1, rounded */
#define SCBRR2_CLOCK 1562500.0F

void scif_install(void)
{
    Sint16 saved_mask;
    volatile int delay;

    saved_mask = get_imask();
    set_imask(IMASK_ALL);
    scif_receive_count = 0;
    scif_transmit_count = 0;
    scif_error_count = 0;
    scif_break_count = 0;
    SCSCR2 = 0;
    SCSPTR2 = 0;
    SCIF_FIFO_CONTROL = SCIF_FIFO_RESET;
    saved_receive = SCIF_VECTOR(SCIF_RX_VECTOR);
    saved_transmit = SCIF_VECTOR(SCIF_TX_VECTOR);
    saved_error = SCIF_VECTOR(SCIF_ERROR_VECTOR);
    saved_break = SCIF_VECTOR(SCIF_BREAK_VECTOR);
    SCIF_VECTOR(SCIF_RX_VECTOR) = scif_receive_interrupt;
    SCIF_VECTOR(SCIF_TX_VECTOR) = scif_transmit_interrupt;
    SCIF_VECTOR(SCIF_ERROR_VECTOR) = scif_error_interrupt;
    SCIF_VECTOR(SCIF_BREAK_VECTOR) = scif_break_interrupt;
    ICR = 0;
    IPRC |= IPRC_SCIF;
    scif_set_speed(MDD_LINESPEED_115200);
    SCIF_FIFO_CONTROL |= SCIF_RX_TRIGGER_8 | SCIF_FLOW_CONTROL;
    scif_readback = SCIF_FIFO_CONTROL;
    SCSCR2 = SCIF_TX_IRQ | SCIF_RX_IRQ | SCIF_TX_ENABLE | SCIF_RX_ENABLE;
    scif_readback = SCSCR2;
    SCIF_FIFO_CONTROL &= ~SCIF_FIFO_RESET;
    scif_readback = SCIF_FIFO_CONTROL;
    for (delay = 0; delay < SCIF_BAUD_SETTLE; delay++) {
    }
    set_imask(saved_mask);
}

void scif_remove(void)
{
    SCSCR2 = 0;
    SCIF_FIFO_CONTROL = 0;
    SCIF_VECTOR(SCIF_RX_VECTOR) = saved_receive;
    SCIF_VECTOR(SCIF_TX_VECTOR) = saved_transmit;
    SCIF_VECTOR(SCIF_ERROR_VECTOR) = saved_error;
    SCIF_VECTOR(SCIF_BREAK_VECTOR) = saved_break;
}

void scif_set_baud(Float rate)
{
    volatile int delay;

    SCBRR2 = (int)((SCBRR2_CLOCK / rate) - 0.5F);
    scif_readback = SCBRR2;
    for (delay = 0; delay < SCIF_BAUD_SETTLE; delay++) {
    }
}

void scif_receive_interrupt(void)
{
    Sint16 status;

    scif_receive_count++;
    status = SCFSR2;
    SCLSR2;
    set_imask(IMASK_ALL);
    scif_drain_receive_fifo();
    if (status & SCIF_RX_TRIGGERED) {
        SCFSR2 &= ~SCIF_RX_TRIGGERED;
        scif_readback = SCFSR2;
    }
    if (status & SCIF_RX_READY) {
        SCFSR2 &= ~SCIF_RX_READY;
        scif_readback = SCFSR2;
    }
}
