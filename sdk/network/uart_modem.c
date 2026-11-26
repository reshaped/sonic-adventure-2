/* the modem API's driver for the expansion UART. the receive interrupt
 * disables itself on a full receive ring and sets uart_rx_paused. the
 * transmit interrupt disables itself once it empties the transmit ring and
 * sets uart_tx_paused. uart_modem_read and uart_modem_write enable each
 * again */
#include <machine.h>
#include <mdmAPI.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "modem_backends.h"
#include "modem_baud.h"
#include "uart_hardware.h"
#include "uart_ring.h"

/* how long uart_modem_select holds SERIAL_RESET low to reset the UART */
#define RESET_MICROSECONDS 1000
/* the handle uart_modem_open returns, which mdmapi.c names DEFAULT_PORT */
#define UART_PORT 1

volatile Bool uart_tx_paused;
volatile Bool uart_rx_paused;
static MDS_DCB uart_settings;
static Sint8 uart_opened;
static int uart_read_calls;
static int uart_write_calls;

#ifdef NONMATCHING
/* this side defines and calls the entry with its handle, as
 * uart_modem_set_default, the plain form the hack tag below gives */
#define SET_DEFAULT(handle, state) uart_modem_set_default(handle, state)
#define SET_DEFAULT_ENTRY void uart_modem_set_default(MDM handle, MDS_DCB state)
#else
/* @hack type: the set_default entry defined and called without its handle,
 * as uart_apply_default, where uart_modem_set_default would do */
#define SET_DEFAULT(handle, state) uart_apply_default(state)
#define SET_DEFAULT_ENTRY void uart_apply_default(MDS_DCB state)
#endif

void uart_modem_drop_dtr(MDM handle)
{
    uart_modem_get_default(&uart_settings);
    uart_settings.Dtr = MDD_MCR_DTR_OFF;
    SET_DEFAULT(handle, uart_settings);
}

void uart_modem_enable_dtr(MDM handle)
{
    uart_modem_get_default(&uart_settings);
    uart_settings.Dtr = MDD_MCR_DTR_ON;
    SET_DEFAULT(handle, uart_settings);
}

int uart_modem_rx_available(MDM handle)
{
    return uart_rx_capacity - uart_rx_space;
}

int uart_modem_rx_free(MDM handle)
{
    return uart_rx_space;
}

int uart_modem_tx_available(MDM handle)
{
    return uart_tx_capacity - uart_tx_space;
}

int uart_modem_tx_free(MDM handle)
{
    return uart_tx_space;
}

void uart_modem_init(char *tx, char *rx, int tx_size, int rx_size)
{
    Sint16 saved_mask;
    saved_mask = get_imask();
    set_imask(IMASK_ALL);
    uart_tx_paused = 0;
    uart_rx_paused = 0;
    uart_tx_buffer = tx;
    uart_tx_capacity = tx_size;
    uart_rx_buffer = rx;
    uart_rx_capacity = rx_size;
    uart_settings.Dtr = MDD_MCR_DTR_ON;
    uart_settings.Rts = MDD_MCR_RTS_ON;
    uart_settings.CharLen = MDD_LCR_CHARLEN_8;
    uart_settings.Parity = MDD_LCR_PARITY_NONE;
    uart_settings.LineSpeed = MDD_LINESPEED_38400;
    uart_settings.Cts = 0;
    uart_settings.Dsr = 0;
    uart_settings.Ri = 0;
    uart_settings.Dcd = 0;
    uart_settings.Brk = 0;
    uart_opened = 0;
    set_imask(saved_mask);
}

