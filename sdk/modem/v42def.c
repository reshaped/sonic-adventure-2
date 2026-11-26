/* the LAPM state that v42def.h declares */
#include <sg_xpt.h>
#include "v42def.h"

Uint8 TM_BUFFER_ARRAY[LAPM_RING_BYTES];
Uint8 *TM_BUFFER, *TM_TOPP, *TM_READ, *TM_WRITE, *tm_shadow_ptr;
Uint16 TM_CNT, tm_lnt, tm_cnt_shadow;
Uint8 RR_BUFFER[LAPM_RING_BYTES];
Uint8 *RR_TOPP, *RR_READ, *RR_WRITE, *rr_shadow_ptr;
Uint16 RR_CNT, rr_cnt_shadow;

Uint8 srej_buffer[LAPM_REJECT_BYTES];
Uint8 *srej_buff_topp, *srej_buff_read, *srej_buff_write;
Uint16 srej_buff_cntr, srej_frame_length;
Uint8 vs_srej, srej_condition, srej_to_send, srej_on;
Uint8 *rm_read_temp, *tm_read_temp;
Uint16 tm_cnt_temp;
Uint8 *pwork0;
union lapm_receive_scratch pwork1;
Uint16 retrans_array[2 * LAPM_SEQUENCE_NUMBERS];
Uint16 *tm_frame_address, *tm_frame_length, rm_frame_length;
Uint8 ch, ns_rx, nr_rx, va, vs, vr, previous_va;
Uint8 nrb_tx, vrb, vsb, nsb_tx, tx_brk, fn;

Uint32 timer_t401_max, timer_t401_offset, timer_t403_offset, timer_test_offset;
Uint8 timer_t401_run, timer_t403_run, n400, retransmission_count;
Uint8 tx_frame_number, tx_code_number, pf_rx, cr_rx, pf_tx, cr_tx;
Uint8 tx_address, break_handling_option_s, break_handling_option_d;
Uint8 break_sequence_number_rx, break_length, tx_break_length;
Uint8 break_option, brk_sent, destructive, break_or_sabme_sent;
Uint8 same_nr_as_previous, timer_recovery_condition, reject_condition;
Uint8 remote_busy, own_busy, break_type, ans_org, tr_full;
Uint8 recover_with_rej, xid_sent, xid_rec, sabme_sent;
Uint8 enter_disconnect, no_connect_possible, disc_sent, test_sent,
    remote_is_mma;

Uint8 group_length, group_identifier, parameter_identifier, parameter_length;
Uint16 n401_tx, n401_tx_init, n401_rx, n401_rx_init;
Uint8 win_k_tx, win_k_rx, win_k_tx_init, win_k_rx_init, win_k_tx_used;
Uint8 hdlc[4];
Uint32 connect_timer;
Uint8 seq, adj_cnt, num_adj_obsv, countx, received_work;
Uint8 *sendptr;
Uint16 start_send;
Uint8 comp, comp_val1, comp_val2, comp_val2a, valid_tx;
Uint8 lapm_established, bis_accept, get_ready;
Uint16 p0, p0_init, p1, p1_init, p2, p2_init;
Uint8 crc_init, crc, s_rej_init, s_rej, loopb_init, loopb;
Uint16 tx_max_packet_size, test_packet_size;

Uint8 *dce_in_pntr;
Uint16 dce_in_cntr;
Uint8 overflow_buffer[LAPM_OVERFLOW_BYTES], overflow_read, overflow_write,
    overflow_cnt;
Uint8 encryption, key_string[KEY_BYTES], ENCRYPT_KEY[KEY_BYTES];
Uint16 RxBreakLength[LAPM_BREAKS];
Uint8 *RxBreakPointer[LAPM_BREAKS];
Uint16 TxBreakLength[LAPM_BREAKS];
