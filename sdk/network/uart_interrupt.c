#include <machine.h>
#include <sg_xpt.h>
#include "uart_hardware.h"
#include "uart_ring.h"

#define UART_FIFO_BYTES 16

static int line_events;
static volatile int modem_events;
static volatile int receive_events;
static volatile int transmit_events;
static int fifo_errors;
static int tx_idle_count;
static int tx_empty_count;
static int breaks;
static int framing_errors;
static int parity_errors;
static int overruns;
static int data_ready_count;

void uart_interrupt(void *argument)
{
    Sint16 saved_mask;
    volatile char cause;
    volatile char line;
    /* nothing reads modem: reading the modem status register clears the
     * change of a modem line that raised IIR_MODEM */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    volatile char modem;
    char received;
    char bytes[UART_FIFO_BYTES];
    char *cursor;
    int pending;
    volatile int room;
    int count;

    saved_mask = get_imask();
    set_imask(IMASK_ALL);
    SERIAL_DELAY();
    cause = SERIAL_INTERRUPT_ID & IIR_CAUSE;
    while (!(cause & IIR_NONE_PENDING)) {
        SERIAL_DELAY();
        switch (cause) {
        case IIR_LINE:
            line_events++;
            line = SERIAL_LINE_STATUS;
            SERIAL_DELAY();
            if (line & LSR_FIFO_ERROR) {
                fifo_errors++;
            }
            if (line & LSR_TX_IDLE) {
                tx_idle_count++;
            }
            if (line & LSR_TX_EMPTY) {
                tx_empty_count++;
            }
            if (line & LSR_BREAK) {
                breaks++;
            }
            if (line & LSR_FRAMING_ERROR) {
                framing_errors++;
            }
            if (line & LSR_PARITY_ERROR) {
                parity_errors++;
            }
            if (line & LSR_OVERRUN) {
                overruns++;
            }
            if (line & LSR_DATA_READY) {
                data_ready_count++;
            }
            break;
        case IIR_RECEIVE:
        case IIR_TIMEOUT:
            uart_received_interrupts++;
            receive_events++;
            room = uart_rx_space;
            if (room == 0) {
                uart_rx_paused = 1;
                SERIAL_INTERRUPT_ENABLE =
                    SERIAL_INTERRUPT_ENABLE & ~IER_RECEIVE;
                SERIAL_DELAY();
            }
            while (uart_rx_space > 0) {
                if (!(SERIAL_LINE_STATUS & LSR_DATA_READY)) {
                    SERIAL_DELAY();
                    break;
                } else {
                    received = SERIAL_DATA;
                    SERIAL_DELAY();
                    uart_put_rx(&received, 1, &room);
                }
            }
            break;
        case IIR_TRANSMIT:
            uart_transmitted_interrupts++;
            transmit_events++;
            count = uart_take_tx(bytes, UART_FIFO_BYTES, &pending);
            if (count) {
                cursor = &bytes[0];
                while (count-- > 0) {
                    SERIAL_DATA = *cursor++;
                    SERIAL_DELAY();
                }
            }
            if (pending == 0) {
                uart_tx_paused = 1;
                SERIAL_INTERRUPT_ENABLE =
                    SERIAL_INTERRUPT_ENABLE & ~IER_TRANSMIT;
                SERIAL_DELAY();
            }
            break;
        case IIR_MODEM:
            modem_events++;
            modem = SERIAL_MODEM_STATUS;
            SERIAL_DELAY();
            break;
        }
        cause = SERIAL_INTERRUPT_ID & IIR_CAUSE;
    }
    set_imask(saved_mask);
}
