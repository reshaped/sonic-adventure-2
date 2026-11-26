#include <machine.h>
#include <mdmAPI.h>
#include <sg_chain.h>
#include <sg_xpt.h>
#include "uart_hardware.h"

/* uart_interrupt's priority on the modem's interrupt chain */
#define CHAIN_PRIORITY 16

SYCHAIN uart_chain;
int uart_speed;
int uart_received_interrupts, uart_transmitted_interrupts;
static Uint32 uart_context;

void uart_install(void)
{
    Sint16 saved_mask;

    saved_mask = get_imask();
    set_imask(IMASK_ALL);
    SERIAL_LINE_CONTROL = LCR_DLAB;
    SERIAL_DELAY();
    uart_speed = MDD_LINESPEED_38400;
    uart_set_speed(MDD_LINESPEED_38400);
    SERIAL_LINE_CONTROL =
        MDD_LCR_CHARLEN_8 | MDD_LCR_STOPBIT_1B | MDD_LCR_PARITY_NONE;
    SERIAL_DELAY();
    SERIAL_MODEM_CONTROL = MCR_DTR | MCR_RTS | MCR_AUTO_FLOW;
    SERIAL_DELAY();
    SERIAL_FIFO_CONTROL = (uart_fifo_trigger << FCR_TRIGGER_SHIFT) | FCR_ENABLE;
    SERIAL_DELAY();
    SERIAL_INTERRUPT_ENABLE = IER_RECEIVE | IER_TRANSMIT | IER_LINE | IER_MODEM;
    SERIAL_DELAY();
    uart_chain = syChainAddHandler(SYD_CHAIN_EVENT_IML4_MODEM, uart_interrupt,
                                   CHAIN_PRIORITY, &uart_context);
    uart_received_interrupts = 0;
    uart_transmitted_interrupts = 0;
    set_imask(saved_mask);
}
