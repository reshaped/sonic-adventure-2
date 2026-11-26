/* the modem's end of the serial port to the host. the controller takes the
 * host's bytes and breaks through it, and gives the host its own. this end also
 * watches for the escape sequence. it sets the port's lines, and raises the
 * modem's events to the host */
#include <sg_xpt.h>

#include "dte_vxd.h"
#include "ecdc_def.h"
#include "ecdcutil.h"
#include "int_vect.h"
#include "targdef.h"
#include "tci_ecdc.h"
#include "tci_modm.h"
#include "vxd_int.h"

/* the bits of a character on the line in a format of 0, which
 * dte_char_length counts. a format adds a bit for FORMAT_PARITY, and one for
 * FORMAT_MARK. it adds one more where its FORMAT_DATA_BITS are FORMAT_DATA_8.
 * modemdef.h's codes are made of these fields: FORMAT_EVEN_PARITY and
 * FORMAT_ODD_PARITY hold FORMAT_PARITY, FORMAT_MARK_PARITY holds FORMAT_MARK,
 * and FORMAT_8_BITS is FORMAT_DATA_8 */
#define BASE_CHAR_LENGTH 9
#define FORMAT_PARITY 0x04
#define FORMAT_DATA_BITS 0x18
#define FORMAT_DATA_8 0x10
#define FORMAT_MARK 0x20

/* the longest break vxd_send_dte_break sends, in 10 ms ticks */
#define BREAK_MAX 500U

/* GenerateIRQ's events up to COM_EVENT_LO_MAX go to event_lo. a higher
 * event goes to event_hi, shifted right by COM_EVENT_HI_SHIFT */
#define COM_EVENT_LO_MAX 0x80
#define COM_EVENT_HI_SHIFT 8

/* FORGET_BREAKS once a break has come while every receive buffer was
 * filled */
#define BREAKS_FORGOTTEN 0xff

/* make the control characters those of flow, with esc the escape character,
 * and guard_time the escape's guard time. a flow without XON and XOFF puts
 * the escape character in the stop entry. format is never read */
static void dte_set_flow(Uint8 format, Uint8 flow, Uint8 esc, Uint8 guard_time,
                         Uint8 xon, Uint8 xoff);

struct dte_counts {
    Uint8 tx;
    Uint8 rx;
};

struct com_memory Buffer_memory;

/* the member exports these names, though no other member uses them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* put the port in command mode */
void dte_set_async_cmd(void);
/* put the port in data mode with the given settings. a port already of
 * DTE_ASYNC_DATA only takes the settings, and leaves format unread */
void dte_set_async_data(Uint8 speed, Uint8 format, Uint8 flow, Uint8 esc,
                        Uint8 guard_time, Uint8 xon, Uint8 xoff);
/* make esc the escape character of the cchar entry whose flags control points
 * at. status_esc_code_ptr keeps control. an esc above ESC_CHAR_MAX sets
 * CCHAR_END in those flags instead */
void vxd_do_escape_code(volatile Uint8 *control, Uint8 esc);
/* point *data at the port's command_rx, which the controller reads under
 * DTE_REMOTE. it returns COM_BUFFER_SIZE, or 0 while bytes wait there */
Uint16 vxd_dte_data_to_remote(Uint8 **data);
/* mark the first length bytes of command_rx waiting */
void dte_data_to_remote_done(Uint16 length);
/* point *data at the bytes waiting in the port's command_tx, which the
 * controller fills under DTE_REMOTE. it sets *length to their count, and
 * returns 1, or 0 for none */
char vxd_dte_data_from_remote(Uint8 **data, Uint16 *length);
/* mark the bytes of command_tx taken */
void dte_data_from_remote_done(void);
/* send the host a break of length, at most BREAK_MAX, and raise
 * COM_EVENT_BREAK. while a break runs, the call does nothing */
void vxd_send_dte_break(Uint16 length);
/* return 1 while the port's mcr has COM_MCR_RTS clear, and 0 while it is
 * set */
