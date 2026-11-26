/* the modem API's driver for the SCIF. the receive interrupt disables
 * itself on a full receive ring and sets scif_rx_paused. the transmit
 * interrupt disables itself once it empties the transmit ring and sets
 * scif_tx_paused. scif_modem_read and scif_modem_write enable each again */
#include <machine.h>
#include <mdmAPI.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include "modem_backends.h"
#include "modem_baud.h"
#include "scif_hardware.h"
#include "scif_ring.h"
#include "scif_serial.h"

/* the longest scif_hang_up waits for a reply after its first ATH, after +++
 * and after its last ATH */
#define FIRST_ATH_WAIT_MICROSECONDS 500000
#define ESCAPE_WAIT_MICROSECONDS 3000000
#define LAST_ATH_WAIT_MICROSECONDS 2000000
/* the silence before +++ that a modem needs to take it as its escape */
#define GUARD_MILLISECONDS 2000
/* the handle scif_modem_open returns */
#define SCIF_PORT 0

volatile Bool scif_tx_paused;
volatile Bool scif_rx_paused;
/* where a read of SCSCR2 or SCSMR2 just written lands. nothing reads
 * scif_control_readback */
static volatile int scif_control_readback;
static int scif_dtr;
static MDS_DCB scif_settings;
static Sint8 scif_opened;
static int scif_read_calls;
static int scif_write_calls;

static void scif_hang_up(MDM handle);

#ifdef NONMATCHING
/* this side defines and calls the entry with its handle, as
 * scif_modem_set_default, the plain form the hack tag below gives */
#define SET_DEFAULT(handle, state) scif_modem_set_default(handle, state)
#define SET_DEFAULT_ENTRY void scif_modem_set_default(MDM handle, MDS_DCB state)
#else
/* @hack type: the set_default entry defined and called without its handle,
 * as scif_apply_default, where scif_modem_set_default would do */
#define SET_DEFAULT(handle, state) scif_apply_default(state)
#define SET_DEFAULT_ENTRY void scif_apply_default(MDS_DCB state)
#endif

static void scif_delay_ms(unsigned int milliseconds)
{
    Uint32 start = syTmrGetCount();
    while (syTmrCountToMicro(syTmrDiffCount(start, syTmrGetCount())) <=
           milliseconds * 1000) {
    }
}

void scif_modem_drop_dtr(MDM handle)
{
    scif_modem_get_state(handle, &scif_settings);
    scif_settings.Dtr = MDD_MCR_DTR_OFF;
    scif_modem_set_state(handle, scif_settings);
}

void scif_modem_enable_dtr(MDM handle)
{
    scif_modem_get_state(handle, &scif_settings);
    scif_settings.Dtr = MDD_MCR_DTR_ON;
    scif_modem_set_state(handle, scif_settings);
}

int scif_modem_rx_available(MDM handle)
{
    return scif_rx_capacity - scif_rx_space;
}

int scif_modem_rx_free(MDM handle)
{
    return scif_rx_space;
}

int scif_modem_tx_available(MDM handle)
{
    return scif_tx_capacity - scif_tx_space;
}

int scif_modem_tx_free(MDM handle)
{
    return scif_tx_space;
}

void scif_modem_init(char *tx, char *rx, int tx_size, int rx_size)
{
    Sint16 saved_mask = get_imask();
    set_imask(IMASK_ALL);
    scif_tx_paused = 0;
    scif_rx_paused = 0;
    scif_tx_buffer = tx;
    scif_tx_capacity = tx_size;
    scif_rx_buffer = rx;
    scif_rx_capacity = rx_size;
    scif_settings.Dtr = MDD_MCR_DTR_ON;
    scif_dtr = MDD_MCR_DTR_ON;
    scif_settings.Rts = MDD_MCR_RTS_ON;
    scif_settings.CharLen = MDD_LCR_CHARLEN_8;
    scif_settings.Parity = MDD_LCR_PARITY_NONE;
    scif_settings.LineSpeed = MDD_LINESPEED_115200;
    scif_settings.Cts = 0;
    scif_settings.Dsr = 0;
    scif_settings.Ri = 0;
    scif_settings.Dcd = 0;
    scif_settings.Brk = 0;
    scif_opened = 0;
    set_imask(saved_mask);
}

