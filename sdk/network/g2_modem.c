#include <machine.h>
#include <mdmAPI.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include <access/dte_if.h>
#include "g2_modem.h"
#include "mdm_images.h"
#include "modem_backends.h"
/* g2_line's mcr and lcr and g2_status's msr hold uart_hardware.h's bits */
#include "uart_hardware.h"

/* how long g2_modem_select holds SERIAL_RESET low to reset the modem */
#define RESET_MICROSECONDS 400000
/* the handle g2_modem_open returns, which mdmapi.c names DEFAULT_PORT */
#define G2_MODEM_PORT 1

static struct dte_api *modem_api;
static MDS_DCB modem_state;
static struct _DTEDEV_OPEN modem_setup;
static Sint8 modem_open;

#pragma section ATA
static struct ddal_api g2_callbacks = {GetTxBuffer, SetRxBuffer,
                                       SetModemStatusRegister,
                                       GetModemControlRegister, SetEventMask};
#pragma section

static void g2_apply_line(MDS_DCB state);
static void g2_read_line(MDS_DCB *state);

#pragma inline(g2_encode_line)
static void g2_encode_line(const MDS_DCB *state)
{
    g2_line.mcr = REPLACE_BITS(g2_line.mcr, MCR_DTR, state->Dtr);
    g2_line.mcr = REPLACE_BITS(g2_line.mcr, MCR_RTS, state->Rts);
    g2_line.lcr = REPLACE_BITS(g2_line.lcr, LCR_CHARLEN, state->CharLen);
    g2_line.lcr = REPLACE_BITS(g2_line.lcr, LCR_STOPBIT, state->StopBit);
    g2_line.lcr = REPLACE_BITS(g2_line.lcr, LCR_PARITY, state->Parity);
    g2_line.lcr = REPLACE_BITS(g2_line.lcr, LCR_BREAK, state->SetBreak);
    g2_line.baud = state->LineSpeed;
}

#pragma inline(clear_pending_event)
static void clear_pending_event(void)
{
    g2_event.event.all = 0;
    /* @hack redundant: the return that ends the body */
    /* NOLINTNEXTLINE(readability-redundant-control-flow) */
    return;
}

void g2_modem_drop_dtr(MDM handle)
{
    g2_read_line(&modem_state);
    modem_state.Dtr = MDD_MCR_DTR_OFF;
    g2_apply_line(modem_state);
}

void g2_modem_enable_dtr(MDM handle)
{
    g2_read_line(&modem_state);
    modem_state.Dtr = MDD_MCR_DTR_ON;
    g2_apply_line(modem_state);
}

int g2_modem_rx_available(MDM handle)
{
    return g2_rx_capacity - g2_rx_space;
}

int g2_modem_rx_free(MDM handle)
{
    return g2_rx_space;
}

int g2_modem_tx_available(MDM handle)
{
    return g2_tx_capacity - g2_tx_space;
}

int g2_modem_tx_free(MDM handle)
{
    return g2_tx_space;
}

void g2_modem_init(char *tx, char *rx, int tx_size, int rx_size)
{
    Sint16 saved_mask;
    saved_mask = get_imask();
    set_imask(IMASK_ALL);
    g2_tx_buffer = (Uint8 *)tx;
    g2_tx_capacity = tx_size;
    g2_rx_buffer = (Uint8 *)rx;
    g2_rx_capacity = rx_size;
    modem_open = 0;
    set_imask(saved_mask);
}

