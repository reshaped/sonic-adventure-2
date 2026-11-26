#include <sg_xpt.h>

#include "anxdef.h"
#include "atcom.h"
#include "bl.h"
#include "cmi.h"
#include "command.h"
#include "country.h"
#include "countrydb.h"
#include "dce.h"
#include "dte_vxd.h"
#include "ecdc_def.h"
#include "ecdcutil.h"
#include "ee_drver.h"
#include "maintain.h"
#include "messages.h"
#include "modemdef.h"
#include "roc_k56.h"
#include "target.h"
#include "tci_modm.h"
#include "util_mod.h"

/* set as DTR's drop under &D3 asks for a reset, which modem_reset then takes
 * as RESET_DTR */
#define BMR8_DTR_RESET 0x04

/* nothing sets it. go_off_line clears it with BMR21_TEXT */
#define BMR21_UNK_40 0x40

const Uint8 invert_byte_table[256] = {
    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0, 0x10, 0x90, 0x50, 0xd0,
    0x30, 0xb0, 0x70, 0xf0, 0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
    0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8, 0x04, 0x84, 0x44, 0xc4,
    0x24, 0xa4, 0x64, 0xe4, 0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
    0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec, 0x1c, 0x9c, 0x5c, 0xdc,
    0x3c, 0xbc, 0x7c, 0xfc, 0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
    0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2, 0x0a, 0x8a, 0x4a, 0xca,
    0x2a, 0xaa, 0x6a, 0xea, 0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
    0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6, 0x16, 0x96, 0x56, 0xd6,
    0x36, 0xb6, 0x76, 0xf6, 0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
    0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe, 0x01, 0x81, 0x41, 0xc1,
    0x21, 0xa1, 0x61, 0xe1, 0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
    0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9, 0x19, 0x99, 0x59, 0xd9,
    0x39, 0xb9, 0x79, 0xf9, 0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
    0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5, 0x0d, 0x8d, 0x4d, 0xcd,
    0x2d, 0xad, 0x6d, 0xed, 0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
    0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3, 0x13, 0x93, 0x53, 0xd3,
    0x33, 0xb3, 0x73, 0xf3, 0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
    0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb, 0x07, 0x87, 0x47, 0xc7,
    0x27, 0xa7, 0x67, 0xe7, 0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef, 0x1f, 0x9f, 0x5f, 0xdf,
    0x3f, 0xbf, 0x7f, 0xff};

#define MODEM_COPYRIGHT                                                        \
    "(C)COPYRIGHT TRISIGNAL COMMUNICATIONS INC. 1994, ALL RIGHTS RESERVED"
static const char copyright_notice[] = MODEM_COPYRIGHT;
const Uint8 S23_to_fmt[4] = {FORMAT_EVEN_PARITY, FORMAT_8_BITS,
                             FORMAT_ODD_PARITY, FORMAT_MARK_PARITY};
static const Uint8 ss22_map[8] = {0, 0x6a, 0x7e, 0x7e, 0x40, 0x50, 0x60, 0x70};
static const Uint8 ois_map[16] = {0, 1, 0, 0, 2, 3, 3, 3,
                                  4, 7, 7, 7, 5, 6, 6, 6};
/* unused_161 fills the data at the offset its name gives. nothing reads it or
 * copyright_copy */
static const Uint8 unused_161[7] = {1, 8, 9, 0};
static const char copyright_copy[] = MODEM_COPYRIGHT;

