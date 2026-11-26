/* the K56flex data pump, driven through its registers and its RAM */
#include <sg_xpt.h>

#include "RamDownl.h"
#include "dce.h"
#include "dspdb.h"
#include "ecdcutil.h"
#include "messages.h"
#include "roc_cc.h"
#include "roc_k56.h"
#include "roc_wa.h"
#include "target.h"
#include "util_mod.h"
#include "v25ter.h"

/* the bits of handshake_options */
#define HANDSHAKE_AUTO_MODE 0x01
#define HANDSHAKE_RATE_RANGE 0x02

/* the words that ready_1c and ready_1a hold once the pump is up */
#define READY_1C 0xea3c
#define READY_1A 0x5536

/* the 10 ms ticks of setup_timer in ms, a time in milliseconds */
#define TICKS(ms) ((ms) / 10)

/* how long the pump gets to end a RAM access or a change of mode */
#define DP_TIMEOUT TICKS(500)

/* words of the pump's RAM, beside those roc_k56.h names. RAM_V34_CONF and
 * RAM_V90_CONF hold the V.34 and V.90 configurations that CONF_AUTO stands
 * for. RAM_V32_RATES holds a mask of the rates a V.32 connection may take */
#define RAM_SELF_TEST 0x0a5
#define RAM_ERRORS 0x0a7
#define RAM_DTMF_ON 0x218
#define RAM_DTMF_OFF 0x219
#define RAM_PULSE_PAUSE 0x21a
#define RAM_PULSE_BREAK 0x21c
#define RAM_PULSE_MAKE 0x22c
#define RAM_ROUND_TRIP 0x239
#define RAM_EQM10 0x26f
#define RAM_TONE_1 0x280
#define RAM_TONE_2 0x282
#define RAM_V32_RATES 0x2c0
#define RAM_DTMF_ON_HIGH 0x2db
#define RAM_DTMF_OFF_HIGH 0x2dc
#define RAM_V8_STATUS 0x302
#define RAM_V34_CONF 0x309
#define RAM_TX_GAIN 0x3da
#define RAM_V90_CONF 0x6a2
#define RAM_K56_EQM 0x810
#define RAM_RX_POWER 0xa00
#define RAM_FAR_ECHO 0xb52

/* the transmit attenuation dp_monitor_handshake sets as V.8 chooses V.90 */
#define V90_ATTENUATION 12

/* the bits of a V.32 rate sequence for the rates V.32 bis alone runs */
#define V32BIS_RATES 0x0880

/* V.32's symbol rate in baud */
#define V32_BAUD 2400

/* the index dp_get_rx_baud_rate returns for a configuration with no V.34
 * symbol rate */
#define BAUD_NONE 10

/* the RAM_EQM10 reading below which dp_will_eqm_be_ok returns 1 */
#define EQM10_OK 10

/* the passes dp_interrupt makes after its first before it gives up */
#define INTERRUPT_REPEATS 50

/* the passes of the empty loop dp_enable_data_mode runs between format's
 * change and the return of rts */
#define FORMAT_SPIN 1000

/* the phases of a remote digital loop. a loop this side asks for passes
 * through RDL_ASKED, RDL_LOOPED and RDL_ENDING. a loop the far end asks for
 * passes through RDL_FAR_ASKED and RDL_FAR_LOOPED. RDL_PUMP_LOOPED lasts
 * while status' looping reports a loop that dp_monitor_remote_rdl found
 * with no request */
#define RDL_NONE 0
#define RDL_LOOPED 2
#define RDL_ASKED 4
#define RDL_FAR_ASKED 5
#define RDL_FAR_LOOPED 6
#define RDL_PUMP_LOOPED 7
#define RDL_ENDING 8

/* a key and the value it selects, in a table sorted by key. the keys are
 * rate codes */
struct byte_pair {
    Uint8 key;
    Uint8 value;
};

struct word_pair {
    Uint16 key;
    Uint16 value;
};

/* the lowest reading of each of dp_snr_ratio's grades 1 to 3. a reading
 * below grade_1 is grade 0 */
struct byte_grades {
    Uint8 grade_1;
    Uint8 grade_2;
    Uint8 grade_3;
};

struct word_grades {
    Uint16 grade_1;
    Uint16 grade_2;
    Uint16 grade_3;
};

/* the registers dp_hardware_reset reads as the pump comes up. ready_1c
 * holds registers 0x1c and 0x1d, the first in its low byte, and ready_1a
 * holds 0x1a and 0x1b. nothing reads the unk_ fields */
struct dp_reset_info {
    Uint16 ready_1c;
    Uint16 ready_1a;
    Uint16 unk_4;
    Uint16 unk_6;
    Uint8 unk_8;
    Uint8 unk_9;
    Uint16 unk_a;
    Uint16 unk_c;
    Uint16 unk_e;
    Uint16 unk_10;
    Uint16 unk_12;
    Uint16 unk_14;
    Uint16 unk_16;
    Uint16 unk_18;
};

/* dp_snr_ratio's grades, a row for each AgressivityLevel. snr_byte_grades
 * grades RAM_EQM10. for K56flex and V.90, bit 0 of K56options picks
 * snr_word_grades_1 or snr_word_grades_0, which hold the same rows */
static const struct byte_grades snr_byte_grades[3] = {
    {0x14, 0x2c, 0x34}, {0x18, 0x30, 0x38}, {0x1c, 0x2c, 0x40}};
static const struct word_grades snr_word_grades_1[3] = {
    {0x1c00, 0x2000, 0x2500},
    {0x1c00, 0x2000, 0x2500},
    {0x1c00, 0x2000, 0x2500}};
static const struct word_grades snr_word_grades_0[3] = {
    {0x1c00, 0x2000, 0x2500},
    {0x1c00, 0x2000, 0x2500},
    {0x1c00, 0x2000, 0x2500}};
/* the pump's code for each digit dp_dial_number takes. digits 10 to 13 take
 * the codes dp_detect_dtmf reads as A to D. digits 14 and 15 take those it
 * reads as * and # */
static const Uint8 dtmf_codes[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                     0x06, 0x07, 0x08, 0x09, 0x0c, 0x0d,
                                     0x0e, 0x0f, 0x0a, 0x0b};
/* the gain, in TxLevelGain's units, for each dB of attenuation past
 * MOST_ATTENUATION, from 1 dB to 25 dB */
static const Uint16 tx_gains[25] = {
    0x390a, 0x32d6, 0x2d4e, 0x2861, 0x23fd, 0x2013, 0x1c96, 0x197a, 0x16b5,
    0x143d, 0x1209, 0x1013, 0x0e53, 0x0cc5, 0x0b61, 0x0a24, 0x090a, 0x080e,
    0x072e, 0x0666, 0x05b4, 0x0515, 0x0487, 0x0409, 0x0399};
/* the words dp_set_dtmf writes at RAM 0x29b and 0x29d, by attenuation in
 * dB. dtmf_29b_N holds table N's words for 0x29b, and dtmf_29d_N those for
 * 0x29d */
static const Uint16 dtmf_29b_1[17] = {
    0x2156, 0x2156, 0x1db6, 0x1a7b, 0x1799, 0x1508, 0x12bf, 0x10b5, 0x0ee4,
    0x0d45, 0x0bd4, 0x0a8a, 0x0965, 0x085f, 0x0776, 0x06a6, 0x05ed};
static const Uint16 dtmf_29d_1[17] = {
    0x2567, 0x2567, 0x2156, 0x1db6, 0x1a7b, 0x1799, 0x1508, 0x12bf, 0x10b5,
    0x0ee4, 0x0d45, 0x0bd4, 0x0a8a, 0x0965, 0x085f, 0x0776, 0x06a6};
static const Uint16 dtmf_29b_2[17] = {
    0x1f78, 0x1f78, 0x1c0c, 0x1900, 0x1648, 0x13db, 0x11b2, 0x0fc6, 0x0e0e,
    0x0c87, 0x0b2a, 0x09f3, 0x08de, 0x07e7, 0x070b, 0x0647, 0x0598};
static const Uint16 dtmf_29d_2[17] = {
    0x279f, 0x279f, 0x2350, 0x1f78, 0x1c0c, 0x1900, 0x1648, 0x13db, 0x11b2,
    0x0fc6, 0x0e0e, 0x0c87, 0x0b2a, 0x09f3, 0x08de, 0x07e7, 0x070b};
static const Uint16 dtmf_29b_3[17] = {
    0x1a7b, 0x1a7b, 0x1a7b, 0x1799, 0x1508, 0x12bf, 0x10b5, 0x0ee4, 0x0d45,
    0x0bd4, 0x0a8a, 0x0965, 0x085f, 0x0776, 0x06a6, 0x05ed, 0x0548};
static const Uint16 dtmf_29d_3[17] = {
    0x2567, 0x2567, 0x2567, 0x2156, 0x1db6, 0x1a7b, 0x1799, 0x1508, 0x12bf,
    0x10b5, 0x0ee4, 0x0d45, 0x0bd4, 0x0a8a, 0x0965, 0x085f, 0x0776};
/* each pair of v32_max_masks gives the mask of the V.32 rates a highest rate
 * allows. each pair of v32_min_masks gives those a lowest rate allows. for
 * MOD_V32BIS, select_conf writes the rates both allow at RAM_V32_RATES */
static const struct word_pair v32_max_masks[5] = {
    {6, 0x0d91}, {7, 0x0dd1}, {8, 0x0fd1}, {11, 0x0ff1}, {12, 0x0ff9}};
static const struct word_pair v32_min_masks[5] = {
    {6, 0x0ff9}, {7, 0x0bf9}, {8, 0x0bb9}, {11, 0x09b9}, {12, 0x0999}};
/* each pair of v34_max_masks gives the mask of the V.34 rates a highest rate
 * allows. each pair of v34_min_masks gives those a lowest rate allows.
 * select_conf writes the rates they allow at RAM_V34_RATES */
static const struct word_pair v34_max_masks[15] = {
    {5, 0x0001},  {6, 0x0003},  {7, 0x0007},  {8, 0x000f},  {11, 0x001f},
    {12, 0x003f}, {13, 0x007f}, {14, 0x00ff}, {21, 0x01ff}, {22, 0x03ff},
    {23, 0x07ff}, {25, 0x0fff}, {29, 0x1fff}, {30, 0x1fff}, {32, 0x3fff}};
static const struct word_pair v34_min_masks[15] = {
    {5, 0x3fff},  {6, 0x3ffe},  {7, 0x3ffc},  {8, 0x3ff8},  {11, 0x3ff0},
    {12, 0x3fe0}, {13, 0x3fc0}, {14, 0x3f80}, {21, 0x3f00}, {22, 0x3e00},
    {23, 0x3c00}, {25, 0x3800}, {29, 0x3000}, {30, 0x2000}, {32, 0x2000}};
/* the pump's index of each rate code of V.34, of K56flex and of V.90 */
static const struct byte_pair v34_rate_indexes[15] = {
    {5, 1},  {6, 2},   {7, 3},   {8, 4},   {11, 5},  {12, 6},  {13, 7}, {14, 8},
    {21, 9}, {22, 10}, {23, 11}, {25, 12}, {29, 13}, {30, 13}, {32, 14}};
static const struct byte_pair k56_rate_indexes[14] = {
    {30, 1}, {32, 1}, {33, 2}, {35, 3},  {37, 4},  {40, 5},  {42, 6},
    {44, 7}, {46, 8}, {48, 9}, {50, 10}, {52, 11}, {54, 12}, {56, 13}};
static const struct byte_pair v90_rate_indexes[22] = {
    {24, 1},  {26, 2},  {28, 3},  {30, 4},  {31, 5},  {34, 6},
    {35, 7},  {36, 8},  {39, 9},  {40, 10}, {41, 11}, {43, 12},
    {44, 13}, {45, 14}, {47, 15}, {48, 16}, {49, 17}, {51, 18},
    {52, 19}, {53, 20}, {55, 21}, {56, 22}};
/* the V.34 symbol rate in baud of each index in RAM_BAUD */
/* @bug logic: gives index 1 a rate of 1000 baud, where V.34's symbol rate is
 * 2743, at 0x0cb03bd0 */
static const Uint32 symbol_rates[6] = {2400, 1000, 2800, 3000, 3200, 3429};

/* the rate code of each index in RAM_RX_RATE and RAM_TX_RATE, or 0xff for
 * none */
static Uint8 v34_rates[15] = {0xff, 5,  6,  7,  8,  11, 12, 13,
                              14,   21, 22, 23, 25, 29, 32};
static Uint8 k56_rates[15] = {0xff, 30, 33, 35, 37, 40, 42,  44,
                              46,   48, 50, 52, 54, 56, 0xff};
static Uint8 v90_rates[24] = {0xff, 24, 26, 28, 30, 31, 34, 35,
                              36,   39, 40, 41, 43, 44, 45, 47,
                              48,   49, 51, 52, 53, 55, 56, 0xff};

volatile struct dp_registers *rcdpi;
struct rc_flags rcflags;
struct k56_flags k56flags;
Uint8 handshake_status;
Uint8 Wa5State;
Uint8 volume_state;
Uint8 MaxV34TransmitSpeed;
Uint8 MaxK56TransmitSpeed;
Uint8 MaxV34ReceiveSpeed;
Uint8 MaxK56ReceiveSpeed;
Uint8 MaxV90ReceiveSpeed;
Uint8 AgressivityLevel;
Uint8 K56options;
Uint16 TxLevelGain;
Uint16 rate_seq_R1;
Uint16 rate_seq_R2;
Uint16 rate_seq_R3;
Uint16 e_seq;
Uint16 K56_training_cnt;

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the parameters dp_set_v34_parameters last took */
Uint32 v34options;
/* the copy of info dp_set_K56_parameters last took */
struct k56flexinfotag K56flexRemoteInfo;
Uint16 rate_mask;
/* 1 from dp_init_handshake until dp_monitor_handshake sees a connection */
Uint8 handshake_flag;
/* the remote digital loop's phase, an RDL_ value */
Uint8 dp_rdl_phase;
/* 1 while dp_detect_dtmf has heard a DTMF tone whose digit it awaits */
char dtmf_tone_detected;
Uint8 v8bis_dual_detected;
Uint8 v8bis_signal_detected;
Uint8 tst_initial_ASYN;
/* r02's v54pe from before a remote digital loop began */
Uint8 tst_initial_V54PE;
/* tx_level's clock from before a local digital loop began */
Uint8 tst_initial_TXCLK;
/* the time dp_detect_v21_fax waits until, as setup_timer sets it */
Uint32 detect_v21_timer;
/* the frequency in hertz that dp_set_tone_detector gave each of tone
 * detectors A, B and C, or 0 */
Uint16 filter_freq[TONE_DETECTORS];
/* 1 once set_tone_c_1650 has set tone detector C to 1650 Hz, until
 * init_tonec_cc or dp_init_data_pump clears it */
char tonec_at_1650;
/* nothing sets tonec_at_2100 to other than 0 */
char tonec_at_2100;

char dp_hardware_reset(void);
void dp_go_to_stop_mode(void);
void dp_enable_v25_answer_tone(void);
void dp_set_short_train(Uint8 on);
void dpSetCallerId(char on);
void dp_reset_tone_detectors(void);
void dp_set_call_progress_tone_detector(void);
char dp_set_tone_detector(Uint16 frequency);
char dp_test_tone_detector(Uint16 frequency);
void dp_set_dtmf_detector(void);
char dp_detect_dtmf(Uint8 *digit);
void dp_stop_dial_number(void);
void dp_set_idle(void);
void dp_set_K56_parameters(Uint8 options, struct k56flexinfotag *info);
void dp_set_rts_cts_delay(Uint8 delay);
void dp_set_rts_off(void);
void dp_set_rts_on(void);
char dp_get_cts(void);
char dp_get_dcd(void);
void dp_stop_local_analog_loop(void);
void dp_abort_retrain(void);
void dp_set_v21_detector_fax(void);
char dp_detect_v21_fax(void);
char dp_detect_v21_in_high_speed_fax(void);
char dp_facsimile_data_mode(void);
char dp_test_irq_pending(void);
Uint8 dp_get_eqm10(void);
char dp_will_eqm_be_ok(Uint32 unused_1, Uint8 unused_2);
Uint8 dp_get_rx_baud_rate(void);
Uint8 dp_get_tx_baud_rate(void);
/* NOLINTEND(misc-use-internal-linkage) */

static struct dp_reset_info reset_info;
/* dp_analyzer calls target_dpNeedsHookControl only while dat_0cb7039e is 0,
 * and nothing sets it to other than 0 */
static char dat_0cb7039e;
static Uint8 dat_0cb7039f;
/* the periods dp_set_ring_periods last took */
static Uint8 ring_on;
static Uint8 ring_off;
static Uint8 dat_0cb703a2[6];
/* the time 100 ms after the pump takes the last pulse digit, when dp_analyzer
 * passes 0 to target_dpNeedsHookControl */
static Uint32 pulse_timer;
/* nothing sets dat_0cb703ac but to 0 */
static char dat_0cb703ac;
/* the parameters dp_enable_data_mode last took, with which
 * dp_monitor_retrain turns data mode on again */
