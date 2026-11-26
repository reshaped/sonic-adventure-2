#include <sg_xpt.h>
#include <string.h>
#include "anxdef.h"
#include "bisdef.h"
#include "dce.h"
#include "dte_vxd.h"
#include "ec_estab.h"
#include "ecdc_def.h"
#include "ecdcutil.h"
#include "user.h"
#include "v42.h"
#include "v42def.h"

enum detected_pattern {
    NO_PATTERN,
    ORIGINATOR_PATTERN,
    ANSWER_PATTERN,
    NO_LAPM_PATTERN,
    ALTERNATE_PATTERN,
    LAPM_FLAGS
};
/* the pattern's bytes tx_v42_detection_char writes at a time */
enum { DETECTION_BLOCK = 128 };
/* the patterns' characters. the originator's pattern alternates DC1 with even
 * and odd parity. rec_odp_adp takes 0x7e and 0x3f in turn for LAPM's flags */
enum {
    DC1_EVEN = 0x11,
    DC1_ODD = 0x91,
    DETECTION_CHAR_NUL = 0,
    DETECTION_CHAR_C = 0x43,
    DETECTION_CHAR_E = 0x45,
    DETECTION_CHAR_M = 0x4d,
    FLAG_CHAR = 0x7e,
    FLAG_CHAR_SHIFTED = 0x3f
};
/* how far detect_cnt counts before rec_odp_adp takes each pattern for
 * found */
enum { ODP_COUNT = 4, ADP_COUNT = 2, NO_LAPM_COUNT = 4, FLAGS_COUNT = 32 };
/* the phases' times, in 10 ms ticks. OIS(3) >> 3 gives the detection
 * phase's time in units of DETECTION_UNIT ticks */
enum {
    ORIGINATE_DELAY = 10,
    FLAGS_TICKS = 20,
    FINISH_TICKS = 50,
    REPEAT_TICKS = 100,
    ANSWER_DETECT_TICKS = 500,
    NEGOTIATE_TICKS = 1200,
    DETECTION_UNIT = 5
};
/* ticks that set setup_timer's deadline 256 ticks before the clock's reading.
 * the deadline has passed at once, unless the clock read below 256. such a
 * deadline wraps to nearly 2^32 */
#define RUN_OUT ((Uint32)0 - 256)
/* an asynchronous character less its parity bit */
#define CHARACTER_BITS 0x7f
/* the terminator of a fallback buffer without one: no 7-bit character
 * matches it */
#define NO_TERMINATOR 0x80

Uint8 ec_estab_phase;
Uint8 char_detected, detect_cnt;
Uint8 previous_char;
Uint8 mdp_sent, v42_call_no_ODP;
Uint32 invalid_data_timer;
Uint8 previous_ois_28;
static Uint8 previous_detection_byte;

static void init_ec_estab(void);
static void v42_detection_phase(void);
static void rx_v42_detection_char(void);
static void send_hdlc_flags(void);
static void v42_establishment_phase(void);
static void drop_the_link(void);
static void fallback(void);
static void fb_char_buff_data(void);
static void analyse_rx_data(void);
static void send_buffered_data(void);
static void tx_v42_detection_char(Uint8 first, Uint8 second);
static void rec_odp_adp(Uint8 byte);
static void init_v42_hand_shake(void);
static void set_dce_in_hdlc_mode(void);
static void user_send_buff_data(Uint8 *data, Uint16 count);

