#include <sg_xpt.h>
#include <string.h>
#include "anxdef.h"
#include "command.h"
#include "dte_vxd.h"
#include "ecdc_def.h"
#include "ecdcutil.h"
#include "messages.h"
#include "tci_ecdc.h"
#include "tci_modm.h"
#include "v42def.h"

#define PROTOCOL_RAW 1
#define PROTOCOL_NORMAL 2
#define PROTOCOL_MNP_ASYNC 3
#define PROTOCOL_MNP_SYNC 4
#define PROTOCOL_LAPM 5
#define OIS_PARAMETERS 29
/* OIS(25)'s fields: how long HALT_AFTER_LINE and HALT_AFTER_TERMINAL wait */
#define LINE_WAIT 0x03
#define TERMINAL_WAIT 0x0c
/* the waits a field gives. each sets fhud_timer, the halt's deadline.
 * WAIT_SECONDS sets it OIS(18) seconds on, and WAIT_FOREVER at the 10 ms
 * clock's LAST_TICK. a field of 3 leaves the deadline as it was */
#define WAIT_NONE 0
#define WAIT_FOREVER 1
#define WAIT_SECONDS 2
#define LAST_TICK 0xffffffff

/* the REQ_LINK each mode asks for, before the line connects and once it
 * has */
static const Uint16 req_link_offline[16] = {
    1, 0x10, 0x82, 0x83, 0x84, 0x86, 0x87, 0x85, 0x100, 0x200, 0x400};
static const Uint16 req_link_online[16] = {0x86, 0,    0x82,  0x82,  0x84, 0x86,
                                           0x86, 0x84, 0x100, 0x200, 0x400};

/* cmd_DIAG tests the first byte of this function's code */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void start_of_ECDC_rom(void)
{
}

void cmd_resp_to_host(Uint8 response)
{
    last_response = response;
    cmd_response_buffer[0] = response;
    switch (response) {
    case ECDC_RESPONSE_RESET:
        cmd_response_buffer[1] = 0;
        if (fhud_failed == 1) {
            cmd_response_buffer[1] = 1;
        }
        fhud_failed = 0;
        break;
    case ECDC_RESPONSE_CONNECTED:
        break;
    }
    TCI_tx_response();
}

void cmd_ACC_LINK(void)
{
    if (!(STATUS_BITS & STATUS_LINKING)) {
        if (!(STATUS_BITS & STATUS_CONNECTED)) {
            TCI_sw_reset();
        } else if ((BMR(27) & BMR27_LAPM) || (BMR(29) & BMR29_MNP)) {
            cmd_resp_to_host(ECDC_RESPONSE_CONNECTED);
        } else if (!get_OIS()) {
            REQ_LINK &= (Uint16)~REQ_ORIGINATE;
            REQ_LINK |= REQ_IGNORE_TIMEOUT;
            STATUS_BITS |= STATUS_LINKING;
        }
    }
}

void cmd_DIAG(void)
{
    const char *rom = (const char *)start_of_ECDC_rom;
    /* @hack spelling: flags, a local that holds the 0 stored to
     * cmd_response_buffer[1], where the 0 itself would do */
    Uint8 flags = 0;
    cmd_response_buffer[1] = flags;
    /* @bug logic: the test never holds, so the diagnostic always reports
     * ECDC_RESPONSE_DIAG_ERROR. the byte it reads is the low byte of
     * start_of_ECDC_rom's RTS, 0x0b, at 0x0caee73c */
    if (*rom == 0) {
        ecdc_self_test = ECDC_RESPONSE_DIAG_OK;
        cmd_resp_to_host(ECDC_RESPONSE_DIAG_OK);
    } else {
        ecdc_self_test = ECDC_RESPONSE_DIAG_ERROR;
        cmd_resp_to_host(ECDC_RESPONSE_DIAG_ERROR);
    }
}

void cmd_GET_ID(void)
{
    cmd_response_buffer[1] = 0;
    cmd_response_buffer[2] = 0;
    cmd_resp_to_host(ECDC_RESPONSE_ID);
}

void cmd_GET_STAT(struct EcdcStatistics *statistics)
{
    const struct EcdcStatistics *current;
    if (last_nb_out) {
        StatisticInfo.compression_ratio = 100 * RATIO_INPUT / last_nb_out;
    }
    current = &StatisticInfo;
    statistics->compression_ratio = current->compression_ratio;
    statistics->frames_sent = current->frames_sent;
    statistics->frames_received = current->frames_received;
    statistics->frames_resent = current->frames_resent;
    statistics->rejects_sent = current->rejects_sent;
    statistics->timeouts = current->timeouts;
    statistics->rejects_received = current->rejects_received;
    statistics->link_end = current->link_end;
}

void cmd_CLEAR_STAT(void)
{
    memset(&StatisticInfo, 0, sizeof(StatisticInfo));
}

