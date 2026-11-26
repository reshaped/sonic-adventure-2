#include <sg_xpt.h>
#include "bl.h"
#include "country.h"
#include "countrydb.h"
#include "dialing.h"
#include "ecdc_def.h"
#include "ecdcutil.h"
#include "line_estab.h"
#include "messages.h"
#include "modemdef.h"
#include "roc_cc.h"
#include "roc_k56.h"
#include "target.h"
#include "util_mod.h"

#define PHASE_WAIT_HOOK 0
#define PHASE_WAIT_TONE 1
#define PHASE_EXECUTE 2
#define PHASE_COMPLETE 3
#define STEP_AUTO_PULSE 0
#define STEP_PAUSE 1
#define STEP_WAIT_SILENCE 2
#define STEP_HOOK_FLASH 3
#define STEP_WAIT_TONE 4
#define STEP_INTERDIGIT 6
#define STEP_NEXT 7
#define STEP_EARTH_RELAY 8
#define STEP_MUTE 9
#define STEP_UNMUTE 10
/* the dial string's @ waits for NO_SILENCE_TICKS of sound on the line, then
 * for QUIET_TICKS of silence */
#define NO_SILENCE_TICKS 22
#define QUIET_TICKS 500
#define ENERGY_LEVEL 25
#define PULSE_GAP_TICKS 75
#define DIAL_END_TICKS 75
#define DOLLAR_PAUSE_TICKS 50
/* a pulse's break and make together, which a fast pulse halves. a fast
 * pulse's pause between digits, also halved, is at least SHORTEST_FAST_PAUSE */
#define PULSE_PERIOD 100
#define SHORTEST_FAST_PAUSE 600
/* dp_dial_number's codes for * and #, after A to D's 10 to 13 */
#define DIGIT_STAR 14
#define DIGIT_HASH 15
#define NO_DIGIT 255
/* dp_filter_test's tone can drop out for a moment. detect_dialtone holds a
 * drop as tone for FILTER_HOLD ticks. only a drop that lasts that long is
 * silence. for country_DialtoneTest, the silence starts where the drop
 * began, FILTER_HOLD ticks back */
#define FILTER_HOLD 7

static Uint8 filtered_tone;
static Uint8 previous_tone;
static Uint32 interval_length;
static Uint32 last_transition;
static Uint8 previous_filter;
static char holding_silence;
static Uint32 hold_deadline;
static Uint32 transition_correction;

#define BEGIN_SILENCE_WAIT()                                                   \
    do {                                                                       \
        setup_timer(&dial_timer, NO_SILENCE_TICKS);                            \
        NoSilenceFor210ms = 0;                                                 \
        setup_timer(&dial_timer2, SREG(7) * TICKS_PER_SECOND);                 \
        init_busy_ringback_detector();                                         \
    } while (0)

