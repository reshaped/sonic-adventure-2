#include <sg_xpt.h>
#include "atcom.h"
#include "command.h"
#include "dte_vxd.h"
#include "ecdc_def.h"
#include "ecdcutil.h"
#include "maintain.h"
#include "messages.h"
#include "modemdef.h"
#include "tci_ecdc.h"
#include "tci_modm.h"
#include "util_mod.h"

#ifdef NONMATCHING
/* this side reads S49 as SREG(49), the plain form the hack tag below gives.
 * tci_s49_flags is a name only symbols.txt places */
#define S49_FLAGS (*(volatile Uint8 *)&SREG(49))
#else
/* @hack type: S49 read through tci_s49_flags, Sregisters[49] by another name,
 * where SREG(49) would do */
extern Uint8 tci_s49_flags;
#define S49_FLAGS (*(volatile Uint8 *)&tci_s49_flags)
#endif

/* tci_line_rates' entries for a speed with no rate bits of its own. each
 * takes DEFAULT_RATE. RATE_EXTENDED's speed codes take S39_SPEED's bits */
#define RATE_NONE 255
#define RATE_EXTENDED 254
#define DEFAULT_RATE 6
/* in setup_timer's 10 ms ticks */
#define ECHO_TICKS 13

/* the letters of a TIES command, A to Z */
#define TIES_ALPHABET 26
/* the highest S-register a TIES command names */
#define TIES_LAST_S_REGISTER 51

/* ties_phase's states, but for 0, under which TIES_validation passes every
 * byte */
enum ties_state {
    TIES_COMMAND = 1,
    TIES_LETTER,
    TIES_ARGUMENT,
    TIES_REGISTER,
    TIES_VALUE,
    TIES_PLUS,
    TIES_F
};

/* S23's rate bits for each of the host's speed codes, as TCI_got_format
 * reads them */
static const Uint8 tci_line_rates[60] = {
#include "modem/tci_modm/tci_line_rates.inc"
};

/* each letter's limit, with no prefix in ties_base and after the prefix each
 * other table names. allowed marks a command, and maximum is the largest
 * number it takes, or 0 for none. ties_base's S takes a register's number
 * instead */
static const struct ties_command_limit ties_base[TIES_ALPHABET] = {
#include "modem/tci_modm/ties_base.inc"
};
static const struct ties_command_limit ties_amp[TIES_ALPHABET] = {
#include "modem/tci_modm/ties_amp.inc"
};
static const struct ties_command_limit ties_percent[TIES_ALPHABET] = {
#include "modem/tci_modm/ties_percent.inc"
};
static const struct ties_command_limit ties_slash[TIES_ALPHABET] = {
#include "modem/tci_modm/ties_slash.inc"
};
static const struct ties_command_limit ties_star[TIES_ALPHABET] = {
#include "modem/tci_modm/ties_star.inc"
};
static const struct ties_command_limit ties_at[TIES_ALPHABET] = {
#include "modem/tci_modm/ties_at.inc"
};

void TCI_got_format(Uint8 speed, Uint8 format)
{
    Uint8 rate;
    format_of_AT = format;
    speed_of_AT = speed;
    SREG(20) = speed;
#ifdef NONMATCHING
    /* this side gives RATE_NONE for a speed past tci_line_rates */
    rate = speed < sizeof(tci_line_rates) ? tci_line_rates[speed] : RATE_NONE;
#else
    rate = tci_line_rates[speed];
#endif
    /* @hack spelling: the masks below as 0xf7, 0xfb and 0xf3, where
     * (Uint8)~ of S39_SPEED or its bits would do */
    if (rate == RATE_NONE) {
        rate = DEFAULT_RATE;
        SREG(39) &= ~S39_SPEED;
    } else if (rate == RATE_EXTENDED) {
        rate = DEFAULT_RATE;
        switch (speed) {
        case TCI_SPEED_115200:
            SREG(39) = (SREG(39) & 0xf7) | S39_SPEED_115200;
            break;
        case TCI_SPEED_230400:
            SREG(39) = (SREG(39) & 0xfb) | S39_SPEED_230400;
            break;
        case TCI_SPEED_460800:
            SREG(39) |= S39_SPEED_460800;
            break;
        default:
            SREG(39) &= 0xf3;
            break;
        }
    } else {
        SREG(39) &= 0xf3;
    }
    rate |= SREG(23) & 0xc1;
    if (format == FORMAT_ODD_PARITY) {
        rate |= S23_ODD_PARITY;
    } else if (format == FORMAT_MARK_PARITY) {
        rate |= S23_MARK_PARITY;
    } else if (format != FORMAT_EVEN_PARITY) {
        rate |= S23_SPACE_PARITY;
    }
    SREG(23) = rate;
}

