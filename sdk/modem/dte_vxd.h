/* dte_vxd: the modem's end of the serial port to the host, and the port's
 * memory */
#ifndef MODEM_DTE_VXD_H
#define MODEM_DTE_VXD_H
#include <sg_xpt.h>

/* the bytes of each of the port's buffers, and of command_tx and
 * command_rx */
#define COM_BUFFER_SIZE 128
/* the buffers each way, a power of two. the modem's side wraps its indexes by
 * masking with COM_BUFFERS - 1 */
#define COM_BUFFERS 8
#define COM_PRIO_SIZE 16 /* the bytes of prio */
#define COM_CMD_SIZE 16 /* the bytes of cmd */
#define COM_BREAKS 4 /* the most breaks a buffer queues */
/* the control characters: stop, start and escape, in that order */
#define COM_CCHARS 3
/* the bits of a control character's flags, as struct cmi_cchar names them */
#define CCHAR_END 0x80
#define CCHAR_REJECT 0x40

/* the bits of a buffer's status. COM_STATUS_EMPTY is set while the buffer
 * belongs to the side that fills it. the host's side fills the receive buffers,
 * and the modem's side the transmit buffers. COM_STATUS_CCHAR marks a receive
 * buffer that closed on a control character it kept. vxd_dte_clr_tx_buff sets
 * COM_STATUS_UNK_2 in each transmit buffer. no code sets it in a receive
 * buffer, where vxd_dte_get_rx_data tests it */
#define COM_STATUS_EMPTY 0x01
#define COM_STATUS_UNK_2 0x02
#define COM_STATUS_CCHAR 0x04

/* the bits of the port's mode, as struct cmi_mode names them */
#define COM_MODE_HOLD_IN 0x01
#define COM_MODE_COMMAND 0x02
#define COM_MODE_HOLD_OUT 0x04
#define COM_MODE_INT_OFF 0x10

/* the bits of the port's mcr. COM_MCR_IDLE is set while the host's side is
 * idle */
#define COM_MCR_DTR 0x01
#define COM_MCR_RTS 0x02
#define COM_MCR_IDLE 0x80

/* the bits of the port's lcr, as in a 16550 UART's line control register.
 * COM_LCR_WLS1 alone selects 7-bit bytes, and with COM_LCR_WLS0 8-bit bytes.
 * under COM_LCR_PARITY, COM_LCR_STICK fixes the parity bit: 0 with
 * COM_LCR_EVEN, and 1 without. COM_LCR_BREAK is set while the host sends a
 * break */
#define COM_LCR_WLS0 0x01
#define COM_LCR_WLS1 0x02
#define COM_LCR_PARITY 0x08
#define COM_LCR_EVEN 0x10
#define COM_LCR_STICK 0x20
#define COM_LCR_BREAK 0x40

/* the bits of the port's msr. each is set while its line is off */
#define COM_MSR_CTS 0x10
#define COM_MSR_DSR 0x20
#define COM_MSR_RI 0x40
#define COM_MSR_DCD 0x80

/* the events GenerateIRQ raises to the host's side, a bit each */
#define COM_EVENT_TX 0x0001 /* a transmit buffer filled */
#define COM_EVENT_RX 0x0002 /* a receive buffer emptied */
#define COM_EVENT_PRIO 0x0004 /* a priority byte waits in prio */
#define COM_EVENT_CMD_DONE 0x0008 /* the modem took cmd's bytes */
#define COM_EVENT_MODE 0x0010 /* the mode changed */
#define COM_EVENT_MSR 0x0020 /* the msr changed */
#define COM_EVENT_MRBLR 0x0040 /* the receive buffers' length changed */
#define COM_EVENT_CCHAR 0x0080 /* the control characters changed */
#define COM_EVENT_BREAK 0x0100 /* a break waits for the host */
#define COM_EVENT_RESTART 0x0200 /* the port is to restart */
#define COM_EVENT_TX_CLEAR 0x0400 /* the transmit buffers were emptied */
#define COM_EVENT_SPEED 0x0800 /* the speed changed */

/* the md_ier that asks the modem's side to reset the port */
#define COM_MD_RESET 0xff
/* the bits of md_ier and md_mask, as struct Md names them */
#define COM_MD_RX 0x01
#define COM_MD_TX 0x02
#define COM_MD_CCHAR 0x08
#define COM_MD_PRIO_SENT 0x10
#define COM_MD_CMD 0x20
#define COM_MD_MCR 0x40
#define COM_MD_LCR 0x80

/* the port's types, as vxd_dte_set_interface takes them. DTE_NONE is no
 * type: next_dte_type holds it while no type waits */