Uint8 dialing(void)
{
    Uint8 result = DIAL_PENDING;
    Uint16 status;
    if (dial_line_est_abort()) {
        SS41 = 0;
        BL_Condition(0);
        return DIAL_ABORTED;
    }
    switch (dial_phase) {
    case PHASE_WAIT_HOOK:
        if (!(cur_cnt->options & ALLOW_TONE_DIALING)) {
            SREG(14) |= S14_PULSE;
        }
        if (!(cur_cnt->options & ALLOW_PULSE_DIALING)) {
            SREG(14) &= ~S14_PULSE;
            SS22 &= ~SS22_AUTO_PULSE;
        }
        if (!(cur_cnt->options & ALLOW_BLIND_DIALING)) {
            SS22 |= SS22_WAIT_DIALTONE;
        }
        if (!(cur_cnt->options & ALLOW_AUTO_PULSE)) {
            SS22 &= ~SS22_AUTO_PULSE;
        }
        if (!timer_expired(OFF_HOOK_TIM)) {
            break;
        }
        init_on_line();
        mute_is_on = 0;
        status = SREG(22) & S22_SPEAKER;
        if (status == S22_SPEAKER_UNTIL_CONNECTED ||
            status == S22_SPEAKER_ALWAYS) {
            adj_spk_level();
        }
        setup_timer(&last_digit_timer, 0);
        BMR(7) &= ~BMR7_HOST_BYTE;
        BMR(7) &= ~BMR7_DIALED;
        BMR(8) &= ~BMR8_NO_CALLING_TONE;
        BMR(16) &= ~0x80;
        if (BMR(1) & 1) {
            BMR(1) &= ~1;
        } else {
            SS41 = SREG(41);
        }
        init_dial_tone_detector(SREG(6), cur_cnt->initial_dialtone);
        dial_phase = PHASE_WAIT_TONE;
        dial_exec_phase = STEP_NEXT;
        break;
    case PHASE_WAIT_TONE:
        status = detect_dialtone();
        if (status == DIALTONE_FOUND) {
            if ((SREG(14) & S14_PULSE) && !(SS22 & SS22_AUTO_PULSE)) {
                setup_in_pulse();
            } else {
                setup_in_dtmf();
            }
            dp_set_dial_mode();
            dial_phase = PHASE_EXECUTE;
            dial_exec_phase = STEP_NEXT;
        } else if (status == DIALTONE_NONE) {
            result = DIAL_NO_DIALTONE;
        }
        if (!((SREG(22) & 0x40) && (SREG(22) & 0x20) && !(SREG(22) & 0x10))) {
            if (detect_busy_ringback() == PROGRESS_BUSY) {
                SS41 = 0;
                result = DIAL_BUSY;
            }
        }
        break;
    case PHASE_EXECUTE:
        if (!dp_ready_to_dial()) {
            if (BMR(18) & BMR18_PULSE) {
                setup_timer(&last_digit_timer, PULSE_GAP_TICKS);
            } else if (cur_cnt->tone_on > 0) {
                setup_timer(&last_digit_timer, cur_cnt->tone_off / 8U);
            } else {
                setup_timer(&last_digit_timer, SREG(11) / 8U);
            }
            return DIAL_PENDING;
        }
        switch (dial_exec_phase) {
        case STEP_AUTO_PULSE:
            if (!(BMR(7) & BMR7_DIALED)) {
                BMR(7) |= BMR7_DIALED;
                init_dial_tone_detector(1, cur_cnt->initial_dialtone);
            } else {
                status = detect_dialtone();
                if (status == DIALTONE_FOUND) {
                    setup_in_pulse();
                    dial_string_ptr--;
                    dial_exec_phase = STEP_NEXT;
                } else if (status == DIALTONE_NONE) {
                    dial_exec_phase = STEP_NEXT;
                }
            }
            break;
        case STEP_PAUSE:
            if (timer_expired(dial_timer)) {
                dial_exec_phase = STEP_NEXT;
            }
            break;
        case STEP_EARTH_RELAY:
            if (timer_expired(dial_timer)) {
                target_set_earth_relay(0);
                dial_exec_phase = STEP_NEXT;
            }
            break;
        case STEP_WAIT_SILENCE:
            if (mute_is_on) {
                if (timer_expired(dial_timer)) {
                    stop_mute();
                    BEGIN_SILENCE_WAIT();
                }
            } else {
                if (NoSilenceFor210ms == 1) {
                    /* @hack type: the (char) cast of dp_energy_on_line's
                     * result, where a plain test would do */
                    if ((char)dp_energy_on_line(ENERGY_LEVEL)) {
                        setup_timer(&dial_timer, QUIET_TICKS);
                    } else if (timer_expired(dial_timer)) {
                        dial_exec_phase = STEP_NEXT;
                        break;
                    }
                    if (detect_busy_ringback() == PROGRESS_BUSY) {
                        SS41 = 0;
                        result = DIAL_BUSY;
                        break;
                    }
                } else {
                    /* @hack type: the (char) cast of dp_energy_on_line's
                     * result, where a plain test would do */
                    if (!(char)dp_energy_on_line(ENERGY_LEVEL)) {
                        setup_timer(&dial_timer, NO_SILENCE_TICKS);
                    } else if (timer_expired(dial_timer)) {
                        NoSilenceFor210ms = 1;
                        setup_timer(&dial_timer, QUIET_TICKS);
                    }
                }
                if (timer_expired(dial_timer2)) {
                    if (SS22 & SS22_EXTENDED) {
                        result = DIAL_NO_ANSWER;
                    } else {
                        result = DIAL_NO_CARRIER;
                    }
                }
            }
            break;
        case STEP_HOOK_FLASH:
            if (timer_expired(last_digit_timer)) {
                if (!timer_expired(dial_timer)) {
                    break;
                }
                if (BMR(9) & BMR9_OFF_HOOK) {
                    if (BMR(2) & BMR2_HOOK_FLASHED) {
                        dial_exec_phase = STEP_NEXT;
                        break;
                    }
                    BMR(9) &= ~BMR9_OFF_HOOK;
                    li_set_hook(1);
                    dp_on_hook();
                    BMR(2) |= BMR2_HOOK_FLASHED;
                } else {
                    BMR(9) |= BMR9_OFF_HOOK;
                    li_set_hook(0);
                    dp_off_hook();
                }
            }
            setup_timer(&dial_timer, cur_cnt->hook_flash_time);
            break;
        case STEP_WAIT_TONE:
            if (mute_is_on) {
                if (timer_expired(dial_timer)) {
                    stop_mute();
                    if (cur_cnt->reset_flags & W_WAITS_S6) {
                        init_dial_tone_detector(SREG(6),
                                                cur_cnt->subsequent_dialtone);
                    } else {
                        init_dial_tone_detector(SREG(7),
                                                cur_cnt->subsequent_dialtone);
                    }
                }
            } else {
                status = detect_dialtone();
                if (status == DIALTONE_FOUND) {
                    dial_exec_phase = STEP_NEXT;
                } else if (status == DIALTONE_NONE) {
                    if (SS22 & SS22_EXTENDED) {
                        result = DIAL_NO_DIALTONE;
                    } else {
                        result = DIAL_NO_CARRIER;
                    }
                }
                if (detect_busy_ringback() == PROGRESS_BUSY) {
                    SS41 = 0;
                    result = DIAL_BUSY;
                }
            }
            break;
        case STEP_MUTE:
            if (timer_expired(dial_timer)) {
                dial_exec_phase = STEP_NEXT;
            }
            break;
        case STEP_UNMUTE:
            if (timer_expired(dial_timer)) {
                dial_exec_phase = STEP_NEXT;
            }
            stop_mute();
            break;
        case STEP_INTERDIGIT:
            /* @bug logic: the test is reversed. the step moves on while
             * last_digit_timer runs, which is when execute_dial_string's T
             * enters it. a pass that finds the timer run out stays, and leaves
             * the dial string stuck there. the test is at 0x0cb0194e */
            if (!timer_expired(last_digit_timer)) {
                dial_exec_phase = STEP_NEXT;
            }
            break;
        case STEP_NEXT:
            switch (execute_dial_string()) {
            case DIAL_DONE:
                dial_phase = PHASE_COMPLETE;
                break;
            case DIAL_COMMAND:
                result = DIAL_COMMAND;
                break;
            }
            break;
        }
        break;
    case PHASE_COMPLETE:
        result = DIAL_DONE;
        break;
    }
    return result;
}