void ec_estab(void)
{
    /* @hack type: BMR(25), BMR(27) and BMR(29) read through a plain pointer,
     * each use cast to the pointer's own type, where BMR would do */
    Uint8 *registers = BMRregisters;
    if (STATUS_BITS & STATUS_HANGUP) {
        STATUS_BITS &= ~STATUS_HANGUP;
        drop_the_link();
        return;
    }
    /* a change of the line's speed starts the phase's timeout again */
    if (previous_ois_28 != OIS(28)) {
        /* @hack type: the count cleared a byte at a time, where
         * common_buffer.fallback.count = 0 would do */
        common_buffer.bytes[0] = 0;
        common_buffer.bytes[1] = 0;
        switch (ec_estab_phase) {
        case WAIT_TO_ORIGINATE:
            setup_timer(&connect_timer, ORIGINATE_DELAY);
            break;
        case DETECT_PATTERN:
            if (!(OIS(3) & OIS3_DETECTION)) {
                setup_timer(&connect_timer, RUN_OUT);
            } else if (!ans_org && (REQ_LINK & REQ_OFFER_LAPM)) {
                setup_timer(&connect_timer, ANSWER_DETECT_TICKS);
            } else {
                setup_timer(&connect_timer,
                            ((OIS(3) & OIS3_DETECTION) >> 3) * DETECTION_UNIT);
            }
            break;
        case REPEAT_ANSWER_PATTERN:
            setup_timer(&connect_timer, REPEAT_TICKS);
            break;
        case SEND_FLAGS:
            setup_timer(&connect_timer, FLAGS_TICKS);
            break;
        case NEGOTIATE_LAPM:
            setup_timer(&connect_timer, NEGOTIATE_TICKS);
            break;
        case START_FALLBACK:
            if ((REQ_LINK & REQ_ORIGINATE) &&
                char_detected != ALTERNATE_PATTERN) {
                setup_timer(&connect_timer, (OIS(3) >> 3) * DETECTION_UNIT);
            } else {
                setup_timer(&connect_timer, 0);
            }
            break;
        case WAIT_FALLBACK:
            setup_timer(&connect_timer, 0);
            break;
        default:
            break;
        }
    }
    previous_ois_28 = OIS(28);
    switch (ec_estab_phase) {
    case INITIALIZE:
        if ((REQ_LINK & REQ_LAPM) && (REQ_LINK & REQ_ORIGINATE)) {
            ec_estab_phase = WAIT_TO_ORIGINATE;
            setup_timer(&connect_timer, ORIGINATE_DELAY);
        } else {
            init_ec_estab();
        }
        memset(&StatisticInfo, 0, sizeof(StatisticInfo));
        break;
    case WAIT_TO_ORIGINATE:
        if (timer_expired(connect_timer)) {
            init_ec_estab();
        }
        break;
    case DETECT_PATTERN:
        v42_detection_phase();
        break;
    case REPEAT_ANSWER_PATTERN:
        rx_v42_detection_char();
        if (*(Uint8 *)(registers + 29) & 8) {
            *(Uint8 *)(registers + 27) = 0;
            setup_timer(&connect_timer, 0);
            ec_estab_phase = START_FALLBACK;
            break;
        }
        if (char_detected == LAPM_FLAGS || timer_expired(connect_timer)) {
            Uint8 *control = (Uint8 *)(registers + 27);
            *control &= 0xfe;
            *control |= BMR27_LAPM;
            setup_timer(&connect_timer, NEGOTIATE_TICKS);
            set_dce_in_hdlc_mode();
            ec_estab_phase = NEGOTIATE_LAPM;
        } else {
            tx_v42_detection_char(DETECTION_CHAR_C, DETECTION_CHAR_E);
        }
        break;
    case SEND_FLAGS:
        send_hdlc_flags();
        break;
    case NEGOTIATE_LAPM:
        v42_establishment_phase();
        break;
    case START_FALLBACK:
    case WAIT_FALLBACK:
        fallback();
        break;
    case DRAIN_LINE:
        /* @hack type: the (char) cast of dce_tx_buff_empty's result, where a
         * plain test would do */
        if ((char)dce_tx_buff_empty()) {
            setup_timer(&connect_timer, FINISH_TICKS);
            ec_estab_phase = FINISH_LINK;
        }
        break;
    case FINISH_LINK:
        rx_v42_detection_char();
        if (timer_expired(connect_timer) || !ans_org ||
            char_detected == LAPM_FLAGS) {
            if (bit_oriented_mode) {
                set_dce_in_hdlc_mode();
            }
            link_made();
        }
        break;
    }
    if (*(Uint8 *)(registers + 25) & 0x80) {
        fallback();
    }
}

