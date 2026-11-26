/* the frames the modem controller and the data pump pass each other. a ring of
 * DCE_FRAMES frames runs each way */
#ifndef MODEM_DCE_LINE_H
#define MODEM_DCE_LINE_H
#include <sg_xpt.h>

#define DCE_FRAMES 8
#define DCE_FRAME_BYTES 280
#define DCE_FRAME_BREAKS 4
#define DCE_FRAME_BAD_CRC 0x01
#define DCE_FRAME_ABORT 0x02
#define DCE_FRAME_TOO_LONG 0x08

struct dce_frame {
    Uint8 status;
    Uint16 length;
    Uint8 data[DCE_FRAME_BYTES];
    Uint16 breaks[DCE_FRAME_BREAKS];
};
struct dce_pump_buffer {
    union {
        Uint32 word;
        Uint8 byte[4];
    } data;
    Uint16 count;
};
extern Uint8 dce_rx_read;
extern Uint8 dce_tx_read;
extern Uint8 dce_rx_write;
extern Uint8 dce_tx_write;
extern struct dce_pump_buffer DpTxBuffer;
extern struct dce_pump_buffer DpRxBuffer;
extern Uint8 drx_break_type;
extern Uint8 crc_length;
/* the length, in bytes, at which V.14's receiver closes a receive frame.
 * v14.c sets it from the line's speed and format */
extern Uint16 MRBLR_v14;
extern struct dce_frame dce_rx_buffer[DCE_FRAMES];
extern struct dce_frame dce_tx_buffer[DCE_FRAMES];

void open_tx_buffer(void);
void close_tx_buffer(void);
void open_rx_buffer(void);
void close_rx_buffer(void);
Uint16 dce_transmit_count(void);
Uint8 rx_buffer_low(void);

#endif
