/* the modem's side of the port's events, which com_memory_interrupt serves */
#include <sg_xpt.h>

#include "dte_vxd.h"
#include "ecdc_def.h"
#include "messages.h"
#include "modemdef.h"
#include "targdef.h"
#include "target.h"
#include "tci_modm.h"
#include "vxd_int.h"

/* the character of a control character entry that holds none */
#define NO_CCHAR 0xff

Uint8 PCRWER_SHADDOW;
Uint8 MD_IER_SHADDOW;
Uint8 RATE_SHADDOW;
Uint8 LCR_SHADDOW;
Uint8 MCR_SHADDOW;

void com_memory_interrupt(void)
{
    /* @hack spelling: pending, a pointer to MD_IER_SHADDOW, where
     * MD_IER_SHADDOW itself would do */
    Uint8 *pending = &MD_IER_SHADDOW;
    volatile struct com_memory *port = &Buffer_memory;
    *pending = port->md_ier;
    if (*pending == COM_MD_RESET) {
        init_mem = 0;
        init_com_memory();
        port->md_ier = 0;
        return;
    }
    port->md_ier = 0;
    if (!(port->md_mask & COM_MD_MCR) && (*pending & COM_MD_MCR)) {
        if (RATE_SHADDOW != port->baud) {
            save_rate(port->baud);
        }
        if (MCR_SHADDOW != port->mcr) {
            save_MCR();
        }
    }
    if (!(port->md_mask & COM_MD_RX) && (*pending & COM_MD_RX)) {
        update_PC_rx_cnt();
    }
    if (!(port->md_mask & COM_MD_TX) && (*pending & COM_MD_TX)) {
        update_PC_tx_cnt();
    }
    if (!(port->md_mask & COM_MD_CCHAR) && (*pending & COM_MD_CCHAR)) {
        vxd_stop_esc_code();
    }
    if (!(port->md_mask & COM_MD_PRIO_SENT) && (*pending & COM_MD_PRIO_SENT)) {
        empty_messg_buffer();
    }
    if (!(port->md_mask & COM_MD_CMD) && (*pending & COM_MD_CMD)) {
        rx_cmd_buff_rdy();
    }
    if (!(port->md_mask & COM_MD_LCR) && (*pending & COM_MD_LCR)) {
        save_LCR();
    }
}

void init_com_memory(void)
{
    Uint8 i;
    close_port = 0;
    Buffer_memory.mrblr = COM_BUFFER_SIZE;
    Buffer_memory.unk_3e = 0;
    Buffer_memory.md_mask = 0;
    Buffer_memory.unk_48 = 0;
    /* each entry holds no control character. its flags set every bit but
     * CCHAR_REJECT, and CCHAR_END among them marks the entry unused */
    Buffer_memory.cchar.entry.stop_char = NO_CCHAR;
    Buffer_memory.cchar.entry.stop_flags = (Uint8)~CCHAR_REJECT;
    Buffer_memory.cchar.entry.start_char = NO_CCHAR;
    Buffer_memory.cchar.entry.start_flags = (Uint8)~CCHAR_REJECT;
    Buffer_memory.cchar.entry.escape_char = NO_CCHAR;
    Buffer_memory.cchar.entry.escape_flags = (Uint8)~CCHAR_REJECT;
    Buffer_memory.prio_count = 0;
    Buffer_memory.msr = COM_MSR_CTS;
    Buffer_memory.mode = COM_MODE_COMMAND;
    for (i = 0; i < COM_PRIO_SIZE; ++i) {
        Buffer_memory.prio[i] = 0;
    }
    MESS_READ = MESS_BUFFER;
    MESS_WRITE = MESS_BUFFER;
    MESS_TOPP = MESS_BUFFER + MESS_SIZE - 1;
    MESS_CNT = 0;
    for (i = 0; i < COM_BUFFERS; ++i) {
        Buffer_memory.tx[i].status.all = (Uint8)~COM_STATUS_CCHAR;
        Buffer_memory.tx[i].length = 0;
    }
    dte_rx_cnt = 0;
    dte_rx_read = 0;
    dte_rx_write = 0;
    dte_tx_cnt = 0;
    dte_tx_read = 0;
    dte_tx_write = 0;
    RATE_SHADDOW = 23;
    LCR_SHADDOW = COM_LCR_WLS0 | COM_LCR_WLS1;
    MCR_SHADDOW = 0;
    Buffer_memory.event_lo = 0;
    Buffer_memory.event_hi = 0;
    init_mem = 1;
}