void modem_reset(Uint8 mode)
{
    Uint8 i;
    Uint8 *to;
    const Uint8 *from;

    target_set_TCI_timer(100);
    timer_at_4k8 = 0;
    vxd_dte_set_ri(1);
    vxd_dte_set_cts(0);
    dte_set_tst(1);
    li_set_hook(1);
    if ((BMR(8) & BMR8_DTR_RESET) || PwrUpWithSpeed) {
        mode = RESET_DTR;
        PwrUpWithSpeed = 0;
    }
    to = BMRregisters;
    /* @hack type: the size cast to Uint8, where sizeof BMRregisters alone
     * would do */
    for (i = 0; i < (Uint8)sizeof BMRregisters; i++) {
        *to++ = 0;
    }
    from = S_REG;
    to = Sregisters;
    for (i = 0; i < MODEM_S_REGISTERS; i++) {
        *to++ = *from++;
    }
    if (mode < EEPROM_PROFILES) {
        BMR(4) |= BMR4_Z_PROFILE;
        Z_COMMAND = mode;
    }
    eedrvTestEeprom(Sregisters);
    target_dip_switch(Sregisters);
    targetReadCurrentSregistersFile();
    if (cur_cnt->options & MUTE_PULSE_DIALING) {
        li_set_mute(0);
    }
    set_dumb_bit();
    S4_MASK = SREG(4) & 0x7f;
    S3_THEN = SREG(3);
    s27_copy = SREG(27);
    s31_copy = SREG(31);
    if (SREG(0) == 1 && (cur_cnt->reset_flags & RAISE_S0_1)) {
        SREG(0)++;
    }
    if (SREG(0) == 2 && (cur_cnt->reset_flags & RAISE_S0_2)) {
        SREG(0)++;
    }
    if (!(SREG(21) & S21_D2) && (cur_cnt->reset_flags & FORCE_DTR_HANGUP)) {
        SREG(21) &= ~S21_D;
        SREG(21) |= S21_D2;
    }
    if (mode >= EEPROM_PROFILES) {
        speed_of_AT = SREG(20);
        format_of_AT = S23_to_fmt[(SREG(23) & S23_PARITY) >> 4];
    } else {
        TCI_got_format(speed_of_AT, format_of_AT);
    }
    fixed_dte_speed = speed_of_AT;
    fixed_dte_format = format_of_AT;
    CMD_BUF[0] = 'A';
    CMD_BUF[1] = 'T';
    CMD_BUF[2] = SREG(3);
    CMD_B_PT = 0;
    if (mode != RESET_KEEP_PUMP) {
        dp_reset_data_pump();
        dp_get_avail_mod_type(&dp_mod_types, &dp_max_rate);
        target_set_volume(0);
    }
    dp_get_avail_mod_type(&dp_mod_types, &dp_max_rate);
    update_SS22();
    li_set_rj11(1);
    if (!vxd_dte_get_dtr()) {
        BMR(16) |= BMR16_DTR_ON;
    }
    if (!(SREG(21) & 0x20)) {
        vxd_dte_set_dcd(0);
    } else {
        vxd_dte_set_dcd(1);
    }
    if (SREG(21) & 2) {
        if (!(SREG(21) & 0x20)) {
            vxd_dte_set_dsr(0);
        } else {
            vxd_dte_set_dsr(1);
        }
    } else {
        if (!(SREG(21) & 0x40)) {
            vxd_dte_set_dsr(0);
        } else {
            vxd_dte_set_dsr(1);
        }
    }
    if (SREG(49) & 0x40) {
        SREG(14) |= S14_QUIET;
    }
    dial_buffer[0] = SREG(3);
    if (mode == RESET_POWER_UP && (SREG(27) & S27_LEASED_LINE)) {
        BMR(11) |= BMR11_LEASED_LINE;
    }
}

/* @bug logic: writes at most nine digits, so a decimal value of ten digits
 * loses its units digit, at 0x0caf46f8 */
#ifdef NONMATCHING
/* this side repairs the bug tagged above: it writes every digit */
#define MORE_DIGITS(power, count) ((power) > 0)
#else
#define MORE_DIGITS(power, count) ((power) > 0 && (count)++ <= 8)
#endif

void convert_value_to_ascii(Uint32 value, Uint8 base, Uint8 width, char *buffer)
{
    Uint8 count = 1;
    Uint32 power = 1;
    char digit;

#ifdef NONMATCHING
    /* this side writes an empty string for a base outside 2 to 16. a base of 1
     * never ends the loop below for a value above 0, one of 0 divides by 0
     * further on, and one past 16 writes digits past 'F' */
    if (base < 2 || base > 16) {
        *buffer = 0;
        return;
    }
#endif
    while (value / power > base - 1) {
        power = power * base;
        count++;
    }
    while (count++ < width) {
        *buffer++ = '0';
    }
    count = 1;
    do {
        digit = value / power;
        value -= digit * power;
        power /= base;
        if (digit >= 10) {
            digit += 'A' - 10;
        } else {
            digit += '0';
        }
        /* @hack spelling: the store and the step apart, where *buffer++ =
         * digit would do */
        *buffer = digit;
        buffer++;
    } while (MORE_DIGITS(power, count));
    *buffer = 0;
}

