#include <sg_xpt.h>
#include "bl.h"
#include "command.h"
#include "country.h"
#include "countrydb.h"
#include "dce.h"
#include "dialing.h"
#include "dte_vxd.h"
#include "ec_estab.h"
#include "ecdc_def.h"
#include "ecdcutil.h"
#include "line_estab.h"
#include "maintain.h"
#include "messages.h"
#include "modemdef.h"
#include "roc_cc.h"
#include "roc_k56.h"
#include "target.h"
#include "util_mod.h"
#include "v25ter.h"

#define MODULATION_BITS 0x3ff
#define ANY_RATE 0xff
/* the wait between answering a call and starting the handshake, and the
 * longest the handshake may take */
#define ANSWER_DELAY_TICKS (2 * TICKS_PER_SECOND)
#define HANDSHAKE_LIMIT_TICKS (60 * TICKS_PER_SECOND)
/* V.25's calling tone, which sounds and stops by turns */
#define CALLING_TONE_HZ 1300
#define CALLING_TONE_ON_TICKS 60
#define CALLING_TONE_OFF_TICKS 175
/* detect_answer_tone takes a tone for the answer once it sounds TONE_TICKS
 * without a break. 2100 Hz takes FIRST_ANSWER_TICKS until anything else is
 * heard, and the country's answer_tone_time after. 1300 Hz counts only while
 * speed_of_dce is 0 */
#define TONE_TICKS 30
#define FIRST_ANSWER_TICKS 25
#define ORIGINATING (SREG(14) & S14_ORIGINATE)
#define ASYNC_ORIGINATING (!(SREG(27) & S27_Q) && ORIGINATING)
#define DIRECT_LINK                                                            \
    ((ASYNC_ORIGINATING && (SREG(36) & 0x0f) == 1) ||                          \
     (!ORIGINATING && (SREG(36) & 0xf0) == 0x10))

static Uint32 modulations;
static Uint8 minimum_tx;
static Uint8 maximum_tx;
static Uint8 minimum_rx;
static Uint8 maximum_rx;

static void abort_call(Uint8 result);
static void begin_handshake(void);
static char rates_allowed(void);
static void get_rates(Uint32 *mask, Uint8 *min_tx, Uint8 *max_tx, Uint8 *min_rx,
                      Uint8 *max_rx, char update);
static void select_modulation(Uint32 *mask, Uint8 *max_tx, Uint8 *max_rx,
                              char update);
static Uint8 host_to_pump_rate(Uint8 speed);
static Uint32 apply_s37(Uint8 setting);
static Uint8 s37_rate(Uint8 setting);
static void start_calling_tone(void);
static void advance_calling_tone(void);
static void set_sync_mode(void);
#define GET_CONFIGURED_RATES(update)                                           \
    get_rates(&modulations, &minimum_tx, &maximum_tx, &minimum_rx,             \
              &maximum_rx, update)

