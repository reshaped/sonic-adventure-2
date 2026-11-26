/* the V.90, K56flex and V.34 monitors sum the pump's EQM readings. each
 * programs the receive rate its sum allows, and a lower sum allows a higher
 * rate */
#include "roc_wa.h"
#include <sg_xpt.h>
#include "ecdcutil.h"
#include "roc_k56.h"
#include "target.h"

/* words of the pump's RAM, beside those roc_k56.h names */
#define RAM_MODULATION_ENABLE 0x401
#define RAM_REMOTE_BIT_MASK 0x6ea
#define RAM_TRAINING_PHASE 0x4d
#define RAM_LEVEL_PEAK 0xa63
#define RAM_TRAINING_TABLE 0x3b0
#define RAM_WA5_COEFFICIENT 0x840

/* RAM_BAUD's indexes, one for each V.34 symbol rate */
#define SYMBOL_RATES 6

#define TRAINING_COEFFICIENTS 15

/* the V.90 monitor shifts a sum of V90_EQM_READINGS readings right by
 * V90_EQM_SHIFT. the K56flex and V.34 monitors shift a sum of EQM_READINGS by
 * EQM_SHIFT, which scales it alike */
#define V90_EQM_READINGS 400
#define V90_EQM_SHIFT 11
#define EQM_READINGS 50
#define EQM_SHIFT 8

/* the bit of V90GetRemoteCaps' word for a V.90 receive rate's index */
#define V90_RATE_BIT(index) (1 << ((index) - 1))

/* in setup_timer's 10 ms ticks */
#define EQM_READ_TICKS 50
#define K56_FALLBACK_TICKS 10
#define V34_STALL_TICKS 800
#define V34_RECOVERY_TICKS 400
#define V34_RENEGOTIATION_TICKS 500
#define V90_RENEGOTIATION_TICKS 200

/* the codes the monitors report to data_pump_error as an EQM reading times
 * out */
#define DP_ERR_V90_EQM 4100
#define DP_ERR_K56_EQM 4120
#define DP_ERR_V34_EQM 4140

/* rcdpi->r17 holds the pump's state as a connection trains. its R17_STAGE
 * bits are R17_RETRAIN as a retrain starts, and R17_RENEGOTIATION through a
 * rate renegotiation */
#define R17_STAGE 0xf0
#define R17_RETRAIN 0x20
#define R17_RENEGOTIATION 0x80
#define R17_TRAIN_SELECT 0x65
#define R17_TRAIN_WRITE 0x67
#define R17_RENEGOTIATE_SELECT 0x85
#define R17_RENEGOTIATE_WRITE 0x87
/* Wa5State's values. WA5_WAIT waits for r17's R17_TRAIN_SELECT, or
 * R17_RENEGOTIATE_SELECT in a renegotiation, and then runs
 * select_wa5_coefficient. WA5_CHOSEN waits for the matching _WRITE state to
 * write the coefficient chosen. WA5_DONE waits for r17 to leave the _SELECT
 * state. under WA5_OFF nothing is written */
#define WA5_WAIT 0
#define WA5_CHOSEN 1
#define WA5_DONE 2
#define WA5_OFF 3

/* the bytes write_training_table writes from RAM_TRAINING_TABLE up, a row
 * for each of RAM_BAUD's indexes */