static void init_ec_estab(void)
{
    BMR(23) = 0;
    BMR(25) &= 0x37;
    BMR(27) = 0;
    BMR(28) &= 0x18;
    BMR(29) = 0x10;
    clear_EC_ram();
    char_detected = NO_PATTERN;
    detect_cnt = 0;
    countx = 0;
    mdp_sent = 0;
    /* @hack type: the count cleared a byte at a time, where
     * common_buffer.fallback.count = 0 would do */
    common_buffer.bytes[0] = 0;
    common_buffer.bytes[1] = 0;
    if (!(REQ_LINK & REQ_OFFER_LAPM) || (REQ_LINK & REQ_LAPM)) {
        init_v42_hand_shake();
        v42_call_no_ODP = 0;
        if (!(REQ_LINK & REQ_ALTERNATE) && !(REQ_LINK & REQ_ALLOW_FALLBACK) &&
            (REQ_LINK & REQ_ORIGINATE) && !(OIS(9) & OIS9_SEND_ODP)) {
            v42_call_no_ODP = 1;
            BMR(27) |= BMR27_LAPM;
            BMR(27) &= 0xfe;
            set_dce_in_hdlc_mode();
            setup_timer(&connect_timer, FLAGS_TICKS);
            ec_estab_phase = SEND_FLAGS;
        } else {
            configure_dce(DCE_FORMAT_FF);
            BMR(27) |= 1;
            async_in_cntr = 0;
            async_in_cntr_bak = 0;
            if (!(OIS(3) & OIS3_DETECTION)) {
                setup_timer(&connect_timer, RUN_OUT);
            } else if (!ans_org && (REQ_LINK & REQ_OFFER_LAPM)) {
                setup_timer(&connect_timer, ANSWER_DETECT_TICKS);
            } else {
                setup_timer(&connect_timer,
                            ((OIS(3) & OIS3_DETECTION) >> 3) * DETECTION_UNIT);
            }
            ec_estab_phase = DETECT_PATTERN;
        }
    } else if (REQ_LINK & REQ_ALTERNATE) {
        if (REQ_LINK & REQ_ORIGINATE) {
            setup_timer(&connect_timer, (OIS(3) >> 3) * DETECTION_UNIT);
        } else {
            setup_timer(&connect_timer, 0);
        }
        ec_estab_phase = START_FALLBACK;
    }
}

static void v42_detection_phase(void)
{
    if (ans_org) {
        tx_v42_detection_char(DC1_ODD, DC1_EVEN);
        rx_v42_detection_char();
        if (char_detected == ALTERNATE_PATTERN) {
            BMR(27) = 0;
            setup_timer(&connect_timer, 0);
            ec_estab_phase = START_FALLBACK;
        } else if (char_detected == ANSWER_PATTERN ||
                   char_detected == LAPM_FLAGS) {
            BMR(27) &= 0xfe;
            BMR(27) |= BMR27_LAPM;
            setup_timer(&connect_timer, FLAGS_TICKS);
            set_dce_in_hdlc_mode();
            ec_estab_phase = SEND_FLAGS;
        } else if (char_detected == NO_LAPM_PATTERN) {
            BMR(27) |= 8;
            fallback();
            return;
        }
    } else {
        rx_v42_detection_char();
        if (BMR(29) & 8) {
            BMR(27) = 0;
            setup_timer(&connect_timer, 0);
            ec_estab_phase = START_FALLBACK;
            return;
        }
        if (char_detected == ORIGINATOR_PATTERN || countx) {
            if (REQ_LINK & REQ_OFFER_LAPM) {
                if ((OIS(10) & 0x10) && !mdp_sent) {
                    if (countx < 10) {
                        tx_v42_detection_char(DETECTION_CHAR_M,
                                              DETECTION_CHAR_E);
                    } else {
                        mdp_sent = 1;
                        countx = 0;
                        tx_v42_detection_char(DETECTION_CHAR_C,
                                              DETECTION_CHAR_E);
                    }
                } else {
                    tx_v42_detection_char(DETECTION_CHAR_C, DETECTION_CHAR_E);
                }
            } else {
                tx_v42_detection_char(DETECTION_CHAR_NUL, DETECTION_CHAR_E);
            }
        }
        if (char_detected == LAPM_FLAGS || countx > 21) {
            if (REQ_LINK & REQ_OFFER_LAPM) {
                if (char_detected != LAPM_FLAGS) {
                    setup_timer(&connect_timer, REPEAT_TICKS);
                    ec_estab_phase = REPEAT_ANSWER_PATTERN;
                } else {
                    BMR(27) &= 0xfe;
                    BMR(27) |= BMR27_LAPM;
                    setup_timer(&connect_timer, NEGOTIATE_TICKS);
                    set_dce_in_hdlc_mode();
                    ec_estab_phase = NEGOTIATE_LAPM;
                }
            } else {
                fallback();
            }
        }
    }
    if (timer_expired(connect_timer)) {
        if (REQ_LINK & REQ_ALLOW_FALLBACK) {
            fallback();
        } else {
            BMR(27) |= BMR27_LAPM;
            setup_timer(&connect_timer, NEGOTIATE_TICKS);
            set_dce_in_hdlc_mode();
            ec_estab_phase = NEGOTIATE_LAPM;
        }
    }
}