Uint8 execute_dial_string(void)
{
    Uint8 result = DIAL_PENDING;
    Uint8 digit;
    switch (*dial_string_ptr) {
    case ' ':
    case '-':
    case '.':
        break;
    case 'T':
        if ((BMR(7) & BMR7_DIALED) && (SREG(14) & S14_PULSE) &&
            !timer_expired(last_digit_timer)) {
            dial_exec_phase = STEP_INTERDIGIT;
        }
        if (!(cur_cnt->options & ALLOW_TONE_DIALING)) {
            break;
        }
        setup_in_dtmf();
        BMR(7) |= BMR7_DIALED;
        SREG(14) &= ~S14_PULSE;
        break;
    case 'P':
        if (!(cur_cnt->options & ALLOW_PULSE_DIALING)) {
            break;
        }
        setup_in_pulse();
        BMR(7) |= BMR7_DIALED;
        SREG(14) |= S14_PULSE;
        break;
    case 'R':
        SREG(14) &= ~S14_ORIGINATE;
        break;
    case ',':
        setup_timer(&dial_timer, SREG(8) * TICKS_PER_SECOND);
        dial_exec_phase = STEP_PAUSE;
        break;
    case '>':
        if (cur_cnt->reset_flags & EARTH_RELAY_DIALING) {
            setup_timer(&dial_timer, SREG(59));
            target_set_earth_relay(1);
            dial_exec_phase = STEP_EARTH_RELAY;
        } else {
            dial_exec_phase = STEP_NEXT;
        }
        break;
    case '=':
        setup_timer(&dial_timer, SREG(8) * (2 * TICKS_PER_SECOND));
        dial_exec_phase = STEP_PAUSE;
        break;
    case ';':
        BMR(16) |= 0x80;
        BMR(16) |= BMR16_RESUME_LINE;
        SS41 = 0;
        result = DIAL_COMMAND;
        break;
    case '@':
        if (mute_is_on) {
            setup_timer(&dial_timer, cur_cnt->unmute_delay);
        } else {
            BEGIN_SILENCE_WAIT();
        }
        dial_exec_phase = STEP_WAIT_SILENCE;
        break;
    case '!':
        if (cur_cnt->reset_flags & IGNORE_HOOK_FLASH) {
            break;
        }
        BMR(2) &= ~BMR2_HOOK_FLASHED;
        setup_timer(&dial_timer, cur_cnt->hook_flash_time);
        dial_exec_phase = STEP_HOOK_FLASH;
        break;
    case 'W':
        if (mute_is_on) {
            setup_timer(&dial_timer, cur_cnt->unmute_delay);
        } else if (cur_cnt->reset_flags & W_WAITS_S6) {
            init_dial_tone_detector(SREG(6), cur_cnt->subsequent_dialtone);
        } else {
            init_dial_tone_detector(SREG(7), cur_cnt->subsequent_dialtone);
        }
        dial_exec_phase = STEP_WAIT_TONE;
        break;
    case '^':
        if (!(cur_cnt->options & KEEP_CALLING_TONE)) {
            BMR(8) |= BMR8_NO_CALLING_TONE;
        }
        break;
    case '$':
        setup_timer(&dial_timer, DOLLAR_PAUSE_TICKS);
        if ((BMR(7) & BMR7_DIALED) && (SREG(14) & S14_PULSE) &&
            !timer_expired(last_digit_timer)) {
            dial_exec_phase = STEP_INTERDIGIT;
        }
        setup_in_dtmf();
        BMR(7) |= BMR7_DIALED;
        SREG(14) &= ~S14_PULSE;
        dial_exec_phase = STEP_PAUSE;
        break;
    default:
        if (*dial_string_ptr == S3_THEN) {
            if (mute_is_on) {
                setup_timer(&dial_timer, cur_cnt->unmute_delay);
            } else {
                setup_timer(&dial_timer, DIAL_END_TICKS);
            }
            result = DIAL_DONE;
        } else {
            digit = *dial_string_ptr;
            if (digit >= '0' && digit <= '9') {
                digit -= '0';
            } else if (!(BMR(18) & BMR18_PULSE)) {
                if (digit >= 'A' && digit <= 'D') {
                    digit -= 'A' - 10;
                } else if (digit == '*') {
                    digit = DIGIT_STAR;
                } else if (digit == '#') {
                    digit = DIGIT_HASH;
                } else {
                    digit = NO_DIGIT;
                }
            } else {
                digit = NO_DIGIT;
            }
            if (digit != NO_DIGIT) {
                dialing_send_digit(digit);
            }
        }
        break;
    }
    dial_string_ptr++;
    return result;
}

