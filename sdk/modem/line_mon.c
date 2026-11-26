#include <sg_xpt.h>

#include "command.h"
#include "data_tst.h"
#include "dce.h"
#include "ecdc_def.h"
#include "ecdcutil.h"
#include "line_estab.h"
#include "line_mon.h"
#include "messages.h"
#include "modemdef.h"
#include "roc_k56.h"
#include "util_mod.h"

/* while the monitor first samples, a window of grades whose sum passes
 * SNR_HIGH shows a bad line. after, one whose sum reaches it does. a window
 * sums to SNR_MAXIMUM where every grade is the worst */
#define SNR_HIGH 24
#define SNR_MAXIMUM 48

/* in setup_timer's 10 ms ticks */
#define GOOD_LINE_TICKS 6000
#define INITIAL_SAMPLE_TICKS 4
#define MONITOR_DELAY_TICKS 300
#define SAMPLE_TICKS 10
#define V22BIS_RETRAIN_TICKS 200
#define V22BIS_RETRAINS 3
#define RETRAIN_REQUEST_TICKS 300
#define V34_RETRAIN_REQUEST_TICKS 2300
#define RETRAIN_TICKS 6000
/* at most MOST_FALLBACKS steps down start within FALLBACK_WINDOW_TICKS of
 * the first. a retrain after MOST_RETRAINS within RETRAIN_WINDOW_TICKS of the
 * first hangs up instead */
#define FALLBACK_WINDOW_TICKS 2000
#define MOST_FALLBACKS 2
#define RETRAIN_WINDOW_TICKS 9000
#define MOST_RETRAINS 3
/* for FAST_RETRAIN_TICKS after a connection, V.90's retrains alternate with
 * fast retrains */
#define FAST_RETRAIN_TICKS 42000
/* every ERROR_SAMPLES samples, more than MOST_FRAME_ERRORS bad frames step
 * the rate down */
#define ERROR_SAMPLES 100
#define MOST_FRAME_ERRORS 10

/* S10 counts tenths of a second. its CARRIER_LOSS_OFF keeps the line up
 * without a carrier */
#define S10_TICKS 10
#define CARRIER_LOSS_OFF 0xff

/* set while the carrier holds. check_carrier_loss clears it as it starts
 * carrier_lost_timer */
#define BMR7_CARRIER_HELD 0x10
/* set after a window of the worst grades at 2400 bit/s or slower. the
 * monitor then takes the line as lost, carrier or not */
#define BMR11_LINE_LOST 0x01

struct receive_rate {
    Uint32 code;
    Uint32 capability;
};

/* each modulation's receive rates, slowest first. a rate holds its data-pump
 * rate code and its bit in the capabilities step_receive_rate reads. for V.34
 * and V.90 those are dp_get_rate_sequence's word. for V.32bis,
 * v32bis_capabilities moves the word's bits to the table's */
static const struct receive_rate v32bis_rates[] = {
    {6, 1}, {7, 2}, {8, 4}, {11, 8}, {12, 16}, {13, 32}, {14, 64}};
static const struct receive_rate v34_rates[] = {
    {5, 1},     {6, 2},     {7, 4},     {8, 8},    {11, 16},
    {12, 32},   {13, 64},   {14, 128},  {21, 256}, {22, 512},
    {23, 1024}, {25, 2048}, {29, 4096}, {32, 8192}};
static const struct receive_rate v90_rates[] = {
    {24, 1},       {26, 2},      {28, 4},      {30, 8},      {31, 16},
    {34, 32},      {35, 64},     {36, 128},    {39, 256},    {40, 512},
    {41, 1024},    {43, 2048},   {44, 4096},   {45, 8192},   {47, 16384},
    {48, 32768},   {49, 65536},  {51, 131072}, {52, 262144}, {53, 524288},
    {55, 1048576}, {56, 2097152}};

Uint8 snr_window[SNR_SAMPLES];
Uint8 snr_win_pnt;
Uint8 last_win_was_HIGH;
static Uint16 error_poll_count;
static Uint8 line_degraded;
/* tested as the carrier's return, but every store to it is a 0 */
static Uint8 carrier_resumed;
static Uint8 fast_retrain_next;
static Uint8 fast_retrain_done;
static Uint8 v90_fallback_requested;
static Uint8 reduce_retrain_rate;
static Uint32 fast_retrain_deadline;

