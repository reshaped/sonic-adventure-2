#include <sg_xpt.h>
#include <stdio.h>
#include <string.h>

#include "atcom_ex.h"
#include "dte_vxd.h"
#include "line_estab.h"
#include "messages.h"
#include "modemdef.h"
#include "roc_k56.h"
#include "util_mod.h"
#include "v25ter.h"

/* ANY_RATE is a maximum above every rate code. v25ter_rate_at_least and
 * v25ter_rate_at_most return NO_RATE for a rate no code rounds to */
#define ANY_RATE 0xff
#define NO_RATE 0xff

/* read_string pads a modulation's name with spaces to NAME_WIDTH */
#define NAME_WIDTH 8
#define BUFFER_LENGTH 70

/* a reply to the host. while direct is set, v25ter_put writes into the room
 * vxd_dte_set_tx_data gave, and otherwise through send_message. only
 * v25ter_put sets direct, and only where it is set already.
 * v25terReadCommand clears it, so every reply goes through send_message */
struct v25ter_message {
    Uint8 direct;
    Uint8 *write;
    Uint16 used;
    Uint16 room;
};

/* each unused_ variable fills the bss at the offset its name gives. nothing
 * reads or writes one */
static Uint8 unused_00;
static Uint8 v25ter_result;
static Uint8 unused_02[106];
static struct v25ter_message v25ter_message;
static char *v25ter_cursor;
static Uint8 unused_7c[8];
static char v25ter_buffer[BUFFER_LENGTH];
static char v25ter_cr_lf[3];

static char v25ter_is_extended(void);
static char v25ter_is_allowed(void);
static Uint8 v25ter_execute(void);
static char v25ter_is_fclass(void);
static Uint8 v25ter_fclass(void);
static void v25ter_execute_m(void);
static void v25ter_ma(void);
static void v25ter_report_ma(Uint32 modulations);
static void v25ter_set_ma(void);
static void v25ter_ms(void);
static void v25ter_report_ms_range(void);
static void v25ter_put_ms_modulations(void);
static void v25ter_report_ms(void);
static char v25ter_put_modulation(Uint32 modulations);
static void v25ter_put_rate(Uint8 rate);
static void v25ter_set_ms(void);
static Uint32 v25ter_modulation_by_name(char *name);
static Uint8 v25ter_rate_at_least(Uint32 bps);
static Uint8 v25ter_rate_at_most(Uint32 bps);
static char v25ter_start_message(char direct);
static char v25ter_put(char *text);
static void v25ter_put_cr_lf(void);
static void v25ter_flush(void);

void send_message_to_dte(char *text)
{
    send_message(text);
    send_cr_lf();
}

Uint8 v25terReadCommand(Uint8 **command)
{
    Uint8 result;

    if (!v25ter_start_message(0)) {
        return V25TER_ERROR;
    }

    v25ter_cursor = (char *)*command;
    v25ter_cursor++;
    if (v25ter_is_extended()) {
        if (v25ter_is_allowed()) {
            result = v25ter_execute();
        } else {
            result = V25TER_ERROR;
        }
    } else if (v25ter_is_fclass()) {
        result = v25ter_fclass();
    } else {
        result = V25TER_ERROR;
    }
    if (result == 0) {
        result = V25TER_ERROR;
    }
    *command = (Uint8 *)v25ter_cursor;
    v25ter_flush();
    return result;
}