void dialing_send_digit(Uint8 number)
{
    /* @hack spelling: BMR(7) reached through status, where BMR(7) itself would
     * do */
    volatile Uint8 *status;
    Uint8 digit;
    /* @hack spelling: pulse in place of BMR18_PULSE, and of SS22_AUTO_PULSE,
     * which has the same value */
    const int pulse = BMR18_PULSE;
#ifdef NONMATCHING
    /* this side ignores a digit the dialer has no signal for: past 9 in pulse
     * mode, and past DIGIT_HASH in tone mode */
    if (number > ((BMR(18) & pulse) ? 9 : DIGIT_HASH)) {
        return;
    }
#endif
    /* @hack spelling: digit set by a comma inside the test, where a statement
     * of its own would do */
    if ((digit = number), (cur_cnt->options & MUTE_PULSE_DIALING)) {
        if (BMR(18) & pulse) {
            if (!mute_is_on) {
                li_set_mute(1);
                mute_is_on = 1;
                setup_timer(&dial_timer, cur_cnt->mute_delay);
                dial_exec_phase = STEP_MUTE;
            }
        } else if (mute_is_on) {
            setup_timer(&dial_timer, cur_cnt->unmute_delay);
            dial_exec_phase = STEP_UNMUTE;
        }
    }
#ifdef NONMATCHING
#else
    else {
        /* @hack redundant: an else holding a dead test of cur_cnt */
        if (0) {
            /* NOLINTNEXTLINE(misc-redundant-expression) */
            if (cur_cnt && cur_cnt) {
            }
        }
    }
#endif
    if (BMR(18) & pulse) {
        digit = country_convert_pulse_format(digit);
    }
    dp_dial_number(digit);
    status = &BMRregisters[7];
    if (!(*status & BMR7_DIALED)) {
        if (SS22 & pulse) {
            dial_exec_phase = STEP_AUTO_PULSE;
        } else {
            *status |= BMR7_DIALED;
        }
    }
}