void TCI_dte_rx_cmd_byte(Uint8 byte)
{
    Uint8 received;
    char echo;
    int count;
    Uint8 *terminator;
    Uint16 *position;

    received = byte;
    if ((SREG(23) & S23_PARITY) != S23_SPACE_PARITY || CMD_B_PT < 2) {
        byte &= SEVEN_BITS;
    }
    if (SREG(14) & 2) {
        if (!(BMR(32) & 0x40) || !(BMR(31) & 2)) {
            echo = 1;
        } else {
            echo = 0;
        }
    } else {
        echo = 0;
    }
    if (byte != S4_MASK) {
        BMR(7) |= BMR7_HOST_BYTE;
    }
#ifdef NONMATCHING
#else
    /* @hack redundant: a call of message_byte_to_dte that never runs */
    if (0) {
        message_byte_to_dte(byte);
    }
#endif
    if (BMR(7) & BMR7_COMMAND_LINE) {
        if (!timer_expired(CMD_ECHO_TIM)) {
            message_byte_to_dte(byte);
        }
        return;
    }
    if (!(BMR(22) & 0x80)) {
        return;
    }
    if (BMR(6) & 1) {
        return;
    }
    if (S49_FLAGS & 0x40) {
        return;
    }
    if ((BMR(6) & BMR6_COMMAND) && !TIES_validation(byte)) {
        return;
    }
    BMR(21) &= 0xdf;
    position = &CMD_B_PT;
    if (!(SREG(31) & 0x80) && CMD_B_PT < 2 && byte != 'a' && byte != 'A') {
        if (CMD_B_PT == 0 || (byte != 't' && byte != 'T' && byte != '/')) {
            *position = 0;
            return;
        }
    }
    if (!(SREG(31) & 0x10) && *position == 0 && !vxd_dte_tx_buff_empty(0)) {
        return;
    }
    count = *position;
    if (byte == SREG(5)) {
        if (BMR(32) & 0x40) {
            if (count == 0) {
                return;
            }
        } else if (count <= 2) {
            return;
        }
        --*position;
        if (echo) {
            message_byte_to_dte(SREG(5));
            message_byte_to_dte(' ');
            message_byte_to_dte(SREG(5));
        }
        return;
    }
    terminator = &S3_THEN;
    if (count >= MODEM_COMMAND_LENGTH - 1) {
        if (byte != *terminator) {
            byte = *terminator;
            *position += 2;
        }
    }
    if (byte == *terminator) {
        *terminator = SREG(3);
        BMR(20) &= 0xf7;
        if (BMR(32) & 0x40) {
            BMR(32) |= 0x80;
        } else {
            BMR(22) &= 0x7f;
            BMR(7) |= BMR7_COMMAND_LINE;
            BMR(6) |= BMR6_RUNNING_COMMAND;
            if (echo) {
                setup_timer(&CMD_ECHO_TIM, ECHO_TICKS);
            }
        }
    }
    count = *position;
    if (count > MODEM_COMMAND_LENGTH) {
        return;
    }
    if (!(BMR(31) & 2) && !(BMR(20) & BMR20_LOGON) && echo &&
        !(BMR(6) & BMR6_DATA)) {
        if (count < 2) {
            vxd_dte_tx_cmd_byte(received);
        } else {
            message_byte_to_dte(byte);
        }
    }
    if (byte >= 'a' && byte <= 'z' && byte != S3_THEN) {
        byte -= 'a' - 'A';
    }
    if (!(BMR(32) & 0x40) && CMD_B_PT == 1) {
        if (byte == 'A') {
            return;
        }
        if (byte == '/') {
            BMR(22) &= 0x7f;
            BMR(7) |= BMR7_COMMAND_LINE;
            BMR(6) |= BMR6_RUNNING_COMMAND;
            if (echo) {
                setup_timer(&CMD_ECHO_TIM, ECHO_TICKS);
            }
        }
    }
    CMD_BUF[*position] = byte;
    ++*position;
}

