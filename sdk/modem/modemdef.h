/* the modem controller's state, which the AT interpreter shares */
#ifndef MODEM_MODEMDEF_H
#define MODEM_MODEMDEF_H
#include <sg_xpt.h>

#define MODEM_S_REGISTERS 80
#define MODEM_COMMAND_LENGTH 128
#define MODEM_DIAL_LENGTH 70

struct at_interrogation {
    Uint8 reserved : 5;
    Uint8 phase : 3;
};

struct ties_command_limit {
    Uint8 allowed;
    Uint8 maximum;
};

extern Uint8 *ptr_CMD_ALT;
/* the AT interpreter: the S-registers, the command line in CMD_BUF, and
 * the interpreter's timers and results */
extern Uint8 Sregisters[MODEM_S_REGISTERS];
extern Uint32 at_command_timer, S18_timer;
extern Uint8 at_com_result, last_s_used;
extern Uint8 *ptr_num_buff;
extern Uint8 fixed_dte_speed, fixed_dte_format, line_length;
extern Uint8 CMD_BUF[MODEM_COMMAND_LENGTH + 1];
extern Uint16 CMD_B_PT;
extern Uint8 S3_THEN, S4_MASK, Z_COMMAND, SS22, SS41, s27_copy, s31_copy;
/* SS22's bits, which update_SS22 sets from S22_RESULTS. SS22_EXTENDED gives
 * the extended results, such as NO DIALTONE where the basic set has NO
 * CARRIER */
#define SS22_AUTO_PULSE 0x04
#define SS22_DETECT_RINGBACK 0x08
#define SS22_WAIT_DIALTONE 0x10
#define SS22_DETECT_BUSY 0x20
#define SS22_EXTENDED 0x40
extern struct at_interrogation interrogation;

/* a stored number to dial, and the logon string sent after it */
extern Uint8 *ptr_dial_string, *ptr_logon_buff;
extern Uint8 log_buf[45], dial_buf[35];
extern Uint8 log_size, logon_to_use;
extern Uint32 logon_timer;
extern Uint8 logon_phase, logon_cmd, pause_started;

/* the local and remote loopback tests */
extern Uint8 test_phase, anl_phase, rdl_phase, end_test_phase;
extern Uint16 error_cnt;
extern Uint32 test_timer;

/* the line monitor, which retrains the link or falls back or forward in
 * rate as the line changes. its rates are data-pump rate codes */
extern Uint8 line_monitor_phase, mon_ret_phase, ret_cntr, v32_v34_ret_cntr;
extern Uint8 fb_cntr, ffwd_on, ffwd_suspended, o1_cmd_retrain, remote_init_ret;
extern Uint8 present_rx_rate, wish_rx_rate, initial_rx_rate;
extern Uint32 present_modulation;
extern Uint32 line_monitor_timer, carrier_lost_timer, mse_pace_timer,
    mse_good_timer;
extern Uint32 v32_v34_fbw_timer, v32_v34_retw_timer;

/* line establishment and its tone detectors, then the connection's upkeep:
 * rings, carrier and the V.24 control lines */
extern Uint32 line_estab_timer, auto_fax_timer;
extern Uint32 timer_1300hz, timer_2100hz, timer_2225_2250hz, cng_timer;
extern Uint8 dp_max_rate;
extern Uint32 dp_mod_types;
extern Uint8 no_connection_yet, max_connection_tx_speed,
    max_connection_rx_speed;
extern Uint8 last_connection_tx_speed, last_connection_rx_speed;
extern Uint8 line_estab_phase, speed_of_dce;
extern Uint32 ct_timer;
extern Uint8 calling_tone_on, previous_filter_state, busy_cntr;
extern Uint32 busy_rb_timer, rts_to_cts_timer, dtr_timer, ring_timer,
    lease_line_timer;
extern Uint8 maintain_phase, ring_monitor_phase, v24_anl_previous,
    v24_rdl_previous;
extern Uint8 cts_is_low;
extern Uint32 OFF_HOOK_TIM, WAIT_TIM, ON_LI_TIM;
extern char carrier_off;
extern Uint32 IdTimer, CMD_ECHO_TIM, dcd_timer;