static void check_carrier_loss(void);
static void discard_async_input(void);
static char select_higher_rate(void);
static char select_lower_rate(void);
static char step_receive_rate(Uint32 capabilities, char lower);
static Uint32 v32bis_capabilities(Uint32 pump_bits);
static void renegotiate_rate(void);
static void start_retrain(void);

void line_monitor(void)
{
    Uint8 transmit_rate;
    Uint8 receive_rate;
    Uint16 frame_errors;
    Uint16 sample;
    Uint16 total;
    Uint32 timeout_ticks;

    if (!(BMR(6) & (BMR6_DATA | BMR6_COMMAND))) {
        return;
    }
    switch (line_monitor_phase) {
    case LINE_RESET:
        v32_v34_fbw_timer = TIM_10MS;
        fb_cntr = 0;
        ffwd_suspended = 0;
        setup_timer(&fast_retrain_deadline, FAST_RETRAIN_TICKS);
        fast_retrain_next = 0;
        fast_retrain_done = 0;
        v90_fallback_requested = 0;
        reduce_retrain_rate = 1;
        dp_get_modulation_type(&present_modulation, &transmit_rate,
                               &initial_rx_rate);
        /* fall through */
    case LINE_SAMPLE_RESET:
        ffwd_on = 0;
        o1_cmd_retrain = 0;
        carrier_resumed = 0;
        remote_init_ret = 0;
        BMR(7) |= BMR7_CARRIER_HELD;
        line_monitor_phase = LINE_INITIAL_SAMPLES;
        mon_ret_phase = 0;
        setup_timer(&mse_good_timer, 0);
        setup_timer(&mse_pace_timer, INITIAL_SAMPLE_TICKS);
        dp_get_modulation_type(&present_modulation, &transmit_rate,
                               &present_rx_rate);
        snr_win_pnt = 0;
        for (sample = 0; sample < SNR_SAMPLES; sample++) {
            snr_window[sample] = 0;
        }
        last_win_was_HIGH = 0;
        line_degraded = 0;
        error_poll_count = 0;
        /* the read clears the count */
        dce_get_hdlc_rx_error_cnt();
        break;
    case LINE_INITIAL_SAMPLES:
        if (!timer_expired(mse_good_timer) && fb_cntr < 1) {
            if (timer_expired(mse_pace_timer)) {
                setup_timer(&mse_pace_timer, INITIAL_SAMPLE_TICKS);
                snr_window[snr_win_pnt++] = dp_snr_ratio();
                if (snr_win_pnt == SNR_SAMPLES) {
                    snr_win_pnt = 0;
                    for (sample = 0, total = 0; sample < SNR_SAMPLES;
                         sample++) {
                        total += snr_window[sample];
                    }
                    if (total > SNR_HIGH) {
                        line_degraded = 1;
                        if (select_lower_rate()) {
                            renegotiate_rate();
                        } else {
                            line_degraded = 0;
                        }
                    }
                }
            }
        } else {
            ffwd_on = 0;
            o1_cmd_retrain = 0;
            carrier_resumed = 0;
            remote_init_ret = 0;
            BMR(7) |= BMR7_CARRIER_HELD;
            line_monitor_phase = LINE_MONITORING;
            mon_ret_phase = 0;
            setup_timer(&mse_good_timer, GOOD_LINE_TICKS);
            setup_timer(&mse_pace_timer, MONITOR_DELAY_TICKS);
            dp_get_modulation_type(&present_modulation, &transmit_rate,
                                   &present_rx_rate);
            snr_win_pnt = 0;
            for (sample = 0; sample < SNR_SAMPLES; sample++) {
                snr_window[sample] = 0;
            }
            last_win_was_HIGH = 0;
            line_degraded = 0;
            error_poll_count = 0;
            /* the read clears the count */
            dce_get_hdlc_rx_error_cnt();
        }
        break;
    case LINE_MONITORING:
        if (dp_carrier_detect()) {
            if (dp_detect_retrain()) {
                if (present_modulation == MOD_V22BIS) {
                    line_monitor_phase = LINE_V22BIS_RETRAIN;
                } else {
                    line_monitor_phase = LINE_RETRAIN;
                }
                remote_init_ret = 1;
            } else {
                if (BMR(11) & BMR11_LINE_LOST) {
                    check_carrier_loss();
                } else {
                    if (timer_expired(WAIT_TIM)) {
                        dce_ignore_rx_async_data(0);
                    }
                    BMR(7) |= BMR7_CARRIER_HELD;
                }
                if (test_phase != TEST_IDLE) {
                    break;
                }
                if (!timer_expired(mse_pace_timer)) {
                    break;
                }
                setup_timer(&mse_pace_timer, SAMPLE_TICKS);
                error_poll_count++;
                if (error_poll_count >= ERROR_SAMPLES) {
                    error_poll_count = 0;
                    line_degraded = 1;
                    frame_errors = dce_get_hdlc_rx_error_cnt();
                    if (frame_errors > MOST_FRAME_ERRORS &&
                        select_lower_rate()) {
                        renegotiate_rate();
                        break;
                    }
                    line_degraded = 0;
                }
                snr_window[snr_win_pnt++] = dp_snr_ratio();
                if (snr_win_pnt == SNR_SAMPLES) {
                    snr_win_pnt = 0;
                    for (sample = 0, total = 0; sample < SNR_SAMPLES;
                         sample++) {
                        total += snr_window[sample];
                    }
                    if (total == 0) {
                        BMR(11) &= ~BMR11_LINE_LOST;
                        last_win_was_HIGH = 0;
                        if ((SREG(39) & S39_LINE_QUALITY) &&
                            select_higher_rate()) {
                            line_monitor_phase = LINE_RETRAIN;
                            dce_ignore_rx_async_data(1);
                            dp_init_rate_renegotiation(0, wish_rx_rate);
                            ffwd_on = 1;
                        }
                    } else if (total < SNR_HIGH) {
                        BMR(11) &= ~BMR11_LINE_LOST;
                        setup_timer(&mse_good_timer, GOOD_LINE_TICKS);
                        last_win_was_HIGH = 0;
                    } else if (speed_of_dce <= TCI_SPEED_2400) {
                        if (total == SNR_MAXIMUM) {
                            BMR(11) |= BMR11_LINE_LOST;
                            discard_async_input();
                        } else if (present_modulation == MOD_V22BIS &&
                                   (SREG(39) & S39_LINE_QUALITY)) {
                            line_monitor_phase = LINE_V22BIS_RETRAIN;
                            dp_init_retrain(RATE_2400, RATE_2400, 0, 0);
                        }
                    } else if (total != SNR_MAXIMUM) {
                        setup_timer(&mse_good_timer, GOOD_LINE_TICKS);
                        if (SREG(52) & S52_AGGRESSIVITY_2) {
                            last_win_was_HIGH = 0;
                            break;
                        } else if (!last_win_was_HIGH) {
                            last_win_was_HIGH = 1;
                        } else if (SREG(39) & S39_LINE_QUALITY) {
                            if (select_lower_rate()) {
                                renegotiate_rate();
                            } else {
                                start_retrain();
                            }
                        }
                    } else {
                        setup_timer(&mse_good_timer, GOOD_LINE_TICKS);
                        if ((SREG(52) & S52_AGGRESSIVITY_2) &&
                            !last_win_was_HIGH) {
                            last_win_was_HIGH = 1;
                        } else if ((SREG(39) & S39_LINE_QUALITY) &&
                                   select_lower_rate()) {
                            renegotiate_rate();
                        } else {
                            start_retrain();
                        }
                    }
                }
            }
        } else {
            check_carrier_loss();
        }
        break;
    case LINE_COMMAND_RETRAIN:
        if (present_modulation == MOD_V22BIS) {
            line_monitor_phase = LINE_V22BIS_RETRAIN;
            dp_init_retrain(RATE_2400, RATE_2400, 0, 0);
        } else {
            line_monitor_phase = LINE_RETRAIN;
            start_retrain();
        }
        o1_cmd_retrain = 1;
        carrier_resumed = 0;
        v32_v34_ret_cntr = 0;
        dce_ignore_rx_async_data(1);
        break;
    case LINE_V22BIS_RETRAIN:
        switch (mon_ret_phase) {
        case 0:
            setup_timer(&line_monitor_timer, V22BIS_RETRAIN_TICKS);
            ret_cntr = V22BIS_RETRAINS;
            mon_ret_phase = 1;
            break;
        case 1:
            if (timer_expired(line_monitor_timer)) {
                if (--ret_cntr == 0) {
                    line_monitor_phase = LINE_HANG_UP;
                } else {
                    setup_timer(&line_monitor_timer, V22BIS_RETRAIN_TICKS);
                    dp_init_retrain(RATE_2400, RATE_2400, 0, 0);
                }
            } else if (dp_monitor_retrain() == TRAIN_CONNECTED) {
                line_monitor_phase = LINE_RESUME;
            }
            break;
        }
        break;
    case LINE_RETRAIN:
        switch (mon_ret_phase) {
        case 0:
            if (present_modulation == MOD_V34 ||
                present_modulation == MOD_V90 ||
                present_modulation == MOD_K56) {
                timeout_ticks = V34_RETRAIN_REQUEST_TICKS;
            } else {
                timeout_ticks = RETRAIN_REQUEST_TICKS;
            }
            setup_timer(&line_monitor_timer, timeout_ticks);
            mon_ret_phase = 1;
            break;
        case 1:
            if (dp_monitor_retrain() == TRAIN_PENDING) {
                if (timer_expired(line_monitor_timer)) {
                    line_monitor_phase = LINE_HANG_UP;
                }
            } else {
                mon_ret_phase = 2;
                setup_timer(&line_monitor_timer, RETRAIN_TICKS);
            }
            break;
        case 2: {
            Uint16 status = dp_monitor_retrain();

            if (status == TRAIN_CONNECTED || status == TRAIN_RENEGOTIATED) {
                line_monitor_phase = LINE_RESUME;
            } else if (timer_expired(line_monitor_timer) ||
                       status == TRAIN_FAILED) {
                line_monitor_phase = LINE_HANG_UP;
            }
            break;
        }
        }
        break;
    case LINE_RESUME:
        update_dce_speed();
        update_OIS();
        cmd_OIS();
        dp_get_modulation_type(&present_modulation, &transmit_rate,
                               &receive_rate);
        if (!ffwd_on || carrier_resumed) {
            setup_timer(&WAIT_TIM, 50);
            if (o1_cmd_retrain || carrier_resumed) {
                umCarrierDetected();
                modem_connect();
                carrier_resumed = 0;
                o1_cmd_retrain = 0;
            }
            if (remote_init_ret == 1 && present_modulation != MOD_V34 &&
                present_modulation != MOD_V90 &&
                present_modulation != MOD_K56) {
                initial_rx_rate = receive_rate;
            }
        } else if (wish_rx_rate != receive_rate) {
            ffwd_suspended = 1;
        }
        line_monitor_phase = LINE_SAMPLE_RESET;
        break;
    case LINE_HANG_UP:
        if (SREG(10) != CARRIER_LOSS_OFF) {
            carrier_lost();
        } else {
            dce_ignore_rx_async_data(1);
            if (dp_carrier_detect()) {
                line_monitor_phase = LINE_SAMPLE_RESET;
            }
        }
        break;
    }
}

