/* LAPM, V.42's error control. an information frame carries its send number,
 * N(S). an information or supervisory frame carries N(R), the number its
 * sender expects next. both count modulo LAPM_SEQUENCE_NUMBERS. T401 bounds
 * the wait for an answer. while T401 is stopped, T403 runs from the last
 * frame received. once T403 has run T401's period, LAPM polls the peer.
 * a receiver that misses a frame asks by REJ for it and every frame after it.
 * where both ends agree on SREJ, it asks for the missed frame alone.
 * srej_buffer then keeps the frames after it */
#include <sg_xpt.h>

#include "bisdef.h"
#include "command.h"
#include "dce.h"
#include "dte_vxd.h"
#include "ecdc_def.h"
#include "ecdcutil.h"
#include "tci_ecdc.h"
#include "user.h"
#include "v42.h"
#include "v42bis.h"
#include "v42def.h"

/* the frames LAPM tells apart, as fn and tx_frame_number hold them. the
 * information and supervisory frames come before SET_BALANCED */
enum lapm_frame {
    NO_FRAME,
    INFORMATION,
    RECEIVE_READY,
    RECEIVE_NOT_READY,
    REJECT,
    SELECTIVE_REJECT,
    SET_BALANCED,
    DISCONNECTED,
    UNNUMBERED_INFO,
    DISCONNECT,
    ACKNOWLEDGE,
    FRAME_REJECT,
    EXCHANGE_ID,
    TEST_FRAME,
    UNKNOWN_FRAME
};

/* each frame's first control octet. an unnumbered frame has one, whose P/F
 * bit, POLL_FINAL, these values leave clear. a supervisory frame has a
 * second, which holds N(R) and the P/F bit */
enum hdlc_control {
    CTL_RR = 0x01,
    CTL_UI = 0x03,
    CTL_RNR = 0x05,
    CTL_REJ = 0x09,
    CTL_SREJ = 0x0d,
    CTL_DM = 0x0f,
    CTL_DISC = 0x43,
    CTL_UA = 0x63,
    CTL_SABME = 0x6f,
    CTL_FRMR = 0x87,
    CTL_XID = 0xaf,
    CTL_TEST = 0xe3,
    POLL_FINAL = 0x10
};
#define POLL_FINAL_SHIFT 4

/* a control octet's FORMAT_BITS give its frame's format. an information
 * frame's octet has FORMAT_SUPERVISORY's bit clear */
#define FORMAT_BITS 0x03
#define FORMAT_SUPERVISORY 0x01
#define FORMAT_UNNUMBERED 0x03
#define IS_INFORMATION(control) (!((control) & FORMAT_SUPERVISORY))
#define IS_UNNUMBERED(control) (((control) & FORMAT_BITS) == FORMAT_UNNUMBERED)
/* a sequence octet holds N(S) or N(R) above its low bit. under N(S) the bit
 * is 0, and under N(R) it is the P/F bit, SEQUENCE_PF */
#define SEQUENCE_SHIFT 1
#define SEQUENCE_PF 0x01

/* an address octet. ADDRESS_EA marks the address's last octet, and
 * ADDRESS_CR tells a command from a response. the originator sets ADDRESS_CR
 * in its commands, and the answerer in its responses. decode_command takes
 * an address whose first octet lacks ADDRESS_EA for LONG_ADDRESS_BYTES */
#define LONG_ADDRESS_BYTES 2
#define ADDRESS_EA 0x01
#define ADDRESS_CR 0x02
#define ADDRESS_CR_SHIFT 1
#define ADDRESS(command)                                                       \
    ((((command) ^ !ans_org) << ADDRESS_CR_SHIFT) + ADDRESS_EA)

/* a break's octets in a UI frame. the first holds BREAK_MESSAGE and the
 * break's sequence bit, with BREAK_ACK in an acknowledgement. the second
 * holds the break's handling, and a third, where present, its length in
 * 10 ms ticks. BREAK_BYTES counts all three */
#define BREAK_BYTES 3
#define BREAK_MESSAGE 0x40
#define BREAK_ACK 0x20
#define BREAK_SEQUENCE_SHIFT 7
#define BREAK_DESTRUCTIVE 0x80
#define BREAK_EXPEDITED 0x40

/* a frame's address and control field. they take INFORMATION_HEADER_BYTES
 * in an information frame and UNNUMBERED_BYTES in an unnumbered one. a
 * supervisory frame has nothing past its SUPERVISORY_BYTES */
#define INFORMATION_HEADER_BYTES 3
#define SUPERVISORY_BYTES 3
#define UNNUMBERED_BYTES 2

/* the frame size and window LAPM takes for a parameter an XID leaves out, or
 * gives as 0. n401_tx and win_k_tx fall below them only where the peer's XID
 * does */
#define DEFAULT_FRAME_BYTES 128
#define DEFAULT_WINDOW 15

/* the receive ring's fill. at RING_BUSY_BYTES the receiver tells the peer it
 * is busy. below RING_READY_BYTES it says it is ready again */
#define RING_BUSY_BYTES 3840
#define RING_READY_BYTES 2048
/* LAPM holds back information frames, new or sent again, while QUEUED_FRAMES
 * or more wait at the line. once the receive ring holds RING_BACKLOG_BYTES,
 * it holds them back while any frame waits */
#define RING_BACKLOG_BYTES 384
#define QUEUED_FRAMES 4
/* the most bytes of the receive ring that pass_RR_to_dte gives the host in
 * one block */
#define DTE_PASS_BYTES 128

/* the wait, in 10 ms ticks, before an XID command's sender changes to the
 * 32-bit FCS agreed */
#define CRC_CHANGE_TICKS 10

#define NEXT_SEQUENCE(value) (((value) + 1) & (LAPM_SEQUENCE_NUMBERS - 1))
/* @hack spelling: the assignments of the macros below as bare statements,
 * where do { } while (0) or a comma expression would do */
#define QUEUE_FRAME(frame, control)                                            \
    tx_frame_number = (frame);                                                 \
    tx_code_number = (control)
#define POLL_COMMAND()                                                         \
    cr_tx = 1;                                                                 \
    pf_tx = 1
#define FINAL_RESPONSE()                                                       \
    cr_tx = 0;                                                                 \
    pf_tx = 1
#define RESPONSE()                                                             \
    cr_tx = 0;                                                                 \
    pf_tx = 0
#define COMMAND()                                                              \
    cr_tx = 1;                                                                 \
    pf_tx = 0

/* set once an XID agrees on a 32-bit FCS. LAPM switches the line to it once
 * the line's transmit buffer is empty and crc_change_deadline has passed.
 * only the end that sent the XID command sets the deadline. the other end's
 * stays 0, as v42 set it */
/* @hack type: changing_crc a char, where a Uint8 would do */
static char changing_crc;
static Uint32 crc_change_deadline;
static void tx_flow_control(void);
static void rx_flow_control(void);
static void break_control(void);
static void fn_manager(void);
static void manager(void);
static void establishment_procedure(void);
static void setup_for_originate(void);
static void execute_command(void);
static Uint8 decode_command(void);
static void info(void);
static void sabme(void);
static void dm(void);
static void ui(void);
static void disc(void);
static void ua(void);
static void frmr(void);
static void xid(void);
static void test(void);
static void unid(void);
static void rr(void);
static void rej(void);
static void rnr(void);
static void srej(void);
static void transmit_frame(void);
static void tx_information(void);
static void tx_supervisory(void);
static void tx_xid(void);
static void tx_test(void);
static void tx_unnumbered(void);
static void tx_ui(void);
static void reset(void);
static void pass_srej_buffer(void);
static void re_transmit_srej(void);
static void scrap_srej_buff(void);
static void store_in_srej_buff(void);
static void test_manager(void);
static void pass_RR_to_dte(void);
static void pass_dte_to_TM(void);
static void buff_rx_EC_brk(Uint16 length);

void v42(void)
{
    if (lapm_established) {
        if (!bis_accept) {
            pass_RR_to_dte();
            if (!(BMR(25) & BMR25_BREAK_PENDING)) {
                pass_dte_to_TM();
            }
        } else if (!(p0 & P0_DECODER)) {
            pass_RR_to_dte();
        } else if (!(p0 & P0_CODER)) {
            if (!(BMR(25) & BMR25_BREAK_PENDING)) {
                pass_dte_to_TM();
            }
        }
        /* @hack spelling: changing_crc and ans_org compared with 1 and 0 here
         * and below in v42, where their truth would do */
        if (changing_crc == 1) {
            /* @hack type: dce_tx_buff_empty's Uint8 read as a char, where the
             * Uint8 would do */
            if ((char)dce_tx_buff_empty()) {
                if (!timer_expired(crc_change_deadline)) {
                    return;
                }
                changing_crc = 0;
                configure_dce(DCE_HDLC32);
                transmit_frame();
            } else {
                return;
            }
        }
        v42_empty_rx();
        if (changing_crc == 0) {
            rx_flow_control();
            transmit_frame();
            break_control();
            transmit_frame();
            manager();
            transmit_frame();
            test_manager();
            transmit_frame();
            tx_flow_control();
            tx_frame_number = INFORMATION;
            transmit_frame();
        }
    } else {
        changing_crc = 0;
        crc_change_deadline = 0;
        lapm_established = 1;
        if (ans_org == 1) {
            setup_for_originate();
            transmit_frame();
        }
    }
}

void v42_empty_rx(void)
{
    /* dce_get_rx_data takes a block received with an error itself. each loop
     * passes over those to the next block received whole */
    while (dce_get_rx_data(&dce_in_pntr, &dce_in_cntr)) {
    }
    while (dce_in_cntr && !enter_disconnect) {
        fn = decode_command();
        fn_manager();
        transmit_frame();
        execute_command();
        if (changing_crc == 0) {
            transmit_frame();
        }
        dce_rx_data_done();
        dce_in_cntr = 0;
        if (!changing_crc) {
            /* @hack spelling: an empty do while, where a while would do */
            do {
            } while (dce_get_rx_data(&dce_in_pntr, &dce_in_cntr));
        }
    }
}

/* overflow_buffer keeps each frame the line had no room for as a length
 * octet and the frame */
#define OVERFLOW_LENGTH_BYTES 1

/* send the frames overflow_buffer keeps. set tr_full where frames stay there,
 * destructive is set or too many frames wait at the line. tr_full holds back
 * information frames */