/* the terminal's speed and format as its AT commands set them, and the timers'
 * clocks. then the parser of TIES, the time-independent escape sequence */
extern Uint8 speed_of_AT, format_of_AT, prsc_100;
extern Uint32 TIM_208US;
extern Uint8 timer_at_4k8;
extern Uint32 BL_clock_parse_tmr;
extern Uint8 ties_cmd_buff[41], ties_cmd_ptr, ties_phase, ties_s_reg,
    ties_upper_range;
extern const struct ties_command_limit *ties_cmd_tbl_ptr;

/* format_of_AT's codes. FORMAT_7_BITS takes a byte's low 7 bits as data, and
 * FORMAT_8_BITS all 8. FORMAT_EVEN_PARITY and FORMAT_ODD_PARITY take 7 data
 * bits and a parity bit */
#define FORMAT_7_BITS 8
#define FORMAT_EVEN_PARITY 12
#define FORMAT_ODD_PARITY 13
#define FORMAT_8_BITS 16
/* FORMAT_MARK_PARITY takes 7 data bits and a parity bit of 1 */
#define FORMAT_MARK_PARITY 40

/* the dial-string interpreter and its wait-for-dial-tone timers */
extern Uint32 dial_timer, dial_timer2, last_digit_timer;
extern Uint8 *dial_string_ptr;
extern Uint8 dial_phase, dial_exec_phase, lcs_previous_state;
extern Uint8 dial_buffer[MODEM_DIAL_LENGTH + 2];
extern Uint8 mute_is_on, NoSilenceFor210ms;
extern Uint32 dt_timer, wfdt_timer, dt_valid_tim;
extern Uint8 v25_bis_com_result, PwrUpWithSpeed;

/* @hack spelling: the S-register's byte addressed by pointer addition,
 * where Sregisters[number] would do */
#define SREG(number) (*(Uint8 *)(Sregisters + (number)))
/* S14's bits. E1 sets S14_ECHO, under which the modem echoes the host's
 * command characters back. the modem dials by pulses under S14_PULSE, and
 * originates the call under S14_ORIGINATE */
#define S14_ECHO 0x02
#define S14_PULSE 0x20
#define S14_ORIGINATE 0x80

/* under S14_QUIET the modem sends no results, and under S14_QUIET_ANSWER
 * none for a call it answers. S14_VERBOSE sends each result as words, not
 * its number */
#define S14_QUIET 0x04
#define S14_VERBOSE 0x08
#define S14_QUIET_ANSWER 0x10
/* &Dn holds n in S21_D's bits, and S21_D2 is &D2's */
#define S21_D 0x18
#define S21_D1 0x08
#define S21_D2 0x10
#define S21_D3 0x18
/* under S21_LONG_SPACE_DISCONNECT, on a link without LAPM or MNP, a break of
 * 1.6 s or more from the line hangs up. on such a link, the modem sends a 4 s
 * break before it hangs up itself */
#define S21_LONG_SPACE_DISCONNECT 0x80
/* S22's fields. L sets S22_VOLUME, the speaker's volume, which
 * adj_spk_level takes at 0 as at 1. M sets S22_SPEAKER, whose 0 keeps the
 * speaker off. the dial turns the speaker on under S22_SPEAKER_UNTIL_CONNECTED
 * and S22_SPEAKER_ALWAYS. line establishment turns it on under any setting but
 * 0. the connection turns it off except under S22_SPEAKER_ALWAYS. X sets
 * S22_RESULTS. &P1 sets S22_FAST_PULSE, under which setup_in_pulse takes the
 * country's fast pulses */
#define S22_VOLUME 0x03
#define S22_SPEAKER 0x0c
#define S22_SPEAKER_UNTIL_CONNECTED 0x04
#define S22_SPEAKER_ALWAYS 0x08
#define S22_RESULTS 0x70
#define S22_FAST_PULSE 0x80
/* under S23_RDL, which &T4 sets and &T5 clears, the data pump grants a remote
 * digital loopback */
