#include <sg_xpt.h>
#include "dce.h"
#include "dce_line.h"
#include "dte_vxd.h"
#include "ecdc_def.h"
#include "ecdcutil.h"
#include "messages.h"
#include "modemdef.h"
#include "roc_k56.h"
#include "targdef.h"
#include "target.h"

/* a call's bits can end inside a character. so between calls, tx and rx keep
 * the phase, the bits left in it and the character */
#define V14_START 0 /* the start bit, a 0 */
#define V14_STOP 1 /* the stop bits, and the marks of an idle line after */
#define V14_DATA 2 /* the data bits, the lowest first */
#define V14_BREAK 3 /* the 0 bits of a break */
#define V14_IDLE 8 /* the receiver's wait for a start bit */
/* a mark, a 1 on the line, in a word's top bit, where each bit enters */
#define V14_MARK 0x80000000UL
/* in setup_timer's 10 ms ticks */
#define V14_IDLE_TICKS 10

/* the types of line v14 treats apart. DIRECT's types pass each character
 * through dte_vxd's direct-mode hooks, which are empty here. types 22 to 25
 * take no XON or XOFF. types 22 to 24 keep the caller's callbacks.
 * dce_set_interface passes none of 22 to 25 */
#define DIRECT(type) ((type) == DCE_DIRECT_1 || (type) == DCE_DIRECT_2)
/* DCE_LOOPBACK drops a stop bit every DELETE_EVERY characters while
 * DELETE_AT_FRAMES frames or more wait to go. a line of type DCE_DIRECT_2 or
 * 24 sets DELETE_EVERY_SHORT instead */
#define DELETE_EVERY 8
#define DELETE_EVERY_SHORT 4
#define DELETE_AT_FRAMES 3

/* the formats v14 treats apart. FORMAT_7_BITS sends 7 data bits, and every
 * other format 8. FORMAT_MNP, for MNP's asynchronous blocks, closes a frame
 * at DCE_FRAME_BYTES. FORMAT_DETECTION, for V.42's detection phase, sends
 * DETECTION_STOPS stop bits a character */
#define FORMAT_MNP 0xfe
#define FORMAT_DETECTION 0xff
#define DETECTION_STOPS 9

/* under FLOW_NONE, v14 passes XON and XOFF from the line as data. under any
 * other flow_mode, XOFF stops the transmitter and XON starts it. the
 * receiver then drops both from the data, but under FLOW_KEEP */
#define FLOW_NONE 0
#define FLOW_KEEP 4

/* the marks the receiver counts on an idle line before it closes its frame */
#define IDLE_MARKS_SLOW 11 /* up to TCI_SPEED_1200 */
#define IDLE_MARKS 55 /* up to TCI_SPEED_38400 */
#define IDLE_MARKS_FAST 242 /* faster */

/* a start bit after this many null characters, with no mark since, begins a
 * break */
#define BREAK_NULLS 2

/* async_receive takes at most FIFO_BUDGET steps a call. past them it reports
 * DP_ERR_ASYNC_RECEIVE */
#define FIFO_BUDGET 500
#define DP_ERR_ASYNC_RECEIVE 6100

struct v14_tx_state {
    Uint8 phase;
    Uint8 bits_left;
    Uint8 character;
};
struct v14_rx_state {
    Uint8 phase;
    Uint8 bits_left;
    Uint32 character;
};
Uint16 MRBLR_v14;
static struct v14_tx_state tx;
static struct v14_rx_state rx;
#ifdef NONMATCHING
#else
/* @hack redundant: unused_10, this object's bss at that offset, which
 * nothing reads or writes */
static Uint8 unused_10;
#endif
static Uint8 line_type;
static Uint8 flow_mode;
static Uint8 omit_stop;
static Uint8 stops_left;
static Uint8 idle_threshold;
/* SHC places uninitialized variables in the order it first meets their
 * names. so v14.h, which declares the callbacks, comes after the statics
 * that lie ahead of them */
#include "v14.h"
void (*OpenRxBuffer)(void);
void (*CloseRxBuffer)(void);
void (*OpenTxBuffer)(void);
void (*CloseTxBuffer)(void);
Uint8 (*RxBufferLow)(void);
static Uint32 last_rx_tick;

/* FILL_MARKS shifts bits marks into the top of word. POSITION_CHARACTER is
 * character's low bits bits, moved to the top of a word */
#ifdef NONMATCHING
/* where bits is 32, this side gives all marks, as SHC's shifts do. C leaves a
 * shift by 32 undefined */