void line_estab(void)
{
    /* @hack type: status held in a Uint16, where the Uint8 its calls return
     * would do */
    Uint16 status;
    if (dial_line_est_abort()) {
        SS41 = 0;
        abort_call(RESULT_NO_CARRIER);
        if (ORIGINATING) {
            BL_Condition(BL_EVENT_ABORTED);
        }
        return;
    }
    switch (line_estab_phase) {
    case ESTAB_BEGIN:
        GET_CONFIGURED_RATES(0);
        if (SREG(22) & S22_SPEAKER) {
            adj_spk_level();
        }
        BMR(7) &= ~BMR7_LINK_LOST;
        BMR(10) &= 0x6b;
        BMR(11) &= 0xf6;
        BMR(12) &= 0xfe;
        BMR(14) &= 0xef;
        BMR(16) &= BMR16_DTR_DROPPED | BMR16_DTR_ON;
        dce_ignore_rx_async_data(1);
        BMR(6) = (BMR(6) & 0xf8) | 9;
        if (!ORIGINATING) {
            setup_timer(&line_estab_timer, ANSWER_DELAY_TICKS);
            if (SREG(27) & S27_LEASED_LINE) {
                begin_handshake();
            } else {
                line_estab_phase = ESTAB_ANSWER_DELAY;
            }
        } else if (SREG(27) & S27_LEASED_LINE) {
            begin_handshake();
        } else {
            init_busy_ringback_detector();
            init_answer_tone_detector();
            line_estab_phase = ESTAB_DIAL_DELAY;
            setup_timer(&line_estab_timer, SREG(7) * TICKS_PER_SECOND);
        }
        break;
    case ESTAB_DIAL_DELAY:
        if (timer_expired(dial_timer)) {
            line_estab_phase = ESTAB_CALLING_TONE;
            start_calling_tone();
            if (mute_is_on) {
                li_set_mute(0);
                mute_is_on = 0;
            }
        }
        /* fall through */
    case ESTAB_CALLING_TONE:
        status = detect_busy_ringback();
        if (status == PROGRESS_BUSY) {
            abort_call(RESULT_BUSY);
            if (ORIGINATING) {
                BL_Condition(BL_EVENT_BUSY);
            }
            return;
        } else if (status == PROGRESS_CONGESTED) {
            abort_call(RESULT_CIRCUIT_BUSY);
            return;
        } else if (status == PROGRESS_UNOBTAINABLE) {
            abort_call(RESULT_BUSY);
            return;
        } else if (status == PROGRESS_RINGBACK) {
            send_coded_message(RESULT_RINGING, NULL);
        } else if (detect_answer_tone()) {
            begin_handshake();
            if (ORIGINATING) {
                BL_Condition(BL_EVENT_ANSWER_TONE);
            }
            return;
        }
        if (line_estab_phase == ESTAB_CALLING_TONE) {
            advance_calling_tone();
        }
        if (timer_expired(line_estab_timer)) {
            SREG(45) = S45_HUNG_UP;
            abort_call(RESULT_NO_CARRIER);
            if (ORIGINATING) {
                BL_Condition(BL_EVENT_NO_ANSWER);
            }
        }
        break;
    case ESTAB_ANSWER_DELAY:
        if (timer_expired(line_estab_timer)) {
            begin_handshake();
        }
        break;
    case ESTAB_HANDSHAKE:
        if (timer_expired(line_estab_timer)) {
            SREG(45) = S45_HUNG_UP;
            abort_call(RESULT_NO_CARRIER);
            if (ORIGINATING) {
                BL_Condition(BL_EVENT_HANDSHAKE_TIMEOUT);
            }
#ifdef NONMATCHING
#else
            /* @hack redundant: the empty else of the test of ORIGINATING */
            else {
            }
#endif
            break;
        }
        status = dp_monitor_handshake();
        if (status == TRAIN_FAILED) {
            abort_call(RESULT_NO_CARRIER);
            return;
#ifdef NONMATCHING
#else
            /* @hack redundant: the break after the return */
            break;
#endif
            /* @hack redundant: the test of TRAIN_PENDING, whose arm is empty */
        } else if (status == TRAIN_PENDING) {
        } else if (status == TRAIN_CONNECTED) {
            if (rates_allowed()) {
                line_estab_phase = ESTAB_CARRIER;
            } else {
                dp_start_sending_clear_down();
                line_estab_phase = ESTAB_CLEAR_DOWN;
                return;
            }
        }
        if (dp_get_dsr()) {
            vxd_dte_set_dsr(0);
        }
        break;
    case ESTAB_CLEAR_DOWN:
        if (dp_is_clear_down_sent()) {
            abort_call(RESULT_NO_CARRIER);
        }
        break;
    case ESTAB_CARRIER:
        max_connection_tx_speed = 0;
        max_connection_rx_speed = 0;
        no_connection_yet = 0;
        update_dce_speed();
        if (ORIGINATING) {
            BL_Condition(BL_EVENT_CONNECTED);
        }
        setup_timer(&invalid_data_timer, 0);
        if ((SREG(22) & S22_SPEAKER) != S22_SPEAKER_ALWAYS) {
            target_set_volume(0);
        }
        if ((ASYNC_ORIGINATING && (SREG(36) & 0x0c)) ||
            (!ORIGINATING && (SREG(36) & 0xc0))) {
            dce_ignore_rx_async_data(0);
            prepare_link();
        } else {
            line_estab_phase = ESTAB_DATA_DELAY;
        }
        break;
    case ESTAB_DATA_DELAY:
        if (timer_expired(invalid_data_timer)) {
            dce_ignore_rx_async_data(0);
            prepare_link();
        }
        break;
    }
}