Uint8 is_hex_number(Uint8 character)
{
    if ((character >= '0' && character <= '9') ||
        (character >= 'A' && character <= 'F')) {
        return 1;
    }
    return 0;
}

void update_SS22(void)
{
    SS22 = ss22_map[(SREG(22) & S22_RESULTS) >> 4] | (SS22 & 0x81);
}

void update_S44(void)
{
    if (BMR(29) & BMR29_MNP) {
        if (BMR(29) & BMR29_COMPRESSION) {
            if (BMR(29) & BMR29_V42BIS) {
                SREG(44) = 8;
            } else {
                SREG(44) = 5;
            }
        } else if (max_info_field) {
            SREG(44) = 4;
        } else if (bit_oriented_mode) {
            SREG(44) = 3;
        } else {
            SREG(44) = 2;
        }
    } else if (BMR(27) & BMR27_LAPM) {
        if (!(BMR(29) & BMR29_COMPRESSION)) {
            SREG(44) = 6;
        } else {
            SREG(44) = 7;
        }
    } else {
        SREG(44) = 0;
    }
}

void update_OIS(void)
{
    Uint8 value;
    Uint8 other;
    Uint16 detection;
    Uint32 v8;
    Uint8 *parameter;

    v8 = dp_get_v8_status() & 1;
    value = 0xc0;
    if (SREG(27) & S27_Q) {
        value |= 1;
    } else {
        other = SREG(36);
        if (v8 && ((SREG(36) & 8) || (SREG(36) & 0x80))) {
            other &= 0x88;
        }
        if (SREG(14) & S14_ORIGINATE) {
            value |= ois_map[other & 0xf];
        } else {
            value |= ois_map[(other & 0xf0) >> 4];
        }
    }
    cmd_parameter_buffer[0] = value;
    /* @hack redundant: a store of 0 that the next line overwrites */
    value = 0;
    value = (SREG(28) & 3) << 4;
    if (SREG(24) & 2) {
        value |= OIS1_FALLBACK_SEND;
    }
    if (SREG(24) & 1) {
        value |= OIS1_FALLBACK_TERMINATOR;
    }
    value |= OIS1_KEEP_LENGTH;
    if (!(SREG(39) & S39_BREAK_DESTRUCTIVE)) {
        if (SREG(39) & S39_BREAK_EXPEDITED) {
            value |= OIS1_BREAK_EXPEDITED;
        } else {
            value |= OIS1_BREAK_IN_SEQUENCE;
        }
    }
    if (SREG(42) & 2) {
        value |= OIS1_SEND_BREAK;
    }
    cmd_parameter_buffer[1] = value;
    value = SREG(46) & 0x7f;
    if (SREG(24) & 4) {
        value |= OIS2_V42BIS;
    }
    cmd_parameter_buffer[2] = value;
    detection = (SREG(28) & 0x7c) >> 2;
    if (BMR(6) & 1) {
        detection += dp_read_round_trip_delay() / 50;
        if (detection > 31) {
            detection = 31;
        }
    }
    cmd_parameter_buffer[3] = detection << 3;
    value = SREG(40);
    if (value > 127) {
        value = 127;
    }
    value |= 0x80;
    cmd_parameter_buffer[4] = value;
    value = 0;
    if (!(BMR(10) & BMR10_DIRECT_MODE) && (SREG(49) & 0xf)) {
        if (SREG(49) & 4) {
            value = 0x20;
        } else if ((SREG(49) & 3) == 3) {
            value = 0x60;
        } else if (SREG(49) & 1) {
            value = 0x40;
        } else {
            value = 0x80;
        }
    }
    cmd_parameter_buffer[5] = value;
    value = 0x80;
    if (BMR(10) & 4) {
        value |= 0x40;
    }
    value |= 0x10;
    cmd_parameter_buffer[6] = value;
    value = 0;
    if (SREG(14) & S14_ORIGINATE) {
        value |= OIS7_ORIGINATE;
    }
    if (!speed_of_dce) {
        if (SREG(14) & S14_ORIGINATE) {
            value |= 0x20;
        } else {
            value |= 0x40;
        }
    }
    cmd_parameter_buffer[7] = value;
    cmd_parameter_buffer[8] = SREG(29);
    value = SREG(30) & ~OIS9_SEND_ODP;
    if ((value & OIS9_WINDOW) > 15) {
        value = (value & ~OIS9_WINDOW) | 0xf;
    }
    if ((SREG(28) & 0x80) && !v8) {
        value |= OIS9_SEND_ODP;
    }
    cmd_parameter_buffer[9] = value;
    value = 0;
    if ((SREG(32) & 0x80) || SREG(16)) {
        value |= 0x40;
    }
    value |= (SREG(32) & 0xc) << 2;
    if (!(SREG(27) & S27_Q)) {
        if (((SREG(14) & S14_ORIGINATE) && (SREG(36) & 1) && !(SREG(36) & 2)) ||
            (!(SREG(14) & S14_ORIGINATE) && (SREG(36) & 0x10) &&
             !(SREG(36) & 0x20))) {
            value |= OIS10_FALLBACK_RAW;
        }
    }
    if (SREG(30) & 0x40) {
        value |= 4;
    }
    cmd_parameter_buffer[10] = value;
    value = 0;
    if (SREG(49) & 8) {
        value |= OIS11_KEEP_XON_XOFF;
    }
    if (SREG(49) & 0x10) {
        value |= OIS11_XON_XOFF;
    }
    if (SREG(42) & 1) {
        value |= OIS11_BREAK_LENGTH;
    }
    if (SREG(32) & 1) {
        value |= OIS11_ENCRYPTION;
    }
    value |= (SREG(42) & 0xc0) >> 4;
    cmd_parameter_buffer[11] = value;
    GetXonXoffValues(&value, &other);
    cmd_parameter_buffer[12] = value;
    cmd_parameter_buffer[13] = other;
    /* @hack redundant: a store of 0 that the next line overwrites */
    value = 0;
    value = SREG(47) & 0x7f;
    cmd_parameter_buffer[14] = value;
    /* @hack redundant: a store of 0 that the next line overwrites */
    value = 0;
    value = SREG(48) & 0x7f;
    cmd_parameter_buffer[15] = value;
    if (SREG(49) & 0x40) {
        cmd_parameter_buffer[16] = 0x80;
    } else {
        cmd_parameter_buffer[16] = SREG(2);
    }
    cmd_parameter_buffer[17] = SREG(12);
    cmd_parameter_buffer[18] = SREG(38);
    cmd_parameter_buffer[19] = SREG(67);
    cmd_parameter_buffer[20] = SREG(10);
    cmd_parameter_buffer[21] = SREG(33);
    cmd_parameter_buffer[22] = SREG(37);
    value = 0;
    value |= SREG(39) & S39_LINE_QUALITY;
    value |= (SREG(32) & 0x10) << 2;
    value |= (BMR(26) & 0x80) >> 1;
    value |= (SREG(24) & S24_N1) << 1;
    cmd_parameter_buffer[23] = value;
    cmd_parameter_buffer[OIS_BREAK_LENGTH] = BREAK_DEFAULT_TICKS;
    value = 0;
    if (SREG(21) & S21_LONG_SPACE_DISCONNECT) {
        value |= OIS25_LONG_SPACE_DISCONNECT;
    }
    value |= (SREG(42) & 0x3c) >> 2;
    if (!(SREG(24) & S24_FIXED_DTE)) {
        value |= OIS25_FOLLOW_LINE;
    }
    if (SREG(72) & 3) {
        value |= 0x20;
    }
    cmd_parameter_buffer[25] = value;
    cmd_parameter_buffer[26] = SREG(74);
    cmd_parameter_buffer[27] = speed_of_AT;
    cmd_parameter_buffer[28] = speed_of_dce;
    if ((cmd_parameter_buffer[0] & LINK_MODE) == 1) {
        cmd_parameter_buffer[0] &= ~LINK_MODE;
        cmd_parameter_buffer[5] &= 0x1f;
        cmd_parameter_buffer[25] |= OIS25_FOLLOW_LINE;
    }
    value = 0;
    other = sizeof OISregisters;
    parameter = cmd_parameter_buffer;
    while (other--) {
        value += *parameter++;
    }
    /* the checksum makes the parameters and itself sum to 0, as get_OIS
     * checks */
    cmd_parameter_buffer[sizeof OISregisters] = -value;
}