#ifdef NONMATCHING
/* this side repairs the bug tagged on the matching side: its parser keeps
 * ties_cmd_ptr within ties_cmd_buff. it also fails a number past 255 and a
 * register past the range tables, where the matching side trusts the
 * terminal's bytes */
/* drop ties_cmd_buff's last byte for S5's backspace. the parser then runs
 * again from the start of the command the byte was in */
static void undo_escape_byte(void)
{
    Uint8 i;

    --ties_cmd_ptr;
    if (ties_cmd_ptr == 2) {
        ties_phase = TIES_COMMAND;
        return;
    }
    i = ties_cmd_ptr - 1;
    while (i > 1 && ((ties_cmd_buff[i] >= '0' && ties_cmd_buff[i] <= '9') ||
                     ties_cmd_buff[i] == '=')) {
        --i;
    }
    if (i > 1 && get_cmd_prefix(ties_cmd_buff[i - 1])) {
        --i;
    }
    ties_phase = TIES_COMMAND;
    while (i < ties_cmd_ptr) {
        ties_analysis(ties_cmd_buff[i++]);
    }
}

char TIES_validation(Uint8 byte)
{
    if (!ties_phase) {
        return 1;
    }
    if ((SREG(23) & S23_PARITY) != S23_SPACE_PARITY) {
        byte &= SEVEN_BITS;
    }
    if (byte >= 'a' && byte <= 'z' && byte != S3_THEN && byte != SREG(5)) {
        byte -= 'a' - 'A';
    }
    if (byte == SREG(5)) {
        if (ties_cmd_ptr < 2 || ties_cmd_ptr > sizeof(ties_cmd_buff)) {
            goto invalid;
        }
        if (ties_cmd_ptr > 2) {
            undo_escape_byte();
        }
        return 1;
    }
    if (byte == ' ' || byte == ',' || byte > '~' ||
        (byte < ' ' && byte != '\r')) {
        return 1;
    }
    if (byte == '\r' && S3_THEN != byte) {
        return 1;
    }
    if (ties_cmd_ptr >= sizeof(ties_cmd_buff)) {
        goto invalid;
    }
    ties_cmd_buff[ties_cmd_ptr++] = byte;
    if (ties_cmd_ptr == 1) {
        if (byte != 'A') {
            goto invalid;
        }
    } else if (ties_cmd_ptr == 2) {
        if (byte == 'A') {
            --ties_cmd_ptr;
        } else if (byte != 'T') {
            goto invalid;
        }
    } else if (!ties_analysis(byte)) {
        goto invalid;
    } else if (byte == S3_THEN) {
        Uint8 i;
        ties_phase = 0;
        BMR(21) |= 0x20;
        CMD_B_PT = 0;
        for (i = 0; i < ties_cmd_ptr; i++) {
            CMD_BUF[CMD_B_PT++] = ties_cmd_buff[i];
        }
        ties_cmd_ptr = 0;
    }
    return 1;

invalid:
    restart_TIES();
    if (BMRregisters[6] & BMR6_COMMAND) {
        force_o_cmd();
    }
    return 0;
}

static char escape_letter(Uint8 value)
{
    const struct ties_command_limit *limit;

    if (value < 'A' || value > 'Z') {
        return 0;
    }
    limit = &ties_cmd_tbl_ptr[value - 'A'];
    if (!limit->allowed) {
        return 0;
    }
    ties_upper_range = limit->maximum;
    if (ties_upper_range) {
        ties_phase = TIES_ARGUMENT;
    } else if (ties_cmd_tbl_ptr == ties_base && value == 'S') {
        ties_phase = TIES_REGISTER;
    }
    return 1;
}

static char escape_command(Uint8 value)
{
    if (value == S3_THEN || value == '?' || value == '!') {
        return 1;
    }
    if (value == '=') {
        ties_s_reg = last_s_used;
        ties_phase = TIES_VALUE;
        return 1;
    }
    return get_cmd_prefix(value) || escape_letter(value);
}

