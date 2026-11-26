#include <sg_xpt.h>
#include "atcom.h"
#include "bl.h"
#include "command.h"
#include "country.h"
#include "countrydb.h"
#include "data_tst.h"
#include "dce.h"
#include "dialing.h"
#include "dte_vxd.h"
#include "ecdc_def.h"
#include "ecdcutil.h"
#include "line_estab.h"
#include "line_mon.h"
#include "maintain.h"
#include "messages.h"
#include "modemdef.h"
#include "roc_k56.h"
#include "target.h"
#include "util_mod.h"

enum ring_state {
    RING_IDLE = 0,
    RING_FIRST_PULSE = 1,
    RING_SHORT_PULSE = 2,
    RING_COUNT = 3,
    RING_AUTO_ANSWER = 4,
    RING_ANSWER = 5,
    RING_GAP = 7,
    RING_SECOND_PULSE = 8
};
/* in setup_timer's 10 ms ticks */
enum {
    SHORT_RING_PULSE = 30,
    SHORT_RING_GAP = 200,
    LEASE_LINE_DELAY = 300,
    RING_COUNT_TIMEOUT = 800
};
/* the &T commands' numbers, as test_command takes them, and S16 while each
 * test runs */
#define T_END_TEST 0
#define T_ANALOG_LOOP 1
#define T_REMOTE_DIGITAL_LOOP 6
#define S16_ANALOG_LOOP 0x01
#define S16_REMOTE_DIGITAL_LOOP 0x10

/* set once the link connects, while modem_connect waits for WAIT_TIM */
#define BMR16_CONNECT_DUE 0x40
/* set while lease_line_timer runs */
#define BMR11_LEASE_TIMER 0x20
static Uint8 idle_polls = 0;
static Uint32 ring_gap_deadline, ring_high_deadline, short_pulse_deadline;
static Uint8 previous_auto_answer;

void maintain(Uint8 run)
{
    Uint8 result;

    if (!run) {
        idle_polls = 0;
        maintain_phase = MAINTAIN_IDLE;
        test_phase = TEST_RESET;
        line_monitor_phase = LINE_RESET;
        dial_phase = 0;
        line_estab_phase = ESTAB_BEGIN;
        ring_monitor_phase = RING_IDLE;
        logon_phase = 0;
        ties_phase = 0;
        dp_set_ring_periods(cur_cnt->ring_on, cur_cnt->ring_off);
        return;
    }
    dp_analyzer();
    switch (maintain_phase) {
    case MAINTAIN_IDLE:
        off_line_maintain();
        carrier_off = 0;
        break;
    case MAINTAIN_DIAL:
        if (dtr_monitor() != 0) {
            maintain_phase = MAINTAIN_IDLE;
            BL_Condition(BL_EVENT_DTR_OFF);
            break;
        }
        result = dialing();
        if (result == DIAL_DONE) {
            maintain_phase = MAINTAIN_ESTABLISH;
        } else if (result == DIAL_COMMAND) {
            maintain(0);
        } else if (result != DIAL_PENDING) {
            go_off_line();
            CMD_B_PT = 0;
            BMR(7) &= ~BMR7_COMMAND_LINE;
            dialing_error(result);
            set_command_mode();
        }
#ifdef NONMATCHING
#else
        /* @hack redundant: a second, empty test of the dial's result */
        if (result != DIAL_PENDING) {
        }
#endif
        break;
    case MAINTAIN_ESTABLISH:
        if (dtr_monitor()) {
            BMR(6) &= 0xf0;
            maintain_phase = MAINTAIN_IDLE;
            if (SREG(14) & S14_ORIGINATE) {
                BL_Condition(BL_EVENT_DTR_OFF);
            }
        } else {
            line_estab();
        }
        break;
    case MAINTAIN_WAIT_CONNECT:
        on_line_maintain();
        if (cmd_response_buffer[0] == ECDC_RESPONSE_CONNECTED) {
            BMR(16) |= BMR16_CONNECT_DUE;
            setup_timer(&WAIT_TIM, 0);
            maintain_phase = MAINTAIN_ONLINE;
        }
        if (!target_get_TD() ||
            ((BMR(7) & BMR7_HOST_BYTE) && !(BMR(11) & BMR11_LEASED_LINE) &&
             ((SREG(14) & S14_ORIGINATE) || !(SREG(31) & 8))) ||
            cmd_response_buffer[0] == ECDC_RESPONSE_RESET) {
            go_off_line();
            cmd_parameter_buffer[0] = HALT_NOW;
            cmd_HALT();
            send_coded_message(RESULT_NO_CARRIER, NULL);
            set_command_mode();
        }
        break;
    case MAINTAIN_ONLINE:
        if (!(BMR(6) & BMR6_ON_LINE)) {
            go_off_line();
            cmd_parameter_buffer[0] = HALT_NOW;
            cmd_HALT();
            set_command_mode();
            return;
        }
        if (BMR(10) & 4) {
            vxd_dte_rx_data_done();
        }
        on_line_maintain();
        break;
    case MAINTAIN_DRAIN:
        if (!carrier_off) {
            line_monitor();
        }
        if (cmd_response_buffer[0] == ECDC_RESPONSE_RESET) {
            if (carrier_off == 1 || cmd_response_buffer[1] == 1) {
                if (!(SREG(14) & S14_ORIGINATE)) {
                    BMR(10) |= BMR10_ANSWERING;
                }
                go_off_line();
                send_coded_message(RESULT_NO_CARRIER, NULL);
                set_command_mode();
                BMR(7) &= ~BMR7_COMMAND_LINE;
                BMR(6) &= ~BMR6_RUNNING_COMMAND;
                CMD_B_PT = 0;
            } else {
                dp_start_sending_clear_down();
                maintain_phase = MAINTAIN_CLEAR_DOWN;
            }
        }
        break;
    case MAINTAIN_CLEAR_DOWN:
        if (dp_is_clear_down_sent()) {
            H0_common_section();
            if (at_com_result == AT_OK) {
                send_coded_message(RESULT_OK, NULL);
                set_command_mode();
                BMR(7) &= ~BMR7_COMMAND_LINE;
                BMR(6) &= ~BMR6_RUNNING_COMMAND;
                CMD_B_PT = 0;
            } else {
                BMR(16) |= BMR16_RESUME_LINE;
            }
        }
    }

    if (maintain_phase == MAINTAIN_IDLE && !(BMR(7) & BMR7_COMMAND_LINE)) {
        if (++idle_polls == 2) {
            GenerateIRQ(0x2000);
            idle_polls = 0;
        }
    } else {
        idle_polls = 0;
    }
}

