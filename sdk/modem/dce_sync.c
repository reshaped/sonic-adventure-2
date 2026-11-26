#include <sg_xpt.h>
#include "dce.h"
#include "dce_line.h"
#include "dce_sync.h"
#include "ecdc_def.h"
#include "messages.h"
#include "roc_k56.h"
#include "targdef.h"
#include "target.h"

/* HDLC sends an octet low bit first. after five ones in a row the sender
 * adds a zero. six ones and a zero are then a flag, and seven ones an abort */
#define FLAG_OCTET 0x7e
#define STUFF_AFTER 5
#define FLAG_ONES 6
#define ABORT_ONES 7
/* the transmitter sends a flag in two halves of FLAG_HALF bits. between them
 * it sets ones to STUFF_AFTER, so the flag's six ones take no stuffed zero.
 * HDLC_FLAG tells the halves apart by ones. only the second half ends in a
 * zero, which leaves ones at STUFF_AFTER */
#define FLAG_HALF 4
#define FCS16_POLYNOMIAL 0x8408
#define FCS32_POLYNOMIAL 0xedb88320UL
#define FCS16_RESIDUE 0xf0b8
#define FCS32_RESIDUE 0xdebb20e3UL
/* the most bytes sync_receive takes, or passes async_sync_transmit makes, in
 * one call. a call that would pass it reports its code to data_pump_error */
#define FIFO_BUDGET 500
#define SYNC_ERR_RX 5140
#define SYNC_ERR_TX 5160
/* the bytes the line carries in 100 ms at bps */
#define TENTH_SECOND_BYTES(bps) ((bps) / 80)

/* one direction of the coder. shift holds the octet or FCS in progress. bits
 * counts its bits left to send or receive. sending, ones counts down the ones
 * left before a stuffed zero. receiving, it counts the ones in a row */
struct hdlc_stream {
    Uint8 phase;
    Uint8 bits;
    Uint8 ones;
    Uint16 crc16;
    Uint32 crc32;
    Uint32 shift;
};
static Uint16 block_size;
/* crc_table is the FCS-16 table, and rx_crc16 a word nothing reads. the
 * library exports both */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Uint16 crc_table[256];
Uint16 rx_crc16;
/* NOLINTEND(misc-use-internal-linkage) */
static Uint32 crc32_table[256];
#ifdef NONMATCHING
#else
/* @hack redundant: a word no code reads, between crc32_table and tx */
static Uint32 reserved_crc_word;
#endif
static struct hdlc_stream tx;
static struct hdlc_stream rx;
static Uint8 crc16_ready;
static Uint8 crc32_ready;
static Uint32 rx_overruns;

#define MAKE_TABLE(table, polynomial, ready)                                   \
    if (!(ready)) {                                                            \
        for (i = 0; i <= 255; i++) {                                           \
            data = i;                                                          \
            (table)[i] = 0;                                                    \
            for (bit = 0; bit <= 7; bit++) {                                   \
                low = (table)[i] & 1;                                          \
                (table)[i] >>= 1;                                              \
                if ((data & 1) != low)                                         \
                    (table)[i] ^= (polynomial);                                \
                data >>= 1;                                                    \
            }                                                                  \
        }                                                                      \
        (ready) = 1;                                                           \
    }
static void make_crc16_table(void)
{
    Uint16 i;
    Uint16 bit;
    Uint8 data;
    Uint8 low;
    MAKE_TABLE(crc_table, FCS16_POLYNOMIAL, crc16_ready);
}
static void make_crc32_table(void)
{
    Uint16 i;
    Uint16 bit;
    Uint8 data;
    Uint8 low;
    MAKE_TABLE(crc32_table, FCS32_POLYNOMIAL, crc32_ready);
}

#define UPDATE_FCS(stream, octet)                                              \
    if (crc_length == FCS16_BYTES) {                                           \
        index = (stream).crc16 ^ (octet);                                      \
        (stream).crc16 >>= 8;                                                  \
        (stream).crc16 ^= crc_table[index];                                    \
    } else {                                                                   \
        index = (stream).crc32 ^ (octet);                                      \
        (stream).crc32 >>= 8;                                                  \
        (stream).crc32 ^= crc32_table[index];                                  \
    }
#define BEGIN_FCS(stream)                                                      \
    (stream).crc16 = 0xffff;                                                   \
    (stream).crc32 = 0xffffffffUL

/* @hack redundant: count, result and the copies of tx's fields declared
 * register, where plain ones would do */
