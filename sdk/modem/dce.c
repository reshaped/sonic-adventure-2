#include <sg_xpt.h>
#include <string.h>
#include "dce.h"
#include "dce_line.h"
#include "dce_sync.h"
#include "ecdcutil.h"
#include "messages.h"
#include "roc_k56.h"
#include "targdef.h"
#include "target.h"
#include "tci_ecdc.h"
#include "v14.h"

/* the codes dce reports to data_pump_error. dce_set_interface reports
 * DCE_ERR_TYPE for a type it doesn't take. each other code is a loop of
 * dce_interrupt's that would pass FIFO_BUDGET */
#define DCE_ERR_TYPE 3100
#define DCE_ERR_ASYNC_RX 3120
#define DCE_ERR_ASYNC_TX 3140
#define DCE_ERR_HDLC_BYTE_RX 3160
#define DCE_ERR_HDLC_RX 3180
#define DCE_ERR_HDLC_BYTE_TX 3200
#define DCE_ERR_HDLC_TX 3220
/* a frame's breaks follow its data rather than lead it. dce_tx_brk sets the
 * bit on a full ring, where the slot it writes holds the oldest frame */
#define DCE_FRAME_DEFER_BREAK 4
#define DCE_FRAME_ERRORS                                                       \
    (DCE_FRAME_BAD_CRC | DCE_FRAME_ABORT | DCE_FRAME_TOO_LONG)
/* the longest frame dce_del_info_frms leaves queued. it empties each longer
 * one, which carries information */
#define SHORT_FRAME_BYTES 64
#define RX_HIGH_WATER 6
/* the most bytes one call of dce_interrupt moves through the pump's FIFOs */
#define FIFO_BUDGET 500
/* a word's most significant byte, in the SH-4's little-endian order, where
 * soft_hdlc_transmit puts the bits it returns */
#define TOP_BYTE 3
#define NEXT_FRAME(index) (((index) + 1) & (DCE_FRAMES - 1))
/* a ring's front is its oldest frame, and its back the slot its next frame
 * fills */
#define RX_FRONT dce_rx_buffer[dce_rx_read]
#define TX_FRONT dce_tx_buffer[dce_tx_read]
#define RX_BACK dce_rx_buffer[dce_rx_write]
#define TX_BACK dce_tx_buffer[dce_tx_write]

Uint8 dce_rx_read;
Uint8 dce_tx_read;
Uint8 dce_rx_write;
Uint8 dce_tx_write;
struct dce_pump_buffer DpTxBuffer;
struct dce_pump_buffer DpRxBuffer;
Uint8 drx_break_type;
Uint8 crc_length;
struct dce_frame dce_rx_buffer[DCE_FRAMES];
struct dce_frame dce_tx_buffer[DCE_FRAMES];

/* a ring's frame count is its produced count less its consumed count. its
 * read and write indices meet when it is empty and when it is full. only the
 * counts tell the two apart */
static Uint16 rx_produced;
static Uint16 rx_consumed;
static Uint16 tx_produced;
static Uint16 tx_consumed;
static Uint32 tx_idle_since;
static char ignore_async;
static Uint16 hdlc_errors;
static Uint8 rx_overrun;
static Uint8 tx_irq_driven;

static void record_overrun(void);
static Uint16 receive_count(void);

#pragma inline(clear_ring)
static void clear_ring(struct dce_frame *ring)
{
    memset(ring, 0, DCE_FRAMES * sizeof *ring);
}

