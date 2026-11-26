/* the state the modem's terminal interface, its line interface and its data
 * pump share */
#include "targdef.h"
#include <sg_xpt.h>

Uint8 next_dte_type;
Uint8 speed_bak;
Uint8 format_bak;
Uint8 flwctl_bak;
Uint8 S2_bak;
Uint8 S12_bak;
Uint8 XON_bak;
Uint8 XOFF_bak;
Uint8 rx_break_type_bak;
Uint8 dte_type;
Uint8 dte_speed;
Uint8 dte_format;
Uint8 dte_flwctl;
Uint8 dte_S2;
Uint8 dte_S12;
Uint8 dte_xon;
Uint8 dte_xoff;
Uint8 dte_rx_break_type;
Uint8 remote_dialog_rx_ctr;
Uint8 remote_dialog_tx_ctr;
Uint32 dte_timer;
Uint32 dte_delay_timer;
Uint16 dte_char_length;
Uint16 DTE_BK_TIME;
Uint16 DTE_BRK_LEN;

Uint16 MESS_CNT;
Uint8 MESS_BUFFER[MESS_SIZE];
Uint8 *MESS_TOPP;
Uint8 *MESS_READ;
Uint8 *MESS_WRITE;
Uint32 ESC_TOUT;
volatile Uint8 *esc_code_ptr;

Uint8 dce_type;
Uint8 dce_speed;
Uint8 dce_format;
Uint8 dce_flow;
Uint8 dce_xon;
Uint8 dce_xoff;
Uint8 dce_rx_break_type;
Uint16 DCE_BK_TIME;
Uint16 DCE_BRK_LEN;
struct dce_sync_marker dceRxSyncMarker;

Uint32 TxComplete;
Uint32 tx_inword;
Uint32 DATA_TO_DP;
Uint8 tx_inbit_cnt;
Uint8 tx_outbit_cnt;
Uint8 rx_outbit_cnt;
Uint16 MRBLRv14;
Uint16 tx_nbyte_total;
Uint8 dflwctl;
Uint8 tx_dce_nstop_flag;
Uint8 tx_dce_nstop;
Uint8 tx_phase;
Uint16 tx_dce_buffer_wcnt;
Uint8 *tx_dce_buffer_wptr;
Uint8 tx_dce_flow;
Uint8 tx_buffer_flag;
Uint8 tx_break_flag;
Uint32 tx_break_timer;

Uint8 rx_phase;
Uint8 DATA_FROM_DP;
Uint16 rx_outword;
Uint16 rx_dce_buffer_wcnt;
Uint8 *rx_dce_buffer_wptr;
Uint8 rx_n_stop_bit;
Uint8 rx_dce_flow;
Uint8 rx_dce_flow_flag;
Uint8 rx_break_flag;
Uint32 rx_break_timer;
Uint16 n_char_tx_since_last_removing;
Uint8 remove_stop_bit_flag;
Uint8 number_of_char_between_stop_deletion;
Uint8 dce_n_bit_per_char;
Uint8 dce_rx_buffer_available;

Uint8 SCC_FLAGS;
Uint8 BREAK_FLAGS;
Uint8 vxd_present;
Uint8 dte_tx_cnt;
Uint8 dte_rx_cnt;
Uint8 dte_rx_read;
Uint8 dte_tx_read;
Uint8 dte_tx_write;
Uint8 dte_rx_write;
Uint8 MD_INT_MASK_MCR;
volatile Uint8 *status_esc_code_ptr;
Uint8 FORGET_BREAKS;
Uint8 init_mem;
Uint8 CC_FLAGS;
Uint8 close_port;