void init_on_line(void)
{
    SREG(45) = 0;
    ON_LI_TIM = TIM_10MS;
    if (SREG(21) & 1) {
        li_set_rj11(0);
    } else {
        li_set_rj11(1);
    }
    if (!(BMR(9) & BMR9_OFF_HOOK)) {
        dp_init_data_pump(cur_cnt->pump_option_1, cur_cnt->pump_option_2);
    }
    BMR(9) |= BMR9_OFF_HOOK;
    li_set_hook(0);
    dp_off_hook();
    BMR(4) &= 0xef;
    if ((SREG(27) & 0xc) > 2) {
        BMR(4) |= 0x10;
    }
}

void umCarrierDetected(void)
{
    send_carrier_message();
}

static void start_data_phase(void);

void modem_connect(void)
{
    SS41 = 0;
    if (SREG(14) & S14_ORIGINATE) {
        BL_Condition(BL_EVENT_CONNECTED);
    }
    update_S44();
    BMR(10) |= 0x20;
    setup_timer(&dtr_timer, SREG(25) * 100);
    send_connect_message();
    start_data_phase();
}

static void start_data_phase(void)
{
    restart_TIES();
    BMR(7) &= ~BMR7_COMMAND_LINE;
    maintain_phase = MAINTAIN_ONLINE;
    if (BMR(3) & BMR3_PATTERN) {
        BMR(6) = (BMR(6) & 0xb8) | BMR6_ON_LINE | BMR6_COMMAND;
        set_command_mode();
    } else {
        BMR(6) = (BMR(6) & 0xb8) | BMR6_ON_LINE | BMR6_DATA;
        cmd_START_DATA();
    }
}