static void check_carrier_loss(void)
{
    discard_async_input();
    if (BMR(7) & BMR7_CARRIER_HELD) {
        BMR(7) &= ~BMR7_CARRIER_HELD;
        setup_timer(&carrier_lost_timer, SREG(10) * S10_TICKS);
    } else if (SREG(10) != CARRIER_LOSS_OFF &&
               timer_expired(carrier_lost_timer)) {
        SREG(45) = 0;
        line_monitor_phase = LINE_HANG_UP;
    }
}

static void discard_async_input(void)
{
    if (!(BMR(27) & BMR27_LAPM) && !(BMR(25) & 0x20)) {
        dce_ignore_rx_async_data(1);
        dce_clr_rx_buff();
    }
}

static char select_higher_rate(void)
{
    if ((SREG(39) & S39_FALL_FORWARD) && present_rx_rate < initial_rx_rate &&
        present_rx_rate < dp_max_rate && ffwd_suspended != 1) {
        if (!step_receive_rate(dp_get_rate_sequence(), 0)) {
            return 0;
        }
        if (present_modulation == MOD_V34 && wish_rx_rate &&
            wish_rx_rate > lestGetMaxRxRate()) {
            return 0;
        } else if (present_modulation == MOD_V32BIS && wish_rx_rate &&
                   wish_rx_rate > lestGetMaxRate()) {
            return 0;
        }
        if (timer_expired(mse_good_timer) || carrier_resumed) {
            setup_timer(&mse_good_timer, GOOD_LINE_TICKS);
            return 1;
        }
    }
    return 0;
}