void dce_clr_rx_buff(void)
{
    char installed;
    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if ((installed = targetIsDpIrqInstalled()) != 0) {
        target_InstallDpIrq(NULL);
    }
    dce_rx_read = 0;
    dce_rx_write = 0;
    rx_produced = 0;
    rx_consumed = 0;
    clear_ring(dce_rx_buffer);
    switch (dce_type) {
    case DCE_ASYNC:
    case DCE_DIRECT_1:
    case DCE_DIRECT_2:
        DpRxBuffer.count = 0;
        v14InitializeRx();
        break;
    case DCE_SYNC:
        rx_phase = HDLC_IDLE;
        break;
    case DCE_HDLC16:
    case DCE_HDLC32:
        DpRxBuffer.count = 0;
        init_dce_hdlc_rx();
        break;
    }
    if (installed) {
        target_InstallDpIrq(dp_interrupt);
    }
}

void dce_clr_tx_buff(void)
{
    char installed;
    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if ((installed = targetIsDpIrqInstalled()) != 0) {
        target_InstallDpIrq(NULL);
    }
    dce_tx_read = 0;
    dce_tx_write = 0;
    tx_produced = 0;
    tx_consumed = 0;
    clear_ring(dce_tx_buffer);
    switch (dce_type) {
    case DCE_ASYNC:
    case DCE_DIRECT_1:
    case DCE_DIRECT_2:
        DpTxBuffer.count = sizeof DpTxBuffer.data;
        v14InitializeTx();
        break;
    case DCE_SYNC:
        tx_phase = HDLC_IDLE;
        break;
    case DCE_HDLC16:
    case DCE_HDLC32:
        DpTxBuffer.count = sizeof DpTxBuffer.data;
        init_dce_hdlc_tx();
        break;
    }
    if (installed) {
        target_InstallDpIrq(dp_interrupt);
    }
}

void dce_del_info_frms(void)
{
    Uint8 slot;
    switch (dce_type) {
    case DCE_RAW:
    case DCE_RAW_ASYNC:
        break;
    default:
        if (tx_dce_buffer_wcnt > SHORT_FRAME_BYTES) {
            dceSetTxPhase(HDLC_ABORT);
        }
        break;
    }
    /* @hack spelling: slot <= DCE_FRAMES - 1, where slot < DCE_FRAMES
     * would do */
    for (slot = 0; slot <= DCE_FRAMES - 1; slot++) {
        if (dce_tx_buffer[slot].length > SHORT_FRAME_BYTES) {
            dce_tx_buffer[slot].status = 0;
            dce_tx_buffer[slot].length = 0;
        }
    }
}
Uint8 dce_get_amount_of_frames(void)
{
    return dce_transmit_count();
}

/* @hack spelling: i <= DCE_FRAME_BREAKS - 1, where i < DCE_FRAME_BREAKS
 * would do */
#define SHIFT_BREAKS(frame, i)                                                 \
    for ((i) = 1; (i) <= DCE_FRAME_BREAKS - 1; (i)++)                          \
        (frame).breaks[(i) - 1] = (frame).breaks[i];                           \
    (frame).breaks[DCE_FRAME_BREAKS - 1] = 0

Uint8 dce_get_rx_data(Uint8 **data, Uint16 *length)
{
    Uint8 slot;
    Uint8 status;
    switch (dce_type) {
    case DCE_ASYNC:
    case DCE_DIRECT_1:
    case DCE_DIRECT_2:
    case DCE_LOOPBACK:
        if (dce_speed <= TCI_SPEED_300) {
            async_receive_idle_timeout();
        }
        break;
    }
    if (!(RX_FRONT.status & DCE_FRAME_DEFER_BREAK)) {
        while (RX_FRONT.breaks[0]) {
            TCI_dce_brk_rx(RX_FRONT.breaks[0]);
            SHIFT_BREAKS(RX_FRONT, slot);
        }
    }
    *length = 0;
    /* @hack spelling: each return of 0 inside an if-else, where an early
     * return would do */
    if (receive_count()) {
        *data = RX_FRONT.data;
        *length = RX_FRONT.length;
    } else {
        return 0;
    }
    status = RX_FRONT.status;
    if (!(status & DCE_FRAME_ERRORS)) {
        return 0;
    } else {
        if (dce_type != DCE_RAW) {
            *length = 0;
            dce_rx_data_done();
            hdlc_errors++;
        }
        /* @hack redundant: the chain's tests of 0x10 and 0x20, bits
         * DCE_FRAME_ERRORS leaves out, so neither holds here */
        if (status & DCE_FRAME_BAD_CRC) {
            status = 1;
        } else if (status & DCE_FRAME_ABORT) {
            status = 2;
        } else if (status & DCE_FRAME_TOO_LONG) {
            status = 3;
        } else if (status & 0x10) {
            status = 4;
        } else if (status & 0x20) {
            status = 5;
        }
        return status;
    }
}