struct EcdcStatus *cmd_GET_STATUS(void)
{
    memset(&StatusInfo, 0, sizeof(StatusInfo));
    if (STATUS_BITS & STATUS_CONNECTED) {
        if (BMR(29) & BMR29_MNP) {
            if (fixed_field_lt_la) {
                StatusInfo.fixed_header = 1;
            }
            if (max_info_field) {
                StatusInfo.max_information_field = 1;
            }
            StatusInfo.frame_size = n401;
            if (bit_oriented_mode) {
                StatusInfo.error_corrected = 1;
            }
            if (bit_oriented_mode) {
                StatusInfo.protocol = PROTOCOL_MNP_SYNC;
            } else {
                StatusInfo.protocol = PROTOCOL_MNP_ASYNC;
            }
            if (class_5) {
                StatusInfo.compress_rx = 1;
                StatusInfo.compress_tx = 1;
            }
            StatusInfo.receive_window = StatusInfo.transmit_window = k;
        } else if (BMR(27) & BMR27_LAPM) {
            if (s_rej) {
                StatusInfo.selective_reject = 1;
            }
            if (crc) {
                StatusInfo.crc32 = 1;
            }
            if (n401_tx > n401_tx_init) {
                StatusInfo.frame_size = n401_tx_init;
            } else {
                StatusInfo.frame_size = n401_tx;
            }
            StatusInfo.error_corrected = 1;
            StatusInfo.protocol = PROTOCOL_LAPM;
            StatusInfo.max_string_length = p2;
            if (bis_accept) {
                StatusInfo.compress_tx = p0 ? 1 : 0;
                StatusInfo.compress_rx = p0 ? 1 : 0;
                StatusInfo.dictionary_size = p1;
            }
            StatusInfo.receive_window = win_k_rx;
            StatusInfo.transmit_window = win_k_tx_used;
        } else if (!(BMR(28) & 0xc0)) {
            StatusInfo.protocol = PROTOCOL_NORMAL;
        } else {
            StatusInfo.protocol = PROTOCOL_RAW;
        }
    }
    return &StatusInfo;
}

void cmd_HALT(void)
{
    Uint8 mode;
    if (!(STATUS_BITS & STATUS_CONNECTED) ||
        cmd_parameter_buffer[0] == HALT_NOW || (BMR(28) & 0xc0)) {
        TCI_sw_reset();
        return;
    }
    if (cmd_parameter_buffer[0] == HALT_AFTER_TERMINAL) {
        if (STATUS_BITS & STATUS_COMMAND_MODE) {
            TCI_sw_reset();
            return;
        }
        mode = (OIS(25) & TERMINAL_WAIT) >> 2;
        scm_main_phase = HALT_AFTER_TERMINAL_PHASE;
    } else if (cmd_parameter_buffer[0] == HALT_END_WAIT) {
        mode = WAIT_NONE;
    } else {
        if (!(BMR(27) & BMR27_LAPM) && !(BMR(29) & BMR29_MNP)) {
            TCI_sw_reset();
            return;
        }
        BMR(28) |= BMR28_HALT_AFTER_LINE;
        mode = OIS(25) & LINE_WAIT;
        scm_main_phase = HALT_AFTER_LINE_PHASE;
    }
    switch (mode) {
    case WAIT_NONE:
        setup_timer(&fhud_timer, 0);
        break;
    case WAIT_FOREVER:
        fhud_timer = LAST_TICK;
        break;
    case WAIT_SECONDS:
        setup_timer(&fhud_timer, OIS(18) * TICKS_PER_SECOND);
        break;
    }
}

void cmd_INIT_LINK(void)
{
    if (!(STATUS_BITS & STATUS_LINKING)) {
        if (!(STATUS_BITS & STATUS_CONNECTED)) {
            TCI_sw_reset();
        } else if ((BMR(27) & BMR27_LAPM) || (BMR(29) & BMR29_MNP)) {
            cmd_resp_to_host(ECDC_RESPONSE_CONNECTED);
        } else if (!get_OIS()) {
            REQ_LINK |= REQ_ORIGINATE;
            STATUS_BITS |= STATUS_LINKING;
        }
    }
}

void cmd_LOOP_TST(void)
{
    if (BMR(27) & BMR27_LAPM) {
        BMR(27) |= BMR27_TEST_REQUEST;
    } else {
        cmd_response_buffer[1] = 0;
        cmd_resp_to_host(ECDC_RESPONSE_TEST_BAD);
    }
}

void cmd_MAKE_LINK(void)
{
    if (!(STATUS_BITS & STATUS_LINKING)) {
        if (STATUS_BITS & STATUS_CONNECTED) {
            cmd_resp_to_host(ECDC_RESPONSE_CONNECTED);
        } else if (!get_OIS()) {
            if (OIS(7) & OIS7_ORIGINATE) {
                REQ_LINK |= REQ_ORIGINATE;
            }
            STATUS_BITS |= STATUS_LINKING;
        }
    }
}