static Uint8 data_mode;
static Uint8 data_option;
static Uint8 data_speed;
/* the time at which dp_analyzer turns tx_irq back on, once rcflags'
 * tx_irq_deferred is set. nothing sets it, so the time has always passed */
static Uint32 tx_irq_timer;
/* 1 once dp_start_sending_clear_down has started a clear down, and 0 once
 * it found none to start */
static char clear_down_started;
/* how many tone detectors dp_set_tone_detector has set, 0 to 3 */
static Uint8 tone_detectors_set;
/* nothing sets dat_0cb703b6, so dp_read_far_echo_level returns 50 */
static Uint8 dat_0cb703b6;
/* the level dp_set_tx_level last took */
static Uint8 tx_level;
/* how the handshake chooses: HANDSHAKE_AUTO_MODE and HANDSHAKE_RATE_RANGE
 * set r15's auto_mode and rate_range */
static Uint8 handshake_options;
/* what read_rate_sequence last read, for dp_get_rate_sequence */
static Uint32 rate_sequence;
/* a time 5 seconds ahead, as dp_init_data_pump, dp_test_ring and dp_on_hook
 * set it */
static Uint32 hook_timer;
/* the time dp_monitor_retrain waits until, 20 ms after a renegotiation's
 * carrier returns */
static Uint32 retrain_timer;
/* how many retrains and renegotiations this side started, and how many the
 * far end did */
static Uint32 retrains_started;
static Uint32 renegotiations_started;
static Uint32 retrains_seen;
static Uint32 renegotiations_seen;
/* the time 2 seconds into an originating handshake. dp_monitor_handshake may
 * move configuration 0x4c to CONF_V32BIS_14400 before it, or CONF_AUTO after
 * it, once a handshake */
static Uint32 handshake_timer;
/* the time dp_start_sending_clear_down's clear down runs until */
static Uint32 clear_down_timer;
/* the time 3 seconds after a V.90 connection, when dp_analyzer clears RAM
 * 0xb11 */
static Uint32 b11_timer;
/* what read_v8_status read as the last handshake connected, for
 * dp_get_v8_status. dp_init_handshake clears it outside a fax configuration */
static Uint32 v8_status;
/* the time each wait for the pump gives up at */
static Uint32 dp_timer;

static void read_rate_sequence(char trained);
static char is_fax_conf(void);
static Uint32 read_v8_status(void);
static void write_biquad(Uint16 base, Uint16 b0, Uint16 b1, Uint16 b2,
                         Uint16 a1, Uint16 a2);
static void write_tone_detector(Uint16 detector, Uint16 at_ba0, Uint16 at_aa0,
                                Uint16 at_ab8, Uint16 at_bb8);
static void set_tone_c_1650(void);

/* unhook the pump's interrupt handler, and reset the pump in up to three
 * tries. each try holds the reset for 40 ms. it then gives the pump 150 ms to
 * come up and set tx_empty. return 1 once those 150 ms have passed. where no
 * try brings the pump up, report DP_ERR_RESET and return 0. where tx_empty
 * stays clear, report DP_ERR_RESET_TX and return 0 */
char dp_hardware_reset(void)
{
    Uint8 tries = 3;
    Uint8 failed = 0;
    /* @hack spelling: the registers read through reg, a pointer stepped a
     * register at a time, where rcdpi's fields would do */
    const volatile Uint8 *reg;

    target_InstallDpIrq(NULL);
    while (tries) {
        target_dp_reset(1);
        setup_timer(&dp_timer, TICKS(40));
        while (!timer_expired(dp_timer)) {
        }
        target_dp_reset(0);
        setup_timer(&dp_timer, TICKS(150));
        for (;;) {
            if (timer_expired(dp_timer)) {
                failed = 1;
                break;
            }
            reg = &rcdpi->r1a.byte;
            reset_info.ready_1a = *reg;
            reg += DP_REG_STRIDE;
            reset_info.ready_1a |= *reg << 8;
            reg += DP_REG_STRIDE;
            reset_info.ready_1c = *reg;
            reg += DP_REG_STRIDE;
            reset_info.ready_1c |= *reg << 8;
            if (reset_info.ready_1c == READY_1C ||
                reset_info.ready_1a == READY_1A) {
                break;
            }
        }
        tries--;
        if (failed) {
            if (tries) {
                failed = 0;
            }
        } else {
            tries = 0;
        }
    }
    if (failed) {
        data_pump_error(DP_ERR_RESET);
        return 0;
    }
    reg = &rcdpi->r16.byte;
    reset_info.unk_6 = *reg;
    reg += DP_REG_STRIDE;
    reset_info.unk_6 |= *reg << 8;
    reg += DP_REG_STRIDE;
    reset_info.unk_4 = *reg;
    reg += DP_REG_STRIDE;
    reset_info.unk_4 |= *reg << 8;
    /* @hack spelling: register 0x15 read as the register after abcode, where
     * rcdpi->r15.byte would do */
    reg = &rcdpi->abcode.byte;
    reset_info.unk_8 = reg[DP_REG_STRIDE];
    reg = &rcdpi->conf.byte;
    reset_info.unk_a = *reg;
    reg += DP_REG_STRIDE;
    reset_info.unk_a |= *reg << 8;
    reset_info.unk_c = rcdpi->rx_data.byte;
    reg = &rcdpi->r11.byte;
    reset_info.unk_c |= *reg << 8;
    (void)rcdpi->tx_data.byte;
    while (!rcdpi->irq.bit.tx_empty) {
        if (timer_expired(dp_timer)) {
            data_pump_error(DP_ERR_RESET_TX);
            return 0;
        }
    }
    reg = &rcdpi->conf.byte;
    reset_info.unk_16 = *reg;
    reg += DP_REG_STRIDE;
    reset_info.unk_16 |= *reg << 8;
    reg += DP_REG_STRIDE;
    reset_info.unk_14 = *reg;
    reg += DP_REG_STRIDE;
    reset_info.unk_14 |= *reg << 8;
    reg += DP_REG_STRIDE;
    reset_info.unk_12 = *reg;
    reg += DP_REG_STRIDE;
    reset_info.unk_12 |= *reg << 8;
    reg += DP_REG_STRIDE;
    reset_info.unk_10 = *reg;
    reg += DP_REG_STRIDE;
    reset_info.unk_10 |= *reg << 8;
    reg += DP_REG_STRIDE;
    reset_info.unk_e = *reg;
    reg += DP_REG_STRIDE;
    reset_info.unk_e |= *reg << 8;
    reset_info.unk_18 = rcdpi->rx_data.byte;
    reg = &rcdpi->r11.byte;
    reset_info.unk_18 |= *reg << 8;
    (void)rcdpi->tx_data.byte;
    while (!timer_expired(dp_timer)) {
    }
    return 1;
}

void dp_go_to_stop_mode(void)
{
    rcdpi->r15.bit.unk_bit5 = 0;
    rcdpi->r15.bit.unk_bit4 = 0;
    rcdpi->r15.bit.unk_bit7 = 1;
    rcdpi->r15.bit.unk_bit6 = 1;
    rcdpi->control.bit.newc = 1;
}

void dp_reset_data_pump(void)
{
    dp_init_data_pump(43, 48);
}

void dp_init_data_pump(Uint8 unused_1, Uint8 unused_2)
{
    if (dp_hardware_reset()) {
        InitialDownloadRamModule();
        rcdpi->control.bit.unk_bit2 = 0;
        rcdpi->control.bit.status_irq = 0;
        rcdpi->irq.bit.rx_irq = 0;
        rcdpi->irq.bit.tx_irq = 0;
        rcflags.in_interrupt = 0;
        rcflags.unk_0_7 = 0;
        k56flags.auto_conf = 0;
        k56flags.unk_0_4 = 0;
        k56flags.v90_download = 0;
        k56flags.v34_download = 0;
        k56flags.post_v8_download = 0;
        k56flags.unk_1_0 = 0;
        k56flags.unk_0_1 = 0;
        v8_status = 0;
        dp_rdl_phase = RDL_NONE;
        tonec_at_1650 = 0;
        tonec_at_2100 = 0;
        dp_reset_tone_detectors();
        rcdpi->r09.bit.unk_bit2 = 0;
        rcdpi->r05.bit.unk_bit4 = 0;
        write_dp_ram(0x6a9, 7, 1);
        setup_timer(&hook_timer, TICKS(5000));
        dat_0cb7039e = 0;
        rcdpi->r09.bit.originate = 1;
        newc();
    }
}

void dp_set_originate(void)
{
    rcdpi->r09.bit.originate = 1;
    target_dpNeedsHookControl(0);
}

void dp_set_answer(void)
{
    rcdpi->r09.bit.originate = 0;
    target_dpNeedsHookControl(0);
}

void dp_set_tx_level(Uint8 level)
{
    Uint16 attenuation;

    tx_level = level;
    if (level > LEVEL_OFFSET) {
        level -= LEVEL_OFFSET;
    } else {
        level = 0;
    }
    if (level > MOST_ATTENUATION) {
        attenuation = level - MOST_ATTENUATION;
        /* @hack type: the bound written 50U, which makes the comparison
         * unsigned, where 50 would do */
        if (attenuation > 50U) {
            attenuation = 50;
        }
        /* @bug memory: reads past the end of tx_gains, which holds 25 gains,
         * for an attenuation of 26 to 50 dB, at 0x0cae458c */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: an attenuation past 25 dB
         * takes tx_gains' last gain */
        if (attenuation > sizeof(tx_gains) / sizeof(tx_gains[0])) {
            attenuation = sizeof(tx_gains) / sizeof(tx_gains[0]);
        }
#endif
        TxLevelGain = tx_gains[attenuation - 1];
        write_dp_ram(RAM_TX_GAIN, TxLevelGain, 2);
        rcdpi->tx_level.bit.attenuation = MOST_ATTENUATION;
    } else {
        TxLevelGain = UNITY_GAIN;
        write_dp_ram(RAM_TX_GAIN, UNITY_GAIN, 2);
        rcdpi->tx_level.bit.attenuation = level;
    }
    newc();
}

void dp_set_ceq(Uint8 on)
{
    if (on) {
        rcdpi->r05.bit.ceq = 1;
    } else {
        rcdpi->r05.bit.ceq = 0;
    }
}

void dp_set_guard_tone(Uint8 tone)
{
    if (tone == 0) {
        rcdpi->r03.bit.guard = 0;
    } else {
        rcdpi->r03.bit.guard = 1;
        if (tone == 1) {
            rcdpi->r03.bit.guard_select = 1;
        } else if (tone == 2) {
            rcdpi->r03.bit.guard_select = 0;
        }
    }
}

void dp_set_v13(void)
{
}

void dp_disable_v25_answer_tone(void)
{
    rcdpi->r09.bit.no_answer_tone = 1;
}

void dp_enable_v25_answer_tone(void)
{
    rcdpi->r09.bit.no_answer_tone = 0;
}

void dp_set_short_train(Uint8 on)
{
    if (on) {
        rcdpi->r04.bit.short_train = 1;
    } else {
        rcdpi->r04.bit.short_train = 0;
    }
}

/* for an on of 1, start an originating handshake in V.23 to receive caller
 * ID. any other on clears r09's unk_bit2 */
void dpSetCallerId(char on)
{
    if (on == 1) {
        dp_set_originate();
        dp_set_modulation_type(MOD_V23, 1, 1, 4, 4);
        rcdpi->r05.bit.ceq = 0;
        dp_init_handshake();
        rcdpi->r05.bit.unk_bit4 = 1;
    } else {
        rcdpi->r09.bit.unk_bit2 = 0;
    }
}

void dp_reset_tone_detectors(void)
{
    tone_detectors_set = 0;
    filter_freq[TONE_DETECTOR_A] = 0;
    filter_freq[TONE_DETECTOR_B] = 0;
    filter_freq[TONE_DETECTOR_C] = 0;
    rcdpi->r02.bit.tone_c = 0;
}

void dp_set_call_progress_tone_detector(void)
{
    dp_set_tone_detector(245);
}

/* set the next of tone detectors A, B and C to hear frequency, and return 1.
 * return 0 once all three are set. the call tunes two of the detector's
 * filters to 245, 1100, 1300, 2100 or 2225 Hz alone. for any other frequency,
 * they stay as they were */
char dp_set_tone_detector(Uint16 frequency)
{
    Uint16 first;
    Uint16 second;

    if (tone_detectors_set < TONE_DETECTORS) {
        filter_freq[tone_detectors_set] = frequency;
        switch (tone_detectors_set++) {
        case TONE_DETECTOR_A:
            first = TONE_A_FILTER;
            second = TONE_A_FILTER + SECOND_FILTER;
            write_tone_detector(TONE_DETECTOR_A, 0x7f30, 0x00cf, 0x0880,
                                0x0580);
            break;
        case TONE_DETECTOR_B:
            first = TONE_B_FILTER;
            second = TONE_B_FILTER + SECOND_FILTER;
            write_tone_detector(TONE_DETECTOR_B, 0x7e67, 0x02df, 0x2a00,
                                0x1c00);
            break;
        case TONE_DETECTOR_C:
            rcdpi->r02.bit.tone_c = 1;
            first = TONE_C_FILTER;
            second = TONE_C_FILTER + SECOND_FILTER;
            write_biquad(TONE_C_PASS_FILTER, 0, 0, 0x7fff, 0, 0);
            write_biquad(TONE_C_PASS_FILTER + SECOND_FILTER, 0, 0, 0x7fff, 0,
                         0);
            write_tone_detector(TONE_DETECTOR_C, 0x7e67, 0x02df, 0x2a00,
                                0x1c00);
            break;
        }
        switch (frequency) {
        case 245:
            write_biquad(first, 0x08c2, 0xee7c, 0x08c2, 0xc774, 0x74fe);
            write_biquad(second, 0x08c2, 0x1184, 0x08c2, 0xcd4f, 0x6495);
            break;
        case 1100:
            write_biquad(first, 0x01b3, 0xfc9c, 0x01b4, 0xc147, 0x48c6);
            write_biquad(second, 0x01b3, 0x0097, 0x01b4, 0xc147, 0x4897);
            break;
        case 1300:
            write_biquad(first, 0x0244, 0xfb7b, 0x0245, 0xc147, 0x35a7);
            write_biquad(second, 0x0244, 0x00ca, 0x0245, 0xc147, 0x3574);
            break;
        case 2100:
            write_biquad(first, 0x01e8, 0xfc32, 0x01e9, 0xc147, 0xdf4f);
            write_biquad(second, 0x01e8, 0x034e, 0x01e9, 0xc147, 0xdf19);
            break;
        case 2225:
            write_biquad(first, 0x0205, 0xfbf9, 0x0206, 0xc147, 0xd22d);
            write_biquad(second, 0x0205, 0x0380, 0x0206, 0xc147, 0xd1f8);
            break;
        }
        return 1;
    }
    return 0;
}

/* return 1 while a tone detector set to frequency hears it, and 0 otherwise.
 * in CONF_TONE, CONF_SILENCE and CONF_DIAL, detector B at 2100 Hz reads tones'
 * answer_2100 instead. so does detector C at 2225 Hz, with answer_2225. return
 * 0 while dtmf's unk_bit7 or tone_on is set */
char dp_test_tone_detector(Uint16 frequency)
{
    /* @hack spelling: each bit compared with 0, where the bit alone would do */
    /* @hack redundant: the else after the arm that returns */
    if (rcdpi->dtmf.bit.unk_bit7 != 0 || rcdpi->dtmf.bit.tone_on != 0) {
        return 0;
    } else if (rcdpi->conf.byte == CONF_TONE ||
               rcdpi->conf.byte == CONF_SILENCE ||
               rcdpi->conf.byte == CONF_DIAL) {
        if (frequency == 2100 && filter_freq[TONE_DETECTOR_B] == 2100) {
            if (rcdpi->tones.bit.answer_2100) {
                return 1;
            }
            return 0;
        }
        if (frequency == 2225 && filter_freq[TONE_DETECTOR_C] == 2225) {
            if (rcdpi->tones.bit.answer_2225) {
                return 1;
            }
            return 0;
        }
    }
    if (frequency == filter_freq[TONE_DETECTOR_A] && rcdpi->tones.bit.tone_a) {
        return 1;
    }
    if (frequency == filter_freq[TONE_DETECTOR_B] && rcdpi->tones.bit.tone_b) {
        return 1;
    }
    if (frequency == filter_freq[TONE_DETECTOR_C] && rcdpi->tones.bit.tone_c) {
        return 1;
    }
    return 0;
}

Uint8 dp_answer_tone_filter_test(void)
{
    if (!tonec_at_2100) {
        if (rcdpi->tones.bit.answer_2100) {
            return HEARD_2100HZ;
        }
    } else {
        if (rcdpi->tones.bit.tone_c) {
            return HEARD_2100HZ;
        }
    }
    /* @hack redundant: the else after the arm that returns */
    if (rcdpi->tones.bit.tone_b) {
        return HEARD_2225HZ;
    } else if (!tonec_at_2100 && !tonec_at_1650 && rcdpi->tones.bit.tone_c) {
        return HEARD_1300HZ;
    }
    return 0;
}

void dp_set_dtmf_detector(void)
{
    dtmf_tone_detected = 0;
}