static Uint32 fill_marks(Uint32 word, Uint8 bits)
{
    if (bits == 0) {
        return word;
    }
    if (bits == 32) {
        return 0xffffffffUL;
    }
    return (word >> bits) | (0xffffffffUL << (32 - bits));
}
#define FILL_MARKS(word, bits) (word) = fill_marks(word, bits)
#define POSITION_CHARACTER(character, bits)                                    \
    ((Uint32)(character) << (32 - (bits)))
#else
#define FILL_MARKS(word, bits)                                                 \
    (word) >>= (bits);                                                         \
    (word) = (word) | (0xffffffffUL << (32 - (bits)))
/* @hack type: character shifted as the int it promotes to, where a shift of
 * (Uint32)character would do */
#define POSITION_CHARACTER(character, bits) ((character) << (32 - (bits)))
#endif

/* @hack redundant: the register on bits and on each local */
Uint32 v14_transmit(register Uint8 bits)
{
    register Uint32 word = 0;
    register Uint8 phase = tx.phase;
    register Uint8 remaining = tx.bits_left;
    register Uint8 character = tx.character;

    while (bits) {
        switch (phase) {
        case V14_START:
            word = word >> 1;
            --bits;
            remaining = dce_n_bit_per_char;
            phase = V14_DATA;
            break;
        case V14_STOP:
            if (omit_stop == 0) {
                word = word >> 1;
                word = word | V14_MARK;
                --bits;
            } else {
                omit_stop = 0;
                n_char_tx_since_last_removing = 0;
            }
            --stops_left;
            if (stops_left != 0) {
                break;
            }
            stops_left = tx_dce_nstop;
            if (tx_break_flag == TX_BREAK_DUE) {
                phase = V14_BREAK;
                break;
            }
            switch (line_type) {
            case 22:
            case 23:
            case 24:
            case 25:
                break;
            default:
                if (rx_dce_flow_flag) {
                    if (rx_dce_flow == 0) {
                        character = dce_xoff;
                    } else {
                        character = dce_xon;
                    }
                    rx_dce_flow_flag = 0;
                    phase = V14_START;
                }
            }
            if (tx_dce_flow == 1 || flow_mode == FLOW_NONE) {
                if (tx_dce_buffer_wcnt) {
                    character = *tx_dce_buffer_wptr++;
                    --tx_dce_buffer_wcnt;
                    ++n_char_tx_since_last_removing;
                    phase = V14_START;
                    if (line_type == DCE_LOOPBACK) {
                        if (dce_transmit_count() >= DELETE_AT_FRAMES) {
                            if (n_char_tx_since_last_removing >=
                                number_of_char_between_stop_deletion) {
                                omit_stop = 1;
                            }
                        }
                    }
                    break;
                }
                if (DIRECT(line_type)) {
                    /* @hack type: the hook's int cast to char, where the int
                     * would do */
                    if ((char)vxd_dte_async_rx_direct_mode(&tx.character,
                                                           &tx.phase)) {
                        phase = tx.phase;
                        character = tx.character;
                        break;
                    }
                } else {
                    if (tx_buffer_flag == TX_FRAME_OPEN) {
                        CloseTxBuffer();
                        tx_buffer_flag = TX_NO_FRAME;
                    }
                    OpenTxBuffer();
                    if (tx_buffer_flag == TX_FRAME_BREAK) {
                        phase = V14_BREAK;
                        break;
#ifdef NONMATCHING
#else
                        /* @hack redundant: a second break, which nothing
                         * reaches */
                        break;
#endif
                    } else if (tx_dce_buffer_wcnt) {
                        character = *tx_dce_buffer_wptr++;
                        --tx_dce_buffer_wcnt;
                        phase = V14_START;
                        break;
                    }
                }
                FILL_MARKS(word, bits);
                bits = 0;
            } else if (tx_dce_flow == 0) {
                FILL_MARKS(word, bits);
                bits = 0;
            }
            break;
        case V14_DATA:
            if (bits >= remaining) {
                word >>= remaining;
                word = word | POSITION_CHARACTER(character, remaining);
                bits -= remaining;
                phase = V14_STOP;
            } else {
                word >>= bits;
                word = word | POSITION_CHARACTER(character, bits);
                remaining -= bits;
                character >>= bits;
                bits = 0;
            }
            break;
        case V14_BREAK:
            word >>= 1;
            --bits;
            if (timer_expired(tx_break_timer)) {
                tx_break_flag = TX_NO_BREAK;
                phase = V14_STOP;
            }
            break;
        }
    }
    tx.phase = phase;
    tx.bits_left = remaining;
    tx.character = character;
    return word;
}