static void tx_flow_control(void)
{
    Uint8 count;
    Uint8 length;
    Uint8 frames;

    while (overflow_cnt && dce_set_tx_data(&pwork0)) {
        count = overflow_buffer[overflow_read++];
        if (overflow_read >= LAPM_OVERFLOW_BYTES) {
            overflow_read = 0;
        }
        length = count;
        while (count--) {
            *pwork0++ = overflow_buffer[overflow_read++];
            if (overflow_read >= LAPM_OVERFLOW_BYTES) {
                overflow_read = 0;
            }
        }
        overflow_cnt -= length + OVERFLOW_LENGTH_BYTES;
        dce_tx_data_done(length, 1);
    }
    if (overflow_cnt) {
        tr_full = 1;
    }
    if (destructive) {
        tr_full = 1;
    }
    frames = dce_get_amount_of_frames();
    if (RR_CNT < RING_BACKLOG_BYTES) {
        if (frames >= QUEUED_FRAMES) {
            tr_full = 1;
        }
    } else if (frames >= 1) {
        tr_full = 1;
    }
}

static void rx_flow_control(void)
{
    tx_frame_number = NO_FRAME;
    tr_full = 0;
    if (own_busy && !srej_condition) {
        if (srej_buff_cntr) {
            if (RR_CNT < RING_READY_BYTES) {
                pass_srej_buffer();
            }
        } else if (RR_CNT < RING_READY_BYTES) {
            own_busy = 0;
            if (!recover_with_rej || reject_condition) {
                QUEUE_FRAME(RECEIVE_READY, CTL_RR);
            } else {
#ifdef NONMATCHING
#else
                /* @hack redundant: a jump to the next line */
                goto discard;
            discard:
#endif
                scrap_srej_buff();
                reject_condition = 1;
                QUEUE_FRAME(REJECT, CTL_REJ);
            }
            COMMAND();
            recover_with_rej = 0;
        }
    }
}

/* queue the host's oldest break as a UI frame. none goes while an earlier
 * break or a SABME waits for its answer */
static void break_control(void)
{
    break_option = 0;
    if (!break_or_sabme_sent && (BMR(25) & BMR25_BREAK_PENDING)) {
        /* a break in sequence waits for the peer to acknowledge the data
         * before it */
        /* @hack type: OISregisters read through a byte pointer to the array,
         * where OIS(1) would do */
        if ((((Uint8 *)&OISregisters)[1] & OIS1_BREAK_TYPE) ==
                OIS1_BREAK_IN_SEQUENCE &&
            TM_CNT) {
            return;
        }
        break_or_sabme_sent = 1;
        QUEUE_FRAME(UNNUMBERED_INFO, CTL_UI);
        COMMAND();
        nsb_tx = vsb;
        if (!(OIS(1) & OIS1_BREAK_IN_SEQUENCE)) {
            break_option = BREAK_EXPEDITED;
        }
        /* @hack type: the break field cast to Uint16, where the field would
         * do */
        if (!(Uint16)(OIS(1) & OIS1_BREAK_TYPE)) {
            break_option = BREAK_DESTRUCTIVE | BREAK_EXPEDITED;
        }
        /* @bug logic: a break sent without its length stays in
         * TxBreakLength. with OIS11_BREAK_LENGTH clear, the branch at
         * 0x0cafc9f0 skips TCI_get_dte_brk, so BMR25_BREAK_PENDING stays set,
         * v42 holds the host's data back, and each acknowledgement sends the
         * break again */
        if (OIS(11) & OIS11_BREAK_LENGTH) {
            tx_break_length = TCI_get_dte_brk();
        } else {
            tx_break_length = 0;
        }
        tx_brk = 1;
    }
}

/* any frame LAPM knows stops T403. an information or supervisory frame's
 * N(R) must lie from va to vs. va takes it unless the frame is an SREJ.
 * an N(R) outside restarts the link by SABME, unless SREJ is agreed. so does
 * an SREJ of vs, which asks for a frame not yet sent. vs_ahead and nr_ahead
 * are vs and N(R) counted on past LAPM_SEQUENCE_NUMBERS where they lie below
 * va */
static void fn_manager(void)
{
    Uint8 first_unsent;
    Uint8 vs_ahead;
    Uint8 nr_ahead;

    if (fn > NO_FRAME && fn < UNKNOWN_FRAME) {
        timer_t403_run = 0;
        if (fn < SET_BALANCED) {
            vs_ahead = vs;
            first_unsent = vs_ahead;
            if (vs_ahead < va) {
                vs_ahead += LAPM_SEQUENCE_NUMBERS;
            }
            if (nr_rx >= va) {
                nr_ahead = nr_rx;
            } else {
                nr_ahead = nr_rx + LAPM_SEQUENCE_NUMBERS;
            }
            if (nr_ahead <= vs_ahead && nr_ahead >= va) {
                if (fn != SELECTIVE_REJECT) {
                    va = nr_rx;
                } else if (nr_rx == first_unsent) {
                    establishment_procedure();
                    BMR(23) |= BMR23_LINK_ERROR;
                    remote_err();
                }
            } else if (!s_rej) {
                establishment_procedure();
                BMR(23) |= BMR23_LINK_ERROR;
                remote_err();
            }
        }
    }
}

static void manager(void)
{
    Uint8 vs_ahead;
#ifdef NONMATCHING
#else
    /* @hack redundant: stores to tm_cnt_temp and the frame to send that never
     * run */
    if (0) {
        tm_cnt_temp = 0;
        QUEUE_FRAME(RECEIVE_READY, CTL_RR);
    }
#endif
    /* the frames the peer has acknowledged since the last pass leave the
     * transmit ring */
    while (previous_va != va) {
        tm_cnt_temp -= tm_frame_length[previous_va];
        TM_CNT -= tm_frame_length[previous_va];
        tm_cnt_shadow -= tm_frame_length[previous_va];
        TM_READ = TM_BUFFER + tm_frame_address[previous_va];
#ifdef NONMATCHING
        /* this side wraps the frame's end as an offset in the ring, at tm_lnt.
         * the matching side forms a pointer past the ring's top before it wraps
         * it */
        {
            Uint16 offset =
                tm_frame_address[previous_va] + tm_frame_length[previous_va];
            TM_READ = TM_BUFFER + (offset % tm_lnt);
        }
#else
        TM_READ += tm_frame_length[previous_va];
        if (TM_READ > TM_TOPP) {
            /* @hack spelling: TM_READ stepped back inside the expression that
             * sets it, where TM_READ - 1 - TM_TOPP + TM_BUFFER would do */
            /* NOLINTNEXTLINE(clang-diagnostic-unsequenced) */
            TM_READ = (TM_READ -= 1) - TM_TOPP + TM_BUFFER;
        }
#endif
        tm_frame_length[previous_va] = 0;
        previous_va = NEXT_SEQUENCE(previous_va);
    }
    valid_tx = 0;
    vs_ahead = vs;
    if (vs_ahead < va) {
        vs_ahead += LAPM_SEQUENCE_NUMBERS;
    }
    if (vs_ahead - va < win_k_tx_used) {
        valid_tx = 1;
    }
    if (xid_sent || sabme_sent || disc_sent) {
        if (TIM_10MS - timer_t401_offset > timer_t401_max) {
            timer_t401_offset = TIM_10MS;
            if (!timer_recovery_condition) {
                timer_recovery_condition = 1;
                retransmission_count = 0;
            } else {
                retransmission_count++;
                StatisticInfo.timeouts++;
            }
            if (n400 && retransmission_count >= n400) {
                BMR(27) |= BMR27_LINK_DOWN;
                STATUS_BITS |= STATUS_LAPM_FAILED;
                StatisticInfo.link_end = LINK_END_NO_ANSWER;
            } else {
                if (xid_sent) {
                    QUEUE_FRAME(EXCHANGE_ID, CTL_XID);
                    POLL_COMMAND();
                }
                if (sabme_sent) {
                    QUEUE_FRAME(SET_BALANCED, CTL_SABME | POLL_FINAL);
                    POLL_COMMAND();
                }
                if (disc_sent) {
                    QUEUE_FRAME(DISCONNECT, CTL_DISC);
                    POLL_COMMAND();
                }
            }
        }
    } else {
        if (timer_t401_run) {
            timer_t403_run = 0;
            if (TIM_10MS - timer_t401_offset > timer_t401_max) {
                if (!timer_recovery_condition) {
                    srej_to_send = 0;
                    timer_recovery_condition = 1;
                    retransmission_count = 0;
                } else {
                    retransmission_count++;
                    StatisticInfo.timeouts++;
                }
                if (n400 && retransmission_count >= n400) {
                    establishment_procedure();
                    BMR(23) |= BMR23_LINK_ERROR;
                    remote_err();
                } else {
                    timer_t401_offset = TIM_10MS;
                    if (!own_busy) {
                        goto ready;
                    }
                    goto busy;
#ifdef NONMATCHING
#else
                    /* @hack redundant: a POLL_COMMAND that never runs */
                    POLL_COMMAND();
#endif
                }
            }
        } else if (xid_rec) {
            if (!timer_t403_run) {
                timer_t403_run = 1;
                timer_t403_offset = TIM_10MS;
            } else if (TIM_10MS - timer_t403_offset > timer_t401_max) {
                timer_t403_run = 0;
                srej_to_send = 0;
                retransmission_count = 0;
                timer_recovery_condition = 1;
                timer_t401_offset = TIM_10MS;
                timer_t401_run = 1;
                if (!own_busy) {
                ready:
                    QUEUE_FRAME(RECEIVE_READY, CTL_RR);
                } else {
                busy:
#ifdef NONMATCHING
#else
                    /* @hack redundant: a jump to the next line */
                    goto busy_step;
                busy_step:
#endif
                    QUEUE_FRAME(RECEIVE_NOT_READY, CTL_RNR);
                }
                POLL_COMMAND();
            }
        }
        if (STATUS_BITS & STATUS_HANGUP) {
            QUEUE_FRAME(DISCONNECT, CTL_DISC);
            POLL_COMMAND();
            timer_recovery_condition = 0;
            timer_t401_offset = TIM_10MS;
        }
    }
#ifdef NONMATCHING
#else
    /* @hack redundant: stores to the timers that never run */
    if (0) {
        timer_recovery_condition = 1;
        timer_recovery_condition = 1;
        timer_recovery_condition = 1;
        timer_recovery_condition = 1;
        TIM_10MS = 1;
        TIM_10MS = 1;
        timer_t401_offset = 1;
        timer_t401_offset = 1;
    }
#endif
}