/* once a DTMF tone has sounded and its digit is ready, store the digit in
 * *digit. the digit is '0' to '9', '*', '#' or 'A' to 'D'. return 1 then,
 * and 0 otherwise */
char dp_detect_dtmf(Uint8 *digit)
{
    if (rcdpi->dtmf.bit.tone_on) {
        if (dtmf_tone_detected == 0) {
            dtmf_tone_detected = 1;
        }
    } else if (rcdpi->dtmf.bit.digit_ready && dtmf_tone_detected == 1) {
        if (rcdpi->dtmf.bit.digit <= 9) {
            *digit = rcdpi->dtmf.bit.digit + '0';
        } else if (rcdpi->dtmf.bit.digit == 10) {
            *digit = '*';
        } else if (rcdpi->dtmf.bit.digit == 11) {
            *digit = '#';
        } else {
            *digit = rcdpi->dtmf.bit.digit + 'A' - 12;
        }
        dtmf_tone_detected = 0;
        rcdpi->dtmf.bit.digit_ready = 0;
        return 1;
    } else {
        dtmf_tone_detected = 0;
    }
    return 0;
}

void dp_start_tone_generator(Uint16 frequency)
{
    switch (frequency) {
    case 1100:
        write_dp_ram(RAM_TONE_1 + 1, 39, 1);
        write_dp_ram(RAM_TONE_1, 28, 1);
        break;
    case 2100:
        write_dp_ram(RAM_TONE_1 + 1, 74, 1);
        write_dp_ram(RAM_TONE_1, 0xab, 1);
        break;
    case 1300:
        write_dp_ram(RAM_TONE_1 + 1, 46, 1);
        write_dp_ram(RAM_TONE_1, 56, 1);
        break;
    }
    rcdpi->conf.byte = CONF_TONE;
    rcdpi->r08.bit.rts = 1;
    newc();
}

void dp_stop_tone_generator(void)
{
    rcdpi->r08.bit.rts = 0;
    write_dp_ram(RAM_TONE_1 + 1, 0, 1);
    write_dp_ram(RAM_TONE_1, 0, 1);
    write_dp_ram(RAM_TONE_2 + 1, 0, 1);
    write_dp_ram(RAM_TONE_2, 0, 1);
    rcdpi->conf.byte = CONF_SILENCE;
    newc();
}

void dp_set_dtmf(Uint8 level, Uint8 table)
{
    rcdpi->r09.bit.dtmf = 1;
    newc();
    if (level > LEVEL_OFFSET) {
        level -= LEVEL_OFFSET;
    } else {
        level = 0;
    }
    if (level > MOST_ATTENUATION) {
        level = MOST_ATTENUATION;
    }
    switch (table) {
    case 1:
        write_dp_ram(0x29b, dtmf_29b_1[level], 2);
        write_dp_ram(0x29c, dtmf_29b_1[level] >> 8, 2);
        write_dp_ram(0x29d, dtmf_29d_1[level], 2);
        write_dp_ram(0x29e, dtmf_29d_1[level] >> 8, 2);
        break;
    case 2:
        write_dp_ram(0x29b, dtmf_29b_2[level], 2);
        write_dp_ram(0x29c, dtmf_29b_2[level] >> 8, 2);
        write_dp_ram(0x29d, dtmf_29d_2[level], 2);
        write_dp_ram(0x29e, dtmf_29d_2[level] >> 8, 2);
        break;
    case 3:
        write_dp_ram(0x29b, dtmf_29b_3[level], 2);
        write_dp_ram(0x29c, dtmf_29b_3[level] >> 8, 2);
        write_dp_ram(0x29d, dtmf_29d_3[level], 2);
        write_dp_ram(0x29e, dtmf_29d_3[level] >> 8, 2);
        break;
    default:
        write_dp_ram(0x29b, dtmf_29b_2[level], 2);
        write_dp_ram(0x29c, dtmf_29b_2[level] >> 8, 2);
        write_dp_ram(0x29d, dtmf_29d_2[level], 2);
        write_dp_ram(0x29e, dtmf_29d_2[level] >> 8, 2);
        break;
    }
}

void dp_set_pulse(void)
{
    rcdpi->r09.bit.dtmf = 0;
    newc();
    target_dpNeedsHookControl(1);
}

void dp_set_dtmf_duration(Uint16 on_time, Uint16 off_time)
{
    Uint16 count;

    /* @hack type: on_time and off_time read as Sint16, where their Uint16 would
     * do for times below 0x8000 */
    count = (Sint16)on_time * 5 >> 1;
    write_dp_ram(RAM_DTMF_ON, count, 1);
    write_dp_ram(RAM_DTMF_ON_HIGH, count >> 8, 1);
    count = (Sint16)off_time * 5 >> 1;
    write_dp_ram(RAM_DTMF_OFF, count, 1);
    write_dp_ram(RAM_DTMF_OFF_HIGH, count >> 8, 1);
}

void dp_set_make_break_ratio(Uint16 make, Uint16 brk, Uint16 pause)
{
    Uint16 count;

    /* @hack type: each time read as Sint16, where its Uint16 would do for
     * times below 0x8000 */
    count = (Sint16)make * 24 / 10;
    write_dp_ram(RAM_PULSE_MAKE, count, 1);
    count = (Sint16)brk * 24 / 10;
    write_dp_ram(RAM_PULSE_BREAK, count, 1);
    count = (Sint16)pause * 24 / 10;
    write_dp_ram(RAM_PULSE_PAUSE, count, 1);
    write_dp_ram(RAM_PULSE_PAUSE + 1, count >> 8, 1);
}

void dp_set_dial_mode(void)
{
    rcdpi->conf.byte = CONF_DIAL;
    newc();
    setup_timer(&dp_timer, TICKS(20));
    while (!timer_expired(dp_timer)) {
    }
}

char dp_ready_to_dial(void)
{
    return rcdpi->irq.bit.tx_empty;
}

void dp_dial_number(Uint8 digit)
{
    if (!rcdpi->r09.bit.dtmf) {
        target_dpNeedsHookControl(1);
        setup_timer(&pulse_timer, TICKS(100));
    }
    rcdpi->tx_data.byte = dtmf_codes[digit];
}

void dp_stop_dial_number(void)
{
    rcdpi->tx_data.byte = 0xff;
}

static void write_connect_ram(void)
{
    write_dp_ram(0x26b, 32, 1);
    write_dp_ram(0x26c, 100, 1);
    write_dp_ram(0x26d, 96, 1);
}

/* return the value of table's greatest key at or below value. a value below
 * every key returns 0. table holds count pairs in rising order of key.
 * floor_word_pair does the same for a table of words */
static Uint8 floor_byte_pair(const struct byte_pair *table, Uint16 value,
                             Uint16 count)
{
    Uint16 i;

    if (value < table[0].key) {
        return 0;
    }
    for (i = 0; i < count; i++) {
        /* @hack redundant: the else after the arm that returns */
        if (value == table[i].key) {
            return table[i].value;
        } else if (value < table[i].key) {
            return table[i - 1].value;
        }
    }
    return table[count - 1].value;
}

/* return the value of table's least key at or above value. a value above
 * every key returns 0. table holds count pairs in rising order of key. a value
 * between the first two keys would take the first key's value. each table's
 * first two keys are consecutive codes, so no value falls there */
static Uint16 ceil_word_pair(const struct word_pair *table, Uint16 value,
                             Uint16 count)
{
    Uint16 i;

    if (value > table[count - 1].key) {
        return 0;
    }
    for (i = count - 1; i > 0; i--) {
        /* @hack redundant: the else after the arm that returns */
        if (value == table[i].key) {
            return table[i].value;
        } else if (value > table[i].key) {
            return table[i + 1].value;
        }
    }
    return table[0].value;
}

static Uint16 floor_word_pair(const struct word_pair *table, Uint16 value,
                              Uint16 count)
{
    Uint16 i;

    if (value < table[0].key) {
        return 0;
    }
    for (i = 0; i < count; i++) {
        /* @hack redundant: the else after the arm that returns */
        if (value == table[i].key) {
            return table[i].value;
        } else if (value < table[i].key) {
            return table[i - 1].value;
        }
    }
    return table[count - 1].value;
}

/* store in *conf the configuration of the highest modulation of mask that
 * fits the rate codes given. write the masks of those rates to RAM. *conf
 * keeps its value where nothing in mask fits */
static void select_conf(Uint32 mask, Uint8 min_rate, Uint8 max_rate,
                        Uint8 min_rx_rate, Uint8 max_rx_rate, Uint8 *conf)
{
    char v32;
    char v32bis;
    Uint16 bits;
    Uint8 rate;
    Uint16 rates;
    Uint8 keep;

    /* @hack spelling: v32bis and v32 cleared in one statement, where two
     * would do */
    v32bis = v32 = 0;
    if (min_rate == 0 || min_rate > max_rate) {
        min_rate = 1;
    }
    if (min_rx_rate == 0 || min_rx_rate > max_rx_rate) {
        min_rx_rate = 1;
    }
    rcdpi->r03.bit.unk_bit7 = 0;
    if (mask & 0x00ff0000) {
        if (mask == 0x00040000) {
            if (max_rate >= 6) {
                *conf = 0x02;
            } else {
                *conf = 0x01;
            }
            rcdpi->r03.bit.unk_bit7 = 1;
        } else if (mask == 0x00080000) {
            if (max_rate >= 8) {
                *conf = 0x14;
            } else if (max_rate >= 7) {
                *conf = 0x12;
            } else {
                *conf = 0x11;
            }
        } else if (mask == 0x00100000) {
            if (max_rate >= 12) {
                *conf = 0xb1;
            } else if (max_rate >= 11) {
                *conf = 0xb2;
            } else if (max_rate >= 8) {
                *conf = 0xb4;
            } else if (max_rate >= 7) {
                *conf = 0xb8;
            }
            rcdpi->r03.bit.unk_bit7 = 1;
        } else if (mask == 0x00020000) {
            if (max_rate >= 2) {
                *conf = 0xa8;
            }
            /* @hack spelling: an empty arm for mask 0x400000, where testing
             * mask != 0x400000 for the return would do */
        } else if (mask == 0x00400000) {
        } else {
            return;
        }
    } else {
        if ((mask & ~MOD_V21) == MOD_V23) {
            if (max_rate == 4) {
                *conf = 0xa4;
            } else {
                *conf = 0xa1;
            }
            return;
        }
        if (mask & MOD_BELL) {
            if (max_rate >= 4) {
                *conf = 0x62;
            } else {
                *conf = 0x60;
            }
        }
        if ((mask & MOD_V21) && max_rate >= 2 && min_rate <= 2) {
            *conf = CONF_V21;
        }
        if ((mask & MOD_V22) && max_rate >= 3 && min_rate <= 4) {
            if (max_rate > 3) {
                *conf = 0x52;
            } else {
                *conf = 0x51;
            }
        }
        if ((mask & MOD_V22BIS) && max_rate >= 4 && min_rate <= 5) {
            if (max_rate > 4) {
                *conf = 0x84;
            } else {
                *conf = 0x82;
            }
        }
        if ((mask & MOD_V32) && max_rate >= 6 && min_rate <= 8) {
            v32 = 1;
            bits = 0x111;
            if (min_rate <= 6) {
                bits = 0x511;
                *conf = 0x71;
            }
            if (max_rate >= 8) {
                bits |= 0x200;
                *conf = 0x75;
            }
            /* @bug logic: tests bit 0x800000 of mask where bits 16 to 23 are
             * clear, so its store of 0x74 never runs, at 0x0cae52a4 */
            if (max_rate >= 8 && (mask & 0x00800000)) {
                bits &= 0x80;
                *conf = 0x74;
            }
        }
        if ((mask & MOD_V32BIS) && max_rate >= 6 && min_rate <= 12) {
            v32 = 1;
            v32bis = 1;
            bits = ceil_word_pair(v32_min_masks, min_rate, 5);
            bits &= floor_word_pair(v32_max_masks, max_rate, 5);
            if (bits & 8) {
                *conf = CONF_V32BIS_14400;
            } else if (bits & 0x20) {
                *conf = 0x72;
            } else if (bits & 0x200) {
                *conf = 0x74;
            } else if (bits & 0x40) {
                *conf = 0x78;
            } else if (bits & 0x400) {
                *conf = 0x71;
            }
        }
        if (v32) {
            if (v32bis) {
                write_dp_ram(RAM_V32_RATES + 4, bits, 1);
                write_dp_ram(RAM_V32_RATES + 5, bits >> 8, 1);
                write_dp_ram(RAM_V32_RATES + 6, bits, 1);
                write_dp_ram(RAM_V32_RATES + 7, bits >> 8, 1);
            } else {
                bits &= (Uint16)~V32BIS_RATES;
            }
            write_dp_ram(RAM_V32_RATES, bits, 1);
            write_dp_ram(RAM_V32_RATES + 1, bits >> 8, 1);
            write_dp_ram(RAM_V32_RATES + 2, bits, 1);
            write_dp_ram(RAM_V32_RATES + 3, bits >> 8, 1);
        }
        if ((mask & MOD_V34) && max_rate >= RATE_V34_MIN &&
            min_rate <= RATE_V34_MAX && max_rx_rate >= RATE_V34_MIN &&
            min_rx_rate <= RATE_V34_MAX) {
            rate = floor_byte_pair(v34_rate_indexes, max_rx_rate, 15);
            MaxV34ReceiveSpeed = rate;
            write_dp_ram(RAM_V34_CONF, rate | CONF_V34, 1);
            *conf = rate | CONF_V34;
            rate = floor_byte_pair(v34_rate_indexes, max_rate, 15);
            MaxV34TransmitSpeed = rate;
            rates = ceil_word_pair(v34_min_masks, min_rx_rate, 15);
            write_dp_ram(RAM_V34_RATES, rates, 1);
            write_dp_ram(RAM_V34_RATES + 1, rates >> 8, 1);
            write_dp_ram(0x304, 0xc8, 1);
            write_dp_ram(0x6a3, 0, 1);
            if (!dp_carrier_detect() && !rcdpi->r09.bit.no_answer_tone) {
                *conf = CONF_AUTO;
            } else {
                write_dp_ram(RAM_MAX_RX_RATE, MaxV34ReceiveSpeed, 1);
            }
        }
        if ((mask & MOD_V90) && max_rate >= RATE_V34_MIN &&
            min_rate <= RATE_V34_MAX && max_rx_rate >= RATE_V90_RX_MIN &&
            min_rx_rate <= RATE_V90_RX_MAX) {
            keep = read_dp_ram(0x6a3, 1) & 0x40;
            write_dp_ram(0x6a3, keep | 5, 1);
            write_dp_ram(0x304, 0xd8, 1);
            rate = floor_byte_pair(v90_rate_indexes, max_rx_rate, 22);
            write_dp_ram(RAM_V90_CONF, rate | CONF_V90, 1);
            *conf = rate | CONF_V90;
            MaxV90ReceiveSpeed = rate;
            rates = ceil_word_pair(v34_min_masks, min_rate, 15);
            rates &= floor_word_pair(v34_max_masks, max_rate, 15);
            write_dp_ram(RAM_V34_RATES, rates, 1);
            write_dp_ram(RAM_V34_RATES + 1, rates >> 8, 1);
            if (!dp_carrier_detect() && !rcdpi->r09.bit.no_answer_tone) {
                *conf = CONF_AUTO;
            } else {
                write_dp_ram(RAM_MAX_RX_RATE, MaxV90ReceiveSpeed, 1);
            }
        }
    }
}

void dp_set_modulation_type(Uint32 mask, Uint8 min_rate, Uint8 max_rate,
                            Uint8 min_rx_rate, Uint8 max_rx_rate)
{
    Uint8 conf = 0;

    if (max_rate == 0) {
        max_rate = RATE_V34_MAX;
    }
    if (max_rx_rate == 0) {
        max_rx_rate = RATE_V90_RX_MAX;
    }
    if ((mask & (MOD_K56 | MOD_V90)) && !(v34options & V34_ALLOW_K56_V90)) {
        mask &= ~MOD_V90;
        mask &= ~MOD_K56;
        mask |= MOD_V34;
    }
    select_conf(mask, min_rate, max_rate, min_rx_rate, max_rx_rate, &conf);
    if (!conf) {
        conf = CONF_V21;
    }
    /* @hack spelling: rate_range compared with 1, where its negation would
     * do */
    if (!dp_carrier_detect() || rcdpi->r15.bit.rate_range != 1) {
        rcdpi->conf.byte = conf;
    }
    if (conf == CONF_AUTO) {
        k56flags.auto_conf = 1;
    }
    if (!is_fax_conf()) {
        handshake_options = 0;
        if (v25terFindMaxBit(mask) != mask) {
            if (conf == CONF_AUTO ||
                (rcdpi->conf.byte >= 0x71 && rcdpi->conf.byte <= 0x76)) {
                handshake_options |= HANDSHAKE_AUTO_MODE;
            }
        }
        if (min_rate != max_rate || min_rx_rate != max_rx_rate) {
            handshake_options |= HANDSHAKE_RATE_RANGE;
        }
        if (!dp_carrier_detect()) {
            newc();
        }
    } else {
        rcdpi->r09.bit.unk_bit0 = 0;
        rcdpi->r08.bit.rts = 0;
    }
}