/* @hack redundant: the register on bits and on each local */
void v14_receive(Uint32 input, register Uint8 bits)
{
    register Uint32 word;
    register Uint8 phase = rx.phase;
    register Uint8 remaining = rx.bits_left;
    register Uint32 character = rx.character;
    register Uint8 skip;

    word = input;
    while (bits) {
        switch (phase) {
        case V14_IDLE:
            if (!(word & 1)) {
                if (rx_break_flag == BREAK_NULLS) {
                    phase = V14_BREAK;
                } else {
                    DCE_INAC_TIM = 0;
                    remaining = dce_n_bit_per_char;
                    character = 0;
                    phase = V14_DATA;
                    rx_n_stop_bit = 0;
                    if (!rx_dce_buffer_wcnt) {
                        OpenRxBuffer();
                    }
                }
                word = word >> 1;
                --bits;
            } else {
                skip = (Uint8)word == 0xff ? 8 : 1;
#ifdef NONMATCHING
                /* this side counts no more stop bits than the word has left */
                if (skip > bits) {
                    skip = bits;
                }
#endif
                rx_break_flag = 0;
                rx_n_stop_bit += skip;
                if (rx_n_stop_bit >= idle_threshold) {
                    /* @hack type: the callback's Uint8 cast to char, where
                     * the Uint8 would do */
                    if (!rx_dce_flow && (char)RxBufferLow()) {
                        rx_dce_flow = 1;
                        rx_dce_flow_flag = 1;
                    }
                    if (DIRECT(line_type)) {
                        vxd_dte_dce_rx_idle();
                    } else if (rx_dce_buffer_wcnt) {
                        CloseRxBuffer();
                    }
#ifdef NONMATCHING
#else
                    /* @hack redundant: an empty else */
                    else {
                    }
#endif
                }
                word = word >> skip;
                bits -= skip;
            }
            break;
        case V14_DATA:
            if (remaining > bits) {
                character >>= bits;
                character |= word << (dce_n_bit_per_char - bits);
                remaining -= bits;
                bits = 0;
            } else {
                character >>= remaining;
                character |= word << (dce_n_bit_per_char - remaining);
                word >>= remaining;
                character &= 0xff;
                bits -= remaining;
                if (DIRECT(line_type)) {
                    vxd_dte_async_tx_direct_mode(character);
                    phase = V14_IDLE;
                    break;
                }
                if (!character) {
                    ++rx_break_flag;
                    rx_break_timer = TIM_10MS;
                } else {
                    rx_break_flag = 0;
                }
                if (dce_rx_buffer_available) {
                    *rx_dce_buffer_wptr++ = character;
                    ++rx_dce_buffer_wcnt;
                }
                switch (line_type) {
                case 22:
                case 23:
                case 24:
                case 25:
                    break;
                default:
                    if (flow_mode != FLOW_NONE &&
                        (character == dce_xoff || character == dce_xon)) {
                        if (character == dce_xoff) {
                            tx_dce_flow = 0;
                        } else {
                            tx_dce_flow = 1;
                        }
#ifdef NONMATCHING
#else
                        /* @hack redundant: a store to tx_dce_flow that never
                         * runs */
                        if (0) {
                            tx_dce_flow = 0;
                        }
#endif
                        if (flow_mode != FLOW_KEEP) {
                            if (dce_rx_buffer_available) {
                                --rx_dce_buffer_wptr;
                                --rx_dce_buffer_wcnt;
                            }
                        }
                    }
                    break;
                }
                if (rx_dce_buffer_wcnt >= MRBLR_v14) {
                    CloseRxBuffer();
                }
                phase = V14_IDLE;
            }
            break;
        case V14_BREAK:
            if (word & 1) {
#ifdef NONMATCHING
#else
                /* @hack redundant: an empty test of the ticks since the
                 * last null character */
                if ((Uint16)(TIM_10MS - rx_break_timer)) {
                }
#endif
                if (rx_dce_buffer_wcnt) {
                    CloseRxBuffer();
                }
                rx_break_flag = 0;
                phase = V14_IDLE;
            }
            word = word >> 1;
            --bits;
        }
    }
    rx.phase = phase;
    rx.bits_left = remaining;
    rx.character = character;
}

void v14InitializeTx(void)
{
    tx.phase = V14_STOP;
    tx_phase = V14_STOP;
    tx_dce_flow = 1;
    tx_buffer_flag = TX_NO_FRAME;
    tx_break_flag = TX_NO_BREAK;
    stops_left = tx_dce_nstop;
    tx_dce_buffer_wcnt = 0;
    n_char_tx_since_last_removing = 0;
    omit_stop = 0;
    tx.character = 0xff;
    tx.bits_left = 0;
}
void v14InitializeRx(void)
{
    rx.phase = V14_IDLE;
    rx_dce_buffer_wcnt = 0;
    rx_dce_flow_flag = 0;
    rx_dce_flow = 1;
    rx_break_flag = 0;
}