static void abort_call(Uint8 result)
{
    go_off_line();
    send_coded_message(result, NULL);
    set_command_mode();
}
void set_fax_is_on(void)
{
    target_set_TCI_timer(100);
    timer_at_4k8 = 0;
}
static void begin_handshake(void)
{
    Uint8 level;
    dp_set_v8_ci(0);
    if (SREG(27) & S27_LEASED_LINE) {
        BMR(11) |= BMR11_LEASED_LINE;
        dp_disable_v25_answer_tone();
    }
    if (ORIGINATING) {
        dp_set_originate();
    } else {
        dp_set_answer();
    }
    select_dce_speed(0);
    if (SREG(27) & S27_LEASED_LINE) {
        level = SREG(58);
    } else {
        /* @hack type: the (Sint8) casts of both levels, where the registers'
         * Uint8 would do */
        level = (SREG(32) & 0x10) ? (Sint8)SREG(33) : (Sint8)SREG(35);
    }
    dp_set_tx_level(level);
    dp_set_ceq(!(SREG(49) & 0x80));
    if (SREG(23) & S23_RDL) {
        dp_enable_rdl();
    } else {
        dp_disable_rdl();
    }
    if (!(cur_cnt->options & COUNTRY_GUARD_TONE)) {
        if (SREG(23) & S23_GUARD_SECOND) {
            dp_set_guard_tone(GUARD_TONE_SECOND);
        } else if (SREG(23) & S23_GUARD_FIRST) {
            dp_set_guard_tone(GUARD_TONE_FIRST);
        } else {
            dp_set_guard_tone(GUARD_TONE_NONE);
        }
    } else if (cur_cnt->options & SECOND_GUARD_TONE) {
        dp_set_guard_tone(GUARD_TONE_SECOND);
    } else {
        dp_set_guard_tone(GUARD_TONE_FIRST);
    }
    if (SREG(27) & S27_Q) {
        set_sync_mode();
        if (SREG(31) & 4) {
            dp_set_v13();
        }
    }
    dp_init_handshake();
    line_estab_phase = ESTAB_HANDSHAKE;
    setup_timer(&line_estab_timer, HANDSHAKE_LIMIT_TICKS);
}

void select_dce_speed(char update)
{
    Uint32 v34;
    Uint8 v8;
    if (dp_mod_types & MOD_V34) {
        v8 = SREG(55);
        v34 = (SREG(53) << 8) + (SREG(54) << 16) + ((Uint32)SREG(50) << 24) +
              SREG(52);
        if ((ASYNC_ORIGINATING && (SREG(36) & 8)) ||
            (!ORIGINATING && (SREG(36) & 0x80))) {
            v8 |= 8;
        } else {
            v8 &= 0xf7;
        }
        if (DIRECT_LINK) {
            v34 &= ~V34_ALLOW_K56_V90;
        }
        dp_set_v8_parameters(v8);
        dp_set_v34_parameters(v34);
    }
    if (update) {
        GET_CONFIGURED_RATES(1);
    }
    speed_of_dce = lestConvertDpRate(maximum_tx);
    dp_set_modulation_type(modulations, minimum_tx, maximum_tx, minimum_rx,
                           maximum_rx);
}

Uint8 lestGetMinRate(void)
{
    return minimum_tx;
}
Uint8 lestGetMaxRate(void)
{
    return maximum_tx;
}
Uint8 lestGetMinRxRate(void)
{
    return minimum_rx;
}
Uint8 lestGetMaxRxRate(void)
{
    return maximum_rx;
}
Uint32 lestGetModulationMask(void)
{
    return modulations;
}
void lestUpdateModRates(void)
{
    GET_CONFIGURED_RATES(0);
}