char ties_analysis(Uint8 value)
{
    Uint8 number;
    char complete;

    switch (ties_phase) {
    case TIES_COMMAND:
        return escape_command(value);
    case TIES_LETTER:
        return escape_letter(value);
    case TIES_PLUS:
        if (value != 'F') {
            return 0;
        }
        ties_phase = TIES_F;
        return 1;
    case TIES_ARGUMENT:
    case TIES_REGISTER:
    case TIES_VALUE:
        break;
    default:
        return 1;
    }

    complete = get_cmd_argument(value, &number);
    if (complete != 1) {
        return complete == 0;
    }
    switch (ties_phase) {
    case TIES_ARGUMENT:
        if (number > ties_upper_range) {
            return 0;
        }
        ties_phase = TIES_LETTER;
        break;
    case TIES_REGISTER:
        if (value == '=') {
            if (number > TIES_LAST_S_REGISTER) {
                return 0;
            }
            ties_s_reg = number;
            ties_phase = TIES_VALUE;
            return 1;
        }
        break;
    case TIES_VALUE:
        if (ties_s_reg >= MODEM_S_REGISTERS ||
            number < S_range_min[ties_s_reg] ||
            number > S_range_max[ties_s_reg]) {
            return 0;
        }
        break;
    }
    ties_cmd_tbl_ptr = ties_base;
    return escape_command(value);
}

char get_cmd_prefix(Uint8 value)
{
    switch (value) {
    case '&':
        ties_cmd_tbl_ptr = ties_amp;
        break;
    case '%':
        ties_cmd_tbl_ptr = ties_percent;
        break;
    case '*':
        ties_cmd_tbl_ptr = ties_star;
        break;
    case '\\':
        ties_cmd_tbl_ptr = ties_slash;
        break;
    case '@':
        ties_cmd_tbl_ptr = ties_at;
        break;
    case '+':
        ties_phase = TIES_PLUS;
        break;
    default:
        return 0;
    }
    return 1;
}

char get_cmd_argument(Uint8 value, Uint8 *number)
{
    unsigned int position;
    unsigned int end;
    unsigned int parsed = 0;

    if (value >= '0' && value <= '9') {
        return 0;
    }
    if (ties_cmd_ptr < 2 || ties_cmd_ptr > sizeof(ties_cmd_buff)) {
        return -1;
    }
    end = ties_cmd_ptr - 1;
    position = end;
    while (position && ties_cmd_buff[position - 1] >= '0' &&
           ties_cmd_buff[position - 1] <= '9') {
        --position;
    }
    while (position < end) {
        parsed = (parsed * 10) + (ties_cmd_buff[position++] - '0');
        if (parsed > 255) {
            return -1;
        }
    }
    *number = (Uint8)parsed;
    return 1;
}