#define DTE_ASYNC_DATA 0
/* re_configure_dte's types for a raw link, picked as re_configure_dce picks
 * dce.h's DCE_DIRECT_ types. a synchronous line takes DTE_DIRECT_SYNC. an
 * asynchronous one takes DTE_DIRECT_2, or DTE_DIRECT_1 where OIS(6) lacks
 * bit 7 */
#define DTE_DIRECT_1 1
#define DTE_DIRECT_2 2
#define DTE_DIRECT_SYNC 3
/* nothing passes DTE_UNK_4, DTE_UNK_5 or DTE_UNK_10. vxd_dte_set_interface
 * takes each as it takes a DTE_DIRECT_ type */
#define DTE_UNK_4 4
#define DTE_UNK_5 5
#define DTE_ASYNC_CMD 6
#define DTE_HELD 7
#define DTE_UNK_10 10
#define DTE_RATE_CMD 11
#define DTE_REMOTE 13
#define DTE_NONE 0xfe

/* the flow controls vxd_dte_set_interface takes. dce_set_interface takes
 * FLOW_XON_XOFF and FLOW_KEEP_XON_XOFF for an asynchronous line too. under
 * either, xoff and xon are the stop and start characters. FLOW_XON_XOFF
 * drops both from the data received, and FLOW_KEEP_XON_XOFF keeps them there.
 * nothing passes FLOW_UNK_7, which the port takes as FLOW_KEEP_XON_XOFF.
 * under any other flow, the port has no stop or start character */
#define FLOW_XON_XOFF 2
#define FLOW_KEEP_XON_XOFF 4
#define FLOW_UNK_7 7

/* the highest escape character. a higher one turns the escape off */
#define ESC_CHAR_MAX 127
/* a byte's bits but cmi.h's PARITY_BIT */
#define SEVEN_BITS 0x7f
/* the 10 ms ticks vxd_dte_tx_buff_empty waits once the transmit buffers
 * empty */
#define TX_EMPTY_TICKS 10
/* the most bytes of text after an escape that vxd_stop_esc_code accepts */
#define ESC_TEXT_MAX 40
/* the 10 ms ticks in a unit of dte_S12, the escape's guard time */
#define S12_TICKS 2U
/* the times vxd_dte_tx_cmd_byte tests a full MESS_BUFFER before it drops
 * its byte */
#define MESS_FULL_TRIES 1000

/* one of the port's buffers: length bytes of data, and up to COM_BREAKS
 * breaks queued ahead of them. each slot holds a break's length, and the
 * first slot of 0 ends the queue. queue names the slots one by one */
struct com_buffer {
    union {
        volatile Uint16 all;
        struct {
            volatile Uint16 : 15;
            volatile Uint16 empty : 1;
        } bit;
    } status;
    volatile Uint16 length;
    volatile Uint8 data[COM_BUFFER_SIZE];
    union {
        volatile Uint16 slot[COM_BREAKS];
        struct {
            volatile Uint16 first;
            volatile Uint16 second;
            volatile Uint16 third;
            volatile Uint16 fourth;
        } queue;
    } breaks;
};

/* the memory the port's two sides share. the host's side is cmm's, and the
 * modem's side is vxd_int's and dte_vxd's. both change it from interrupt
 * handlers, so every field is volatile. the receive buffers carry the host's
 * bytes to the modem. the transmit buffers carry the modem's bytes to the
 * host */
struct com_memory {
    volatile Uint8 unk_0[60];
    volatile Uint16 mrblr; /* the bytes a receive buffer takes */
    volatile Uint8 unk_3e;
    /* the receive buffer the host's side fills, and the transmit buffer it
     * takes next */
    volatile Uint8 rx_index;
    volatile Uint8 tx_index;
    /* the events raised to the host's side. event_lo holds bits 0 to 7 of
     * GenerateIRQ's event, and event_hi bits 8 to 15. an event whose bit is
     * set in event_mask_lo or event_mask_hi raises no interrupt */
    volatile Uint8 event_mask_lo;
    volatile Uint8 event_mask_hi;
    volatile Uint8 event_lo;
    volatile Uint8 event_hi;
    /* the events raised to the modem's side, a bit each as struct Md names
     * them. md_ier holds those raised. the modem's side serves none whose bit
     * is set in md_mask */
    volatile Uint8 md_mask;
    volatile Uint8 md_ier;
    volatile Uint8 unk_47;
    volatile Uint8 unk_48;
    volatile Uint8 prio_char; /* a control character the host's side rejected */
    /* the control characters, each as its flags and then itself. the flags'
     * bits are those of struct cmi_cchar */
    union {
        volatile Uint8 byte[2 * COM_CCHARS];
        struct {
            volatile Uint8 stop_flags;
            volatile Uint8 stop_char;
            volatile Uint8 start_flags;
            volatile Uint8 start_char;
            volatile Uint8 escape_flags;
            volatile Uint8 escape_char;
        } entry;
    } cchar;
    /* the modem's priority bytes for the host, prio_count of them */
    volatile Uint8 prio_count;
    volatile Uint8 prio[COM_PRIO_SIZE];
    /* the host's command bytes for the modem, cmd_count of them */
    volatile Uint8 cmd_count;
    volatile Uint8 cmd[COM_CMD_SIZE];
    volatile Uint8 baud; /* the host's baud setting */
    volatile Uint8 lcr; /* the line control register, as the host sets it */
    volatile Uint8 mcr; /* the modem control register: COM_MCR_ bits */
    volatile Uint8 msr; /* the modem status register: COM_MSR_ bits */
    volatile Uint8 mode; /* the port's mode: COM_MODE_ bits */
    struct com_buffer rx[COM_BUFFERS];
    struct com_buffer tx[COM_BUFFERS];
    /* the port's speed, as vxd_dte_set_interface numbers it */
    volatile Uint8 speed;
    /* under DTE_REMOTE, command_tx takes the place of the transmit buffers,
     * and command_rx that of the receive buffers */
    volatile Uint8 command_tx[COM_BUFFER_SIZE];
    volatile Uint8 command_rx[COM_BUFFER_SIZE];
};