void cmd_NORM_LINK(void)
{
    if (!(STATUS_BITS & STATUS_LINKING)) {
        if (!(STATUS_BITS & STATUS_CONNECTED)) {
            TCI_sw_reset();
        } else if ((BMR(27) & BMR27_LAPM) || (BMR(29) & BMR29_MNP)) {
            cmd_resp_to_host(ECDC_RESPONSE_CONNECTED);
        } else if (!get_OIS()) {
            if (OIS(7) & OIS7_ORIGINATE) {
                REQ_LINK |= REQ_ORIGINATE;
            }
            STATUS_BITS |= STATUS_LINKING;
        }
    }
}

void cmd_OIS(void)
{
    if (!(STATUS_BITS & STATUS_LINKING)) {
        if (get_OIS()) {
            return;
        }
        REQ_LINK &= 0x71;
    }
    cmd_resp_to_host(ECDC_RESPONSE_READY);
}

void cmd_RESEND(void)
{
    cmd_resp_to_host(last_response);
}

void cmd_SND_BRK(void)
{
    if (!(STATUS_BITS & STATUS_CONNECTED)) {
        cmd_resp_to_host(ECDC_RESPONSE_READY);
    } else {
        dte_simulate_rx_break(cmd_parameter_buffer[0]);
    }
}

void cmd_START_DATA(void)
{
    if (STATUS_BITS & STATUS_CONNECTED) {
        do_strt_data();
    }
    cmd_resp_to_host(ECDC_RESPONSE_READY);
}

void do_strt_data(void)
{
    if (STATUS_BITS & STATUS_COMMAND_MODE) {
        STATUS_BITS &= ~STATUS_COMMAND_MODE;
        re_configure_dte();
    }
}

void cmd_STOP_DATA(void)
{
    if (STATUS_BITS & STATUS_CONNECTED) {
        do_stop_data();
    }
    cmd_resp_to_host(ECDC_RESPONSE_READY);
}

void do_stop_data(void)
{
    if (!(STATUS_BITS & STATUS_COMMAND_MODE)) {
        STATUS_BITS |= STATUS_COMMAND_MODE;
        configure_dte(DTE_HELD);
    }
}

void cmd_STOP_LINK(void)
{
    if (STATUS_BITS & (STATUS_LINKING | STATUS_CONNECTED)) {
        if ((STATUS_BITS & STATUS_LINKING) || (BMR(27) & BMR27_LAPM) ||
            (BMR(29) & BMR29_MNP)) {
            reason_code = 0;
            user_code = 0;
            STATUS_BITS |= STATUS_HANGUP;
        } else {
            cmd_resp_to_host(ECDC_RESPONSE_CONNECTED);
        }
    } else {
        TCI_sw_reset();
    }
}

char get_OIS(void)
{
    Uint8 index;
    Uint8 checksum;
    checksum = 0;
    /* cmd_parameter_buffer[OIS_PARAMETERS] makes the bytes through it sum
     * to 0 */
    for (index = 0; index <= OIS_PARAMETERS; index++) {
        checksum += cmd_parameter_buffer[index];
    }
    if (checksum) {
        cmd_resp_to_host(ECDC_RESPONSE_BAD_CHECKSUM);
        return 1;
    }
    for (index = 0; index < OIS_PARAMETERS; index++) {
        OISregisters[index] = cmd_parameter_buffer[index];
    }
    configure_ECDC();
    return 0;
}

void configure_ECDC(void)
{
    Uint8 mode;

    mode = OIS(0) & LINK_MODE;
    if (STATUS_BITS & STATUS_CONNECTED) {
        REQ_LINK = req_link_online[mode];
    } else {
        REQ_LINK = req_link_offline[mode];
    }
    if ((OIS(10) & OIS10_FALLBACK_RAW) && (REQ_LINK & REQ_ALLOW_FALLBACK) &&
        (REQ_LINK & (REQ_LAPM | REQ_IGNORE_TIMEOUT))) {
        REQ_LINK &= (Uint16)~REQ_ALLOW_FALLBACK;
        REQ_LINK |= REQ_RAW;
    }
    if (OIS(28) <= TCI_SPEED_300) {
        REQ_LINK |= 0x20;
        REQ_LINK &= (Uint16)~REQ_LAPM;
        REQ_LINK &= (Uint16) ~(REQ_OFFER_LAPM | REQ_ALTERNATE);
    }
    DCE_INAC_TIM = 0;
    DTE_INAC_TIM = 0;
    if (STATUS_BITS & STATUS_CONNECTED) {
        re_configure_dce();
        if (!(STATUS_BITS & STATUS_COMMAND_MODE)) {
            re_configure_dte();
        }
    }
}