static void rx_v42_detection_char(void)
{
    Uint8 pattern = char_detected;
    if (!async_in_cntr_bak) {
        dce_get_rx_data(&async_in_pntr, &async_in_cntr);
        async_in_cntr_bak = async_in_cntr;
        async_in_pntr_bak = async_in_pntr;
    }
    if (!async_in_cntr_bak) {
        return;
    }
    while (pattern == char_detected && async_in_cntr_bak) {
        rec_odp_adp(*async_in_pntr_bak++);
        async_in_cntr_bak--;
    }
    if (async_in_cntr_bak == 0) {
        if ((!ans_org && (REQ_LINK & REQ_ALTERNATE)) ||
            ec_estab_phase == WAIT_FALLBACK) {
            async_in_cntr_bak = async_in_cntr;
            async_in_pntr_bak = async_in_pntr;
        }
        fb_char_buff_data();
        dce_rx_data_done();
    }
}

static void send_hdlc_flags(void)
{
    if (timer_expired(connect_timer)) {
        setup_timer(&connect_timer, NEGOTIATE_TICKS);
        ec_estab_phase = NEGOTIATE_LAPM;
    }
}

static void v42_establishment_phase(void)
{
    v42();
    if (BMR(27) & BMR27_LINK_DOWN) {
        BMR(27) = 0;
        drop_the_link();
    }
    if (BMR(27) & BMR27_LINK_UP) {
        link_made();
    }
    if (!(REQ_LINK & REQ_IGNORE_TIMEOUT) && timer_expired(connect_timer)) {
        BMR(27) = 0;
        StatisticInfo.link_end = LINK_END_SETUP_FAILED;
        drop_the_link();
    }
}

static void drop_the_link(void)
{
    if (STATUS_BITS & STATUS_CONNECTED) {
        fallback();
        return;
    }
    BMR(27) &= 8;
    BMR(29) = 0;
    BMR(25) = 0x7f;
    scm_main_phase = RESET_PHASE;
}

static void fallback(void)
{
    if (STATUS_BITS & STATUS_CONNECTED) {
        REQ_LINK |= REQ_ALLOW_FALLBACK;
    }
    BMR(27) &= 8;
    BMR(29) = 0;
    BMR(25) = 0x7f;
    if (!(REQ_LINK & (REQ_ALLOW_FALLBACK | REQ_RAW))) {
        drop_the_link();
        StatisticInfo.link_end = LINK_END_SETUP_FAILED;
    } else {
        if (!(REQ_LINK & REQ_RAW) && (OIS(1) & OIS1_FALLBACK_SEND)) {
            send_buffered_data();
        }
        link_made();
    }
}

static void fb_char_buff_data(void)
{
    if (!(BMR(29) & 8) &&
        (OIS(1) & (OIS1_FALLBACK_SEND | OIS1_FALLBACK_TERMINATOR)) &&
        !(BMR(25) & 0x80) && (REQ_LINK & REQ_ALLOW_FALLBACK) &&
        timer_expired(invalid_data_timer) && (ans_org || !countx)) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((async_in_cntr_bak = async_in_cntr) != 0) {
            async_in_pntr_bak = async_in_pntr;
            analyse_rx_data();
            async_in_pntr_bak = async_in_pntr;
            async_in_cntr_bak = 0;
        }
    }
}

