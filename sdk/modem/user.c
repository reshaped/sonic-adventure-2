#include <sg_xpt.h>
#include "anxdef.h"
#include "command.h"
#include "ecdc_def.h"
#include "user.h"
#include "v42def.h"

void remote_err(void)
{
    cmd_resp_to_host(ECDC_RESPONSE_REMOTE_ERROR);
}

void local_err(void)
{
    cmd_resp_to_host(ECDC_RESPONSE_LOCAL_ERROR);
}

void test_good(void)
{
    cmd_resp_to_host(ECDC_RESPONSE_TEST_PASSED);
}

void test_bad(Uint8 reason)
{
    /* @hack spelling: each reason's response written apart, where reason <<
     * TEST_REASON_SHIFT would do for a reason from 1 to 3 */
    if (reason == TEST_NO_LOOPBACK) {
        cmd_response_buffer[1] = TEST_NO_LOOPBACK << TEST_REASON_SHIFT;
    } else if (reason == TEST_TIMED_OUT) {
        cmd_response_buffer[1] = TEST_TIMED_OUT << TEST_REASON_SHIFT;
    } else {
        cmd_response_buffer[1] = TEST_ANSWER_DIFFERS << TEST_REASON_SHIFT;
    }
    cmd_resp_to_host(ECDC_RESPONSE_TEST_BAD);
}

void clear_EC_ram(void)
{
    int remaining;
    Uint8 *bytes;
    Uint16 *words;

    TM_CNT = 0;
    TM_BUFFER = TM_BUFFER_ARRAY;
    bytes = TM_BUFFER;
    for (remaining = LAPM_RING_BYTES; remaining > 0; remaining--) {
        *bytes++ = 0;
    }
    bytes = RR_BUFFER;
    for (remaining = LAPM_RING_BYTES; remaining > 0; remaining--) {
        *bytes++ = 0;
    }
    RR_CNT = 0;
    bytes = srej_buffer;
    for (remaining = LAPM_REJECT_BYTES; remaining > 0; remaining--) {
        *bytes++ = 0;
    }
    srej_buff_cntr = 0;
    srej_frame_length = 0;
    vs_srej = 0;
    srej_condition = 0;
    srej_to_send = 0;
    srej_on = 0;
    tm_cnt_temp = 0;
    rm_frame_length = 0;
    ch = 0;
    ns_rx = 0;
    nr_rx = 0;
    va = 0;
    vs = 0;
    vr = 0;
    previous_va = 0;
    nrb_tx = 0;
    vrb = 0;
    vsb = 0;
    nsb_tx = 0;
    tx_brk = 0;
    fn = 0;

    timer_t401_max = 0;
    timer_t401_offset = 0;
    timer_t403_offset = 0;
    timer_test_offset = 0;
    timer_t401_run = 0;
    timer_t403_run = 0;
    n400 = 0;
    retransmission_count = 0;
    tx_frame_number = 0;
    tx_code_number = 0;
    pf_rx = 0;
    cr_rx = 0;
    pf_tx = 0;
    cr_tx = 0;
    tx_address = 0;
    break_handling_option_s = 0;
    break_handling_option_d = 0;
    break_sequence_number_rx = 0;
    break_length = 0;
    tx_break_length = 0;
    break_option = 0;
    brk_sent = 0;
    destructive = 0;
    break_or_sabme_sent = 0;
    same_nr_as_previous = 0;
    timer_recovery_condition = 0;
    reject_condition = 0;
    remote_busy = 0;
    own_busy = 0;
    break_type = 0;
    ans_org = 0;
    tr_full = 0;
    recover_with_rej = 0;
    xid_sent = 0;
    xid_rec = 0;
    sabme_sent = 0;
    enter_disconnect = 0;
    no_connect_possible = 0;
    disc_sent = 0;
    test_sent = 0;
    remote_is_mma = 0;

    group_length = 0;
    group_identifier = 0;
    parameter_identifier = 0;
    parameter_length = 0;
    n401_tx = 0;
    n401_tx_init = 0;
    n401_rx = 0;
    n401_rx_init = 0;
    win_k_tx = 0;
    win_k_rx = 0;
    win_k_tx_init = 0;
    win_k_rx_init = 0;
    win_k_tx_used = 0;
    bytes = hdlc;
    for (remaining = sizeof hdlc; remaining > 0; remaining--) {
        *bytes++ = 0;
    }
    connect_timer = 0;
    seq = 0;
    adj_cnt = 0;
    num_adj_obsv = 0;
    countx = 0;
    received_work = 0;
    comp = 0;
    comp_val1 = 0;
    comp_val2 = 0;
    comp_val2a = 0;
    valid_tx = 0;
    lapm_established = 0;
    bis_accept = 0;
    get_ready = 0;
    p0 = 0;
    p0_init = 0;
    p1 = 0;
    p1_init = 0;
    p2 = 0;
    p2_init = 0;
    crc_init = 0;
    crc = 0;
    s_rej_init = 0;
    s_rej = 0;
    loopb_init = 0;
    loopb = 0;
    tx_max_packet_size = 0;
    test_packet_size = 0;
    dce_in_cntr = 0;
    encryption = 0;
    words = RxBreakLength;
    for (remaining = LAPM_BREAKS; remaining > 0; remaining--) {
        *words++ = 0;
    }
    words = TxBreakLength;
    for (remaining = LAPM_BREAKS; remaining > 0; remaining--) {
        *words++ = 0;
    }
}

void divide_t401(void)
{
#ifdef NONMATCHING
    /* this side skips a division by a zero bps, which SHC's runtime answers
     * with 0 */
    if (!bps) {
        return;
    }
#endif
    host_t401 /= bps;
}
