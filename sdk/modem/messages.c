#include <sg_xpt.h>
#include <machine.h>
#include <stdio.h>
#include <string.h>

#include "cmi.h"
#include "dte_vxd.h"
#include "ecdc_def.h"
#include "messages.h"
#include "modemdef.h"
#include "roc_k56.h"
#include "util_mod.h"

/* set while send_coded_message sends a result */
#define BMR8_SENDING_RESULT 0x01

/* every text in one record. a text's field holds it, its nul, and zeros to
 * the next four-byte boundary */
struct result_strings {
#define AT_TEXT(name, width, text) char name[width];
#include "message_text.inc"
#undef AT_TEXT
};
static const struct result_strings result_text;
const char *const CodedMessages[134] = {
#define AT_TEXT(name, width, text) result_text.name,
#include "message_text.inc"
#undef AT_TEXT
};

static const Uint32 tci_speed_bps[60] = {
    75,     300,    1200,   2400,   4800,   7200,  9600,   12000,  14400,
    16800,  19200,  21600,  24000,  26400,  28000, 28800,  29333,  30666,
    31200,  32000,  33333,  33600,  34000,  34666, 36000,  37333,  38000,
    38400,  38666,  40000,  41333,  42000,  42666, 44000,  45333,  46000,
    46666,  48000,  49333,  50000,  50666,  52000, 53333,  54000,  54666,
    56000,  57600,  64000,  72000,  76800,  96000, 115200, 128000, 230400,
    460800, 460800, 691200, 691200, 691200, 921600};
static const struct result_strings result_text = {
#define AT_TEXT(name, width, text) text,
#include "message_text.inc"
#undef AT_TEXT
};
static const char bps_format[4] = "%d";
static const char bps_suffix[] = " BPS";

static void append_reliable_suffix(char **cursor);

void send_connect_message(void)
{
    Uint8 code;
    char text[60];
    char number[12];
    char *cursor;
    const char *source;
    Uint8 speed;

    if (!(SS22 & SS22_EXTENDED)) {
        send_coded_message(RESULT_CONNECT, NULL);
        return;
    }
    switch (speed_of_dce) {
    case 0:
        if (SREG(14) & S14_ORIGINATE) {
            code = RESULT_CONNECT_75TX_1200RX;
        } else {
            code = RESULT_CONNECT_1200TX_75RX;
        }
        break;
    case 1:
        code = RESULT_CONNECT;
        break;
#define LINE_RESULT(rate, connection, carrier)                                 \
    case rate:                                                                 \
        code = connection;                                                     \
        break;
#include "message_rates.inc"
#undef LINE_RESULT
    default:
        code = RESULT_CONNECT;
    }
    if ((BMR(32) & 0x40) || (BMR(10) & BMR10_DIRECT_MODE)) {
        send_coded_message(code, NULL);
        return;
    }
    cursor = text;
    if (!(SREG(24) & S24_W2)) {
        if (SREG(24) & S24_W1) {
            if (BMR(27) & BMR27_LAPM) {
                source = CodedMessages[RESULT_PROTOCOL_V42];
                while (*source != 0) {
                    *cursor++ = *source++;
                }
                add_bis_secur(&cursor);
                *cursor = 0;
                send_coded_message(RESULT_PROTOCOL_V42, text);
            } else if (BMR(29) & BMR29_MNP) {
                source = CodedMessages[RESULT_PROTOCOL_MNP_REL];
                while (*source != 0) {
                    *cursor++ = *source++;
                }
                append_reliable_suffix(&cursor);
                *cursor = 0;
                send_coded_message(RESULT_PROTOCOL_MNP_REL, text);
            } else {
                send_coded_message(RESULT_PROTOCOL_NONE, NULL);
            }
        }
        send_coded_message(get_dte_spd_mess(), NULL);
    } else {
        source = CodedMessages[code];
        while (*source != 0) {
            *cursor++ = *source++;
        }
        if (SREG(24) & S24_W1) {
            speed = dp_get_tx_bit_rate();
            if (speed) {
                *cursor++ = '/';
                sprintf(number, bps_format, (int)ConvertTCISpeedToBps(speed));
                _builtin_strcpy(cursor, number);
                cursor += strlen(number);
            }
        }
        if ((BMR(27) & BMR27_LAPM) || (BMR(29) & BMR29_MNP)) {
            *cursor++ = '/';
            if (BMR(27) & BMR27_LAPM) {
                *cursor++ = 'V';
                *cursor++ = '4';
                *cursor++ = '2';
                add_bis_secur(&cursor);
            } else {
                *cursor++ = 'R';
                *cursor++ = 'E';
                *cursor++ = 'L';
                *cursor++ = ' ';
                append_reliable_suffix(&cursor);
            }
        }
        *cursor = 0;
        send_coded_message(code, text);
    }
}