static void analyse_rx_data(void)
{
    Uint8 terminator;
    Uint16 limit;
    Uint16 count;
    /* @hack spelling: common_buffer's fallback reached through buffer, data
     * and header, where its fields would do */
    Uint8 *data;
    Uint16 *header;
    union modem_compression_storage *buffer;
    if (OIS(1) & OIS1_FALLBACK_TERMINATOR) {
        terminator = OIS(2) & CHARACTER_BITS;
    } else {
        terminator = NO_TERMINATOR;
    }
    if (OIS(1) & OIS1_FALLBACK_SEND) {
        limit = 512;
    } else {
        limit = 4000;
    }
    buffer = &common_buffer;
    data = buffer->fallback.data;
    header = &buffer->fallback.count;
    count = *header;
    while (async_in_cntr_bak--) {
        if ((*async_in_pntr_bak & CHARACTER_BITS) != DC1_EVEN) {
#ifdef NONMATCHING
            /* this side stops at the end of fallback.data, which the matching
             * side writes past, and sets bit 7 of BMR(25) */
            if (count >= sizeof(buffer->fallback.data)) {
                BMR(25) |= 0x80;
                break;
            }
#endif
            data[count++] = *async_in_pntr_bak;
        }
        if ((*async_in_pntr_bak++ & CHARACTER_BITS) == terminator ||
            count > limit) {
            BMR(25) |= 0x80;
        }
    }
    async_in_cntr_bak = 0;
    *header = count;
}

static void send_buffered_data(void)
{
#ifdef NONMATCHING
    /* this side sends fallback.data by its own name, the plain form the hack
     * tag below gives, and refuses a count past its end */
    Uint16 count = common_buffer.fallback.count;
    if (count && count <= sizeof(common_buffer.fallback.data)) {
        user_send_buff_data(common_buffer.fallback.data, count);
    }
#else
    /* @hack spelling: the data reached through header, past the count, where
     * common_buffer.fallback.data would do */
    Uint16 *header = &common_buffer.fallback.count;
    if (*header) {
        Uint16 count = *header++;
        user_send_buff_data((Uint8 *)header, count);
    }
#endif
}