void carrier_lost(void)
{
    carrier_off = 1;
    cmd_parameter_buffer[0] = HALT_AFTER_TERMINAL;
    cmd_response_buffer[0] = ECDC_RESPONSE_NONE;
    cmd_HALT();
    maintain_phase = MAINTAIN_DRAIN;
}

void go_off_line(void)
{
    if (BMR(9) & BMR9_OFF_HOOK) {
        if (BMR(6) & BMR6_ON_LINE) {
            ON_LI_TIM = TIM_10MS - ON_LI_TIM;
        }
        TCI_got_format(speed_of_AT, format_of_AT);
        li_set_hook(1);
        BMR(9) &= ~BMR9_OFF_HOOK;
        dce_set_interface(DCE_HELD, TCI_SPEED_14400, FORMAT_8_BITS, 0, 0, 0, 1);
        dp_on_hook();
        target_set_volume(0);
        BMR(3) &= ~BMR3_TESTS;
        BMR(6) &= 0xf0;
        BMR(10) &= 0xfb;
        BMR(12) &= 0xde;
        BMR(16) &= 0x7f;
        BMR(16) &= 0xfd;
        BMR(20) &= 0x7f;
        BMR(21) &= (Uint8) ~(BMR21_UNK_40 | BMR21_TEXT);
        BMR(26) &= 0x1f;
        BMR(27) &= ~BMR27_LAPM;
        BMR(29) &= ~BMR29_MNP;
        SREG(16) &= 2;
        SREG(1) = 0;
        if (SREG(21) & 0x20) {
            vxd_dte_set_dcd(1);
        }
        if (SREG(21) & 0x40) {
            vxd_dte_set_dsr(1);
        }
        vxd_dte_set_cts(0);
        dte_set_tst(1);
        setup_timer(&OFF_HOOK_TIM, cur_cnt->redial_delay);
        dp_test();
        BMR(11) &= 0xdf;
    }
    maintain(0);
}

void force_h0_cmd(void)
{
    if (BMR(6) & BMR6_ON_LINE) {
        set_on_line_cmd();
    }
    ties_phase = 0;
    BMR(22) &= 0x7f;
    BMR(7) |= BMR7_COMMAND_LINE;
    BMR(6) |= BMR6_RUNNING_COMMAND;
    BMR(5) |= 5;
}

