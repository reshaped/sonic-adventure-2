#include <sg_xpt.h>

#include "command.h"
#include "data_tst.h"
#include "dce.h"
#include "dte_vxd.h"
#include "ecdc_def.h"
#include "ecdcutil.h"
#include "line_estab.h"
#include "line_mon.h"
#include "messages.h"
#include "modemdef.h"
#include "roc_k56.h"
#include "target.h"
#include "util_mod.h"

enum analog_loop_phase { ANALOG_START, ANALOG_WAIT_FOR_CARRIER };

/* nothing sets rdl_phase to DIGITAL_DRAIN, so it and DIGITAL_SETTLE never
 * run */
enum digital_loop_phase {
    DIGITAL_REQUEST,
    DIGITAL_DRAIN,
    DIGITAL_SETTLE,
    DIGITAL_WAIT_FOR_LAPM,
    DIGITAL_START,
    DIGITAL_WAIT_FOR_LOOP,
    DIGITAL_WAIT_FOR_DATA
};

enum test_report_phase { REPORT_STOP, REPORT_ERRORS, REPORT_DRAIN };

/* S16's settings for the tests that run the test pattern.
 * TEST_REMOTE_REQUESTED marks a remote digital loop the far end asked for */
#define LOCAL_ANALOG_PATTERN 0x40
#define REMOTE_DIGITAL_PATTERN 0x20
#define TEST_REMOTE_REQUESTED 0x08

/* the tests' times, in 10 ms ticks. while a loop starts or stops, received
 * data is ignored until WAIT_TIM, a deadline modemdef.h declares. WAIT_TIM
 * is set LOOP_CHANGING_TICKS on while the far end changes its loop. a local
 * analog loop sets it ANALOG_IGNORE_TICKS on, and a remote digital loop
 * ANSWER_TICKS on. a loop that ends sets it a second on. CARRIER_TICKS is how
 * long the local analog loop's carrier may take. ANSWER_TICKS is also the
 * far end's time to answer a loop request, over LAPM or the line.
 * SETTLE_TICKS times DIGITAL_SETTLE, which never runs */
#define LOOP_CHANGING_TICKS 10
#define ANALOG_IGNORE_TICKS 25
#define CARRIER_TICKS 2000
#define ANSWER_TICKS 400
#define SETTLE_TICKS 10