MDM uart_modem_open(MD_DEVICE device)
{
    Sint16 saved_mask;
    Uint32 i;
    MDM result;
    if (uart_opened != 1) {
        saved_mask = get_imask();
        set_imask(IMASK_ALL);
        uart_tx_reader = uart_tx_writer = uart_tx_buffer;
        uart_tx_space = uart_tx_capacity;
        uart_rx_reader = uart_rx_writer = uart_rx_buffer;
        uart_rx_space = uart_rx_capacity;
        for (i = 0; i < uart_tx_capacity; i++) {
            uart_tx_buffer[i] = 0;
        }
        for (i = 0; i < uart_rx_capacity; i++) {
            uart_rx_buffer[i] = 0;
        }
        uart_install();
        if ((SERIAL_PRESENT & PRESENT_UART) == PRESENT_UART) {
            uart_opened = 1;
            SET_DEFAULT(0, uart_settings);
            result = UART_PORT;
        } else {
            uart_opened = 0;
            result = -1;
        }
        uart_read_calls = uart_write_calls = 0;
        set_imask(saved_mask);
    } else {
        result = -1;
    }
    return result;
}

int uart_modem_read(MDM handle, char *buffer, int size)
{
    Sint16 saved_mask;
    int remaining = size;
    uart_read_calls++;
    if (uart_opened) {
        saved_mask = get_imask();
        set_imask(IMASK_ALL);
        while (uart_rx_capacity - uart_rx_space > 0 && remaining > 0) {
            *buffer++ = *uart_rx_reader++;
            uart_rx_space++;
            remaining--;
            if (uart_rx_reader >= uart_rx_buffer + uart_rx_capacity) {
                uart_rx_reader = uart_rx_buffer;
            }
        }
        if (uart_rx_paused == 1 && size > 0) {
            uart_rx_paused = 0;
            SERIAL_INTERRUPT_ENABLE = SERIAL_INTERRUPT_ENABLE | IER_RECEIVE;
            SERIAL_DELAY();
        }
        set_imask(saved_mask);
        return size - remaining;
    }
    return 0;
}

int uart_modem_write(MDM handle, char *buffer, int size)
{
    Sint16 saved_mask;
    int remaining = size;
    uart_write_calls++;
    if (uart_opened) {
        saved_mask = get_imask();
        set_imask(IMASK_ALL);
        while (uart_tx_space > 0 && remaining > 0) {
            *uart_tx_writer++ = *buffer++;
            uart_tx_space--;
            remaining--;
            if (uart_tx_writer >= uart_tx_buffer + uart_tx_capacity) {
                uart_tx_writer = uart_tx_buffer;
            }
        }
        if (uart_tx_paused == 1 && size > 0) {
            uart_tx_paused = 0;
            SERIAL_INTERRUPT_ENABLE = SERIAL_INTERRUPT_ENABLE | IER_TRANSMIT;
            SERIAL_DELAY();
        }
        set_imask(saved_mask);
        return size - remaining;
    }
    return 0;
}

/* write state's DTR, format and break, and turn RTS on whatever state.Rts
 * says. the speed stays as it was: nothing here reads state.LineSpeed */
/* @hack spelling: the body in both entries, by macro, where an inline
 * function both call with the state's address would do */
#define APPLY_LINE(state)                                                      \
    Sint16 saved_mask;                                                         \
    if (uart_opened) {                                                         \
        saved_mask = get_imask();                                              \
        set_imask(IMASK_ALL);                                                  \
        SERIAL_MODEM_CONTROL =                                                 \
            REPLACE_BITS(SERIAL_MODEM_CONTROL, MCR_DTR, (state).Dtr);          \
        SERIAL_DELAY();                                                        \
        SERIAL_MODEM_CONTROL =                                                 \
            REPLACE_BITS(SERIAL_MODEM_CONTROL, MCR_RTS, MCR_RTS);              \
        SERIAL_DELAY();                                                        \
        SERIAL_LINE_CONTROL =                                                  \
            REPLACE_BITS(SERIAL_LINE_CONTROL, LCR_CHARLEN, (state).CharLen);   \
        SERIAL_DELAY();                                                        \
        SERIAL_LINE_CONTROL =                                                  \
            REPLACE_BITS(SERIAL_LINE_CONTROL, LCR_STOPBIT, (state).StopBit);   \
        SERIAL_DELAY();                                                        \
        SERIAL_LINE_CONTROL =                                                  \
            REPLACE_BITS(SERIAL_LINE_CONTROL, LCR_PARITY, (state).Parity);     \
        SERIAL_DELAY();                                                        \
        SERIAL_LINE_CONTROL =                                                  \
            REPLACE_BITS(SERIAL_LINE_CONTROL, LCR_BREAK, (state).SetBreak);    \
        SERIAL_DELAY();                                                        \
        set_imask(saved_mask);                                                 \
    }