MDM scif_modem_open(MD_DEVICE device)
{
    Sint16 saved_mask;
    Uint32 i;
    MDM result;
    if (scif_opened != 1) {
        saved_mask = get_imask();
        set_imask(IMASK_ALL);
        scif_tx_reader = scif_tx_writer = scif_tx_buffer;
        scif_tx_space = scif_tx_capacity;
        scif_rx_reader = scif_rx_writer = scif_rx_buffer;
        scif_rx_space = scif_rx_capacity;
        for (i = 0; i < scif_tx_capacity; i++) {
            scif_tx_buffer[i] = 0;
        }
        for (i = 0; i < scif_rx_capacity; i++) {
            scif_rx_buffer[i] = 0;
        }
        scif_install();
        scif_opened = 1;

        SET_DEFAULT(0, scif_settings);
        result = SCIF_PORT;
        scif_read_calls = scif_write_calls = 0;
        set_imask(saved_mask);
    } else {
        result = -1;
    }
    return result;
}

int scif_modem_read(MDM handle, char *buffer, int size)
{
    Sint16 saved_mask;
    int remaining = size;
    scif_read_calls++;
    if (scif_opened) {
        saved_mask = get_imask();
        set_imask(IMASK_ALL);
        while (scif_rx_capacity - scif_rx_space > 0 && remaining > 0) {
            *buffer++ = *scif_rx_reader++;
            scif_rx_space++;
            remaining--;
            if (scif_rx_reader >= scif_rx_buffer + scif_rx_capacity) {
                scif_rx_reader = scif_rx_buffer;
            }
        }
        if (scif_rx_paused == 1 && size > 0) {
            scif_rx_paused = 0;
            SCSCR2 |= SCIF_RX_IRQ;
            scif_control_readback = SCSCR2;
        }
        set_imask(saved_mask);
        return size - remaining;
    }
    return 0;
}

int scif_modem_write(MDM handle, char *buffer, int size)
{
    Sint16 saved_mask;
    int remaining = size;
    scif_write_calls++;
    if (scif_opened) {
        saved_mask = get_imask();
        set_imask(IMASK_ALL);
        while (scif_tx_space > 0 && remaining > 0) {
            *scif_tx_writer++ = *buffer++;
            scif_tx_space--;
            remaining--;
            if (scif_tx_writer >= scif_tx_buffer + scif_tx_capacity) {
                scif_tx_writer = scif_tx_buffer;
            }
        }
        if (scif_tx_paused == 1 && size > 0) {
            scif_tx_paused = 0;
            SCSCR2 |= SCIF_TX_IRQ;
            scif_control_readback = SCSCR2;
        }
        set_imask(saved_mask);
        return size - remaining;
    }
    return 0;
}

#pragma inline(scif_write_line_format)
static void scif_write_line_format(const MDS_DCB *settings)
{
    switch (settings->CharLen) {
    case MDD_LCR_CHARLEN_8:
        SCIF_MODE &= ~SCIF_CHAR_7;
        break;
    case MDD_LCR_CHARLEN_7:
        SCIF_MODE |= SCIF_CHAR_7;
        break;
    }
    scif_control_readback = SCIF_MODE;
    switch (settings->StopBit) {
    case MDD_LCR_STOPBIT_1B:
        SCIF_MODE &= ~SCIF_STOP_2;
        break;
    case MDD_LCR_STOPBIT_2B:
        SCIF_MODE |= SCIF_STOP_2;
        break;
    }
    scif_control_readback = SCIF_MODE;
    switch (settings->Parity) {
    case MDD_LCR_PARITY_NONE:
        SCIF_MODE &= ~SCIF_PARITY;
        break;
    case MDD_LCR_PARITY_EVEN:
        SCIF_MODE |= SCIF_PARITY;
        SCIF_MODE &= ~SCIF_PARITY_ODD;
        break;
    case MDD_LCR_PARITY_ODD:
        SCIF_MODE |= SCIF_PARITY;
        SCIF_MODE |= SCIF_PARITY_ODD;
        scif_control_readback = SCIF_MODE;
        break;
    }
    scif_control_readback = SCIF_MODE;
}

/* write state's format and speed. the SCIF has no DTR line, so turning DTR
 * off hangs the modem up through scif_hang_up */
/* @hack spelling: the body in both entries, by macro, where an inline
 * function both call with the state's address would do */