extern struct com_memory Buffer_memory;

/* set *length to the count of bytes the modem takes next from the host. where
 * there are any, point *data at them. the call returns 1 where the count isn't
 * 0, and 0 where it is. under DTE_REMOTE, the bytes are those waiting in
 * command_rx.
 *
 * otherwise they are the bytes of the receive buffer dte_rx_read names, once
 * the host's side has filled it. breaks queued with that buffer go to
 * TCI_dte_brk_rx first, unless its status holds COM_STATUS_UNK_2. a call that
 * passes breaks on returns 0, with a count of 0. with no receive buffer
 * filled, the count is 0. the call then still returns 1 while the host's side
 * isn't idle and the mode lacks COM_MODE_COMMAND */
char vxd_dte_get_rx_data(Uint8 **data, Uint16 *length);
/* mark the bytes vxd_dte_get_rx_data gave taken. outside DTE_REMOTE, the
 * call hands their receive buffer back to the host's side, and raises
 * COM_EVENT_RX */
void vxd_dte_rx_data_done(void);
/* point *data at room for bytes to send to the host. it returns how many
 * fit there, 0 for none */
Uint16 vxd_dte_set_tx_data(Uint8 **data);
/* send the first length bytes of the room vxd_dte_set_tx_data gave, and
 * raise COM_EVENT_TX. a length of 0 sends nothing. where breaks are queued
 * ahead of the bytes, the bytes wait for vxd_send_next_dte_break. under
 * DTE_REMOTE, the bytes wait in command_tx, and the call raises nothing */
void vxd_dte_tx_data_done(Uint16 length);
/* run the escape sequence's guard time. ModemMainCall calls this every 10 ms.
 * the call acts while BMRregisters[21] marks a guard time. it waits for the
 * host's side to stay idle for the guard time, dte_S12 units of 20 ms. the
 * escape then goes to TCI_dte_esc_seq_found. a call while the host's side is
 * busy starts the wait over. so does a call while SCC_FLAGS keeps the escape
 * off */
void vxd_esc_seq_timr(void);
/* drop the breaks queued with the receive buffers, and take every filled
 * one as vxd_dte_rx_data_done does. under DTE_REMOTE, dte_rx_cnt must be 0,
 * or the call never returns */
void vxd_dte_clr_rx_buff(void);
/* empty the transmit buffers and their breaks, and raise
 * COM_EVENT_TX_CLEAR */
void vxd_dte_clr_tx_buff(void);
/* set the port up as type, a DTE_ value, with the given settings. a type of
 * DTE_NONE does nothing.
 *
 * while vxd_message_buff_empty returns 0, the call only defers type. it keeps
 * type in next_dte_type, and the settings in targdef.h's _bak names.
 *
 * otherwise the call makes speed the port's speed, and raises
 * COM_EVENT_SPEED where that changes it. DTE_ASYNC_DATA puts the port in data
 * mode. DTE_ASYNC_CMD, DTE_RATE_CMD and DTE_REMOTE put it in command mode.
 * DTE_RATE_CMD first passes the port's baud to save_rate, and DTE_REMOTE
 * empties command_tx and command_rx. the call then keeps type and the
 * settings in targdef.h's dte_ names, and sets next_dte_type to DTE_NONE.
 *
 * DTE_HELD keeps only dte_type: it holds the host's bytes both ways, and
 * turns CTS off. a type no DTE_ name stands for changes only the speed, and
 * stays in next_dte_type */
