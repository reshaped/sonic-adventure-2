/* the G2 modem's interface to its host: the calls each gives the other, and the
 * records they pass */
#ifndef ACCESS_DTE_IF_H
#define ACCESS_DTE_IF_H
#include <sg_xpt.h>

/* a transfer between the modem and the host: length bytes at data */
struct ddalDataTransfer {
    Uint8 *data;
    Uint32 length;
};

/* what a get_tx or a set_rx moved. length counts the bytes it moved. remaining
 * counts the output a get_tx left queued, or the input a set_rx turned away */
struct ddal_result {
    Uint32 length;
    Uint32 remaining;
};

/* the modem's state, as the host is told it */
struct dteModemStatusRegister {
    Uint8 msr; /* the port's modem status register */
    Uint8 rx_break; /* 1 where a break came for the host */
    Uint8 unk_2;
    Uint8 idle; /* 1 where the port has gone idle */
};

/* the host's settings of the port: its modem control and line control
 * registers, and its baud */
struct dte_line {
    Uint8 mcr;
    Uint8 lcr;
    Uint16 baud;
};

/* the events the host passes to set_event, a bit each. line says the host's
 * settings changed. flush_out drops the bytes the modem holds from the host.
 * flush_in drops those it holds for the host. ready starts the modem's work,
 * which runs until the next call of dte_api's open or close. the other bits do
 * nothing */
struct _DTEDEV_SETEVENT {
    union {
        Uint32 all;
        struct {
#ifdef NONMATCHING
            /* GCC allocates bit-fields from bit 0 up, where SHC allocates them
             * from bit 31 down. this order puts each field on SHC's bit */
            Uint32 : 24;
            Uint32 ready : 1;
            Uint32 flush_in : 1;
            Uint32 flush_out : 1;
            Uint32 unk_bit27 : 1;
            Uint32 unk_bit28 : 1;
            Uint32 line : 1;
            Uint32 rx : 1;
            Uint32 tx : 1;
#else
            Uint32 tx : 1;
            Uint32 rx : 1;
            Uint32 line : 1;
            Uint32 unk_bit28 : 1;
            Uint32 unk_bit27 : 1;
            Uint32 flush_out : 1;
            Uint32 flush_in : 1;
            Uint32 ready : 1;
#endif
        } bit;
    } event;
};

/* the host's calls, which dte_api's open takes. get_tx fills the transfer with
 * the host's bytes for the line, up to its length. set_rx gives the host the
 * transfer's bytes from the line. each returns what it moved. set_status tells
 * the host the modem's state, and get_line returns the host's settings. open
 * passes set_mask an event word with every bit set */
struct ddal_api {
    struct ddal_result *(*get_tx)(struct ddalDataTransfer *transfer);
    struct ddal_result *(*set_rx)(struct ddalDataTransfer *transfer);
    void (*set_status)(struct dteModemStatusRegister *status);
    struct dte_line *(*get_line)(void);
    void (*set_mask)(struct _DTEDEV_SETEVENT *mask);
};

/* what the host gives open. host is the host's own handle, which the modem
 * keeps and never reads */
struct _DTEDEV_OPEN {
    void *host;
    Uint32 unk_4[4];
    struct ddal_api *ddal;
    struct SegaOptionalInterface *opt;
};

/* what the host gives close, which reads none of it */
struct _DTEDEV_CLOSE {
    Uint32 flags;
};

/* the calls dteGetApi gives the host. set_event's line, flush_out and flush_in
 * bits need an open that succeeded, and no close since */
struct dte_api {
    Uint32 (*open)(struct _DTEDEV_OPEN *open);
    Bool (*close)(struct _DTEDEV_CLOSE *close);
    void (*set_event)(struct _DTEDEV_SETEVENT *event);
};

struct dte_api *dteGetApi(void);

#endif
