/* targdef: the state of the modem's end of the port, as far as the sources
 * use it */
#ifndef MODEM_TARGDEF_H
#define MODEM_TARGDEF_H
#include <sg_xpt.h>

/* the type vxd_dte_set_interface has deferred, or DTE_NONE while none
 * waits. speed_bak to rx_break_type_bak hold the deferred call's other
 * arguments */
extern Uint8 next_dte_type;
extern Uint8 speed_bak;
extern Uint8 format_bak;
extern Uint8 flwctl_bak;
extern Uint8 S2_bak;
extern Uint8 S12_bak;
extern Uint8 XON_bak;
extern Uint8 XOFF_bak;
extern Uint8 rx_break_type_bak;
/* the port's type, a DTE_ value. dte_speed to dte_rx_break_type hold its
 * settings, as vxd_dte_set_interface keeps them */
extern Uint8 dte_type;
extern Uint8 dte_speed;
extern Uint8 dte_format;
extern Uint8 dte_flwctl;
extern Uint8 dte_S2;
extern Uint8 dte_S12;
extern Uint8 dte_xon;
extern Uint8 dte_xoff;
extern Uint8 dte_rx_break_type;
/* remote_dialog_rx_ctr and remote_dialog_tx_ctr count the bytes waiting in
 * the port's command_rx and command_tx */
extern Uint8 remote_dialog_rx_ctr;
extern Uint8 remote_dialog_tx_ctr;
/* the time vxd_dte_tx_buff_empty waits until, as setup_timer sets it */
extern Uint32 dte_timer;
/* the time until which vxd_message_buff_empty returns 0 */
extern Uint32 dte_delay_timer;
/* the bits of a character on the line */
extern Uint16 dte_char_length;
/* the 10 ms ticks left of the break to the host, which break_check counts
 * down */
extern Uint16 DTE_BK_TIME;
extern Uint16 DTE_BRK_LEN;
/* MESS_BUFFER is a ring of MESS_SIZE bytes. the modem's messages to the host
 * wait there for Buffer_memory's prio to empty */
#define MESS_SIZE 2000
/* the count of bytes waiting */
extern Uint16 MESS_CNT;
extern Uint8 MESS_BUFFER[MESS_SIZE];
/* the ring's last byte */
extern Uint8 *MESS_TOPP;
/* the next byte to give the host */
extern Uint8 *MESS_READ;
/* the next free byte */
extern Uint8 *MESS_WRITE;
/* the TIM_10MS reading that the escape's guard time runs from */
extern Uint32 ESC_TOUT;
extern volatile Uint8 *esc_code_ptr;

/* DCE configuration and the marker recording how many complete frames
 * precede the current position in the receive stream. */
extern Uint8 dce_type;
extern Uint8 dce_speed;
extern Uint8 dce_format;
extern Uint8 dce_flow;
extern Uint8 dce_xon;
extern Uint8 dce_xoff;
extern Uint8 dce_rx_break_type;
extern Uint16 DCE_BK_TIME;
extern Uint16 DCE_BRK_LEN;
struct dce_sync_marker {
    Sint8 frames;
    Uint16 length;
};
extern struct dce_sync_marker dceRxSyncMarker;

/* Bit and byte counters for the serial data pump. */
extern Uint32 TxComplete;
extern Uint32 tx_inword;
extern Uint32 DATA_TO_DP;
extern Uint8 tx_inbit_cnt;
extern Uint8 tx_outbit_cnt;
extern Uint8 rx_outbit_cnt;
extern Uint16 MRBLRv14;
extern Uint16 tx_nbyte_total;
extern Uint8 dflwctl;
extern Uint8 tx_dce_nstop_flag;
extern Uint8 tx_dce_nstop;

extern Uint8 tx_phase;
extern Uint16 tx_dce_buffer_wcnt;
extern Uint8 *tx_dce_buffer_wptr;
extern Uint8 tx_dce_flow;
extern Uint8 tx_buffer_flag;
/* open_tx_buffer sets tx_buffer_flag to TX_FRAME_OPEN for a frame whose bytes
 * go out next, to TX_FRAME_BREAK for one whose break goes first, and to
 * TX_NO_FRAME for none */