Uint32 soft_hdlc_transmit(register Uint8 count)
{
    register Uint8 phase = tx.phase;
    register Uint8 bits = tx.bits;
    register Uint8 ones = tx.ones;
    register Uint32 shift = tx.shift;
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: result starts at 0 */
    register Uint32 result = 0;
#else
    /* @bug uninitialized: the first shift reads result before anything sets
     * it, at 0x0caf7b40. a count under 32 leaves what the register held in
     * the result's low 32 - count bits */
    register Uint32 result;
#endif
    Uint8 index;
    while (count--) {
        result >>= 1;
        if (ones == 0) {
            ones = STUFF_AFTER;
        } else {
            if (shift & 1) {
                ones--;
                result |= 0x80000000UL;
            } else {
                ones = STUFF_AFTER;
            }
            shift >>= 1;
            bits--;
        }
        if (bits) {
            continue;
        }
        switch (phase) {
        case HDLC_DATA:
        next_octet:
            if (tx_dce_buffer_wcnt) {
                shift = *tx_dce_buffer_wptr++;
                tx_dce_buffer_wcnt--;
                bits = 8;
                UPDATE_FCS(tx, shift);
            } else {
                if (crc_length == FCS16_BYTES) {
                    shift = ~tx.crc16;
                    bits = 16;
                } else {
                    shift = ~tx.crc32;
                    bits = 32;
                }
                phase = HDLC_FCS;
            }
            break;
        case HDLC_FCS:
            close_tx_buffer();
            shift = FLAG_OCTET;
            bits = FLAG_HALF;
            phase = HDLC_FLAG;
            break;
        case HDLC_FLAG:
            if (ones != STUFF_AFTER) {
                bits = FLAG_HALF;
                ones = STUFF_AFTER;
                break;
            }
            tx_dce_buffer_wcnt = 0;
            while (dce_transmit_count() && !tx_dce_buffer_wcnt) {
                open_tx_buffer();
                if (!tx_dce_buffer_wcnt) {
                    close_tx_buffer();
                }
            }
            if (tx_dce_buffer_wcnt) {
                BEGIN_FCS(tx);
                phase = HDLC_DATA;
                goto next_octet;
            } else {
                while (count > 8) {
                    result >>= 8;
                    result |= (Uint32)FLAG_OCTET << 24;
                    count -= 8;
                }
                shift = FLAG_OCTET;
                bits = FLAG_HALF;
            }
            break;
        case HDLC_ABORT:
            count = 0;
            result = 0xffffffffUL;
            shift = FLAG_OCTET;
            bits = FLAG_HALF;
            phase = HDLC_FLAG;
            if (tx_buffer_flag != TX_NO_FRAME) {
                close_tx_buffer();
            }
            break;
        }
    }
    tx.phase = phase;
    tx.bits = bits;
    tx.ones = ones;
    tx.shift = shift;
    return result;
}

#define SKIP_FLAGS()                                                           \
    while (count >= 8 && (Uint8)word == FLAG_OCTET) {                          \
        word >>= 8;                                                            \
        count -= 8;                                                            \
    }
void soft_hdlc_receive(Uint32 data, Uint8 length)
{
    /* @hack redundant: count, word, phase, ones and shift declared register,
     * where plain locals would do */
    register Uint8 count = length;
    register Uint32 word = data;
    register Uint8 phase = rx.phase;
    Uint8 bits = rx.bits;
    register Uint8 ones = rx.ones;
    register Uint32 shift = rx.shift;
    Uint8 index;
    if (phase == HDLC_FLAG) {
        SKIP_FLAGS();
    }
    while (count--) {
        if (word & 1) {
            shift = (shift >> 1) | 0x80;
            ones++;
            bits--;
            if (ones == ABORT_ONES) {
                bits = 0;
                phase = HDLC_ABORT;
            }
        } else {
            if (ones < STUFF_AFTER) {
                shift >>= 1;
                bits--;
            } else if (ones == FLAG_ONES) {
                DCE_INAC_TIM = 0;
                bits = 0;
                phase = HDLC_FLAG;
            }
            ones = 0;
        }
        word >>= 1;
        if (bits) {
            continue;
        }
        switch (phase) {
        case HDLC_DATA:
        store_octet:
            UPDATE_FCS(rx, shift);
            *rx_dce_buffer_wptr++ = shift;
            if (++rx_dce_buffer_wcnt > DCE_FRAME_BYTES - 4) {
                dce_rx_buffer[dce_rx_write].status |= DCE_FRAME_TOO_LONG;
                close_rx_buffer();
                phase = HDLC_IDLE;
            }
            break;
        case HDLC_FLAG:
            phase = HDLC_OPEN;
            if (rx_dce_buffer_wcnt > 0) {
                if (rx_dce_buffer_wcnt <= crc_length ||
                    (crc_length == FCS16_BYTES && rx.crc16 != FCS16_RESIDUE) ||
                    (crc_length == FCS32_BYTES && rx.crc32 != FCS32_RESIDUE)) {
                    dce_rx_buffer[dce_rx_write].status |= DCE_FRAME_BAD_CRC;
                    close_rx_buffer();
                    phase = HDLC_IDLE;
                } else {
                    rx_dce_buffer_wcnt -= crc_length;
                    close_rx_buffer();
                }
            }
            SKIP_FLAGS();
            break;
        case HDLC_OPEN:
            open_rx_buffer();
            if (dce_rx_buffer_available) {
                BEGIN_FCS(rx);
                phase = HDLC_DATA;
                goto store_octet;
            }
            break;
        case HDLC_ABORT:
            /* rx.phase holds the phase the call began in, or HDLC_IDLE once
             * an abort has come */
            if (rx.phase != HDLC_IDLE) {
                open_rx_buffer();
                if (dce_rx_buffer_available) {
                    dce_rx_buffer[dce_rx_write].status |= DCE_FRAME_ABORT;
                    close_rx_buffer();
                }
                rx.phase = HDLC_IDLE;
            }
            phase = HDLC_IDLE;
            ones = 0;
            break;
        case HDLC_IDLE:
            break;
        default:
            break;
        }
        shift = 0;
        bits = 8;
    }
    rx.phase = phase;
    rx.bits = bits;
    rx.ones = ones;
    rx.shift = shift;
}