void empty_messg_buffer(void)
{
    volatile struct com_memory *port;
    Uint8 i;
    DTE_INAC_TIM = 0;
    port = &Buffer_memory;
    port->prio_count = 0;
    if (MESS_CNT == 0) {
        if (next_dte_type == DTE_RATE_CMD) {
            vxd_dte_set_interface(next_dte_type, speed_bak, format_bak,
                                  flwctl_bak, S2_bak, S12_bak, XON_bak,
                                  XOFF_bak, rx_break_type_bak);
        }
    } else {
        i = 0;
        while (i < COM_PRIO_SIZE && MESS_CNT != 0) {
            port->prio[i++] = *MESS_READ;
            MESS_READ++;
            if (MESS_READ > MESS_TOPP) {
                MESS_READ = MESS_BUFFER;
            }
            --MESS_CNT;
        }
        port->prio_count = i;
        GenerateIRQ(COM_EVENT_PRIO);
    }
}

void update_PC_rx_cnt(void)
{
    Uint8 last;
    /* @hack redundant: register on length and packet */
    register Uint16 length;
    volatile struct com_memory *port;
    register volatile struct com_buffer *packet;
    port = &Buffer_memory;
    packet = &port->rx[dte_rx_write];
    length = packet->length;
#ifdef NONMATCHING
    /* the GCC build reads no last byte where length is 0 or past
     * COM_BUFFER_SIZE */
    if (length == 0 || length > COM_BUFFER_SIZE) {
        DTE_INAC_TIM = 0;
        vxd_stop_esc_code();
    } else {
#endif
        last = packet->data[length - 1];
        DTE_INAC_TIM = 0;
        /* @hack spelling: the buffer indexed again, where packet would do */
        if ((port->rx[dte_rx_write].status.all & COM_STATUS_CCHAR) &&
            (last & SEVEN_BITS) == dte_S2) {
            escape_code();
        } else {
            vxd_stop_esc_code();
        }
        /* an escape character that ends a buffer counts at least as the
         * first. it counts whether the buffer closed on it or not.
         * BMR21_LEAD_GUARD holds the count back */
        if (!(BMR(21) & BMR21_LEAD_GUARD) &&
            ((BMR(14) & BMR14_ESC_ON) || !(MD_INT_MASK_MCR & MASK_ESC_ON)) &&
            (last & SEVEN_BITS) == dte_S2) {
            BMR(14) |= BMR14_ESC_FIRST;
        }
        /* never holds: where last isn't the escape character,
         * vxd_stop_esc_code above has cleared BMR21_GUARD */
        if (((BMR(14) & BMR14_ESC_FIRST) || (BMR(14) & BMR14_ESC_SECOND)) &&
            (last & SEVEN_BITS) != dte_S2 && (BMR(21) & BMR21_GUARD)) {
            BMR(14) &= (Uint8)~BMR14_ESC_COUNT;
            BMR(21) &= (Uint8)~BMR21_ESCAPE;
        }
#ifdef NONMATCHING
        /* this side closes the else its test above opens */
    }
#endif
    dte_rx_write = (dte_rx_write + 1) & (COM_BUFFERS - 1);
    ++dte_rx_cnt;
    dteExecCallbackRx();
}