void add_bis_secur(char **cursor)
{
    if (BMR(29) & BMR29_COMPRESSION) {
        *(*cursor)++ = 'B';
        *(*cursor)++ = 'I';
        *(*cursor)++ = 'S';
    }
}

static void append_reliable_suffix(char **cursor)
{
    if (!(BMR(29) & BMR29_V42BIS)) {
        /* S44 is the MNP class, 2 to 5 */
        *(*cursor)++ = ' ';
        *(*cursor)++ = SREG(44) + '0';
    }
    if (BMR(29) & BMR29_V42BIS) {
        *(*cursor)++ = ' ';
        *(*cursor)++ = '-';
        *(*cursor)++ = ' ';
        *(*cursor)++ = 'V';
        *(*cursor)++ = '4';
        *(*cursor)++ = '2';
        *(*cursor)++ = 'B';
        *(*cursor)++ = 'I';
        *(*cursor)++ = 'S';
    }
}

Uint8 get_dte_spd_mess(void)
{
    Uint8 speed;
    if ((SREG(24) & S24_FIXED_DTE) && !(BMR(10) & BMR10_DIRECT_MODE)) {
        speed = speed_of_AT;
    } else {
        speed = dte_speed_restriction(speed_of_dce);
    }
    switch (speed) {
    case 1:
        break;
/* 38400 bit/s is a rate of the terminal's alone, which message_rates.inc
 * places among the line's */
#define TERMINAL_38400                                                         \
    case 27:                                                                   \
        return RESULT_CONNECT_38400;
#define LINE_RESULT(rate, connection, carrier)                                 \
    case rate:                                                                 \
        return connection;
#include "message_rates.inc"
#undef LINE_RESULT
#undef TERMINAL_38400
    case 46:
        return RESULT_CONNECT_57600;
    case 47:
        return RESULT_CONNECT_64000;
    case 51:
        return RESULT_CONNECT_115200;
    case 52:
        return RESULT_CONNECT_128000;
    case 53:
        return RESULT_CONNECT_230400;
    case 54:
        return RESULT_CONNECT_460800;
    case 56:
        return RESULT_CONNECT_691200;
    case 59:
        return RESULT_CONNECT_921600;
    default:
        break;
    }
    return RESULT_CONNECT;
}

void send_carrier_message(void)
{
    Uint8 code;
    if (!(SS22 & SS22_EXTENDED) || !(SREG(24) & S24_W1) ||
        (SREG(24) & S24_W2) || (BMR(32) & 0x40) ||
        (BMR(10) & BMR10_DIRECT_MODE)) {
        return;
    }
    switch (speed_of_dce) {
    case 0:
        if (SREG(14) & S14_ORIGINATE) {
            code = RESULT_CARRIER_75TX_1200RX;
        } else {
            code = RESULT_CARRIER_1200TX_75RX;
        }
        break;
    case 1:
        code = RESULT_CARRIER_300;
        break;
#define LINE_RESULT(rate, connection, carrier)                                 \
    case rate:                                                                 \
        code = carrier;                                                        \
        break;
#include "message_rates.inc"
#undef LINE_RESULT
    default:
        code = RESULT_CARRIER_2400;
        break;
    }
    send_coded_message(code, NULL);
}