static char rates_allowed(void)
{
    Uint32 negotiated;
    Uint8 available_rate;
    Uint8 tx;
    Uint8 rx;
    Uint32 available;
    Uint8 max_rx;
    Uint8 max_tx;
    dp_get_modulation_type(&negotiated, &tx, &rx);
    dp_get_avail_mod_type(&available, &available_rate);
    /* @hack type: the (Sint8) casts of the rates, where their own Uint8
     * would do */
    max_tx = maximum_tx ? (Sint8)maximum_tx : (Sint8)available_rate;
    max_rx = maximum_rx ? (Sint8)maximum_rx : (Sint8)available_rate;
    if (!(negotiated & modulations)) {
        if (negotiated != modulations || negotiated) {
            return 0;
        }
    }
    if (max_tx < tx || minimum_tx > tx) {
        return 0;
    }
    if (negotiated & (MOD_V34 | MOD_K56 | MOD_V90)) {
        if (max_rx < rx || minimum_rx > rx) {
            return 0;
        }
    }
    return 1;
}

static void get_rates(Uint32 *mask, Uint8 *min_tx, Uint8 *max_tx, Uint8 *min_rx,
                      Uint8 *max_rx, char update)
{
    Uint32 available;
    if ((SREG(24) & S24_N2) && (SREG(24) & S24_N1)) {
        *mask = MS_MODULATIONS;
        *min_tx = MS_MIN_TX;
        *min_rx = MS_MIN_RX;
        if (SREG(37) == ANY_RATE) {
            *max_tx = host_to_pump_rate(speed_of_AT);
            *max_rx = *max_tx;
            if (*max_tx > MS_MAX_TX && MS_MAX_TX > 0) {
                *max_tx = MS_MAX_TX;
            }
            if (*max_rx > MS_MAX_RX && MS_MAX_RX > 0) {
                *max_rx = MS_MAX_RX;
            }
        } else {
            *max_tx = MS_MAX_TX;
            *max_rx = MS_MAX_RX;
        }
        if (update || *mask <= (MOD_BELL | MOD_V21 | MOD_V23)) {
            v25terRemoveModNotAuthorised(mask, *min_tx, *max_tx, *min_rx,
                                         *max_rx);
            select_modulation(mask, max_tx, max_rx, update);
            *min_tx = *max_tx;
            *min_rx = *max_rx;
        } else {
            if (SREG(66) & S66_NO_AUTOMODE) {
                *mask = v25terFindMaxBit(*mask);
            }
            v25terRemoveModNotAuthorised(mask, *min_tx, *max_tx, *min_rx,
                                         *max_rx);
            if (*min_tx > *max_tx) {
                *min_tx = *max_tx;
            }
            if (*min_rx > *max_rx) {
                *min_rx = *max_rx;
            }
        }
    } else {
        available = dp_mod_types & MODULATION_BITS;
        if ((SREG(24) & S24_N1) && !(SREG(27) & S27_LEASED_LINE) && !update) {
            *mask = available;
            if (SREG(37) == ANY_RATE) {
                *max_tx = host_to_pump_rate(speed_of_AT);
            } else {
                *max_tx = 0;
            }
            *min_tx = 0;
            *min_rx = 0;
            *max_rx = *max_tx;
            v25terRemoveModNotAuthorised(mask, *min_tx, *max_tx, *min_rx,
                                         *max_rx);
        } else {
            if (SREG(37) == 0 || SREG(37) == ANY_RATE) {
                *mask = available;
                *max_tx = host_to_pump_rate(speed_of_AT);
            } else {
                *mask = apply_s37(SREG(37));
                *max_tx = s37_rate(SREG(37));
            }
            *max_rx = *max_tx;
            select_modulation(mask, max_tx, max_rx, update);
            if ((SREG(24) & S24_N2) && !update) {
                if (*mask) {
                    /* add each lower modulation the pump has */
                    *mask += (*mask - 1) & dp_mod_types;
                }
                *min_tx = 0;
                *min_rx = 0;
            } else {
                *min_tx = *max_tx;
                *min_rx = *max_rx;
            }
            v25terRemoveModNotAuthorised(mask, *min_tx, *max_tx, *min_rx,
                                         *max_rx);
        }
    }
}

#define CAP(rate, limit)                                                       \
    do {                                                                       \
        if (*(rate) > (limit)) {                                               \
            *(rate) = (limit);                                                 \
        }                                                                      \
    } while (0)