void v25terRemoveModNotAuthorised(Uint32 *modulations, Uint8 min_tx,
                                  Uint8 max_tx, Uint8 min_rx, Uint8 max_rx)
{
    if (max_tx == 0) {
        max_tx = ANY_RATE;
    }
    if (max_rx == 0) {
        max_rx = ANY_RATE;
    }
    *modulations &= dp_mod_types;
    if (*modulations & MOD_V90) {
        if (max_tx < RATE_V34_MIN || max_rx < RATE_V90_RX_MIN ||
            min_tx > RATE_V34_MAX || min_rx > RATE_V90_RX_MAX) {
            *modulations &= ~MOD_V90;
        }
    }
    if (*modulations & MOD_K56) {
        if (max_tx < RATE_V34_MIN || max_rx < RATE_32000 ||
            min_tx > RATE_V34_MAX || min_rx > RATE_56000) {
            *modulations &= ~MOD_K56;
        }
    }
    if ((*modulations & MOD_V34) &&
        (max_tx < RATE_V34_MIN || max_rx < RATE_V34_MIN ||
         min_tx > RATE_V34_MAX || min_rx > RATE_V34_MAX)) {
        *modulations &= ~MOD_V34;
    }
    if ((*modulations & MOD_V32BIS) &&
        (max_tx < RATE_4800 || min_tx > RATE_14400 || max_rx < RATE_4800 ||
         min_rx > RATE_14400)) {
        *modulations &= ~MOD_V32BIS;
    }
    if ((*modulations & MOD_V32) &&
        (max_tx < RATE_4800 || min_tx > RATE_9600 || max_rx < RATE_4800 ||
         min_rx > RATE_9600)) {
        *modulations &= ~MOD_V32;
    }
    if ((*modulations & MOD_V22BIS) &&
        (max_tx < RATE_1200 || min_tx > RATE_2400 || max_rx < RATE_1200 ||
         min_rx > RATE_2400)) {
        *modulations &= ~MOD_V22BIS;
    }
    if ((*modulations & MOD_V22) &&
        (max_tx < RATE_1200 || min_tx > RATE_1200 || max_rx < RATE_1200 ||
         min_rx > RATE_1200)) {
        *modulations &= ~MOD_V22;
    }
    if ((*modulations & MOD_V23) &&
        ((max_tx < RATE_1200 && max_rx < RATE_1200) ||
         (min_tx > RATE_75 && min_rx > RATE_75) || min_tx > RATE_1200 ||
         min_rx > RATE_1200 || (min_tx > RATE_75 && max_tx < RATE_1200) ||
         (min_rx > RATE_75 && max_rx < RATE_1200))) {
        *modulations &= ~MOD_V23;
    }
    if ((*modulations & MOD_V21) && (max_tx < RATE_300 || min_tx > RATE_300 ||
                                     max_rx < RATE_300 || min_rx > RATE_300)) {
        *modulations &= ~MOD_V21;
    }
    /* @bug logic: a receive range of 600 bit/s alone keeps MOD_BELL, which
     * has no 600 bit/s mode. the test at 0x0caf5cba reads only the transmit
     * range */
    if ((*modulations & MOD_BELL) &&
        (max_tx < RATE_300 || min_tx > RATE_1200 || max_rx < RATE_300 ||
         min_rx > RATE_1200 || (max_tx == RATE_600 && min_tx == RATE_600))) {
        *modulations &= ~MOD_BELL;
    }
}

void v25terUpdateV25terPar(void)
{
}

Uint32 v25terFindMaxBit(Uint32 mask)
{
    Uint32 bit = 0x80000000UL;
    Uint8 count = 31;

    while (!(mask & bit)) {
        if (count-- == 0) {
            return 1;
        }
        bit >>= 1;
    }
    return bit;
}

struct v25ter_config v25terReportConf(void)
{
    struct v25ter_config conf;
    struct v25ter_config copy;
    struct v25ter_config *to;
    struct v25ter_config *from;

    /* @bug uninitialized: the result's reserved bits are as the stack left
     * them. nothing writes conf.reserved before the copy reads it at
     * 0x0caf5de6 */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: the reserved bits are 0 */
    conf.reserved = 0;
#endif
    conf.bit0 = (SREG(36) & 1) ? 1 : 0;
    conf.bit1 = (SREG(36) & 2) ? 1 : 0;
    conf.bit2 = (SREG(36) & 4) ? 1 : 0;
    conf.bit3 = (SREG(36) & 8) ? 1 : 0;
    conf.bit4 = (SREG(36) & 0x10) ? 1 : 0;
    conf.bit5 = (SREG(36) & 0x20) ? 1 : 0;
    conf.bit6 = (SREG(36) & 0x40) ? 1 : 0;
    conf.bit7 = (SREG(36) & 0x80) ? 1 : 0;

    /* @hack spelling: the field-by-field copy of conf through from and to,
     * where return conf would do */
    from = &conf;
    to = &copy;

    to->reserved = from->reserved;
    to->bit7 = from->bit7;
    to->bit6 = from->bit6;
    to->bit4 = from->bit4;
    to->bit5 = from->bit5;
    to->bit3 = from->bit3;
    to->bit2 = from->bit2;
    to->bit1 = from->bit1;
    to->bit0 = from->bit0;
    return copy;
}

