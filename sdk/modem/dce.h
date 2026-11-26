/* dce: the modem controller's interface to the data pump's line side */
#ifndef MODEM_DCE_H
#define MODEM_DCE_H
#include <sg_xpt.h>

/* point *data at the next block received from the line and not yet taken.
 * *length gets its length, 0 for none. it returns 1 to 3 for a block
 * received with an error, and 0 otherwise. it takes such a block itself and
 * sets *length to 0, but on a line of DCE_RAW it leaves the block to the
 * caller */
Uint8 dce_get_rx_data(Uint8 **data, Uint16 *length);
/* mark the block dce_get_rx_data gave taken */
void dce_rx_data_done(void);
/* point *data at room for bytes to send to the line. it returns how many
 * fit there, 0 for none */
Uint16 dce_set_tx_data(Uint8 **data);
/* send the first length bytes of the room dce_set_tx_data gave. a length
 * of 0 sends none, and unused is never read */
void dce_tx_data_done(Uint16 length, char unused);

/* serve the data pump's interrupt for the line's side */
void dce_interrupt(void);

/* types of line dce_set_interface takes: asynchronous characters, and HDLC
 * frames with a 16-bit or a 32-bit FCS */
#define DCE_ASYNC 0
#define DCE_HDLC16 8
#define DCE_HDLC32 9
/* re_configure_dce's types for a raw link: DCE_DIRECT_SYNC on a synchronous
 * line, and on an asynchronous one DCE_DIRECT_2, or DCE_DIRECT_1 where OIS(6)
 * lacks bit 7. v14 passes each character of DCE_DIRECT_1 and DCE_DIRECT_2
 * through dte_vxd's direct-mode hooks */
#define DCE_DIRECT_1 1
#define DCE_DIRECT_2 2
#define DCE_DIRECT_SYNC 3
/* dce_set_interface leaves the pump's data mode off for DCE_DIRECT_SYNC and
 * for DCE_HELD. TCI_sw_reset sets DCE_HELD beside the port's DTE_HELD, and
 * go_off_line sets it as the modem hangs up */
#define DCE_HELD 7
/* types no caller in the library passes. DCE_LOOPBACK sends back what it
 * receives */
#define DCE_SYNC 12
#define DCE_LOOPBACK 18
#define DCE_RAW 26
#define DCE_RAW_ASYNC 27
/* set the line's type, speed, format and flow control. dce_clr_rx_buff and
 * dce_clr_tx_buff empty one side's frames when the controller resets it */
void dce_set_interface(Uint8 type, Uint8 speed, Uint8 format, Uint8 flow,
                       Uint8 xon, Uint8 xoff, Uint8 rx_break_type);
void dce_clr_rx_buff(void);
void dce_clr_tx_buff(void);
Uint8 dce_tx_brk(Uint16 length, Uint8 queued);

Uint8 dce_tx_buff_empty(void);
/* discard the information frames queued to send, those over 64 bytes.
 * dce_get_amount_of_frames counts the frames still to send */
void dce_del_info_frms(void);
Uint8 dce_get_amount_of_frames(void);

void dce_ignore_rx_async_data(char ignore);
Uint16 dce_get_hdlc_rx_error_cnt(void);

/* mark the synchronous receiver's position. dce_get_marked_rx_data then
 * gives each whole frame before the mark, and the partial frame at it last */
void dce_mark_current_rx_state(void);
void dce_get_marked_rx_data(Uint8 **data, Uint16 *length);

/* the line driver's hooks, each empty in this library */
void send_next_dce_break(void);
void dce_sync_tx_direct_mode(void);
void dce_sync_rx_direct_mode(void);
void end_of_dce_brk_rx(Uint16 length);
void dce_Open(void);
void dce_Close(void);
void dceUnderrun(void);

#endif