static void select_modulation(Uint32 *mask, Uint8 *max_tx, Uint8 *max_rx,
                              char update)
{
    Uint32 options = *mask & ~MODULATION_BITS;
    if (*max_tx == 0) {
        *max_tx = ANY_RATE;
    }
    if (*max_rx == 0) {
        *max_rx = ANY_RATE;
    }
    if ((*mask & MOD_V90) && *max_rx >= RATE_V90_RX_MIN && !update) {
        CAP(max_tx, RATE_V34_MAX);
        CAP(max_rx, RATE_V90_RX_MAX);
        *mask = MOD_V90;
    } else if ((*mask & MOD_K56) && *max_rx >= RATE_32000 && !update) {
        CAP(max_tx, RATE_V34_MAX);
        CAP(max_rx, RATE_V90_RX_MAX);
        *mask = MOD_K56;
    } else if ((*mask & MOD_V34) && *max_tx >= RATE_V34_MIN) {
        CAP(max_tx, RATE_V34_MAX);
        CAP(max_rx, RATE_V34_MAX);
        *mask = MOD_V34;
    } else if ((*mask & MOD_V32BIS) && *max_tx >= RATE_4800) {
        CAP(max_tx, RATE_14400);
        *mask = MOD_V32BIS;
    } else if ((*mask & MOD_V32) && *max_tx >= RATE_4800) {
        CAP(max_tx, RATE_9600);
        *mask = MOD_V32;
    } else if ((*mask & MOD_V22BIS) && *max_tx >= RATE_1200) {
        CAP(max_tx, RATE_2400);
        *mask = MOD_V22BIS;
    } else if ((*mask & MOD_V22) && *max_tx >= RATE_1200) {
        CAP(max_tx, RATE_1200);
        *mask = MOD_V22;
    } else if ((*mask & MOD_V23) && !update) {
        *mask = MOD_V23;
        if (ORIGINATING) {
            *max_tx = RATE_75;
            *max_rx = RATE_1200;
        } else {
            *max_tx = RATE_1200;
            *max_rx = RATE_75;
        }
    } else if (*mask & MOD_V21) {
        *max_tx = RATE_300;
        *mask = MOD_V21;
    } else {
        if (*max_tx > RATE_300) {
            *max_tx = RATE_1200;
        } else {
            *max_tx = RATE_300;
        }
        *mask = MOD_BELL;
    }
    *mask |= options;
}

/* the pump's rate code for speed, a TCI speed code, held to dp_max_rate.
 * TCI's 38400 bps takes rate code 37, as 38000 does. TCI's 56000 bps and up,
 * codes 45 to 54, take rate code 56. the switch has no case past 54, so a
 * higher speed code comes back as the rate code */
static Uint8 host_to_pump_rate(Uint8 speed)
{
    switch (speed) {
    case 0:
        speed = 1;
        break;
    case 1:
        speed = 2;
        break;
    case 2:
        speed = 4;
        break;
    case 3:
        speed = 5;
        break;
    case 4:
        speed = 6;
        break;
    case 5:
        speed = 7;
        break;
    case 6:
        speed = 8;
        break;
    case 7:
        speed = 11;
        break;
    case 8:
        speed = 12;
        break;
    case 9:
        speed = 13;
        break;
    case 10:
        speed = 14;
        break;
    case 11:
        speed = 21;
        break;
    case 12:
        speed = 22;
        break;
    case 13:
        speed = 23;
        break;
    case 14:
        speed = 24;
        break;
    case 15:
        speed = 25;
        break;
    case 16:
        speed = 26;
        break;
    case 17:
        speed = 28;
        break;
    case 18:
        speed = 29;
        break;
    case 19:
        speed = 30;
        break;
    case 20:
        speed = 31;
        break;
    case 21:
        speed = 32;
        break;
    case 22:
        speed = 33;
        break;
    case 23:
        speed = 34;
        break;
    case 24:
        speed = 35;
        break;
    case 25:
        speed = 36;
        break;
    case 26:
        speed = 37;
        break;
    case 27:
        speed = 37;
        break;
    case 28:
        speed = 39;
        break;
    case 29:
        speed = 40;
        break;
    case 30:
        speed = 41;
        break;
    case 31:
        speed = 42;
        break;
    case 32:
        speed = 43;
        break;
    case 33:
        speed = 44;
        break;
    case 34:
        speed = 45;
        break;
    case 35:
        speed = 46;
        break;
    case 36:
        speed = 47;
        break;
    case 37:
        speed = 48;
        break;
    case 38:
        speed = 49;
        break;
    case 39:
        speed = 50;
        break;
    case 40:
        speed = 51;
        break;
    case 41:
        speed = 52;
        break;
    case 42:
        speed = 53;
        break;
    case 43:
        speed = 54;
        break;
    case 44:
        speed = 55;
        break;
    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
        speed = 56;
        break;
    }
    if (speed > dp_max_rate) {
        return dp_max_rate;
    }
    return speed;
}