char dial_line_est_abort(void)
{
    if (!target_get_TD()) {
        return 1;
    }
    if ((BMR(7) & BMR7_HOST_BYTE) && !(BMR(11) & BMR11_LEASED_LINE) &&
        !(BMR(4) & 0x10) && !(BMR(6) & BMR6_DATA)) {
        /* @hack spelling: the second test's S31 read through a cast of
         * &Sregisters, where SREG(31) would do */
        if (SREG(14) & S14_ORIGINATE) {
            return 1;
        } else if (!(((const Uint8 *)&Sregisters)[31] & 8)) {
            return 1;
        }
    }
    return 0;
}

void dialing_error(Uint8 result)
{
    switch (result) {
    case DIAL_ABORTED:
        result = RESULT_OK;
        break;
    case DIAL_NO_CARRIER:
        result = RESULT_NO_CARRIER;
        break;
    case DIAL_NO_DIALTONE:
        result = RESULT_NO_DIALTONE;
        break;
    case DIAL_BUSY:
        result = RESULT_BUSY;
        break;
    case DIAL_NO_ANSWER:
        result = RESULT_NO_ANSWER;
        break;
    case DIAL_CIRCUIT_BUSY:
        result = RESULT_CIRCUIT_BUSY;
        break;
    }
    send_coded_message(result, NULL);
}

void setup_in_pulse(void)
{
    Uint16 pause;
    Uint16 on;
    Uint16 off = PULSE_PERIOD;
    if (SREG(22) & S22_FAST_PULSE) {
        on = cur_cnt->fast_pulse_break;
        off -= cur_cnt->fast_pulse_break;
        on /= 2;
        off /= 2;
        pause = cur_cnt->fast_pulse_pause;
        pause =
            pause / 2 < SHORTEST_FAST_PAUSE ? SHORTEST_FAST_PAUSE : pause / 2;
    } else {
        on = cur_cnt->pulse_break;
        off -= cur_cnt->pulse_break;
        pause = cur_cnt->pulse_pause;
    }
    if (cur_cnt->reset_flags & STRETCH_PULSES) {
        on = on + (((on * 6 / 10) + 5) / 10);
        off = off + (((off * 6 / 10) + 5) / 10);
    }
    dp_set_make_break_ratio(on, off, pause);
    dp_set_pulse();
    BMR(18) |= BMR18_PULSE;
}