void restart_TIES(void)
{
    ties_phase = TIES_COMMAND;
    ties_cmd_ptr = 0;
    ties_cmd_tbl_ptr = ties_base;
}
#else
char TIES_validation(Uint8 byte)
{
    Uint8 i;

    if (!ties_phase) {
        return 1;
    }
    if ((SREG(23) & S23_PARITY) != S23_SPACE_PARITY) {
        byte &= SEVEN_BITS;
    }
    if (byte >= 'a' && byte <= 'z' && byte != S3_THEN && byte != SREG(5)) {
        byte -= 'a' - 'A';
    }
    if (byte == SREG(5)) {
        /* @hack spelling: an else after the goto and after each return, here
         * and below, where the statements after each if would do */
        if (ties_cmd_ptr < 2) {
            goto invalid;
        } else if (ties_cmd_ptr == 2) {
            return 1;
        } else {
            --ties_cmd_ptr;
            if (ties_cmd_ptr == 2) {
                ties_phase = TIES_COMMAND;
            } else {
                i = ties_cmd_ptr - 1;
                while ((ties_cmd_buff[i] >= '0' && ties_cmd_buff[i] <= '9') ||
                       ties_cmd_buff[i] == '=') {
                    --i;
                }
                if (get_cmd_prefix(ties_cmd_buff[i - 1])) {
                    --i;
                }
                ties_phase = TIES_COMMAND;
                while (i != ties_cmd_ptr) {
                    ties_analysis(ties_cmd_buff[i++]);
                }
            }
            return 1;
        }
    }
    if (byte == ' ' || byte == ',' || byte > '~' ||
        (byte < ' ' && byte != '\r')) {
        return 1;
    } else {
        /* @hack spelling: S3_THEN on the comparison's left, here and below,
         * where byte would do */
        if (byte == '\r' && S3_THEN != byte) {
            return 1;
        }
        /* @bug memory: stores the 42nd byte of a command a byte past
         * ties_cmd_buff, at 0x0caea3d6, since the test for a full buffer
         * follows the store. the byte lands on ties_cmd_ptr */
        ties_cmd_buff[ties_cmd_ptr++] = byte;
        /* @hack type: the buffer's size as an int, where sizeof alone would
         * do */
        if (ties_cmd_ptr > (int)sizeof(ties_cmd_buff)) {
            goto invalid;
        }
        if (ties_cmd_ptr == 1) {
            /* @hack spelling: the command's first two bytes read back from
             * ties_cmd_buff, where byte holds each */
            if (ties_cmd_buff[0] != 'A') {
                goto invalid;
            }
            return 1;
        }
        if (ties_cmd_ptr == 2) {
            if (ties_cmd_buff[1] == 'T') {
                return 1;
            }
            if (ties_cmd_buff[1] != 'A') {
                goto invalid;
            }
            --ties_cmd_ptr;
            return 1;
        }
        if (!ties_analysis(byte)) {
            goto invalid;
        } else if (S3_THEN == byte) {
            ties_phase = 0;
            BMR(21) |= 0x20;
            CMD_B_PT = 0;
            /* the count leaves ties_cmd_ptr 255. nothing reads it while
             * ties_phase is 0, and restart_TIES, which alone ends that,
             * clears it */
            while (ties_cmd_ptr--) {
                CMD_BUF[CMD_B_PT] = ties_cmd_buff[CMD_B_PT];
                ++CMD_B_PT;
            }
        }
        return 1;
    }
invalid:
    restart_TIES();
    if (BMRregisters[6] & BMR6_COMMAND) {
        force_o_cmd();
    }
    return 0;
}

char ties_analysis(Uint8 value)
{
    Uint8 number;
    int byte_offset;
    const struct ties_command_limit *limit;

    switch (ties_phase) {
    case TIES_COMMAND:
    command:
        /* @hack spelling: S3_THEN on the comparison's left, and ! in a test
         * of its own, where one test of value against the three would do */
        if (S3_THEN == value || value == '?') {
            break;
        }
        if (value == '!') {
            break;
        }
        if (value == '=') {
            ties_s_reg = last_s_used;
            ties_phase = TIES_VALUE;
            break;
        }
        if (get_cmd_prefix(value)) {
            break;
        }
        /* fall through */
    case TIES_LETTER:
        if (value < 'A' || value > 'Z') {
            goto invalid;
        }
        /* @hack spelling: the letter's limit found by its offset in bytes,
         * narrowed to a Uint8, and S told by that offset, where
         * &ties_cmd_tbl_ptr[value - 'A'] and value == 'S' would do */
        byte_offset = (value - 'A') * sizeof(*limit);
        limit = (const struct ties_command_limit *)((const Uint8 *)
                                                        ties_cmd_tbl_ptr +
                                                    (Uint8)byte_offset);
        if (!limit->allowed) {
            goto invalid;
        }
        ties_upper_range = limit->maximum;
        /* @hack spelling: the test for no number first, and ties_base on its
         * comparison's left, where a test of ties_upper_range alone, then
         * an else if, would do */
        if (ties_upper_range == 0) {
            if (ties_base == ties_cmd_tbl_ptr &&
                (Uint8)byte_offset == ('S' - 'A') * (int)sizeof(*limit)) {
                ties_phase = TIES_REGISTER;
            }
        } else {
            ties_phase = TIES_ARGUMENT;
        }
        break;
    case TIES_ARGUMENT:
        if (get_cmd_argument(value, &number)) {
            /* @hack spelling: the else after a goto, where the statements
             * after the if would do */
            if (number > ties_upper_range) {
                goto invalid;
            } else {
                ties_cmd_tbl_ptr = ties_base;
                ties_phase = TIES_LETTER;
                goto command;
            }
        }
        break;
    case TIES_REGISTER:
        if (get_cmd_argument(value, &number)) {
            if (value == '=') {
                if (number > TIES_LAST_S_REGISTER) {
                    goto invalid;
                }
                ties_s_reg = number;
                ties_phase = TIES_VALUE;
            } else {
                ties_cmd_tbl_ptr = ties_base;
                goto command;
            }
        }
        break;
    case TIES_VALUE:
        if (get_cmd_argument(value, &number)) {
            if (number < S_range_min[ties_s_reg] ||
                number > S_range_max[ties_s_reg]) {
                goto invalid;
            }
            ties_cmd_tbl_ptr = ties_base;
            goto command;
        }
        break;
    case TIES_PLUS:
        /* @hack spelling: the goto in an else, where a test for any other
         * byte ahead of the store would do */
        if (value == 'F') {
            ties_phase = TIES_F;
        } else {
            goto invalid;
        }
        break;
    /* @hack redundant: the cases that fall to the default, 8 among them,
     * a state nothing sets */
    case TIES_F:
    case 8:
    default:
        break;
    }
    /* @hack spelling: a goto over the failing return, where returning 1
     * here would do */
    goto valid;
invalid:
    return 0;
valid:
    return 1;
}

