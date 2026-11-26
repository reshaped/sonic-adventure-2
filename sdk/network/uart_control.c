#include <sg_chain.h>
#include <sg_xpt.h>
#include "uart_hardware.h"

/* the baud is the UART's clock divided by TICKS_PER_BIT and by the
 * divisor */
#define TICKS_PER_BIT 16.0F

static Float uart_clock_mhz = 4.8F;
/* the receive FIFO's trigger, in the FIFO control register's high four bits.
 * 8 triggers at 8 bytes */
Sint8 uart_fifo_trigger = 8;

void uart_remove(void)
{
    /* nothing reads cause: the read clears a transmit interrupt the UART holds
     * pending */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    volatile Uint8 cause;

    SERIAL_INTERRUPT_ENABLE = 0;
    cause = SERIAL_INTERRUPT_ID;
    SERIAL_FIFO_CONTROL = FCR_RX_RESET | FCR_TX_RESET;
    SERIAL_DELAY();
    SERIAL_LINE_CONTROL = 0;
    SERIAL_DELAY();
    SERIAL_MODEM_CONTROL = 0;
    SERIAL_DELAY();
    SERIAL_LINE_STATUS = 0;
    SERIAL_DELAY();
    syChainDeleteHandler(uart_chain);
}

void uart_set_baud(Float baud)
{
    Float quotient;
    Sint16 divisor;

    SERIAL_LINE_CONTROL = SERIAL_LINE_CONTROL | LCR_DLAB;
    SERIAL_DELAY();
    quotient = uart_clock_mhz * 1000000.0F / (baud * TICKS_PER_BIT);
    /* for a positive baud, adding 0.5 rounds the quotient to the nearest
     * divisor */
    /* NOLINTNEXTLINE(bugprone-incorrect-roundings) */
    divisor = quotient + 0.5F;
    SERIAL_DIVISOR_HIGH = (divisor & 0xff00) >> 8;
    SERIAL_DELAY();
    SERIAL_DIVISOR_LOW = divisor;
    SERIAL_DELAY();
    SERIAL_LINE_CONTROL = SERIAL_LINE_CONTROL & ~LCR_DLAB;
    SERIAL_DELAY();
}

void uart_send_byte(Uint8 byte)
{
    SERIAL_DATA = byte;
    SERIAL_DELAY();
}

Uint8 uart_read_byte(void)
{
    Uint8 byte = SERIAL_DATA;
    SERIAL_DELAY();
    return byte;
}

void uart_enable_interrupts(Uint8 mask)
{
    SERIAL_INTERRUPT_ENABLE |= mask;
    SERIAL_DELAY();
}

void uart_disable_interrupts(Uint8 mask)
{
    SERIAL_INTERRUPT_ENABLE &= ~mask;
    SERIAL_DELAY();
}