static char v25ter_is_extended(void)
{
    switch (*v25ter_cursor) {
    case 'A':
    case 'D':
    case 'E':
    case 'G':
    case 'I':
    case 'M':
    case 'T':
        return 1;
    }
    return 0;
}

static char v25ter_is_allowed(void)
{
    char allowed = 0;

    if (!strncmp(v25ter_cursor, "GCI", 3)) {
        allowed = 1;
    } else if (SREG(31) & S31_EXTENDED) {
        allowed = 1;
    } else if ((SREG(24) & S24_N2) && (SREG(24) & S24_N1)) {
        if (*v25ter_cursor == 'A' || !strncmp(v25ter_cursor, "MS", 2) ||
            !strncmp(v25ter_cursor, "MA", 2)) {
            allowed = 1;
        }
    }
    return allowed;
}

static Uint8 v25ter_execute(void)
{
    v25ter_result = V25TER_OK;
    switch (*v25ter_cursor++) {
    case 'M':
        v25ter_execute_m();
        break;
    default:
        return 0;
    }
    return v25ter_result;
}

static char v25ter_is_fclass(void)
{
    char found = 0;

    if (!strncmp(v25ter_cursor, "FCLASS", 6)) {
        found = 1;
    }
    return found;
}

static Uint8 v25ter_fclass(void)
{
    /* @bug uninitialized: +FCLASS returns result, which nothing sets. at
     * 0x0caf5fb8 the result is the stack pointer's low byte */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: +FCLASS returns V25TER_ERROR */
    return V25TER_ERROR;
#else
    Uint8 result;
    return result; /* NOLINT(clang-diagnostic-uninitialized) */
#endif
}

static void v25ter_execute_m(void)
{
    if (*v25ter_cursor == 'S') {
        v25ter_cursor++;
        v25ter_ms();
    } else if (*v25ter_cursor == 'A') {
        v25ter_cursor++;
        v25ter_ma();
    } else {
        v25ter_result = V25TER_ERROR;
    }
}

static void v25ter_ma(void)
{
    switch (*v25ter_cursor++) {
    case '=':
        if (*v25ter_cursor == '?') {
            v25ter_cursor++;
            if (SREG(14) & S14_VERBOSE) {
                v25ter_put_cr_lf();
            }
            /* dp_mod_types holds other bits above its low 16, which
             * v25terFindMaxBit would take for the carrier */
            v25ter_report_ma((Uint16)dp_mod_types);
        } else {
            v25ter_set_ma();
        }
        break;
    case '?':
        if (SREG(14) & S14_VERBOSE) {
            v25ter_put_cr_lf();
        }
        v25ter_report_ma(MS_MODULATIONS);
        break;
    }
}

static void v25ter_report_ma(Uint32 modulations)
{
    Uint32 rest;

    v25ter_put("+MA: ");
    rest = (modulations - v25terFindMaxBit(modulations)) & dp_mod_types &
           ~MOD_BELL;
    v25ter_put("(");
    while (v25ter_put_modulation(rest)) {
        rest -= v25terFindMaxBit(rest);
        if (rest) {
            v25ter_put(",");
        }
    }
    v25ter_put(")");
    v25ter_put_cr_lf();
}

static void v25ter_set_ma(void)
{
    Uint32 modulations = 0;
    Uint32 requested;
    Uint32 modulation;
    Uint8 field;

    do {
        field = read_string(v25ter_buffer, &v25ter_cursor, NAME_WIDTH);
        /* @hack spelling: !field, where field == AT_FIELD_VALUE would do */
        if (!field) {
            modulation = v25ter_modulation_by_name(v25ter_buffer);
            if (!modulation) {
                v25ter_result = V25TER_ERROR;
                break;
            }
            modulations |= modulation;
        }
    } while (field != AT_FIELD_MISSING);
    requested = modulations;
    v25terRemoveModNotAuthorised(&modulations, MS_MIN_TX, MS_MAX_TX, MS_MIN_RX,
                                 MS_MAX_RX);
    if (modulations >= v25terFindMaxBit(MS_MODULATIONS) ||
        requested != modulations) {
        v25ter_result = V25TER_ERROR;
    } else {
        SREG(60) = v25terFindMaxBit(MS_MODULATIONS) | modulations;
        SREG(61) =
            ((v25terFindMaxBit(MS_MODULATIONS) | modulations) & 0xff00) >> 8;
    }
}

