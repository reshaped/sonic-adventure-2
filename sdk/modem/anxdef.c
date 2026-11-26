/* the V.42 Annex A state that anxdef.h declares */
#include <sg_xpt.h>
#include "anxdef.h"

/* the MNP link's phases, timers and CRCs. then the LR, LA, LT, LN and LD frames
 * it has sent, has outstanding and has received */
Uint8 outstanding_la, outstanding_ln, retransmission_of_lt, nack_received;
Uint8 data_phase, mnp_phase, protocol_establishment_phase, disconnect_phase;
Uint8 t401_not_running, t402_not_running, t403_not_running, lr_count;
Uint16 transmit_crc, rec_calculated_crc, receive_fcs;
Uint8 frame_sent, lr_receive, la_receive, ld_receive, lt_receive, ln_receive;
Uint8 lna_receive, incorrect_frame, first_lt_sent, lt_unacknowledged_tx;

/* the parameters the link negotiates, the host's offer in the host_ names,
 * then its sequence numbers and windows */
Uint8 facilities, host_k, host_mode, host_facilities;
Uint32 host_t401, host_t402, host_t403, host_t404;
Uint16 host_n401, host_n400;
Uint8 last_vs_to_retransmit, class_5;
Uint8 vsa, nsa, nsa_rx, nra, nra_rx, vra;
Uint8 nr, last_nr_rx, ns, k, sk, rk, nk, nk_rx, last_nk_rx;
Uint8 bit_oriented_mode;
Uint16 length, frame_size;
Uint8 type;

Uint32 t401, t402, t403, t404, success_timer;
Uint16 n401, info_field;
Uint8 max_info_field, fixed_field_lt_la;
Uint16 rx_n401;
Uint8 lt_retransmission_count, ln_retransmission_count;
Uint8 reason_code, user_code, lt_good_transmission_count;

/* the received bytes and their asynchronous framing. async_in_pntr and
 * async_in_cntr give the bytes dce_get_rx_data last returned, and the _bak
 * pair steps through them */
Uint16 async_in_cntr_bak;
Uint8 *async_in_pntr_bak;
Uint16 async_in_cntr;
Uint8 *async_in_pntr, *octet_buff_pntr;
Uint8 v42a_established, dle_found, async_frame_status;
Uint8 threshold_to_increase_lt_size, disconnect_reason, flow_on;
Uint8 mode_nego, bit_oriented_mode_nego, ignore_next_nack;
Uint16 dle_count;
Uint8 param_9_to_send, send_param_14;
Uint16 bps;
Uint8 protocol_message_received, mnp_frame_receive, max_retransmit_lr;