void off_line_maintain(void)
{
    at_commands();
    dtr_monitor();
    dte_tests_monitor();
    ring_monitor();
    redial_monitor();
    lease_line_monitor();
    data_test_manager();
}

void on_line_maintain(void)
{
    if (BMR(6) & BMR6_ON_LINE) {
        if (STATUS_BITS & STATUS_INACTIVE) {
            SREG(45) = S45_INACTIVE;
            force_h0_cmd();
        }
        if (BMR(7) & BMR7_LINK_LOST) {
            go_off_line();
            cmd_parameter_buffer[0] = HALT_NOW;
            cmd_HALT();
            send_coded_message(RESULT_NO_CARRIER, NULL);
            set_command_mode();
            BMR(7) &= ~BMR7_LINK_LOST;
            CMD_B_PT = 0;
            return;
        }
        dtr_monitor();
        rts_monitor();
        dte_tests_monitor();
        escape_seq_monitor();
        if (timer_expired(WAIT_TIM)) {
            if (BMR(16) & BMR16_CONNECT_DUE) {
                BMR(16) |= 0x10;
                BMR(16) &= ~BMR16_CONNECT_DUE;
                modem_connect();
            }
            if (line_monitor_phase == LINE_MONITORING) {
                dce_ignore_rx_async_data(0);
            }
        }
        at_commands();
        data_test_manager();
        line_monitor();
    }
}