static void establishment_procedure(void)
{
    if (!break_or_sabme_sent) {
        QUEUE_FRAME(SET_BALANCED, CTL_SABME | POLL_FINAL);
        POLL_COMMAND();
        timer_recovery_condition = 0;
        timer_t401_offset = TIM_10MS;
        destructive = 1;
        break_or_sabme_sent = 1;
    }
}

static void setup_for_originate(void)
{
    Uint16 i;

    enter_disconnect = 0;
    vs = 0;
    va = 0;
    vr = 0;
    previous_va = 0;
    remote_busy = 0;
    own_busy = 0;
    tm_frame_address[0] = TM_READ - TM_BUFFER;
    /* @hack spelling: i <= LAPM_SEQUENCE_NUMBERS - 1, where
     * i < LAPM_SEQUENCE_NUMBERS would do */
    for (i = 0; i <= LAPM_SEQUENCE_NUMBERS - 1; i++) {
        tm_frame_length[i] = 0;
    }
    QUEUE_FRAME(EXCHANGE_ID, CTL_XID);
    POLL_COMMAND();
    xid_sent = 1;
    timer_recovery_condition = 0;
    timer_t401_offset = TIM_10MS;
}

static void execute_command(void)
{
    switch (fn) {
    case NO_FRAME:
        break;
    case INFORMATION:
        info();
        break;
    case RECEIVE_READY:
        rr();
        break;
    case RECEIVE_NOT_READY:
        rnr();
        break;
    case REJECT:
        rej();
        break;
    case SELECTIVE_REJECT:
        srej();
        break;
    case SET_BALANCED:
        sabme();
        break;
    case DISCONNECTED:
        dm();
        break;
    case UNNUMBERED_INFO:
        ui();
        break;
    case DISCONNECT:
        disc();
        break;
    case ACKNOWLEDGE:
        ua();
        break;
    case FRAME_REJECT:
        frmr();
        break;
    case EXCHANGE_ID:
        xid();
        break;
    case TEST_FRAME:
        test();
        break;
    default:
        unid();
        break;
    }
}

/* the lapm_frame of the frame at dce_in_pntr. until an XID has come, every
 * other frame is an UNKNOWN_FRAME. cr_rx is set to 1 for a command, and pf_rx
 * to the P/F bit. an information frame sets ns_rx. it and a supervisory frame
 * set nr_rx, and same_nr_as_previous where nr_rx stays as it was.
 * rm_read_temp is left past the control field. rm_frame_length counts an
 * information frame's information, and any other frame from its first
 * control octet on */
static Uint8 decode_command(void)
{
    Uint8 control;
    Uint8 sequence;

    pf_tx = 0;
    if (dce_in_cntr) {
        ns_rx = 0;
        rm_read_temp = dce_in_pntr;
        rm_frame_length = dce_in_cntr;
#ifdef NONMATCHING
        /* this side drops a frame too short for its address and a control
         * octet: 2 octets with a one-octet address, 3 with a two-octet one */
        if (rm_frame_length < ((*rm_read_temp & 1) ? 2 : 3)) {
            return UNKNOWN_FRAME;
        }
#endif
        /* @hack spelling: ADDRESS_CR's bit tested > 1, where != 0 would do */
        cr_rx = (*rm_read_temp & ADDRESS_CR) > 1;
        cr_rx ^= ans_org;
        if (*rm_read_temp & ADDRESS_EA) {
            rm_read_temp++;
            rm_frame_length--;
        } else {
            rm_read_temp += LONG_ADDRESS_BYTES;
            rm_frame_length -= LONG_ADDRESS_BYTES;
        }
        control = *rm_read_temp++;
        if (!xid_rec) {
            if (control != CTL_XID && control != (CTL_XID | POLL_FINAL)) {
                return UNKNOWN_FRAME;
            }
        }
#ifdef NONMATCHING
        /* this side drops an information or supervisory frame too short for its
         * second control octet */
        if (rm_frame_length < 2 && (control & 3) != 3) {
            return UNKNOWN_FRAME;
        }
#else
        /* @hack redundant: a jump to the next line */
        goto frame_kind;
    frame_kind:
#endif
        if (IS_INFORMATION(control)) {
            sequence = *rm_read_temp++;
            rm_frame_length -= 2;
            ns_rx = control >> SEQUENCE_SHIFT;
            pf_rx = sequence & SEQUENCE_PF;
            if (nr_rx != (sequence >> SEQUENCE_SHIFT)) {
                same_nr_as_previous = 0;
                nr_rx = sequence >> SEQUENCE_SHIFT;
            } else {
                same_nr_as_previous = 1;
            }
            return INFORMATION;
        } else if (IS_UNNUMBERED(control)) {
            pf_rx = control & POLL_FINAL;
            pf_rx >>= POLL_FINAL_SHIFT;
            control &= ~POLL_FINAL;
            switch (control) {
            case CTL_SABME:
                return SET_BALANCED;
            case CTL_DM:
                return DISCONNECTED;
            case CTL_UI:
                return UNNUMBERED_INFO;
            case CTL_DISC:
                return DISCONNECT;
            case CTL_UA:
                return ACKNOWLEDGE;
            case CTL_FRMR:
                return FRAME_REJECT;
            case CTL_XID:
                return EXCHANGE_ID;
            case CTL_TEST:
                return TEST_FRAME;
            }
        } else {
            sequence = *rm_read_temp++;
            pf_rx = sequence & SEQUENCE_PF;
            if (nr_rx != (sequence >> SEQUENCE_SHIFT)) {
                nr_rx = sequence >> SEQUENCE_SHIFT;
                same_nr_as_previous = 0;
            } else {
                same_nr_as_previous = 1;
            }
            if (control == CTL_SREJ) {
                return SELECTIVE_REJECT;
            } else if (control == CTL_RNR) {
                return RECEIVE_NOT_READY;
            } else if (control == CTL_REJ) {
                return REJECT;
            } else if (control == CTL_RR) {
                return RECEIVE_READY;
            }
        }
        return UNKNOWN_FRAME;
    }
    return NO_FRAME;
}

static void info(void)
{
    Uint8 ns_ahead;
    Uint16 i;

    if (destructive) {
        recover_with_rej = 0;
        return;
    }
    if (!own_busy) {
        if (ns_rx == vr) {
#ifdef NONMATCHING
            /* this side goes busy, and queues an RNR, where the frame holds
             * more than the receive ring has room for */
            if (rm_frame_length > LAPM_RING_BYTES - RR_CNT) {
                own_busy = 1;
                QUEUE_FRAME(RECEIVE_NOT_READY, CTL_RNR);
                cr_tx = 0;
                pf_tx = pf_rx;
                return;
            }
#endif
            StatisticInfo.frames_received++;
            vr = NEXT_SEQUENCE(vr);
            reject_condition = 0;
            pwork1.data = RR_WRITE;
            for (i = 1; i <= rm_frame_length; i++) {
                *pwork1.data++ = *rm_read_temp++;
                RR_CNT++;
                if (pwork1.data > RR_TOPP) {
                    pwork1.data = RR_BUFFER;
                }
            }
#ifdef NONMATCHING
#else
            /* @hack redundant: a store to RR_CNT that never runs */
            if (0) {
                RR_CNT = i + StatisticInfo.frames_received;
            }
#endif
            RR_WRITE = pwork1.data;
            if (RR_CNT < RING_BUSY_BYTES) {
                if (!srej_condition) {
                    if (pf_rx) {
                        QUEUE_FRAME(RECEIVE_READY, CTL_RR);
                        FINAL_RESPONSE();
                    } else if (tm_cnt_temp == TM_CNT || remote_busy) {
                        QUEUE_FRAME(RECEIVE_READY, CTL_RR);
                        RESPONSE();
                    }
                } else {
                    srej_condition = 0;
                    pass_srej_buffer();
                }
            } else {
                srej_condition = 0;
                own_busy = 1;
                if (pf_rx) {
                    QUEUE_FRAME(RECEIVE_NOT_READY, CTL_RNR);
                    FINAL_RESPONSE();
                } else {
                    QUEUE_FRAME(RECEIVE_NOT_READY, CTL_RNR);
                    RESPONSE();
                }
            }
        } else {
            ns_ahead = ns_rx;
            if (ns_ahead < vr) {
                ns_ahead += LAPM_SEQUENCE_NUMBERS;
            }
            if (ns_ahead - vr >= win_k_rx) {
                return;
            }
            remote_err();
            BMR(23) |= BMR23_LINK_ERROR;
            if (!s_rej) {
                rm_frame_length = 0;
                if (pf_rx) {
                    if (!reject_condition) {
                        reject_condition = 1;
                        QUEUE_FRAME(REJECT, CTL_REJ);
                        FINAL_RESPONSE();
                    } else {
                        QUEUE_FRAME(RECEIVE_READY, CTL_RR);
                        FINAL_RESPONSE();
                    }
                } else if (!reject_condition) {
                    StatisticInfo.rejects_sent++;
                    reject_condition = 1;
                    QUEUE_FRAME(REJECT, CTL_REJ);
                    COMMAND();
                }
            } else if (!reject_condition) {
                if (!srej_buff_cntr) {
                    StatisticInfo.rejects_sent++;
                    srej_condition = 1;
                    QUEUE_FRAME(SELECTIVE_REJECT, CTL_SREJ);
                    COMMAND();
                }
                store_in_srej_buff();
            } else {
                rm_frame_length = 0;
                if (pf_rx) {
                    QUEUE_FRAME(RECEIVE_READY, CTL_RR);
                    FINAL_RESPONSE();
                }
            }
        }
    } else {
        if (!s_rej) {
            recover_with_rej = 1;
        } else {
            recover_with_rej = 0;
            store_in_srej_buff();
        }
        if (pf_rx) {
            QUEUE_FRAME(RECEIVE_NOT_READY, CTL_RNR);
            FINAL_RESPONSE();
        }
    }
    if (!remote_busy && !same_nr_as_previous) {
        timer_t401_run = 0;
        timer_recovery_condition = 0;
        if (va != vs) {
            timer_t401_run = 1;
            timer_t401_offset = TIM_10MS;
        }
    }
}

static void sabme(void)
{
    tx_frame_number = ACKNOWLEDGE;
    tx_code_number = CTL_UA;
    pf_tx = pf_rx;
    cr_tx = 0;
    reset();
    remote_busy = 0;
    BMR(27) |= BMR27_LINK_UP;
}