void update_PC_tx_cnt(void)
{
    Buffer_memory.tx[dte_tx_read].length = 0;
    DTE_INAC_TIM = 0;
    dte_tx_read = (dte_tx_read + 1) & (COM_BUFFERS - 1);
    if (dte_tx_cnt) {
        --dte_tx_cnt;
    }
    if (Buffer_memory.tx[dte_tx_read].breaks.queue.first != 0) {
        vxd_send_next_dte_break();
    }
    dteExecCallbackTx();
}

void rx_cmd_buff_rdy(void)
{
    /* command holds the count, then the bytes */
    Uint8 command[COM_CMD_SIZE + 1];
    Uint8 i;
#ifdef NONMATCHING
    /* the GCC build takes no more of cmd's bytes than cmd holds */
    command[0] = Buffer_memory.cmd_count;
    if (command[0] > COM_CMD_SIZE) {
        command[0] = COM_CMD_SIZE;
    }
    if (command[0]) {
        DTE_INAC_TIM = 0;
        for (i = 0; i < command[0]; ++i) {
            command[i + 1] = Buffer_memory.cmd[i];
        }
#else
    /* @hack spelling: cmd_count ? 1 : 0, where cmd_count itself would do */
    if (Buffer_memory.cmd_count ? 1 : 0) {
        DTE_INAC_TIM = 0;
        for (i = 0; i < Buffer_memory.cmd_count; ++i) {
            command[i + 1] = Buffer_memory.cmd[i];
        }
        command[0] = Buffer_memory.cmd_count;
#endif
        for (i = 1; i <= command[0]; ++i) {
            parity_adjust(&command[i]);
            TCI_dte_rx_cmd_byte(command[i]);
        }
        GenerateIRQ(COM_EVENT_CMD_DONE);
    }
}

void save_rate(Uint8 baud)
{
    Uint8 *shadow;
    volatile struct com_memory *port;
    TCI_got_format(baud, format_of_AT);
    port = &Buffer_memory;
    /* @hack spelling: shadow, a pointer to RATE_SHADDOW, where RATE_SHADDOW
     * itself would do */
    shadow = &RATE_SHADDOW;
    *shadow = port->baud;
    port->speed = *shadow;
    GenerateIRQ(COM_EVENT_SPEED);
}

void save_LCR(void)
{
    volatile struct com_memory *port = &Buffer_memory;
    if ((port->lcr & COM_LCR_BREAK) == (LCR_SHADDOW & COM_LCR_BREAK)) {
        get_format_from_PC();
    } else if (!(port->mode & COM_MODE_COMMAND)) {
        check_break_rx_from_PC();
    }
    LCR_SHADDOW = port->lcr;
}

void get_format_from_PC(void)
{
    Uint8 format = FORMAT_8_BITS;
    Uint8 lcr = Buffer_memory.lcr;
    /* 7-bit bytes without parity, or with mark parity, take FORMAT_7_BITS.
     * other sizes take FORMAT_8_BITS, as does space parity */
    if (!(lcr & COM_LCR_WLS0) && (lcr & COM_LCR_WLS1)) {
        if (!(lcr & COM_LCR_STICK) && (lcr & COM_LCR_PARITY)) {
            if (lcr & COM_LCR_EVEN) {
                format = FORMAT_EVEN_PARITY;
            } else {
                format = FORMAT_ODD_PARITY;
            }
        } else if (!(lcr & COM_LCR_PARITY) || !(lcr & COM_LCR_EVEN)) {
            format = FORMAT_7_BITS;
        }
    }
    if (format != format_of_AT) {
        TCI_got_format(Buffer_memory.baud, format);
    }
}

void check_break_rx_from_PC(void)
{
    if (Buffer_memory.lcr & COM_LCR_BREAK) {
        vxd_stop_esc_code();
        DTE_INAC_TIM = 0;
        if (!(BREAK_FLAGS & BREAK_FROM_HOST)) {
            BREAK_FLAGS |= BREAK_FROM_HOST;
            DTE_BRK_LEN = 0;
            BREAK_FLAGS &= (Uint8)~BREAK_END;
        }
        /* @hack spelling: the bit compared with BREAK_FROM_HOST, where its
         * test alone would do */
    } else if ((BREAK_FLAGS & BREAK_FROM_HOST) == BREAK_FROM_HOST) {
        BREAK_FLAGS |= BREAK_END;
        break_check();
    }
}

void save_MCR(void)
{
    volatile struct com_memory *port = &Buffer_memory;
    if ((port->mcr & COM_MCR_IDLE) != (MCR_SHADDOW & COM_MCR_IDLE) &&
        (port->mcr & COM_MCR_IDLE) && !(MD_INT_MASK_MCR & MASK_ESC_ON)) {
        vxd_prog_esc_code();
    }
    MCR_SHADDOW = port->mcr;
}

void escape_code(void)
{
    Uint32 now;
#ifdef NONMATCHING
    /* this side keeps no deadline. its test below repairs the bug tagged
     * there */
#else
    /* @bug arithmetic: an escape character after reset_esc_seq counts before
     * its guard time has passed, unless TIM_10MS and the guard time's end both
     * lie below 256. the code at 0x0caf7538 keeps only the end's low byte in
     * deadline, and the test compares that with the whole of TIM_10MS */
    Uint8 deadline;
#endif
    Uint8 last;
    /* @hack spelling: the assignment to now inside the test, where a statement
     * of its own would do */
    if (now = TIM_10MS, BMR(21) & BMR21_LEAD_GUARD) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it compares the ticks since
         * ESC_TOUT with the whole guard time */
        if (now - ESC_TOUT < (Uint32)dte_S12 * S12_TICKS) {
#else
        /* @hack type: an int 2, where S12_TICKS, an unsigned 2U, would do */
        deadline = ESC_TOUT + (dte_S12 * 2);
        if (deadline > now) {
#endif
            ESC_TOUT = now;
            return;
        }
#ifdef NONMATCHING
        /* this side clears the guard unconditionally, the plain form the hack
         * tag below gives */
        BMR(21) &= (Uint8)~BMR21_LEAD_GUARD;
#else
        /* @hack redundant: a test of deadline <= now, which the return above
         * makes always true */
        if (deadline <= now) {
            BMR(21) &= (Uint8)~BMR21_LEAD_GUARD;
        }
#endif
    }
    if (BMR(21) & BMR21_TEXT_VALID) {
        volatile struct com_buffer *packet;
        /* @hack spelling: packet set inside length's initializer, where a
         * statement of its own would do */
        Uint16 length = (packet = &Buffer_memory.rx[dte_rx_write])->length;
#ifdef NONMATCHING
        /* the GCC build reads no last byte where length is 0 or past
         * COM_BUFFER_SIZE */
        if (length == 0 || length > COM_BUFFER_SIZE) {
            return;
        }
#endif
        last = packet->data[length - 1];
        if (TIES_validation(last)) {
            return;
        }
        /* the second test runs TIES_validation on last again, once the first
         * has refused it and restarted TIES. a refusal's force_o_cmd can then
         * run twice */
        if (!TIES_validation(last)) {
            BMR(14) &= (Uint8)~BMR14_ESC_COUNT;
            BMR(21) &= (Uint8)~BMR21_ESCAPE;
            BMR(14) |= BMR14_ESC_FIRST;
            return;
        }
    }
    if (BMR(14) & BMR14_ESC_SECOND) {
        if (BMR(21) & BMR21_ESC_THIRD) {
            reset_esc_seq();
            return;
        }
        ESC_TOUT = TIM_10MS;
        BMR(21) |= BMR21_TEXT | BMR21_ESC_THIRD | BMR21_GUARD;
    } else if (BMR(14) & BMR14_ESC_FIRST) {
        BMR(14) |= BMR14_ESC_SECOND;
    } else {
        BMR(14) |= BMR14_ESC_FIRST;
    }
}
void reset_esc_seq(void)
{
    BMR(21) &= (Uint8)~BMR21_TEXT;
    vxd_stop_esc_code();
    BMR(21) |= BMR21_LEAD_GUARD;
    ESC_TOUT = TIM_10MS;
}