static void tx_v42_detection_char(Uint8 first, Uint8 second)
{
    Uint8 *data;
    Uint8 count;
#ifdef NONMATCHING
    /* this side writes no more than the room dce_set_tx_data gives, and
     * DETECTION_BLOCK at most */
    Uint16 available = dce_set_tx_data(&data);
    if (available > DETECTION_BLOCK) {
        available = DETECTION_BLOCK;
    }
    if (available) {
        for (count = 0; count < available; count++) {
#else
    /* the block always fits: dce_set_tx_data gives DCE_FRAME_BYTES of room,
     * or none */
    if (dce_set_tx_data(&data)) {
        for (count = 0; count < (Uint8)DETECTION_BLOCK; count++) {
#endif
            if (countx++ % 2) {
                *data++ = first;
            } else {
                *data++ = second;
            }
        }
        dce_tx_data_done(count, 1);
    }
}

static void rec_odp_adp(Uint8 byte)
{
    Uint8 old_count = detect_cnt;
    switch (byte) {
    case DC1_EVEN:
        if (previous_detection_byte == DC1_ODD) {
            detect_cnt++;
        }
        break;
    case DC1_ODD:
        if (previous_detection_byte == DC1_EVEN) {
            if (detect_cnt++ >= ODP_COUNT) {
                char_detected = ORIGINATOR_PATTERN;
            }
        }
        break;
    case DETECTION_CHAR_E:
        if (previous_detection_byte == DETECTION_CHAR_C ||
            previous_detection_byte == DETECTION_CHAR_NUL ||
            previous_detection_byte == DETECTION_CHAR_M) {
            detect_cnt++;
        }
        break;
    case DETECTION_CHAR_C:
        if (previous_detection_byte == DETECTION_CHAR_E) {
            if (detect_cnt++ >= ADP_COUNT) {
                char_detected = ANSWER_PATTERN;
            }
        }
        break;
    case DETECTION_CHAR_NUL:
        if (previous_detection_byte == DETECTION_CHAR_E) {
            if (detect_cnt++ >= NO_LAPM_COUNT) {
                char_detected = NO_LAPM_PATTERN;
            }
        }
        break;
    case DETECTION_CHAR_M:
        if (previous_detection_byte == DETECTION_CHAR_E) {
            if ((OIS(10) & 0x30) && detect_cnt++ >= ADP_COUNT) {
                char_detected = ALTERNATE_PATTERN;
            }
        }
        break;
    case FLAG_CHAR_SHIFTED:
        if (previous_detection_byte == FLAG_CHAR) {
            detect_cnt++;
        }
        break;
    case FLAG_CHAR:
        if (previous_detection_byte == FLAG_CHAR_SHIFTED) {
            if (detect_cnt++ >= FLAGS_COUNT) {
                char_detected = LAPM_FLAGS;
            }
        }
        break;
    }
    previous_detection_byte = byte;
    if (old_count == detect_cnt) {
        detect_cnt = 0;
    }
}

static void init_v42_hand_shake(void)
{
    Uint8 dictionary;
    xid_rec = 0;
    timer_t401_max = ((OIS(8) & OIS8_T401) * 10) + 400;
    p0 = (OIS(11) & OIS11_P0) >> 2;
    dictionary = OIS(19) & OIS19_DICTIONARY;
    if (dictionary == 0) {
        p1 = 512;
    } else if (dictionary == 0x40) {
        p1 = V42BIS_MAX_NODES;
    } else if (dictionary == 0x80) {
        p1 = 2048;
    } else {
        p1 = V42BIS_MAX_NODES;
    }
    if (p1 > V42BIS_MAX_NODES) {
        p1 = V42BIS_MAX_NODES;
    }
    p2 = (OIS(19) & ~OIS19_DICTIONARY) + 6;
    bis_accept = 0;
    if (OIS(8) & OIS8_SREJ) {
        s_rej = HDLC_SREJ;
    } else {
        s_rej = 0;
    }
    if (OIS(8) & OIS8_FCS32) {
        crc = 1;
    } else {
        crc = 0;
    }
    if (OIS(9) & OIS9_LOOPBACK) {
        loopb = HDLC_TEST;
    } else {
        loopb = 0;
    }
    n400 = (OIS(0) & OIS0_N400) >> 4;
    n401_tx = OIS(1) & OIS1_N401;
    n401_tx = (n401_tx << 2) + 64;
    n401_rx = n401_tx;
    win_k_tx = OIS(9) & OIS9_WINDOW;
    if (win_k_tx < 15) {
        win_k_rx = 15;
    } else {
        win_k_rx = win_k_tx;
    }
    if (REQ_LINK & REQ_ORIGINATE) {
        ans_org = 1;
    } else {
        ans_org = 0;
    }
    p0_init = p0;
    p1_init = p1;
    p2_init = p2;
    s_rej_init = s_rej;
    crc_init = crc;
    loopb_init = loopb;
    n401_rx_init = n401_rx;
    n401_tx_init = n401_tx;
    win_k_tx_init = win_k_tx;
    win_k_rx_init = win_k_rx;

    RR_CNT = 0;
    RR_READ = RR_BUFFER;
    RR_WRITE = RR_BUFFER;
    RR_TOPP = RR_BUFFER + LAPM_RING_BYTES - 1;
    rr_shadow_ptr = RR_BUFFER;
    rr_cnt_shadow = 0;
    tm_lnt = LAPM_RING_BYTES;
    TM_CNT = 0;
    TM_BUFFER = TM_BUFFER_ARRAY;
    TM_READ = TM_BUFFER;
    TM_WRITE = TM_BUFFER;
    TM_TOPP = TM_BUFFER + LAPM_RING_BYTES - 1;
    tm_shadow_ptr = TM_BUFFER;
    tm_cnt_shadow = 0;
    tm_read_temp = TM_BUFFER;
    tm_frame_address = retrans_array;
    tm_frame_length = retrans_array + LAPM_SEQUENCE_NUMBERS;
    *tm_frame_address = 0;
    srej_buff_topp = srej_buffer + LAPM_REJECT_BYTES - 1;
    overflow_read = overflow_write = overflow_cnt = 0;
}

static void set_dce_in_hdlc_mode(void)
{
    BMR(29) |= 0x40;
    dce_clr_rx_buff();
    dce_clr_tx_buff();
    configure_dce(DCE_HDLC16);
}

static void user_send_buff_data(Uint8 *data, Uint16 count)
{
    Uint8 *output;
    Uint16 available;
    Uint16 written;
    while (count) {
        written = 0;
        available = vxd_dte_set_tx_data(&output);
        if (available) {
            for (; count && written < available; written++, count--) {
                *output++ = *data++;
            }
            vxd_dte_tx_data_done(written);
        } else {
            return;
        }
    }
}