static const Uint8 training_coefficients[SYMBOL_RATES][TRAINING_COEFFICIENTS] =
    {
        {32, 12, 7, 3, 2, 1, 0, 53, 5, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {32, 16, 7, 4, 2, 1, 0, 160, 64, 16, 3, 1, 0, 0, 0},
        {32, 22, 13, 8, 3, 2, 1, 0, 160, 80, 3, 2, 1, 0, 0},
        {32, 24, 16, 10, 8, 5, 3, 2, 1, 0, 48, 2, 2, 1, 0},
        {32, 24, 16, 10, 8, 5, 3, 2, 1, 0, 74, 21, 3, 1, 1},
};

static Uint8 remote_bit_count;
static Uint8 half_remote_bit_count;
static Uint8 high_level_ratio;
static Uint8 v34_status;
/* each unused_ variable fills the bss at the offset its name gives. nothing
 * reads or writes one */
static Uint32 unused_04;
static Uint32 v34_eqm_sum;
static Uint32 k56_eqm_sum;
static Uint32 v90_eqm_sum;
static Uint32 k56_fallback_deadline;
static Uint32 remote_caps;
static Uint32 level_ratio;
static Uint8 rate_backoff;
static Uint8 renegotiation_state;
static Uint8 last_v34_tx_state;
static Uint8 last_v34_rx_state;
static Uint8 last_abcode;
static Uint8 unused_25[5];
static Uint16 wa5_coefficient;
/* RAM_BAUD's index as write_training_table last read it */
static Uint16 baud_index;
static Uint16 v34_retrains;
static Uint32 unused_30[3];
static Uint32 abcode_change_count;
static Uint32 unused_40;
static Uint32 renegotiation_deadline;
static Uint32 v34_progress_deadline;
static Uint8 unused_4c;

static void write_training_table(void)
{
    Uint8 coefficient;
    Uint16 address = RAM_TRAINING_TABLE;

    baud_index = read_dp_ram(RAM_BAUD, 1);
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: it writes no row for an index
     * past the table */
    if (baud_index >= SYMBOL_RATES) {
        return;
    }
#endif
    /* @bug memory: reads past training_coefficients where RAM_BAUD holds
     * SYMBOL_RATES or more, at 0x0caf21f8 */
    for (coefficient = 0; coefficient < TRAINING_COEFFICIENTS; coefficient++) {
        write_dp_ram(address++, training_coefficients[baud_index][coefficient],
                     1);
    }
}

void InitWaVariables(void)
{
    k56flags.unk_1_7 = 0;
    k56flags.unk_0_0 = 0;
    k56flags.keep_v34_rate_limits = 0;
    k56flags.fallback_limits_set = 0;
    k56flags.v90_modulation_checked = 0;
    k56flags.k56_as_v34 = 0;
    k56flags.v90_as_v34 = 0;
    k56flags.in_k56_training_window = 0;
    k56flags.k56_equalizer_checked = 0;
    k56flags.v34_retrain_counted = 0;
    k56flags.receive_limit_adjusted = 0;
    k56flags.training_control_cleared = 0;
    k56flags.v90_state72_adjusted = 0;
    k56flags.b11_timing = 0;
    k56flags.k56_capabilities_checked = 0;
    k56flags.v8_parameter_checked = 0;
    K56_training_cnt = 0;
    k56flags.measure_eqm = 0;
    k56flags.fallback_requested = 0;
    k56flags.training_rate_programmed = 0;
    rcflags.training_table_written = 0;
    rcflags.v34_measurement_configured = 0;
    rcflags.training_progressed = 0;
    rcflags.ceq_checked = 0;
    rcflags.renegotiation_retrained = 0;
    v34_retrains = 0;
    k56flags.v34_progress_timer_set = 0;
    k56flags.v34_stall_recovery_started = 0;
    k56flags.v34_stall_command_sent = 0;
    last_abcode = 0;
    if (rcdpi->r09.bit.originate && (read_dp_ram(0x304, 1) & 0x10)) {
        k56flags.v8_workaround_enabled = 1;
    } else {
        k56flags.v8_workaround_enabled = 0;
    }
}

void MonitorV8(void)
{
    Uint8 value;

    if (k56flags.v8_workaround_enabled) {
        if ((value = read_dp_ram(0x303, 1)) & 2) {
            write_dp_ram(0x303, value & 0xfffd, 1);
        }
        if (!k56flags.v8_parameter_checked && rcdpi->r17.byte == 2) {
            k56flags.v8_parameter_checked = 1;
            if (read_dp_ram(0x305, 1) & 1) {
                write_dp_ram(0x335, 126, 1);
            }
        }
    }
}

Uint32 V90GetRemoteCaps(void)
{
    Uint32 caps = 0;
    Uint32 part;

    part = read_dp_ram(0x6c1, 1);
    caps |= part;
    part = read_dp_ram(0x6c2, 1);
    caps |= part << 8;
    part = read_dp_ram(0x6c3, 1);
    caps |= part << 16;
    return caps;
}

static void count_remote_bits(void)
{
    Uint8 bits;
    Uint8 mask;

    remote_bit_count = 0;
    bits = read_dp_ram(RAM_REMOTE_BIT_MASK, 1);
    for (mask = 1; mask < 64; mask = mask << 1) {
        if (bits & mask) {
            remote_bit_count++;
        }
    }
}

static Uint8 measure_level_ratio(void)
{
    Uint32 reference_level = 0;

    reference_level += read_dp_ram(0xa62, 2);
    reference_level += read_dp_ram(0xa61, 2);
    reference_level += read_dp_ram(0xa60, 2);
    reference_level /= 3;
    /* @bug arithmetic: divides by zero where the three words average 0, at
     * 0x0caf2478 */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: a reference of 0 gives a ratio of
     * 0 */
    if (!reference_level) {
        level_ratio = 0;
        return 0;
    }
#endif
    level_ratio = read_dp_ram(RAM_LEVEL_PEAK, 2) / reference_level;
    /* @hack spelling: the comparison's 1 and 0 spelled out, where returning
     * the comparison would do */
    return level_ratio > 6 ? 1 : 0;
}

void MonitorV90Train(void)
{
    Uint16 sample_high;
    Uint32 deadline;
    Uint8 receive_limit;
    Uint8 value;
    Uint16 code;
    Uint16 level;
    Uint16 sample;
    Uint8 rate;

    if (rcdpi->r17.byte == 0xff) {
        return;
    }
    if (rcdpi->r16.byte == 0xff) {
        return;
    }
    if ((!k56flags.measure_eqm && rcdpi->r15.bit.rate_range == 1) ||
        rcflags.renegotiation_retrained) {
        k56flags.measure_eqm = 1;
        rcdpi->r15.bit.rate_range = 0;
    }
    if (rcdpi->r17.byte > 0x21 && rcdpi->r16.byte > 0x22 &&
        rcdpi->r17.byte < 0x60) {
        rcflags.training_progressed = 1;
    }
    if (!k56flags.training_control_cleared && rcdpi->r17.byte >= 0x21 &&
        ((value = read_dp_ram(RAM_RATE_RANGE_CONTROL, 1)) & 4)) {
        k56flags.training_control_cleared = 1;
        write_dp_ram(RAM_RATE_RANGE_CONTROL, value & 0xfffb, 1);
    }
    if (rcdpi->r17.byte > 0x2f && !k56flags.v90_modulation_checked) {
        k56flags.v90_modulation_checked = 1;
        if (!(read_dp_ram(RAM_MODULATION_ENABLE, 1) & 1)) {
            rcdpi->r15.bit.rate_range = rcflags.saved_rate_range;
            k56flags.v90_as_v34 = 1;
            return;
        }
    }
    if (rcdpi->r17.byte > 0x30 && !k56flags.receive_limit_adjusted) {
        k56flags.receive_limit_adjusted = 1;
        receive_limit = read_dp_ram(0x406, 1);
        write_dp_ram(0x406, receive_limit & 0xff7f, 1);
    }
    if (!k56flags.v90_state72_adjusted && rcdpi->r17.byte == 0x48) {
        k56flags.v90_state72_adjusted = 1;
        write_dp_ram(0x49, 0xd0, 1);
    }
    if (rcdpi->r16.byte != 0x62 || (read_dp_ram(0x275, 2) & 0xff00) != 0x0e00) {
        return;
    }
    if (!k56flags.measure_eqm) {
        return;
    }
    if (k56flags.training_rate_programmed) {
        return;
    }
    /* each access of RAM_EQM takes a new EQM reading */
    v90_eqm_sum = 0;
    rcdpi->ram_control.bit.address = RAM_EQM >> 8;
    rcdpi->ram_addr.byte = RAM_EQM & 0xff;
    for (sample = 0; sample < V90_EQM_READINGS; sample++) {
        rcdpi->ram_control.bit.busy = 1;
        setup_timer(&deadline, EQM_READ_TICKS);
        while (rcdpi->ram_control.bit.busy) {
            if (timer_expired(deadline)) {
                data_pump_error(DP_ERR_V90_EQM);
                return;
            }
        }
        sample_high = rcdpi->ram_data_high.byte << 8;
        v90_eqm_sum += (Uint16)(sample_high | rcdpi->ram_data.byte);
    }
    v90_eqm_sum >>= V90_EQM_SHIFT;
    receive_limit = read_dp_ram(RAM_MAX_RX_RATE, 1);

    /* @bug uninitialized: the steps down from 0x0caf29b0 read rate unset
     * where no rate qualifies. the fallback that 0x0caf2964 asks for then
     * leaves rate unwritten */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: rate starts at 0 */
    rate = 0;
#endif
    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if (remote_caps = V90GetRemoteCaps(),
        v90_eqm_sum < 81 && (remote_caps & V90_RATE_BIT(22))) {
        rate = 22;
    } else if (v90_eqm_sum < 113 && (remote_caps & V90_RATE_BIT(21))) {
        rate = 21;
    } else if (v90_eqm_sum < 129 && (remote_caps & V90_RATE_BIT(20))) {
        rate = 20;
    } else if (v90_eqm_sum < 145 && (remote_caps & V90_RATE_BIT(19))) {
        rate = 19;
    } else if (v90_eqm_sum < 177 && (remote_caps & V90_RATE_BIT(18))) {
        rate = 18;
    } else if (v90_eqm_sum < 189 && (remote_caps & V90_RATE_BIT(17))) {
        rate = 17;
    } else if (v90_eqm_sum < 208 && (remote_caps & V90_RATE_BIT(16))) {
        rate = 16;
    } else if (v90_eqm_sum < 216 && (remote_caps & V90_RATE_BIT(15))) {
        rate = 15;
    } else if (v90_eqm_sum < 232 && (remote_caps & V90_RATE_BIT(14))) {
        rate = 14;
    } else if (v90_eqm_sum < 246 && (remote_caps & V90_RATE_BIT(13))) {
        rate = 13;
    } else if (v90_eqm_sum < 251 && (remote_caps & V90_RATE_BIT(12))) {
        rate = 12;
    } else if (v90_eqm_sum < 262 && (remote_caps & V90_RATE_BIT(11))) {
        rate = 11;
    } else if (v90_eqm_sum < 282 && (remote_caps & V90_RATE_BIT(10))) {
        rate = 10;
    } else if (v90_eqm_sum < 288 && (remote_caps & V90_RATE_BIT(9))) {
        rate = 9;
    } else if (v90_eqm_sum < 298 && (remote_caps & V90_RATE_BIT(8))) {
        rate = 8;
    } else if (v90_eqm_sum < 309 && (remote_caps & V90_RATE_BIT(7))) {
        rate = 7;
    } else if (v90_eqm_sum < 313 && (remote_caps & V90_RATE_BIT(6))) {
        rate = 6;
    } else if (v90_eqm_sum < 317 && (remote_caps & V90_RATE_BIT(5))) {
        rate = 5;
    } else if (v90_eqm_sum < 325 && (remote_caps & V90_RATE_BIT(4))) {
        rate = 4;
    } else if (v90_eqm_sum < 335 && (remote_caps & V90_RATE_BIT(3))) {
        rate = 3;
    } else if (v90_eqm_sum < 337 && (remote_caps & V90_RATE_BIT(2))) {
        rate = 2;
        /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
    } else if (v90_eqm_sum < 341 && (remote_caps & V90_RATE_BIT(1))) {
        rate = 1;
    } else {
        k56flags.fallback_requested = 1;
    }
    count_remote_bits();
    half_remote_bit_count = remote_bit_count >> 1;
    high_level_ratio = measure_level_ratio();
    rate_backoff = 0;
    code = read_dp_ram(0x6c9, 2);
    level = read_dp_ram(RAM_LEVEL_PEAK, 2);

    if (code == 0x2000) {
        if (level > 0x600 && rate > 5) {
            if (remote_bit_count <= 1) {
                rate -= 1;
            } else if (rate > 6) {
                rate -= 2;
            }
        }
    } else if ((code & 0xf000) == 0x2000) {
        if (level > 1000 || level_ratio >= 4) {
            rate_backoff = remote_bit_count <= 1 ? 1 : 2;
        }
    } else if ((code & 0xf000) == 0x3000 || (code & 0xf000) == 0x4000) {
        if (level > 0x200 && remote_bit_count >= 2) {
            /* @hack redundant: remote_bit_count tested again, where a
             * rate_backoff of 2 would do */
            rate_backoff = remote_bit_count > 1 ? 2 : 0;
        }
    }
    if (rate > rate_backoff) {
        rate -= rate_backoff;
    } else {
        k56flags.fallback_requested = 1;
    }
    if (rate > half_remote_bit_count) {
        rate -= half_remote_bit_count;
    } else {
        k56flags.fallback_requested = 1;
    }
    if (rate > high_level_ratio) {
        rate -= high_level_ratio;
    } else {
        k56flags.fallback_requested = 1;
    }
    if (rate > 2 && AgressivityLevel == 0) {
        rate -= 2;
    } else if (rate > 1 && AgressivityLevel == 1) {
        rate -= 1;
    }
    if (rate > 1) {
        rate -= 1;
    } else {
        k56flags.fallback_requested = 1;
    }
    if (rate > receive_limit) {
        rate = receive_limit;
    }
    if (rate == 1) {
        k56flags.fallback_requested = 1;
    }
    if (k56flags.fallback_requested) {
        k56flags.v90_as_v34 = 1;
        write_dp_ram(RAM_MODULATION_ENABLE,
                     read_dp_ram(RAM_MODULATION_ENABLE, 1) & 0xfffe, 1);
        write_dp_ram(0x6c0, read_dp_ram(0x6c0, 1) & 0xfffe, 1);
        rcdpi->r15.bit.rate_range = 1;
    } else {
        write_dp_ram(RAM_RX_RATE, rate, 1);
    }
    k56flags.training_rate_programmed = 1;
}

void MonitorK56Train(void)
{
    Uint16 sample_high;
    Uint16 receive_limit;
    Uint32 deadline;
    Uint8 value;
    Uint8 sample;
    Uint8 rate_limit;
    Uint16 near_level;
    Uint16 far_level;
    char low_rate;
    /* @bug uninitialized: tests low_rate at 0x0caf3056, which only the three
     * lowest rates set */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: low_rate starts at 0 */
    low_rate = 0;
#endif

    if (rcdpi->r17.byte == 0xff) {
        return;
    }
    if (rcdpi->r16.byte == 0xff) {
        return;
    }
    if (k56flags.fallback_requested) {
        if (timer_expired(k56_fallback_deadline)) {
            write_dp_ram(0x53, 0xff, 1);
            k56flags.in_k56_training_window = 0;
        } else {
            return;
        }
    }
    if (rcdpi->r17.byte > 0x21 && rcdpi->r16.byte > 0x22 &&
        rcdpi->r17.byte < 0x60) {
        rcflags.training_progressed = 1;
    }
    if (!k56flags.training_control_cleared && rcdpi->r17.byte >= 0x21 &&
        ((value = read_dp_ram(RAM_RATE_RANGE_CONTROL, 1)) & 4)) {
        k56flags.training_control_cleared = 1;
        write_dp_ram(RAM_RATE_RANGE_CONTROL, value & 0xfffb, 1);
    }
    if (!k56flags.in_k56_training_window && rcdpi->r17.byte >= 0x20 &&
        rcdpi->r17.byte <= 0x2f) {
        k56flags.in_k56_training_window = 1;
        if (k56flags.fallback_requested) {
            k56flags.fallback_limits_set = 1;
            write_dp_ram(0xafb, 0x7fff, 2);
            write_dp_ram(0xbfb, 0, 2);
            k56flags.k56_as_v34 = 1;
            write_dp_ram(RAM_RX_RATE, 14, 1);
            rcdpi->r15.bit.rate_range = 1;
            return;
        }
    }
    if (rcdpi->r17.byte > 0x2f) {
        if (k56flags.in_k56_training_window) {
            k56flags.in_k56_training_window = 0;
            if (!(read_dp_ram(RAM_MODULATION_ENABLE, 1) & 1)) {
                k56flags.k56_as_v34 = 1;
                return;
            }
            if (rcdpi->r15.bit.rate_range == 1) {
                k56flags.measure_eqm = 1;
                rcdpi->r15.bit.rate_range = 0;
            }
        }
    }
    if (rcdpi->r17.byte > 0x30 && !k56flags.receive_limit_adjusted) {
        k56flags.receive_limit_adjusted = 1;
        receive_limit = read_dp_ram(0x406, 1);
        write_dp_ram(0x406, receive_limit & 0xff7f, 1);
    }
    if (!k56flags.k56_capabilities_checked && rcdpi->r17.byte >= 0x47) {
        k56flags.k56_capabilities_checked = 1;
        if ((read_dp_ram(RAM_REMOTE_BIT_MASK, 1) & 0x3f) &&
            (read_dp_ram(0x368, 1) & 0x40)) {
            write_dp_ram(RAM_MODULATION_ENABLE,
                         read_dp_ram(RAM_MODULATION_ENABLE, 1) & 0xfffe, 1);
            write_dp_ram(0x276, 0, 1);
            setup_timer(&k56_fallback_deadline, K56_FALLBACK_TICKS);
            k56flags.fallback_requested = 1;
            return;
        }
    }
    if (!k56flags.k56_equalizer_checked && rcdpi->r17.byte >= 0x40 &&
        rcdpi->r17.byte <= 0x4f) {
        if (rcdpi->r15.bit.rate_range == 1) {
            write_dp_ram(0xa29, read_dp_ram(0xa29, 2), 2);
        }
        k56flags.k56_equalizer_checked = 1;
    }
    if (rcdpi->r17.byte != 0x63 || read_dp_ram(RAM_TRAINING_PHASE, 1) != 2) {
        return;
    }
    if (!k56flags.measure_eqm) {
        return;
    }
    if (!k56flags.training_rate_programmed) {
        k56_eqm_sum = 0;
        rcdpi->ram_control.bit.address = RAM_EQM >> 8;
        rcdpi->ram_addr.byte = RAM_EQM & 0xff;
        for (sample = 0; sample < EQM_READINGS; sample++) {
            rcdpi->ram_control.bit.busy = 1;
            setup_timer(&deadline, EQM_READ_TICKS);
            while (rcdpi->ram_control.bit.busy) {
                if (timer_expired(deadline)) {
                    data_pump_error(DP_ERR_K56_EQM);
                    return;
                }
            }
            sample_high = rcdpi->ram_data_high.byte << 8;
            k56_eqm_sum += (Uint16)(sample_high | rcdpi->ram_data.byte);
        }
        k56_eqm_sum >>= EQM_SHIFT;
        /* @hack redundant: an assignment of RAM_RX_RATE that never runs */
        if (0) {
            k56_eqm_sum = RAM_RX_RATE;
        }

        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
        if (rate_limit = read_dp_ram(RAM_MAX_RX_RATE, 1),
            k56_eqm_sum < 80 && rate_limit == 13) {
            write_dp_ram(RAM_RX_RATE, 11, 1);
        } else if (k56_eqm_sum < 144 && rate_limit >= 12) {
            write_dp_ram(RAM_RX_RATE, 11, 1);
        } else if (k56_eqm_sum < 149 && rate_limit >= 11) {
            write_dp_ram(RAM_RX_RATE, 11, 1);
        } else if (k56_eqm_sum < 166 && rate_limit >= 10) {
            write_dp_ram(RAM_RX_RATE, 10, 1);
        } else if (k56_eqm_sum < 176 && rate_limit >= 9) {
            write_dp_ram(RAM_RX_RATE, 9, 1);
        } else if (k56_eqm_sum < 192 && rate_limit >= 8) {
            write_dp_ram(RAM_RX_RATE, 8, 1);
        } else if (k56_eqm_sum < 208 && rate_limit >= 7) {
            write_dp_ram(RAM_RX_RATE, 7, 1);
        } else if (k56_eqm_sum < 224 && rate_limit >= 6) {
            write_dp_ram(RAM_RX_RATE, 6, 1);
        } else if (k56_eqm_sum < 229 && rate_limit >= 5) {
            write_dp_ram(RAM_RX_RATE, 5, 1);
        } else if (k56_eqm_sum < 240 && rate_limit >= 4) {
            write_dp_ram(RAM_RX_RATE, 4, 1);
        } else if (k56_eqm_sum < 256 && rate_limit >= 3) {
            write_dp_ram(RAM_RX_RATE, 3, 1);
            low_rate = 1;
        } else if (k56_eqm_sum < 261 && rate_limit >= 2) {
            write_dp_ram(RAM_RX_RATE, 2, 1);
            low_rate = 1;
        } else if (k56_eqm_sum < 272 && rate_limit >= 1) {
            write_dp_ram(RAM_RX_RATE, 1, 1);
            low_rate = 1;
        } else {
            k56flags.fallback_requested = 1;
            write_dp_ram(RAM_MODULATION_ENABLE,
                         read_dp_ram(RAM_MODULATION_ENABLE, 1) & 0xfffe, 1);
            write_dp_ram(0x276, 0, 1);
        }

        /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
        if (low_rate) {
            near_level = read_dp_ram(0x6a5, 2);
            far_level = read_dp_ram(0x6a7, 2);
            if (near_level - far_level <= 10) {
                k56flags.fallback_requested = 1;
            }
        }
        if (k56flags.fallback_requested) {
            write_dp_ram(RAM_MODULATION_ENABLE,
                         read_dp_ram(RAM_MODULATION_ENABLE, 1) & 0xfffe, 1);
            write_dp_ram(0x276, 0, 1);
            setup_timer(&k56_fallback_deadline, K56_FALLBACK_TICKS);
        }
        k56flags.training_rate_programmed = 1;
    }
}

static char select_wa5_coefficient(Uint8 tx_rate, Uint8 baud)
{
    char found;

    switch (tx_rate) {
    case 11:
        switch (baud) {
        case 5:
            wa5_coefficient = 0x5f2;
            found = 1;
            break;
        case 4:
            wa5_coefficient = 0x2c8;
            found = 1;
            break;
        case 3:
            wa5_coefficient = 0x4f5;
            found = 1;
            break;
        default:
            found = 0;
            break;
        }
        break;
    case 12:
        switch (baud) {
        case 5:
            wa5_coefficient = 0x54d;
            found = 1;
            break;
        case 4:
            wa5_coefficient = 0x3ff;
            found = 1;
            break;
        default:
            found = 0;
            break;
        }
        break;
    default:
        found = 0;
        break;
    }
    return found;
}

void MonitorV34Train(void)
{
    Uint16 sample_high;
    Uint8 value;
    Uint32 deadline;
    Uint8 mask;
    Uint8 sample;
    Uint8 rate_limit;
    Uint8 rate;

    /* @hack redundant: a read of RAM_RATE_RANGE_CONTROL that never runs */
    if (0) {
        rcflags.saved_rate_range = read_dp_ram(RAM_RATE_RANGE_CONTROL, 1);
    }
    if ((rcdpi->r17.byte & R17_STAGE) == R17_RETRAIN) {
        if (!k56flags.v34_progress_timer_set) {
            setup_timer(&v34_progress_deadline, V34_STALL_TICKS);
            k56flags.v34_progress_timer_set = 1;
            last_v34_tx_state = rcdpi->r17.byte;
            last_v34_rx_state = rcdpi->r16.byte;
        } else if (last_v34_tx_state != rcdpi->r17.byte ||
                   last_v34_rx_state != rcdpi->r16.byte) {
            setup_timer(&v34_progress_deadline, V34_STALL_TICKS);
            last_v34_tx_state = rcdpi->r17.byte;
            last_v34_rx_state = rcdpi->r16.byte;
        } else if (last_v34_tx_state == rcdpi->r17.byte ||
                   last_v34_tx_state == rcdpi->r16.byte) {

            if (timer_expired(v34_progress_deadline)) {
                if (!k56flags.v34_stall_recovery_started) {
                    value = read_dp_ram(0x52, 1);
                    write_dp_ram(0x52, value & 0xffdf, 1);
                    write_dp_ram(0x2ad, 0, 1);
                    k56flags.v34_stall_recovery_started = 1;
                    setup_timer(&v34_progress_deadline, V34_RECOVERY_TICKS);
                } else if (!k56flags.v34_stall_command_sent) {
                    newc();
                    k56flags.v34_stall_command_sent = 1;
                }
            }
        }
    } else {
        k56flags.v34_progress_timer_set = 0;
        k56flags.v34_stall_recovery_started = 0;
        k56flags.v34_stall_command_sent = 0;
    }
    if (!k56flags.fallback_limits_set &&
        (k56flags.unk_1_7 || k56flags.unk_0_0)) {
        k56flags.fallback_limits_set = 1;
        write_dp_ram(0xafb, 0x7fff, 2);
        write_dp_ram(0xbfb, 0, 2);
        rcdpi->r15.bit.rate_range = 1;
    }
    if (rcdpi->r17.byte > 0x21 && rcdpi->r16.byte > 0x22 &&
        rcdpi->r17.byte < 0x60) {
        rcflags.training_progressed = 1;
    }
    if (!k56flags.training_control_cleared && !k56flags.unk_1_7 &&
        !k56flags.unk_0_0 && rcdpi->r17.byte >= 0x21 &&
        ((mask = read_dp_ram(RAM_RATE_RANGE_CONTROL, 1)) & 4)) {
        k56flags.training_control_cleared = 1;
        write_dp_ram(RAM_RATE_RANGE_CONTROL, mask & 0xfffb, 1);
    }
    if (!k56flags.v34_retrain_counted && rcdpi->r17.byte > 0x22 &&
        rcdpi->r17.byte < 0x2a) {
        k56flags.v34_retrain_counted = 1;
        v34_retrains++;
        if (v34_retrains > 1) {
            rcdpi->r15.bit.rate_range = rcflags.saved_rate_range;
        }
        rcflags.v34_measurement_configured = 0;
        k56flags.training_rate_programmed = 0;
        if (!(read_dp_ram(0x11b, 1) & 0x20) && !(read_dp_ram(0x11a, 1) & 1)) {
            Wa5State = WA5_WAIT;
            if (!rcdpi->r09.bit.originate) {
                write_dp_ram(0x101, 31, 1);
            }
        } else {
            Wa5State = WA5_OFF;
        }
    }
    if (!k56flags.keep_v34_rate_limits && rcdpi->r17.byte >= 0x30) {
        write_dp_ram(RAM_MAX_RX_RATE, MaxV34ReceiveSpeed, 1);
        write_dp_ram(RAM_MAX_TX_RATE, MaxV34TransmitSpeed, 1);
    }
    if (rcflags.training_table_written == 0 && rcdpi->r15.bit.rate_range == 1 &&
        rcdpi->r17.byte == 0x40) {
        value = read_dp_ram(RAM_RATE_RANGE_CONTROL, 1);
        write_dp_ram(RAM_RATE_RANGE_CONTROL, value | 0x10, 1);
        write_training_table();
        rcflags.training_table_written = 1;
    }
    if (rcdpi->r17.byte == 0x63) {
        k56flags.v34_retrain_counted = 0;
        if (!rcflags.v34_measurement_configured) {

            if ((v34_status = read_dp_ram(0x39a, 1)) <= 4 || baud_index < 3 ||
                !(read_dp_ram(RAM_V34_RATES, 2) & 0x3ff0) ||
                !rcdpi->r15.bit.rate_range) {
                k56flags.measure_eqm = 0;
            } else {
                write_dp_ram(0xa29, 0x7800, 2);
                write_dp_ram(0xb29, 0x7f00, 2);
                rcdpi->r15.bit.rate_range = 0;
                k56flags.measure_eqm = 1;
            }
            rcflags.v34_measurement_configured = 1;
        }
        if (read_dp_ram(RAM_TRAINING_PHASE, 1) == 2 && k56flags.measure_eqm &&
            !k56flags.training_rate_programmed) {
            v34_eqm_sum = 0;
            rcdpi->ram_control.bit.address = RAM_EQM >> 8;
            rcdpi->ram_addr.byte = RAM_EQM & 0xff;
            for (sample = 0; sample < EQM_READINGS; sample++) {
                rcdpi->ram_control.bit.busy = 1;
                setup_timer(&deadline, EQM_READ_TICKS);
                while (rcdpi->ram_control.bit.busy) {
                    if (timer_expired(deadline)) {
                        data_pump_error(DP_ERR_V34_EQM);
                        return;
                    }
                }
                sample_high = rcdpi->ram_data_high.byte << 8;
                v34_eqm_sum += (Uint16)(sample_high | rcdpi->ram_data.byte);
            }
            v34_eqm_sum >>= EQM_SHIFT;
            rate_limit = read_dp_ram(RAM_MAX_RX_RATE, 1);
            /* @bug uninitialized: rate is unset at the write at 0x0caf377c
             * where baud_index is outside 3 to 5 */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: rate starts at
             * rate_limit */
            rate = rate_limit;
#endif
            if (baud_index == 5) {
                if (v34_eqm_sum <= 42 && rate_limit == 14) {
                    rate = 14;
                } else if (v34_eqm_sum <= 64 && rate_limit >= 13) {
                    rate = 13;
                } else if (v34_eqm_sum <= 106 && rate_limit >= 12) {
                    rate = 12;
                } else if (v34_eqm_sum <= 175 && rate_limit >= 11) {
                    rate = 11;
                } else if (v34_eqm_sum <= 293 && rate_limit >= 10) {
                    rate = 10;
                } else if (v34_eqm_sum <= 500 && rate_limit >= 9) {
                    rate = 9;
                } else if (v34_eqm_sum <= 731 && rate_limit >= 8) {
                    rate = 8;
                } else if (v34_eqm_sum <= 1150 && rate_limit >= 7) {
                    rate = 7;
                } else if (v34_eqm_sum <= 1800 && rate_limit >= 6) {
                    rate = 6;
                } else {
                    rate = 5;
                }
                /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
            } else if (baud_index == 4 || baud_index == 3) {
                if (v34_eqm_sum <= 46 && rate_limit >= 13 && baud_index == 4) {
                    rate = 13;
                } else if (v34_eqm_sum <= 64 && rate_limit >= 12) {
                    rate = 12;
                } else if (v34_eqm_sum <= 146 && rate_limit >= 11) {
                    rate = 11;
                } else if (v34_eqm_sum <= 265 && rate_limit >= 10) {
                    rate = 10;
                } else if (v34_eqm_sum <= 450 && rate_limit >= 9) {
                    rate = 9;
                } else if (v34_eqm_sum <= 750 && rate_limit >= 8) {
                    rate = 8;
                } else if (v34_eqm_sum <= 1200 && rate_limit >= 7) {
                    rate = 7;
                } else if (v34_eqm_sum <= 1850 && rate_limit >= 6) {
                    rate = 6;
                } else {
                    rate = 5;
                }
            }

            write_dp_ram(RAM_RX_RATE, rate, 1);
            k56flags.training_rate_programmed = 1;
        }
    }
    switch (Wa5State) {
    case WA5_WAIT:
        if (rcdpi->r17.byte == R17_TRAIN_SELECT) {
            if (select_wa5_coefficient(read_dp_ram(RAM_TX_RATE, 1),
                                       read_dp_ram(RAM_BAUD, 1)) == 1) {
                Wa5State = WA5_CHOSEN;
            } else {
                Wa5State = WA5_DONE;
            }
        }
        break;
    case WA5_CHOSEN:
        if (rcdpi->r17.byte == R17_TRAIN_WRITE) {
            write_dp_ram(RAM_WA5_COEFFICIENT, wa5_coefficient, 2);
            Wa5State = WA5_DONE;
        }
        break;
    case WA5_DONE:
        if (rcdpi->r17.byte != R17_TRAIN_SELECT) {
            Wa5State = WA5_WAIT;
        }
        break;
    case WA5_OFF:
        break;
    }
}

void MonitorV32Train(void)
{
    if (rcdpi->r0c.bit.unk_bit3 || rcdpi->r0c.bit.unk_bit2) {
        rcflags.training_progressed = 1;
        handshake_status = 1;
        if (rcdpi->r09.bit.originate == 1) {
            rate_seq_R1 = read_dp_ram(0x208, 2);
        } else {
            rate_seq_R1 = read_dp_ram(0x204, 2);
        }
    } else if (rcdpi->r0c.bit.unk_bit0) {
        handshake_status = 3;
        if (read_dp_ram(0x208, 2) == 0x111) {
            handshake_status = HANDSHAKE_FAILED;
        }
        rcdpi->r0c.bit.unk_bit0 = 0;
    } else if (rcdpi->r0d.bit.unk_bit5) {
        handshake_status = 8;
    } else if (rcdpi->r0d.bit.unk_bit4) {
        handshake_status = 9;
    } else if (rcdpi->r0d.bit.unk_bit3) {
        handshake_status = 10;
    } else if (rcdpi->r0c.bit.unk_bit7 || rcdpi->r0c.bit.unk_bit6 ||
               rcdpi->r0c.bit.unk_bit5 || rcdpi->r0c.bit.unk_bit4) {
        handshake_status = 11;
    } else if (rcdpi->r0a.bit.unk_bit7) {
        handshake_status = 12;
    }
}

void MonitorV34RR(void)
{
    char restarted = 0;
    Uint8 value;

    switch (Wa5State) {
    case WA5_WAIT:
        if (rcdpi->r17.byte == R17_RENEGOTIATE_SELECT) {
            if (select_wa5_coefficient(read_dp_ram(RAM_TX_RATE, 1),
                                       read_dp_ram(RAM_BAUD, 1)) == 1) {
                Wa5State = WA5_CHOSEN;
            } else {
                Wa5State = WA5_DONE;
            }
        }
        break;
    case WA5_CHOSEN:
        if (rcdpi->r17.byte == R17_RENEGOTIATE_WRITE) {
            write_dp_ram(RAM_WA5_COEFFICIENT, wa5_coefficient, 2);
            Wa5State = WA5_DONE;
        }
        break;
    case WA5_DONE:
        if (rcdpi->r17.byte != R17_RENEGOTIATE_SELECT) {
            Wa5State = WA5_WAIT;
        }
        break;
    case WA5_OFF:
        break;
    }
    if ((rcdpi->r17.byte & R17_STAGE) == R17_RENEGOTIATION) {
        if (!k56flags.renegotiation_timer_set) {
            setup_timer(&renegotiation_deadline, V34_RENEGOTIATION_TICKS);
            k56flags.renegotiation_timer_set = 1;
            renegotiation_state = rcdpi->r17.byte;
        } else if (renegotiation_state == rcdpi->r17.byte &&
                   timer_expired(renegotiation_deadline)) {
            value = read_dp_ram(0x45, 1);
            write_dp_ram(0x45, value | 0x20, 1);
            restarted = 1;
        } else if (renegotiation_state != rcdpi->r17.byte) {
            setup_timer(&renegotiation_deadline, V34_RENEGOTIATION_TICKS);
            renegotiation_state = rcdpi->r17.byte;
        }
    }
    if (restarted == 1 || (rcdpi->r17.byte & R17_STAGE) == R17_RETRAIN) {
        InitWaVariables();
        rcflags.renegotiation_retrained = 1;
        rcflags.retraining = 1;
        rcflags.renegotiating = 0;
        if (rcflags.renegotiation_started) {
            rcflags.retrain_started = 1;
        }
    }
}

void MonitorV90RR(void)
{
    char restarted = 0;
    Uint8 value;

    if (rcdpi->r17.byte == 0x84 || rcdpi->r17.byte == 0x85) {
        if (!k56flags.renegotiation_timer_set) {
            setup_timer(&renegotiation_deadline, V90_RENEGOTIATION_TICKS);
            k56flags.renegotiation_timer_set = 1;
            renegotiation_state = rcdpi->r17.byte;
        } else if (renegotiation_state == rcdpi->r17.byte &&
                   timer_expired(renegotiation_deadline)) {
            value = read_dp_ram(0x45, 1);
            write_dp_ram(0x45, value | 0x20, 1);
            restarted = 1;
        } else if (renegotiation_state != rcdpi->r17.byte) {
            setup_timer(&renegotiation_deadline, V90_RENEGOTIATION_TICKS);
            renegotiation_state = rcdpi->r17.byte;
        }
    }
    if (restarted == 1 || (rcdpi->r17.byte & R17_STAGE) == R17_RETRAIN) {
        InitWaVariables();
        rcflags.renegotiation_retrained = 1;
        rcflags.retraining = 1;
        rcflags.renegotiating = 0;
        k56flags.renegotiation_timer_set = 0;
        if (rcflags.renegotiation_started) {
            rcflags.retrain_started = 1;
        }
    }
}

void WaV34Disc(void)
{
}

void MonitorABcode(void)
{
    if (rcdpi->abcode.byte != last_abcode) {
        last_abcode = rcdpi->abcode.byte;
        abcode_change_count++;
    }
}