Uint16 dce_get_hdlc_rx_error_cnt(void)
{
    Uint16 count;
    if (dce_type != DCE_HDLC16 && dce_type != DCE_HDLC32) {
        return 0;
    }
    count = hdlc_errors;
    hdlc_errors = 0;
    return count;
}

void dce_rx_data_done(void)
{
    if (receive_count()) {
        RX_FRONT.status = 0;
        RX_FRONT.length = 0;
        dce_rx_read = NEXT_FRAME(dce_rx_read);
        rx_consumed++;
    }
}

void dce_set_interface(Uint8 type, Uint8 speed, Uint8 format, Uint8 flow,
                       Uint8 xon, Uint8 xoff, Uint8 rx_break_type)
{
    if (type == DCE_ASYNC || type == DCE_RAW_ASYNC) {
        dflwctl = flow;
    } else {
        dflwctl = 0;
    }
    drx_break_type = rx_break_type;
    dp_disable_data_mode();
    tx_irq_driven = 0;
    /* @hack spelling: the assignment inside the switch, where a statement of
     * its own would do */
    switch (dce_type = type) {
    case DCE_ASYNC:
    case DCE_DIRECT_1:
    case DCE_DIRECT_2:
    case DCE_LOOPBACK:
    /* configure_dce passes DCE_ASYNC in place of DCE_FORMAT_FE, so this label
     * never matches */
    case DCE_FORMAT_FE:
        dce_speed = speed;
        if (speed <= TCI_SPEED_300) {
            dp_set_async_mode();
            v14Initialize(type, speed, format, dflwctl);
            dceSetTxPhase(HDLC_IDLE);
            dp_enable_data_mode(1, 0, speed);
            tx_irq_driven = 1;
        } else {
            DpRxBuffer.count = 0;
            DpTxBuffer.count = sizeof DpTxBuffer.data;
            v14Initialize(type, speed, format, dflwctl);
            dp_enable_data_mode(0, 0, speed);
        }
        break;
    case DCE_HDLC16:
    case DCE_HDLC32:
    case 29:
        hdlc_errors = 0;
        if (type == DCE_HDLC16) {
            crc_length = FCS16_BYTES;
        } else {
            crc_length = FCS32_BYTES;
        }
        DpTxBuffer.count = sizeof DpTxBuffer.data;
        DpRxBuffer.count = 0;
        init_dce_hdlc();
        dp_enable_data_mode(0, 0, speed);
        break;
    case DCE_DIRECT_SYNC:
    case DCE_HELD:
        break;
    case DCE_SYNC:
        dp_set_sync_mode(0);
        init_dce_sync(speed);
        dp_enable_data_mode(3, 0, speed);
        tx_irq_driven = 1;
        break;
    case DCE_RAW:
    case DCE_RAW_ASYNC:
        break;
    default:
        data_pump_error(DCE_ERR_TYPE);
        break;
    }
    dce_format = format;
    dce_speed = speed;
    dce_rx_break_type = rx_break_type;
    dce_xon = xon;
    dce_xoff = xoff;
}

