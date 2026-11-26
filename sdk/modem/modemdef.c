/* the modem controller's state, which modemdef.h declares */
#include <sg_xpt.h>
#include "modemdef.h"

Uint8 *ptr_CMD_ALT = CMD_BUF;

/* the AT interpreter: the S-registers, the command line in CMD_BUF, and
 * the interpreter's timers and results */
Uint8 Sregisters[MODEM_S_REGISTERS];
Uint32 at_command_timer, S18_timer;
Uint8 at_com_result, last_s_used;
Uint8 *ptr_num_buff;
Uint8 fixed_dte_speed, fixed_dte_format, line_length;
Uint8 CMD_BUF[MODEM_COMMAND_LENGTH + 1];
Uint16 CMD_B_PT;
Uint8 S3_THEN, S4_MASK, Z_COMMAND, SS22, SS41, s27_copy, s31_copy;
struct at_interrogation interrogation;

/* a stored number to dial, and the logon string sent after it */
Uint8 *ptr_dial_string, *ptr_logon_buff;
Uint8 log_buf[45], dial_buf[35];
Uint8 log_size, logon_to_use;
Uint32 logon_timer;
Uint8 logon_phase, logon_cmd, pause_started;

/* the local and remote loopback tests */
Uint8 test_phase, anl_phase, rdl_phase, end_test_phase;
Uint16 error_cnt;
Uint32 test_timer;

/* the line monitor, which retrains the link or falls back or forward in
 * rate as the line changes. its rates are data-pump rate codes */
Uint8 line_monitor_phase, mon_ret_phase, ret_cntr, v32_v34_ret_cntr;
Uint8 fb_cntr, ffwd_on, ffwd_suspended, o1_cmd_retrain, remote_init_ret;
Uint8 present_rx_rate, wish_rx_rate, initial_rx_rate;
Uint32 present_modulation;
Uint32 line_monitor_timer, carrier_lost_timer, mse_pace_timer, mse_good_timer;
Uint32 v32_v34_fbw_timer, v32_v34_retw_timer;

/* line establishment and its tone detectors, then the connection's upkeep:
 * rings, carrier and the V.24 control lines */
Uint32 line_estab_timer, auto_fax_timer;
Uint32 timer_1300hz, timer_2100hz, timer_2225_2250hz, cng_timer;
Uint8 dp_max_rate;
Uint32 dp_mod_types;
Uint8 no_connection_yet, max_connection_tx_speed, max_connection_rx_speed;
Uint8 last_connection_tx_speed, last_connection_rx_speed;
Uint8 line_estab_phase, speed_of_dce;
Uint32 ct_timer;
Uint8 calling_tone_on, previous_filter_state, busy_cntr;
Uint32 busy_rb_timer, rts_to_cts_timer, dtr_timer, ring_timer, lease_line_timer;
Uint8 maintain_phase, ring_monitor_phase, v24_anl_previous, v24_rdl_previous;
Uint8 cts_is_low;
Uint32 OFF_HOOK_TIM, WAIT_TIM, ON_LI_TIM;
char carrier_off;
Uint32 IdTimer, CMD_ECHO_TIM, dcd_timer;

/* the terminal's speed and format as its AT commands set them, and the timers'
 * clocks. then the parser of TIES, the time-independent escape sequence */
Uint8 speed_of_AT, format_of_AT, prsc_100;
Uint32 TIM_208US;
Uint8 timer_at_4k8;
Uint32 BL_clock_parse_tmr;
Uint8 ties_cmd_buff[41], ties_cmd_ptr, ties_phase, ties_s_reg, ties_upper_range;
const struct ties_command_limit *ties_cmd_tbl_ptr;

/* the dial-string interpreter and its wait-for-dial-tone timers */
Uint32 dial_timer, dial_timer2, last_digit_timer;
Uint8 *dial_string_ptr;
Uint8 dial_phase, dial_exec_phase, lcs_previous_state;
Uint8 dial_buffer[MODEM_DIAL_LENGTH + 2];
Uint8 mute_is_on, NoSilenceFor210ms;
Uint32 dt_timer, wfdt_timer, dt_valid_tim;
Uint8 v25_bis_com_result, PwrUpWithSpeed;