void dp_init_handshake(void)
{
    Uint8 value;

    rcflags.interrupt_on_carrier_loss = 0;
    rcflags.carrier_seen = 0;
    if (!is_fax_conf()) {
        v8_status = 0;
        handshake_status = 0;
        handshake_flag = 1;
        rcflags.retraining = 0;
        rcflags.renegotiating = 0;
        rcflags.retrain_started = 0;
        rcflags.renegotiation_started = 0;
        rcflags.unk_1_3 = 0;
        rcflags.v32bis_fallback_checked = 0;
        InitWaVariables();
        rcdpi->r05.bit.unk_bit4 = 0;
        if (handshake_options & HANDSHAKE_AUTO_MODE) {
            rcdpi->r15.bit.auto_mode = 1;
            rcdpi->r09.bit.unk_bit2 = 0;
        } else {
            rcdpi->r15.bit.auto_mode = 0;
            rcdpi->r09.bit.unk_bit2 = 1;
        }
        if (handshake_options & HANDSHAKE_RATE_RANGE) {
            rcdpi->r03.bit.unk_bit3 = 1;
            rcdpi->r15.bit.rate_range = 1;
            value = read_dp_ram(RAM_RATE_RANGE_CONTROL, 1);
            write_dp_ram(RAM_RATE_RANGE_CONTROL, value | 0x10, 1);
        } else {
            rcdpi->r03.bit.unk_bit3 = 0;
            rcdpi->r15.bit.rate_range = 0;
        }
        rcdpi->r09.bit.unk_bit0 = 1;
        rcdpi->r08.bit.rts = 1;
        if ((rcdpi->conf.byte & CONF_CLASS) == CONF_V21 &&
            !rcdpi->r09.bit.originate) {
            write_dp_ram(0xa04, 0, 2);
        }
        rcflags.saved_rate_range = rcdpi->r15.bit.rate_range;
        value = read_dp_ram(0x10d, 1);
        write_dp_ram(0x10d, value | 8, 1);
        if (rcdpi->r09.bit.originate) {
            setup_timer(&handshake_timer, TICKS(2000));
        }
    } else {
        rcdpi->r05.bit.ceq = 0;
        rcdpi->r09.bit.unk_bit2 = 1;
        /* @hack spelling: the bit stored through ?:, where
         * !originate would do */
        rcdpi->r05.bit.unk_bit4 = rcdpi->r09.bit.originate ? 0 : 1;
        newc();
        rcdpi->r08.bit.unk_bit6 = 1;
        if (!rcdpi->r09.bit.originate &&
            (rcdpi->conf.byte == 0x02 || rcdpi->conf.byte == 0x01)) {
            rcflags.interrupt_on_carrier_loss = 1;
        }
        if (rcdpi->r09.bit.originate) {
            /* @hack redundant: an empty loop of one pass before the store */
            do {
                break;
            } while (1);
            rcdpi->r08.bit.rts = 1;
            setup_timer(&dp_timer, TICKS(10));
            while (!timer_expired(dp_timer)) {
            }
            /* @hack redundant: a read of rcflags.interrupt_on_carrier_loss,
             * which nothing uses */
            (void)rcflags.interrupt_on_carrier_loss;
        }
    }
}

Uint8 dp_monitor_handshake(void)
{
    Uint8 value;
    char retrain;
    int status;

    MonitorABcode();
    if (!rcflags.ceq_checked && !rcdpi->r15.bit.auto_mode &&
        rcdpi->conf.byte != CONF_AUTO) {
        if (rcdpi->conf.byte == 0xa1 || rcdpi->conf.byte == 0xa4 ||
            rcdpi->conf.byte == 0x60 || rcdpi->conf.byte == CONF_V21 ||
            rcdpi->conf.byte == 0xa8) {
            rcdpi->r05.bit.ceq = 0;
        }
        rcflags.ceq_checked = 1;
    }
    if (rcdpi->conf.byte == CONF_K56 || rcdpi->conf.byte == CONF_V32 ||
        rcdpi->conf.byte == 0xaf || rcdpi->conf.byte == CONF_V34 ||
        rcdpi->conf.byte == 0x40) {
        handshake_status = HANDSHAKE_FAILED;
        return TRAIN_FAILED;
    }
    if (dp_carrier_detect() && rcdpi->status.bit.cts &&
        !rcdpi->speed.bit.retrain) {
        handshake_status = HANDSHAKE_CONNECTED;
        handshake_flag = 0;
        rcdpi->r15.bit.rate_range = rcflags.saved_rate_range;
        if ((rcdpi->conf.byte & CONF_V90) == CONF_V90 ||
            (rcdpi->conf.byte & CONF_CLASS) == CONF_K56 ||
            (rcdpi->conf.byte & CONF_CLASS) == CONF_V34) {
            value = read_dp_ram(0x600, 1);
            write_dp_ram(0x459, value, 1);
            value = read_dp_ram(0x601, 1);
            write_dp_ram(0x45b, value, 1);
        }
        if ((rcdpi->conf.byte & CONF_V90) == CONF_V90 && !k56flags.v90_as_v34) {
            setup_timer(&b11_timer, TICKS(3000));
            k56flags.b11_timing = 1;
        }
        read_rate_sequence(1);
        write_connect_ram();
        WaV34Disc();
        v8_status = read_v8_status();
        return TRAIN_CONNECTED;
    }
    if (k56flags.auto_conf && (rcdpi->conf.byte & CONF_CLASS) != CONF_V21) {
        k56flags.auto_conf = 0;
        k56flags.unk_1_0 = 1;
        if ((rcdpi->conf.byte & CONF_V90) == CONF_V90) {
            rcdpi->tx_level.bit.attenuation = V90_ATTENUATION;
            k56flags.unk_1_7 = 1;
            k56flags.post_v8_download = 1;
            k56flags.v90_download = 1;
        } else if ((rcdpi->conf.byte & CONF_CLASS) == CONF_V34) {
            k56flags.post_v8_download = 1;
            k56flags.v34_download = 1;
        }
    }
    if (k56flags.post_v8_download) {
        status = PostV8DownloadRamModule();
        if (status == RAM_DOWNLOAD_WAITING) {
            return TRAIN_RUNNING;
        }
        if (status == RAM_DOWNLOAD_DONE) {
            k56flags.post_v8_download = 0;
            return TRAIN_RUNNING;
        }
        if (status == RAM_DOWNLOAD_FAILED) {
            k56flags.post_v8_download = 0;
            return TRAIN_FAILED;
        }
    }
    if (!k56flags.post_v8_download && k56flags.unk_1_0) {
        k56flags.unk_1_0 = 0;
    }
    if ((rcdpi->conf.byte & CONF_CLASS) == CONF_V21) {
        MonitorV8();
    } else if ((rcdpi->conf.byte & CONF_V90) == CONF_V90 &&
               !k56flags.v90_as_v34 && !k56flags.post_v8_download) {
        MonitorV90Train();
    } else if ((rcdpi->conf.byte & CONF_CLASS) == CONF_K56 &&
               !k56flags.k56_as_v34 && !k56flags.post_v8_download) {
        MonitorK56Train();
    } else if (((rcdpi->conf.byte & CONF_CLASS) == CONF_V34 &&
                !k56flags.post_v8_download) ||
               k56flags.k56_as_v34 || k56flags.v90_as_v34) {
        MonitorV34Train();
    } else if ((rcdpi->conf.byte & CONF_CLASS) == CONF_V32) {
        MonitorV32Train();
        if (handshake_status == HANDSHAKE_FAILED) {
            return TRAIN_FAILED;
        }
    }
    if (rcdpi->r09.bit.originate && !rcflags.v32bis_fallback_checked) {
        retrain = 0;
        if (!timer_expired(handshake_timer)) {
            if (rcdpi->conf.byte == 0x4c) {
                retrain = 1;
            }
            /* @hack spelling: auto_mode and unk_bit2 each compared with 1,
             * where the bit alone would do */
        } else if (rcdpi->r15.bit.auto_mode == 1 &&
                   rcdpi->r09.bit.unk_bit2 == 1 &&
                   rcdpi->conf.byte == CONF_AUTO &&
                   !(read_dp_ram(RAM_V8_STATUS, 1) & 8)) {
            retrain = 1;
        } else {
            rcflags.v32bis_fallback_checked = 1;
        }
        /* @hack spelling: retrain compared with 1, where the flag alone would
         * do */
        if (retrain == 1) {
            rcdpi->r15.bit.auto_mode = 1;
            rcdpi->r09.bit.unk_bit2 = 0;
            rcdpi->conf.byte = CONF_V32BIS_14400;
            newc();
            write_dp_ram(0x2ec, 1, 1);
            rcflags.v32bis_fallback_checked = 1;
        }
    }
    if (handshake_status) {
        return TRAIN_RUNNING;
    }
    return TRAIN_PENDING;
}

void dp_get_modulation_type(Uint32 *mask, Uint8 *tx_rate, Uint8 *rx_rate)
{
    if ((rcdpi->conf.byte & CONF_V90) == CONF_V90 && !k56flags.v90_as_v34) {
        *mask = MOD_V90;
        *tx_rate = v34_rates[read_dp_ram(RAM_TX_RATE, 1)];
        *rx_rate = v90_rates[read_dp_ram(RAM_RX_RATE, 1)];
    } else if ((rcdpi->conf.byte & CONF_CLASS) == CONF_K56 &&
               !k56flags.k56_as_v34) {
        *mask = MOD_K56;
        *tx_rate = v34_rates[read_dp_ram(RAM_TX_RATE, 1)];
        *rx_rate = k56_rates[read_dp_ram(RAM_RX_RATE, 1)];
    } else if ((rcdpi->conf.byte & CONF_CLASS) == CONF_V34 ||
               k56flags.k56_as_v34 || k56flags.v90_as_v34) {
        *mask = MOD_V34;
        *tx_rate = v34_rates[read_dp_ram(RAM_TX_RATE, 1)];
        *rx_rate = v34_rates[read_dp_ram(RAM_RX_RATE, 1)];
    } else {
        switch (rcdpi->conf.byte) {
        case CONF_V32BIS_14400:
            *mask = MOD_V32BIS;
            *tx_rate = *rx_rate = 12;
            break;
        case 0x72:
            *mask = MOD_V32BIS;
            *tx_rate = *rx_rate = 11;
            break;
        case 0x74:
            *mask = rate_seq_R3 & V32BIS_RATES ? MOD_V32BIS : MOD_V32;
            *tx_rate = *rx_rate = 8;
            break;
        case 0x75:
            *mask = MOD_V32;
            *tx_rate = *rx_rate = 8;
            break;
        case 0x78:
            *mask = MOD_V32BIS;
            *tx_rate = *rx_rate = 7;
            break;
        case 0x71:
            *mask = rate_seq_R3 & V32BIS_RATES ? MOD_V32BIS : MOD_V32;
            *tx_rate = *rx_rate = 6;
            break;
        case 0x84:
            *mask = MOD_V22BIS;
            *tx_rate = *rx_rate = 5;
            break;
        case 0x82:
            *mask = MOD_V22BIS;
            *tx_rate = *rx_rate = 4;
            break;
        case 0x52:
            *mask = MOD_V22;
            *tx_rate = *rx_rate = 4;
            break;
        case 0xa4:
            *mask = MOD_V23;
            *tx_rate = 4;
            *rx_rate = 1;
            break;
        case 0xa1:
            *mask = MOD_V23;
            *tx_rate = 1;
            *rx_rate = 4;
            break;
        case CONF_V21:
            *mask = MOD_V21;
            *tx_rate = *rx_rate = 2;
            break;
        case 0x62:
            *mask = MOD_BELL;
            *tx_rate = *rx_rate = 4;
            break;
        case 0x60:
            *mask = MOD_BELL;
            *tx_rate = *rx_rate = 2;
            break;
        default:
            *mask = MOD_UNKNOWN;
            *tx_rate = *rx_rate = 0xff;
            break;
        }
    }
}

/* set rate_sequence for the connection's modulation. trained is 1 after a
 * handshake or a retrain, and 0 after a rate renegotiation */
static void read_rate_sequence(char trained)
{
    /* @hack spelling: valid set from ok, a local that holds only 1, where
     * valid = 1 would do */
    char ok = 1;
    char valid;
    Uint8 low;
    Uint8 high;

    if ((rcdpi->conf.byte & CONF_CLASS) == CONF_V32) {
        valid = ok;
        if (trained == 1) {
            /* @hack spelling: originate compared with 1, where the bit alone
             * would do */
            if (rcdpi->r09.bit.originate == 1) {
                rate_seq_R2 = read_dp_ram(0x204, 2);
                rate_seq_R3 = read_dp_ram(0x208, 2);
                rate_sequence = rate_seq_R1;
            } else {
                rate_seq_R2 = read_dp_ram(0x208, 2);
                rate_seq_R3 = read_dp_ram(0x204, 2);
                rate_sequence = rate_seq_R2;
            }
        } else if (!rcflags.renegotiation_started) {
            rate_sequence = read_dp_ram(0x208, 2);
        } else {
            valid = 0;
        }
        if (valid) {
            rate_sequence &= 0xf66e;
            /* @hack spelling: each byte copied into low or high before the
             * table reads it, where the index alone would do */
            low = rate_sequence;
            low = invert_byte_table[low];
            high = rate_sequence >> 8;
            high = invert_byte_table[high];
            rate_sequence = (low << 8) | high;
        }
    } else if ((rcdpi->conf.byte & CONF_CLASS) == CONF_V34 ||
               (rcdpi->conf.byte & CONF_CLASS) == CONF_K56) {
        low = read_dp_ram(0x208, 1);
        high = read_dp_ram(0x209, 1);
        high &= 0x7f;
        rate_sequence = (high << 8) | low;
    } else if ((rcdpi->conf.byte & CONF_V90) == CONF_V90) {
        /* @hack redundant: a store of the byte pair that never runs */
        if (0) {
            rate_sequence = (high << 8) | low;
        }
        rate_sequence = V90GetRemoteCaps();
    }
}

Uint32 dp_get_rate_sequence(void)
{
    return rate_sequence;
}

/* drop rts, and wait for cts to drop, or report DP_ERR_IDLE. where rcflags'
 * tx_only is set, first turn tx_irq off and give tx_data time to empty */
void dp_set_idle(void)
{
    Uint32 timer;

    if (rcflags.tx_only) {
        rcdpi->irq.bit.tx_irq = 0;
        setup_timer(&timer, TICKS(100));
        while (!rcdpi->irq.bit.tx_empty) {
            if (timer_expired(timer)) {
                break;
            }
        }
    }
    rcdpi->r09.bit.unk_bit0 = 0;
    rcdpi->r08.bit.rts = 0;
    setup_timer(&timer, TICKS(500));
    while (rcdpi->status.bit.cts) {
        if (timer_expired(timer)) {
            data_pump_error(DP_ERR_IDLE);
            break;
        }
    }
}

static char is_fax_conf(void)
{
    Uint8 type;

    if (rcdpi->conf.byte == 0xa8) {
        return 1;
    }
    type = rcdpi->conf.byte & CONF_CLASS;
    if (type == 0xb0 || type == 0x10 || type == 0) {
        return 1;
    }
    return 0;
}

Uint32 dp_get_v8_status(void)
{
    return v8_status;
}

static Uint32 read_v8_status(void)
{
    Uint8 first;
    Uint8 second;

    first = read_dp_ram(RAM_V8_STATUS, 1);
    second = read_dp_ram(RAM_V8_STATUS + 1, 1);
    if ((first & 0x10) && (second >> 5) == 1) {
        return 1;
    }
    return 0;
}

void dp_set_v8_ci(char ci)
{
}

void dp_set_v8_parameters(Uint8 parameters)
{
    Uint8 options;
    Uint8 modes;

    options = read_dp_ram(0x305, 1);
    modes = read_dp_ram(0x304, 1);
    write_dp_ram(0x306, 0x40, 1);
    write_dp_ram(0x307, 3, 1);
    write_dp_ram(0x308, 0x84, 1);
    /* @hack spelling: the bit stored through ?:, where
     * parameters & 1 would do */
    rcflags.unk_0_7 = (parameters & 1) ? 1 : 0;
    if (parameters & 2) {
        options |= 1;
        options &= 0x1f;
        if (parameters & 8) {
            options |= 0x20;
        }
    } else {
        options &= 0xfe;
    }
    if (parameters & 4) {
        modes |= 0x10;
    } else {
        modes &= 0xef;
    }
    if (parameters & 0x10) {
        options |= 4;
    } else {
        options &= 0xfb;
    }
    write_dp_ram(0x304, modes, 1);
    write_dp_ram(0x305, options, 1);
}