char get_cmd_prefix(Uint8 value)
{
    switch (value) {
    case '&':
        ties_cmd_tbl_ptr = ties_amp;
        break;
    case '%':
        ties_cmd_tbl_ptr = ties_percent;
        break;
    case '*':
        ties_cmd_tbl_ptr = ties_star;
        break;
    case '\\':
        ties_cmd_tbl_ptr = ties_slash;
        break;
    case '@':
        ties_cmd_tbl_ptr = ties_at;
        break;
    case '+':
        ties_phase = TIES_PLUS;
        break;
    default:
        return 0;
    }
    return 1;
}

char get_cmd_argument(Uint8 value, Uint8 *number)
{
    Uint8 i;
    Uint8 result;

    if (value >= '0' && value <= '9') {
        return 0;
    }
    i = ties_cmd_ptr - 2;
    while (ties_cmd_buff[i] >= '0' && ties_cmd_buff[i] <= '9') {
        --i;
    }
    do {
        ++i;
    } while (ties_cmd_buff[i] == '0');
    result = 0;
    while (ties_cmd_buff[i] >= '0' && ties_cmd_buff[i] <= '9') {
        result = (result * 10) + ties_cmd_buff[i] - '0';
        ++i;
    }
    *number = result;
    return 1;
}

void restart_TIES(void)
{
    ties_phase = TIES_COMMAND;
    ties_cmd_ptr = 0;
    ties_cmd_tbl_ptr = ties_base;
}
#endif

void TCI_timer(void)
{
    if (timer_at_4k8) {
        ++TIM_208US;
        if (++prsc_100 != 10) {
            return;
        }
        prsc_100 = 0;
    }
    if (BMR(16) & 0x10) {
        if (!(BMR(16) & 0x20)) {
            if (vxd_dte_tx_buff_empty(0)) {
                BMR(16) |= 0x20;
                setup_timer(&dcd_timer, SREG(9) * 10);
            }
        } else if (timer_expired(dcd_timer)) {
            vxd_dte_set_dcd(0);
            if (SREG(21) & 2) {
                vxd_dte_set_dsr(0);
            }
            BMR(16) &= 0xce;
        }
    }
    TCI_timer_10ms();
}

void TCI_tx_response(void)
{
    if (cmd_response_buffer[0] == ECDC_RESPONSE_ESCAPE) {
        BMR(14) |= 0x10;
    }
    if (cmd_response_buffer[0] == ECDC_RESPONSE_CONNECTED) {
        if (maintain_phase == MAINTAIN_ONLINE) {
            cmd_STOP_DATA();
            modem_connect();
        }
    } else if (cmd_response_buffer[0] == ECDC_RESPONSE_RESET) {
        if (maintain_phase == MAINTAIN_ONLINE && !carrier_off) {
            BMR(7) |= BMR7_LINK_LOST;
        }
    } else if (cmd_response_buffer[0] == ECDC_RESPONSE_BAD_CODEWORD) {
        SREG(45) = ECDC_RESPONSE_BAD_CODEWORD;
        BMR(7) |= BMR7_LINK_LOST;
    }
}