char vxd_dte_get_rts(void);
/* raise event to the host's side, and return 1. an event masked in
 * event_mask_lo or event_mask_hi still sets its bit, but sends no
 * interrupt */
char check_and_set_int_event(Uint16 event);
void send_interrupt(void);
/* keep tx and rx as the callbacks of dteExecCallbackTx and
 * dteExecCallbackRx, and turn them on. dteStopCallback turns them off */
void dteSetCallback(void (*tx)(void), void (*rx)(void));
void dteStopCallback(void);
/* return dte_tx_cnt and dte_rx_cnt */
struct dte_counts dteGetStatus(void);

/* the callbacks dteSetCallback keeps */
void (*pCallbackTx)(void);
void (*pCallbackRx)(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* 1 from dteSetCallback until dteStopCallback */
static Uint8 dte_callbacks_on = 0;

void vxd_dte_clr_rx_buff(void)
{
    Uint8 i;
    Uint8 j;

    /* @hack spelling: i counted down from COM_BUFFERS, with i - 1 the
     * buffer, where a loop up from 0 would do */
    for (i = COM_BUFFERS; i > 0; i--) {
        for (j = 0; j < COM_BREAKS; j++) {
            Buffer_memory.rx[i - 1].breaks.slot[j] = 0;
        }
    }
    while (dte_rx_cnt) {
        vxd_dte_rx_data_done();
    }
}

void vxd_dte_clr_tx_buff(void)
{
    Uint8 i;
    Uint8 j;

    /* @hack spelling: i counted down from COM_BUFFERS, with i - 1 the
     * buffer, where a loop up from 0 would do */
    for (i = COM_BUFFERS; i > 0; i--) {
        Buffer_memory.tx[i - 1].status.all |=
            COM_STATUS_EMPTY | COM_STATUS_UNK_2;
        Buffer_memory.tx[i - 1].length = 0;
        for (j = 0; j < COM_BREAKS; j++) {
            Buffer_memory.tx[i - 1].breaks.slot[j] = 0;
        }
    }
    GenerateIRQ(COM_EVENT_TX_CLEAR);
    dte_tx_write = 0;
    dte_tx_read = 0;
    dte_tx_cnt = 0;
}

char vxd_dte_get_rx_data(Uint8 **data, Uint16 *length)
{
    Uint8 i;

    if (dte_type == DTE_REMOTE) {
        if (remote_dialog_rx_ctr) {
            *length = remote_dialog_rx_ctr;
            *data = (Uint8 *)Buffer_memory.command_rx;
            return 1;
        }
        *length = 0;
        return 0;
    }
    if (Buffer_memory.rx[dte_rx_read].breaks.slot[0] &&
        !(Buffer_memory.rx[dte_rx_read].status.all & COM_STATUS_UNK_2)) {
        for (i = 0;
             i != COM_BREAKS && Buffer_memory.rx[dte_rx_read].breaks.slot[i];) {
            TCI_dte_brk_rx(Buffer_memory.rx[dte_rx_read].breaks.slot[i]);
            /* @hack spelling: i stepped in the store, where the loop's own step
             * would do */
            Buffer_memory.rx[dte_rx_read].breaks.slot[i++] = 0;
        }
        *length = 0;
        return 0;
    }
    if (!dte_rx_cnt) {
        *length = 0;
        if (!(Buffer_memory.mcr & COM_MCR_IDLE) &&
            !(Buffer_memory.mode & COM_MODE_COMMAND)) {
            return 1;
        }
    } else {
        *data = (Uint8 *)Buffer_memory.rx[dte_rx_read].data;
        *length = Buffer_memory.rx[dte_rx_read].length;
    }
    if (!*length) {
        return 0;
    }
    return 1;
}

void vxd_dte_rx_data_done(void)
{
    if (dte_type == DTE_REMOTE) {
        remote_dialog_rx_ctr = 0;
        return;
    }
    if (!dte_rx_cnt) {
        return;
    }
    Buffer_memory.rx[dte_rx_read].status.all &=
        (Uint8) ~(COM_STATUS_UNK_2 | COM_STATUS_CCHAR);
    dte_rx_cnt--;
    Buffer_memory.rx[dte_rx_read].status.all |= COM_STATUS_EMPTY;
    dte_rx_read = (dte_rx_read + 1) & (COM_BUFFERS - 1);
    GenerateIRQ(COM_EVENT_RX);
}

void vxd_dte_set_interface(Uint8 type, Uint8 speed, Uint8 format, Uint8 flow,
                           Uint8 esc, Uint8 guard_time, Uint8 xon, Uint8 xoff,
                           Uint8 rx_break_type)
{
    if (type == DTE_NONE) {
        return;
    }
    next_dte_type = type;
    if (!vxd_message_buff_empty()) {
        speed_bak = speed;
        format_bak = format;
        flwctl_bak = flow;
        S2_bak = esc;
        S12_bak = guard_time;
        XON_bak = xon;
        XOFF_bak = xoff;
        rx_break_type_bak = rx_break_type;
        return;
    }
    if (Buffer_memory.speed != speed) {
        Buffer_memory.speed = speed;
        GenerateIRQ(COM_EVENT_SPEED);
    }
    switch (type) {
    case DTE_ASYNC_DATA:
        dte_set_async_data(speed, format, flow, esc, guard_time, xon, xoff);
        break;
    case DTE_HELD:
        Buffer_memory.mode |= COM_MODE_HOLD_OUT | COM_MODE_HOLD_IN;
        GenerateIRQ(COM_EVENT_MODE);
        vxd_dte_set_cts(0);
        dte_type = type;
        next_dte_type = DTE_NONE;
        return;
    case DTE_RATE_CMD:
        save_rate(Buffer_memory.baud);
        /* fall through */
    case DTE_ASYNC_CMD:
        dte_set_async_cmd();
        break;
    case DTE_REMOTE:
        dte_set_async_cmd();
        remote_dialog_tx_ctr = 0;
        remote_dialog_rx_ctr = 0;
        break;
    case DTE_DIRECT_1:
    case DTE_DIRECT_2:
    case DTE_DIRECT_SYNC:
    case DTE_UNK_4:
    case DTE_UNK_5:
    case DTE_UNK_10:
        break;
    default:
        return;
    }
    dte_type = type;
    dte_speed = speed;
    dte_format = format;
    dte_flwctl = flow;
    dte_S2 = esc;
    dte_S12 = guard_time;
    dte_xon = xon;
    dte_xoff = xoff;
    dte_rx_break_type = rx_break_type;
    next_dte_type = DTE_NONE;
}

void dte_set_async_cmd(void)
{
    if (Buffer_memory.mode & COM_MODE_COMMAND) {
        return;
    }
    Buffer_memory.mode |= COM_MODE_HOLD_OUT | COM_MODE_HOLD_IN;
    GenerateIRQ(COM_EVENT_MODE);
    Buffer_memory.mode |= COM_MODE_COMMAND;
    GenerateIRQ(COM_EVENT_MODE);
    Buffer_memory.mode &= (Uint8)~COM_MODE_HOLD_OUT;
    Buffer_memory.mode &= (Uint8)~COM_MODE_HOLD_IN;
    GenerateIRQ(COM_EVENT_MODE);
}

void dte_set_async_data(Uint8 speed, Uint8 format, Uint8 flow, Uint8 esc,
                        Uint8 guard_time, Uint8 xon, Uint8 xoff)
{
    Uint16 bits;

    if (dte_type == DTE_ASYNC_DATA) {
        /* @hack redundant: a store of speed under a test, which the store
         * below repeats */
        if (dte_speed != speed) {
            dte_speed = speed;
        }
        if (dte_flwctl != flow) {
            dte_set_flow(format, flow, esc, guard_time, xon, xoff);
        }
        dte_speed = speed;
        dte_flwctl = flow;
        dte_S2 = esc;
        dte_S12 = guard_time;
        dte_xon = xon;
        dte_xoff = xoff;
        next_dte_type = DTE_NONE;
        return;
    }
    Buffer_memory.mode |= COM_MODE_HOLD_OUT | COM_MODE_HOLD_IN;
    GenerateIRQ(COM_EVENT_MODE);
    dte_speed = speed;
    bits = BASE_CHAR_LENGTH;
    if (format & FORMAT_PARITY) {
        bits = BASE_CHAR_LENGTH + 1;
    }
    if ((format & FORMAT_DATA_BITS) == FORMAT_DATA_8) {
        bits++;
    }
    if (format & FORMAT_MARK) {
        bits++;
    }
    dte_char_length = bits;
    dte_set_flow(format, flow, esc, guard_time, xon, xoff);
    if (Buffer_memory.mode & COM_MODE_COMMAND) {
        Buffer_memory.mode &= (Uint8)~COM_MODE_COMMAND;
        GenerateIRQ(COM_EVENT_MODE);
    }
    Buffer_memory.mode &= (Uint8)~COM_MODE_HOLD_OUT;
    Buffer_memory.mode &= (Uint8)~COM_MODE_HOLD_IN;
    GenerateIRQ(COM_EVENT_MODE);
}

static void dte_set_flow(Uint8 format, Uint8 flow, Uint8 esc, Uint8 guard_time,
                         Uint8 xon, Uint8 xoff)
{
    volatile Uint8 *control;

    /* @hack redundant: a read of stop_flags, which nothing uses */
    (void)Buffer_memory.cchar.entry.stop_flags;
    CC_FLAGS = 0;
    if (flow == FLOW_XON_XOFF) {
        Buffer_memory.cchar.entry.stop_flags &= (Uint8)~CCHAR_END;
        Buffer_memory.cchar.entry.stop_flags |= CCHAR_REJECT;
        Buffer_memory.cchar.entry.stop_char = xoff;
        Buffer_memory.cchar.entry.start_flags &= (Uint8)~CCHAR_END;
        Buffer_memory.cchar.entry.start_flags |= CCHAR_REJECT;
        Buffer_memory.cchar.entry.start_char = xon;
        Buffer_memory.cchar.entry.escape_flags &= (Uint8)~CCHAR_REJECT;
        control = &Buffer_memory.cchar.entry.escape_flags;
        CC_FLAGS &= (Uint8)~CC_NO_XON_XOFF;
        CC_FLAGS |= CC_XON_XOFF;
    } else if (flow == FLOW_KEEP_XON_XOFF || flow == FLOW_UNK_7) {
        Buffer_memory.cchar.entry.stop_flags &= (Uint8)~CCHAR_END;
        Buffer_memory.cchar.entry.stop_flags &= (Uint8)~CCHAR_REJECT;
        Buffer_memory.cchar.entry.stop_char = xoff;
        Buffer_memory.cchar.entry.start_flags &= (Uint8)~CCHAR_END;
        Buffer_memory.cchar.entry.start_flags &= (Uint8)~CCHAR_REJECT;
        Buffer_memory.cchar.entry.start_char = xon;
        Buffer_memory.cchar.entry.escape_flags &= (Uint8)~CCHAR_REJECT;
        control = &Buffer_memory.cchar.entry.escape_flags;
        CC_FLAGS &= (Uint8)~CC_NO_XON_XOFF;
        CC_FLAGS |= CC_XON_XOFF;
    } else {
        control = &Buffer_memory.cchar.entry.stop_flags;
        Buffer_memory.cchar.entry.stop_flags &= (Uint8)~CCHAR_REJECT;
        Buffer_memory.cchar.entry.start_flags |= CCHAR_END;
        Buffer_memory.cchar.entry.escape_flags |= CCHAR_END;
        CC_FLAGS |= CC_NO_XON_XOFF;
        CC_FLAGS &= (Uint8)~CC_XON_XOFF;
    }
    vxd_do_escape_code(control, esc);
    GenerateIRQ(COM_EVENT_CCHAR);
    dte_S12 = guard_time;
}

void vxd_do_escape_code(volatile Uint8 *control, Uint8 esc)
{
    status_esc_code_ptr = control;
    if (esc <= ESC_CHAR_MAX) {
        control++;
        *control = esc;
        BMR(14) |= BMR14_ESC_ON;
        *status_esc_code_ptr &= (Uint8)~CCHAR_END;
    } else {
        *status_esc_code_ptr |= CCHAR_END;
    }
    BMR(21) &= (Uint8)~BMR21_ESCAPE;
    BMR(14) &= (Uint8)~BMR14_ESC_COUNT;
    ESC_TOUT = TIM_10MS;
}

void vxd_esc_seq_timr(void)
{
    if (!(BMR(21) & BMR21_GUARD)) {
        return;
    }
    /* @hack spelling: an empty arm under the test, where the negated test
     * over the else's statements would do */
    if ((SCC_FLAGS & SCC_ESC_OFF) != 0 ||
        (Buffer_memory.mcr & COM_MCR_IDLE) == 0) {
    } else {
        if (TIM_10MS < ESC_TOUT + (dte_S12 * S12_TICKS)) {
            return;
        }
        BMR(21) &= (Uint8)~BMR21_GUARD;
        TCI_dte_esc_seq_found();
        BMR(21) &= (Uint8) ~(BMR21_ESCAPE & ~BMR21_TEXT);
        BMRregisters[14] &= (Uint8) ~(BMR14_ESC_ON | BMR14_ESC_COUNT);
    }
    ESC_TOUT = TIM_10MS;
}

void vxd_prog_esc_code(void)
{
    if ((dte_type == DTE_ASYNC_DATA || dte_type == DTE_DIRECT_1 ||
         dte_type == DTE_DIRECT_2) &&
        dte_S2 <= ESC_CHAR_MAX && !(SCC_FLAGS & SCC_ESC_OFF)) {
        if (!(BMRregisters[14] & BMR14_ESC_ON)) {
            BMRregisters[14] |= BMR14_ESC_ON;
            MD_INT_MASK_MCR |= MASK_ESC_ON;
            *status_esc_code_ptr &= (Uint8)~CCHAR_END;
            GenerateIRQ(COM_EVENT_CCHAR);
        }
    } else {
        BMRregisters[14] &= (Uint8) ~(BMR14_ESC_ON | BMR14_ESC_COUNT);
        ESC_TOUT = TIM_10MS;
    }
}

void vxd_stop_esc_code(void)
{
    Uint16 count;
    Uint8 *data;

    if (!(BMR(21) & BMR21_TEXT)) {
        BMR(14) &= (Uint8)~BMR14_ESC_COUNT;
        BMR(21) &= (Uint8)~BMR21_ESCAPE;
        if (Buffer_memory.mcr & COM_MCR_IDLE) {
            vxd_prog_esc_code();
        } else if (BMR(14) & BMR14_ESC_ON) {
            BMR(14) &= (Uint8)~BMR14_ESC_ON;
            *status_esc_code_ptr |= CCHAR_END;
            GenerateIRQ(COM_EVENT_CCHAR);
            MD_INT_MASK_MCR &= (Uint8)~MASK_ESC_ON;
        }
    } else {
        count = Buffer_memory.rx[dte_rx_write].length;
        data = (Uint8 *)Buffer_memory.rx[dte_rx_write].data;
        BMR(21) &= (Uint8)~BMR21_GUARD;
        if (count <= ESC_TEXT_MAX) {
            while (count--) {
                if (!TIES_validation(*data++)) {
                    /* @hack spelling: a goto to the else's statements, where
                     * a copy of them and a return would do */
                    goto invalid;
                }
            }
            BMR(21) |= BMR21_TEXT_VALID;
        } else {
        invalid:
            BMR(14) &= (Uint8)~BMR14_ESC_COUNT;
            BMR(21) &= (Uint8)~BMR21_ESCAPE;
        }
    }
}

Uint16 vxd_dte_set_tx_data(Uint8 **data)
{
    if (dte_type == DTE_REMOTE) {
        /* @hack spelling: the else after a return, where the statements after
         * the if would do */
        if (remote_dialog_tx_ctr) {
            return 0;
        } else {
            *data = (Uint8 *)Buffer_memory.command_tx;
            return COM_BUFFER_SIZE;
        }
    }
    if (dte_tx_cnt >= COM_BUFFERS) {
        return 0;
    }
    *data = (Uint8 *)Buffer_memory.tx[dte_tx_write].data;
    return COM_BUFFER_SIZE;
}

void vxd_dte_tx_data_done(Uint16 length)
{
    if (length != 0) {
        if (dte_type == DTE_REMOTE) {
            remote_dialog_tx_ctr = length;
            return;
        }
        Buffer_memory.tx[dte_tx_write].length = length;
        if (Buffer_memory.tx[dte_tx_write].breaks.slot[0] == 0) {
            dte_tx_cnt++;
            Buffer_memory.tx[dte_tx_write].status.all &=
                (Uint16)~COM_STATUS_EMPTY;
            dte_tx_write = (dte_tx_write + 1) & (COM_BUFFERS - 1);
            GenerateIRQ(COM_EVENT_TX);
        } else {
            dte_tx_write = (dte_tx_write + 1) & (COM_BUFFERS - 1);
            dte_tx_cnt++;
        }
    }
}

Uint16 vxd_dte_data_to_remote(Uint8 **data)
{
    if (!remote_dialog_rx_ctr) {
        *data = (Uint8 *)Buffer_memory.command_rx;
        return COM_BUFFER_SIZE;
    }
    return 0;
}

void dte_data_to_remote_done(Uint16 length)
{
    remote_dialog_rx_ctr = length;
}

char vxd_dte_data_from_remote(Uint8 **data, Uint16 *length)
{
    /* @hack spelling: the else after a return, where the statements after the
     * if would do */
    if (remote_dialog_tx_ctr) {
        *length = remote_dialog_tx_ctr;
        *data = (Uint8 *)Buffer_memory.command_tx;
        return 1;
    } else {
        *length = 0;
        return 0;
    }
}

void dte_data_from_remote_done(void)
{
    remote_dialog_tx_ctr = 0;
}

void dte_simulate_rx_break(Uint16 length)
{
}

Uint8 dte_speed_restriction(Uint8 speed)
{
    return speed;
}

char vxd_dte_tx_brk(Uint16 length, Uint8 queued)
{
    Uint8 i;
    volatile struct com_buffer *buffer;

    if (!queued) {
        vxd_send_dte_break(length);
        return 1;
    }
    for (i = 0; i != COM_BREAKS;) {
        /* @hack spelling: buffer set and i stepped inside the test, where
         * statements of their own would do */
        if ((buffer = &Buffer_memory.tx[dte_tx_write])->breaks.slot[i++] == 0) {
            buffer->breaks.slot[i - 1] = length;
            if (dte_tx_cnt == 0 && (BREAK_FLAGS & BREAK_TO_HOST) == 0) {
                vxd_send_next_dte_break();
            }
            return 1;
        }
    }
    return 0;
}

void vxd_send_next_dte_break(void)
{
    /* @hack type: the breaks named by queue's fields, where slot[0] to
     * slot[3] would do */
    if (Buffer_memory.tx[dte_tx_read].breaks.queue.first) {
        vxd_send_dte_break(Buffer_memory.tx[dte_tx_read].breaks.queue.first);
        Buffer_memory.tx[dte_tx_read].breaks.queue.first =
            Buffer_memory.tx[dte_tx_read].breaks.queue.second;
        Buffer_memory.tx[dte_tx_read].breaks.queue.second =
            Buffer_memory.tx[dte_tx_read].breaks.queue.third;
        Buffer_memory.tx[dte_tx_read].breaks.queue.third =
            Buffer_memory.tx[dte_tx_read].breaks.queue.fourth;
        Buffer_memory.tx[dte_tx_read].breaks.queue.fourth = 0;
    } else if (Buffer_memory.tx[dte_tx_read].length) {
        Buffer_memory.tx[dte_tx_read].status.all &= (Uint16)~COM_STATUS_EMPTY;
        GenerateIRQ(COM_EVENT_TX);
    }
}

void vxd_send_dte_break(Uint16 length)
{
    if (BREAK_FLAGS & BREAK_TO_HOST) {
        return;
    }
    if (length > BREAK_MAX) {
        length = BREAK_MAX;
    }
    DTE_BK_TIME = length;
    BREAK_FLAGS |= BREAK_TO_HOST;
    GenerateIRQ(COM_EVENT_BREAK);
}

void vxd_end_of_dte_brk_rx(Uint16 length)
{
    Uint8 i;

    if (dte_rx_break_type == 0) {
        TCI_dte_brk_rx(length);
        return;
    }
    /* @hack spelling: i stepped in the test, with i - 1 the slot it tested,
     * where a for loop's own step would do */
    i = 0;
    do {
        if (!Buffer_memory.rx[dte_rx_write].breaks.slot[i++]) {
            Buffer_memory.rx[dte_rx_write].breaks.slot[i - 1] = length;
            break;
        }
    } while (i != COM_BREAKS);
    if (dte_rx_cnt == COM_BUFFERS) {
        FORGET_BREAKS = BREAKS_FORGOTTEN;
    }
}

char vxd_dte_tx_buff_empty(char wait)
{
    if (wait) {
        if (dte_tx_cnt) {
            setup_timer(&dte_timer, TX_EMPTY_TICKS);
        } else if (timer_expired(dte_timer)) {
            return 1;
        }
    } else {
        if (next_dte_type != DTE_RATE_CMD && vxd_message_buff_empty()) {
            return 1;
        }
    }
    return 0;
}

char vxd_message_buff_empty(void)
{
    if (!MESS_CNT && !Buffer_memory.prio_count &&
        timer_expired(dte_delay_timer)) {
        return 1;
    }
    return 0;
}

char vxd_dte_tx_cmd_byte(Uint8 c)
{
    int limit;

    if (close_port) {
        return 1;
    }
    if (MESS_CNT == MESS_SIZE) {
        limit = MESS_FULL_TRIES;
        while (--limit && MESS_CNT == MESS_SIZE) {
        }
        if (!limit) {
            return 0;
        }
    }
    *MESS_WRITE = c;
    MESS_WRITE++;
    if (MESS_WRITE > MESS_TOPP) {
        MESS_WRITE = MESS_BUFFER;
    }
    MESS_CNT++;
    if (Buffer_memory.prio_count == 0) {
        Buffer_memory.prio[0] = *MESS_READ;
        MESS_READ++;
        if (MESS_READ > MESS_TOPP) {
            MESS_READ = MESS_BUFFER;
        }
        MESS_CNT--;
        Buffer_memory.prio_count = 1;
        GenerateIRQ(COM_EVENT_PRIO);
    }
    return 1;
}

char dte_get_anl(void)
{
    return 1;
}

char vxd_dte_get_dtr(void)
{
    /* @hack spelling: value, a local for the bit and its negation, where
     * return !(Buffer_memory.mcr & COM_MCR_DTR) would do */
    int value = Buffer_memory.mcr & COM_MCR_DTR;

    value = !value;
    return value;
}

char dte_get_rdl(void)
{
    return 1;
}

char vxd_dte_get_rts(void)
{
    /* @hack spelling: value, a local for the bit and its negation, where
     * return !(Buffer_memory.mcr & COM_MCR_RTS) would do */
    int value = Buffer_memory.mcr & COM_MCR_RTS;

    value = !value;
    return value;
}

void vxd_dte_set_cts(char on)
{
    if (!on) {
        Buffer_memory.msr |= COM_MSR_CTS;
    } else {
        Buffer_memory.msr &= (Uint8)~COM_MSR_CTS;
    }
    GenerateIRQ(COM_EVENT_MSR);
}

void vxd_dte_set_dcd(char on)
{
    if (!on) {
        Buffer_memory.msr |= COM_MSR_DCD;
    } else {
        Buffer_memory.msr &= (Uint8)~COM_MSR_DCD;
    }
    GenerateIRQ(COM_EVENT_MSR);
}

void vxd_dte_set_dsr(char on)
{
    if (!on) {
        Buffer_memory.msr |= COM_MSR_DSR;
    } else {
        Buffer_memory.msr &= (Uint8)~COM_MSR_DSR;
    }
    GenerateIRQ(COM_EVENT_MSR);
}

void vxd_dte_set_ri(char on)
{
    if (!on) {
        Buffer_memory.msr |= COM_MSR_RI;
    } else {
        Buffer_memory.msr &= (Uint8)~COM_MSR_RI;
    }
    GenerateIRQ(COM_EVENT_MSR);
}

void dte_set_tst(char on)
{
    /* @hack redundant: a test of on, which does nothing either way */
    if (on) {
    }
}

void GenerateIRQ(Uint16 event)
{
    if (!init_mem) {
        return;
    }
    if (close_port) {
        return;
    }
    check_and_set_int_event(event);
}

char check_and_set_int_event(Uint16 event)
{
    /* @hack redundant: the else's test of event <= COM_EVENT_LO_MAX, which
     * always holds there */
    if (event > COM_EVENT_LO_MAX) {
        Uint8 shifted = event >> COM_EVENT_HI_SHIFT;
        Buffer_memory.event_hi |= shifted;
        /* @hack spelling: the else after a return, where the statement after
         * the if would do */
        if (shifted & Buffer_memory.event_mask_hi) {
            return 1;
        } else {
            send_interrupt();
        }
    } else if (event <= COM_EVENT_LO_MAX) {
        Buffer_memory.event_lo |= event;
        if (!((Uint8)event & Buffer_memory.event_mask_lo)) {
            send_interrupt();
        }
    }
    return 1;
}

void send_interrupt(void)
{
    Comm_Int();
}

void vxd_dte_dce_rx_idle(void)
{
}

int vxd_dte_async_rx_direct_mode(Uint8 *data, Uint8 *control)
{
    return 0;
}

void vxd_dte_async_tx_direct_mode(Uint8 c)
{
}

void dteSetCallback(void (*tx)(void), void (*rx)(void))
{
    pCallbackTx = tx;
    pCallbackRx = rx;
    dte_callbacks_on = 1;
}

void dteExecCallbackTx(void)
{
    if (dte_callbacks_on == 1 && pCallbackTx) {
        pCallbackTx();
    }
}

void dteExecCallbackRx(void)
{
    if (dte_callbacks_on == 1 && pCallbackRx) {
        pCallbackRx();
    }
}

void dteStopCallback(void)
{
    dte_callbacks_on = 0;
}

struct dte_counts dteGetStatus(void)
{
    struct dte_counts counts;
    struct dte_counts result;
    /* @hack spelling: the counts copied to result through two pointers, where
     * return counts would do */
    struct dte_counts *dst;
    struct dte_counts *src;

    counts.rx = dte_rx_cnt;
    counts.tx = dte_tx_cnt;
    src = &counts;
    dst = &result;
    dst->tx = src->tx;
    dst->rx = src->rx;
    return result;
}