void dp_set_v34_parameters(Uint32 parameters)
{
    Uint8 value;
    Uint8 control;

    v34options = parameters;
    value = read_dp_ram(0x13f, 1);
    if (parameters & 1) {
        value |= 0x40;
    } else {
        value &= 0xbf;
    }
    write_dp_ram(0x13f, value, 1);
    value = read_dp_ram(0x100, 1);
    if (parameters & 2) {
        value &= 0xf7;
    } else {
        value |= 8;
    }
    if (parameters & 8) {
        value &= 0xfd;
    } else {
        value |= 2;
    }
    write_dp_ram(0x100, value, 1);
    if (!(parameters & 0x20)) {
        value = read_dp_ram(0x10d, 1);
        value &= 0x7f;
        write_dp_ram(0x10d, value, 1);
    }
    AgressivityLevel = (parameters & 0xc0) >> 6;
    value = read_dp_ram(0x105, 1);
    control = read_dp_ram(0x101, 1);
    if (parameters & 0x4000) {
        value |= 0x20;
    } else {
        value &= 0xdf;
    }
    if (parameters & 0x8000) {
        value |= 0x10;
    } else {
        value &= 0xef;
    }
    if (!(value & 0x30)) {
        control &= 0xf7;
    } else {
        control |= 8;
    }
    if (parameters & 0x10000) {
        value |= 0x80;
    } else {
        value &= 0x7f;
    }
    if (parameters & 0x20000) {
        value |= 0x40;
    } else {
        value &= 0xbf;
    }
    if (!(value & 0xc0)) {
        control &= 0xef;
    } else {
        control |= 0x10;
    }
    if (parameters & 0x40000) {
        control |= 0x20;
    } else {
        control &= 0xdf;
    }
    write_dp_ram(0x105, value, 1);
    write_dp_ram(0x101, control, 1);
}

/* keep options in K56options, and a copy of info in K56flexRemoteInfo. bit 7
 * of RAM 0x39b takes bit 0 of options. where info's unk_0 is 0x9400 or
 * 0x4a00, its unk_2 and unk_3 set RAM 0x67d */
void dp_set_K56_parameters(Uint8 options, struct k56flexinfotag *info)
{
    Uint16 value;
    struct k56flexinfotag *saved;

    K56options = options;
    /* @hack spelling: the copy made through saved, where K56flexRemoteInfo's
     * fields would do */
    saved = &K56flexRemoteInfo;
    saved->unk_0 = info->unk_0;
    saved->unk_2 = info->unk_2;
    saved->unk_3 = info->unk_3;
    saved->unk_4 = info->unk_4;
    /* @hack redundant: a goto to the next statement */
    goto read;
read:
    value = read_dp_ram(0x39b, 1);
    if (options & 1) {
        write_dp_ram(0x39b, value | 0x80, 1);
    } else {
        write_dp_ram(0x39b, value & 0xff7f, 1);
    }
    if (K56flexRemoteInfo.unk_0 == 0x9400 ||
        K56flexRemoteInfo.unk_0 == 0x4a00) {
        if ((K56flexRemoteInfo.unk_3 & 2) ||
            ((K56flexRemoteInfo.unk_3 & 1) && K56flexRemoteInfo.unk_2)) {
            write_dp_ram(0x67d, 1, 1);
        } else {
            write_dp_ram(0x67d, 0, 1);
        }
    }
}

Uint16 dp_read_rec_level(void)
{
    Uint16 power;
    Uint16 level;
    Uint8 shift;
    Uint8 offset;

    /* @hack spelling: power, a local for the reading, where dividing the
     * reading would do */
    power = read_dp_ram(RAM_RX_POWER, 2);
    level = power / 683;
    if ((rcdpi->conf.byte & CONF_V90) == CONF_V90 ||
        (rcdpi->conf.byte & CONF_CLASS) == CONF_K56 ||
        (rcdpi->conf.byte & CONF_CLASS) == CONF_V34 ||
        (rcdpi->conf.byte & CONF_CLASS) == 0x40 ||
        (rcdpi->conf.byte & CONF_CLASS) == 0x30 ||
        (rcdpi->conf.byte & CONF_CLASS) == 0xb0 ||
        (rcdpi->conf.byte & CONF_CLASS) == 0x10 || rcdpi->conf.byte == 0x02 ||
        rcdpi->conf.byte == 0x01) {
        level = 52 - level;
        if ((rcdpi->conf.byte & CONF_V90) == CONF_V90 ||
            (rcdpi->conf.byte & CONF_CLASS) == CONF_K56) {
            /* @hack type: the reading cast to Uint8 before its shift, where the
             * shift alone would do */
            shift = (Uint8)read_dp_ram(0x3cb, 2) >> 1 & 3;
            switch (shift) {
            case 1:
                offset = 10;
                break;
            case 2:
                offset = 15;
                break;
            case 3:
                offset = 20;
                break;
            case 0:
                offset = 0;
                break;
                /* @hack redundant: an arm for a shift past 3, which
                 * never runs */
            default:
                offset = 0;
                break;
            }
            level += offset;
            read_dp_ram(0x3cd, 2);
            if (shift && level > 3) {
                level -= 4;
            }
        }
    } else if ((rcdpi->conf.byte & CONF_CLASS) == CONF_V32) {
        level = 53 - level;
    } else if (rcdpi->conf.byte == 0x84 || rcdpi->conf.byte == 0x82 ||
               rcdpi->conf.byte == 0x52 || rcdpi->conf.byte == 0x51 ||
               rcdpi->conf.byte == 0x62 || rcdpi->conf.byte == 0xa4) {
        level = 48 - level;
    } else if (rcdpi->conf.byte == 0xa0 || rcdpi->conf.byte == 0xa8 ||
               rcdpi->conf.byte == 0x86 || rcdpi->conf.byte == 0xa1) {
        level = 54 - level;
    } else if (rcdpi->conf.byte == 0x60) {
        level = 51 - level;
    }
    return level;
}

/* where rcflags' interrupt_on_carrier_loss and carrier_seen are both set, run
 * dp_interrupt once with rx_irq off, and clear carrier_seen. dp_carrier_detect
 * sets carrier_seen as it reports a carrier under interrupt_on_carrier_loss,
 * and calls this where it reports none */
static void run_deferred_interrupt(void)
{
    Uint8 enabled;

    if (rcflags.interrupt_on_carrier_loss && rcflags.carrier_seen) {
        enabled = rcdpi->irq.bit.rx_irq;
        rcdpi->irq.bit.rx_irq = 0;
        dp_interrupt();
        rcdpi->irq.bit.rx_irq = enabled;
        rcflags.carrier_seen = 0;
    }
}

char dp_carrier_detect(void)
{
    if (rcdpi->r07.bit.digital_loop) {
        if (rcdpi->status.bit.unk_bit6) {
            return 1;
        }
        return 0;
    }
    if (rcdpi->status.bit.dcd) {
        if (rcdpi->conf.byte == CONF_V32) {
            return 0;
        }
        /* @hack spelling: the test of 0xaf with its return in an else, where a
         * test of its own would do */
        if (rcdpi->conf.byte != 0xaf) {
            if (rcdpi->conf.byte == CONF_V34) {
                return 0;
            }
        } else {
            return 0;
        }
        if (rcdpi->conf.byte == 0x40) {
            return 0;
        }
        if (rcdpi->status.bit.unk_bit6) {
            if (rcflags.interrupt_on_carrier_loss) {
                rcflags.carrier_seen = 1;
            }
            return 1;
        }
    }
    run_deferred_interrupt();
    return 0;
}

Uint8 dp_energy_on_line(Uint8 level)
{
    /* @hack redundant: the else after the arm that returns */
    if (rcdpi->status.bit.dcd) {
        if (dp_read_rec_level() < level) {
            return 1;
        }
        return 0;
    } else {
        return dp_filter_test();
    }
}

Uint16 dp_read_eqm(void)
{
    return read_dp_ram(RAM_EQM, 2);
}

Uint8 dp_snr_ratio(void)
{
    Uint16 value;
    Uint8 reading;
    const struct word_grades *grades;
    const struct word_grades *row;

    if (rcdpi->tones.bit.unk_bit1 && !rcdpi->r07.bit.analog_loop &&
        (rcdpi->conf.byte < 0xc1 || rcdpi->conf.byte > 0xce)) {
        rcdpi->tones.bit.unk_bit1 = 0;
        return 3;
    }
    switch (rcdpi->conf.byte) {
    case 0x60:
    case CONF_V21:
    case 0xa1:
    case 0xa4:
        return 0;
    }
    /* @hack redundant: the else after the arm that returns */
    if (((rcdpi->conf.byte & CONF_CLASS) == CONF_K56 && !k56flags.k56_as_v34) ||
        ((rcdpi->conf.byte & CONF_V90) == CONF_V90 && !k56flags.v90_as_v34)) {
        if (K56options & 1) {
            grades = snr_word_grades_1;
        } else {
            grades = snr_word_grades_0;
        }
        value = read_dp_ram(RAM_K56_EQM, 2);
        /* @hack spelling: the row's assignment joined to the first test by a
         * comma, where a statement of its own would do */
        if (row = &grades[AgressivityLevel], value < row->grade_1) {
            return 0;
        }
        if (value < row->grade_2) {
            return 1;
        }
        if (value < row->grade_3) {
            return 2;
        }
        return 3;
    } else {
        reading = read_dp_ram(RAM_EQM10, 1);
        if (reading < snr_byte_grades[AgressivityLevel].grade_1) {
            return 0;
        }
        if (reading < snr_byte_grades[AgressivityLevel].grade_2) {
            return 1;
        }
        if (reading < snr_byte_grades[AgressivityLevel].grade_3) {
            return 2;
        }
        return 3;
    }
}

Uint16 dp_read_round_trip_delay(void)
{
    Uint32 delay;
    Uint32 rate;
    Uint32 scaled;
    Sint32 result;

    delay = read_dp_ram(RAM_ROUND_TRIP, 2);
    rate = symbol_rates[read_dp_ram(RAM_BAUD, 1)];
    scaled = delay * 1000;
    if (rcdpi->conf.byte >= 0xc1 && rcdpi->conf.byte <= 0xce) {
        result = (scaled / rate) - 10;
    } else if (rcdpi->conf.byte >= 0x46 && rcdpi->conf.byte <= 0x4c) {
        result = ((delay * 10000 / rate) - 35) / 10;
    } else if ((rcdpi->conf.byte & CONF_CLASS) == CONF_V32) {
        result = (scaled / V32_BAUD) - 29;
    } else {
        return 0;
    }
    if (result < 0) {
        result = 0;
    }
    return result;
}

Uint16 dp_read_near_echo_level(void)
{
    return 0;
}

Uint16 dp_read_far_echo_level(void)
{
    Uint16 value;

    value = read_dp_ram(RAM_FAR_ECHO, 2);
    if (dat_0cb703b6 == 0x80) {
        if (value >= 0x1100) {
            return 8;
        }
        if (value >= 0xf00) {
            return 9;
        }
        if (value >= 0xd00) {
            return 10;
        }
        if (value >= 0xc00) {
            return 11;
        }
        if (value >= 0xa00) {
            return 12;
        }
        if (value >= 0x900) {
            return 13;
        }
        if (value >= 0x700) {
            return 15;
        }
        if (value >= 0x400) {
            return 20;
        }
        if (value >= 0x250) {
            return 25;
        }
        if (value >= 0x150) {
            return 30;
        }
        if (value >= 0xc0) {
            return 35;
        }
        if (value >= 0x70) {
            return 40;
        }
        if (value >= 0x45) {
            return 45;
        }
    }
    if (dat_0cb703b6 == 0x40) {
        if (value >= 0x1300) {
            return 8;
        }
        if (value >= 0x1100) {
            return 9;
        }
        if (value >= 0xf00) {
            return 10;
        }
        if (value >= 0xd00) {
            return 11;
        }
        if (value >= 0xc00) {
            return 12;
        }
        if (value >= 0xa00) {
            return 13;
        }
        if (value >= 0x800) {
            return 15;
        }
        if (value >= 0x500) {
            return 20;
        }
        if (value >= 0x300) {
            return 25;
        }
        if (value >= 0x180) {
            return 30;
        }
        if (value >= 0xd0) {
            return 35;
        }
        if (value >= 0x80) {
            return 40;
        }
        if (value >= 0x40) {
            return 45;
        }
    }
    return 50;
}

void dp_set_ring_periods(Uint8 on, Uint8 off)
{
    ring_on = on;
    ring_off = off;
    write_dp_ram(0x10f, 0, 1);
    write_dp_ram(0x21e, 1, 1);
    write_dp_ram(0x21f, 0xd2, 1);
}

char dp_test_ring(void)
{
    if (rcdpi->r09.bit.unk_bit2) {
        rcdpi->r09.bit.unk_bit2 = 0;
        setup_timer(&hook_timer, TICKS(5000));
    }
    if (rcdpi->status.bit.ring) {
        setup_timer(&hook_timer, TICKS(5000));
        return 1;
    }
    return 0;
}

void dp_off_hook(void)
{
    rcdpi->r07.bit.off_hook = 1;
}

void dp_on_hook(void)
{
    rcdpi->r07.bit.off_hook = 0;
    setup_timer(&hook_timer, TICKS(5000));
}

void dp_set_sync_mode(Uint8 mode)
{
}

void dp_set_async_mode(void)
{
    Uint8 enabled;

    rcdpi->format.bit.unk_bit6 = 0;
    rcdpi->format.bit.unk_bit3 = 0;
    rcdpi->format.bit.unk_bit2 = 0;
    setup_timer(&dp_timer, DP_TIMEOUT);
    while (rcdpi->format.bit.unk_low != 3) {
        rcdpi->format.bit.unk_low = 3;
        if (timer_expired(dp_timer)) {
            data_pump_error(DP_ERR_ASYNC);
            return;
        }
    }
    enabled = rcdpi->r08.bit.rts;
    rcdpi->r08.bit.rts = 0;
    rcdpi->r08.bit.async = 1;
    rcdpi->r08.bit.rts = enabled;
}

void dp_set_rts_cts_delay(Uint8 delay)
{
}

void dp_set_rts_off(void)
{
    rcdpi->r08.bit.rts = 0;
}

void dp_set_rts_on(void)
{
    rcdpi->r08.bit.rts = 1;
}

char dp_get_cts(void)
{
    return rcdpi->status.bit.cts;
}

char dp_get_dsr(void)
{
    return rcdpi->status.bit.dsr;
}

char dp_get_dcd(void)
{
    return rcdpi->status.bit.dcd;
}

void dp_enter_local_analog_loop(void)
{
    Uint32 timer;

    if (rcdpi->conf.byte == CONF_AUTO) {
        rcdpi->conf.byte = read_dp_ram(RAM_V34_CONF, 1);
        if (rcdpi->conf.byte == 0xc1) {
            write_dp_ram(RAM_BAUD, 0, 1);
        }
        if (rcdpi->conf.byte == 0xce) {
            write_dp_ram(RAM_BAUD, 5, 1);
        }
    }
    setup_timer(&timer, TICKS(2000));
    do {
        rcdpi->r05.bit.ceq = 0;
        rcdpi->r03.bit.unk_bit3 = 0;
        rcdpi->r09.bit.unk_bit0 = 0;
        rcdpi->r09.bit.unk_bit2 = 0;
        rcdpi->r15.bit.unk_bit1 = 0;
        rcdpi->r07.bit.analog_loop = 1;
        if ((rcdpi->conf.byte & CONF_CLASS) == CONF_V34) {
            rcdpi->control.bit.newc = 1;
            setup_timer(&dp_timer, TICKS(100));
            while (rcdpi->abcode.byte != V34_MODULE) {
                if (timer_expired(dp_timer)) {
                    data_pump_error(DP_ERR_ABCODE);
                    return;
                }
            }
            if (!RamDownload(RAM_BANK_CONTROLLER, V34_MODULE, 1, 0, 0)) {
                data_pump_error(DP_ERR_DOWNLOAD);
                return;
            }
            setup_timer(&dp_timer, TICKS(100));
            while (rcdpi->control.bit.newc) {
                if (timer_expired(dp_timer)) {
                    data_pump_error(DP_ERR_TEST_NEWC);
                    return;
                }
            }
        } else {
            newc();
        }
        rcdpi->r09.bit.unk_bit2 = 1;
        rcdpi->r08.bit.rts = 1;
        rcdpi->r09.bit.unk_bit0 = 1;
        setup_timer(&dp_timer, TICKS(60));
        while (!timer_expired(dp_timer)) {
        }
        if (timer_expired(timer)) {
            data_pump_error(DP_ERR_ANALOG_LOOP);
            return;
        }
    } while (!rcdpi->status.bit.dsr);
}

char dp_monitor_local_analog_loop(Uint16 *errors)
{
    *errors = read_dp_ram(RAM_ERRORS, 2);
    if (rcdpi->status.bit.cts && dp_carrier_detect()) {
        return 1;
    }
    return 0;
}

void dp_stop_local_analog_loop(void)
{
    rcdpi->r07.bit.analog_loop = 0;
    newc();
}

void dp_enable_rdl(void)
{
    rcdpi->r07.bit.rdl_enable = 1;
    rcdpi->r02.bit.v54pe = 1;
    rcflags.rdl_enabled = 1;
}

void dp_disable_rdl(void)
{
    rcdpi->r07.bit.rdl_enable = 0;
    rcdpi->r02.bit.v54pe = 0;
    rcflags.rdl_enabled = 0;
}