void force_a_cmd(void)
{
    set_dte_for_message();
    BMR(22) &= 0x7f;
    BMR(7) |= BMR7_COMMAND_LINE;
    BMR(6) |= BMR6_RUNNING_COMMAND;
    BMR(5) |= 3;
}

void force_d_cmd(void)
{
    set_dte_for_message();
    ties_phase = 0;
    BMR(22) &= 0x7f;
    BMR(7) |= BMR7_COMMAND_LINE;
    BMR(6) |= BMR6_RUNNING_COMMAND;
    BMR(5) |= 9;
}

void force_z_cmd(void)
{
    set_dte_for_message();
    BMR(22) &= 0x7f;
    BMR(7) |= BMR7_COMMAND_LINE;
    BMR(6) |= BMR6_RUNNING_COMMAND;
    CMD_B_PT = 4;
    CMD_BUF[0] = 'A';
    CMD_BUF[1] = 'T';
    CMD_BUF[2] = 'Z';
    CMD_BUF[3] = S3_THEN;
}

void force_DS_cmd(Uint8 number)
{
    set_dte_for_message();
    BMR(22) &= 0x7f;
    BMR(7) |= BMR7_COMMAND_LINE;
    BMR(6) |= BMR6_RUNNING_COMMAND;
    CMD_B_PT = 6;
    CMD_BUF[0] = 'A';
    CMD_BUF[1] = 'T';
    CMD_BUF[2] = 'D';
    CMD_BUF[3] = 'S';
    CMD_BUF[4] = '=';
    CMD_BUF[5] = number + '0';
    CMD_BUF[6] = S3_THEN;
}

void force_o_cmd(void)
{
    set_dte_for_message();
    BMR(22) &= 0x7f;
    BMR(7) |= BMR7_COMMAND_LINE;
    BMR(6) |= BMR6_RUNNING_COMMAND;
    CMD_B_PT = 4;
    CMD_BUF[0] = 'A';
    CMD_BUF[1] = 'T';
    CMD_BUF[2] = 'O';
    CMD_BUF[3] = S3_THEN;
}

void enter_escape_mode(void)
{
    set_on_line_cmd();
    message_byte_to_dte(SREG(3));
    send_coded_message(RESULT_OK, NULL);
    CMD_B_PT = 0;
    set_command_mode();
}

void set_on_line_cmd(void)
{
    if (!(BMR(6) & BMR6_COMMAND)) {
        BMR(6) = (BMR(6) & 0xf8) | BMR6_ON_LINE | BMR6_COMMAND;
        BMR(22) &= 0x7f;
        cmd_STOP_DATA();
        set_dte_for_message();
    }
}

void set_command_mode(void)
{
    Uint8 xon;
    Uint8 xoff;
    Uint8 flow;
    Uint8 mode;

    BMR(6) &= ~BMR6_RUNNING_COMMAND;
    if (SREG(49) & 0x40) {
        vxd_dte_set_interface(DTE_HELD, speed_of_AT, format_of_AT, 0, SREG(2),
                              SREG(12), SREG(47), SREG(48), 0);
        BMR(22) &= 0x7f;
    } else {
        BMR(31) &= 0xfd;
        BMR(32) &= 0xbf;
        if ((SREG(49) & 3) == 3) {
            flow = 1;
        } else if (SREG(49) & 1) {
            flow = 5;
        } else if (SREG(49) & 4) {
            flow = 2;
        } else {
            flow = 0;
        }
        GetXonXoffValues(&xon, &xoff);
        mode = (s31_copy & 0x10) ? DTE_ASYNC_CMD : DTE_RATE_CMD;
        vxd_dte_set_interface(mode, speed_of_AT, format_of_AT, flow, SREG(2),
                              SREG(12), xon, xoff, 0);
        BMR(22) |= 0x80;
    }
}

