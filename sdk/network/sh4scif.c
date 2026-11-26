/* the Katana serial console on the SCIF, apart from the modem API's driver.
 * scif_init points the SCIF's four vectors at its handlers, as scif_install
 * points them at the driver's. nothing here points them back */
#include <machine.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <sh4scif.h>

#include "scif_hardware.h"

/* the received bytes at which SCIF_RX_TRIGGER_14 raises the receive
 * interrupt */
#define RX_TRIGGER_BYTES 14
/* SCBRR1's value out of reset, before anything sets a divisor in it */
#define RESET_DIVISOR 0xff

/* last_event's values. RX_RING_FULL says the receive ring filled. each
 * CLEARED_ value names the bit of SCFSR2 or SCLSR2 that a handler cleared
 * last */
enum {
    RX_RING_FULL = -1,
    CLEARED_OVERRUN = -2,
    CLEARED_RX_READY = -3,
    CLEARED_RX_ERROR = -4,
    CLEARED_RX_TRIGGERED = -5,
    CLEARED_BREAK = -6
};

static Sint32 receive_interrupts;
static Sint32 transmit_interrupts;
static Sint32 error_interrupts;
static Sint32 break_interrupts;
static Sint32 overrun_count;
static Sint32 framing_count;
static Sint32 parity_count;
static Sint32 break_count;
/* the rings: the caller's buffers, each with a write and a read index. a ring
 * is empty where its indices meet. the receive ring is full when it holds
 * rx_capacity - 1 bytes. scif_putq writes the FIFO itself, so nothing fills
 * the transmit ring */
static char *rx_buffer;
static Sint32 rx_capacity;
static Sint32 rx_write;
static Sint32 rx_read;
static char *tx_buffer;
static Sint32 tx_capacity;
static Sint32 tx_write;
static Sint32 tx_read;
static Sint32 last_event;
static Bool break_seen;
static Sint32 baud_divisor;

static void scif_debug_receive(void);
static void scif_debug_transmit(void);
static void scif_debug_error(void);
static void scif_debug_break(void);

long scif_init(void *rx, long rx_size, void *tx, long tx_size)
{
    overrun_count = parity_count = framing_count = break_count =
        receive_interrupts = error_interrupts = break_interrupts = 0;
    rx_buffer = rx;
    rx_capacity = rx_size;
    rx_write = 0;
    rx_read = 0;
    tx_buffer = tx;
    tx_capacity = tx_size;
    tx_write = 0;
    tx_read = 0;
    last_event = 0;
    break_seen = FALSE;
    SCSCR2 = 0;
    SCSPTR2 = 0;
    SCFCR2 = SCIF_FIFO_RESET;
    SCIF_VECTOR(SCIF_RX_VECTOR) = scif_debug_receive;
    SCIF_VECTOR(SCIF_TX_VECTOR) = scif_debug_transmit;
    SCIF_VECTOR(SCIF_ERROR_VECTOR) = scif_debug_error;
    SCIF_VECTOR(SCIF_BREAK_VECTOR) = scif_debug_break;
    ICR = 0;
    IPRC |= IPRC_SCIF;
    return 0;
}

long scif_open(long brr)
{
    volatile int i;

    SCFCR2 = SCIF_FIFO_RESET;
    SCSMR2 = 0;
    if (brr <= 0) {
        brr = SCBRR1;
        if (brr == RESET_DIVISOR) {
            brr = BPS_38400;
        }
    }
    SCBRR2 = brr;
    for (i = 0; i < SCIF_BAUD_SETTLE; i++) {
    }
    SCFCR2 = SCIF_RX_TRIGGER_14 | SCIF_FLOW_CONTROL;
    SCSCR2 = SCIF_TX_IRQ | SCIF_RX_IRQ | SCIF_TX_ENABLE | SCIF_RX_ENABLE;
    baud_divisor = brr;
    for (i = 0; i < SCIF_BAUD_SETTLE; i++) {
    }
    return 0;
}

long scif_close(void)
{
    SCSCR2 = 0;
    SCFCR2 = 0;
    return 0;
}

void scif_putq(char c)
{
    while ((SCFDR2 >> 8) >= SCIF_FIFO_BYTES) {
    }
    SCFTDR2 = c;
    SCFSR2 &= ~SCIF_TX_EMPTY;
}

short scif_get(void)
{
    Uint16 saved_mask;
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: an empty ring gives 0 */
    char c = 0;
#else
    /* @bug uninitialized: scif_get returns c unset from an empty ring, whose
     * branch at 0x0c9ee3b6 skips to the return at 0x0c9ee400 */
    char c;
#endif

    saved_mask = get_imask();
    set_imask(IMASK_ALL);
    /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
    if (rx_read != rx_write) {
        if ((rx_write + 1) % rx_capacity == rx_read) {
            SCSCR2 |= SCIF_RX_IRQ;
        }
        c = rx_buffer[rx_read++];
        rx_read %= rx_capacity;
    }
    set_imask(saved_mask);

    return c;
}