void init_dce_hdlc_rx(void)
{
    rx_dce_buffer_wcnt = 0;
    rx.shift = 0;
    rx.bits = 8;
    rx.ones = 0;
    rx.phase = HDLC_IDLE;
}
void init_dce_hdlc_tx(void)
{
    tx.phase = HDLC_FLAG;
    tx.bits = FLAG_HALF;
    tx.ones = STUFF_AFTER;
    tx.shift = FLAG_OCTET;
    tx_dce_buffer_wcnt = 0;
}
void init_dce_hdlc(void)
{
    dce_clr_tx_buff();
    dce_clr_rx_buff();
    init_dce_hdlc_rx();
    init_dce_hdlc_tx();
    make_crc32_table();
    make_crc16_table();
}

void init_dce_sync(Uint8 speed)
{
    dce_clr_tx_buff();
    dce_clr_rx_buff();
    tx_phase = HDLC_IDLE;
    rx_phase = HDLC_IDLE;
    switch (speed) {
    case TCI_SPEED_14400:
        block_size = TENTH_SECOND_BYTES(14400);
        break;
    case TCI_SPEED_12000:
        block_size = TENTH_SECOND_BYTES(12000);
        break;
    case TCI_SPEED_9600:
        block_size = TENTH_SECOND_BYTES(9600);
        break;
    case TCI_SPEED_7200:
        block_size = TENTH_SECOND_BYTES(7200);
        break;
    case TCI_SPEED_4800:
        block_size = TENTH_SECOND_BYTES(4800);
        break;
    case TCI_SPEED_2400:
        block_size = TENTH_SECOND_BYTES(2400);
        break;
    case TCI_SPEED_300:
        block_size = 20;
        break;
    default:
        block_size = DCE_FRAME_BYTES;
    }
}

void sync_receive(void)
{
    Uint8 byte;
    /* @hack redundant: budget declared register, where a plain local would
     * do */
    register Uint16 budget = FIFO_BUDGET;
    while (dp_get_rx_data_byte(&byte)) {
        if (budget-- == 0) {
            data_pump_error(SYNC_ERR_RX);
            return;
        }
        switch (rx_phase) {
        case HDLC_IDLE:
            open_rx_buffer();
            if (dce_rx_buffer_available == 1) {
                *rx_dce_buffer_wptr++ = byte;
                rx_dce_buffer_wcnt++;
                rx_phase = HDLC_DATA;
            } else {
                rx_overruns++;
            }
            break;
        case HDLC_DATA:
            DCE_INAC_TIM = 0;
            *rx_dce_buffer_wptr++ = byte;
            rx_dce_buffer_wcnt++;
            if (rx_dce_buffer_wcnt == block_size) {
                close_rx_buffer();
                rx_phase = HDLC_IDLE;
            }
            break;
        }
    }
}
void async_sync_transmit(void)
{
    /* @hack redundant: budget declared register, where a plain local would
     * do */
    register Uint16 budget = FIFO_BUDGET;
    char done = 0;
    while (!done) {
        if (budget-- == 0) {
            data_pump_error(SYNC_ERR_TX);
            return;
        }
        switch (tx_phase) {
        case HDLC_IDLE:
            if (dce_transmit_count()) {
                open_tx_buffer();
                if (tx_dce_buffer_wcnt) {
                    tx_phase = HDLC_DATA;
                } else {
                    close_tx_buffer();
                }
            } else {
                if (dp_tx_irqs_enabled()) {
                    dp_disable_tx_irqs();
                }
                done = 1;
            }
            break;
        case HDLC_DATA:
            if (dp_ready_to_tx_data()) {
                dp_tx_data_byte(*tx_dce_buffer_wptr++);
                tx_dce_buffer_wcnt--;
                if (!tx_dce_buffer_wcnt) {
                    close_tx_buffer();
                    tx_phase = HDLC_IDLE;
                }
            } else {
                if (!dp_tx_irqs_enabled()) {
                    dp_enable_tx_irqs();
                }
                done = 1;
            }
            break;
        default:
            done = 1;
        }
    }
}
void dceSetTxPhase(Uint8 phase)
{
    tx.phase = phase;
    tx_phase = phase;
}