void setup_in_dtmf(void)
{
    BMR(18) &= ~BMR18_PULSE;
    dp_set_dtmf(cur_cnt->tone_level, cur_cnt->tone_table);
    if (cur_cnt->tone_on > 0) {
        dp_set_dtmf_duration(cur_cnt->tone_on, cur_cnt->tone_off);
    } else {
        dp_set_dtmf_duration(SREG(11), SREG(11));
    }
}

void stop_mute(void)
{
    li_set_mute(0);
    mute_is_on = 0;
}

void init_dial_tone_detector(Uint8 seconds, Uint32 duration)
{
    dp_set_handshake_tone_detector(cur_cnt->handshake_tones, 0);
    if (cur_cnt->dialtone.on1.maximum > 0) {
        country_InitDialtoneDetection();
    }
    dt_valid_tim = duration;
    setup_timer(&dt_timer, dt_valid_tim);
    if (!(SS22 & SS22_WAIT_DIALTONE) &&
        (cur_cnt->reset_flags & BLIND_DIAL_DELAY)) {
        setup_timer(&wfdt_timer, cur_cnt->blind_dial_delay);
    } else {
        setup_timer(&wfdt_timer, seconds * TICKS_PER_SECOND);
    }
}
Uint8 detect_dialtone(void)
{
    if (BMR(16) & 2) {
        BMR(16) &= ~2;
        return DIALTONE_FOUND;
    }
    if (dial_exec_phase != STEP_WAIT_TONE) {
        if ((SREG(27) & S27_LEASED_LINE) || (SREG(27) & S27_Q) == 3) {
            return DIALTONE_FOUND;
        }
        if (!(SS22 & SS22_WAIT_DIALTONE) ||
            (dt_valid_tim == 0 && !(cur_cnt->dialtone.on1.maximum > 0))) {
            if (timer_expired(wfdt_timer)) {
                return DIALTONE_FOUND;
            } else {
                return DIALTONE_WAITING;
            }
        }
    }
    filtered_tone = dp_filter_test();
    if (filtered_tone == 0 && previous_filter == 1) {
        setup_timer(&hold_deadline, FILTER_HOLD);
        filtered_tone = 1;
        previous_filter = 0;
        holding_silence = 1;
        transition_correction = 0;
    } else if (filtered_tone == 0 && previous_filter == 0 && holding_silence) {
        if (timer_expired(hold_deadline)) {
            filtered_tone = 0;
            previous_filter = 0;
            holding_silence = 0;
            transition_correction = FILTER_HOLD;
        } else {
            filtered_tone = 1;
            transition_correction = 0;
        }
    } else {
        previous_filter = filtered_tone;
        holding_silence = 0;
        transition_correction = 0;
    }
    if (cur_cnt->dialtone.on1.maximum > 0) {
        if (filtered_tone != previous_tone) {
            interval_length =
                TIM_10MS - last_transition - transition_correction;
            last_transition = TIM_10MS - transition_correction;
            previous_tone = filtered_tone;
            if (country_DialtoneTest(filtered_tone, interval_length)) {
                return DIALTONE_FOUND;
            }
        }
    }
    if (dt_valid_tim > 0) {
        if (!holding_silence) {
            if (filtered_tone) {
                if (timer_expired(dt_timer)) {
                    return DIALTONE_FOUND;
                }
            } else {
                setup_timer(&dt_timer, dt_valid_tim);
            }
        }
    }
    if (timer_expired(wfdt_timer)) {
        return DIALTONE_NONE;
    }
    return DIALTONE_WAITING;
}