static void v25ter_ms(void)
{
    switch (*v25ter_cursor++) {
    case '=':
        if (*v25ter_cursor == '?') {
            v25ter_cursor++;
            if (SREG(14) & S14_VERBOSE) {
                v25ter_put_cr_lf();
            }
            v25ter_put("+MS: ");
            v25ter_report_ms_range();
            v25ter_put_cr_lf();
        } else {
            v25ter_set_ms();
        }
        break;
    case '?':
        if (SREG(14) & S14_VERBOSE) {
            v25ter_put_cr_lf();
        }
        v25ter_put("+MS: ");
        v25ter_report_ms();
        v25ter_put_cr_lf();
        break;
    }
}

static void v25ter_report_ms_range(void)
{
    Uint32 rate;
    Uint32 tx_rate;

    v25ter_put_ms_modulations();
    v25ter_put(",(0,1),");
    rate = ConvertTCISpeedToBps(lestConvertDpRate(dp_max_rate));
    tx_rate = (dp_mod_types & (MOD_K56 | MOD_V90)) ? 33600 : rate;
    if (sprintf(v25ter_buffer, "(0,300-%d),(0,300-%d),(0,300-%d),(0,300-%d)",
                (int)tx_rate, (int)tx_rate, (int)rate,
                (int)rate) > BUFFER_LENGTH) {
        v25ter_result = V25TER_ERROR;
        return;
    }
    v25ter_put(v25ter_buffer);
}

static void v25ter_put_ms_modulations(void)
{
    v25ter_put("(");
    if (dp_mod_types & MOD_V21) {
        v25ter_put("V21");
    }
    if (dp_mod_types & MOD_V22) {
        v25ter_put(",V22");
    }
    if (dp_mod_types & MOD_V22BIS) {
        v25ter_put(",V22B");
    }
    if (dp_mod_types & MOD_V23) {
        v25ter_put(",V23C");
    }
    if (dp_mod_types & MOD_V32) {
        v25ter_put(",V32");
    }
    if (dp_mod_types & MOD_V32BIS) {
        v25ter_put(",V32B");
    }
    if (dp_mod_types & MOD_V34) {
        v25ter_put(",V34");
    }
    if (dp_mod_types & MOD_K56) {
        v25ter_put(",K56");
    }
    if (dp_mod_types & MOD_V90) {
        v25ter_put(",V90");
    }
    v25ter_put(")");
}

static void v25ter_report_ms(void)
{
    if (!v25ter_put_modulation(MS_MODULATIONS)) {
        v25ter_put("Unknown modulation");
    }
    v25ter_put(",");
    if (!(SREG(66) & S66_NO_AUTOMODE)) {
        v25ter_put("1,");
    } else {
        v25ter_put("0,");
    }
    v25ter_put_rate(MS_MIN_TX);
    v25ter_put(",");
    v25ter_put_rate(MS_MAX_TX);
    v25ter_put(",");
    v25ter_put_rate(MS_MIN_RX);
    v25ter_put(",");
    v25ter_put_rate(MS_MAX_RX);
}

static char v25ter_put_modulation(Uint32 modulations)
{
    if (modulations & MOD_V90) {
        v25ter_put("V90");
    } else if (modulations & MOD_K56) {
        v25ter_put("K56");
    } else if (modulations & MOD_V34) {
        v25ter_put("V34");
    } else if (modulations & MOD_V32BIS) {
        v25ter_put("V32B");
    } else if (modulations & MOD_V32) {
        v25ter_put("V32");
    } else if (modulations & MOD_V22BIS) {
        v25ter_put("V22B");
    } else if (modulations & MOD_V22) {
        v25ter_put("V22");
    } else if (modulations & MOD_V23) {
        v25ter_put("V23C");
    } else if (modulations & MOD_V21) {
        v25ter_put("V21");
    } else if (modulations & MOD_BELL) {
        v25ter_put("BELL");
    } else {
        return 0;
    }
    return 1;
}

static void v25ter_put_rate(Uint8 rate)
{
    char text[12];

    if (!rate) {
        v25ter_put("0");
    } else {
        sprintf(text, "%d", (int)ConvertTCISpeedToBps(lestConvertDpRate(rate)));
        v25ter_put(text);
    }
}