static char select_lower_rate(void)
{
    char rate_change_enabled;
    Uint32 capabilities;
    Uint32 modulation;

    if ((SREG(24) & S24_N1) || (SREG(24) & S24_N2)) {
        rate_change_enabled = 1;
    } else {
        rate_change_enabled = 0;
    }
    if (!((SREG(39) & S39_FALL_BACK) && rate_change_enabled) &&
        !line_degraded) {
        return 0;
    }
    if (fb_cntr >= MOST_FALLBACKS && !timer_expired(v32_v34_fbw_timer)) {
        return 0;
    }
    if (SREG(15) && present_modulation == MOD_V90 && !fast_retrain_done) {
        v90_fallback_requested = 1;
        if (v32_v34_fbw_timer <= TIM_10MS) {
            setup_timer(&v32_v34_fbw_timer, FALLBACK_WINDOW_TICKS);
            fb_cntr = 1;
        } else {
            fb_cntr++;
        }
        return 0;
    } else {
        fast_retrain_done = 0;
    }
    modulation = present_modulation;
    if (modulation != MOD_V32BIS && modulation != MOD_V34 &&
        modulation != MOD_K56 && modulation != MOD_V90) {
        return 0;
    }
    capabilities = dp_get_rate_sequence();
    /* K56flex steps down 2000 bit/s at a time, from 56000 to 32000 */
    if (present_modulation == MOD_K56) {
        switch (present_rx_rate) {
        case 56:
            wish_rx_rate = 54;
            break;
        case 54:
            wish_rx_rate = 52;
            break;
        case 52:
            wish_rx_rate = 50;
            break;
        case 50:
            wish_rx_rate = 48;
            break;
        case 48:
            wish_rx_rate = 46;
            break;
        case 46:
            wish_rx_rate = 44;
            break;
        case 44:
            wish_rx_rate = 42;
            break;
        case 42:
            wish_rx_rate = 40;
            break;
        case 40:
            wish_rx_rate = 37;
            break;
        case 37:
            wish_rx_rate = 35;
            break;
        case 35:
            wish_rx_rate = 33;
            break;
        case 33:
            wish_rx_rate = 30;
            break;
        default:
            return 0;
        }
    } else if (!step_receive_rate(capabilities, 1)) {
        return 0;
    }
    if ((present_modulation == MOD_V34 || present_modulation == MOD_K56 ||
         present_modulation == MOD_V90) &&
        wish_rx_rate < lestGetMinRxRate()) {
        return 0;
    } else if (present_modulation == MOD_V32BIS &&
               wish_rx_rate < lestGetMinRate()) {
        return 0;
    }
    return 1;
}