#define APPLY_LINE(state, handle)                                              \
    Sint16 saved_mask;                                                         \
    MDS_DCB *settings;                                                         \
    if (scif_opened) {                                                         \
        saved_mask = get_imask();                                              \
        set_imask(IMASK_ALL);                                                  \
        settings = &(state);                                                   \
        scif_write_line_format(settings);                                      \
        if (settings->Dtr == MDD_MCR_DTR_OFF) {                                \
            if (scif_dtr == MDD_MCR_DTR_ON) {                                  \
                scif_hang_up(handle);                                          \
                scif_dtr = MDD_MCR_DTR_OFF;                                    \
            }                                                                  \
        } else {                                                               \
            scif_dtr = MDD_MCR_DTR_ON;                                         \
        }                                                                      \
        scif_set_speed(settings->LineSpeed);                                   \
        set_imask(saved_mask);                                                 \
    }

SET_DEFAULT_ENTRY
{
    /* @bug memory: turning DTR off here hangs up through handle 2, at
     * 0x0c9e904c. the purges then read devices[2], past mdmapi.c's two ports,
     * at 0x0c9e3102 and 0x0c9e318a */
    APPLY_LINE(state, 2)
}

void scif_modem_set_state(MDM handle, MDS_DCB state)
{
    APPLY_LINE(state, handle)
}

void scif_set_speed(unsigned int speed)
{
    scif_speed = speed;
    MODEM_BAUD_RATE(speed)
    scif_set_baud(speed);
}

/* @hack spelling: the body in both entries, by macro, where an inline
 * function both call would do */
#define READ_LINE(state)                                                       \
    Sint16 saved_mask = get_imask();                                           \
    char mode;                                                                 \
    set_imask(IMASK_ALL);                                                      \
    (state)->Ri = 1;                                                           \
    (state)->Cts = 1;                                                          \
    (state)->Dsr = 1;                                                          \
    (state)->Dtr = MDD_MCR_DTR_ON;                                             \
    (state)->Rts = MDD_MCR_RTS_ON;                                             \
    mode = SCIF_MODE;                                                          \
    if (mode & SCIF_CHAR_7)                                                    \
        (state)->CharLen = MDD_LCR_CHARLEN_7;                                  \
    else                                                                       \
        (state)->CharLen = MDD_LCR_CHARLEN_8;                                  \
    if (!(mode & SCIF_PARITY))                                                 \
        (state)->Parity = MDD_LCR_PARITY_NONE;                                 \
    else if (!(mode & SCIF_PARITY_ODD))                                        \
        (state)->Parity = MDD_LCR_PARITY_EVEN;                                 \
    else                                                                       \
        (state)->Parity = MDD_LCR_PARITY_ODD;                                  \
    if (!(mode & SCIF_STOP_2))                                                 \
        (state)->StopBit = MDD_LCR_STOPBIT_1B;                                 \
    else                                                                       \
        (state)->StopBit = MDD_LCR_STOPBIT_2B;                                 \
    /* @bug logic: a received break goes in the control field SetBreak, at     \
     * 0x0c9e92a4 and 0x0c9e9376. the status field Brk gets 0, at              \
     * 0x0c9e92b8 and 0x0c9e938a */                                            \
    (state)->SetBreak = (SCFSR2 & SCIF_BREAK) >> 4;                            \
    (state)->LineSpeed = scif_speed;                                           \
    (state)->Dcd = 1;                                                          \
    (state)->Dtr = scif_dtr;                                                   \
    (state)->Brk = 0;                                                          \
    set_imask(saved_mask);

void scif_modem_get_state(MDM handle, MDS_DCB *state)
{
    READ_LINE(state)
}
void scif_modem_get_default(MDS_DCB *state)
{
    READ_LINE(state)
}

void scif_modem_purge_tx(MDM handle)
{
    Sint16 saved_mask;
    /* @hack spelling: get_imask inside the test of scif_opened, where a return
     * before it would do */
    if (scif_opened) {
        saved_mask = get_imask();
    } else {
        return;
    }
    set_imask(IMASK_ALL);
    scif_tx_reader = scif_tx_writer = scif_tx_buffer;
    scif_tx_space = scif_tx_capacity;
    set_imask(saved_mask);
}