void data_test_manager(void)
{
    Uint16 pump_status;
    Uint8 saved_status;
    char error_digits[10];

    if (line_monitor_phase > LINE_MONITORING) {
        return;
    }
    switch (test_phase) {
    case TEST_RESET:
        reset_test_vars();
        break;
    case TEST_IDLE:
        if ((BMR(6) & (BMR6_DATA | BMR6_COMMAND)) && (SREG(23) & 1) &&
            !(BMR(29) & BMR29_MNP) && !(BMR(27) & BMR27_LAPM)) {
            pump_status = dp_monitor_remote_rdl();
            if (pump_status != 0) {
                if (pump_status == LOOP_CHANGING) {
                    WAIT_TIM = TIM_10MS + LOOP_CHANGING_TICKS;
                    dce_ignore_rx_async_data(1);
                    dce_clr_rx_buff();
                } else {
                    SREG(16) = TEST_REMOTE_REQUESTED;
                    target_local_digital_loop(1);
                    BMR(12) |= 1;
                    BMR(3) |= BMR3_TEST;
                    dte_set_tst(0);
                    test_phase = TEST_REMOTE_REQUEST;
                }
            }
        }
        break;
    case TEST_REMOTE_REQUEST:
        if (dp_monitor_remote_rdl() == 0) {
            WAIT_TIM = TIM_10MS + TICKS_PER_SECOND;
            dce_ignore_rx_async_data(1);
            target_local_digital_loop(0);
            line_monitor_phase = LINE_RESET;
            dce_clr_rx_buff();
            dce_clr_tx_buff();
            reset_test_vars();
        }
        break;
    case TEST_ACTIVE:
        if (SREG(18) && S18_timer < TIM_10MS) {
            BMR(12) |= 0x20;
            set_on_line_cmd();
            test_phase = TEST_REPORT;
        }
        break;
    case TEST_LOCAL_ANALOG:
        switch (anl_phase) {
        case ANALOG_START:
            BMR(7) &= ~BMR7_HOST_BYTE;
            SREG(14) |= S14_ORIGINATE;
            BMR(9) |= BMR9_OFF_HOOK;
            BMR(6) = (BMR(6) & 0xf8) | 9;
            dp_enter_local_analog_loop();
            setup_timer(&test_timer, CARRIER_TICKS);
            anl_phase = ANALOG_WAIT_FOR_CARRIER;
            break;
        case ANALOG_WAIT_FOR_CARRIER:
            if (dp_monitor_local_analog_loop(&error_cnt)) {
                BMR(3) |= BMR3_TEST;
                BMR(3) |= BMR3_LOCAL_ANALOG;
                dte_set_tst(0);
                WAIT_TIM = TIM_10MS + ANALOG_IGNORE_TICKS;
                dce_ignore_rx_async_data(1);
                if (SREG(16) == LOCAL_ANALOG_PATTERN) {
                    BMR(3) |= BMR3_PATTERN;
                    dp_enter_self_test();
                }
                S18_timer = (SREG(18) * TICKS_PER_SECOND) + TIM_10MS;
                update_dce_speed();
                if (!(SREG(27) & 3) && (SREG(36) & 0x0c)) {
                    dce_ignore_rx_async_data(0);
                }
                prepare_link();
                test_phase = TEST_ACTIVE;
            } else if (timer_expired(test_timer) || (BMR(7) & BMR7_HOST_BYTE)) {
                data_tst_error();
            }
            break;
        }
        break;
    case TEST_REMOTE_DIGITAL:
        switch (rdl_phase) {
        case DIGITAL_REQUEST:
            if (BMR(27) & BMR27_LAPM) {
                dte_set_tst(0);
                BMR(24) &= 0x3f;
                BMR(27) |= BMR27_TEST_REQUEST;
                setup_timer(&test_timer, ANSWER_TICKS);
                rdl_phase = DIGITAL_WAIT_FOR_LAPM;
            } else {
                rdl_phase = DIGITAL_START;
            }
            break;
        case DIGITAL_DRAIN:
            /* @hack type: dce_tx_buff_empty's Uint8 converted to char, where
             * the Uint8 would do */
            if (timer_expired(test_timer)) {
                data_tst_error();
            } else if (!(BMR(29) & BMR29_MNP) && (char)dce_tx_buff_empty()) {
                setup_timer(&test_timer, SETTLE_TICKS);
                rdl_phase = DIGITAL_SETTLE;
            }
            break;
        case DIGITAL_SETTLE:
            if (timer_expired(test_timer)) {
                rdl_phase = DIGITAL_START;
            }
            break;
        case DIGITAL_WAIT_FOR_LAPM:
            if (timer_expired(test_timer) || (BMR(24) & 0xc0)) {
                if (BMR(24) & 0x80) {
                    send_coded_message(RESULT_TEST_PASSED, NULL);
                } else {
                    send_coded_message(RESULT_TEST_FAILED, NULL);
                }
                reset_test_vars();
                send_coded_message(RESULT_OK, NULL);
                set_command_mode();
            }
            break;
        case DIGITAL_START:
            dce_ignore_rx_async_data(1);
            BMR(3) &= ~BMR3_TESTS;
            BMR(12) &= 0xde;
            dp_enter_remote_digital_loop();
            setup_timer(&WAIT_TIM, ANSWER_TICKS);
            setup_timer(&test_timer, ANSWER_TICKS);
            rdl_phase = DIGITAL_WAIT_FOR_LOOP;
            break;
        case DIGITAL_WAIT_FOR_LOOP:
            if (dp_monitor_rdl_response(&error_cnt)) {
                dce_clr_rx_buff();
                BMR(3) |= BMR3_REMOTE_DIGITAL;
                BMR(3) |= BMR3_TEST;
                dte_set_tst(0);
                line_monitor_phase = LINE_RESET;
                setup_timer(&test_timer, TICKS_PER_SECOND);
                rdl_phase = DIGITAL_WAIT_FOR_DATA;
            } else if (timer_expired(test_timer)) {
                dce_clr_rx_buff();
                reset_test_vars();
                send_coded_message(RESULT_ERROR, NULL);
                BMR(6) &= ~BMR6_RUNNING_COMMAND;
                set_command_mode();
            }
            break;
        case DIGITAL_WAIT_FOR_DATA:
            if (timer_expired(test_timer)) {
                S18_timer = (SREG(18) * TICKS_PER_SECOND) + TIM_10MS;
                if (SREG(16) == REMOTE_DIGITAL_PATTERN) {
                    BMR(3) |= BMR3_PATTERN;
                    dp_enter_self_test();
                }
                modem_connect();
                dce_ignore_rx_async_data(0);
                test_phase = TEST_ACTIVE;
            }
            break;
        }
        break;
    case TEST_REPORT:
        switch (end_test_phase) {
        case REPORT_STOP:
            set_dte_for_message();
            if (BMR(12) & 0x20) {
                send_cr_lf();
            }
            if (BMR(3) & BMR3_LOCAL_ANALOG) {
                dp_monitor_local_analog_loop(&error_cnt);
                saved_status = BMR(3);
                go_off_line();
                BMR(3) = saved_status;
                cmd_parameter_buffer[0] = HALT_NOW;
                cmd_HALT();
            } else {
                WAIT_TIM = TIM_10MS + TICKS_PER_SECOND;
                dce_ignore_rx_async_data(1);
                line_monitor_phase = LINE_RESET;
                if (BMR(3) & BMR3_REMOTE_DIGITAL) {
                    dp_monitor_rdl_response(&error_cnt);
                    dp_stop_remote_digital_loop();
                    end_test_phase = REPORT_DRAIN;
                    break;
                } else {
                    target_local_digital_loop(0);
                }
            }
            /* fall through: only a remote loop waits, in REPORT_DRAIN, before
             * its report */
        case REPORT_ERRORS:
            if (BMR(3) & BMR3_PATTERN) {
                send_cr_lf();
                send_cr_lf();
                if (error_cnt > 255) {
                    error_cnt = 255;
                }
                convert_value_to_ascii(error_cnt, 10, 3, error_digits);
                send_message(error_digits);
            }
            reset_test_vars();
            send_coded_message(RESULT_OK, NULL);
            BMR(6) &= ~BMR6_RUNNING_COMMAND;
            set_command_mode();
            break;
        case REPORT_DRAIN:
            if (!dp_monitor_rdl_response(&pump_status)) {
                dce_clr_rx_buff();
                vxd_dte_clr_tx_buff();
                end_test_phase = REPORT_ERRORS;
            }
            break;
        }
        break;
    }
}

void data_tst_error(void)
{
    reset_test_vars();
    go_off_line();
    send_coded_message(RESULT_NO_CARRIER, NULL);
    set_command_mode();
}

void reset_test_vars(void)
{
    dte_set_tst(1);
    SREG(16) = 0;
    test_phase = TEST_IDLE;
    rdl_phase = DIGITAL_REQUEST;
    anl_phase = ANALOG_START;
    end_test_phase = REPORT_STOP;
    BMR(3) &= ~BMR3_TESTS;
    BMR(12) &= 0xde;
}