static char step_receive_rate(Uint32 capabilities, char lower)
{
    Uint32 candidate;
    const struct receive_rate *rates;
    Uint8 count;
    Uint8 index;

    candidate = 0;
    switch (present_modulation) {
    case MOD_V32BIS:
        rates = v32bis_rates;
        count = sizeof(v32bis_rates) / sizeof(v32bis_rates[0]);
        capabilities = v32bis_capabilities(capabilities);
        break;
    case MOD_V34:
        rates = v34_rates;
        count = sizeof(v34_rates) / sizeof(v34_rates[0]);
        break;
    case MOD_V90:
        rates = v90_rates;
        count = sizeof(v90_rates) / sizeof(v90_rates[0]);
        break;
    default:
        return 0;
#ifdef NONMATCHING
#else
        /* @hack redundant: a break after the return, which nothing
         * reaches */
        break;
#endif
    }
    for (index = 0; index < count; index++) {
        if (present_rx_rate == rates[index].code) {
            candidate = rates[index].capability;
            break;
        }
    }
    if (count == index) {
        return 0;
    }
    if (lower) {
        while (candidate && !((candidate >>= 1) & capabilities)) {
        }
    } else {
        candidate = (candidate << 1) & capabilities;
    }
    if (!candidate) {
        return 0;
    }
    for (index = 0; index < count; index++) {
        if (candidate == rates[index].capability) {
            wish_rx_rate = rates[index].code;
            break;
        }
    }
    if (count == index) {
        return 0;
    }
    return 1;
}