static void v25ter_set_ms(void)
{
    Uint32 modulations;
    Uint32 value;
    Uint8 automode;
    Uint32 requested;
    Uint8 min_tx;
    Uint8 max_tx;
    Uint8 min_rx;
    Uint8 max_rx;
    Uint8 field;

    field = read_string(v25ter_buffer, &v25ter_cursor, NAME_WIDTH);
    if (field == AT_FIELD_VALUE) {
        modulations = v25ter_modulation_by_name(v25ter_buffer) & dp_mod_types;
        if (modulations) {
            requested = modulations;
        } else {
            v25ter_result = V25TER_ERROR;
            return;
        }
    } else if (field == AT_FIELD_EMPTY) {
        modulations = v25terFindMaxBit(MS_MODULATIONS);
        requested = 0;
    } else if (field == AT_FIELD_MISSING) {
        v25ter_result = V25TER_ERROR;
        return;
    }
#ifdef NONMATCHING
    /* this side fails a field of any other kind, for which the matching side
     * goes on with modulations and requested unset */
    else {
        v25ter_result = V25TER_ERROR;
        return;
    }
#endif
    field = read_parameter_fv(&value, &v25ter_cursor, 'd');
    if (field == AT_FIELD_VALUE) {
        /* @hack spelling: the else after a return, where the statement after
         * the if would do */
        if (value > 1) {
            v25ter_result = V25TER_ERROR;
            return;
        } else {
            automode = value ? 1 : 0;
        }
    } else {
        /* @hack redundant: the ? 1 : 0 after the !, which gives 1 or 0
         * itself */
        automode = !(SREG(66) & S66_NO_AUTOMODE) ? 1 : 0;
    }
    field = read_parameter_fv(&value, &v25ter_cursor, 'n');
    if (field == AT_FIELD_VALUE) {
        min_tx = v25ter_rate_at_least(value);
        if (min_tx == NO_RATE) {
            v25ter_result = V25TER_ERROR;
            return;
        }
    } else {
        min_tx = MS_MIN_TX;
    }
    field = read_parameter_fv(&value, &v25ter_cursor, 'n');
    if (field == AT_FIELD_VALUE) {
        max_tx = v25ter_rate_at_most(value);
        if (max_tx == NO_RATE || (max_tx != 0 && min_tx > max_tx) ||
            max_tx > dp_max_rate) {
            v25ter_result = V25TER_ERROR;
            return;
        }
    } else {
        max_tx = MS_MAX_TX;
    }
    field = read_parameter_fv(&value, &v25ter_cursor, 'n');
    if (field == AT_FIELD_VALUE) {
        min_rx = v25ter_rate_at_least(value);
        if (min_rx == NO_RATE) {
            v25ter_result = V25TER_ERROR;
            return;
        }
    } else {
        min_rx = MS_MIN_RX;
    }
    field = read_parameter_fv(&value, &v25ter_cursor, 'n');
    if (field == AT_FIELD_VALUE) {
        max_rx = v25ter_rate_at_most(value);
        if (max_rx == NO_RATE || (max_rx != 0 && min_rx > max_rx) ||
            max_rx > dp_max_rate) {
            v25ter_result = V25TER_ERROR;
            return;
        }
    } else {
        max_rx = MS_MAX_RX;
    }
    /* modulations is one bit, the carrier. add each modulation below it that
     * the pump runs */
    modulations = (modulations + modulations - 1) & dp_mod_types;
    v25terRemoveModNotAuthorised(&modulations, min_tx, max_tx, min_rx, max_rx);
    if ((requested > 0 && !(modulations & requested)) || !modulations ||
        modulations == MOD_BELL || (max_tx < min_tx && max_tx != 0)) {
        v25ter_result = V25TER_ERROR;
    } else {
        SREG(60) = modulations;
        SREG(61) = (modulations & 0xff00) >> 8;
        if (!automode) {
            SREG(66) |= S66_NO_AUTOMODE;
        } else {
            SREG(66) &= ~S66_NO_AUTOMODE;
        }
        MS_MIN_TX = min_tx;
        MS_MAX_TX = max_tx;
        MS_MIN_RX = min_rx;
        MS_MAX_RX = max_rx;
    }
}