void scif_modem_purge_rx(MDM handle)
{
    Sint16 saved_mask;
    /* @hack spelling: get_imask inside the test of scif_opened, where a return
     * before it would do */
    if (scif_opened) {
        saved_mask = get_imask();
    } else {
        return;
    }
    set_imask(IMASK_ALL);
    scif_rx_reader = scif_rx_writer = scif_rx_buffer;
    scif_rx_space = scif_rx_capacity;
    set_imask(saved_mask);
}

void scif_modem_close(MDM handle)
{
    if (scif_opened) {
        scif_opened = 0;
        scif_hang_up(handle);
        scif_remove();
        scif_tx_paused = 0;
        scif_rx_paused = 0;
    }
}

void scif_modem_finish(void)
{
}

/* put byte in the transmit FIFO, past the ring, and read it back into the
 * caller's readback */
#define WRITE_AT_BYTE(byte)                                                    \
    SCFTDR2 = (byte);                                                          \
    readback = SCFTDR2
/* ATH, CR and LF, each by WRITE_AT_BYTE */
#define WRITE_HANG_UP()                                                        \
    WRITE_AT_BYTE('A');                                                        \
    WRITE_AT_BYTE('T');                                                        \
    WRITE_AT_BYTE('H');                                                        \
    WRITE_AT_BYTE('\r');                                                       \
    WRITE_AT_BYTE('\n')
/* shift the receive FIFO's next byte into response, if the FIFO holds one.
 * response keeps the last four bytes received */
#define POLL_RESPONSE()                                                        \
    if (SCIF_RX_COUNT != 0) {                                                  \
        response[0] = response[1];                                             \
        response[1] = response[2];                                             \
        response[2] = response[3];                                             \
        response[3] = SCFRDR2;                                                 \
    }

/* hang the modem up with ATH. where no OK or ER answers in time, send the
 * escape +++ and ATH again. it works the FIFOs past the rings, with every
 * interrupt masked, for as long as 7.5 seconds */
static void scif_hang_up(MDM handle)
{
    Sint16 saved_mask;

    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    volatile char readback;
    char response[4];
    Uint32 start;
    Uint32 elapsed;
    saved_mask = get_imask();
    set_imask(IMASK_ALL);
    mdmPurgeTxBuffer(handle);
    mdmPurgeRxBuffer(handle);
    WRITE_HANG_UP();
    response[0] = response[1] = response[2] = response[3] = 0;
    start = syTmrGetCount();
    while (syTmrCountToMicro(syTmrDiffCount(start, syTmrGetCount())) <
           FIRST_ATH_WAIT_MICROSECONDS) {
        POLL_RESPONSE()
        if (response[3] == '\n') {
            if (response[0] == 'O' && response[1] == 'K' &&
                response[2] == '\r') {
                set_imask(saved_mask);
                return;
            }
            /* @bug logic: the ER test never holds for an ERROR reply, which
             * ends OR CR LF: here at 0x0c9e955a, and in the last wait at
             * 0x0c9e96dc */
            if (response[0] == 'E' && response[1] == 'R' &&
                response[2] == '\r') {
                set_imask(saved_mask);
                return;
            }
        }
    }
    scif_delay_ms(GUARD_MILLISECONDS);
    WRITE_AT_BYTE('+');
    WRITE_AT_BYTE('+');
    WRITE_AT_BYTE('+');
    response[0] = response[1] = response[2] = response[3] = 0;
    start = syTmrGetCount();
    while ((elapsed = syTmrCountToMicro(syTmrDiffCount(
                start, syTmrGetCount()))) < ESCAPE_WAIT_MICROSECONDS) {
        POLL_RESPONSE()
        if (response[3] == '\n') {
            if (response[0] == 'O' && response[1] == 'K' &&
                response[2] == '\r') {
                WRITE_HANG_UP();
                break;
            }
        }
    }
    if (elapsed >= ESCAPE_WAIT_MICROSECONDS) {
        WRITE_HANG_UP();
    }
    response[0] = response[1] = response[2] = response[3] = 0;
    start = syTmrGetCount();
    while (syTmrCountToMicro(syTmrDiffCount(start, syTmrGetCount())) <
           LAST_ATH_WAIT_MICROSECONDS) {
        POLL_RESPONSE()
        if (response[3] == '\n') {
            if (response[0] == 'O' && response[1] == 'K' &&
                response[2] == '\r') {
                break;
            }
            if (response[0] == 'E' && response[1] == 'R' &&
                response[2] == '\r') {
                break;
            }
        }
    }
    set_imask(saved_mask);
}