void set_dte_for_message(void)
{
    Uint8 xon;
    Uint8 xoff;
    Uint8 flow;

    if (SREG(49) & 0x40) {
        vxd_dte_set_interface(DTE_HELD, speed_of_AT, format_of_AT, 0, SREG(2),
                              SREG(12), SREG(47), SREG(48), 0);
    }
    BMR(32) &= 0xbf;
    BMR(22) &= 0x7f;
    BMR(31) &= 0xfd;
    if ((SREG(49) & 3) == 3) {
        flow = 1;
    } else if (SREG(49) & 1) {
        flow = 5;
    } else if (SREG(49) & 4) {
        flow = 2;
    } else {
        flow = 0;
    }
    GetXonXoffValues(&xon, &xoff);
    vxd_dte_set_interface(DTE_ASYNC_CMD, speed_of_AT, format_of_AT, flow,
                          SREG(2), SREG(12), xon, xoff, 0);
}

void adj_spk_level(void)
{
    if ((SREG(22) & S22_VOLUME) <= 1) {
        target_set_volume(1);
    } else if ((SREG(22) & S22_VOLUME) == 2) {
        target_set_volume(2);
    } else {
        target_set_volume(3);
    }
}

void redial_monitor(void)
{
    if (BMR(7) & BMR7_HOST_BYTE) {
        SS41 = 0;
        BMR(1) &= 0xfe;
        return;
    }
    if (SS41) {
        if (!SREG(1) && !(BMR(1) & 1)) {
            if (timer_expired(OFF_HOOK_TIM)) {
                if (!(BMR(9) & BMR9_OFF_HOOK) && li_get_lcs()) {
                    SS41 = 0;
                    BMR(1) &= 0xfe;
                    return;
                }
                SS41--;
                BMR(1) |= 1;
                force_d_cmd();
            }
        }
    }
}

void GetXonXoffValues(Uint8 *xon, Uint8 *xoff)
{
    Uint8 xon_parity;
    Uint8 xoff_parity;
    Uint8 xon_byte;
    Uint8 xoff_byte;

    xon_parity = 0;
    if (SREG(47) & 1) {
        xon_parity++;
    }
    if (SREG(47) & 2) {
        xon_parity++;
    }
    if (SREG(47) & 4) {
        xon_parity++;
    }
    if (SREG(47) & 8) {
        xon_parity++;
    }
    if (SREG(47) & 0x10) {
        xon_parity++;
    }
    if (SREG(47) & 0x20) {
        xon_parity++;
    }
    if (SREG(47) & 0x40) {
        xon_parity++;
    }
    if (SREG(47) & 0x80) {
        xon_parity++;
    }
    xon_parity &= 1;
    xoff_parity = 0;
    if (SREG(48) & 1) {
        xoff_parity++;
    }
    if (SREG(48) & 2) {
        xoff_parity++;
    }
    if (SREG(48) & 4) {
        xoff_parity++;
    }
    if (SREG(48) & 8) {
        xoff_parity++;
    }
    if (SREG(48) & 0x10) {
        xoff_parity++;
    }
    if (SREG(48) & 0x20) {
        xoff_parity++;
    }
    if (SREG(48) & 0x40) {
        xoff_parity++;
    }
    if (SREG(48) & 0x80) {
        xoff_parity++;
    }
    xoff_parity &= 1;
    if (!(SREG(23) & S23_PARITY)) {
        if (xon_parity == 0) {
            xon_byte = SREG(47);
        } else {
            xon_byte = SREG(47) | PARITY_BIT;
        }
        if (xoff_parity == 0) {
            xoff_byte = SREG(48);
        } else {
            xoff_byte = SREG(48) | PARITY_BIT;
        }
    } else if ((SREG(23) & S23_PARITY) == S23_SPACE_PARITY) {
        xon_byte = SREG(47);
        xoff_byte = SREG(48);
    } else if ((SREG(23) & S23_PARITY) == S23_ODD_PARITY) {
        if (xon_parity) {
            xon_byte = SREG(47);
        } else {
            xon_byte = SREG(47) | PARITY_BIT;
        }
        if (xoff_parity) {
            xoff_byte = SREG(48);
        } else {
            xoff_byte = SREG(48) | PARITY_BIT;
        }
    } else {
        xon_byte = SREG(47) | PARITY_BIT;
        xoff_byte = SREG(48) | PARITY_BIT;
    }
    *xon = xon_byte;
    *xoff = xoff_byte;
}

static void hang_up_if_on_line(void)
{
    if (BMR(9) & BMR9_OFF_HOOK) {
        force_h0_cmd();
    }
}