static Uint32 v25ter_modulation_by_name(char *name)
{
    if (!strncmp(name, "V21     ", NAME_WIDTH)) {
        return MOD_V21;
    }
    if (!strncmp(name, "V22     ", NAME_WIDTH)) {
        return MOD_V22;
    }
    if (!strncmp(name, "V22B    ", NAME_WIDTH)) {
        return MOD_V22BIS;
    }
    if (!strncmp(name, "V23C    ", NAME_WIDTH)) {
        return MOD_V23;
    }
    if (!strncmp(name, "V32     ", NAME_WIDTH)) {
        return MOD_V32;
    }
    if (!strncmp(name, "V32B    ", NAME_WIDTH)) {
        return MOD_V32BIS;
    }
    if (!strncmp(name, "V34     ", NAME_WIDTH)) {
        return MOD_V34;
    }
    if (!strncmp(name, "K56     ", NAME_WIDTH)) {
        return MOD_K56;
    }
    if (!strncmp(name, "V90     ", NAME_WIDTH)) {
        return MOD_V90;
    }
    return 0;
}

static Uint8 v25ter_rate_at_least(Uint32 bps)
{
    if (!bps) {
        return 0;
    }
    if (bps <= 75) {
        return 1;
    }
    if (bps <= 300) {
        return 2;
    }
    if (bps <= 600) {
        return 3;
    }
    if (bps <= 1200) {
        return 4;
    }
    if (bps <= 2400) {
        return 5;
    }
    if (bps <= 4800) {
        return 6;
    }
    if (bps <= 7200) {
        return 7;
    }
    if (bps <= 9600) {
        return 8;
    }
    if (bps <= 12000) {
        return 11;
    }
    if (bps <= 14400) {
        return 12;
    }
    if (bps <= 16800) {
        return 13;
    }
    if (bps <= 19200) {
        return 14;
    }
    if (bps <= 21600) {
        return 21;
    }
    if (bps <= 24000) {
        return 22;
    }
    if (bps <= 26400) {
        return 23;
    }
    if (bps <= 28000) {
        return 24;
    }
    if (bps <= 28800) {
        return 25;
    }
    if (bps <= 29333) {
        return 26;
    }
    if (bps <= 30667) {
        return 28;
    }
    if (bps <= 31200) {
        return 29;
    }
    if (bps <= 32000) {
        return 30;
    }
    if (bps <= 33333) {
        return 31;
    }
    if (bps <= 33600) {
        return 32;
    }
    if (bps <= 34000) {
        return 33;
    }
    if (bps <= 34667) {
        return 34;
    }
    if (bps <= 36000) {
        return 35;
    }
    if (bps <= 37333) {
        return 36;
    }
    if (bps <= 38000) {
        return 37;
    }
    if (bps <= 38667) {
        return 39;
    }
    if (bps <= 40000) {
        return 40;
    }
    if (bps <= 41333) {
        return 41;
    }
    if (bps <= 42000) {
        return 42;
    }
    if (bps <= 42667) {
        return 43;
    }
    if (bps <= 44000) {
        return 44;
    }
    if (bps <= 45333) {
        return 45;
    }
    if (bps <= 46000) {
        return 46;
    }
    if (bps <= 46667) {
        return 47;
    }
    if (bps <= 48000) {
        return 48;
    }
    if (bps <= 49333) {
        return 49;
    }
    if (bps <= 50000) {
        return 50;
    }
    if (bps <= 50667) {
        return 51;
    }
    if (bps <= 52000) {
        return 52;
    }
    if (bps <= 53333) {
        return 53;
    }
    if (bps <= 54000) {
        return 54;
    }
    if (bps <= 54667) {
        return 55;
    }
    if (bps <= 56000) {
        return 56;
    }
    return NO_RATE;
}