#define S23_RDL 0x01
/* S23's parity. under S23_SPACE_PARITY a byte's eighth bit is data */
#define S23_PARITY 0x30
#define S23_SPACE_PARITY 0x10
#define S23_ODD_PARITY 0x20
#define S23_MARK_PARITY 0x30
/* S23_GUARD holds the guard tone, where the country leaves the choice to S23:
 * S23_GUARD_FIRST, S23_GUARD_SECOND, or neither. shifted down, it is the tone
 * as dp_set_guard_tone numbers it */
#define S23_GUARD 0xc0
#define S23_GUARD_FIRST 0x40
#define S23_GUARD_SECOND 0x80
/* S24's bits. N1 sets S24_N1, N2 sets S24_N2, and N3 sets both, the bits of
 * S24_N. Wn holds n in S24_W's bits, S24_W1 and S24_W2. S24_FIXED_DTE keeps
 * the terminal's rate whatever the line's */
#define S24_N2 0x08
#define S24_N1 0x10
#define S24_N (S24_N1 | S24_N2)
#define S24_W1 0x20
#define S24_W2 0x40
#define S24_W (S24_W1 | S24_W2)
#define S24_FIXED_DTE 0x80
/* &Qn holds n in S27_Q's bits, and an n other than 0 is a synchronous mode */
#define S27_Q 0x03
/* &L1's bit of S27, a leased line, on which the modem answers no rings */
#define S27_LEASED_LINE 0x04
/* B1's bit of S27. under it, S37's setting 5 takes Bell's modulation in place
 * of V.22bis, and a setting apply_s37 has no case for takes it in place of
 * V.21 */
#define S27_BELL 0x40
/* under S31_EXTENDED, v25terReadCommand admits every V.25 ter command */
#define S31_EXTENDED 0x40
/* S39's bits. the line monitor reads S39_FALL_FORWARD before it steps the
 * rate up, and S39_FALL_BACK before it steps it down. it reads
 * S39_LINE_QUALITY before it answers a window of grades. &T9 sets
 * S39_TEST_INPUTS, and &T10 clears it */
#define S39_FALL_FORWARD 0x01
#define S39_FALL_BACK 0x02
/* the host's speed past S23's: S39_SPEED_115200, S39_SPEED_230400 or
 * S39_SPEED_460800 in S39_SPEED's bits, or none */
#define S39_SPEED 0x0c
#define S39_SPEED_115200 0x04
#define S39_SPEED_230400 0x08
#define S39_SPEED_460800 0x0c
/* the break handling \K sets in S39_BREAK's bits: S39_BREAK_EXPEDITED,
 * S39_BREAK_DESTRUCTIVE, which outranks it, or in sequence without either */
#define S39_BREAK 0x30
#define S39_BREAK_EXPEDITED 0x10
#define S39_BREAK_DESTRUCTIVE 0x20
#define S39_TEST_INPUTS 0x40
#define S39_LINE_QUALITY 0x80
/* S45 says why the last connection ended. the modem sets S45_HUNG_UP as it
 * hangs up itself, by ATH or as a call's setup times out, and S45_INACTIVE
 * once the terminal or the line has been idle too long. a bad codeword sets
 * ECDC_RESPONSE_BAD_CODEWORD. the line report shows S45_LOCAL_REQUEST, which
 * nothing sets, as a local request */
#define S45_LOCAL_REQUEST 1
#define S45_HUNG_UP 2
#define S45_INACTIVE 4
/* S52 holds bits 0 to 7 of dp_set_v34_parameters' parameters. its bit 7 is
 * AgressivityLevel 2 */
#define S52_AGGRESSIVITY_2 0x80
/* +MS's settings. S60 and S61 hold its modulation mask, low byte first. the
 * mask's highest bit is the carrier, and +MA lists the bits below it.
 * MS_MIN_TX to MS_MAX_RX hold rate codes, and a maximum of 0 is no limit.
 * +MS's automode is off while S66_NO_AUTOMODE is set */
#define MS_MODULATIONS (SREG(61) << 8 | SREG(60))
#define MS_MIN_TX SREG(62)
#define MS_MAX_TX SREG(63)
#define MS_MIN_RX SREG(64)
#define MS_MAX_RX SREG(65)
#define S66_NO_AUTOMODE 0x80

#endif