Uint16 dce_set_tx_data(Uint8 **data)
{
    /* @hack spelling: the count <= DCE_FRAMES - 1, where < DCE_FRAMES would
     * do */
    if (dce_transmit_count() <= DCE_FRAMES - 1) {
        *data = TX_BACK.data;
        return sizeof TX_BACK.data;
    }
    return 0;
}
Uint8 dce_tx_brk(Uint16 length, Uint8 queued)
{
    Uint8 slot;
    if (!queued) {
        setup_timer(&tx_break_timer, length);
        tx_break_flag = TX_BREAK_DUE;
        return 1;
    }
    /* @hack spelling: slot <= DCE_FRAME_BREAKS - 1, where
     * slot < DCE_FRAME_BREAKS would do */
    for (slot = 0; slot <= DCE_FRAME_BREAKS - 1; slot++) {
        if (TX_BACK.breaks[slot] == 0) {
            TX_BACK.breaks[slot] = length;
            if (dce_transmit_count() == DCE_FRAMES) {
                TX_BACK.status |= DCE_FRAME_DEFER_BREAK;
            }
            return 1;
        }
    }
    return 0;
}

Uint8 dce_tx_buff_empty(void)
{
    if (dce_transmit_count()) {
        return 0;
    }
    if (timer_expired(tx_idle_since + dp_get_tx_fifo_empty_time())) {
        return 1;
    }
    return 0;
}