long scif_isget(void)
{
    Uint16 saved_mask;
    int available;

    saved_mask = get_imask();
    set_imask(IMASK_ALL);
    if (rx_read == rx_write) {
        if (SCIF_RX_COUNT == 0) {
            available = 0;
            /* @hack spelling: the goto past available = 1, where an else
             * would do */
            goto done;
        }
        rx_buffer[rx_write++] = SCFRDR2;
        rx_write = rx_write % rx_capacity;
    }
    available = 1;
done:
    set_imask(saved_mask);
    return available;
}

long scif_isbreak(void)
{
    /* @bug logic: scif_isbreak returns 0 whatever breaks arrive, since no
     * handler sets break_seen, which it reads at 0x0c9ee46e. scif_debug_break
     * records a break in last_event alone, at 0x0c9ee7e0 */
    Bool seen = break_seen;

    break_seen = FALSE;
    return seen;
}

static void scif_debug_receive(void)
{
    Sint16 status;
    Sint16 line;
    int count;
    int i;
    char c;

    receive_interrupts++;
    status = SCFSR2;
    line = SCLSR2;
    if (status & SCIF_RX_READY) {
        count = SCIF_RX_COUNT;
    } else {
        count = RX_TRIGGER_BYTES;
    }
    for (i = 0; i < count; i++) {
        if ((rx_write + 1) % rx_capacity == rx_read) {
            last_event = RX_RING_FULL;
            SCSCR2 &= ~SCIF_RX_IRQ;
            return;
        } else {
            c = SCFRDR2;
            rx_buffer[rx_write++] = c;
            rx_write %= rx_capacity;
        }
    }
    if (line & SCIF_OVERRUN) {
        SCLSR2 &= ~SCIF_OVERRUN;
        last_event = CLEARED_OVERRUN;
    }
    if (status & SCIF_RX_READY) {
        SCFSR2 &= ~SCIF_RX_READY;
        last_event = CLEARED_RX_READY;
    }
    if (status & SCIF_RX_ERROR) {
        SCFSR2 &= ~SCIF_RX_ERROR;
        last_event = CLEARED_RX_ERROR;
    }
    if (status & SCIF_BREAK) {
        SCFSR2 &= ~SCIF_BREAK;
        last_event = CLEARED_BREAK;
    }
    if (status & SCIF_RX_TRIGGERED) {
        SCFSR2 &= ~SCIF_RX_TRIGGERED;
        last_event = CLEARED_RX_TRIGGERED;
    }
}

static void scif_debug_transmit(void)
{
    int i;

    transmit_interrupts++;
    for (i = 0; i < SCIF_FIFO_BYTES - 1; i++) {
        if (tx_read != tx_write) {
            SCFTDR2 = tx_buffer[tx_read++];
            tx_read %= tx_capacity;
        } else {
            SCSCR2 &= ~SCIF_TX_IRQ;
            break;
        }
    }
    SCFSR2 &= ~SCIF_TX_EMPTY;
}

static void scif_debug_error(void)
{
    Sint16 status;
    Sint16 line;
    int count;
    char c;

    error_interrupts++;
    syIntEnableInt(IMASK_ALL);
    status = SCFSR2;
    line = SCLSR2;
    if (line & SCIF_OVERRUN) {
        overrun_count++;
        SCLSR2 &= ~SCIF_OVERRUN;
        last_event = CLEARED_OVERRUN;
    }
    if (status & SCIF_RX_ERROR) {
        if (status & SCIF_FRAMING_ERROR) {
            framing_count++;
        }
        if (status & SCIF_PARITY_ERROR) {
            parity_count++;
        }
        SCFSR2 &= ~SCIF_RX_ERROR;
        last_event = CLEARED_RX_ERROR;
    }
    if (status & SCIF_BREAK) {
        break_count++;
        SCFSR2 &= ~SCIF_BREAK;
        last_event = CLEARED_BREAK;
    }
    for (count = SCIF_RX_COUNT; count > 0; count--) {
        if ((rx_write + 1) % rx_capacity == rx_read) {
            last_event = RX_RING_FULL;
            SCSCR2 &= ~SCIF_RX_IRQ;
            break;
        } else {
            c = SCFRDR2;
            rx_buffer[rx_write++] = c;
            rx_write %= rx_capacity;
        }
    }
}

static void scif_debug_break(void)
{
    Sint16 status;
    Sint16 line;

    break_interrupts++;
    syIntEnableInt(IMASK_ALL);
    status = SCFSR2;
    line = SCLSR2;
    if (line & SCIF_OVERRUN) {
        SCLSR2 &= ~SCIF_OVERRUN;
        last_event = CLEARED_OVERRUN;
    }
    if (status & SCIF_RX_ERROR) {
        SCFSR2 &= ~SCIF_RX_ERROR;
        last_event = CLEARED_RX_ERROR;
    }
    if (status & SCIF_BREAK) {
        SCFSR2 &= ~SCIF_BREAK;
        last_event = CLEARED_BREAK;
        SCFRDR2;
    }
}