static Uint8 v25ter_rate_at_most(Uint32 bps)
{
    if (!bps) {
        return 0;
    }
    /* @bug logic: a maximum above 56000 bit/s takes 54667's code, 55. the
     * test at 0x0caf6ac2 holds for 56000 alone */
    if (bps == 56000) {
        return 56;
    }
    if (bps >= 54666) {
        return 55;
    }
    if (bps >= 54000) {
        return 54;
    }
    if (bps >= 53333) {
        return 53;
    }
    if (bps >= 52000) {
        return 52;
    }
    if (bps >= 50666) {
        return 51;
    }
    if (bps >= 50000) {
        return 50;
    }
    if (bps >= 49333) {
        return 49;
    }
    if (bps >= 48000) {
        return 48;
    }
    if (bps >= 46666) {
        return 47;
    }
    if (bps >= 46000) {
        return 46;
    }
    if (bps >= 45333) {
        return 45;
    }
    if (bps >= 44000) {
        return 44;
    }
    if (bps >= 42666) {
        return 43;
    }
    if (bps >= 42000) {
        return 42;
    }
    if (bps >= 41333) {
        return 41;
    }
    if (bps >= 40000) {
        return 40;
    }
    if (bps >= 38666) {
        return 39;
    }
    if (bps >= 38000) {
        return 37;
    }
    if (bps >= 37333) {
        return 36;
    }
    if (bps >= 36000) {
        return 35;
    }
    if (bps >= 34666) {
        return 34;
    }
    if (bps >= 34000) {
        return 33;
    }
    if (bps >= 33600) {
        return 32;
    }
    if (bps >= 33333) {
        return 31;
    }
    if (bps >= 32000) {
        return 30;
    }
    if (bps >= 31200) {
        return 29;
    }
    if (bps >= 30666) {
        return 28;
    }
    if (bps >= 29333) {
        return 26;
    }
    if (bps >= 28800) {
        return 25;
    }
    if (bps >= 28000) {
        return 24;
    }
    if (bps >= 26400) {
        return 23;
    }
    if (bps >= 24000) {
        return 22;
    }
    if (bps >= 21600) {
        return 21;
    }
    if (bps >= 19200) {
        return 14;
    }
    if (bps >= 16800) {
        return 13;
    }
    if (bps >= 14400) {
        return 12;
    }
    if (bps >= 12000) {
        return 11;
    }
    if (bps >= 9600) {
        return 8;
    }
    if (bps >= 7200) {
        return 7;
    }
    if (bps >= 4800) {
        return 6;
    }
    if (bps >= 2400) {
        return 5;
    }
    if (bps >= 1200) {
        return 4;
    }
    if (bps >= 600) {
        return 3;
    }
    if (bps >= 300) {
        return 2;
    }
    if (bps >= 75) {
        return 1;
    }
    return NO_RATE;
}

static char v25ter_start_message(char direct)
{
    v25ter_message.direct = direct;
    /* @hack spelling: the else after a return, where the statements after
     * the if would do */
    if (direct) {
        v25ter_message.used = 0;
        v25ter_message.room = vxd_dte_set_tx_data(&v25ter_message.write);
        if (!v25ter_message.room) {
            return 0;
        }
        return 1;
    } else {
        set_dte_for_message();
        return 1;
    }
}

static char v25ter_put(char *text)
{
    /* @hack spelling: the else after a return, where the statements after
     * the if would do */
    if (v25ter_message.direct) {
        while (*text) {
            if (v25ter_message.used < v25ter_message.room) {
                *v25ter_message.write++ = *text++;
                v25ter_message.used++;
            } else {
                v25ter_flush();
                if (!v25ter_start_message(1)) {
                    return 0;
                }
            }
        }
        return 1;
    } else {
        send_message(text);
        return 1;
    }
}

static void v25ter_put_cr_lf(void)
{
    v25ter_cr_lf[0] = SREG(3);
    v25ter_cr_lf[1] = S4_MASK;
    v25ter_cr_lf[2] = 0;
    v25ter_put(v25ter_cr_lf);
}

static void v25ter_flush(void)
{
    if (v25ter_message.direct) {
        vxd_dte_tx_data_done(v25ter_message.used);
    }
}

/* the image holds +GCAP's reply and the manufacturer, model and revision,
 * though no command here reports them. SHC writes a string literal into the
 * object's constants even where only sizeof reads it. the enum takes their
 * sizes for that alone */
enum {
    CAPABILITIES_LENGTH = sizeof("+GCAP : +FCLASS, +MS, +ES, +DS"),
    MANUFACTURER_LENGTH = sizeof("Sega"),
    MODEL_LENGTH = sizeof("BuiltIn Modem"),
    REVISION_LENGTH = sizeof("REV 1.000")
};