void v14Initialize(Uint8 type, Uint8 speed, Uint8 format, Uint8 flow)
{
    line_type = type;
    flow_mode = flow;
    tx_dce_nstop = 1;
    v14InitializeTx();
    v14InitializeRx();
    if (format == FORMAT_7_BITS) {
        dce_n_bit_per_char = 7;
    } else {
        dce_n_bit_per_char = 8;
    }
    if (line_type == DCE_DIRECT_2 || line_type == 24) {
        number_of_char_between_stop_deletion = DELETE_EVERY_SHORT;
    } else {
        number_of_char_between_stop_deletion = DELETE_EVERY;
    }
    if (speed <= TCI_SPEED_1200) {
        idle_threshold = IDLE_MARKS_SLOW;
    } else if (speed <= TCI_SPEED_38400) {
        idle_threshold = IDLE_MARKS;
    } else {
        idle_threshold = IDLE_MARKS_FAST;
    }
    if (format == FORMAT_MNP) {
        MRBLR_v14 = DCE_FRAME_BYTES;
    } else if (format == FORMAT_DETECTION) {
        tx_dce_nstop = DETECTION_STOPS;
        /* the line's bit/s over 200, at least 6 and at most 128 */
        switch (speed) {
        case 0:
        case 1:
        case 2:
            MRBLR_v14 = 6;
            break;
        case 3:
            MRBLR_v14 = 12;
            break;
        case 4:
            MRBLR_v14 = 24;
            break;
        case 5:
            MRBLR_v14 = 36;
            break;
        case 6:
            MRBLR_v14 = 48;
            break;
        case 7:
            MRBLR_v14 = 60;
            break;
        case 8:
            MRBLR_v14 = 72;
            break;
        case 9:
            MRBLR_v14 = 84;
            break;
        case 10:
            MRBLR_v14 = 96;
            break;
        case 11:
            MRBLR_v14 = 108;
            break;
        case 12:
            MRBLR_v14 = 120;
            break;
        default:
            MRBLR_v14 = 128;
            break;
        }
    } else {
        MRBLR_v14 = 128;
    }
    switch (line_type) {
    case 22:
    case 23:
    case 24:
        break;
    default:
        OpenRxBuffer = open_rx_buffer;
        CloseRxBuffer = close_rx_buffer;
        OpenTxBuffer = open_tx_buffer;
        CloseTxBuffer = close_tx_buffer;
        RxBufferLow = rx_buffer_low;
        break;
    }
}

void async_receive(void)
{
    /* @hack redundant: the register on budget */
    register Uint16 budget = FIFO_BUDGET;
    char done = 0;
    Uint8 character;
    while (!done) {
        if (!budget--) {
            data_pump_error(DP_ERR_ASYNC_RECEIVE);
            return;
        }
        switch (rx.phase) {
        case V14_IDLE:
            open_rx_buffer();
            rx.phase = V14_DATA;
            break;
        case V14_DATA:
            if (dp_get_rx_data_byte(&character)) {
                DCE_INAC_TIM = 0;
                if (dce_rx_buffer_available == 1) {
                    *rx_dce_buffer_wptr++ = character;
                    ++rx_dce_buffer_wcnt;
                    setup_timer(&last_rx_tick, 0);
                    if (rx_dce_buffer_wcnt == MRBLR_v14) {
                        close_rx_buffer();
                        rx.phase = V14_IDLE;
                    }
                } else {
                    rx.phase = V14_IDLE;
                }
            } else {
                done = 1;
            }
            break;
        default:
            done = 1;
        }
    }
}

void async_receive_idle_timeout(void)
{
    char installed;
#ifdef NONMATCHING
#else
    /* @hack redundant: a jump to the next statement */
    goto check_irq;
check_irq:
#endif
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((installed = targetIsDpIrqInstalled()) != 0) {
        target_InstallDpIrq(NULL);
    }
    if (rx_dce_buffer_wcnt && timer_expired(last_rx_tick + V14_IDLE_TICKS)) {
        close_rx_buffer();
        rx.phase = V14_IDLE;
    }
    if (installed) {
        target_InstallDpIrq(dp_interrupt);
    }
}

void sendv14_dce_break(Uint16 ticks)
{
    if (tx.phase == V14_BREAK) {
        return;
    }
    setup_timer(&tx_break_timer, ticks);
    tx_break_flag = TX_BREAK_DUE;
}
