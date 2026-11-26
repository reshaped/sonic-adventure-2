/* the state of LAPM, V.42's error control. it holds LAPM's rings and frame
 * queues, its timers and the parameters it negotiates */
#ifndef MODEM_V42DEF_H
#define MODEM_V42DEF_H
#include <sg_xpt.h>

#define LAPM_RING_BYTES 4096
#define LAPM_REJECT_BYTES 1036
#define LAPM_BREAKS 4
#define LAPM_SEQUENCE_NUMBERS 128
#define LAPM_OVERFLOW_BYTES 28

/* scratch. the receiver steps data through the receive ring as it copies a
 * frame in. a poll of the terminal for data writes data and length, which no
 * code reads after */
union lapm_receive_scratch {
    Uint8 *data;
    Uint16 length;
};

/* the transmit ring, TM, holds the data to send, which stays for
 * retransmission until the peer acknowledges it. the receive ring, RR, holds
 * the data received for the terminal */
extern Uint8 TM_BUFFER_ARRAY[LAPM_RING_BYTES];
extern Uint8 *TM_BUFFER, *TM_TOPP, *TM_READ, *TM_WRITE, *tm_shadow_ptr;
extern Uint16 TM_CNT, tm_lnt, tm_cnt_shadow;
extern Uint8 RR_BUFFER[LAPM_RING_BYTES];
extern Uint8 *RR_TOPP, *RR_READ, *RR_WRITE, *rr_shadow_ptr;
extern Uint16 RR_CNT, rr_cnt_shadow;

extern Uint8 srej_buffer[LAPM_REJECT_BYTES];
extern Uint8 *srej_buff_topp, *srej_buff_read, *srej_buff_write;
extern Uint16 srej_buff_cntr, srej_frame_length;
extern Uint8 vs_srej, srej_condition, srej_to_send, srej_on;
extern Uint8 *rm_read_temp, *tm_read_temp;
extern Uint16 tm_cnt_temp;
extern Uint8 *pwork0;
extern union lapm_receive_scratch pwork1;

/* for each sequence number, its sent frame's offset in the transmit ring
 * and then, LAPM_SEQUENCE_NUMBERS on, its length. tm_frame_address and
 * tm_frame_length point at the two halves */
extern Uint16 retrans_array[2 * LAPM_SEQUENCE_NUMBERS];
extern Uint16 *tm_frame_address, *tm_frame_length, rm_frame_length;
extern Uint8 ch, ns_rx, nr_rx, va, vs, vr, previous_va;
extern Uint8 nrb_tx, vrb, vsb, nsb_tx, tx_brk, fn;
extern Uint32 timer_t401_max, timer_t401_offset, timer_t403_offset;
extern Uint32 timer_test_offset;
extern Uint8 timer_t401_run, timer_t403_run, n400, retransmission_count;
extern Uint8 tx_frame_number, tx_code_number, pf_rx, cr_rx, pf_tx, cr_tx;
extern Uint8 tx_address, break_handling_option_s, break_handling_option_d;
extern Uint8 break_sequence_number_rx, break_length, tx_break_length;
extern Uint8 break_option, brk_sent, destructive, break_or_sabme_sent;
extern Uint8 same_nr_as_previous, timer_recovery_condition, reject_condition;
/* break_type is 1 where the last UI frame received holds a break. it is 0
 * where that frame acknowledges one */
extern Uint8 remote_busy, own_busy, break_type, ans_org, tr_full;
extern Uint8 recover_with_rej, xid_sent, xid_rec, sabme_sent;
extern Uint8 enter_disconnect, no_connect_possible, disc_sent, test_sent;
extern Uint8 remote_is_mma;

/* XID negotiation, which agrees V.42bis's P0 to P2 and each direction's frame
 * size and window. n401 holds the frame sizes, and win_k the windows. the
 * link's setup and break state follow */
extern Uint8 group_length, group_identifier, parameter_identifier,
    parameter_length;
extern Uint16 n401_tx, n401_tx_init, n401_rx, n401_rx_init;
extern Uint8 win_k_tx, win_k_rx, win_k_tx_init, win_k_rx_init, win_k_tx_used;
extern Uint8 hdlc[4];
extern Uint32 connect_timer;
extern Uint8 seq, adj_cnt, num_adj_obsv, countx, received_work;
extern Uint8 *sendptr;
extern Uint16 start_send;
extern Uint8 comp, comp_val1, comp_val2, comp_val2a, valid_tx;
extern Uint8 lapm_established, bis_accept, get_ready;
extern Uint16 p0, p0_init, p1, p1_init, p2, p2_init;
/* p0's bits, the directions V.42bis compresses, from the modem's side:
 * P0_CODER for what it sends, and P0_DECODER for what it receives */
#define P0_CODER 0x01
#define P0_DECODER 0x02
extern Uint8 crc_init, crc, s_rej_init, s_rej, loopb_init, loopb;
/* where offered or agreed, s_rej holds HDLC_SREJ and loopb HDLC_TEST, each as
 * the XID's HDLC options carry it. crc is 1 for a 32-bit FCS */
#define HDLC_SREJ 0x04
#define HDLC_TEST 0x20
extern Uint16 tx_max_packet_size, test_packet_size;
extern Uint8 *dce_in_pntr;
extern Uint16 dce_in_cntr;
extern Uint8 overflow_buffer[LAPM_OVERFLOW_BYTES], overflow_read,
    overflow_write, overflow_cnt;
#define KEY_BYTES 8
extern Uint8 encryption, key_string[KEY_BYTES], ENCRYPT_KEY[KEY_BYTES];
extern Uint16 RxBreakLength[LAPM_BREAKS];
extern Uint8 *RxBreakPointer[LAPM_BREAKS];
extern Uint16 TxBreakLength[LAPM_BREAKS];

#endif