void dp_enter_remote_digital_loop(void)
{
    tst_initial_V54PE = rcdpi->r02.bit.v54pe;
    rcdpi->r02.bit.v54pe = 0;
    if (rcdpi->conf.byte == 0x84 || rcdpi->conf.byte == 0x82 ||
        rcdpi->conf.byte == 0x52) {
        rcdpi->r07.bit.rdl = 1;
    } else if ((rcdpi->conf.byte & CONF_CLASS) != CONF_V34 ||
               dp_get_tx_bit_rate() == dp_get_rx_bit_rate()) {
        write_dp_ram(0xa0f, 16, 2);
        write_dp_ram(0xb0f, 0x100, 2);
        dp_rdl_phase = RDL_ASKED;
        rcdpi->r08.bit.rdl_request = 1;
        rcdpi->r02.bit.rdl_asked = 1;
    }
}

Uint8 dp_monitor_rdl_response(Uint16 *errors)
{
    *errors = 0;
    if (dp_rdl_phase == RDL_NONE) {
        if (rcdpi->status.bit.looping && rcdpi->status.bit.cts) {
            return LOOP_RUNNING;
        }
    } else {
        switch (dp_rdl_phase) {
        case RDL_ASKED:
            if (rcdpi->r08.bit.rdl_request || !rcdpi->status.bit.rdl_signal) {
                break;
            }
            dp_rdl_phase = RDL_LOOPED;
            rcdpi->r02.bit.rdl_asked = 0;
            rcdpi->status.bit.rdl_signal = 0;
            break;
        case RDL_LOOPED:
            if (rcdpi->r02.bit.self_test) {
                *errors = read_dp_ram(RAM_ERRORS, 2);
            }
            return LOOP_RUNNING;
        case RDL_ENDING:
            if (!rcdpi->r08.bit.rdl_release) {
                dp_rdl_phase = RDL_NONE;
                break;
            }
            return LOOP_CHANGING;
        }
    }
    return 0;
}

void dp_stop_remote_digital_loop(void)
{
    rcdpi->r02.bit.self_test = 0;
    rcdpi->r02.bit.v54pe = tst_initial_V54PE;
    write_dp_ram(0xa0f, 2, 2);
    write_dp_ram(0xb0f, 16, 2);
    write_dp_ram(RAM_SELF_TEST, 0, 1);
    if (dp_rdl_phase) {
        dp_rdl_phase = RDL_ENDING;
        rcdpi->r08.bit.rdl_release = 1;
    } else {
        rcdpi->r07.bit.rdl = 0;
    }
}

void dp_enter_self_test(void)
{
    rcdpi->r02.bit.self_test = 1;
    setup_timer(&dp_timer, TICKS(100));
    while (!timer_expired(dp_timer)) {
    }
    write_dp_ram(RAM_SELF_TEST, 0xff, 1);
}

void dp_enter_local_digital_loop(void)
{
    Uint8 enabled;

    tst_initial_TXCLK = rcdpi->tx_level.bit.clock;
    enabled = rcdpi->r08.bit.rts;
    rcdpi->r08.bit.rts = 0;
    rcdpi->tx_level.bit.clock = TX_CLOCK_LOOP;
    rcdpi->r08.bit.rts = enabled;
    rcdpi->r07.bit.digital_loop = 1;
}

void dp_stop_local_digital_loop(void)
{
    Uint8 enabled;

    rcdpi->r07.bit.digital_loop = 0;
    setup_timer(&dp_timer, DP_TIMEOUT);
    while (rcdpi->status.bit.looping) {
        if (timer_expired(dp_timer)) {
            data_pump_error(DP_ERR_DIGITAL_LOOP);
            return;
        }
    }
    enabled = rcdpi->r08.bit.rts;
    rcdpi->r08.bit.rts = 0;
    rcdpi->tx_level.bit.clock = tst_initial_TXCLK;
    rcdpi->r08.bit.rts = enabled;
}

Uint8 dp_monitor_remote_rdl(void)
{
    if (!rcflags.rdl_enabled) {
        return 0;
    }
    switch (dp_rdl_phase) {
    case RDL_NONE:
        if (rcdpi->status.bit.rdl_signal) {
            tst_initial_V54PE = rcdpi->r02.bit.v54pe;
            rcdpi->r02.bit.v54pe = 0;
            rcdpi->r08.bit.rdl_grant = 1;
            dp_rdl_phase = RDL_FAR_ASKED;
            return LOOP_CHANGING;
        }
        if (rcdpi->status.bit.looping) {
            dp_rdl_phase = RDL_PUMP_LOOPED;
            return LOOP_RUNNING;
        }
        break;
    case RDL_FAR_ASKED:
        if (!rcdpi->r08.bit.rdl_grant) {
            rcdpi->status.bit.rdl_signal = 0;
            rcdpi->r02.bit.rdl_far_looped = 1;
            dp_rdl_phase = RDL_FAR_LOOPED;
            return LOOP_RUNNING;
        }
        return LOOP_CHANGING;
    case RDL_FAR_LOOPED:
        if (rcdpi->status.bit.rdl_signal) {
            rcdpi->r02.bit.rdl_far_looped = 0;
            rcdpi->status.bit.rdl_signal = 0;
            dp_rdl_phase = RDL_NONE;
            rcdpi->r02.bit.v54pe = tst_initial_V54PE;
            break;
        }
        return LOOP_RUNNING;
    case RDL_PUMP_LOOPED:
        /* @hack redundant: the else after the arm that returns */
        /* @hack spelling: looping compared with 1, where the bit alone would
         * do */
        if (rcdpi->status.bit.looping == 1) {
            return LOOP_RUNNING;
        } else {
            dp_rdl_phase = RDL_NONE;
        }
        break;
    }
    return 0;
}

void dp_init_V90_fast_retrain(void)
{
    Uint8 value;

    dp_disable_data_mode();
    rcdpi->r15.bit.rate_range = 0;
    value = read_dp_ram(0x680, 1);
    write_dp_ram(0x680, value | 0x20, 1);
    rcdpi->r08.bit.retrain = 1;
    rcflags.retraining = 0;
    rcflags.renegotiating = 1;
}

void dp_init_retrain(Uint8 min_rate, Uint8 max_rate, Uint8 min_rx_rate,
                     Uint8 max_rx_rate)
{
    Uint8 conf;
    Uint32 mask;
    Uint8 rate;
    Uint8 value;

    value = read_dp_ram(0x680, 1);
    write_dp_ram(0x680, value & 0xffdf, 1);
    dp_disable_data_mode();
    dp_get_modulation_type(&mask, &rate, &rate);
    /* @bug uninitialized: the store below takes conf unset where select_conf
     * sets none, as for a K56flex connection's MOD_K56 while rate_range is
     * clear, at 0x0cae7636 */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: conf starts at the pump's
     * configuration, which the store below keeps where select_conf sets
     * none */
    conf = rcdpi->conf.byte;
#endif
    select_conf(mask, min_rate, max_rate, min_rx_rate, max_rx_rate, &conf);
    handshake_status = 0;
    retrains_started++;
    if (!rcdpi->r15.bit.rate_range) {
        rcdpi->conf.byte = conf;
    }
    rcdpi->r08.bit.retrain = 1;
    rcflags.retraining = 1;
    rcflags.renegotiating = 0;
    rcflags.retrain_started = 1;
    rcdpi->r05.bit.unk_bit7 = 0;
    InitWaVariables();
    if ((rcdpi->conf.byte & CONF_V90) == CONF_V90) {
        k56flags.unk_1_7 = 1;
    } else if ((rcdpi->conf.byte & CONF_CLASS) == CONF_K56) {
        k56flags.unk_0_0 = 1;
    }
}

void dp_init_rate_renegotiation(Uint8 min_rate, Uint8 max_rate)
{
    Uint32 mask;
    Uint8 conf;
    Uint8 rate;
    Uint8 value;

    value = read_dp_ram(0x680, 1);
    write_dp_ram(0x680, value & 0xffdf, 1);
    dp_disable_data_mode();
    dp_get_modulation_type(&mask, &rate, &rate);
    /* @bug uninitialized: the store below takes conf unset where select_conf
     * sets none, as for every K56flex connection's MOD_K56, at 0x0cae7790 */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: conf starts at the pump's
     * configuration, which the store below keeps where select_conf sets
     * none */
    conf = rcdpi->conf.byte;
#endif
    if (mask == MOD_V32BIS) {
        select_conf(mask, min_rate, max_rate, min_rate, max_rate, &conf);
    } else {
        select_conf(mask, 0, RATE_V34_MAX, min_rate, max_rate, &conf);
    }
    handshake_status = 0;
    renegotiations_started++;
    if (mask == MOD_K56 || mask == MOD_V90 || mask == MOD_V34 ||
        mask == MOD_V32BIS || k56flags.unk_0_0 || k56flags.unk_1_7) {
        rcdpi->r15.bit.rate_range = 0;
    }
    if (!rcdpi->r15.bit.rate_range) {
        rcdpi->conf.byte = conf;
    }
    rcdpi->r15.bit.renegotiate = 1;
    rcflags.retraining = 0;
    rcflags.renegotiating = 1;
    rcflags.renegotiation_retrained = 0;
    rcflags.renegotiation_started = 1;
    k56flags.b11_timing = 0;
    k56flags.renegotiation_timer_set = 0;
}

Uint8 dp_detect_retrain(void)
{
    Uint8 result;

    if (rcdpi->speed.bit.retrain) {
        retrains_seen++;
        rcflags.retraining = 1;
        rcflags.renegotiating = 0;
        rcflags.retrain_started = 0;
        rcdpi->r05.bit.unk_bit7 = 0;
        InitWaVariables();
        if ((rcdpi->conf.byte & CONF_V90) == CONF_V90) {
            k56flags.unk_1_7 = 1;
        } else if ((rcdpi->conf.byte & CONF_CLASS) == CONF_K56) {
            k56flags.unk_0_0 = 1;
        }
        result = DETECT_RETRAIN;
    } else if (rcdpi->speed.bit.renegotiation) {
        result = DETECT_RENEGOTIATION;
        renegotiations_seen++;
        rcflags.retraining = 0;
        rcflags.renegotiating = 1;
        rcflags.renegotiation_retrained = 0;
        rcflags.renegotiation_started = 0;
        k56flags.b11_timing = 0;
        k56flags.renegotiation_timer_set = 0;
    } else {
        return 0;
    }
    dp_disable_data_mode();
    handshake_status = 0;
    return result;
}

Uint8 dp_monitor_retrain(void)
{
    Uint8 value;

    MonitorABcode();
    if (rcdpi->conf.byte == CONF_V90 || rcdpi->conf.byte == CONF_K56 ||
        rcdpi->conf.byte == CONF_V32 || rcdpi->conf.byte == 0xaf ||
        rcdpi->conf.byte == CONF_V34 || rcdpi->conf.byte == 0x40) {
        return TRAIN_FAILED;
    }
    if (rcflags.retraining) {
        if ((rcdpi->conf.byte & CONF_V90) == CONF_V90 && !k56flags.v90_as_v34) {
            MonitorV90Train();
        } else if ((rcdpi->conf.byte & CONF_CLASS) == CONF_K56 &&
                   !k56flags.k56_as_v34) {
            MonitorK56Train();
        } else if ((rcdpi->conf.byte & CONF_CLASS) == CONF_V34 ||
                   k56flags.k56_as_v34 || k56flags.v90_as_v34) {
            MonitorV34Train();
        } else if ((rcdpi->conf.byte & CONF_CLASS) == CONF_V32) {
            MonitorV32Train();
            if (handshake_status == HANDSHAKE_FAILED) {
                return TRAIN_FAILED;
            }
        }
        /* @hack redundant: the else after the arm that returns */
        if (dp_carrier_detect() && rcdpi->status.bit.cts &&
            !rcdpi->speed.bit.retrain) {
            rcdpi->r15.bit.rate_range = rcflags.saved_rate_range;
            read_rate_sequence(1);
            write_connect_ram();
            WaV34Disc();
            dp_enable_data_mode(data_mode, data_option, data_speed);
            if ((rcdpi->conf.byte & CONF_V90) == CONF_V90 ||
                (rcdpi->conf.byte & CONF_CLASS) == CONF_K56 ||
                (rcdpi->conf.byte & CONF_CLASS) == CONF_V34) {
                value = read_dp_ram(0x600, 1);
                write_dp_ram(0x459, value, 1);
                value = read_dp_ram(0x601, 1);
                write_dp_ram(0x45b, value, 1);
            }
            if ((rcdpi->conf.byte & CONF_V90) == CONF_V90 &&
                !k56flags.v90_as_v34) {
                setup_timer(&b11_timer, TICKS(3000));
                k56flags.b11_timing = 1;
            }
            return TRAIN_CONNECTED;
        } else if (rcflags.retrain_started &&
                   (rcdpi->r08.bit.retrain || !rcflags.training_progressed)) {
            return TRAIN_PENDING;
        }
        return TRAIN_RUNNING;
    }
    if (!dp_carrier_detect() || !rcdpi->status.bit.cts) {
        if ((rcdpi->conf.byte & CONF_CLASS) == CONF_V34 ||
            k56flags.k56_as_v34 || k56flags.v90_as_v34) {
            MonitorV34RR();
        } else if ((rcdpi->conf.byte & CONF_V90) == CONF_V90 &&
                   !k56flags.v90_as_v34) {
            MonitorV90RR();
        }
        setup_timer(&retrain_timer, TICKS(20));
        if ((rcflags.renegotiation_started && rcdpi->r15.bit.renegotiate) ||
            rcflags.renegotiation_retrained) {
            return TRAIN_PENDING;
        }
        return TRAIN_RUNNING;
    }
    if (!timer_expired(retrain_timer)) {
        return TRAIN_RUNNING;
    }
    rcdpi->r15.bit.rate_range = rcflags.saved_rate_range;
    read_rate_sequence(0);
    rcdpi->speed.bit.renegotiation = 0;
    if (!rate_sequence) {
        return TRAIN_FAILED;
    }
    write_connect_ram();
    WaV34Disc();
    dp_enable_data_mode(data_mode, data_option, data_speed);
    if ((rcdpi->conf.byte & CONF_V90) == CONF_V90 ||
        (rcdpi->conf.byte & CONF_CLASS) == CONF_K56 ||
        (rcdpi->conf.byte & CONF_CLASS) == CONF_V34) {
        value = read_dp_ram(0x600, 1);
        write_dp_ram(0x459, value, 1);
        value = read_dp_ram(0x601, 1);
        write_dp_ram(0x45b, value, 1);
    }
    if ((rcdpi->conf.byte & CONF_V90) == CONF_V90 && !k56flags.v90_as_v34) {
        setup_timer(&b11_timer, TICKS(3000));
        k56flags.b11_timing = 1;
    }
    return TRAIN_RENEGOTIATED;
}

void dp_abort_retrain(void)
{
}

Uint16 read_dp_ram(Uint16 address, Uint8 width)
{
    if (target_HwFault()) {
        return 0;
    }
    if ((address >> 8) & 0x40) {
        rcdpi->ram_control.bit.high = 1;
    } else {
        rcdpi->ram_control.bit.high = 0;
    }
    rcdpi->ram_control.bit.address = (address >> 8) & 0x0f;
    rcdpi->ram_addr.byte = address;
    rcdpi->ram_control.bit.busy = 1;
    setup_timer(&dp_timer, DP_TIMEOUT);
    while (rcdpi->ram_control.bit.busy) {
        if (timer_expired(dp_timer)) {
            data_pump_error(DP_ERR_RAM_READ);
            return 0;
        }
    }
    rcdpi->ram_control.bit.high = 0;
    if (width == 1) {
        return rcdpi->ram_data.byte;
    }
    return (rcdpi->ram_data_high.byte << 8) | rcdpi->ram_data.byte;
}

void write_dp_ram(Uint16 address, Uint16 value, Uint8 width)
{
    if (target_HwFault()) {
        return;
    }
    rcdpi->ram_control.bit.address = (address >> 8) & 0x0f;
    if ((address >> 8) & 0x40) {
        rcdpi->ram_control.bit.high = 1;
    } else {
        rcdpi->ram_control.bit.high = 0;
    }
    rcdpi->ram_control.bit.write = 1;
    rcdpi->ram_addr.byte = address;
    rcdpi->ram_data.byte = value;
    if (width == 2) {
        rcdpi->ram_data_high.byte = value >> 8;
    }
    rcdpi->ram_control.bit.busy = 1;
    setup_timer(&dp_timer, DP_TIMEOUT);
    while (rcdpi->ram_control.bit.busy) {
        if (timer_expired(dp_timer)) {
            data_pump_error(DP_ERR_RAM_WRITE);
            return;
        }
    }
    rcdpi->ram_control.bit.write = 0;
    rcdpi->ram_control.bit.high = 0;
}

void init_biquad_cc(Uint16 base, Uint16 b0, Uint16 b1, Uint16 b2, Uint16 a1,
                    Uint16 a2)
{
    write_biquad(base, b0, b1, b2, a1, a2);
}

void init_biquad_parameter_cc(Uint16 detector, Uint16 at_ba0, Uint16 at_aa0,
                              Uint16 at_ab8, Uint16 at_bb8)
{
    write_tone_detector(detector, at_ba0, at_aa0, at_ab8, at_bb8);
}