void vxd_dte_set_interface(Uint8 type, Uint8 speed, Uint8 format, Uint8 flow,
                           Uint8 esc, Uint8 guard_time, Uint8 xon, Uint8 xoff,
                           Uint8 rx_break_type);
/* turn the watch for the escape character on, and raise COM_EVENT_CCHAR,
 * unless the watch is on. the watch needs a port of DTE_ASYNC_DATA,
 * DTE_DIRECT_1 or DTE_DIRECT_2. its escape character, dte_S2, must be at most
 * ESC_CHAR_MAX, and SCC_FLAGS mustn't keep the escape off. without them, the
 * call clears the watch's bits of BMRregisters[14], and sets ESC_TOUT to
 * TIM_10MS */
void vxd_prog_esc_code(void);
/* end the escape sequence, or test the text that follows an escape.
 *
 * where BMRregisters[21] marks no text after the escape, the call clears the
 * escape's bits of BMRregisters[14] and [21]. where the host's side is idle, it
 * then calls vxd_prog_esc_code. otherwise it turns the watch for the escape
 * character off.
 *
 * where text follows, the call passes each byte of the receive buffer the
 * host's side fills to TIES_validation. text of at most ESC_TEXT_MAX bytes
 * that all pass marks the text valid in BMRregisters[21]. any other text
 * clears the escape's bits */
void vxd_stop_esc_code(void);
/* do nothing */
void dte_simulate_rx_break(Uint16 length);
/* return speed */
Uint8 dte_speed_restriction(Uint8 speed);
/* send the host a break of length 10 ms ticks. where queued is 0, the break
 * goes at once, unless a break already runs: then it is dropped. otherwise it
 * queues ahead of the bytes of the transmit buffer the modem fills. the call
 * returns 0 where that buffer's COM_BREAKS slots are full, and 1 otherwise */
char vxd_dte_tx_brk(Uint16 length, Uint8 queued);
/* send the next break queued with the transmit buffer the host's side takes
 * next. with no break left, hand the buffer to the host's side where it holds
 * bytes, and raise COM_EVENT_TX */
void vxd_send_next_dte_break(void);
/* take a break of length from the host. while dte_rx_break_type is 0, the
 * break goes to TCI_dte_brk_rx at once. otherwise it queues ahead of the
 * bytes of the receive buffer the host's side fills. a break that finds that
 * buffer's COM_BREAKS slots full is dropped. where every receive buffer is
 * filled, the call also sets FORGET_BREAKS */
void vxd_end_of_dte_brk_rx(Uint16 length);
/* where wait is set, return 1 once the transmit buffers have stayed empty
 * for TX_EMPTY_TICKS. otherwise return vxd_message_buff_empty, or 0 while
 * next_dte_type is DTE_RATE_CMD */
char vxd_dte_tx_buff_empty(char wait);
/* return 1 where no message byte waits for the host and dte_delay_timer
 * has run out, and 0 otherwise */
char vxd_message_buff_empty(void);
/* queue c in MESS_BUFFER for the host, and return 1. while the port's prio
 * is empty, the call moves MESS_BUFFER's oldest byte to prio, and raises
 * COM_EVENT_PRIO. that byte is c where no byte was queued before.
 *
 * where the buffer stays full for MESS_FULL_TRIES tests, the call drops c
 * and returns 0. while close_port is set, it drops c and returns 1 */
char vxd_dte_tx_cmd_byte(Uint8 c);
/* the host's analog loopback request: always 1, which by vxd_dte_get_dtr's
 * sense is off */
char dte_get_anl(void);
/* return 1 while the port's mcr has COM_MCR_DTR clear, and 0 while it is
 * set */
char vxd_dte_get_dtr(void);
/* the host's remote digital loopback request: always 1, off, as dte_get_anl's
 * is */
char dte_get_rdl(void);
/* turn the port's CTS line on or off, and raise COM_EVENT_MSR.
 * vxd_dte_set_dcd, vxd_dte_set_dsr and vxd_dte_set_ri do the same for DCD,
 * DSR and RI */
void vxd_dte_set_cts(char on);
void vxd_dte_set_dcd(char on);
void vxd_dte_set_dsr(char on);
void vxd_dte_set_ri(char on);
/* do nothing */
void dte_set_tst(char on);
/* raise event, COM_EVENT_ bits, to the host's side. it raises none while
 * init_mem is 0 or close_port is set */
void GenerateIRQ(Uint16 event);
/* do nothing */
void vxd_dte_dce_rx_idle(void);
/* do nothing, and return 0 */
int vxd_dte_async_rx_direct_mode(Uint8 *data, Uint8 *control);
/* do nothing */
void vxd_dte_async_tx_direct_mode(Uint8 c);
/* call the transmit or receive callback dteSetCallback gave, until
 * dteStopCallback */
void dteExecCallbackTx(void);
void dteExecCallbackRx(void);

#endif