MDM g2_modem_open(MD_DEVICE device)
{
    Sint16 saved_mask;
    MDM result;
    if (modem_open != 1) {
        saved_mask = get_imask();
        set_imask(IMASK_ALL);
        g2_rx_reader = g2_rx_writer = g2_rx_buffer;
        g2_rx_space = g2_rx_capacity;
        g2_tx_reader = g2_tx_writer = g2_tx_buffer;
        g2_tx_space = g2_tx_capacity;
        g2_event.event.all = 0;
        g2_event_mask.event.all = 0;
        g2_line.mcr = 0;
        g2_line.lcr = 0;
        g2_line.baud = 0;
        g2_status.msr = 0;
        g2_status.rx_break = 0;
        modem_setup.host = 0;
        modem_setup.unk_4[0] = 0;
        modem_setup.unk_4[1] = 0;
        modem_setup.unk_4[2] = 0;
        modem_setup.unk_4[3] = 0;
        modem_state.Dtr = MDD_MCR_DTR_ON;
        modem_state.Rts = MDD_MCR_RTS_ON;
        modem_state.CharLen = MDD_LCR_CHARLEN_8;
        modem_state.Parity = MDD_LCR_PARITY_NONE;
        modem_state.LineSpeed = MDD_LINESPEED_115200;
        modem_state.Cts = 0;
        modem_state.Dsr = 0;
        modem_state.Ri = 0;
        modem_state.Dcd = 0;
        modem_state.Brk = 0;
        modem_setup.ddal = &g2_callbacks;
        modem_setup.host = (void *)device;
        modem_setup.opt = mdmOptInit();
        modem_api = dteGetApi();
        if (modem_api->open(&modem_setup)) {
            modem_open = 0;
            result = -1;
        } else {
            g2_event.event.bit.ready = 1;
            modem_api->set_event(&g2_event);
            g2_event.event.all = 0;
            modem_open = 1;
            g2_apply_line(modem_state);
            result = G2_MODEM_PORT;
        }
        set_imask(saved_mask);
    } else {
        result = -1;
    }
    return result;
}

int g2_modem_read(MDM handle, char *buffer, int size)
{
    Sint16 saved_mask;
    int remaining = size;
    if (modem_open) {
        saved_mask = get_imask();
        set_imask(IMASK_ALL);
        while (g2_rx_capacity - g2_rx_space > 0 && remaining > 0) {
            *buffer++ = *g2_rx_reader++;
            g2_rx_space++;
            remaining--;
            if (g2_rx_reader >= g2_rx_buffer + g2_rx_capacity) {
                g2_rx_reader = g2_rx_buffer;
            }
        }
        if (g2_event_mask.event.bit.rx == 1) {
            g2_event.event.bit.rx = 1;
            modem_api->set_event(&g2_event);
            clear_pending_event();
        }
        set_imask(saved_mask);
    } else {
        return 0;
    }
    return size - remaining;
}

int g2_modem_write(MDM handle, char *buffer, int size)
{
    Sint16 saved_mask;
    int remaining = size;
    if (modem_open) {
        saved_mask = get_imask();
        set_imask(IMASK_ALL);
        while (g2_tx_space > 0 && remaining > 0) {
            *g2_tx_writer++ = *buffer++;
            g2_tx_space--;
            remaining--;
            if (g2_tx_writer >= g2_tx_buffer + g2_tx_capacity) {
                g2_tx_writer = g2_tx_buffer;
            }
        }
        if (g2_event_mask.event.bit.tx == 1) {
            g2_event.event.bit.tx = 1;
            modem_api->set_event(&g2_event);
            clear_pending_event();
        }
        set_imask(saved_mask);
    } else {
        return 0;
    }
    return size - remaining;
}

static void g2_apply_line(MDS_DCB state)
{
    Sint16 saved_mask;
    if (modem_open) {
        saved_mask = get_imask();
        set_imask(IMASK_ALL);
        g2_encode_line(&state);
        if (g2_event_mask.event.bit.line == 1) {
            g2_event.event.bit.line = 1;
            modem_api->set_event(&g2_event);
            clear_pending_event();
            set_imask(saved_mask);
        } else {
            /* the driver hangs here with interrupts masked where the modem's
             * mask lacks the line event. the modem's open sets every bit of the
             * mask, so an open port never comes here */
#ifdef NONMATCHING
            /* this side restores the interrupt mask and returns, where the
             * matching side hangs */
            set_imask(saved_mask);
#else
            for (;;) {
            }
#endif
        }
    }
}