void init_tonec_cc(void)
{
    tonec_at_1650 = 0;
    tonec_at_2100 = 0;
}

/* write a biquad filter's five coefficient words at RAM base onward */
static void write_biquad(Uint16 base, Uint16 b0, Uint16 b1, Uint16 b2,
                         Uint16 a1, Uint16 a2)
{
    write_dp_ram(base++, b0, 2);
    write_dp_ram(base++, b1, 2);
    write_dp_ram(base++, b2, 2);
    write_dp_ram(base++, a1, 2);
    write_dp_ram(base, a2, 2);
}

/* write the four words of tone detector detector beside its filters. at_ba0
 * and at_aa0 go to RAM 0x0ba0 and 0x0aa0, plus 6 for each detector past A.
 * at_ab8 and at_bb8 go to RAM 0x0ab8 and 0x0bb8, plus 1 for each */
static void write_tone_detector(Uint16 detector, Uint16 at_ba0, Uint16 at_aa0,
                                Uint16 at_ab8, Uint16 at_bb8)
{
    Uint16 offset;
    Uint16 address;

    /* @hack spelling: offset assigned inside the first address, where a
     * statement of its own would do */
    address = (offset = detector * 6) + 0x0ba0;
    write_dp_ram(address, at_ba0, 2);
    address = offset + 0x0aa0;
    write_dp_ram(address, at_aa0, 2);
    address = detector + 0x0ab8;
    write_dp_ram(address, at_ab8, 2);
    address = detector + 0x0bb8;
    write_dp_ram(address, at_bb8, 2);
}

static void set_ram_702_bit0(void)
{
    Uint16 value;

    value = read_dp_ram(0x702, 1);
    value |= 1;
    write_dp_ram(0x702, value, 1);
}

static void clear_ram_702_bit0(void)
{
    Uint16 value;

    value = read_dp_ram(0x702, 1);
    value &= 0xfffe;
    write_dp_ram(0x702, value, 1);
}

static void set_ram_701_bit0(void)
{
    Uint16 value;

    value = read_dp_ram(0x701, 1);
    value |= 1;
    write_dp_ram(0x701, value, 1);
}

static void clear_ram_701_bit0(void)
{
    Uint16 value;

    value = read_dp_ram(0x701, 1);
    value &= 0xfffe;
    write_dp_ram(0x701, value, 1);
}

void newc(void)
{
    Uint32 timer;

    rcdpi->control.bit.newc = 1;
    setup_timer(&timer, TICKS(200));
    while (rcdpi->control.bit.newc) {
        if (timer_expired(timer)) {
            data_pump_error(DP_ERR_NEWC);
            break;
        }
    }
}

void dp_analyzer(void)
{
    if (!dat_0cb7039e) {
        /* @hack spelling: dtmf compared with 0, where its negation would do */
        if (rcdpi->conf.byte == CONF_DIAL && rcdpi->r09.bit.dtmf == 0) {
            if (rcdpi->irq.bit.tx_empty) {
                if (timer_expired(pulse_timer)) {
                    target_dpNeedsHookControl(0);
                }
            } else {
                setup_timer(&pulse_timer, TICKS(100));
            }
        } else {
            target_dpNeedsHookControl(0);
        }
    }
    if (rcflags.tx_irq_deferred && timer_expired(tx_irq_timer)) {
        rcdpi->r07.bit.unk_bit0 = 0;
        rcflags.tx_irq_deferred = 0;
        rcdpi->irq.bit.tx_irq = 1;
    }
    if (k56flags.b11_timing && timer_expired(b11_timer)) {
        write_dp_ram(0xb11, 0, 2);
        k56flags.b11_timing = 0;
    }
}

void dp_set_v21_detector_fax(void)
{
    set_tone_c_1650();
    setup_timer(&detect_v21_timer, TICKS(100));
}

static void set_tone_c_1650(void)
{
    rcdpi->r02.bit.tone_c = 1;
    write_biquad(TONE_C_PASS_FILTER, 0, 0, 0x7fff, 0, 0);
    write_biquad(TONE_C_PASS_FILTER + SECOND_FILTER, 0, 0, 0x7fff, 0, 0);
    write_biquad(TONE_C_FILTER, 0x306, 0xf9f9, 0x307, 0xc147, 0x10a6);
    write_biquad(TONE_C_FILTER + SECOND_FILTER, 0x306, 0x10d, 0x307, 0xc147,
                 0x106e);
    write_tone_detector(TONE_DETECTOR_C, 0x7e67, 0x2df, 0x2a00, 0x1c00);
    tonec_at_1650 = 1;
    tonec_at_2100 = 0;
}

/* return 1 once tone detector C has heard its tone for 100 ms, and 0
 * before */
char dp_detect_v21_fax(void)
{
    if (rcdpi->tones.bit.tone_c) {
        if (timer_expired(detect_v21_timer)) {
            return 1;
        }
    } else {
        setup_timer(&detect_v21_timer, TICKS(100));
    }
    return 0;
}

char dp_detect_v21_in_high_speed_fax(void)
{
    return rcdpi->r0a.bit.flag;
}

/* return 1 once a fax modulation carries data, and 0 before */
char dp_facsimile_data_mode(void)
{
    /* @hack spelling: originate compared with 1, where the bit alone would
     * do */
    if (rcdpi->r09.bit.originate == 1) {
        if (rcdpi->status.bit.dsr && rcdpi->status.bit.cts) {
            if (!dat_0cb703ac) {
                clear_ram_702_bit0();
            }
            return 1;
        }
    } else if (rcdpi->r0a.bit.unk_bit7 && rcdpi->status.bit.dcd) {
        if (rcdpi->conf.byte == 0x01) {
            write_dp_ram(0x4803, read_dp_ram(0x4803, 2) & 0xffef, 2);
        } else if ((rcdpi->conf.byte & CONF_CLASS) == 0xb0) {
            write_dp_ram(0x270, 20, 1);
        }
        return 1;
    }
    return 0;
}

void dp_enable_data_mode(Uint8 mode, Uint8 option, Uint8 speed)
{
    Uint8 value;
    Uint8 enabled;

    data_mode = mode;
    data_option = option;
    data_speed = speed;
    rcflags.unk_1_0 = 0;
    rcflags.tx_irq_deferred = 0;
    rcflags.unk_3_6 = 0;
    rcflags.rx_only = 0;
    rcflags.tx_only = 0;
    write_dp_ram(0x247, 0, 1);
    write_dp_ram(0x246, 0, 1);
    value = read_dp_ram(0x89, 1);
    write_dp_ram(0x89, value | 0x40, 1);
    rcdpi->r04.bit.fifo = 1;
    rcdpi->r08.bit.unk_bit6 = 1;
    dat_0cb703ac = 0;
    if (mode == 0) {
        value = read_dp_ram(0x32c, 1);
        value &= 0x3d;
        if (speed < 5) {
            clear_ram_702_bit0();
            clear_ram_701_bit0();
            value |= 0x80;
            if (speed == 1) {
                rcdpi->r04.bit.fifo = 0;
                if (!rcdpi->r08.bit.rts) {
                    rcflags.rx_only = 1;
                    value &= 0x3f;
                } else {
                    rcflags.tx_only = 1;
                }
            }
        } else {
            set_ram_701_bit0();
            clear_ram_702_bit0();
            if (ConvertTCISpeedToBps(dp_get_rx_bit_rate()) <=
                ConvertTCISpeedToBps(dp_get_tx_bit_rate()) >> 1) {
                value |= 0x40;
            } else {
                value |= 0x80;
            }
        }
        write_dp_ram(0x32c, value, 1);
        if (rcflags.tx_only) {
            rcdpi->irq.bit.tx_irq = 1;
        } else {
            rcdpi->irq.bit.rx_irq = 1;
        }
    } else if (mode == 1) {
        clear_ram_702_bit0();
        clear_ram_701_bit0();
        value = read_dp_ram(0x32c, 1);
        value &= 0x3f;
        value |= 0x82;
        write_dp_ram(0x32c, value, 1);
        (void)rcdpi->rx_data.byte;
        rcdpi->irq.bit.rx_irq = 1;
    } else if (mode == 2) {
        value = read_dp_ram(0x32c, 1);
        value &= 0x3f;
        if (speed < 5) {
            clear_ram_702_bit0();
            clear_ram_701_bit0();
            value |= 0x82;
            if (speed == 1) {
                rcdpi->r04.bit.fifo = 0;
            }
        } else {
            set_ram_702_bit0();
            set_ram_701_bit0();
            value |= 0xc2;
        }
        write_dp_ram(0x32c, value, 1);
        value = read_dp_ram(0xb3, 1);
        /* @hack spelling: bit 0 set or cleared through ?:, where an if and an
         * else would do */
        value = option == 1 ? value | 1 : value & ~1;
        write_dp_ram(0xb3, value, 1);
        /* @hack redundant: a test of mode, which the arm has already found 2 */
        if (mode == 2 && !is_fax_conf() && !k56flags.unk_3_7) {
            Uint32 count;

            enabled = rcdpi->r08.bit.rts;
            rcdpi->r08.bit.rts = 0;
            setup_timer(&dp_timer, DP_TIMEOUT);
            while (rcdpi->status.bit.cts) {
                if (timer_expired(dp_timer)) {
                    data_pump_error(DP_ERR_SYNC_CTS_OFF);
                    return;
                }
            }
            rcdpi->format.bit.unk_bit4 = 1;
            for (count = FORMAT_SPIN; count; count--) {
            }
            rcdpi->r08.bit.rts = enabled;
            if (enabled) {
                setup_timer(&dp_timer, DP_TIMEOUT);
                while (!rcdpi->status.bit.cts) {
                    if (timer_expired(dp_timer)) {
                        data_pump_error(DP_ERR_SYNC_CTS_ON);
                        return;
                    }
                }
            }
        } else {
            rcdpi->format.bit.unk_bit4 = 1;
        }
        setup_timer(&dp_timer, DP_TIMEOUT);
        while (rcdpi->r0c.bit.rx_waiting) {
            (void)rcdpi->rx_data.byte;
            if (timer_expired(dp_timer)) {
                data_pump_error(DP_ERR_SYNC_DRAIN);
                return;
            }
        }
        if (rcdpi->r04.bit.fifo) {
            write_dp_ram(0x247, 4, 1);
            rcflags.saved_fifo = 1;
        } else {
            rcflags.saved_fifo = 0;
            write_dp_ram(0x246, 4, 1);
        }
        value = read_dp_ram(RAM_RATE_RANGE_CONTROL, 1);
        write_dp_ram(RAM_RATE_RANGE_CONTROL, value | 0x40, 1);
        rcdpi->control.bit.new_status = 0;
        rcflags.unk_2_7 = rcdpi->r0a.bit.unk_bit5;
        rcflags.unk_2_6 = rcdpi->r0a.bit.unk_bit4;
        rcflags.unk_2_5 = rcdpi->r0a.bit.unk_bit3;
        rcflags.unk_2_4 = rcdpi->r0a.bit.unk_bit0;
        rcdpi->control.bit.status_irq = 1;
        rcdpi->irq.bit.rx_irq = 1;
    } else {
        write_dp_ram(0x32c, 0xc0, 1);
        set_ram_701_bit0();
        clear_ram_702_bit0();
        if (rcdpi->format.bit.unk_bit4) {
            if (!is_fax_conf()) {
                enabled = rcdpi->r08.bit.rts;
                rcdpi->r08.bit.rts = 0;
                setup_timer(&dp_timer, DP_TIMEOUT);
                while (rcdpi->status.bit.cts) {
                    if (timer_expired(dp_timer)) {
                        data_pump_error(DP_ERR_ASYNC_CTS_OFF);
                        return;
                    }
                }
                rcdpi->format.bit.unk_bit4 = 0;
                rcdpi->r08.bit.rts = enabled;
                if (enabled) {
                    setup_timer(&dp_timer, DP_TIMEOUT);
                    while (!rcdpi->status.bit.cts) {
                        if (timer_expired(dp_timer)) {
                            data_pump_error(DP_ERR_ASYNC_CTS_ON);
                            return;
                        }
                    }
                }
            } else {
                rcdpi->format.bit.unk_bit4 = 0;
            }
        }
        rcdpi->irq.bit.rx_irq = 1;
    }
    target_InstallDpIrq(dp_interrupt);
}

void dp_disable_data_mode(void)
{
    Uint8 enabled;
    Uint8 ready;

    target_InstallDpIrq(NULL);
    rcdpi->irq.bit.tx_irq = 0;
    rcdpi->irq.bit.rx_irq = 0;
    rcdpi->control.bit.status_irq = 0;
    if (rcdpi->format.bit.unk_bit4) {
        ready = rcdpi->status.bit.cts;
        enabled = rcdpi->r08.bit.rts;
        rcdpi->r08.bit.rts = 0;
        setup_timer(&dp_timer, DP_TIMEOUT);
        while (rcdpi->status.bit.cts) {
            if (timer_expired(dp_timer)) {
                data_pump_error(DP_ERR_DISABLE_CTS_OFF);
                return;
            }
        }
        rcdpi->format.bit.unk_bit4 = 0;
        rcdpi->r08.bit.rts = enabled;
        if (enabled && ready) {
            setup_timer(&dp_timer, DP_TIMEOUT);
            while (!rcdpi->status.bit.cts) {
                if (timer_expired(dp_timer)) {
                    data_pump_error(DP_ERR_DISABLE_CTS_ON);
                    return;
                }
            }
        }
    }
}

void dp_enable_tx_irqs(void)
{
    if (rcflags.in_interrupt) {
        rcflags.saved_tx_irq = 1;
    } else {
        rcdpi->irq.bit.tx_irq = 1;
    }
}

void dp_disable_tx_irqs(void)
{
    if (rcflags.in_interrupt) {
        rcflags.saved_tx_irq = 0;
    } else {
        rcdpi->irq.bit.tx_irq = 0;
    }
}

char dp_tx_irqs_enabled(void)
{
    char enabled;

    if (rcflags.unk_1_0 || rcflags.tx_irq_deferred || rcflags.unk_3_6) {
        return 1;
    }
    /* @hack spelling: enabled, a local for the result, where returning it would
     * do */
    enabled =
        rcflags.in_interrupt ? rcflags.saved_tx_irq : rcdpi->irq.bit.tx_irq;
    return enabled;
}

char dp_get_rx_data_byte(Uint8 *byte)
{
    if (rcdpi->r0c.bit.rx_waiting || rcdpi->irq.bit.rx_full) {
        *byte = rcdpi->rx_data.byte;
        return 1;
    }
    return 0;
}

char dp_ready_to_tx_data(void)
{
    /* @hack redundant: the else after the arm that returns */
    if (rcflags.rx_only) {
        return 0;
    } else {
        return rcdpi->r04.bit.fifo ? rcdpi->r0d.bit.tx_room
                                   : rcdpi->irq.bit.tx_empty;
    }
}

void dp_tx_data_byte(Uint8 byte)
{
    rcdpi->tx_data.byte = byte;
}

void dp_fill_tx_fifo(void)
{
}

Uint32 dp_get_tx_fifo_empty_time(void)
{
    Uint8 rate;

    rate = dp_get_tx_bit_rate();
    /* @hack redundant: the else after the arm that returns */
    if (dat_0cb703ac == 1) {
        switch (rate) {
        case 2:
            return 96;
        case 3:
            return 48;
        case 4:
            return 24;
        case 5:
            return 16;
        case 6:
            return 12;
        case 7:
            return 10;
        case 8:
            return 8;
        }
        return 1;
    } else {
        if (rate > 5) {
            return 2;
        }
        switch (rate) {
        case 0:
            return 171;
        case 1:
            /* @hack redundant: a store of data_mode that never runs */
            if (0) {
                data_mode = 0;
            }
            if (rcdpi->format.bit.unk_bit4) {
                return 13;
            }
            return 4;
        case 2:
            return 11;
        case 3:
            return 6;
        case 4:
            return 3;
        case 5:
            return 2;
        }
    }
    return 1;
}

char dp_test_irq_pending(void)
{
    if (!rcdpi->irq.bit.unk_bit6 && !rcdpi->irq.bit.unk_bit7 &&
        !rcdpi->control.bit.unk_bit7 && !rcdpi->control.bit.unk_bit6) {
        return 0;
    }
    return 1;
}

void dp_interrupt(void)
{
    Uint16 count = INTERRUPT_REPEATS;

    rcflags.in_interrupt = 1;
    rcflags.saved_tx_irq = rcdpi->irq.bit.tx_irq;
    rcflags.saved_status_irq = rcdpi->control.bit.status_irq;
    rcflags.saved_rx_irq = rcdpi->irq.bit.rx_irq;
    rcdpi->irq.bit.tx_irq = 0;
    rcdpi->control.bit.status_irq = 0;
    rcdpi->irq.bit.rx_irq = 0;
    do {
        rcdpi->control.bit.new_status = 0;
        dce_interrupt();
        rcdpi->control.bit.new_status = 0;
        if (count-- == 0) {
            data_pump_error(DP_ERR_INTERRUPT);
            return;
        }
    } while (rcdpi->irq.bit.rx_full ||
             (rcflags.saved_tx_irq && rcdpi->irq.bit.tx_empty) ||
             (rcflags.saved_status_irq && rcdpi->control.bit.new_status));
    rcdpi->irq.bit.tx_irq = rcflags.saved_tx_irq;
    rcdpi->control.bit.status_irq = rcflags.saved_status_irq;
    rcdpi->irq.bit.rx_irq = rcflags.saved_rx_irq;
    rcflags.in_interrupt = 0;
}