static void dm(void)
{
    if (sabme_sent && !cr_rx) {
        if (pf_rx) {
            enter_disconnect = 1;
            no_connect_possible = 1;
            BMR(27) |= BMR27_LINK_DOWN;
            sabme_sent = 0;
        }
    }
    if (disc_sent && !cr_rx) {
        if (pf_rx) {
            enter_disconnect = 1;
            BMR(27) |= BMR27_LINK_DOWN;
            disc_sent = 0;
        }
    } else if (!pf_rx) {
        establishment_procedure();
    }
}

static void ui(void)
{
#ifdef NONMATCHING
    /* this side drops a frame with no octet past its control field */
    if (rm_frame_length < 2) {
        return;
    }
#endif
    if ((*rm_read_temp & BREAK_ACK) == BREAK_ACK) {
        break_type = 0;
        brk_sent = 0;
    } else {
        break_type = 1;
    }
    break_sequence_number_rx =
        (*rm_read_temp++ & (1 << BREAK_SEQUENCE_SHIFT)) >> BREAK_SEQUENCE_SHIFT;
    if (break_type == 1) {
#ifdef NONMATCHING
        /* this side drops a break with no options octet before the frame's
         * end */
        if (rm_read_temp >= dce_in_pntr + dce_in_cntr) {
            return;
        }
#endif
        break_handling_option_d = *rm_read_temp & BREAK_DESTRUCTIVE;
        break_handling_option_s = *rm_read_temp++ & BREAK_EXPEDITED;
#ifdef NONMATCHING
        /* this side repairs the bug tagged below: the length octet is read
         * where the frame holds one */
        if (rm_read_temp < dce_in_pntr + dce_in_cntr) {
#else
        /* @bug logic: a break with no length octet takes the octet past the
         * frame for its length. one with a length octet takes the default
         * length instead. the test at 0x0cafd594 compares BREAK_BYTES with
         * rm_frame_length, which counts the control octet too */
        if (rm_frame_length == BREAK_BYTES) {
#endif
            break_length = *rm_read_temp++;
        } else {
            break_length = OIS(OIS_BREAK_LENGTH);
        }
        if (break_sequence_number_rx == vrb) {
            vrb ^= 1;
            if (break_handling_option_d) {
                destructive = 1;
                reset();
                destructive = 0;
            }
            cmd_resp_to_host(ECDC_RESPONSE_DCE_BREAK);
            if (break_handling_option_s) {
                vxd_dte_tx_brk(break_length, 0);
            } else {
                buff_rx_EC_brk(break_length);
            }
        }
        pf_tx = pf_rx;
        QUEUE_FRAME(UNNUMBERED_INFO, CTL_UI);
        cr_tx = 0;
        nrb_tx = vrb;
        tx_brk = 0;
    } else if (break_sequence_number_rx == (vsb ^ 1)) {
        vsb ^= 1;
        rm_read_temp = rm_read_temp + (rm_frame_length - 2);
        break_or_sabme_sent = 0;
        if (destructive) {
            reset();
            destructive = 0;
        }
    }
}

static void disc(void)
{
    pf_tx = pf_rx;
    tx_code_number = CTL_UA;
    tx_frame_number = ACKNOWLEDGE;
    cr_tx = 0;
    enter_disconnect = 1;
    BMR(27) |= BMR27_LINK_DOWN;
    StatisticInfo.link_end = LINK_END_DISCONNECT;
}

static void ua(void)
{
    BMR(27) |= BMR27_LINK_UP;
    if (break_or_sabme_sent) {
        break_or_sabme_sent = 0;
        destructive = 1;
        reset();
        destructive = 0;
    }
    if (disc_sent && !cr_rx) {
        if (pf_rx) {
            enter_disconnect = 1;
            BMR(27) |= BMR27_LINK_DOWN;
            disc_sent = 0;
        }
    }
    if (sabme_sent && !cr_rx) {
        if (pf_rx) {
            sabme_sent = 0;
            reset();
        }
    }
}

static void frmr(void)
{
    tx_code_number = CTL_SABME | POLL_FINAL;
    tx_frame_number = SET_BALANCED;
    cr_tx = 1;
    pf_tx = 0;
}

/* an XID's format identifier, and the identifiers of its groups. the
 * negotiation group comes first, and each other group is optional. the
 * address, the control octet and the format identifier make
 * XID_HEADER_BYTES. a group starts with its identifier and two octets of
 * length, high first */
#define XID_FORMAT 0x82
#define XID_NEGOTIATION 0x80
#define XID_V42BIS_GROUP 0xf0
#define XID_PRIVATE_GROUP 0xff
#define XID_HEADER_BYTES 3
#define XID_GROUP_HEADER_BYTES 3
/* a parameter's octets: its identifier, its length, and bytes of value */
#define XID_PARAMETER_BYTES(bytes) (2 + (bytes))
/* the negotiation group's parameters, and the bytes of value each takes.
 * XID_NEGOTIATION_BYTES is the group's length as this end sends it. its HDLC
 * options are HDLC_OFFER_0 with s_rej, HDLC_OFFER_1 with loopb, and crc. an
 * XID gives a frame size in bits, and n401 holds it in octets, SIZE_SHIFT
 * bits down */
enum xid_parameter {
    XID_HDLC_OPTIONS = 3,
    XID_TRANSMIT_BITS = 5,
    XID_RECEIVE_BITS = 6,
    XID_TRANSMIT_WINDOW = 7,
    XID_RECEIVE_WINDOW = 8
};
#define XID_HDLC_BYTES 3
#define XID_SIZE_BYTES 2
#define XID_WINDOW_BYTES 1
#define XID_NEGOTIATION_BYTES                                                  \
    (XID_PARAMETER_BYTES(XID_HDLC_BYTES) +                                     \
     (2 * XID_PARAMETER_BYTES(XID_SIZE_BYTES)) +                               \
     (2 * XID_PARAMETER_BYTES(XID_WINDOW_BYTES)))
#define HDLC_OFFER_0 0x8a
#define HDLC_OFFER_1 0x89
#define SIZE_SHIFT 3
/* the V.42bis group's parameters, and the bytes of value each takes: the
 * name V42, then P0 to P2 */
enum xid_compression_parameter {
    XID_COMPRESSION_NAME,
    XID_DIRECTIONS,
    XID_DICTIONARY_SIZE,
    XID_STRING_LENGTH
};
#define XID_NAME_BYTES 3
#define XID_DIRECTIONS_BYTES 1
#define XID_DICTIONARY_BYTES 2
#define XID_STRING_BYTES 1
#define XID_V42BIS_BYTES                                                       \
    (XID_PARAMETER_BYTES(XID_NAME_BYTES) +                                     \
     XID_PARAMETER_BYTES(XID_DIRECTIONS_BYTES) +                               \
     XID_PARAMETER_BYTES(XID_DICTIONARY_BYTES) +                               \
     XID_PARAMETER_BYTES(XID_STRING_BYTES))
/* the private group's parameters, and the bytes of value each takes. an MMA
 * from the peer sets remote_is_mma. MIX offers encryption. its value is the
 * name MIX and then the originator's end of key_string, from KEY_CLOCK on */
enum xid_private_parameter { XID_MMA = 1, XID_MIX };
#define XID_MMA_BYTES 3
#define XID_MIX_BYTES 5
#define KEY_CLOCK 6
/* an XID gives P0 from the originator's side. OTHER_SIDE swaps P0_CODER and
 * P0_DECODER for the answerer's */
#define OTHER_SIDE(bits)                                                       \
    ((((bits) << 1) & (P0_CODER | P0_DECODER)) + ((bits) >> 1))

#ifdef NONMATCHING
/* this side checks each parameter's length against the frame, and steps over a
 * parameter by its length, read or not */
#define REQUIRE_XID(condition)                                                 \
    if (!(condition)) {                                                        \
        return;                                                                \
    }
#define XID_OPTIONAL(bytes, test) (end - pwork0 >= (bytes) && (test))
#define XID_SET_NEXT() next = pwork0 + parameter_length
#define XID_ADVANCE() pwork0 = next
#else
/* the image checks no length against the frame. it steps over a parameter
 * only by the octets it reads, none for one it doesn't know */
#define REQUIRE_XID(condition)
#define XID_OPTIONAL(bytes, test) (test)
#define XID_SET_NEXT()
#define XID_ADVANCE()
#endif

/* take an XID: its negotiation group, then any V.42bis or private group. a
 * command gets the XID response, and a response gets a SABME */
static void xid(void)
{
#ifdef NONMATCHING
    /* this side's used and i are unsigned ints, which pass 255 without
     * wrapping. that repairs the bugs tagged below whose loops wrap a Uint8 */
    unsigned int used;
    unsigned int i;
#else
    Uint8 used;
    Uint8 i;
#endif
    Uint8 saw_hdlc;
    Uint8 saw_rx_size;
    Uint8 saw_tx_size;
    Uint8 saw_rx_window;
    Uint8 saw_tx_window;
    Uint8 peer_mode;
    Uint16 high;
    Uint8 *option;
#ifdef NONMATCHING
    /* end is the frame's end, for this side's length checks. a frame too short
     * for an XID's format identifier and group header is dropped */
    Uint8 *end = dce_in_pntr + dce_in_cntr;
    Uint8 *group_end;
    Uint8 *next;
    if (end - rm_read_temp < 4) {
        return;
    }
#endif

    xid_sent = 0;
    xid_rec = 1;
    /* pwork0 skips the format identifier, the group's identifier and the
     * high octet of its length. xid keeps only the low octet */
    pwork0 = rm_read_temp + 3;
    group_length = *pwork0++;
    used = 1;
    saw_hdlc = 0;
    saw_rx_size = 0;
    saw_tx_size = 0;
    saw_rx_window = 0;
    saw_tx_window = 0;
#ifdef NONMATCHING
    /* this side repairs the bug tagged below, with used's type and REQUIRE_XID:
     * it drops a negotiation group that runs past the frame */
    if (end - pwork0 < group_length) {
        return;
    }
    group_end = pwork0 + group_length;
#else
    /* @hack redundant: stores to tm_lnt and RR_CNT that never run */
    if (0) {
        tm_lnt = 52;
        RR_CNT = 52;
    }
    /* @hack redundant: stores to saw_rx_size, saw_tx_size, saw_rx_window and
     * saw_tx_window that never run */
    if (0) {
        saw_rx_size = 99;
        saw_tx_size = 98;
        saw_rx_window = 97;
        saw_tx_window = 96;
    }
    /* @hack redundant: second stores to saw_rx_size and saw_tx_size that
     * never run */
    if (0) {
        saw_rx_size = 95;
        saw_tx_size = 94;
    }
#endif
    /* @bug memory: the loop at 0x0cafdae8 reads on past the frame without
     * end for a group_length of 255. used, a Uint8, can't pass 255 */
    while (used <= group_length) {
        REQUIRE_XID(group_end - pwork0 >= 2)
        parameter_identifier = *pwork0++;
        parameter_length = *pwork0++;
        used += parameter_length + 2;
        REQUIRE_XID(group_end - pwork0 >= parameter_length)
        XID_SET_NEXT();
        if (parameter_identifier == XID_HDLC_OPTIONS) {
            saw_hdlc = 1;
            i = 0;
            option = hdlc;
            REQUIRE_XID(parameter_length <= sizeof(hdlc))
            /* @bug memory: the copy writes past hdlc's 4 octets for a longer
             * HDLC options parameter. the store at 0x0cafd8e4 takes
             * parameter_length octets without a bound */
            while (i < parameter_length) {
                *option = *pwork0++;
                i++;
                option++;
            }
            /* @hack type: hdlc read through a byte pointer to the array, here
             * and in the two reads below, where hdlc[0] to hdlc[2] would do */
            s_rej = ((Uint8 *)&hdlc)[0] & HDLC_SREJ;
            if (s_rej) {
                s_rej = s_rej_init;
            }
            loopb = ((Uint8 *)&hdlc)[1] & HDLC_TEST;
            if (loopb) {
                loopb = loopb_init;
            }
            /* @hack spelling: the octet && 1, where != 0 would do */
            crc = ((Uint8 *)&hdlc)[2] && 1;
            if (crc) {
                crc = crc_init;
            }
#ifdef NONMATCHING
#else
            /* @hack redundant: a store to crc_init that never runs */
            if (0) {
                crc_init = crc_init ? 1 : 0;
            }
#endif
        } else if (parameter_identifier == XID_TRANSMIT_BITS) {
            REQUIRE_XID(parameter_length == XID_SIZE_BYTES)
            saw_rx_size = 1;
            high = *pwork0++;
            high <<= 8;
            n401_rx = *pwork0++;
            n401_rx |= high;
            n401_rx >>= SIZE_SHIFT;
            if (!n401_rx) {
                n401_rx = DEFAULT_FRAME_BYTES;
            }
            if (n401_rx_init < n401_rx) {
                n401_rx = n401_rx_init;
            }
        } else if (parameter_identifier == XID_RECEIVE_BITS) {
            REQUIRE_XID(parameter_length == XID_SIZE_BYTES)
            saw_tx_size = 1;
            high = *pwork0++;
            high <<= 8;
            n401_tx = *pwork0++;
            n401_tx |= high;
            n401_tx >>= SIZE_SHIFT;
            if (!n401_tx) {
                n401_tx = DEFAULT_FRAME_BYTES;
            }
            if (n401_tx_init > n401_tx) {
                tx_max_packet_size = n401_tx + INFORMATION_HEADER_BYTES;
            } else {
                tx_max_packet_size = n401_tx_init + INFORMATION_HEADER_BYTES;
                if (n401_tx_init < DEFAULT_FRAME_BYTES) {
                    if (n401_tx > DEFAULT_FRAME_BYTES) {
                        n401_tx = DEFAULT_FRAME_BYTES;
                    }
                } else {
                    n401_tx = n401_tx_init;
                }
            }
        } else if (parameter_identifier == XID_TRANSMIT_WINDOW) {
            REQUIRE_XID(parameter_length == XID_WINDOW_BYTES)
            saw_rx_window = 1;
            win_k_rx = *pwork0++;
            if (win_k_rx == 0) {
                win_k_rx = DEFAULT_WINDOW;
            }
            if (win_k_rx_init < win_k_rx) {
                win_k_rx = win_k_rx_init;
            }
        } else if (parameter_identifier == XID_RECEIVE_WINDOW) {
            REQUIRE_XID(parameter_length == XID_WINDOW_BYTES)
            saw_tx_window = 1;
            win_k_tx = *pwork0++;
            if (!win_k_tx) {
                win_k_tx = DEFAULT_WINDOW;
            }
            if (win_k_tx_init > win_k_tx) {
                win_k_tx_used = win_k_tx;
            } else {
                win_k_tx_used = win_k_tx_init;
                if (win_k_tx_init < DEFAULT_WINDOW) {
                    if (win_k_tx > DEFAULT_WINDOW) {
                        win_k_tx = DEFAULT_WINDOW;
                    }
                } else {
                    win_k_tx = win_k_tx_init;
                }
            }
        }
        XID_ADVANCE();
    }
    if (!saw_hdlc) {
        s_rej = 0;
        crc = 0;
        loopb = 0;
    }
    if (!saw_rx_size) {
        n401_rx = DEFAULT_FRAME_BYTES;
    }
    if (!saw_tx_size) {
        n401_tx = DEFAULT_FRAME_BYTES;
        if (n401_tx_init > DEFAULT_FRAME_BYTES) {
            tx_max_packet_size = n401_tx + INFORMATION_HEADER_BYTES;
        } else {
            tx_max_packet_size = n401_tx_init + INFORMATION_HEADER_BYTES;
        }
    }
    if (!saw_rx_window) {
        win_k_rx = DEFAULT_WINDOW;
    }
    if (!saw_tx_window) {
        win_k_tx = DEFAULT_WINDOW;
        if (win_k_tx_init > DEFAULT_WINDOW) {
            win_k_tx_used = win_k_tx;
        } else {
            win_k_tx_used = win_k_tx_init;
#ifdef NONMATCHING
#else
            /* @hack redundant: a store to TM_CNT that never runs */
            if (0) {
                TM_CNT = 128;
            }
#endif
        }
    }
    if (n401_tx > n401_rx) {
        test_packet_size = n401_rx;
    } else {
        test_packet_size = n401_tx;
    }
#ifdef NONMATCHING
    /* this side takes the V.42bis group only where the frame holds its
     * identifier and length */
    if (end - pwork0 >= 3 && *pwork0 == XID_V42BIS_GROUP &&
        rm_frame_length > 35) {
#else
    if (*pwork0 == XID_V42BIS_GROUP && rm_frame_length > 35) {
        /* @hack redundant: a store to cr_rx that never runs */
        if (0) {
            cr_rx = 0;
        }
#endif
        group_identifier = *pwork0++;
        /* as for the negotiation group, xid keeps only the low octet of the
         * group's length */
        group_length = *pwork0++;
        group_length = *pwork0++;
        used = 1;
#ifdef NONMATCHING
        /* this side repairs the bug tagged below, with used's type and
         * REQUIRE_XID: it drops a V.42bis group that runs past the frame */
        if (end - pwork0 < group_length) {
            return;
        }
        group_end = pwork0 + group_length;
#endif
        /* @bug memory: the loop at 0x0cafdce0 reads on past the group where a
         * parameter takes used past 255. used, a Uint8, then wraps to a small
         * count */
        while (used < group_length) {
            REQUIRE_XID(group_end - pwork0 >= 2)
            parameter_identifier = *pwork0++;
            parameter_length = *pwork0++;
            used += parameter_length + 2;
            REQUIRE_XID(group_end - pwork0 >= parameter_length)
            XID_SET_NEXT();
            if (parameter_identifier == XID_COMPRESSION_NAME) {
                /* @bug arithmetic: the loop at 0x0cafdc2c never ends for a
                 * name parameter_length of 255. i, a Uint8, wraps before it
                 * can pass 255 */
                for (i = 1; i <= parameter_length; i++) {
                    pwork0++;
                }
            } else if (parameter_identifier == XID_DIRECTIONS) {
                REQUIRE_XID(parameter_length == XID_DIRECTIONS_BYTES)
                peer_mode = *pwork0++;
                if (ans_org) {
                    p0 = peer_mode & p0_init;
                } else {
                    p0 = OTHER_SIDE(peer_mode) & p0_init;
                }
            } else if (parameter_identifier == XID_DICTIONARY_SIZE) {
                REQUIRE_XID(parameter_length == XID_DICTIONARY_BYTES)
                p1 = *pwork0++ << 8;
                p1 = p1 | *pwork0++;
                if (p1_init < p1) {
                    p1 = p1_init;
                }
            } else if (parameter_identifier == XID_STRING_LENGTH) {
                REQUIRE_XID(parameter_length == XID_STRING_BYTES)
                p2 = *pwork0++;
                if (p2_init < p2) {
                    p2 = p2_init;
                }
            }
            XID_ADVANCE();
        }
        if ((OIS(2) & OIS2_V42BIS) && p0) {
            bis_accept = 1;
            if (p0 > 0 && p0 < (P0_CODER | P0_DECODER)) {
                BMR(29) |= BMR29_ONE_WAY;
                if (ans_org) {
                    if (p0 & P0_CODER) {
                        BMR(29) |= BMR29_FROM_ORIGINATOR;
                    } else {
                        BMR(29) &= ~BMR29_FROM_ORIGINATOR;
                    }
                } else {
                    if (p0 & P0_DECODER) {
                        BMR(29) |= BMR29_FROM_ORIGINATOR;
                    } else {
                        BMR(29) &= ~BMR29_FROM_ORIGINATOR;
                    }
                }
            } else {
                BMR(29) &= ~BMR29_ONE_WAY;
            }
            BMR(27) |= BMR27_V42BIS;
            BMR(29) |= BMR29_COMPRESSION;
            raz(p0, p1, p2);
        }
    }
    remote_is_mma = 0;
    encryption = 0;
    /* @bug logic: a group of any identifier passes for the private group
     * where its low octet of length is nonzero. the identifier's test guards
     * only the high octet's, and the branch at 0x0cafdda2 takes any other
     * group to the low octet's test at 0x0cafddac */
    if (XID_OPTIONAL(XID_GROUP_HEADER_BYTES,
                     (*pwork0 == XID_PRIVATE_GROUP && pwork0[1]) ||
                         pwork0[2])) {
        pwork0 += XID_GROUP_HEADER_BYTES;
        if (XID_OPTIONAL(XID_PARAMETER_BYTES(XID_MMA_BYTES),
                         pwork0[0] == XID_MMA && pwork0[1] == XID_MMA_BYTES &&
                             pwork0[2] == 'M' && pwork0[3] == 'M' &&
                             pwork0[4] == 'A')) {
            remote_is_mma = 1;
            pwork0 += XID_PARAMETER_BYTES(XID_MMA_BYTES);
        }
        if (XID_OPTIONAL(XID_PARAMETER_BYTES(XID_MIX_BYTES),
                         pwork0[0] == XID_MIX && pwork0[1] == XID_MIX_BYTES &&
                             pwork0[2] == 'M' && pwork0[3] == 'I' &&
                             pwork0[4] == 'X')) {
            if (OIS(11) & OIS11_ENCRYPTION) {
                encryption = 1;
            }
            if (!ans_org) {
                key_string[KEY_CLOCK] = pwork0[5];
                key_string[KEY_CLOCK + 1] = pwork0[6];
            }
            pwork0 += XID_PARAMETER_BYTES(XID_MIX_BYTES);
        }
    }
    /* @bug arithmetic: the loop at 0x0cafde84 never ends for an
     * rm_frame_length of 255 or more. i, a Uint8, wraps before it can pass
     * it */
    /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
    for (i = 2; i <= rm_frame_length; i++) {
        rm_read_temp++;
    }
    if (cr_rx) {
        QUEUE_FRAME(EXCHANGE_ID, CTL_XID);
        FINAL_RESPONSE();
    } else {
        if (crc) {
            changing_crc = 1;
            setup_timer(&crc_change_deadline, CRC_CHANGE_TICKS);
        }
        QUEUE_FRAME(SET_BALANCED, CTL_SABME | POLL_FINAL);
        POLL_COMMAND();
        timer_recovery_condition = 0;
        timer_t401_offset = TIM_10MS;
        break_or_sabme_sent = 1;
    }
}

static void test(void)
{
    Uint8 expected;
    Uint16 i;

    if (loopb) {
        if (test_sent) {
            test_sent = 0;
            expected = 1;
            pwork0 = rm_read_temp;
            i = 1;
            while (i++ < rm_frame_length && expected++ == *pwork0++) {
            }
            i--;
            /* @hack type: i - 1 cast to Uint16, where its int would do for an
             * i of 1 or more */
            if ((Uint16)(i - 1) == test_packet_size &&
                --expected == *--pwork0) {
                test_good();
            } else {
                test_bad(TEST_ANSWER_DIFFERS);
            }
        } else if (cr_rx == 1) {
            QUEUE_FRAME(TEST_FRAME, CTL_TEST);
        }
    }
}

static void unid(void)
{
}

#define ANSWER_POLL()                                                          \
    do {                                                                       \
        scrap_srej_buff();                                                     \
        if (!own_busy) {                                                       \
            QUEUE_FRAME(RECEIVE_READY, CTL_RR);                                \
            FINAL_RESPONSE();                                                  \
        } else {                                                               \
            QUEUE_FRAME(RECEIVE_NOT_READY, CTL_RNR);                           \
            FINAL_RESPONSE();                                                  \
        }                                                                      \
    } while (0)

static void rr(void)
{
    remote_busy = 0;
    if (cr_rx && pf_rx) {
        ANSWER_POLL();
    } else if (!cr_rx && pf_rx && timer_recovery_condition) {
        vs = nr_rx;
        timer_recovery_condition = 0;
    }
    if (!timer_recovery_condition && !same_nr_as_previous) {
        timer_t401_run = 0;
        if (va != vs) {
            timer_t401_run = 1;
            timer_t401_offset = TIM_10MS;
        }
    }
}

static void rej(void)
{
    dce_del_info_frms();
    StatisticInfo.rejects_received++;
    if (!remote_busy) {
        BMR(23) |= BMR23_LINK_ERROR;
        local_err();
    }
    remote_busy = 0;
    if (!timer_recovery_condition) {
        vs = nr_rx;
        va = nr_rx;
        timer_t401_run = 0;
        if (cr_rx && pf_rx) {
            ANSWER_POLL();
        }
    } else if (!cr_rx && pf_rx) {
        vs = nr_rx;
        va = nr_rx;
        timer_recovery_condition = 0;
        timer_t401_run = 0;
    } else {
        va = nr_rx;
        if (cr_rx && pf_rx) {
            ANSWER_POLL();
        }
    }
}

static void rnr(void)
{
    if (brk_sent != 1) {
        remote_busy = 1;
    }
    if (cr_rx && pf_rx) {
        ANSWER_POLL();
    } else if (!cr_rx && pf_rx && timer_recovery_condition) {
        vs = nr_rx;
        timer_recovery_condition = 0;
    }
    if (!timer_recovery_condition) {
        timer_t401_run = 1;
        timer_t401_offset = TIM_10MS;
    }
}

static void srej(void)
{
    StatisticInfo.rejects_received++;
    if (!pf_rx && !timer_recovery_condition) {
        remote_busy = 0;
        vs_srej = nr_rx;
        srej_to_send = 1;
        timer_t401_offset = TIM_10MS;
        timer_t401_run = 1;
    }
}

static void transmit_frame(void)
{
    tx_address = ADDRESS(cr_tx);
    switch (tx_frame_number) {
    case NO_FRAME:
        break;
    case INFORMATION:
        pf_tx = 0;
        tx_information();
        break;
    case RECEIVE_READY:
    case RECEIVE_NOT_READY:
    case REJECT:
    case SELECTIVE_REJECT:
        tx_supervisory();
        break;
    case SET_BALANCED:
        sabme_sent = 1;
        tx_unnumbered();
        break;
    case DISCONNECTED:
        tx_unnumbered();
        break;
    case UNNUMBERED_INFO:
        tx_ui();
        break;
    case DISCONNECT:
        disc_sent = 1;
        tx_unnumbered();
        break;
    case ACKNOWLEDGE:
    case FRAME_REJECT:
        tx_unnumbered();
        break;
    case EXCHANGE_ID:
        tx_xid();
        if (xid_rec && crc) {
            changing_crc = 1;
        }
        break;
    case TEST_FRAME:
        tx_test();
    }
    tx_frame_number = NO_FRAME;
}

#define START_T401()                                                           \
    do {                                                                       \
        if (!timer_t401_run) {                                                 \
            timer_t401_offset = TIM_10MS;                                      \
            timer_t401_run = 1;                                                \
            timer_recovery_condition = 0;                                      \
        }                                                                      \
    } while (0)
#define INFORMATION_HEADER(sequence)                                           \
    do {                                                                       \
        length = INFORMATION_HEADER_BYTES;                                     \
        *pwork0++ = tx_address;                                                \
        *pwork0++ = (sequence) << SEQUENCE_SHIFT;                              \
        *pwork0++ = (vr << SEQUENCE_SHIFT) + pf_tx;                            \
    } while (0)

static void tx_information(void)
{
    Uint16 remaining;
    Uint16 length;
    Uint16 i;

    if (xid_rec && !break_or_sabme_sent && !brk_sent) {
        if (srej_to_send) {
            if (!tr_full) {
                srej_to_send = 0;
                re_transmit_srej();
            }
        } else if (valid_tx) {
            cr_tx = 1;
            tx_address = ADDRESS(1);
            if (!tr_full && dce_set_tx_data(&pwork0) &&
                !timer_recovery_condition && !remote_busy) {
                if (!tm_frame_length[vs]) {
                    remaining = TM_CNT - tm_cnt_temp;
                    if (remaining >= tx_max_packet_size ||
                        (remaining &&
                         !vxd_dte_get_rx_data(&pwork1.data, &pwork1.length) &&
                         !dce_get_amount_of_frames())) {
                        START_T401();
                        tm_frame_address[vs] = tm_read_temp - TM_BUFFER;
                        INFORMATION_HEADER(vs);
                        while (remaining-- && length < tx_max_packet_size) {
                            *pwork0++ = *tm_read_temp++;
                            if (tm_read_temp > TM_TOPP) {
                                tm_read_temp = TM_BUFFER;
                            }
                            length++;
                        }
                        tm_cnt_temp += length - INFORMATION_HEADER_BYTES;
                        tm_frame_length[vs] = length - INFORMATION_HEADER_BYTES;
                        vs = NEXT_SEQUENCE(vs);
                        dce_tx_data_done(length, 1);
                        StatisticInfo.frames_sent++;
                    }
                } else {
                    START_T401();
                    INFORMATION_HEADER(vs);
                    tm_read_temp = TM_BUFFER + tm_frame_address[vs];
                    for (i = 1; i <= tm_frame_length[vs]; i++) {
                        *pwork0++ = *tm_read_temp++;
                        length++;
                        if (tm_read_temp > TM_TOPP) {
                            tm_read_temp = TM_BUFFER;
                        }
                    }
                    vs = NEXT_SEQUENCE(vs);
                    dce_tx_data_done(length, 1);
                    StatisticInfo.frames_resent++;
                }
            }
        }
    }
    valid_tx = 0;
}

#define SAVE_OVERFLOW_BYTE(value)                                              \
    do {                                                                       \
        overflow_buffer[overflow_write++] = (value);                           \
        if (overflow_write >= LAPM_OVERFLOW_BYTES) {                           \
            overflow_write = 0;                                                \
        }                                                                      \
    } while (0)

static void tx_supervisory(void)
{
    if (!destructive) {
        if (dce_set_tx_data(&pwork0) && !overflow_cnt) {
            *pwork0++ = tx_address;
            *pwork0++ = tx_code_number;
            *pwork0++ = (vr << SEQUENCE_SHIFT) + pf_tx;
            dce_tx_data_done(SUPERVISORY_BYTES, 1);
        } else if (overflow_cnt <= LAPM_OVERFLOW_BYTES - OVERFLOW_LENGTH_BYTES -
                                       SUPERVISORY_BYTES) {
            overflow_cnt += OVERFLOW_LENGTH_BYTES + SUPERVISORY_BYTES;
            SAVE_OVERFLOW_BYTE(SUPERVISORY_BYTES);
            SAVE_OVERFLOW_BYTE(tx_address);
            SAVE_OVERFLOW_BYTE(tx_code_number);
            SAVE_OVERFLOW_BYTE((vr << SEQUENCE_SHIFT) + pf_tx);
        }
    }
}

#define PUT_XID(value) (*pwork0++ = (value))
#define XID_PARAMETER(id, bytes)                                               \
    do {                                                                       \
        PUT_XID(id);                                                           \
        PUT_XID(bytes);                                                        \
    } while (0)

static void tx_xid(void)
{
    Uint16 length;
    if (dce_set_tx_data(&pwork0) && !overflow_cnt) {
        PUT_XID(tx_address);
#ifdef NONMATCHING
#else
        /* @hack redundant: stores to xid_sent and xid_rec that never run */
        if (0) {
            xid_sent = 20;
            xid_rec = 20;
        }
#endif
        PUT_XID(CTL_XID);
        PUT_XID(XID_FORMAT);
        PUT_XID(XID_NEGOTIATION);
        PUT_XID(0);
        PUT_XID(XID_NEGOTIATION_BYTES);
        XID_PARAMETER(XID_HDLC_OPTIONS, XID_HDLC_BYTES);
        PUT_XID(s_rej | HDLC_OFFER_0);
        PUT_XID(loopb | HDLC_OFFER_1);
        PUT_XID(crc);
        XID_PARAMETER(XID_TRANSMIT_BITS, XID_SIZE_BYTES);
        /* @hack spelling: each frame size's octets put through length, where
         * PUT_XID of each would do */
        length = n401_tx >> (8 - SIZE_SHIFT);
        PUT_XID(length);
        length = n401_tx * 8;
        PUT_XID(length);
        XID_PARAMETER(XID_RECEIVE_BITS, XID_SIZE_BYTES);
        length = n401_rx;
        length >>= 8 - SIZE_SHIFT;
        PUT_XID(length);
        length = n401_rx * 8;
        PUT_XID(length);
        XID_PARAMETER(XID_TRANSMIT_WINDOW, XID_WINDOW_BYTES);
        PUT_XID(win_k_tx);
        XID_PARAMETER(XID_RECEIVE_WINDOW, XID_WINDOW_BYTES);
        PUT_XID(win_k_rx);
        length =
            XID_HEADER_BYTES + XID_GROUP_HEADER_BYTES + XID_NEGOTIATION_BYTES;
        if ((OIS(2) & OIS2_V42BIS) && (cr_tx || bis_accept)) {
            PUT_XID(XID_V42BIS_GROUP);
            PUT_XID(0);
            PUT_XID(XID_V42BIS_BYTES);
            XID_PARAMETER(XID_COMPRESSION_NAME, XID_NAME_BYTES);
            PUT_XID('V');
            PUT_XID('4');
            PUT_XID('2');
            XID_PARAMETER(XID_DIRECTIONS, XID_DIRECTIONS_BYTES);
            if (ans_org) {
                PUT_XID(p0);
            } else {
                PUT_XID(OTHER_SIDE(p0));
            }
            XID_PARAMETER(XID_DICTIONARY_SIZE, XID_DICTIONARY_BYTES);
            PUT_XID(p1 >> 8);
            PUT_XID(p1);
            XID_PARAMETER(XID_STRING_LENGTH, XID_STRING_BYTES);
            PUT_XID(p2);
            length = XID_HEADER_BYTES + (2 * XID_GROUP_HEADER_BYTES) +
                     XID_NEGOTIATION_BYTES + XID_V42BIS_BYTES;
        }
        if (cr_tx || remote_is_mma ||
            ((OIS(11) & OIS11_ENCRYPTION) && encryption)) {
            /* pwork1.data points at the group's two octets of length. each
             * parameter after adds its octets to the low one */
            PUT_XID(XID_PRIVATE_GROUP);
            pwork1.data = pwork0;
            PUT_XID(0);
            PUT_XID(0);
            length += XID_GROUP_HEADER_BYTES;
        }
        if (cr_tx || remote_is_mma) {
            pwork1.data[0] = 0;
#ifdef NONMATCHING
#else
            /* @hack redundant: a store to pwork1.length that never runs */
            if (0) {
                pwork1.length = 99;
            }
#endif
            pwork1.data[1] = XID_PARAMETER_BYTES(XID_MMA_BYTES);
            XID_PARAMETER(XID_MMA, XID_MMA_BYTES);
            PUT_XID('M');
            PUT_XID('M');
            PUT_XID('A');
            length += XID_PARAMETER_BYTES(XID_MMA_BYTES);
        }
        if ((OIS(11) & OIS11_ENCRYPTION) && (cr_tx || encryption)) {
            pwork1.data[1] += XID_PARAMETER_BYTES(XID_MIX_BYTES);
            XID_PARAMETER(XID_MIX, XID_MIX_BYTES);
            PUT_XID('M');
            PUT_XID('I');
            PUT_XID('X');
            key_string[0] = ENCRYPT_KEY[0];
            key_string[1] = ENCRYPT_KEY[1];
            key_string[2] = ENCRYPT_KEY[2];
            key_string[3] = ENCRYPT_KEY[3];
            key_string[4] = ENCRYPT_KEY[4];
            key_string[5] = ENCRYPT_KEY[5];
            length += XID_PARAMETER_BYTES(XID_MIX_BYTES);
            /* the originator picks its end of key_string from the clock */
            if (ans_org) {
                key_string[KEY_CLOCK] = TIM_10MS >> 8;
                key_string[KEY_CLOCK + 1] = TIM_10MS;
                PUT_XID(key_string[KEY_CLOCK]);
                PUT_XID(key_string[KEY_CLOCK + 1]);
            } else {
                PUT_XID(TIM_10MS >> 8);
                PUT_XID(TIM_10MS);
            }
        }
#ifdef NONMATCHING
#else
        /* @hack redundant: a store to win_k_rx that never runs, and a jump to
         * the next line */
        if (0) {
            win_k_rx = 5;
        }
        goto send;
    send:
#endif
        dce_tx_data_done(length, 1);
    }
}

static void tx_test(void)
{
    Uint16 length;
    Uint16 i;

    if (!destructive && dce_set_tx_data(&pwork0) && !overflow_cnt) {
        *pwork0++ = tx_address;
        *pwork0++ = (pf_tx << POLL_FINAL_SHIFT) | tx_code_number;
        length = UNNUMBERED_BYTES;
        if (test_sent) {
            for (i = 1; i <= test_packet_size; i++) {
                *pwork0++ = i;
                length++;
            }
        } else {
            for (i = 1; i < rm_frame_length; i++) {
                *pwork0++ = *rm_read_temp++;
                length++;
            }
        }
        tr_full = 1;
        dce_tx_data_done(length, 1);
    }
}

static void tx_unnumbered(void)
{
    if (dce_set_tx_data(&pwork0) && !overflow_cnt) {
        *pwork0++ = tx_address;
        *pwork0++ = (pf_tx << POLL_FINAL_SHIFT) | tx_code_number;
        dce_tx_data_done(UNNUMBERED_BYTES, 1);
    } else if (overflow_cnt <=
               LAPM_OVERFLOW_BYTES - OVERFLOW_LENGTH_BYTES - UNNUMBERED_BYTES) {
        overflow_cnt += OVERFLOW_LENGTH_BYTES + UNNUMBERED_BYTES;
        SAVE_OVERFLOW_BYTE(UNNUMBERED_BYTES);
        SAVE_OVERFLOW_BYTE(tx_address);
        SAVE_OVERFLOW_BYTE((pf_tx << POLL_FINAL_SHIFT) | tx_code_number);
    }
}

static void tx_ui(void)
{
    Uint16 length;
    if (dce_set_tx_data(&pwork0) && !overflow_cnt) {
        *pwork0++ = tx_address;
        *pwork0++ = (pf_tx << POLL_FINAL_SHIFT) | tx_code_number;
        if (tx_brk == 1) {
            *pwork0++ = (nsb_tx << BREAK_SEQUENCE_SHIFT) | BREAK_MESSAGE;
            *pwork0++ = break_option;
            if (tx_break_length) {
                *pwork0++ = tx_break_length;
                length = UNNUMBERED_BYTES + BREAK_BYTES;
            } else {
                length = UNNUMBERED_BYTES + BREAK_BYTES - 1;
            }
            /* @hack spelling: break_option tested twice, where one test would
             * do */
            if (break_option == (BREAK_DESTRUCTIVE | BREAK_EXPEDITED)) {
                brk_sent = 1;
            }
            if (break_option == (BREAK_DESTRUCTIVE | BREAK_EXPEDITED)) {
                destructive = 1;
                tr_full = 1;
                timer_t401_run = 0;
                timer_recovery_condition = 0;
            }
        } else {
            *pwork0++ =
                (nrb_tx << BREAK_SEQUENCE_SHIFT) | BREAK_MESSAGE | BREAK_ACK;
            length = UNNUMBERED_BYTES + 1;
        }
        dce_tx_data_done(length, 1);
    }
}

static void reset(void)
{
    Uint16 i;
    if (destructive) {
        vxd_dte_clr_rx_buff();
        vxd_dte_clr_tx_buff();
    }
    TM_READ = TM_WRITE;
    TM_BUFFER = TM_BUFFER_ARRAY;
    tm_read_temp = TM_WRITE;
    tm_shadow_ptr = TM_WRITE;
    TM_CNT = 0;
    tm_cnt_temp = 0;
    tm_cnt_shadow = 0;
    RR_READ = RR_WRITE;
    rr_shadow_ptr = RR_WRITE;
    RR_CNT = 0;
    rr_cnt_shadow = 0;
    own_busy = 0;
    vs = 0;
    va = 0;
    vr = 0;
    nr_rx = 0;
    previous_va = 0;
    remote_busy = 0;
    test_sent = 0;
    tm_frame_address = retrans_array;
    tm_frame_length = retrans_array + LAPM_SEQUENCE_NUMBERS;
    tm_frame_address[0] = tm_read_temp - TM_BUFFER;
    /* @hack spelling: i <= LAPM_SEQUENCE_NUMBERS - 1, where
     * i < LAPM_SEQUENCE_NUMBERS would do */
    for (i = 0; i <= LAPM_SEQUENCE_NUMBERS - 1; ++i) {
        tm_frame_length[i] = 0;
    }
    scrap_srej_buff();
    timer_t401_run = 0;
    timer_recovery_condition = 0;
    recover_with_rej = 0;
    comp_in_cntr_bak = 0;
    raz(p0, p1, p2);
    cmd_CLEAR_STAT();
    overflow_cnt = 0;
    overflow_write = 0;
    overflow_read = 0;
}

#define WRAP_REJECT_READ()                                                     \
    do {                                                                       \
        if (srej_buff_read > srej_buff_topp) {                                 \
            srej_buff_read = srej_buffer;                                      \
        }                                                                      \
    } while (0)

/* srej_buffer keeps each frame as SREJ_HEADER_BYTES and its information.
 * the header holds the frame's N(S) and its length in two octets, high
 * first */
#define SREJ_HEADER_BYTES 3

/* move each frame of srej_buffer numbered vr into the receive ring, and drop
 * each outside the window. a frame inside it waits for one still missing,
 * which an SREJ asks for */
static void pass_srej_buffer(void)
{
    Uint8 ns;
    Uint8 ns_ahead;
    int high;
    Uint16 i;
#ifdef NONMATCHING
#else
    /* @hack redundant: a local that only takes pwork1.data */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    Uint8 *unused;
#endif
    if (destructive) {
        return;
    }
#ifdef NONMATCHING
#else
    /* @hack redundant: stores to srej_condition and srej_to_send that never
     * run */
    if (0) {
        srej_condition = 0;
        srej_to_send = 0;
    }
#endif
    while (srej_buff_cntr && RR_CNT < RING_BUSY_BYTES && !srej_condition) {
        ns = *srej_buff_read++;
        if (ns == vr) {
            WRAP_REJECT_READ();
            srej_buff_cntr--;
            high = *srej_buff_read++;
            WRAP_REJECT_READ();
            srej_buff_cntr--;
            srej_frame_length = *srej_buff_read++;
            WRAP_REJECT_READ();
            high <<= 8;
            srej_buff_cntr--;
            srej_frame_length |= high;
#ifdef NONMATCHING
#else
            unused = pwork1.data;
#endif
            vr = NEXT_SEQUENCE(vr);
            pwork1.data = RR_WRITE;
            for (i = 1; i <= srej_frame_length; i++) {
                *pwork1.data++ = *srej_buff_read++;
                WRAP_REJECT_READ();
                srej_buff_cntr--;
                RR_CNT++;
                if (pwork1.data > RR_TOPP) {
#ifdef NONMATCHING
#else
                    /* @hack redundant: an empty block that never runs */
                    if (0) {
                    }
#endif
                    pwork1.data = RR_BUFFER;
                }
            }
            RR_WRITE = pwork1.data;
#ifdef NONMATCHING
#else
            unused = pwork1.data;
#endif
        } else {
            ns_ahead = ns;
            if (ns_ahead < vr) {
                ns_ahead += LAPM_SEQUENCE_NUMBERS;
            }
#ifdef NONMATCHING
#else
            /* @hack redundant: a store to ns_ahead that never runs */
            if (0) {
                ns_ahead = 1;
            }
#endif
            if (ns_ahead - vr < win_k_rx) {
                srej_condition = 1;
                srej_buff_read--;
            } else {
                WRAP_REJECT_READ();
                high = *srej_buff_read++;
                WRAP_REJECT_READ();
                srej_frame_length = *srej_buff_read++;
                WRAP_REJECT_READ();
                high <<= 8;
                srej_frame_length |= high;
                srej_buff_cntr -= srej_frame_length + SREJ_HEADER_BYTES;
#ifdef NONMATCHING
                /* this side wraps the frame's end as an offset in the buffer,
                 * at LAPM_REJECT_BYTES. the matching side forms a pointer past
                 * the buffer's top before it wraps it */
                srej_buff_read =
                    srej_buffer +
                    ((srej_buff_read - srej_buffer + srej_frame_length) %
                     LAPM_REJECT_BYTES);
#else
                srej_buff_read += srej_frame_length;
                if (srej_buff_read > srej_buff_topp) {
                    /* @hack spelling: srej_buff_read stepped back inside the
                     * expression that sets it, where srej_buff_read - 1 -
                     * srej_buff_topp + srej_buffer would do */
                    /* NOLINTBEGIN(clang-diagnostic-unsequenced) */
                    srej_buff_read =
                        (srej_buff_read -= 1) - srej_buff_topp + srej_buffer;
                    /* NOLINTEND(clang-diagnostic-unsequenced) */
                }
#endif
            }
        }
    }
    if (!srej_condition) {
        if (!own_busy) {
            if (RR_CNT < RING_BUSY_BYTES) {
                if (cr_rx && pf_rx) {
                    return;
                }
                if (tm_cnt_temp != TM_CNT && !remote_busy && valid_tx) {
                    return;
                }
                QUEUE_FRAME(RECEIVE_READY, CTL_RR);
                RESPONSE();
            } else {
                own_busy = 1;
                QUEUE_FRAME(RECEIVE_NOT_READY, CTL_RNR);
                RESPONSE();
            }
        }
    } else {
        own_busy = 0;
        BMR(23) |= BMR23_LINK_ERROR;
        remote_err();
        QUEUE_FRAME(SELECTIVE_REJECT, CTL_SREJ);
        COMMAND();
    }
}

static void re_transmit_srej(void)
{
    Uint16 length;
    Uint16 i;
    Uint8 *data;
    if (dce_set_tx_data(&pwork0) && !overflow_cnt) {
        INFORMATION_HEADER(vs_srej);
        data = tm_frame_address[vs_srej] + TM_BUFFER;
        for (i = 1; i <= tm_frame_length[vs_srej]; ++i) {
            *pwork0++ = *data++;
            if (data > TM_TOPP) {
                data = TM_BUFFER;
            }
            ++length;
        }
        dce_tx_data_done(length, 1);
        ++StatisticInfo.frames_resent;
    }
}

static void scrap_srej_buff(void)
{
    srej_buff_read = srej_buffer;
    srej_buff_write = srej_buffer;
    srej_buff_cntr = 0;
    srej_condition = 0;
}

#define BUFFER_REJECT_BYTE(value)                                              \
    do {                                                                       \
        *srej_buff_write++ = (value);                                          \
        if (srej_buff_write > srej_buff_topp) {                                \
            srej_buff_write = srej_buffer;                                     \
        }                                                                      \
        ++srej_buff_cntr;                                                      \
    } while (0)

static void store_in_srej_buff(void)
{
    Uint16 i;
    if (rm_frame_length + srej_buff_cntr + SREJ_HEADER_BYTES <=
        LAPM_REJECT_BYTES) {
#ifdef NONMATCHING
#else
    /* @hack redundant: a label no jump takes */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-label) */
    store_header:
#endif
        BUFFER_REJECT_BYTE(ns_rx);
        BUFFER_REJECT_BYTE(rm_frame_length >> 8);
        BUFFER_REJECT_BYTE(rm_frame_length);
        for (i = 1; i <= rm_frame_length; ++i) {
            BUFFER_REJECT_BYTE(*rm_read_temp++);
        }
    }
}

static void test_manager(void)
{
    if (loopb) {
        if ((BMR(27) & BMR27_TEST_REQUEST) && !test_sent && !destructive) {
            if (dce_set_tx_data(&pwork0) && !overflow_cnt) {
                QUEUE_FRAME(TEST_FRAME, CTL_TEST);
                pf_tx = 0;
                cr_tx = 1;
                test_sent = 1;
                BMR(27) &= ~BMR27_TEST_REQUEST;
                timer_test_offset = TIM_10MS;
            }
        } else if (test_sent && TIM_10MS - timer_test_offset > timer_t401_max) {
            test_bad(TEST_TIMED_OUT);
            test_sent = 0;
        }
    } else if (BMR(27) & BMR27_TEST_REQUEST) {
        BMR(27) &= ~BMR27_TEST_REQUEST;
        test_bad(TEST_NO_LOOPBACK);
    }
}

static void pass_RR_to_dte(void)
{
    Uint8 *buffer;
    Uint16 length;
    Uint16 count;

    rr_cnt_shadow = RR_CNT;
    do {
        length = vxd_dte_set_tx_data(&buffer);
        if (rr_cnt_shadow < length) {
            length = rr_cnt_shadow;
        }
        count = 0;
        if (length > DTE_PASS_BYTES) {
            length = DTE_PASS_BYTES;
        }
        /* a break from the line stops the bytes at its place in the ring */
        while (length--) {
            if ((BMR(23) & BMR23_LINE_BREAK) && RxBreakPointer[0] == RR_READ) {
                break;
            }
            *buffer++ = *RR_READ++;
            if (RR_READ > RR_TOPP) {
                RR_READ = RR_BUFFER;
            }
            ++count;
        }
        if (count) {
            RR_CNT -= count;
            rr_cnt_shadow -= count;
            vxd_dte_tx_data_done(count);
        }
        if ((BMR(23) & BMR23_LINE_BREAK) && RxBreakPointer[0] == RR_READ) {
            vxd_dte_tx_brk(get_EC_brk_length(), 1);
        }
    } while (vxd_dte_set_tx_data(&buffer) && rr_cnt_shadow);
}

static void pass_dte_to_TM(void)
{
    Uint8 *buffer;
    Uint16 length;
    for (;;) {
        vxd_dte_get_rx_data(&buffer, &length);
        if (!length || tm_cnt_shadow + length >= tm_lnt) {
            break;
        }
        tm_cnt_shadow += length;
        while (length--) {
            *TM_WRITE++ = *buffer++;
            if (TM_WRITE > TM_TOPP) {
                TM_WRITE = TM_BUFFER;
            }
        }
        vxd_dte_rx_data_done();
    }
    TM_CNT = tm_cnt_shadow;
}

static void buff_rx_EC_brk(Uint16 length)
{
    Uint8 i = 0;
    while (RxBreakLength[i]) {
        if (i == LAPM_BREAKS - 1) {
            return;
        }
        ++i;
    }
    RxBreakLength[i] = length;
    RxBreakPointer[i] = RR_WRITE;
    BMR(23) |= BMR23_LINE_BREAK;
}

Uint16 get_EC_brk_length(void)
{
    Uint16 length = RxBreakLength[0];
    /* @bug logic: a break that waits behind another goes to the host at the
     * other's place in the ring. it goes ahead of the bytes between them. the
     * shift at 0x0caff7b6 moves RxBreakLength alone, not RxBreakPointer */
    RxBreakLength[0] = RxBreakLength[1];
    RxBreakLength[1] = RxBreakLength[2];
    RxBreakLength[2] = RxBreakLength[3];
    RxBreakLength[3] = 0;
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: the pointers shift with the
     * lengths */
    RxBreakPointer[0] = RxBreakPointer[1];
    RxBreakPointer[1] = RxBreakPointer[2];
    RxBreakPointer[2] = RxBreakPointer[3];
    RxBreakPointer[3] = 0;
#endif
    if (!RxBreakLength[0]) {
        BMR(23) &= ~BMR23_LINE_BREAK;
    }
    return length;
}