SET_DEFAULT_ENTRY
{
    APPLY_LINE(state)
}

void uart_modem_set_state(MDM handle, MDS_DCB state)
{
    APPLY_LINE(state)
}

void uart_set_speed(int baud_code)
{
    Uint32 rate = baud_code;
    uart_speed = baud_code;
    MODEM_BAUD_RATE(rate)
    uart_set_baud(rate);
}

#pragma inline(uart_modem_get_state)
void uart_modem_get_state(MDM handle, MDS_DCB *state)
{
    Sint16 saved_mask;
    saved_mask = get_imask();
    set_imask(IMASK_ALL);
    state->Dtr = SERIAL_MODEM_CONTROL & MCR_DTR;
    state->Rts = SERIAL_MODEM_CONTROL & MCR_RTS;
    state->CharLen = SERIAL_LINE_CONTROL & LCR_CHARLEN;
    state->Parity = SERIAL_LINE_CONTROL & LCR_PARITY;
    state->StopBit = SERIAL_LINE_CONTROL & LCR_STOPBIT;
    state->SetBreak = SERIAL_LINE_CONTROL & LCR_BREAK;
    state->LineSpeed = uart_speed;
    state->Dcd = (SERIAL_MODEM_STATUS & MSR_DCD) >> MSR_DCD_SHIFT;
    state->Dsr = (SERIAL_MODEM_STATUS & MSR_DSR) >> MSR_DSR_SHIFT;
    state->Ri = (SERIAL_MODEM_STATUS & MSR_RI) >> MSR_RI_SHIFT;
    state->Cts = (SERIAL_MODEM_STATUS & MSR_CTS) >> MSR_CTS_SHIFT;
    state->Brk = (SERIAL_LINE_STATUS & LSR_BREAK) >> LSR_BREAK_SHIFT;
    set_imask(saved_mask);
}

void uart_modem_get_default(MDS_DCB *state)
{
    uart_modem_get_state(0, state);
}

void uart_modem_purge_tx(MDM handle)
{
    Sint16 saved_mask;
    /* @hack spelling: get_imask inside the test of uart_opened, where a return
     * before it would do */
    if (uart_opened) {
        saved_mask = get_imask();
    } else {
        return;
    }
    set_imask(IMASK_ALL);
    uart_tx_reader = uart_tx_writer = uart_tx_buffer;
    uart_tx_space = uart_tx_capacity;
    SERIAL_FIFO_CONTROL =
        (uart_fifo_trigger << FCR_TRIGGER_SHIFT) | FCR_ENABLE | FCR_TX_RESET;
    set_imask(saved_mask);
}

void uart_modem_purge_rx(MDM handle)
{
    Sint16 saved_mask;
    /* @hack spelling: get_imask inside the test of uart_opened, where a return
     * before it would do */
    if (uart_opened) {
        saved_mask = get_imask();
    } else {
        return;
    }
    set_imask(IMASK_ALL);
    uart_rx_reader = uart_rx_writer = uart_rx_buffer;
    uart_rx_space = uart_rx_capacity;
    SERIAL_FIFO_CONTROL =
        (uart_fifo_trigger << FCR_TRIGGER_SHIFT) | FCR_ENABLE | FCR_RX_RESET;
    set_imask(saved_mask);
}

void uart_modem_close(MDM handle)
{
    uart_remove();
    uart_tx_paused = 0;
    uart_rx_paused = 0;
    if (uart_opened) {
        uart_opened = 0;
    }
}

void uart_modem_finish(void)
{
}
void uart_modem_service(void)
{
}

void uart_modem_select(MD_DEVICE device)
{
    Uint32 start;
    SERIAL_RESET = 0;
    /* @hack spelling: the wait as a for loop, where a while loop would do */
    for (start = syTmrGetCount();
         syTmrCountToMicro(syTmrDiffCount(start, syTmrGetCount())) <
         RESET_MICROSECONDS;) {
    }
    SERIAL_RESET = 1;
}
