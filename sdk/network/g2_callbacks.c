#include <machine.h>
#include <sg_xpt.h>
#include <access/dte_if.h>
#include "g2_modem.h"

struct dte_line g2_line;
struct dteModemStatusRegister g2_status;
struct _DTEDEV_SETEVENT g2_event, g2_event_mask;
Uint32 g2_tx_capacity;
Uint8 *g2_tx_buffer, *g2_tx_reader, *g2_tx_writer;
int g2_tx_space;
Uint32 g2_rx_capacity;
Uint8 *g2_rx_buffer, *g2_rx_reader, *g2_rx_writer;
int g2_rx_space;

struct ddal_result *GetTxBuffer(struct ddalDataTransfer *transfer)
{
    static struct ddal_result result;
    Uint16 saved_mask;
    Uint8 *cursor;
    Uint8 *destination;
    Uint32 available;
    int count;

    saved_mask = get_imask();
    set_imask(IMASK_ALL);
    destination = transfer->data;
    cursor = g2_tx_reader;
    available = g2_tx_capacity - g2_tx_space;
    if (available > transfer->length) {
        count = transfer->length;
        result.length = count;
        result.remaining = g2_tx_capacity - g2_tx_space - count;
    } else {
        count = available;
        result.length = count;
        result.remaining = 0;
    }
    g2_tx_space += count;
    while (count > 0) {
        *destination++ = *cursor++;
        if (cursor == g2_tx_buffer + g2_tx_capacity) {
            cursor = g2_tx_buffer;
        }
        count--;
    }
    g2_tx_reader = cursor;
    set_imask(saved_mask);
    return &result;
}

struct ddal_result *SetRxBuffer(struct ddalDataTransfer *transfer)
{
    static struct ddal_result result;
    Uint16 saved_mask;
    Uint8 *cursor;
    Uint8 *source;
    int count;

    saved_mask = get_imask();
    set_imask(IMASK_ALL);
    cursor = g2_rx_writer;
    source = transfer->data;
    count = g2_rx_space;
    if ((Uint32)count > transfer->length) {
        count = transfer->length;
        result.length = count;
        result.remaining = 0;
    } else {
        result.length = count;
        result.remaining = transfer->length - count;
    }
    g2_rx_space -= count;
    while (count > 0) {
        *cursor++ = *source++;
        if (cursor == g2_rx_buffer + g2_rx_capacity) {
            cursor = g2_rx_buffer;
        }
        count--;
    }
    g2_rx_writer = cursor;
    set_imask(saved_mask);
    return &result;
}

void SetModemStatusRegister(struct dteModemStatusRegister *status)
{
    g2_status = *status;
}

struct dte_line *GetModemControlRegister(void)
{
    return &g2_line;
}

void SetEventMask(struct _DTEDEV_SETEVENT *mask)
{
    g2_event_mask.event.all = mask->event.all;
}