/* the first S37 setting of each modulation's range */
#define S37_V90 47
#define S37_K56 34
#define S37_V34 20
#define S37_V32BIS 13
/* return the modulations that setting, a value of S37, asks for. for V.23
 * the call also turns the calling tone off */
static Uint32 apply_s37(Uint8 setting)
{
    if (setting >= S37_V90) {
        return MOD_V90;
    } else if (setting >= S37_K56) {
        return MOD_K56;
    } else if (setting >= S37_V34) {
        return MOD_V34;
    } else if (setting >= S37_V32BIS) {
        return MOD_V32BIS;
    }
    switch (setting) {
    case 9:
        return MOD_TRELLIS | MOD_V32;
    case 7:
    case 10:
        return MOD_V32;
    case 12:
        BMR(8) |= BMR8_NO_CALLING_TONE;
        return MOD_V23;
#ifdef NONMATCHING
#else
        /* @hack redundant: the break after the return */
        break;
#endif
    case 5:
        if (SREG(27) & S27_BELL) {
            return MOD_BELL;
        }
        /* fall through */
    case 6:
        return MOD_V22BIS;
    default:
        if (SREG(27) & S27_BELL) {
            return MOD_BELL;
        }
        /* fall through */
    case 11:
        return MOD_V21;
    }
}

/* return the rate code setting asks for, or ANY_RATE for one with no rate */
static Uint8 s37_rate(Uint8 setting)
{
    Uint8 rate;
    switch (setting) {
    case 1:
    case 2:
    case 3:
    case 11:
        rate = 2;
        break;
    case 12:
        if (ORIGINATING) {
            rate = RATE_75;
        } else {
            rate = RATE_1200;
        }
        break;
    case 5:
        rate = 4;
        break;
    case 6:
    case 20:
        rate = 5;
        break;
    case 7:
    case 15:
    case 21:
        rate = 6;
        break;
    case 16:
    case 22:
        rate = 7;
        break;
    case 9:
    case 10:
    case 17:
    case 23:
        rate = 8;
        break;
    case 18:
    case 24:
        rate = 11;
        break;
    case 19:
    case 25:
        rate = 12;
        break;
    case 26:
        rate = 13;
        break;
    case 27:
        rate = 14;
        break;
    case 28:
        rate = 21;
        break;
    case 29:
        rate = 22;
        break;
    case 30:
        rate = 23;
        break;
    case 31:
        rate = 25;
        break;
    case 32:
        rate = 29;
        break;
    case 33:
        rate = 32;
        break;
    case 34:
    case 50:
        rate = 30;
        break;
    case 35:
        rate = 33;
        break;
    case 36:
    case 53:
        rate = 35;
        break;
    case 37:
        rate = 37;
        break;
    case 38:
    case 56:
        rate = 40;
        break;
    case 39:
        rate = 42;
        break;
    case 40:
    case 59:
        rate = 44;
        break;
    case 41:
        rate = 46;
        break;
    case 42:
    case 62:
        rate = 48;
        break;
    case 43:
        rate = 50;
        break;
    case 44:
    case 65:
        rate = 52;
        break;
    case 45:
        rate = 54;
        break;
    case 46:
    case 68:
        rate = 56;
        break;
    case 47:
        rate = 24;
        break;
    case 48:
        rate = 26;
        break;
    case 49:
        rate = 28;
        break;
    case 51:
        rate = 31;
        break;
    case 52:
        rate = 34;
        break;
    case 54:
        rate = 36;
        break;
    case 55:
        rate = 39;
        break;
    case 57:
        rate = 41;
        break;
    case 58:
        rate = 43;
        break;
    case 60:
        rate = 45;
        break;
    case 61:
        rate = 47;
        break;
    case 63:
        rate = 49;
        break;
    case 64:
        rate = 51;
        break;
    case 66:
        rate = 53;
        break;
    case 67:
        rate = 55;
        break;
    default:
        rate = ANY_RATE;
        break;
    }
    return rate;
}
Uint8 lestConvertDpRate(Uint8 rate)
{
    switch (rate) {
    case 56:
        return 45;
    case 55:
        return 44;
    case 54:
        return 43;
    case 53:
        return 42;
    case 52:
        return 41;
    case 51:
        return 40;
    case 50:
        return 39;
    case 49:
        return 38;
    case 48:
        return 37;
    case 47:
        return 36;
    case 46:
        return 35;
    case 45:
        return 34;
    case 44:
        return 33;
    case 43:
        return 32;
    case 42:
        return 31;
    case 41:
        return 30;
    case 40:
        return 29;
    case 39:
        return 28;
    case 37:
        return 26;
    case 36:
        return 25;
    case 35:
        return 24;
    case 34:
        return 23;
    case 33:
        return 22;
    case 32:
        return 21;
    case 31:
        return 20;
    case 30:
        return 19;
    case 29:
        return 18;
    case 28:
        return 17;
    case 26:
        return 16;
    case 25:
        return 15;
    case 24:
        return 14;
    case 23:
        return 13;
    case 22:
        return 12;
    case 21:
        return 11;
    case 14:
        return 10;
    case 13:
        return 9;
    case 12:
        return 8;
    case 11:
        return 7;
    case 8:
        return 6;
    case 7:
        return 5;
    case 6:
        return 4;
    case 5:
        return 3;
    case 4:
        return 2;
    case 2:
        return 1;
    }
    return 0;
}