static Uint32 v32bis_capabilities(Uint32 pump_bits)
{
    Uint32 rates;

    rates = 0;
    if (pump_bits & 0x20) {
        rates = 1;
    }
    if (pump_bits & 0x200) {
        rates |= 2;
    }
    if (pump_bits & 0x40) {
        rates |= 4;
    }
    if (pump_bits & 0x400) {
        rates |= 8;
    }
    if (pump_bits & 0x1000) {
        rates |= 0x10;
    }
    if (pump_bits & 0x2000) {
        rates |= 0x20;
    }
    if (pump_bits & 0x4000) {
        rates |= 0x40;
    }
    return rates;
}

static void renegotiate_rate(void)
{
    line_monitor_phase = LINE_RETRAIN;
    dp_init_rate_renegotiation(0, wish_rx_rate);
    if (v32_v34_fbw_timer <= TIM_10MS) {
        setup_timer(&v32_v34_fbw_timer, FALLBACK_WINDOW_TICKS);
        fb_cntr = 1;
    } else {
        fb_cntr++;
    }
    ffwd_suspended = 0;
}

static void start_retrain(void)
{
    Uint8 rate;

    if (v90_fallback_requested) {
        fast_retrain_done = 1;
        dp_init_V90_fast_retrain();
    }
    fb_cntr = 0;
    setup_timer(&v32_v34_fbw_timer, 0);
    if (present_modulation == MOD_V90 && !o1_cmd_retrain &&
        !timer_expired(fast_retrain_deadline)) {
        line_monitor_phase = LINE_RETRAIN;
        if (v32_v34_ret_cntr == MOST_RETRAINS &&
            !timer_expired(v32_v34_retw_timer)) {
            line_monitor_phase = LINE_HANG_UP;
            return;
        }
        if (fast_retrain_next) {
            fast_retrain_next = 0;
            if (!reduce_retrain_rate) {
                reduce_retrain_rate = 1;
                if (step_receive_rate(dp_get_rate_sequence(), 1)) {
                    rate = wish_rx_rate;
                } else {
                    rate = present_rx_rate;
                }
            } else {
                reduce_retrain_rate = 0;
                rate = initial_rx_rate;
            }
            dp_init_retrain(lestGetMinRate(), lestGetMaxRate(),
                            lestGetMinRxRate(), rate);
        } else {
            fast_retrain_next = 1;
            dp_init_V90_fast_retrain();
            return;
        }
        if (timer_expired(v32_v34_retw_timer)) {
            setup_timer(&v32_v34_retw_timer, RETRAIN_WINDOW_TICKS);
            v32_v34_ret_cntr = 1;
        } else {
            v32_v34_ret_cntr++;
        }
    } else if (v32_v34_ret_cntr == MOST_RETRAINS &&
               !timer_expired(v32_v34_retw_timer)) {
        line_monitor_phase = LINE_HANG_UP;
        return;
    } else {
        line_monitor_phase = LINE_RETRAIN;
        if ((SREG(24) & S24_N1) || (SREG(24) & S24_N2)) {
            if (present_modulation == MOD_V34 ||
                present_modulation == MOD_V90 ||
                present_modulation == MOD_K56) {
                dp_init_retrain(lestGetMinRate(), lestGetMaxRate(),
                                lestGetMinRxRate(), initial_rx_rate);
            } else {
                dp_init_retrain(lestGetMinRate(), initial_rx_rate, 0, 0);
            }
        } else {
            dp_init_retrain(present_rx_rate, present_rx_rate, present_rx_rate,
                            present_rx_rate);
        }
        if (timer_expired(v32_v34_retw_timer)) {
            setup_timer(&v32_v34_retw_timer, RETRAIN_WINDOW_TICKS);
            v32_v34_ret_cntr = 1;
        } else {
            v32_v34_ret_cntr++;
        }
    }
}