void dp_start_sending_clear_down(void)
{
    dp_disable_data_mode();
    if ((rcdpi->conf.byte >= 0xc1 && rcdpi->conf.byte <= 0xce) ||
        (rcdpi->conf.byte >= 0x91 && rcdpi->conf.byte <= 0x9d) ||
        (rcdpi->conf.byte & CONF_V90) == CONF_V90) {
        rcdpi->r15.bit.rate_range = 0;
        rcdpi->conf.byte = CONF_V34;
        rcdpi->r15.bit.renegotiate = 1;
        setup_timer(&clear_down_timer, TICKS(2000));
        clear_down_started = 1;
    } else if (rcdpi->conf.byte >= 0x71 && rcdpi->conf.byte <= 0x76) {
        rcdpi->r15.bit.rate_range = 0;
        rcdpi->conf.byte = CONF_V32;
        rcdpi->r15.bit.renegotiate = 1;
        setup_timer(&clear_down_timer, TICKS(1000));
        clear_down_started = 1;
    } else {
        clear_down_started = 0;
    }
}

char dp_is_clear_down_sent(void)
{
    if (clear_down_started && rcdpi->r15.bit.renegotiate &&
        !timer_expired(clear_down_timer)) {
        return 0;
    }
    return 1;
}

Uint32 dp_read_ram_location(Uint16 address)
{
    return read_dp_ram(address, 2);
}

Uint16 dp_report_fw(void)
{
    return DspDbGetRamFlexPcmVersion();
}

void dp_get_avail_mod_type(Uint32 *mask, Uint8 *rate)
{
    if (!(read_dp_ram(0x9f, 1) & 0x80)) {
        *mask = 0x005f00ff;
    } else {
        *mask = 0x005f02ff;
    }
    if (*mask & (MOD_K56 | MOD_V90)) {
        *rate = RATE_V90_RX_MAX;
    } else {
        *rate = RATE_V34_MAX;
    }
}

void dp_transmit_PTT(Uint8 mode, Uint8 guard, Uint8 level)
{
    char started = 0;

    dp_set_tx_level(level);
    /* @hack redundant: the else after the arm that returns */
    if (mode <= 15) {
        dp_set_dtmf_duration(0, 0);
        dp_set_dial_mode();
        if (mode < 10) {
            dp_dial_number(mode);
        } else {
            switch (mode) {
            case 10:
                dp_dial_number(14);
                break;
            case 11:
                dp_dial_number(10);
                break;
            case 12:
                dp_dial_number(11);
                break;
            case 13:
                dp_dial_number(12);
                break;
            case 14:
                dp_dial_number(15);
                break;
            case 15:
                dp_dial_number(13);
                break;
            }
        }
        return;
    } else {
        switch (mode) {
        case 16:
            rcdpi->r07.bit.unk_bit0 = 1;
            rcdpi->conf.byte = CONF_V21;
            break;
        case 17:
            rcdpi->r07.bit.unk_bit0 = 1;
            rcdpi->conf.byte = 0xa8;
            break;
        case 18:
            rcdpi->r07.bit.unk_bit0 = 1;
            rcdpi->conf.byte = 0xa4;
            break;
        case 19:
            rcdpi->r07.bit.unk_bit0 = 1;
            rcdpi->conf.byte = 0xa1;
            break;
        case 21:
            rcdpi->r07.bit.unk_bit0 = 1;
            rcdpi->conf.byte = 0x52;
            break;
        case 22:
            rcdpi->r07.bit.unk_bit0 = 1;
            rcdpi->conf.byte = 0x84;
            break;
        case 23:
            dp_set_guard_tone(guard);
            rcdpi->conf.byte = 0x51;
            break;
        case 24:
            dp_set_guard_tone(guard);
            rcdpi->r07.bit.unk_bit0 = 0;
            rcdpi->conf.byte = 0x82;
            break;
        case 25:
            rcdpi->r07.bit.unk_bit0 = 0;
            rcdpi->conf.byte = CONF_V21;
            break;
        case 26:
            rcdpi->conf.byte = 0x75;
            break;
        case 27:
            rcdpi->conf.byte = CONF_V32BIS_14400;
            break;
        case 28:
            rcdpi->r07.bit.unk_bit0 = 0;
            rcdpi->conf.byte = 0xa8;
            break;
        case 29:
            rcdpi->r07.bit.unk_bit0 = 0;
            rcdpi->conf.byte = 0xa4;
            break;
        case 30:
            rcdpi->r07.bit.unk_bit0 = 0;
            rcdpi->conf.byte = 0xa1;
            break;
        case 31:
            rcdpi->conf.byte = 0x86;
            return;
        case 32:
            dp_start_tone_generator(2100);
            return;
        case 33:
            dp_set_guard_tone(2);
            rcdpi->conf.byte = 0x86;
            break;
        case 34:
            dp_start_tone_generator(1300);
            return;
        case 35:
            dp_start_tone_generator(1100);
            return;
        case 36:
            break;
        case 37:
            rcdpi->conf.byte = 0x01;
            break;
        case 38:
            rcdpi->conf.byte = 0x02;
            break;
        case 39:
            rcdpi->conf.byte = 0x12;
            break;
        case 40:
            rcdpi->conf.byte = 0x14;
            break;
        case 41:
            dp_set_short_train(0);
            rcdpi->conf.byte = 0xb8;
            break;
        case 42:
            dp_set_short_train(1);
            rcdpi->conf.byte = 0xb8;
            break;
        case 43:
            dp_set_short_train(0);
            rcdpi->conf.byte = 0xb4;
            break;
        case 44:
            dp_set_short_train(1);
            rcdpi->conf.byte = 0xb4;
            break;
        case 45:
            dp_set_short_train(0);
            rcdpi->conf.byte = 0xb2;
            break;
        case 46:
            dp_set_short_train(1);
            rcdpi->conf.byte = 0xb2;
            break;
        case 47:
            dp_set_short_train(0);
            rcdpi->conf.byte = 0xb1;
            break;
        case 48:
            dp_set_short_train(1);
            rcdpi->conf.byte = 0xb1;
            break;
        case 49:
            rcdpi->conf.byte = 0x71;
            break;
        case 50:
            rcdpi->conf.byte = 0x75;
            break;
        case 51:
            rcdpi->conf.byte = 0x71;
            break;
        case 52:
            rcdpi->conf.byte = 0x78;
            break;
        case 53:
            rcdpi->conf.byte = 0x74;
            break;
        case 54:
            rcdpi->conf.byte = 0x72;
            break;
        case 55:
            rcdpi->conf.byte = CONF_V32BIS_14400;
            break;
        case 56:
            dp_set_modulation_type(MOD_V34, 5, 5, 5, 5);
            started = 1;
            break;
        case 57:
            dp_set_modulation_type(MOD_V34, 6, 6, 6, 6);
            started = 1;
            break;
        case 58:
            dp_set_modulation_type(MOD_V34, 7, 7, 7, 7);
            started = 1;
            break;
        case 59:
            dp_set_modulation_type(MOD_V34, 8, 8, 8, 8);
            started = 1;
            break;
        case 60:
            dp_set_modulation_type(MOD_V34, 12, 12, 12, 12);
            started = 1;
            break;
        case 61:
            dp_set_modulation_type(MOD_V34, 13, 13, 13, 13);
            started = 1;
            break;
        case 62:
            dp_set_modulation_type(MOD_V34, 14, 14, 14, 14);
            started = 1;
            break;
        case 63:
            dp_set_modulation_type(MOD_V34, 21, 21, 21, 21);
            started = 1;
            break;
        case 64:
            dp_set_modulation_type(MOD_V34, 22, 22, 22, 22);
            started = 1;
            break;
        case 65:
            dp_set_modulation_type(MOD_V34, 23, 23, 23, 23);
            started = 1;
            break;
        case 66:
            dp_set_modulation_type(MOD_V34, 25, 25, 25, 25);
            started = 1;
            break;
        case 67:
            dp_set_modulation_type(MOD_V34, 29, 29, 29, 29);
            started = 1;
            break;
        case 68:
            dp_set_modulation_type(MOD_V34, 32, 32, 32, 32);
            started = 1;
            break;
        default:
            return;
        }
    }
    if (started) {
        rcdpi->conf.byte = read_dp_ram(RAM_V34_CONF, 1);
        if (rcdpi->conf.byte == 0xc1) {
            write_dp_ram(RAM_BAUD, 0, 1);
        }
        if (rcdpi->conf.byte == 0xce) {
            write_dp_ram(RAM_BAUD, 5, 1);
        }
    }
    rcdpi->r15.bit.auto_mode = 0;
    rcdpi->r08.bit.rts = 1;
    rcdpi->r09.bit.unk_bit2 = 1;
    rcdpi->r09.bit.unk_bit0 = 1;
    rcdpi->r15.bit.unk_bit1 = 1;
    rcdpi->r07.bit.analog_loop = 1;
    if (started) {
        rcdpi->control.bit.newc = 1;
        setup_timer(&dp_timer, TICKS(100));
        while (rcdpi->abcode.byte != V34_MODULE) {
            if (timer_expired(dp_timer)) {
                data_pump_error(DP_ERR_ABCODE);
                return;
            }
        }
        if (!RamDownload(RAM_BANK_CONTROLLER, V34_MODULE, 1, 0, 0)) {
            data_pump_error(DP_ERR_DOWNLOAD);
            return;
        }
        setup_timer(&dp_timer, TICKS(100));
        while (rcdpi->control.bit.newc) {
            if (timer_expired(dp_timer)) {
                data_pump_error(DP_ERR_TEST_NEWC);
                return;
            }
        }
    } else {
        newc();
    }
}

void dp_set_volume(Uint8 volume)
{
    if (volume == 1) {
        rcdpi->volume.bit.level = volume_state = 3;
    } else if (volume == 2) {
        rcdpi->volume.bit.level = volume_state = 1;
    } else if (volume == 3) {
        rcdpi->volume.bit.level = volume_state = 2;
    } else {
        rcdpi->volume.bit.level = volume_state = 0;
    }
}

Uint8 dp_get_eqm10(void)
{
    return read_dp_ram(RAM_EQM10, 1);
}

char dp_will_eqm_be_ok(Uint32 unused_1, Uint8 unused_2)
{
    if (dp_get_eqm10() < EQM10_OK) {
        return 1;
    }
    return 0;
}

/* return RAM_BAUD's index of the symbol rate. a K56flex or V.90
 * configuration that doesn't run V.34 returns BAUD_NONE */
Uint8 dp_get_rx_baud_rate(void)
{
    /* @hack redundant: the else after the arm that returns */
    if (((rcdpi->conf.byte & CONF_V90) == CONF_V90 && !k56flags.v90_as_v34) ||
        ((rcdpi->conf.byte & CONF_CLASS) == CONF_K56 && !k56flags.k56_as_v34)) {
        return BAUD_NONE;
    } else {
        return read_dp_ram(RAM_BAUD, 1);
    }
}

Uint8 dp_get_tx_baud_rate(void)
{
    return read_dp_ram(RAM_BAUD, 1);
}

/* return the speed code of the V.90 receive rate whose index in RAM_RX_RATE
 * is rate. an index of 0, or past 22, returns itself */
static Uint8 v90_rx_speed(Uint8 rate)
{
    switch (rate) {
    case 1:
        rate = 14;
        break;
    case 2:
        rate = 16;
        break;
    case 3:
        rate = 17;
        break;
    case 4:
        rate = 19;
        break;
    case 5:
        rate = 20;
        break;
    case 6:
        rate = 23;
        break;
    case 7:
        rate = 24;
        break;
    case 8:
        rate = 25;
        break;
    case 9:
        rate = 28;
        break;
    case 10:
        rate = 29;
        break;
    case 11:
        rate = 30;
        break;
    case 12:
        rate = 32;
        break;
    case 13:
        rate = 33;
        break;
    case 14:
        rate = 34;
        break;
    case 15:
        rate = 36;
        break;
    case 16:
        rate = 37;
        break;
    case 17:
        rate = 38;
        break;
    case 18:
        rate = 40;
        break;
    case 19:
        rate = 41;
        break;
    case 20:
        rate = 42;
        break;
    case 21:
        rate = 44;
        break;
    case 22:
        rate = 45;
        break;
    }
    return rate;
}

/* the same for K56flex, whose index of 0, or past 13, returns itself */
static Uint8 k56_rx_speed(Uint8 rate)
{
    switch (rate) {
    case 1:
        rate = 19;
        break;
    case 2:
        rate = 22;
        break;
    case 3:
        rate = 24;
        break;
    case 4:
        rate = 26;
        break;
    case 5:
        rate = 29;
        break;
    case 6:
        rate = 31;
        break;
    case 7:
        rate = 33;
        break;
    case 8:
        rate = 35;
        break;
    case 9:
        rate = 37;
        break;
    case 10:
        rate = 39;
        break;
    case 11:
        rate = 41;
        break;
    case 12:
        rate = 43;
        break;
    case 13:
        rate = 45;
        break;
    }
    return rate;
}

/* the same for V.34, whose index of 0, or past 14, returns itself */
static Uint8 v34_rx_speed(Uint8 rate)
{
    switch (rate) {
    case 1:
        rate = 3;
        break;
    case 2:
        rate = 4;
        break;
    case 3:
        rate = 5;
        break;
    case 4:
        rate = 6;
        break;
    case 5:
        rate = 7;
        break;
    case 6:
        rate = 8;
        break;
    case 7:
        rate = 9;
        break;
    case 8:
        rate = 10;
        break;
    case 9:
        rate = 11;
        break;
    case 10:
        rate = 12;
        break;
    case 11:
        rate = 13;
        break;
    case 12:
        rate = 15;
        break;
    case 13:
        rate = 18;
        break;
    case 14:
        rate = 21;
        break;
    }
    return rate;
}

/* return the speed code of rate, as rcdpi's speed reports it, 0 to 29. a
 * rate past 29 returns itself */
static Uint8 register_speed(Uint8 rate)
{
    switch (rate) {
    case 0:
    case 1:
        rate = 1;
        break;
    case 2:
        rate = 2;
        break;
    case 3:
        rate = 3;
        break;
    case 4:
        rate = 4;
        break;
    case 5:
        rate = 6;
        break;
    case 6:
        rate = 7;
        break;
    case 7:
        rate = 8;
        break;
    case 8:
        rate = 5;
        break;
    case 9:
        rate = 9;
        break;
    case 10:
        rate = 10;
        break;
    case 11:
        rate = 11;
        break;
    case 12:
        rate = 12;
        break;
    case 13:
        rate = 13;
        break;
    case 14:
        rate = 15;
        break;
    case 15:
        rate = 18;
        break;
    case 16:
        rate = 21;
        break;
    case 17:
        rate = 19;
        break;
    case 18:
        rate = 22;
        break;
    case 19:
        rate = 24;
        break;
    case 20:
        rate = 26;
        break;
    case 21:
        rate = 29;
        break;
    case 22:
        rate = 31;
        break;
    case 23:
        rate = 33;
        break;
    case 24:
        rate = 35;
        break;
    case 25:
        rate = 37;
        break;
    case 26:
        rate = 39;
        break;
    case 27:
        rate = 41;
        break;
    case 28:
        rate = 43;
        break;
    case 29:
        rate = 45;
        break;
    }
    return rate;
}

Uint8 dp_get_tx_bit_rate(void)
{
    /* @hack redundant: the else after the arm that returns */
    if ((rcdpi->conf.byte & CONF_CLASS) == CONF_K56 ||
        (rcdpi->conf.byte & CONF_CLASS) == CONF_V34 ||
        (rcdpi->conf.byte & CONF_V90) == CONF_V90) {
        return register_speed(rcdpi->speed.bit.rate);
    } else {
        return register_speed(rcdpi->speed.old.rate);
    }
}

Uint8 dp_get_rx_bit_rate(void)
{
    if ((rcdpi->conf.byte & CONF_V90) == CONF_V90 && !k56flags.v90_as_v34) {
        return v90_rx_speed(read_dp_ram(RAM_RX_RATE, 1));
    }
    if ((rcdpi->conf.byte & CONF_CLASS) == CONF_K56 && !k56flags.k56_as_v34) {
        return k56_rx_speed(read_dp_ram(RAM_RX_RATE, 1));
    }
    if ((rcdpi->conf.byte & CONF_CLASS) == CONF_V34 || k56flags.k56_as_v34 ||
        k56flags.v90_as_v34) {
        return v34_rx_speed(read_dp_ram(RAM_RX_RATE, 1));
    }
    return register_speed(rcdpi->speed.old.rate);
}

char dp_test(void)
{
    rcdpi = targetGetComMemInterfaceAddr();
    if (!dp_hardware_reset()) {
        return 0;
    }
    return 1;
}