void ring_monitor(void)
{
    Uint8 auto_answer_rings;

    if (timer_expired(OFF_HOOK_TIM) && !(SREG(27) & S27_LEASED_LINE) &&
        !(BMR(9) & BMR9_OFF_HOOK)) {
        if (SREG(1) && (BMR(32) & BMR32_ANSWER)) {
            BMR(32) &= ~BMR32_ANSWER;
            ring_monitor_phase = RING_ANSWER;
        }
        if ((auto_answer_rings = SREG(0)) != previous_auto_answer &&
            auto_answer_rings) {
            if (SREG(1) >= auto_answer_rings) {
                ring_monitor_phase = RING_AUTO_ANSWER;
            }
        }
        previous_auto_answer = SREG(0);
        switch (ring_monitor_phase) {
        case RING_IDLE:
            if (timer_expired(ring_timer)) {
                BMR(31) &= ~BMR31_NO_ANSWER;
                SREG(1) = 0;
            }
            if (get_ring_state()) {
                setup_timer(&ring_timer, cur_cnt->ringback.on1.minimum);
                setup_timer(&ring_high_deadline, cur_cnt->ringback.on1.maximum);
                setup_timer(&short_pulse_deadline, SHORT_RING_PULSE);
                setup_timer(&ring_gap_deadline, SHORT_RING_GAP);
                vxd_dte_set_ri(0);
                ring_monitor_phase = RING_FIRST_PULSE;
            }
            break;
        case RING_FIRST_PULSE:
        case RING_SECOND_PULSE:
            if (!get_ring_state()) {
                vxd_dte_set_ri(1);
                if (!timer_expired(ring_timer)) {
                    if (timer_expired(short_pulse_deadline) &&
                        (cur_cnt->reset_flags & SHORT_RING_PULSES)) {
                        ring_monitor_phase = RING_SHORT_PULSE;
                    } else {
                        ring_monitor_phase = RING_IDLE;
                        setup_timer(&ring_timer, RING_COUNT_TIMEOUT);
                    }
                } else if (timer_expired(ring_high_deadline)) {
                    ring_monitor_phase = RING_IDLE;
                    setup_timer(&ring_timer, RING_COUNT_TIMEOUT);
                } else if (cur_cnt->ringback.off1.minimum !=
                               cur_cnt->ringback.off2.minimum &&
                           ring_monitor_phase == RING_FIRST_PULSE) {
                    setup_timer(&ring_timer, cur_cnt->ringback.off1.minimum);
                    setup_timer(&ring_gap_deadline,
                                cur_cnt->ringback.off1.maximum);
                    ring_monitor_phase = RING_GAP;
                } else {
                    ring_monitor_phase = RING_COUNT;
                }
            }
            break;
        case RING_GAP:
            if (get_ring_state()) {
                vxd_dte_set_ri(0);
                if (!timer_expired(ring_timer)) {
                    ring_monitor_phase = RING_IDLE;
                    setup_timer(&ring_timer, RING_COUNT_TIMEOUT);
                } else {
                    setup_timer(&ring_timer, cur_cnt->ringback.on2.minimum);
                    setup_timer(&ring_high_deadline,
                                cur_cnt->ringback.on1.maximum);
                    ring_monitor_phase = RING_SECOND_PULSE;
                }
            } else if (timer_expired(ring_gap_deadline)) {
                ring_monitor_phase = RING_IDLE;
                setup_timer(&ring_timer, RING_COUNT_TIMEOUT);
            }
            break;
        case RING_SHORT_PULSE:
            if (timer_expired(ring_gap_deadline) && !get_ring_state()) {
                ring_monitor_phase = RING_COUNT;
            }
            break;
        case RING_COUNT:
            SREG(1)++;
            setup_timer(&ring_timer, RING_COUNT_TIMEOUT);
            if (CMD_B_PT == 0) {
                BMR(10) |= BMR10_ANSWERING;
                send_coded_message(RESULT_RING, NULL);
                set_command_mode();
            }
            if (!auto_answer_rings || SREG(1) < auto_answer_rings) {
                ring_monitor_phase = RING_IDLE;
                break;
            }

            /* fall through */
        case RING_AUTO_ANSWER:
            if ((BMR(31) & BMR31_NO_ANSWER) ||
                (((SREG(27) & 2) || (SREG(21) & S21_D) == S21_D2) &&
                 vxd_dte_get_dtr())) {
                ring_monitor_phase = RING_IDLE;
                set_command_mode();
                break;
            }

            /* fall through */
        case RING_ANSWER:
            SREG(1) = 0;
            SS41 = 0;
            force_a_cmd();
            ring_monitor_phase = RING_IDLE;
            break;
        }
    }
}

char get_ring_state(void)
{
    return dp_test_ring();
}

void lease_line_monitor(void)
{
    if (!(BMR(11) & BMR11_LEASED_LINE)) {
        BMR(11) &= ~BMR11_LEASE_TIMER;
    } else if (CMD_B_PT >= 2) {
        BMR(11) &= ~BMR11_LEASED_LINE;
        BMR(11) &= ~BMR11_LEASE_TIMER;
    } else if (!(BMR(11) & BMR11_LEASE_TIMER)) {
        setup_timer(&lease_line_timer, LEASE_LINE_DELAY);
        BMR(11) |= BMR11_LEASE_TIMER;
    } else if (timer_expired(lease_line_timer) && timer_expired(OFF_HOOK_TIM)) {
        if (SREG(14) & S14_ORIGINATE) {
            force_d_cmd();
        } else {
            force_a_cmd();
        }
        BMR(11) &= ~BMR11_LEASE_TIMER;
    }
}

void dte_tests_monitor(void)
{
    Uint8 saved_result;

    if (!(SREG(39) & S39_TEST_INPUTS) || (BMR(6) & BMR6_RUNNING_COMMAND)) {
        return;
    }
    if (!dte_get_anl()) {
        if (!v24_anl_previous && !SREG(16) && !(BMR(6) & BMR6_ON_LINE)) {
            saved_result = at_com_result;
            test_command(T_ANALOG_LOOP);
            at_com_result = saved_result;
        }
        v24_anl_previous = 1;
    } else {
        if (v24_anl_previous && SREG(16) == S16_ANALOG_LOOP) {
            saved_result = at_com_result;
            test_command(T_END_TEST);
            at_com_result = saved_result;
        }
        v24_anl_previous = 0;
    }
    if (!dte_get_rdl()) {
        if (!v24_rdl_previous && !SREG(16) && (BMR(6) & BMR6_ON_LINE)) {
            saved_result = at_com_result;
            test_command(T_REMOTE_DIGITAL_LOOP);
            at_com_result = saved_result;
        }
        v24_rdl_previous = 1;
    } else {
        if (v24_rdl_previous && SREG(16) == S16_REMOTE_DIGITAL_LOOP) {
            saved_result = at_com_result;
            test_command(T_END_TEST);
            at_com_result = saved_result;
        }
        v24_rdl_previous = 0;
    }
}