void g2_modem_set_default(MDM handle, MDS_DCB state)
{
    Sint16 saved_mask;
    if (modem_open) {
        saved_mask = get_imask();
        set_imask(IMASK_ALL);
        g2_encode_line(&state);
        if (g2_event_mask.event.bit.line == 1) {
            g2_event.event.bit.line = 1;
            modem_api->set_event(&g2_event);
            clear_pending_event();
        }
        set_imask(saved_mask);
    }
}

#pragma inline(g2_modem_get_state)
void g2_modem_get_state(MDM handle, MDS_DCB *state)
{
    state->Dtr = g2_line.mcr & MCR_DTR;
    state->Rts = g2_line.mcr & MCR_RTS;
    state->CharLen = g2_line.lcr & LCR_CHARLEN;
    state->Parity = g2_line.lcr & LCR_PARITY;
    state->StopBit = g2_line.lcr & LCR_STOPBIT;
    state->SetBreak = g2_line.lcr & LCR_BREAK;
    state->LineSpeed = g2_line.baud;
    state->Dcd = (g2_status.msr & MSR_DCD) >> MSR_DCD_SHIFT;
    state->Ri = (g2_status.msr & MSR_RI) >> MSR_RI_SHIFT;
    state->Dsr = (g2_status.msr & MSR_DSR) >> MSR_DSR_SHIFT;
    state->Cts = (g2_status.msr & MSR_CTS) >> MSR_CTS_SHIFT;
    state->Brk = g2_status.rx_break & 1;
}

static void g2_read_line(MDS_DCB *state)
{
    g2_modem_get_state(0, state);
}

void g2_modem_purge_tx(MDM handle)
{
    Sint16 saved_mask;
    if (modem_open) {
        saved_mask = get_imask();
        set_imask(IMASK_ALL);
        g2_tx_reader = g2_tx_writer = g2_tx_buffer;
        g2_tx_space = g2_tx_capacity;
        if (g2_event_mask.event.bit.flush_out == 1) {
            g2_event.event.bit.flush_out = 1;
            modem_api->set_event(&g2_event);
            clear_pending_event();
            set_imask(saved_mask);
        } else {
            /* the driver hangs here as g2_apply_line does, where the modem's
             * mask lacks flush_out */
#ifdef NONMATCHING
            /* this side restores the interrupt mask and returns, where the
             * matching side hangs */
            set_imask(saved_mask);
#else
            for (;;) {
            }
#endif
        }
    }
}

void g2_modem_purge_rx(MDM handle)
{
    Sint16 saved_mask;
    if (modem_open) {
        saved_mask = get_imask();
        set_imask(IMASK_ALL);
        g2_rx_reader = g2_rx_writer = g2_rx_buffer;
        g2_rx_space = g2_rx_capacity;
        if (g2_event_mask.event.bit.flush_in == 1) {
            g2_event.event.bit.flush_in = 1;
            modem_api->set_event(&g2_event);
            clear_pending_event();
        }
        set_imask(saved_mask);
    }
}

void g2_modem_close(MDM handle)
{
    Sint16 saved_mask;
    struct _DTEDEV_CLOSE request;
    if (modem_open) {
        saved_mask = get_imask();
        set_imask(IMASK_ALL);
        request.flags = 1;
        modem_api->close(&request);
        mdmOptFinish();
        modem_open = 0;
        set_imask(saved_mask);
    }
}

void g2_modem_finish(void)
{
}

void g2_modem_select(MD_DEVICE device)
{
    Uint32 start;
    SERIAL_RESET = 0;
    /* @hack spelling: start set in the first clause of a for, where a
     * statement and a while would do */
    for (start = syTmrGetCount();
         (Sint32)syTmrCountToMicro(syTmrDiffCount(start, syTmrGetCount())) <
         RESET_MICROSECONDS;) {
    }
    SERIAL_RESET = 1;
}