#define TX_NO_FRAME 0
#define TX_FRAME_OPEN 1
#define TX_FRAME_BREAK 2
extern Uint8 tx_break_flag;
/* tx_break_flag is TX_BREAK_DUE from a break's request until the break ends,
 * TX_BREAK_FRAMED for a break a frame queued, and TX_NO_BREAK with none */
#define TX_BREAK_DUE 0
#define TX_BREAK_FRAMED 1
#define TX_NO_BREAK 255
extern Uint32 tx_break_timer;

extern Uint8 rx_phase;
extern Uint8 DATA_FROM_DP;
extern Uint16 rx_outword;
extern Uint16 rx_dce_buffer_wcnt;
extern Uint8 *rx_dce_buffer_wptr;
extern Uint8 rx_n_stop_bit;
extern Uint8 rx_dce_flow;
extern Uint8 rx_dce_flow_flag;
extern Uint8 rx_break_flag;
extern Uint32 rx_break_timer;
extern Uint16 n_char_tx_since_last_removing;
extern Uint8 remove_stop_bit_flag;
extern Uint8 number_of_char_between_stop_deletion;
extern Uint8 dce_n_bit_per_char;
extern Uint8 dce_rx_buffer_available;

/* the port's flags. while SCC_ESC_OFF is set, the modem takes no escape
 * sequence */
extern Uint8 SCC_FLAGS;
#define SCC_ESC_OFF 0x08
/* the breaks' flags. BREAK_TO_HOST is set while a break to the host runs */
extern Uint8 BREAK_FLAGS;
#define BREAK_TO_HOST 0x02
/* BREAK_FROM_HOST is set while a break from the host runs. BREAK_END marks
 * that the host has ended it, until the next break from the host starts */
#define BREAK_FROM_HOST 0x01
#define BREAK_END 0x10
/* BREAK_TO_LINE and BREAK_FROM_LINE are the line's, which DCE_BK_TIME and
 * DCE_BRK_LEN time */
#define BREAK_FROM_LINE 0x04
#define BREAK_TO_LINE 0x08
extern Uint8 vxd_present;
/* dte_tx_cnt counts the transmit buffers the modem has filled and the
 * host's side not yet taken. dte_rx_cnt counts the receive buffers the
 * host's side has filled and the modem not yet taken */
extern Uint8 dte_tx_cnt;
extern Uint8 dte_rx_cnt;
/* dte_rx_read is the receive buffer the modem takes next. dte_tx_read is the
 * transmit buffer the host's side takes next */
extern Uint8 dte_rx_read;
extern Uint8 dte_tx_read;
/* dte_tx_write is the transmit buffer the modem fills next. dte_rx_write is
 * the receive buffer the host's side fills next */
extern Uint8 dte_tx_write;
extern Uint8 dte_rx_write;
/* vxd_prog_esc_code sets MASK_ESC_ON in MD_INT_MASK_MCR as it turns the
 * watch for the escape character on. vxd_stop_esc_code clears it as it
 * turns the watch off */
extern Uint8 MD_INT_MASK_MCR;
#define MASK_ESC_ON 0x80
/* the flags of the cchar entry that holds the escape character */
extern volatile Uint8 *status_esc_code_ptr;
/* 0xff once a break has come while every receive buffer was filled. only
 * a break that vxd_end_of_dte_brk_rx would queue sets it */
extern Uint8 FORGET_BREAKS;
/* GenerateIRQ raises no event while init_mem is 0 */
extern Uint8 init_mem;
/* the flow's flags, as dte_set_flow last set them. CC_XON_XOFF marks a flow
 * whose stop and start characters are xoff and xon, and CC_NO_XON_XOFF a
 * flow without them */
extern Uint8 CC_FLAGS;
#define CC_NO_XON_XOFF 0x01
#define CC_XON_XOFF 0x02
/* while close_port is set, GenerateIRQ raises no event, and
 * vxd_dte_tx_cmd_byte queues nothing */
extern Uint8 close_port;

#endif