void update_dce_speed(void)
{
    Uint32 mask;
    Uint8 tx;
    Uint8 rx;
    dp_get_modulation_type(&mask, &tx, &rx);
    last_connection_tx_speed = lestConvertDpRate(tx);
    speed_of_dce = last_connection_rx_speed = lestConvertDpRate(rx);
    if (last_connection_tx_speed > max_connection_tx_speed) {
        max_connection_tx_speed = last_connection_tx_speed;
    }
    if (last_connection_rx_speed > max_connection_rx_speed) {
        max_connection_rx_speed = last_connection_rx_speed;
    }
    if (mask & MOD_V23) {
        speed_of_dce = 0;
    }
}
void init_busy_ringback_detector(void)
{
    dp_set_handshake_tone_detector(cur_cnt->handshake_tones,
                                   CALL_PROGRESS_FILTER);
    country_InitCallProgress();
    busy_cntr = 0;
    BMR(2) &= 0x7f;
    busy_rb_timer = TIM_10MS;
}
Uint8 detect_busy_ringback(void)
{
    Uint8 state = dp_filter_test();
    Uint32 elapsed;
    if (state != previous_filter_state) {
        elapsed = TIM_10MS - busy_rb_timer;
        busy_rb_timer = TIM_10MS;
        previous_filter_state = state;
        if ((SS22 & SS22_DETECT_BUSY) && country_BusyTest(state, elapsed)) {
            return PROGRESS_BUSY;
        }
        if ((SS22 & SS22_DETECT_RINGBACK) &&
            country_RingbackTest(state, elapsed)) {
            return PROGRESS_RINGBACK;
        }
        if (country_CongestedTest(state, elapsed)) {
            return PROGRESS_CONGESTED;
        }
        if (cur_cnt->unobtainable.off1.maximum != 0 &&
            country_UnobtainableTest(state, elapsed)) {
            return PROGRESS_UNOBTAINABLE;
        }
    } else if (state && (cur_cnt->options & DETECT_UNOBTAINABLE) &&
               !cur_cnt->unobtainable.off1.maximum) {
        if (TIM_10MS - busy_rb_timer >= cur_cnt->unobtainable.on1.maximum) {
            return PROGRESS_UNOBTAINABLE;
        }
    }
    return PROGRESS_NONE;
}
void init_answer_tone_detector(void)
{
    setup_timer(&timer_1300hz, TONE_TICKS);
    setup_timer(&timer_2100hz, FIRST_ANSWER_TICKS);
    setup_timer(&timer_2225_2250hz, TONE_TICKS);
}
char detect_answer_tone(void)
{
    /* @hack type: tone held in a Uint16, where the Uint8
     * dp_answer_tone_filter_test returns would do */
    Uint16 tone = dp_answer_tone_filter_test();
    if (tone == HEARD_2100HZ) {
        if (timer_expired(timer_2100hz)) {
            return 1;
        }
#ifdef NONMATCHING
#else
        /* @hack redundant: the empty else of the 2100 Hz timer's test */
        else {
        }
#endif
        setup_timer(&timer_1300hz, TONE_TICKS);
        setup_timer(&timer_2225_2250hz, TONE_TICKS);
    } else if (tone == HEARD_2225HZ) {
        if (timer_expired(timer_2225_2250hz)) {
            return 1;
        }
        setup_timer(&timer_1300hz, TONE_TICKS);
        setup_timer(&timer_2100hz, cur_cnt->answer_tone_time);
    } else if (!speed_of_dce && tone == HEARD_1300HZ) {
        if (timer_expired(timer_1300hz)) {
            return 1;
        }
        setup_timer(&timer_2100hz, cur_cnt->answer_tone_time);
        setup_timer(&timer_2225_2250hz, TONE_TICKS);
    } else {
        setup_timer(&timer_2100hz, cur_cnt->answer_tone_time);
        setup_timer(&timer_1300hz, TONE_TICKS);
        setup_timer(&timer_2225_2250hz, TONE_TICKS);
    }
    return 0;
}
static void start_calling_tone(void)
{
    if ((cur_cnt->options & SEND_CALLING_TONE) &&
        !(BMR(8) & BMR8_NO_CALLING_TONE)) {
        dp_set_tx_level(SREG(35));
        dp_start_tone_generator(CALLING_TONE_HZ);
        setup_timer(&ct_timer, CALLING_TONE_ON_TICKS);
        calling_tone_on = 1;
    }
    if ((SREG(55) & 0x20) && (dp_mod_types & MOD_V34) && !calling_tone_on) {
        dp_set_v8_ci(2);
    } else {
        dp_set_v8_ci(0);
    }
}
static void advance_calling_tone(void)
{
    if ((cur_cnt->options & SEND_CALLING_TONE) &&
        !(BMR(8) & BMR8_NO_CALLING_TONE) && timer_expired(ct_timer)) {
        if (!calling_tone_on) {
            calling_tone_on = 1;
            dp_start_tone_generator(CALLING_TONE_HZ);
            setup_timer(&ct_timer, CALLING_TONE_ON_TICKS);
        } else {
            calling_tone_on = 0;
            dp_stop_tone_generator();
            setup_timer(&ct_timer, CALLING_TONE_OFF_TICKS);
        }
    }
}
static void set_sync_mode(void)
{
    if (SREG(27) & 0x20) {
        dp_set_sync_mode(2);
    } else if (SREG(27) & 0x10) {
        dp_set_sync_mode(1);
    } else {
        dp_set_sync_mode(0);
    }
}

void prepare_link(void)
{
    BMR(10) &= 0xfb;
    BMR(10) &= ~BMR10_DIRECT_MODE;
    if (SREG(27) & S27_Q) {
        BMR(10) |= 4;
        BMR(10) |= BMR10_DIRECT_MODE;
        BMR(16) |= 0x10;
        if (SREG(21) & 4) {
            vxd_dte_set_cts(0);
        } else {
            vxd_dte_set_cts(1);
        }
    } else if (DIRECT_LINK) {
        BMR(10) |= BMR10_DIRECT_MODE;
    } else {
        vxd_dte_set_cts(0);
    }
    send_carrier_message();
    cmd_response_buffer[0] = ECDC_RESPONSE_NONE;
    update_OIS();
    cmd_MAKE_LINK();
    maintain_phase = MAINTAIN_WAIT_CONNECT;
    target_set_TCI_timer(100);
    timer_at_4k8 = 0;
}