void message_byte_to_dte(Uint8 value)
{
    parity_adjust(&value);
    vxd_dte_tx_cmd_byte(value);
}

void parity_adjust(Uint8 *byte)
{
    Uint8 data = *byte;
    Uint8 count = 0;
    if (data & 1) {
        count++;
    }
    if (data & 2) {
        count++;
    }
    if (data & 4) {
        count++;
    }
    if (data & 8) {
        count++;
    }
    if (data & 16) {
        count++;
    }
    if (data & 32) {
        count++;
    }
    if (data & 64) {
        count++;
    }
    if (data & 128) {
        count++;
    }
    count &= 1;
    switch (format_of_AT) {
    case FORMAT_EVEN_PARITY:
        if (count) {
            *byte |= PARITY_BIT;
        }
        break;
    case FORMAT_ODD_PARITY:
        if (!count) {
            *byte |= PARITY_BIT;
        }
        break;
    case FORMAT_MARK_PARITY:
        *byte |= PARITY_BIT;
        break;
    default:
        break;
    }
}

void send_message(const char *text)
{
    set_dte_for_message();
    while (*text) {
        message_byte_to_dte(*text++);
    }
}

void send_message_in_current_mode(const char *text)
{
    while (*text) {
        message_byte_to_dte(*text++);
    }
}

void send_coded_message(Uint8 code, const char *text)
{
    char number[4];
    Uint8 width;
    if (SREG(14) & S14_QUIET) {
        return;
    }
    if ((SREG(14) & S14_QUIET_ANSWER) &&
        ((BMR(10) & BMR10_ANSWERING) ||
         (!(SREG(14) & S14_ORIGINATE) && (BMR(6) & BMR6_ON_LINE)))) {
        BMR(10) &= ~BMR10_ANSWERING;
        return;
    }
    set_dte_for_message();
    if (!text) {
#ifdef NONMATCHING
        /* this side sends nothing for a code past CodedMessages */
        if (code >= sizeof(CodedMessages) / sizeof(CodedMessages[0])) {
            return;
        }
#endif
        text = CodedMessages[code];
    }
    BMR(8) |= BMR8_SENDING_RESULT;
    if (!(SREG(14) & S14_VERBOSE)) {
        width = 1;

        if (code > 9) {
            width = 2;
        } else if (code > 99) {
            /* @hack redundant: a width of 3 for a code past 99, which the
             * test for one past 9 takes first */
            width = 3;
        }
        convert_value_to_ascii(code, 10, width, number);
        send_message(number);
        message_byte_to_dte(SREG(3));
    } else {
        send_cr_lf();
        while (*text) {
            message_byte_to_dte(*text++);
        }
        send_cr_lf();
    }
    BMR(8) &= ~BMR8_SENDING_RESULT;
}

void send_cr_lf(void)
{
    if (!(SREG(14) & S14_QUIET)) {
        message_byte_to_dte(SREG(3));
        message_byte_to_dte(S4_MASK);
    }
}

void send_bps_message(Uint8 speed)
{
    char number[8];
    sprintf(number, bps_format, (int)ConvertTCISpeedToBps(speed));
    send_message(number);
    send_message_in_current_mode(bps_suffix);
}

Uint32 ConvertTCISpeedToBps(Uint8 speed)
{
    /* @bug memory: speed codes 60 to 239 read past the rate table. the bound
     * at 0x0caf1b4c counts the table's bytes, not its entries */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: the bound counts the table's
     * entries */
    if (speed < sizeof(tci_speed_bps) / sizeof(tci_speed_bps[0])) {
        return tci_speed_bps[speed];
    }
#else
    if (speed < sizeof(tci_speed_bps)) {
        return tci_speed_bps[speed];
    }
#endif
    return 115200;
}