Uint8 dtr_monitor(void)
{
    char dtr_off = vxd_dte_get_dtr();
    char expired = timer_expired(dtr_timer);
    Uint8 mode;

    if (SREG(21) & S21_D) {
        if (SREG(21) & 2) {
            if (!(SREG(21) & 0x20)) {
                if (dtr_off) {
                    if ((BMR(16) & BMR16_DTR_DROPPED) &&
                        !(BMR(16) & BMR16_DTR_ON) && expired) {
                        vxd_dte_set_dcd(1);
                    }
                } else {
                    vxd_dte_set_dcd(0);
                }
            }
            if (!(SREG(21) & 0x40)) {
                if (dtr_off) {
                    if ((BMR(16) & BMR16_DTR_DROPPED) &&
                        !(BMR(16) & BMR16_DTR_ON) && expired) {
                        vxd_dte_set_dsr(1);
                    }
                } else {
                    vxd_dte_set_dsr(0);
                }
            }
        }
    }
    mode = SREG(27) & S27_Q;
    if (mode == 0 || (!(BMR(6) & BMR6_ON_LINE) && mode == 1)) {
        if (SREG(21) & S21_D) {
            if (dtr_off) {
                if (BMR(16) & BMR16_DTR_ON) {
                    BMR(16) &= ~BMR16_DTR_ON;
                    BMR(16) |= BMR16_DTR_DROPPED;
                    setup_timer(&dtr_timer, SREG(25));
                    return 0;
                } else if ((BMR(16) & BMR16_DTR_DROPPED) && expired) {
                    BMR(16) &= ~BMR16_DTR_DROPPED;
                    switch (SREG(21) & S21_D) {
                    case S21_D1:
                        if ((BMR(6) & BMR6_DATA) && !(BMR(3) & BMR3_PATTERN)) {
                            enter_escape_mode();
                        }
                        return 1;
                    case S21_D2:
                        hang_up_if_on_line();
                        return 2;
                    case S21_D3:
                        force_z_cmd();
                        BMR(8) |= BMR8_DTR_RESET;
                        return 3;
                    }
                }
            } else {
                BMR(16) |= BMR16_DTR_ON;
            }
        }
    } else if (BMR(6) & BMR6_ON_LINE) {
        if (BMR(10) & 0x20) {
            if (mode != 1 || expired) {
                BMR(10) &= 0xdf;
                if (dtr_off) {
                    BMR(16) &= ~BMR16_DTR_ON;
                } else {
                    BMR(16) |= BMR16_DTR_ON;
                }
            }
        } else if (!dtr_off) {
            BMR(16) |= BMR16_DTR_ON;
        } else {
            if (BMR(16) & BMR16_DTR_ON) {
                BMR(16) &= ~BMR16_DTR_ON;
                BMR(16) |= BMR16_DTR_DROPPED;
                setup_timer(&dtr_timer, SREG(25));
                return 0;
            }
            if ((BMR(16) & BMR16_DTR_DROPPED) && expired) {
                BMR(16) &= ~BMR16_DTR_DROPPED;
                if ((SREG(21) & S21_D) == S21_D1 && mode == 1) {
                    if ((BMR(6) & BMR6_DATA) && !(BMR(3) & BMR3_PATTERN)) {
                        enter_escape_mode();
                    }
                    return 1;
                }
                hang_up_if_on_line();
                return 2;
            }
            return 0;
        }
    } else if (!dtr_off) {
        if (!(BMR(16) & BMR16_DTR_ON)) {
            BMR(16) |= BMR16_DTR_ON;
            if (!(BMR(31) & 2)) {
                BMR(2) |= BMR2_DTR_DIAL;
                force_d_cmd();
                return 4;
            }
        }
    } else {
        BMR(16) &= ~BMR16_DTR_ON;
    }
    return 0;
}

void escape_seq_monitor(void)
{
    if (BMR(14) & 0x10) {
        BMR(14) &= 0xef;
        enter_escape_mode();
    } else if (BMR(21) & 0x20) {
        BMR(21) &= 0xdf;
        set_on_line_cmd();
        BMR(6) |= BMR6_RUNNING_COMMAND;
        BMR(7) |= BMR7_COMMAND_LINE;
    }
}

void rts_monitor(void)
{
}