void dce_tx_data_done(Uint16 length, char unused)
{
#ifdef NONMATCHING
    /* this side refuses a length past the frame's data, and a frame while all
     * DCE_FRAMES are queued */
    if (length > sizeof TX_BACK.data || dce_transmit_count() >= DCE_FRAMES) {
        return;
    }
#endif
    if (length != 0) {
        TX_BACK.length = length;
        dce_tx_write = NEXT_FRAME(dce_tx_write);
        tx_produced++;
        switch (dce_type) {
        case DCE_RAW:
        case DCE_RAW_ASYNC:
            break;
        default:
            if (tx_irq_driven && !dp_tx_irqs_enabled()) {
                dp_enable_tx_irqs();
            }
            break;
        }
    }
}
void dce_mark_current_rx_state(void)
{
    char installed;
    if (dce_type != DCE_SYNC) {
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if ((installed = targetIsDpIrqInstalled()) != 0) {
        target_InstallDpIrq(NULL);
    }
    dceRxSyncMarker.frames = receive_count();
    dceRxSyncMarker.length = rx_dce_buffer_wcnt;
    if (installed) {
        target_InstallDpIrq(dp_interrupt);
    }
}

void dce_get_marked_rx_data(Uint8 **data, Uint16 *length)
{
    if (dce_type != DCE_SYNC || dceRxSyncMarker.frames < 0) {
        *length = 0;
        return;
    }
    *data = RX_FRONT.data;
    if (dceRxSyncMarker.frames-- == 0) {
        *length = dceRxSyncMarker.length;
    } else {
        *length = RX_FRONT.length;
    }
}

void send_next_dce_break(void)
{
}
void dce_sync_tx_direct_mode(void)
{
}
void dce_sync_rx_direct_mode(void)
{
}
void end_of_dce_brk_rx(Uint16 length)
{
}

void dce_ignore_rx_async_data(char ignore)
{
    ignore_async = ignore;
}

void open_tx_buffer(void)
{
    Uint8 slot;
    switch (dce_type) {
    case 22:
    case 23:
    case 24:
        break;
    default:
#ifdef NONMATCHING
        /* this side repairs the bug tagged below: a frame that doesn't defer
         * its break sends it */
        if (!(TX_FRONT.status & DCE_FRAME_DEFER_BREAK) && TX_FRONT.breaks[0]) {
#else
        /* @bug logic: no break that dce_tx_brk queues is ever sent. the test
         * ORs DCE_FRAME_DEFER_BREAK into the status, at 0x0caf0536, so it
         * never holds */
        /* NOLINTNEXTLINE(misc-redundant-expression) */
        if ((TX_FRONT.status | DCE_FRAME_DEFER_BREAK) == 0) {
#endif
            if (TX_FRONT.breaks[0]) {
                tx_buffer_flag = TX_FRAME_BREAK;
                tx_break_flag = TX_BREAK_FRAMED;
                setup_timer(&tx_break_timer, TX_FRONT.breaks[0]);
                SHIFT_BREAKS(TX_FRONT, slot);
            }
        } else if (dce_transmit_count()) {
            tx_buffer_flag = TX_FRAME_OPEN;
            TX_FRONT.status = 0;
            tx_dce_buffer_wptr = TX_FRONT.data;
            tx_dce_buffer_wcnt = TX_FRONT.length;
        } else {
            tx_buffer_flag = TX_NO_FRAME;
            tx_dce_buffer_wcnt = 0;
        }
        break;
    }
}

void close_tx_buffer(void)
{
    switch (dce_type) {
    case 22:
    case 23:
    case 24:
        break;
    default:
        TX_FRONT.status = 0;
        TX_FRONT.length = 0;
        dce_tx_read = NEXT_FRAME(dce_tx_read);
        tx_consumed++;
        setup_timer(&tx_idle_since, 0);
        break;
    }
}

void open_rx_buffer(void)
{
    switch (dce_type) {
    case 22:
    case 23:
    case 24:
        break;
    default:
        dce_rx_buffer_available = 0;
        if (ignore_async == 1) {
            switch (dce_type) {
            case DCE_ASYNC:
            case DCE_DIRECT_1:
            case DCE_DIRECT_2:
                return;
            }
        }
        switch (dce_type) {
        case DCE_LOOPBACK:
            if (dce_transmit_count() < DCE_FRAMES) {
                rx_dce_buffer_wcnt = 0;
                rx_dce_buffer_wptr = TX_BACK.data;
                TX_BACK.status &= DCE_FRAME_DEFER_BREAK;
                dce_rx_buffer_available = 1;
                rx_overrun = 0;
            } else {
                record_overrun();
            }
            break;
        default:
            if (receive_count() < DCE_FRAMES) {
                rx_dce_buffer_wcnt = 0;
                rx_dce_buffer_wptr = RX_BACK.data;
                RX_BACK.status &= DCE_FRAME_DEFER_BREAK;
                dce_rx_buffer_available = 1;
                rx_overrun = 0;
            } else {
                record_overrun();
            }
            break;
        }
        break;
    }
}

void close_rx_buffer(void)
{
    switch (dce_type) {
    case 22:
    case 23:
    case 24:
        break;
    default:
        if (dce_type == DCE_LOOPBACK) {
            TX_BACK.length = rx_dce_buffer_wcnt;
            rx_dce_buffer_wcnt = 0;
            dce_tx_write = NEXT_FRAME(dce_tx_write);
            tx_produced++;
        } else {
            RX_BACK.length = rx_dce_buffer_wcnt;
            rx_dce_buffer_wcnt = 0;
            dce_rx_write = NEXT_FRAME(dce_rx_write);
            rx_produced++;
        }
        if (dflwctl != 0) {
            if (rx_dce_flow == 1) {
                if (receive_count() >= RX_HIGH_WATER) {
                    rx_dce_flow = 0;
                    rx_dce_flow_flag = 1;
                }
            }
        }
        break;
    }
}

/* the pump's FIFOs move bytes, which DpRxBuffer and DpTxBuffer gather into
 * words across calls, and count. V.14 and the HDLC coder take a word's 32
 * bits at a time. at 300 bps or slower, asynchronous characters pass a byte
 * at a time, through async_receive and async_sync_transmit. at 300 bps
 * exactly, the HDLC coder takes 8 bits at a time */
void dce_interrupt(void)
{
    /* @hack redundant: budget and count declared register, where plain locals
     * would do */
    register Uint16 budget = FIFO_BUDGET;
    register Uint16 count;

    switch (dce_type) {
    case DCE_ASYNC:
    case DCE_LOOPBACK:
        if (dce_speed <= TCI_SPEED_300) {
            async_receive();
            async_sync_transmit();
        } else {
            count = DpRxBuffer.count;
            while (dp_get_rx_data_byte(&DpRxBuffer.data.byte[count])) {
                if (budget-- == 0) {
                    data_pump_error(DCE_ERR_ASYNC_RX);
                    return;
                }
                if (++count == sizeof DpRxBuffer.data) {
                    v14_receive(DpRxBuffer.data.word, 32);
                    count = 0;
                }
            }
            DpRxBuffer.count = count;
            count = DpTxBuffer.count;
            while (dp_ready_to_tx_data()) {
                if (budget-- == 0) {
                    data_pump_error(DCE_ERR_ASYNC_TX);
                    return;
                }
                if (count == sizeof DpTxBuffer.data) {
                    if (dce_transmit_count()) {
                        DpTxBuffer.data.word = v14_transmit(32);
                        count = 0;
                    } else {
                        dp_fill_tx_fifo();
                        break;
                    }
                }
                dp_tx_data_byte(DpTxBuffer.data.byte[count++]);
            }
            DpTxBuffer.count = count;
        }
        break;
    case DCE_HDLC16:
    case DCE_HDLC32:
    case 29:
        if (dce_speed == TCI_SPEED_300) {
            while (dp_get_rx_data_byte(DpRxBuffer.data.byte)) {
                if (budget-- == 0) {
                    data_pump_error(DCE_ERR_HDLC_BYTE_RX);
                    return;
                }
                soft_hdlc_receive(DpRxBuffer.data.word, 8);
            }
        } else {
            count = DpRxBuffer.count;
            while (dp_get_rx_data_byte(&DpRxBuffer.data.byte[count])) {
                if (budget-- == 0) {
                    data_pump_error(DCE_ERR_HDLC_RX);
                    return;
                }
                if (++count == sizeof DpRxBuffer.data) {
                    soft_hdlc_receive(DpRxBuffer.data.word, 32);
                    count = 0;
                }
            }
            DpRxBuffer.count = count;
        }
        if (dce_speed == TCI_SPEED_300) {
            while (dp_ready_to_tx_data()) {
                if (budget-- == 0) {
                    data_pump_error(DCE_ERR_HDLC_BYTE_TX);
                    return;
                }
                DpTxBuffer.data.word = soft_hdlc_transmit(8);
                dp_tx_data_byte(DpTxBuffer.data.byte[TOP_BYTE]);
            }
        } else {
            count = DpTxBuffer.count;
            while (dp_ready_to_tx_data()) {
                if (budget-- == 0) {
                    data_pump_error(DCE_ERR_HDLC_TX);
                    return;
                }
                if (count == sizeof DpTxBuffer.data) {
                    DpTxBuffer.data.word = soft_hdlc_transmit(32);
                    count = 0;
                }
                dp_tx_data_byte(DpTxBuffer.data.byte[count++]);
            }
            DpTxBuffer.count = count;
        }
        break;
    case DCE_SYNC:
        sync_receive();
        async_sync_transmit();
        break;
    }
}

void dce_Open(void)
{
}
void dce_Close(void)
{
}

static void record_overrun(void)
{
    /* @hack redundant: the test before the store, which leaves rx_overrun 1
     * either way */
    if (!rx_overrun) {
        rx_overrun = 1;
    }
}
void dceUnderrun(void)
{
}

Uint8 rx_buffer_low(void)
{
    /* @hack spelling: low set by a test, where returning the comparison would
     * do */
    Uint8 low = 0;
    if (receive_count() <= DCE_FRAMES / 2) {
        low = 1;
    }
    return low;
}
static Uint16 receive_count(void)
{
    return rx_produced - rx_consumed;
}
Uint16 dce_transmit_count(void)
{
    return tx_produced - tx_consumed;
}
