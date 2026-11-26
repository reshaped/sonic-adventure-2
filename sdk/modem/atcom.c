/* the AT command interpreter. a command line in CMD_BUF starts AT, or is A/,
 * which runs the last line's commands again. a command reads its text at
 * ptr_CMD_ALT, and leaves at_com_result AT_CONTINUE for the next to run */
#include <sg_xpt.h>
#include <stdio.h>
#include <string.h>

#include "atcom.h"
#include "modem/atcom/text.inc"
#include "bl.h"
#include "command.h"
#include "country.h"
#include "countrydb.h"
#include "data_tst.h"
#include "dce.h"
#include "dte_vxd.h"
#include "ecdc_def.h"
#include "ecdcutil.h"
#include "ee_drver.h"
#include "ee_layout.h"
#include "ee_ram.h"
#include "line_estab.h"
#include "line_mon.h"
#include "lnk_time.h"
#include "maintain.h"
#include "messages.h"
#include "modemdef.h"
#include "roc_k56.h"
#include "scm_main.h"
#include "target.h"
#include "tci_modm.h"
#include "util_mod.h"
#include "v25ter.h"

/* times in setup_timer's 10 ms ticks. Y1 sends a break of LONG_SPACE_TICKS
 * to end a call without error control. ATZ waits up to RESET_WAIT_TICKS for
 * the terminal's transmit buffer to empty */
#define LONG_SPACE_TICKS 400
#define RESET_WAIT_TICKS 100

const Uint8 S_range_min[MODEM_S_REGISTERS] = {
#include "modem/atcom/S_range_min.inc"
};
const Uint8 S_range_max[MODEM_S_REGISTERS] = {
#include "modem/atcom/S_range_max.inc"
};

const Uint8 S_REG[MODEM_S_REGISTERS] = {
#include "modem/atcom/S_REG.inc"
};
/* the factory profiles of &F1 and &F2, which differ from S_REG only in S24,
 * S36 and S52 */
static const Uint8 at_profile_1[MODEM_S_REGISTERS] = {
#include "modem/atcom/at_profile_1.inc"
};
static const Uint8 at_profile_2[MODEM_S_REGISTERS] = {
#include "modem/atcom/at_profile_2.inc"
};
static const Uint8 at_x_bits[8] = {
#include "modem/atcom/at_x_bits.inc"
};
static const Uint8 at_x_values[8] = {
#include "modem/atcom/at_x_values.inc"
};
static const Uint8 at_n_values[4] = {
#include "modem/atcom/at_n_values.inc"
};
static const Uint8 at_c_values[4] = {
#include "modem/atcom/at_c_values.inc"
};
static const Uint8 at_k_values[4] = {
#include "modem/atcom/at_k_values.inc"
};
static const Uint8 at_link_values[16] = {
#include "modem/atcom/at_link_values.inc"
};
static const Uint8 at_e_values[8] = {
#include "modem/atcom/at_e_values.inc"
};
/* the S-registers the configuration report lists, with saved set and
 * without. each list ends at REGISTER_LIST_END */
#define REGISTER_LIST_END 255
static const Uint8 at_saved_registers[24] = {
#include "modem/atcom/at_saved_registers.inc"
};
static const Uint8 at_active_registers[20] = {
#include "modem/atcom/at_active_registers.inc"
};
static const Uint8 at_port_rates[11] = {
#include "modem/atcom/at_port_rates.inc"
};
static const Uint8 other_registers[30] = {
#include "modem/atcom/other_registers.inc"
};

static void at_command_answer(void);
static void at_command_dial(void);
static void at_command_hang_up(void);
static void at_register_command(Uint8 reg);
static void at_reset_profile(Uint8 profile);
static void report_line(Uint8 mode);
static Uint8 at_hex_digit(Uint8 digit);
/* the image's at_assignment_label starts on an 8-byte boundary */
#ifdef NONMATCHING
#else
#pragma aligndata8(at_assignment_label)
#endif
static const char at_assignment_label[4];
static void at_backslash_command(void);
static void at_tilde_command(void);
static void at_dollar_command(void);
static void at_star_command(void);
static void at_paren_command(void);
static void at_country_command(void);
static void at_hash_command(void);
static void at_v25_command(void);
static void at_bang_command(void);
static void at_skip_separators(void);
static char at_line_available(void);
static void at_prepare_command(void);
static void at_finish_command(Uint8 mode);
static char at_dial_restricted(Uint8 *number);
static void command_off_hook(void);
static void at_save_profile(Uint8 profile);
static void at_percent_command(void);
static void country_parameter(void);
static void command_next(void);
static void command_test_only(void);
static void command_basic(void);
static Uint8 dial_slot(Uint8 **cursor);
static void echo_number(Uint8 *number);
static void hang_up(void);
static void off_hook(void);
static void identify_modem(Uint8 page);
static void speaker_level(Uint8 level);
static void speaker_mode(Uint8 setting);
static void speaker_update(Uint8 mode);
static void return_online(Uint8 retrain);
static void resume_data(Uint8 retrain);
static void update_phone_jack(void);
static void ampersand_command(void);
static void load_factory_profile(Uint8 profile);
static void sync_mode(Uint8 mode);
static void start_test(Uint8 test);

/* last_number holds the number dialed last, which ATDL dials again */
struct at_dial_history {
    Uint8 reserved_before_number[70];
    Uint8 last_number[MODEM_DIAL_LENGTH + 1];
    Uint8 reserved_after_number[63];
};

Uint8 second_dialing;
Uint8 use_keypad_ie;
Uint8 keypad_index;
Uint8 time_block_expired;
Uint32 IsdnRedialTimer;
static struct at_dial_history at_dial_history;
/* the profile ATZ loads once the terminal's transmit buffer empties, or
 * RESET_WAIT_TICKS pass. hang_up_inserted marks the H that ATZ on line puts
 * before its Z */
static Uint8 at_pending_profile;
static Uint8 hang_up_inserted;
static Uint8 reserved_tail[6];

void at_commands(void)
{
    at_prepare_command();
    while (at_com_result == AT_CONTINUE) {
        command_next();
    }
    at_finish_command(0);
}

static void command_next(void)
{
    Uint8 *command;
    at_skip_separators();
    command = ptr_CMD_ALT;
    if (*command == S3_THEN) {
        at_com_result = AT_OK;
        return;
    }
    if (BMR(3) & BMR3_PATTERN) {
        command_test_only();
    } else {
        switch (*command) {
        case '=':
        case '?': {
            Uint8 reg = last_s_used;
            at_register_command(reg);
            break;
        }
        case '!':
            at_bang_command();
            break;
        case '&':
            ampersand_command();
            break;
        case '%':
            at_percent_command();
            break;
        case '*':
            at_star_command();
            break;
        case '\\':
            at_backslash_command();
            break;
        case ')':
            at_paren_command();
            break;
        case '@':
            at_country_command();
            break;
        case '~':
            at_tilde_command();
            break;
        case '#':
            at_hash_command();
            break;
        case '+':
            at_v25_command();
            break;
        case '$':
            at_dollar_command();
            break;
        default:
            command_basic();
            break;
        }
    }
}

static void command_test_only(void)
{
    if (*ptr_CMD_ALT == '&') {
        advance_ptr_and_eliminate_space();
        if (*ptr_CMD_ALT == 'T') {
            advance_ptr_and_eliminate_space();
            if (*ptr_CMD_ALT < '0' || *ptr_CMD_ALT > '9' ||
                *ptr_CMD_ALT == '0') {
                if (*ptr_CMD_ALT == '0') {
                    ptr_CMD_ALT++;
                }
                test_command(0);
                return;
            }
        }
    }
    at_com_result = AT_ERROR;
}

static void command_basic(void)
{
    Uint8 command = *ptr_CMD_ALT;
    Uint8 value;
    advance_ptr_and_eliminate_space();
    if (command != 'D') {
        value = read_number();
    }
    switch (command) {
    case 'A':
        at_command_answer();
        break;
    case 'C':
        if (value != 1) {
            at_com_result = AT_ERROR;
        }
        break;
    case 'D':
        at_command_dial();
        break;
    case 'E':
        if (value > 1) {
            at_com_result = AT_ERROR;
        } else if (value == 0) {
            SREG(14) &= ~S14_ECHO;
        } else {
            SREG(14) |= S14_ECHO;
        }
        break;
    case 'F':
        if (value != 1) {
            at_com_result = AT_ERROR;
        }
        break;
    case 'G':
    case 'H':
        if (cur_cnt->options & OFF_HOOK_IS_H9) {
            if (value != 0 && value != 9) {
                at_com_result = AT_ERROR;
                break;
            }
            if (value == 9) {
                value = 1;
            }
        }
        if (value == 1) {
            command_off_hook();
        } else if (value == 0) {
            at_command_hang_up();
        } else {
            at_com_result = AT_ERROR;
        }
        break;
    case 'I':
        identify_modem(value);
        break;
    case 'L':
        speaker_level(value);
        break;
    case 'M':
        speaker_mode(value);
        break;
    case 'N':
        switch (value) {
        case 0:
            SREG(24) &= ~S24_N;
            break;
        case 1:
            SREG(24) &= ~S24_N;
            SREG(24) |= S24_N1;
            break;
        case 2:
            SREG(24) &= ~S24_N;
            SREG(24) |= S24_N2;
            break;
        case 3:
            SREG(24) |= S24_N;
            break;
        default:
            at_com_result = AT_ERROR;
            break;
        }
        break;
    case 'O':
        if (value > 1) {
            at_com_result = AT_ERROR;
        } else {
            return_online(value);
        }
        break;
    case 'P':
        SREG(14) |= S14_PULSE;
        break;
    case 'S':
        at_register_command(value);
        break;
    case 'T':
        SREG(14) &= ~S14_PULSE;
        break;
    case 'V':
        if (value == 0) {
            SREG(14) &= ~S14_VERBOSE;
        } else if (value == 1) {
            SREG(14) |= S14_VERBOSE;
        } else {
            at_com_result = AT_ERROR;
        }
        break;
    case 'W':
        if (value == 3) {
            /* @hack spelling: S24 written through reg, with its bits held in
             * bits, where one assignment to SREG(24) would do */
            int bits = value << 5;
            Uint8 *reg = &SREG(24);
            *reg = (*reg & ~S24_W) | bits;
        } else {
            at_com_result = AT_ERROR;
        }
        break;
    case 'X':
        if (value < 7) {
            SREG(22) = at_x_bits[value] | (SREG(22) & ~S22_RESULTS);
            update_SS22();
        } else {
            at_com_result = AT_ERROR;
        }
        return;
    case 'Z':
        at_reset_profile(value);
        break;
    default:
        at_com_result = AT_ERROR;
        break;
    }
}

static void at_command_answer(void)
{
    SREG(1) = 0;
    answer_modem_on_pstn();
}

void answer_modem_on_pstn(void)
{
    if (get_ring_state()) {
        at_com_result = AT_WAIT_COMMAND;
        return;
    }
    if (!timer_expired(OFF_HOOK_TIM)) {
        BMR(11) |= 2;
        at_com_result = AT_WAIT_COMMAND;
        return;
    }
    BMR(5) &= 0xfd;
    BMR(5) &= 0xfe;
    if ((cur_cnt->options & CHECK_LINE_IN_USE) && !at_line_available()) {
        return;
    }
    if (BMR(6) & BMR6_ON_LINE) {
        at_com_result = AT_ERROR;
        return;
    }
    init_on_line();
    SREG(14) &= ~S14_ORIGINATE;
    speed_of_dce = TCI_SPEED_14400;
    maintain_phase = MAINTAIN_ESTABLISH;
    at_com_result = AT_WAIT_LINE;
}

static void at_command_dial(void)
{
    if (!timer_expired(OFF_HOOK_TIM) || SREG(1)) {
        BMR(11) |= 2;
        at_com_result = AT_WAIT_COMMAND;
        return;
    }
    do_modem_dial_on_pstn();
}

void do_modem_dial_on_pstn(void)
{
    Uint8 resume = BMR(5) & 8;
    Uint8 result = 1;
    BMR(5) &= 0xf7;
    BMR(5) &= 0xfe;
    BMR(13) &= 0xbf;
    if (!(BMR(6) & BMR6_ON_LINE)) {
        if (cur_cnt->options & CHECK_LINE_IN_USE) {
            if (!at_line_available()) {
                return;
            }
        }
        if (!(SREG(27) & S27_Q) || !(SREG(27) & 2)) {
            if (BMR(16) & 0x80) {
                BMR(16) &= 0x7f;
                BMR(16) |= 2;
            }
            if (*ptr_CMD_ALT == 'L') {
                /* @hack type: digit is a volatile pointer, where a plain one
                 * would do */
                Uint8 *volatile digit;
                echo_number(at_dial_history.last_number);
                copy_dial_string(dial_buffer, at_dial_history.last_number);
                digit = at_dial_history.last_number;
                do {
                    if (*digit == ';') {
                        result = 2;
                    } else if (*digit != S3_THEN) {
                        continue;
                    }
                    break;
                } while (*digit++);
            } else if (!(BMR(1) & 1)) {
                result = get_dial_string(&ptr_CMD_ALT, dial_buffer, resume,
                                         MODEM_DIAL_LENGTH);
                if (!result) {
                    at_com_result = AT_ERROR;
                    return;
                }
                if (BMR(8) & 0x80) {
                    echo_number(dial_buffer);
                    BMR(8) &= 0x7f;
                }
            }
        } else if (BMR(2) & BMR2_DTR_DIAL) {
            BMR(2) &= ~BMR2_DTR_DIAL;
            result = get_dial_string(&ptr_CMD_ALT, dial_buffer, resume,
                                     MODEM_DIAL_LENGTH);
            if (!result) {
                at_com_result = AT_ERROR;
                return;
            }
        } else {
            at_com_result = AT_OK;
            return;
        }
        copy_dial_string(at_dial_history.last_number, dial_buffer);
        if (at_dial_restricted(dial_buffer) == 1) {
            at_com_result = AT_SILENT;
        } else {
            SREG(14) |= S14_ORIGINATE;
            speed_of_dce = TCI_SPEED_14400;
            dial_string_ptr = dial_buffer;
            /* @hack spelling: S3_THEN on the comparison's left, where
             * dial_buffer[0] first would do */
            if (S3_THEN == dial_buffer[0]) {
                BMR(16) |= 2;
            }
            maintain_phase = MAINTAIN_DIAL;
            BMR(30) |= 4;
            if (result == 2) {
                at_com_result = AT_WAIT_COMMAND;
            } else {
                at_com_result = AT_WAIT_LINE;
            }
        }
    } else {
        at_com_result = AT_ERROR;
    }
}

Uint8 get_dial_string(Uint8 **source, Uint8 *buffer, char resume, Uint16 limit)
{
    Uint8 *cursor = *source;
    Uint8 *out = buffer;
#ifdef NONMATCHING
    /* this side counts length in a Uint16, up to limit, and keeps a stored
     * number's length apart */
    Uint16 length = 0;
    Uint16 stored_length;
#else
    Uint8 length = 0;
#endif
    Uint8 slot;
    int remaining;
    BMR(8) &= 0x7f;
    if (resume) {
        if ((SREG(27) & S27_Q) == 2) {
            BMR(13) |= 0x40;
#ifdef NONMATCHING
            /* this side appends slot 0's number only where it holds one, in the
             * room limit leaves, and fails where it doesn't fit. the matching
             * side appends it unchecked */
            if (GetLengthOfDialString(0) != 0 &&
                !eedrvAppendDialString(0, &out, limit)) {
                return 0;
            }
#else
            read_dial_and_logon_strings(0, &out);
#endif
            BMR(8) &= 0x7f;
        }
        *out = S3_THEN;
        return 1;
    }
    do {
        /* @hack redundant: a dead test of S3_THEN and source */
        if (0) {
            if (S3_THEN && source) {
            }
        }
        remaining = limit;
        if (*cursor == 'S') {
            cursor++;
            if (S3_THEN != ' ') {
                while (*cursor == ' ') {
                    cursor++;
                }
            }
            if (*cursor == '=') {
                cursor++;
                if (S3_THEN != ' ') {
                    while (*cursor == ' ') {
                        cursor++;
                    }
                }
            }
            slot = dial_slot(&cursor);
            if (slot >= EEPROM_DIAL_SLOTS) {
                *source = cursor;
                return 0;
            }
            BMR(13) |= 0x40;
#ifdef NONMATCHING
            /* this side appends the slot's number in the room left before
             * limit, and fails where it doesn't fit. length then counts the
             * bytes out holds */
            stored_length = GetLengthOfDialString(slot);
            if (stored_length != 0 ||
                (cur_cnt->reset_flags & NO_STORED_LENGTH_CHECK)) {
                if (!eedrvAppendDialString(slot, &out,
                                           (Uint16)(limit - length))) {
                    return 0;
                }
            }
            length = (Uint16)(out - buffer);
#else
            if (cur_cnt->reset_flags & NO_STORED_LENGTH_CHECK) {
                if (!read_dial_and_logon_strings(slot, &out)) {
                    return 0;
                }
            } else {
                if (GetLengthOfDialString(slot) <=
                    (remaining = remaining - length)) {
                    read_dial_and_logon_strings(slot, &out);
                    length += GetLengthOfDialString(slot);
                } else {
                    return 0;
                }
            }
#endif
            BMR(8) |= 0x80;
        } else {
#ifdef NONMATCHING
            /* this side stops at limit bytes, unless the byte at the cursor is
             * the S3 that closes the string */
            if (length == limit && *cursor != S3_THEN) {
                break;
            }
#endif
            *out++ = *cursor;
            if (*cursor == S3_THEN) {
                *out++ = 0;
                *source = cursor;
                return 1;
            } else {
                cursor++;
                length++;
                if (length >= remaining || cursor[-1] == ';') {
                    break;
                }
            }
        }
    } while (1);
    /* @hack redundant: a dead test of source */
    if (0) {
        if (source) {
        }
    }
    *out = S3_THEN;
    if (length >= remaining) {
        *source = cursor;
        return 3;
    } else {
        *source = cursor;
        return 2;
    }
}

static Uint8 dial_slot(Uint8 **cursor)
{
    Uint8 slot = 0;
    while (**cursor >= '0' && **cursor <= '9') {
#ifdef NONMATCHING
        /* this side repairs the bug tagged below: a slot past EEPROM_DIAL_SLOTS
         * stops growing, so the Uint8 can't wrap back into range. its digits
         * still move the cursor */
        if (slot < EEPROM_DIAL_SLOTS) {
            slot = (slot * 10) + **cursor - '0';
        }
#else
        /* @bug arithmetic: slot keeps only the number's low 8 bits, so S256
         * dials stored number 0. the sum runs at 0x0caebaac, and
         * get_dial_string tests its low byte against EEPROM_DIAL_SLOTS at
         * 0x0caeb992 */
        slot = (slot * 10) + **cursor - '0';
#endif
        (*cursor)++;
        if (S3_THEN != ' ') {
            while (**cursor == ' ') {
                (*cursor)++;
            }
        }
    }
    return slot;
}

static void echo_number(Uint8 *number)
{
    Uint8 hidden;
    if (!(SREG(14) & S14_QUIET)) {
        hidden = 0;
        send_dc_cr_lf();
        while (*number != SREG(3) && *number != 0) {
            if (hidden == 1) {
                if (*number == ']') {
                    hidden = 0;
                }
            } else if (*number == '[') {
                hidden = 1;
            } else {
                message_byte_to_dte(*number);
            }
            number++;
        }
        send_dc_cr_lf();
    }
}

static void at_command_hang_up(void)
{
    hang_up();
}

static void hang_up(void)
{
    if (BMR(6) & BMR6_ON_LINE) {
        if (!(STATUS_BITS & STATUS_INACTIVE)) {
            SREG(45) = S45_HUNG_UP;
        }
        if (BMR(3) & BMR3_LOCAL_ANALOG) {
            test_phase = TEST_REPORT;
            at_com_result = AT_WAIT_LINE;
            if (BMR(5) & 4) {
                BMR(5) &= 0xfb;
                BMR(5) &= 0xfe;
            }
        } else {
            if ((BMR(27) & BMR27_LAPM) || (BMR(29) & BMR29_MNP)) {
                cmd_parameter_buffer[0] = HALT_AFTER_LINE;
                cmd_response_buffer[0] = ECDC_RESPONSE_NONE;
                cmd_HALT();
                maintain_phase = MAINTAIN_DRAIN;
            } else if (SREG(21) & S21_LONG_SPACE_DISCONNECT) {
                dce_tx_brk(LONG_SPACE_TICKS, 0);
                BMR(1) |= 0x40;
                setup_timer(&at_command_timer, LONG_SPACE_TICKS);
                at_com_result = AT_WAIT_COMMAND;
                return;
            } else {
                dp_start_sending_clear_down();
                maintain_phase = MAINTAIN_CLEAR_DOWN;
            }
            at_com_result = AT_WAIT_COMMAND;
        }
    } else {
        go_off_line();
        if (BMR(5) & 4) {
            BMR(5) &= 0xfb;
            BMR(5) &= 0xfe;
            at_com_result = AT_OK;
        } else {
            at_com_result = AT_CONTINUE;
        }
    }
}

void H0_common_section(void)
{
    BMR(16) &= 0x7f;
    BMR(11) &= ~BMR11_LEASED_LINE;
    go_off_line();
    cmd_parameter_buffer[0] = HALT_NOW;
    cmd_HALT();
    BMR(1) &= 0x7f;
    BMR(1) &= 0xbf;
    if (BMR(5) & 4) {
        BMR(5) &= 0xfb;
        BMR(5) &= 0xfe;
        at_com_result = AT_OK;
    } else {
        at_com_result = AT_CONTINUE;
    }
}

static void command_off_hook(void)
{
    off_hook();
}

static void off_hook(void)
{
    if (!(BMR(6) & BMR6_ON_LINE)) {
        if (!(BMR(9) & BMR9_OFF_HOOK)) {
            dp_init_data_pump(cur_cnt->pump_option_1, cur_cnt->pump_option_2);
        }
        if (SREG(22) & S22_SPEAKER) {
            adj_spk_level();
        }
        BMR(9) |= BMR9_OFF_HOOK;
        li_set_hook(0);
        dp_off_hook();
        if (SREG(21) & 1) {
            li_set_rj11(0);
        } else {
            li_set_rj11(1);
        }
    }
}

static void identify_modem(Uint8 page)
{
    char number[11];
    char timestamp[32];
    Uint16 checksum;
    /* @hack type: i is an int compared through a cast, where a size_t would
     * do */
    int i;
    switch (page) {
    case 0:
        send_dc_cr_lf();
        send_message(AT_PRODUCT_ID);
        send_dc_cr_lf();
        break;
    case 1:
        checksum = 0;
        CompiledTime((Uint8 *)timestamp);
        for (i = 0; (size_t)i < strlen(timestamp); i++) {
            checksum += (Uint8)timestamp[i];
        }
        send_dc_cr_lf();
        convert_value_to_ascii(checksum, 10, 3, number);
        send_message(number);
        send_dc_cr_lf();
        break;
    case 2:
        send_dc_cr_lf();
        send_coded_message(RESULT_OK, NULL);
        send_dc_cr_lf();
        break;
    case 3:
        send_dc_cr_lf();
        send_message(AT_REVISION_TEXT);
        send_dc_cr_lf();
        break;
    case 4:
        send_dc_cr_lf();
        send_message(AT_IDENTIFICATION_A);
        send_dc_cr_lf();
        send_dc_cr_lf();
        send_message(AT_IDENTIFICATION_B);
        send_dc_cr_lf();
        send_dc_cr_lf();
        send_message(AT_IDENTIFICATION_R1);
        send_dc_cr_lf();
        send_dc_cr_lf();
        send_message(AT_IDENTIFICATION_R3);
        send_dc_cr_lf();
        send_dc_cr_lf();
        break;
    case 5:
        send_dc_cr_lf();
        send_message(target_send_dp_version());
        convert_value_to_ascii(dp_report_fw(), 16, 4, number);
        send_message(number);
        send_dc_cr_lf();
        break;
    case 6:
        break;
    case 7:
        send_cr_lf();
        send_message(target_id());
        send_cr_lf();
        break;
    case 8:
        send_cr_lf();
        send_message((char *)cur_cnt->country_name);
        send_cr_lf();
        break;
    case 9:
        lnk_timeDisplayCompiledTime();
        break;
    default:
        at_com_result = AT_ERROR;
        break;
    }
}

static void speaker_level(Uint8 level)
{
    if (level > 3) {
        at_com_result = AT_ERROR;
    } else {
        if (level == 0) {
            level++;
        }
        SREG(22) = (SREG(22) & ~S22_VOLUME) + level;
        speaker_update(SREG(22) & S22_SPEAKER);
    }
}

static void speaker_mode(Uint8 setting)
{
    if (setting > 3) {
        at_com_result = AT_ERROR;
    } else {
        Uint8 mode = setting << 2;
        SREG(22) = (SREG(22) & ~S22_SPEAKER) + mode;
        speaker_update(mode);
    }
}

static void speaker_update(Uint8 mode)
{
    if (BMR(9) & BMR9_OFF_HOOK) {
        if (((BMR(6) & BMR6_ON_LINE) && mode == S22_SPEAKER_ALWAYS) ||
            (!(BMR(6) & BMR6_ON_LINE) && mode != 0)) {
            /* @hack redundant: an empty block no path reaches */
            if (0) {
            }
            adj_spk_level();
            return;
        }
        /* @hack redundant: an empty block no path reaches */
        if (0) {
        }
        target_set_volume(0);
    }
}

static void return_online(Uint8 retrain)
{
    resume_data(retrain);
}

static void resume_data(Uint8 retrain)
{
    if (BMR(12) & 1) {
        at_com_result = AT_ERROR;
    } else if (BMR(16) & 0x80) {
        maintain_phase = MAINTAIN_ESTABLISH;
        at_com_result = AT_WAIT_LINE;
    } else if (!(BMR(6) & BMR6_ON_LINE)) {
        at_com_result = AT_NO_CARRIER;
    } else if (retrain != 0) {
        if (speed_of_dce >= TCI_SPEED_1200 && !(BMR(3) & BMR3_LOCAL_ANALOG)) {
            if (BMR(3) & BMR3_LOCAL_ANALOG) {
                at_com_result = AT_ERROR;
            }
            line_monitor_phase = LINE_COMMAND_RETRAIN;
            at_com_result = AT_WAIT_LINE;
        } else {
            at_com_result = AT_ERROR;
        }
    } else {
        v25terUpdateV25terPar();
        update_OIS();
        cmd_OIS();
        umCarrierDetected();
        modem_connect();
        BMR(7) &= ~BMR7_COMMAND_LINE;
        S3_THEN = SREG(3);
        at_com_result = AT_WAIT_LINE;
    }
}

static void at_register_command(Uint8 reg)
{
    Uint8 value;
    char number[11];
    Uint8 speed;
#ifdef NONMATCHING
    /* this side sets setting below, once reg is in range */
    Uint8 *setting;
#else
    Uint8 *setting = &SREG(reg);
#endif
    if (reg >= MODEM_S_REGISTERS) {
        at_com_result = AT_ERROR;
        return;
    }
#ifdef NONMATCHING
    /* this side forms the register's address only for a reg in range */
    setting = &SREG(reg);
#endif
    at_skip_separators();
    switch (*ptr_CMD_ALT) {
    case '?':
        send_cr_lf();
        convert_value_to_ascii(*setting, 10, 3, number);
        send_message(number);
        send_cr_lf();
        advance_ptr_and_eliminate_space();
        last_s_used = reg;
        break;
    case '!':
        send_cr_lf();
        convert_value_to_ascii(*setting, 16, 2, number);
        send_message(number);
        send_cr_lf();
        advance_ptr_and_eliminate_space();
        last_s_used = reg;
        break;
    case '=':
        advance_ptr_and_eliminate_space();
        value = read_number();
        if (value < S_range_min[reg] || value > S_range_max[reg]) {
            at_com_result = AT_ERROR;
            break;
        }
        if (reg == 27 && ((SREG(27) ^ value) & S27_Q)) {
            if (BMR(6) & BMR6_ON_LINE) {
                at_com_result = AT_ERROR;
                break;
            }
            BMR(16) &= ~BMR16_DTR_ON;
            BMR(16) &= ~BMR16_DTR_DROPPED;
            if (vxd_dte_get_dtr()) {
                BMR(16) |= BMR16_DTR_ON;
            }
        } else if (reg == 21 && ((SREG(21) ^ value) & 1)) {
            if (BMR(6) & BMR6_ON_LINE) {
                update_phone_jack();
            }
        } else if (reg == 20 && SREG(20) != value) {
            if (value == 48 || value == 49 || value == 50 || value == 55 ||
                value == 57 || value == 58) {
                at_com_result = AT_ERROR;
                break;
            }
            if (value != dte_speed_restriction(value)) {
                at_com_result = AT_ERROR;
                return;
            }
            speed_of_AT = value;
            TCI_got_format(speed_of_AT, format_of_AT);
        } else if ((reg == 23 && ((SREG(23) ^ value) & 0x0e)) ||
                   (reg == 39 && ((SREG(39) ^ value) & S39_SPEED))) {
            *setting = value;
            if ((SREG(39) & S39_SPEED) == S39_SPEED_460800) {
                speed = 10;
            } else if (SREG(39) & S39_SPEED_115200) {
                speed = 8;
            } else if (SREG(39) & S39_SPEED_230400) {
                speed = 9;
            } else {
                speed = (SREG(23) & 0x0e) >> 1;
            }
            speed_of_AT = at_port_rates[speed];
            SREG(20) = speed_of_AT;
            format_of_AT = S23_to_fmt[(SREG(23) & S23_PARITY) >> 4];
        } else {
            if (reg == 3 && (value == SREG(4) || value == SREG(5))) {
                at_com_result = AT_ERROR;
                break;
            } else if (reg == 4 && (value == SREG(3) || value == SREG(5))) {
                at_com_result = AT_ERROR;
                break;
            }
            if (reg == 5 && (value == SREG(3) || value == SREG(4))) {
                at_com_result = AT_ERROR;
                break;
            }
            if (reg == 0) {
                if (value == 1 && (cur_cnt->reset_flags & RAISE_S0_1)) {
                    value = 2;
                }
                if (value == 2 && (cur_cnt->reset_flags & RAISE_S0_2)) {
                    value = 3;
                }
            } else {
                if (reg == 35 && !(cur_cnt->options & ALLOW_S35)) {
                    at_com_result = AT_ERROR;
                    break;
                }
                if (reg == 56) {
                    if (country_CheckCountry(value)) {
                        eedrvSaveCountryCode(value);
                    } else {
                        at_com_result = AT_ERROR;
                    }
                    break;
                }
            }
        }
        country_CheckSregLimits(reg, &value);
        *setting = value;
        last_s_used = reg;
        S4_MASK = SREG(4) & 0x7f;
        update_SS22();
        if (SREG(49) & 0x40) {
            SREG(14) |= S14_QUIET;
        }
        break;
    }
}

static void update_phone_jack(void)
{
    if (SREG(21) & 1) {
        li_set_rj11(0);
    } else {
        li_set_rj11(1);
    }
}

static void at_reset_profile(Uint8 profile)
{
    Uint8 *cursor;
    if ((BMR(6) & BMR6_ON_LINE) && !hang_up_inserted) {
        if (CMD_B_PT != MODEM_COMMAND_LENGTH) {
            /* put an H before the Z, so the line hangs up and then resets */
            cursor = CMD_BUF + CMD_B_PT;
            do {
#ifdef NONMATCHING
                /* this side fails where no Z lies before the cursor, where the
                 * matching side's scan passes the buffer's start */
                if (cursor == CMD_BUF) {
                    at_com_result = AT_ERROR;
                    return;
                }
#endif
                cursor--;
                cursor[1] = *cursor;
            } while (*cursor != 'Z' || cursor >= ptr_CMD_ALT);
            ptr_CMD_ALT = cursor;
            *ptr_CMD_ALT = 'H';
            CMD_B_PT++;
            hang_up_inserted = 1;
            return;
        }
    }
    if (profile < EEPROM_PROFILES) {
        setup_timer(&at_command_timer, RESET_WAIT_TICKS);
        if (!vxd_dte_tx_buff_empty(0)) {
            at_pending_profile = profile;
            BMR(5) |= 0x10;
            at_com_result = AT_WAIT_LINE;
        } else {
            BMR(5) &= 0xef;
            hang_up_inserted = 0;
            target_control(0);
            modem_reset(profile);
            maintain(0);
            TCI_sw_reset();
            set_dte_for_message();
        }
    } else {
        at_com_result = AT_ERROR;
    }
    /* @hack redundant: the return that ends the function */
    return; /* NOLINT(readability-redundant-control-flow) */
}

static void ampersand_command(void)
{
    Uint8 command;
    Uint8 value;
    advance_ptr_and_eliminate_space();
    command = *ptr_CMD_ALT;
    advance_ptr_and_eliminate_space();
    value = read_number();
    switch (command) {
    case 'C':
        set_data_carrier_detect_option(value);
        break;
    case 'D':
        set_data_terminal_option(value);
        break;
    case 'F':
        load_factory_profile(value);
        break;
    case 'P':
        if (value == 0) {
            SREG(22) &= ~S22_FAST_PULSE;
        } else if (value == 1) {
            SREG(22) |= S22_FAST_PULSE;
        } else {
            at_com_result = AT_ERROR;
        }
        break;
    case 'Q':
        sync_mode(value);
        break;
    case 'T':
        test_command(value);
        break;
    case 'W':
        at_save_profile(value);
        break;
    default:
        at_com_result = AT_ERROR;
        break;
    }
}

void set_data_carrier_detect_option(Uint8 setting)
{
    if (setting == 0) {
        SREG(31) &= 0xfb;
        SREG(21) &= 0xdf;
        if (!(SREG(21) & 2) || !(SREG(21) & S21_D)) {
            vxd_dte_set_dcd(0);
        }
    } else if (setting == 1) {
        SREG(31) &= 0xfb;
        SREG(21) |= 0x20;
        if (!(BMR(6) & BMR6_ON_LINE)) {
            vxd_dte_set_dcd(1);
        }
    } else {
        at_com_result = AT_ERROR;
    }
}

void set_data_terminal_option(Uint8 setting)
{
    if (setting <= 3) {
        if (setting < 2 && (cur_cnt->reset_flags & FORCE_DTR_HANGUP)) {
            setting = 2;
        }
        BMR(16) &= ~BMR16_DTR_DROPPED;
        BMR(16) &= ~BMR16_DTR_ON;
        if (!vxd_dte_get_dtr()) {
            BMR(16) |= BMR16_DTR_ON;
        }
        setting = setting << 3;
        SREG(21) = (SREG(21) & ~S21_D) + setting;
    } else {
        at_com_result = AT_ERROR;
    }
}

static void load_factory_profile(Uint8 profile)
{
    const Uint8 *defaults;
    Uint8 *settings;
    Uint8 old17;
    Uint8 old20;
    Uint8 old23;
    Uint8 old31;
    Uint8 old39;
    Uint8 i;
    if (BMR(6) & BMR6_ON_LINE) {
        at_com_result = AT_ERROR;
        return;
    }
    switch (profile) {
    case 0:
        defaults = S_REG;
        break;
    case 1:
        defaults = at_profile_1;
        break;
    case 2:
        defaults = at_profile_2;
        break;
    default:
        at_com_result = AT_ERROR;
        return;
    }
    old17 = SREG(17) & 0xf0;
    old20 = SREG(20);
    old23 = SREG(23);
    old31 = SREG(31);
    old39 = SREG(39);
    settings = Sregisters;
    for (i = 0; i < MODEM_S_REGISTERS; i++) {
        *settings++ = *defaults++;
    }
    update_SS22();
    S4_MASK = SREG(4) & 0x7f;
    SREG(17) = old17;
    SREG(20) = old20;
    SREG(23) = (SREG(23) & 0xc1) | (old23 & 0x3e);
    SREG(31) = (SREG(31) & 0xef) | (old31 & 0x10);
    SREG(39) = (SREG(39) & ~S39_SPEED) | (old39 & S39_SPEED);
    country_CheckAllSregDefaults(Sregisters);
    eeprom_read_data(EE_COUNTRY, 1, &SREG(56));
    vxd_dte_set_cts(0);
    if (SREG(21) & 0x40) {
        vxd_dte_set_dsr(1);
    } else if (!(SREG(21) & 2) || !(SREG(21) & S21_D)) {
        vxd_dte_set_dsr(0);
    }
    if (SREG(21) & 0x20) {
        vxd_dte_set_dcd(1);
    } else if (!(SREG(21) & 2) || !(SREG(21) & S21_D)) {
        vxd_dte_set_dcd(0);
    }
}

static void sync_mode(Uint8 mode)
{
    if (mode > 7 || (BMR(6) & BMR6_ON_LINE)) {
        at_com_result = AT_ERROR;
        return;
    }
    if (mode >= 1 && mode <= 3) {
        at_com_result = AT_ERROR;
        return;
    }
    /* @hack redundant: the test the one above leaves false, and its branch */
    if (mode >= 1 && mode <= 3) {
        Sregisters[27] = (Sregisters[27] & 0xf4) | (mode & 3);
    } else {
        Sregisters[27] &= ~S27_Q;
        switch (mode) {
        case 0:
            Sregisters[36] = 0x22;
            break;
        case 4:
            Sregisters[36] = 0x11;
            break;
        case 7:
            Sregisters[36] = 0xaa;
            break;
        default:
            at_com_result = AT_ERROR;
            break;
        }
    }
    BMR(16) &= ~BMR16_DTR_DROPPED;
    BMR(16) &= ~BMR16_DTR_ON;
    if (!vxd_dte_get_dtr()) {
        BMR(16) |= BMR16_DTR_ON;
    }
}

void set_data_set_ready_options(Uint8 setting)
{
    if (setting == 1) {
        SREG(21) |= 0x40;
        if (!(BMR(6) & BMR6_ON_LINE)) {
            vxd_dte_set_dsr(1);
        } else {
            vxd_dte_set_dsr(0);
        }
    } else if (setting == 0) {
        SREG(21) &= 0xbf;
        if (!(SREG(21) & 2) || !(SREG(21) & S21_D)) {
            vxd_dte_set_dsr(0);
        }
    } else {
        at_com_result = AT_ERROR;
    }
}

void test_command(Uint8 test)
{
    start_test(test);
}

static void start_test(Uint8 test)
{
    if (test == 10) {
        SREG(39) &= ~S39_TEST_INPUTS;
    } else if (test == 9) {
        SREG(39) |= S39_TEST_INPUTS;
    } else {
        /* @hack type: mode, an int copy of test, where test itself would do */
        int mode = test;
        if (SREG(16) && mode) {
            at_com_result = AT_ERROR;
            return;
        }
        switch (mode) {
        case 8:
            if (!(SREG(27) & S27_Q) && (SREG(36) & 0x0d)) {
                at_com_result = AT_ERROR;
                break;
            }
            /* fall through */
        case 1:
        case 2:
            if (BMR(6) & BMR6_ON_LINE) {
                at_com_result = AT_ERROR;
                break;
            }
            dp_init_data_pump(cur_cnt->pump_option_1, cur_cnt->pump_option_2);
            select_dce_speed(1);
            if (SREG(37) == 12) {
                at_com_result = AT_ERROR;
                break;
            }
            SREG(16) = (mode == 8) ? 0x40 : 1;
            ON_LI_TIM = TIM_10MS;
            test_phase = TEST_LOCAL_ANALOG;
            at_com_result = AT_WAIT_LINE;
            break;
        case 6:
        case 7:
            if (!(BMR(6) & BMR6_ON_LINE) || speed_of_dce < TCI_SPEED_1200 ||
                dp_get_tx_bit_rate() != dp_get_rx_bit_rate()) {
                at_com_result = AT_ERROR;
                break;
            }
            SREG(16) = (mode == 6) ? 0x10 : 0x20;
            test_phase = TEST_REMOTE_DIGITAL;
            at_com_result = AT_WAIT_LINE;
            break;
        case 5:
            SREG(23) &= ~S23_RDL;
            if (BMR(6) & BMR6_ON_LINE) {
                dp_disable_rdl();
            }
            break;
        case 4:
            SREG(23) |= S23_RDL;
            if (BMR(6) & BMR6_ON_LINE) {
                dp_enable_rdl();
            }
            break;
        case 3:
            if ((BMR(29) & BMR29_MNP) || (BMR(27) & BMR27_LAPM) ||
                !(BMR(6) & BMR6_ON_LINE)) {
                at_com_result = AT_ERROR;
                break;
            }
            SREG(16) = 4;
            target_local_digital_loop(1);
            BMR(12) |= 1;
            BMR(3) |= BMR3_TEST;
            dte_set_tst(0);
            setup_timer(&S18_timer, (Uint32)(SREG(18) * TICKS_PER_SECOND));
            test_phase = TEST_ACTIVE;
            at_com_result = AT_OK;
            break;
        case 0:
            if (BMR(3) & BMR3_TEST) {
                test_phase = TEST_REPORT;
                at_com_result = AT_WAIT_LINE;
            }
            break;
        default:
            at_com_result = AT_ERROR;
            break;
        }
    }
}

#define REPORT_OPTION(name, value) send_conf_to_dte((name), (value), 1)

void send_configuration_to_dte(Uint8 saved)
{
    char label[8];
    char number[4];
    Uint8 option;
    const Uint8 *registers;
    Uint8 width;
    char *digit;
    char *out;
    line_length = 0;
    REPORT_OPTION(AT_REPORT_B, (SREG(27) & S27_BELL) >> 6);
    REPORT_OPTION(AT_REPORT_E, (SREG(14) & S14_ECHO) >> 1);
    REPORT_OPTION(AT_REPORT_L, SREG(22) & S22_VOLUME);
    REPORT_OPTION(AT_REPORT_M, (SREG(22) & S22_SPEAKER) >> 2);
    REPORT_OPTION(AT_REPORT_N, at_n_values[(SREG(24) & S24_N) >> 3]);
    REPORT_OPTION(AT_REPORT_Q, ((SREG(14) & S14_QUIET_ANSWER) >> 3) +
                                   ((SREG(14) & S14_QUIET) >> 2));
    REPORT_OPTION(AT_REPORT_V, (SREG(14) & S14_VERBOSE) >> 3);
    REPORT_OPTION(AT_REPORT_W, (SREG(24) & S24_W) >> 5);
    REPORT_OPTION(AT_REPORT_X, at_x_values[(SREG(22) & S22_RESULTS) >> 4]);
    REPORT_OPTION(AT_REPORT_Y, (SREG(21) & S21_LONG_SPACE_DISCONNECT) >> 7);
    REPORT_OPTION(AT_REPORT_AMPERSAND_B, (SREG(24) & S24_FIXED_DTE) >> 7);
    REPORT_OPTION(AT_REPORT_AMPERSAND_C, (SREG(21) & 0x20) >> 5);
    REPORT_OPTION(AT_REPORT_AMPERSAND_D, (SREG(21) & S21_D) >> 3);
    REPORT_OPTION(AT_REPORT_AMPERSAND_E,
                  at_e_values[((SREG(39) & S39_LINE_QUALITY) >> 5) |
                              (SREG(39) & (S39_FALL_FORWARD | S39_FALL_BACK))]);
    REPORT_OPTION(AT_REPORT_AMPERSAND_G, (SREG(23) & S23_GUARD) >> 6);
    REPORT_OPTION(AT_REPORT_AMPERSAND_I, (SREG(39) & S39_BREAK) >> 4);
    REPORT_OPTION(AT_REPORT_AMPERSAND_J, SREG(21) & 1);
    REPORT_OPTION(AT_REPORT_AMPERSAND_K, SREG(49) & 0x1f);
    REPORT_OPTION(AT_REPORT_AMPERSAND_L, (SREG(27) & S27_LEASED_LINE) >> 2);
    REPORT_OPTION(AT_REPORT_AMPERSAND_N, SREG(24) & 1);
    REPORT_OPTION(AT_REPORT_AMPERSAND_O, (SREG(24) & 2) >> 1);
    REPORT_OPTION(AT_REPORT_AMPERSAND_P, (SREG(22) & S22_FAST_PULSE) >> 7);
    if ((SREG(27) & S27_Q) > 0) {
        REPORT_OPTION(AT_REPORT_AMPERSAND_Q, SREG(27) & S27_Q);
    } else if ((SREG(36) & 4) && (SREG(36) & 8)) {
        REPORT_OPTION(AT_REPORT_AMPERSAND_Q, 6);
    } else if (SREG(36) & 8) {
        REPORT_OPTION(AT_REPORT_AMPERSAND_Q, 7);
    } else if (SREG(36) & 4) {
        REPORT_OPTION(AT_REPORT_AMPERSAND_Q, 5);
    } else if (SREG(36) & 1) {
        REPORT_OPTION(AT_REPORT_AMPERSAND_Q, 4);
    } else {
        REPORT_OPTION(AT_REPORT_AMPERSAND_Q, 0);
    }
    REPORT_OPTION(AT_REPORT_AMPERSAND_R, (SREG(21) & 4) >> 2);
    REPORT_OPTION(AT_REPORT_AMPERSAND_S, (SREG(21) & 0x40) >> 6);
    REPORT_OPTION(AT_REPORT_AMPERSAND_U, (SREG(24) & 4) >> 2);
    REPORT_OPTION(AT_REPORT_AMPERSAND_X, (SREG(27) & 0x30) >> 4);
    if (saved) {
        eeprom_read_data(EE_PROFILE_SELECTOR, 1, &option);
        option &= 15;
        REPORT_OPTION(AT_REPORT_AMPERSAND_Y, option);
    }
    REPORT_OPTION(AT_REPORT_PERCENT_C, (SREG(24) & 4) >> 2);
    send_conf_to_dte(AT_REPORT_PERCENT_D, SREG(38), 3);
    REPORT_OPTION(AT_REPORT_BACKSLASH_A, SREG(28) & 3);
    REPORT_OPTION(AT_REPORT_BACKSLASH_C, at_c_values[SREG(24) & 3]);
    send_conf_to_dte(AT_REPORT_BACKSLASH_F, SREG(46), 3);
    REPORT_OPTION(AT_REPORT_BACKSLASH_G, (SREG(49) & 0x10) >> 4);
    if (SREG(24) & S24_FIXED_DTE) {
        REPORT_OPTION(AT_REPORT_BACKSLASH_J, 0);
    } else {
        REPORT_OPTION(AT_REPORT_BACKSLASH_J, 1);
    }
    REPORT_OPTION(AT_REPORT_BACKSLASH_K,
                  at_k_values[(SREG(39) & S39_BREAK) >> 4]);
    if ((SREG(27) & S27_Q) > 0) {
        REPORT_OPTION(AT_REPORT_BACKSLASH_N, 1);
    } else {
        REPORT_OPTION(AT_REPORT_BACKSLASH_N, at_link_values[SREG(36) & 15]);
    }
    if (!(SREG(24) & S24_W1) && (SREG(24) & S24_W2)) {
        REPORT_OPTION(AT_REPORT_BACKSLASH_V, 1);
    } else {
        REPORT_OPTION(AT_REPORT_BACKSLASH_V, 0);
    }
    if (saved) {
        send_conf_to_dte(AT_REPORT_AT_C, SREG(56), 3);
    }
    send_dc_cr_lf();
    line_length = 0;
    if (saved) {
        registers = at_saved_registers;
    } else {
        registers = at_active_registers;
    }
    while (*registers != REGISTER_LIST_END) {
        if (*registers >= 100) {
            width = 3;
        } else {
            width = 2;
        }
        convert_value_to_ascii(*registers, 10, width, number);
        out = label;
        digit = number;
        *out++ = 'S';
        while (width--) {
            *out++ = *digit++;
        }
        *out++ = '=';
        *out = 0;
        send_conf_to_dte(label, Sregisters[*registers++], 3);
    }
}

void send_list_of_dial_string(Uint8 first, Uint8 last)
{
    Uint16 offset;
    Uint16 end;
    Uint8 byte;
    char number[2];
    Uint8 slot;
    send_dc_cr_lf();
    send_message(AT_PHONE_NUMBERS_HEADING);
    send_dc_cr_lf();
    for (slot = first; slot <= last; ++slot) {
        eeprom_read_data((slot * EE_ENTRY_BYTES) + EE_DIRECTORY, EE_ENTRY_BYTES,
                         (Uint8 *)&offset);
        eeprom_read_data(((slot + 1) * EE_ENTRY_BYTES) + EE_DIRECTORY,
                         EE_ENTRY_BYTES, (Uint8 *)&end);
        send_message(AT_PHONE_NUMBER_PREFIX);
        convert_value_to_ascii(slot, 10, 1, number);
        send_message(number);
        send_message(at_assignment_label);
        while (offset < end) {
            eeprom_read_data(offset++, 1, &byte);
            message_byte_to_dte(byte);
        }
        /* @hack redundant: the continue that ends the loop's body */
        continue; /* NOLINT(readability-redundant-control-flow) */
    }
}

void send_conf_to_dte(char *name, Uint8 value, Uint8 width)
{
    char number[11];
    char *digit;
    if (line_length > REPORT_LAST_COLUMN) {
        line_length = 0;
        send_dc_cr_lf();
    }
    while (*name != 0) {
        message_byte_to_dte(*name++);
        line_length++;
    }
    convert_value_to_ascii(value, 10, width, number);
    digit = number;
    while (*digit != 0) {
        message_byte_to_dte(*digit++);
        line_length++;
    }
    if (line_length <= REPORT_LAST_COLUMN) {
        message_byte_to_dte(' ');
        line_length++;
    }
}

void send_dc_cr_lf(void)
{
    message_byte_to_dte(S3_THEN);
    message_byte_to_dte(S4_MASK);
}

static void at_save_profile(Uint8 profile)
{
    if (profile < EEPROM_PROFILES) {
        write_profile_to_eeprom(profile, Sregisters);
    } else {
        at_com_result = AT_ERROR;
    }
}

static void at_percent_command(void)
{
    Uint8 suffix;
    Uint8 command;
    Uint8 value;
    advance_ptr_and_eliminate_space();
    command = ptr_CMD_ALT[0];
    suffix = ptr_CMD_ALT[1];
    advance_ptr_and_eliminate_space();
    value = read_number();
    switch (command) {
    case 'C':
        if (value > 1) {
            at_com_result = AT_ERROR;
        } else if (value == 0) {
            SREG(24) &= 0xfb;
        } else {
            SREG(24) |= 4;
            SREG(42) |= 0xc0;
        }
        break;
    case 'H':
        if (cur_cnt->reset_flags & REFUSE_PTT_TESTS) {
            at_com_result = AT_ERROR;
            return;
        }
        if (suffix == 'L') {
            country_parameter();
            return;
        }
        command_off_hook();
        if (value <= 15) {
            dp_set_dtmf(cur_cnt->tone_level, cur_cnt->tone_table);
            dp_transmit_PTT(value, (SREG(23) & S23_GUARD) >> 6,
                            cur_cnt->tone_level);
        } else {
            if (value >= 56) {
                scm_set_PTT_homologation_phase();
            }
            dp_transmit_PTT(value, (SREG(23) & S23_GUARD) >> 6, SREG(35));
        }
        break;
    case 'L':
        report_line(value);
        break;
    default:
        at_com_result = AT_ERROR;
        break;
    }
}

static void country_parameter(void)
{
    char number[11];
    Uint8 index;
    Uint8 value;
    Uint8 *record;
    advance_ptr_and_eliminate_space();
    index = read_number();
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: it refuses an index at or past
     * the record's size */
    if (index >= sizeof(*cur_cnt)) {
        at_com_result = AT_ERROR;
        return;
    }
#else
    /* @bug memory: takes an index of the record's size, whose byte lies past
     * the country's record. the test at 0x0caed508 passes it, the ? below
     * reads that byte and the = writes it */
    if (index > sizeof(*cur_cnt)) {
        at_com_result = AT_ERROR;
        return;
    }
#endif
    record = (Uint8 *)cur_cnt;
    if (*ptr_CMD_ALT == '?') {
        advance_ptr_and_eliminate_space();
        convert_value_to_ascii(record[index], 16, 2, number);
        send_cr_lf();
        send_message(number);
        send_cr_lf();
    } else if (*ptr_CMD_ALT == '=') {
        advance_ptr_and_eliminate_space();
        value = 0;
        do {
            value <<= 4;
            value += at_hex_digit(*ptr_CMD_ALT);
            advance_ptr_and_eliminate_space();
        } while (*ptr_CMD_ALT != S3_THEN);
        record[index] = value;
    } else {
        at_com_result = AT_ERROR;
    }
}

static const char at_assignment_label[4] = {
#include "modem/atcom/at_assignment_label.inc"
};
static const char number_separator[4] = {
#include "modem/atcom/number_separator.inc"
};
static const char modulation_label[20] = {
#include "modem/atcom/modulation_label.inc"
};
static const char bell_103[12] = {
#include "modem/atcom/bell_103.inc"
};
static const char bell_1200[12] = {
#include "modem/atcom/bell_1200.inc"
};
static const char v21_300[8] = {
#include "modem/atcom/v21_300.inc"
};
static const char v23_low_tx[16] = {
#include "modem/atcom/v23_low_tx.inc"
};
static const char v23_high_tx[16] = {
#include "modem/atcom/v23_high_tx.inc"
};
static const char v22_1200[12] = {
#include "modem/atcom/v22_1200.inc"
};
static const char v22_600[8] = {
#include "modem/atcom/v22_600.inc"
};
static const char v22bis_1200[12] = {
#include "modem/atcom/v22bis_1200.inc"
};
static const char v22bis_2400[12] = {
#include "modem/atcom/v22bis_2400.inc"
};
static const char v32_4800[12] = {
#include "modem/atcom/v32_4800.inc"
};
static const char v32_9600_coded[28] = {
#include "modem/atcom/v32_9600_coded.inc"
};
static const char v32_9600_uncoded[20] = {
#include "modem/atcom/v32_9600_uncoded.inc"
};
static const char v32bis_4800[12] = {
#include "modem/atcom/v32bis_4800.inc"
};
static const char v32bis_7200[12] = {
#include "modem/atcom/v32bis_7200.inc"
};
static const char v32bis_9600[12] = {
#include "modem/atcom/v32bis_9600.inc"
};
static const char v32bis_12000[16] = {
#include "modem/atcom/v32bis_12000.inc"
};
static const char v32bis_14400[16] = {
#include "modem/atcom/v32bis_14400.inc"
};
static const char v32terbo_16800[16] = {
#include "modem/atcom/v32terbo_16800.inc"
};
static const char v32terbo_19200[16] = {
#include "modem/atcom/v32terbo_19200.inc"
};
static const char v34[8] = {
#include "modem/atcom/v34.inc"
};
static const char k56[8] = {
#include "modem/atcom/k56.inc"
};
static const char v90[8] = {
#include "modem/atcom/v90.inc"
};
static const char unknown_modulation[20] = {
#include "modem/atcom/unknown_modulation.inc"
};
static const char rate_format[4] = {
#include "modem/atcom/rate_format.inc"
};
static const char receive_level[20] = {
#include "modem/atcom/receive_level.inc"
};
static const char receive_error[36] = {
#include "modem/atcom/receive_error.inc"
};
static const char round_trip[20] = {
#include "modem/atcom/round_trip.inc"
};
static const char milliseconds[4] = {
#include "modem/atcom/milliseconds.inc"
};
static const char near_echo[20] = {
#include "modem/atcom/near_echo.inc"
};
static const char dbm[8] = {
#include "modem/atcom/dbm.inc"
};
static const char far_echo[20] = {
#include "modem/atcom/far_echo.inc"
};
static const char termination_reason[32] = {
#include "modem/atcom/termination_reason.inc"
};
static const char local_request[16] = {
#include "modem/atcom/local_request.inc"
};
static const char link_disconnect[16] = {
#include "modem/atcom/link_disconnect.inc"
};
static const char last_tx[32] = {
#include "modem/atcom/last_tx.inc"
};
static const char highest_tx[32] = {
#include "modem/atcom/highest_tx.inc"
};
static const char last_rx[32] = {
#include "modem/atcom/last_rx.inc"
};
static const char highest_rx[32] = {
#include "modem/atcom/highest_rx.inc"
};
static const char error_protocol[32] = {
#include "modem/atcom/error_protocol.inc"
};
static const char lapm[8] = {
#include "modem/atcom/lapm.inc"
};
static const char mnp[4] = {
#include "modem/atcom/mnp.inc"
};
static const char no_protocol[8] = {
#include "modem/atcom/no_protocol.inc"
};
static const char compression[32] = {
#include "modem/atcom/compression.inc"
};
static const char v42bis_label[8] = {
#include "modem/atcom/v42bis_label.inc"
};
static const char line_quality[32] = {
#include "modem/atcom/line_quality.inc"
};

/* the prompts for a new key and for its retyping */
static const char enter_key_prompt[16] = {
#include "modem/atcom/enter_key_prompt.inc"
};
static const char verify_key_prompt[28] = {
#include "modem/atcom/verify_key_prompt.inc"
};

static void v25_result(Uint8 result);

/* %L's reports. REPORT_DIAGNOSTIC is the factory's, whose next letter picks
 * the pump's RAM or the console's memory */
enum line_report {
    REPORT_RECEIVE_LEVEL = 0,
    REPORT_SIGNAL = 1,
    REPORT_HISTORY = 2,
    REPORT_DIAGNOSTIC = 9
};

#ifdef NONMATCHING
#else
/* the factory's memory diagnostic, %L9Y. the hex digits after Y give an
 * address, less its low bit. =VALUE stores the 32-bit VALUE there. otherwise
 * the command writes the word there to the terminal, or as many words as
 * the two hex digits after a T give, eight to a line */
static void memory_diagnostic(void)
{
    /* @hack type: the address as a volatile union of a number and a
     * pointer, where a Uint32 cast at each use would do */
    volatile union {
        Uint32 value;
        Uint32 *pointer;
    } address;
    char number[11];
    Uint32 value;
    Uint8 count;
    Uint8 i;

    send_cr_lf();
    send_cr_lf();
    address.value = 0;
    count = 1;
    advance_ptr_and_eliminate_space();
    do {
        address.value <<= 4;
        address.value += at_hex_digit(*ptr_CMD_ALT);
        advance_ptr_and_eliminate_space();
    } while (*ptr_CMD_ALT != S3_THEN && *ptr_CMD_ALT != '=' &&
             *ptr_CMD_ALT != 'T');
    address.value &= ~1;
    if (*ptr_CMD_ALT != '=') {
        if (*ptr_CMD_ALT == 'T') {
            advance_ptr_and_eliminate_space();
            count = at_hex_digit(*ptr_CMD_ALT);
            advance_ptr_and_eliminate_space();
            if (*ptr_CMD_ALT != S3_THEN) {
                count <<= 4;
                count += at_hex_digit(*ptr_CMD_ALT);
                advance_ptr_and_eliminate_space();
            }
        }
        if (count == 255) {
            count--;
        }
        for (i = 1; i <= count; i++) {
            convert_value_to_ascii(*address.pointer, 16, 8, number);
            send_message(number_separator);
            send_message(number);
            address.pointer++;
            /* @hack redundant: the test of i against 0, where the loop
             * starts it at 1 */
            if (i > 0 && i % 8 == 0) {
                send_cr_lf();
            }
        }
    } else {
        value = 0;
        advance_ptr_and_eliminate_space();
        do {
            value <<= 4;
            value += at_hex_digit(*ptr_CMD_ALT);
            advance_ptr_and_eliminate_space();
        } while (*ptr_CMD_ALT != S3_THEN);
        *address.pointer = value;
    }
    send_cr_lf();
}
#endif

static void report_line(Uint8 mode)
{
    char number[11];
    char bit_rate[12];
    Uint8 tx_rate;
    Uint8 rx_rate;
    Uint32 modulation;
#ifdef NONMATCHING
#else
    Uint16 address;
#endif
    /* @hack type: show_rate, a flag, held in a char and compared with 1,
     * where a Uint8 tested as it is would do */
    char show_rate;
    if (mode == REPORT_DIAGNOSTIC) {
#ifdef NONMATCHING
        /* this side refuses the factory's diagnostics, which read the pump's
         * RAM, and read and write the console's memory at any address */
        at_com_result = AT_ERROR;
#else
        if (*ptr_CMD_ALT == 'Y') {
            memory_diagnostic();
        } else if (*ptr_CMD_ALT == 'X') {
            send_cr_lf();
            advance_ptr_and_eliminate_space();
            address = at_hex_digit(*ptr_CMD_ALT) << 12;
            advance_ptr_and_eliminate_space();
            address += at_hex_digit(*ptr_CMD_ALT) << 8;
            advance_ptr_and_eliminate_space();
            address += at_hex_digit(*ptr_CMD_ALT) << 4;
            advance_ptr_and_eliminate_space();
            address += at_hex_digit(*ptr_CMD_ALT);
            advance_ptr_and_eliminate_space();
            convert_value_to_ascii(dp_read_ram_location(address), 16, 8,
                                   number);
            send_message(number);
        } else {
            at_com_result = AT_ERROR;
        }
#endif
    } else if ((BMR(6) & BMR6_ON_LINE) && mode == REPORT_RECEIVE_LEVEL) {
        send_cr_lf();
        convert_value_to_ascii(dp_read_rec_level(), 10, 3, number);
        send_message(number);
        send_cr_lf();
    } else if ((BMR(6) & BMR6_ON_LINE) && mode == REPORT_SIGNAL) {
        show_rate = 0;
        send_cr_lf();
        dp_get_modulation_type(&modulation, &tx_rate, &rx_rate);
        send_message(modulation_label);
        switch (modulation & ~MOD_TRELLIS) {
        case MOD_BELL:
            if (rx_rate == 2) {
                send_message(bell_103);
            } else {
                send_message(bell_1200);
            }
            break;
        case MOD_V21:
            send_message(v21_300);
            break;
        case MOD_V23:
            /* @bug logic: a call sending at 75 bps reports V23 1200TX/75RX.
             * the test at 0x0caed888 wants RATE_300, but V.23 sends at RATE_75
             * or RATE_1200 */
            if (tx_rate == RATE_300) {
                send_message(v23_low_tx);
            } else {
                send_message(v23_high_tx);
            }
            break;
        case MOD_V22:
            if (rx_rate == 4) {
                send_message(v22_1200);
            } else if (rx_rate == 3) {
                send_message(v22_600);
            }
            break;
        case MOD_V22BIS:
            if (rx_rate == 4) {
                send_message(v22bis_1200);
            } else {
                send_message(v22bis_2400);
            }
            break;
        case MOD_V32:
            if (rx_rate == 6) {
                send_message(v32_4800);
            } else if (rx_rate == 8 && (modulation & MOD_TRELLIS)) {
                send_message(v32_9600_coded);
            } else {
                send_message(v32_9600_uncoded);
            }
            break;
        case MOD_V32BIS:
            if (rx_rate == 6) {
                send_message(v32bis_4800);
            } else if (rx_rate == 7) {
                send_message(v32bis_7200);
            } else if (rx_rate == 8) {
                send_message(v32bis_9600);
            } else if (rx_rate == 11) {
                send_message(v32bis_12000);
            } else if (rx_rate == 12) {
                send_message(v32bis_14400);
            } else if (rx_rate == 13) {
                send_message(v32terbo_16800);
            } else if (rx_rate == 14) {
                send_message(v32terbo_19200);
            }
            break;
        case MOD_V34:
            send_message(v34);
            show_rate = 1;
            break;
        case MOD_K56:
            send_message(k56);
            show_rate = 1;
            break;
        case MOD_V90:
            send_message(v90);
            show_rate = 1;
            break;
        default:
            send_message(unknown_modulation);
            break;
        }
        if (show_rate == 1) {
            sprintf(bit_rate, rate_format,
                    (int)ConvertTCISpeedToBps(lestConvertDpRate(rx_rate)));
            send_message(bit_rate);
        }
        send_cr_lf();
        send_message(receive_level);
        convert_value_to_ascii(dp_read_rec_level(), 10, 3, number);
        send_message(number);
        send_cr_lf();
        if (modulation != MOD_V21 && modulation != MOD_V23 &&
            modulation != MOD_BELL) {
            /* @hack redundant: an empty block that never runs */
            if (0) {
            }
            send_message(receive_error);
            convert_value_to_ascii(dp_read_eqm(), 16, 4, number);
            send_message(number);
            send_cr_lf();
            if (modulation == MOD_V32 || modulation == MOD_V32BIS ||
                modulation == MOD_V34 || modulation == MOD_K56 ||
                modulation == MOD_V90) {
                send_message(round_trip);
                convert_value_to_ascii(dp_read_round_trip_delay(), 10, 4,
                                       number);
                send_message(number);
                send_message(milliseconds);
                send_cr_lf();
                send_message(near_echo);
                convert_value_to_ascii(dp_read_near_echo_level(), 10, 3,
                                       number);
                send_message(number);
                send_message(dbm);
                send_cr_lf();
                send_message(far_echo);
                convert_value_to_ascii(dp_read_far_echo_level(), 10, 3, number);
                send_message(number);
                send_message(dbm);
                send_cr_lf();
            }
        }
        send_cr_lf();
    } else if (mode == REPORT_HISTORY && !no_connection_yet) {
        send_cr_lf();
        send_message(termination_reason);
        if (SREG(45) == S45_LOCAL_REQUEST || (BMR(6) & BMR6_ON_LINE)) {
            send_message(local_request);
        } else {
            send_message(link_disconnect);
        }
        send_cr_lf();
        send_message(last_tx);
        send_bps_message(last_connection_tx_speed);
        send_cr_lf();
        send_message(highest_tx);
        send_bps_message(max_connection_tx_speed);
        send_cr_lf();
        send_message(last_rx);
        send_bps_message(last_connection_rx_speed);
        send_cr_lf();
        send_message(highest_rx);
        send_bps_message(max_connection_rx_speed);
        send_cr_lf();
        send_message(error_protocol);
        if (Last_BMR27 & BMR27_LAPM) {
            send_message(lapm);
        } else if (Last_BMR29 & BMR29_MNP) {
            send_message(mnp);
        } else {
            send_message(no_protocol);
        }
        send_cr_lf();
        send_message(compression);
        if (Last_BMR29 & BMR29_COMPRESSION) {
            send_message(v42bis_label);
        } else {
            send_message(no_protocol);
        }
        send_cr_lf();
        send_message(line_quality);
        convert_value_to_ascii(dp_read_eqm() >> 8, 10, 3, number);
        send_message(number);
        send_cr_lf();
    } else {
        at_com_result = AT_ERROR;
    }
}

static void set_error_control(Uint8 mode);

static Uint8 at_hex_digit(Uint8 digit)
{
    if (digit >= '0' && digit <= '9') {
        digit -= '0';
    } else if (digit >= 'A' && digit <= 'F') {
        digit -= 'A' - 10;
    } else {
        at_com_result = AT_ERROR;
    }
    return digit;
}

static void at_backslash_command(void)
{
    Uint8 command;
    Uint8 value;
    advance_ptr_and_eliminate_space();
    command = *ptr_CMD_ALT;
    advance_ptr_and_eliminate_space();
    value = read_number();
    switch (command) {
    case 'N':
        if (!(BMR(6) & BMR6_ON_LINE)) {
            set_error_control(value);
        } else {
            at_com_result = AT_ERROR;
        }
        break;
    default:
        at_com_result = AT_ERROR;
        break;
    }
}

static void set_error_control(Uint8 mode)
{
    SREG(27) &= ~S27_Q;
    if (mode >= 8) {
        at_com_result = AT_ERROR;
    } else if (mode == 0) {
        SREG(36) = 0x22;
    } else if (mode == 1) {
        SREG(36) = 0x11;
    } else if (mode == 2 || mode == 3) {
        at_com_result = AT_ERROR;
    } else {
        SREG(36) = (mode << 1) + (mode << 5);
    }
}

static void at_tilde_command(void)
{
    Uint8 command;
    advance_ptr_and_eliminate_space();
    command = *ptr_CMD_ALT;
    advance_ptr_and_eliminate_space();
    /* @hack redundant: the switch, whose one arm sets AT_ERROR */
    switch (command) {
    default:
        at_com_result = AT_ERROR;
        break;
    }
}

static void at_dollar_command(void)
{
    Uint8 command;
    advance_ptr_and_eliminate_space();
    command = *ptr_CMD_ALT;
    advance_ptr_and_eliminate_space();
    if (command != 'K') {
        read_number();
    }
    /* @hack redundant: the switch, whose one arm sets AT_ERROR */
    switch (command) {
    default:
        at_com_result = AT_ERROR;
        break;
    }
}

/* end a key's interrogation, with AT_OK where success is set and AT_ERROR
 * otherwise. the library exports the call, though no source makes it */
void at_finish_key(char success) /* NOLINT(misc-use-internal-linkage) */
{
    CMD_BUF[2] = S3_THEN;
    interrogation.phase = 0;
    if (success) {
        at_com_result = AT_OK;
    } else {
        at_com_result = AT_ERROR;
    }
}

static void at_star_command(void)
{
    Uint8 command;
    advance_ptr_and_eliminate_space();
    command = *ptr_CMD_ALT;
    advance_ptr_and_eliminate_space();
    if (command != 'D') {
        read_number();
    }
    /* @hack redundant: the switch, whose one arm sets AT_ERROR */
    switch (command) {
    default:
        at_com_result = AT_ERROR;
        break;
    }
}

void hangup_voice_call(Uint8 mode)
{
}

static void at_paren_command(void)
{
    Uint8 command;
    advance_ptr_and_eliminate_space();
    command = *ptr_CMD_ALT;
    advance_ptr_and_eliminate_space();
    read_number();
    /* @hack redundant: the switch, whose one arm sets AT_ERROR */
    switch (command) {
    default:
        at_com_result = AT_ERROR;
        break;
    }
}

static void at_country_command(void)
{
    char number[11];
    Uint8 command;
    Uint8 selection;
    Uint16 country;
    char first;
    advance_ptr_and_eliminate_space();
    command = *ptr_CMD_ALT;
    advance_ptr_and_eliminate_space();
    selection = read_number();
    switch (command) {
    case 'C':
        if (*ptr_CMD_ALT == '?') {
            advance_ptr_and_eliminate_space();
            send_cr_lf();
            convert_value_to_ascii(SREG(56), 10, 1, number);
            send_message(number);
            send_cr_lf();
        } else if (*ptr_CMD_ALT == '=') {
            ptr_CMD_ALT++;
            if (*ptr_CMD_ALT == '?') {
                advance_ptr_and_eliminate_space();
                set_dte_for_message();
                send_cr_lf();
                send_message(AT_OPEN_PAREN);
                first = 1;
                for (country = 0; country < 255; country++) {
                    if (country_CheckCountry(country)) {
                        if (first) {
                            first = 0;
                        } else {
                            send_message(AT_COMMA);
                        }
                        convert_value_to_ascii(country, 10, 1, number);
                        send_message(number);
                    }
                }
                send_message(AT_CLOSE_PAREN);
                send_cr_lf();
            } else {
                at_com_result = AT_ERROR;
            }
        } else if (country_CheckCountry(selection)) {
            eedrvSaveCountryCode(selection);
            country_InitCountry(selection);
            country_CheckAllSregDefaults(Sregisters);
            SREG(56) = selection;
        } else {
            at_com_result = AT_ERROR;
        }
        break;
    default:
        at_com_result = AT_ERROR;
        break;
    }
}

static void at_hash_command(void)
{
    ptr_CMD_ALT++;
    at_com_result = AT_ERROR;
}

static void at_v25_command(void)
{
    v25_result(v25terReadCommand(&ptr_CMD_ALT));
}

static void v25_result(Uint8 result)
{
    if (result == V25TER_SILENT && *ptr_CMD_ALT == S3_THEN) {
        at_com_result = AT_SILENT;
    } else if (result == V25TER_WAIT_LINE) {
        at_com_result = AT_WAIT_LINE;
    } else if (result == V25TER_ERROR) {
        at_com_result = AT_ERROR;
    } else if (result == V25TER_WAIT_COMMAND) {
        at_com_result = AT_WAIT_COMMAND;
    } else if (*ptr_CMD_ALT == ';') {
        ptr_CMD_ALT++;
    }
}

static void at_bang_command(void)
{
    Uint8 *next = ptr_CMD_ALT + 1;
    Uint8 command;
    if (S3_THEN != ' ') {
        while (*next == ' ') {
            next++;
        }
    }
    if (*next == S3_THEN) {
        at_register_command(last_s_used);
        return;
    }
    advance_ptr_and_eliminate_space();
    command = *ptr_CMD_ALT;
    advance_ptr_and_eliminate_space();
    read_number();
    /* @hack redundant: the switch, whose one arm sets AT_ERROR */
    switch (command) {
    case '+':
    default:
        at_com_result = AT_ERROR;
        break;
    }
}

Uint8 read_number(void)
{
    Uint8 value = 0;
    while (*ptr_CMD_ALT >= '0' && *ptr_CMD_ALT <= '9') {
        value = (value * 10) + *ptr_CMD_ALT - '0';
        advance_ptr_and_eliminate_space();
    }
    return value;
}

static void at_skip_separators(void)
{
    while ((*ptr_CMD_ALT == ',' || *ptr_CMD_ALT <= ' ' || *ptr_CMD_ALT > '~') &&
           *ptr_CMD_ALT != S3_THEN) {
        ptr_CMD_ALT++;
    }
}

void advance_ptr_and_eliminate_space(void)
{
    ptr_CMD_ALT++;
    if (S3_THEN != ' ') {
        while (*ptr_CMD_ALT == ' ') {
            ptr_CMD_ALT++;
        }
    }
}

char isphonedigit(Uint8 digit)
{
    return (digit >= '0' && digit <= '9') || (digit >= 'A' && digit <= 'D') ||
           digit == '*' || digit == '#';
}

static char at_line_available(void)
{
    Uint8 *next = ptr_CMD_ALT;
    while (*next == ' ') {
        next++;
    }
    if (*next == SREG(3) && (cur_cnt->reset_flags & TAKE_OVER_LINE)) {
        if (!li_get_lcs()) {
            at_com_result = AT_ERROR;
            return 0;
        }
    } else if (!(BMR(9) & BMR9_OFF_HOOK) && li_get_lcs()) {
        send_coded_message(RESULT_CIRCUIT_BUSY, NULL);
        at_com_result = AT_SILENT;
        return 0;
    }
    return 1;
}

char copy_dial_string(Uint8 *destination, Uint8 *source)
{
    Uint8 length = 0;
    while (*source != S3_THEN && *source != 0) {
        *destination++ = *source++;
        if (++length > MODEM_DIAL_LENGTH) {
            return 0;
        }
    }
    *destination = S3_THEN;
    return 1;
}

static void at_prepare_command(void)
{
    Uint8 command;
    at_com_result = AT_WAIT_COMMAND;
    if (BMR(1) & 0x40) {
        if (timer_expired(at_command_timer)) {
            dp_start_sending_clear_down();
            maintain_phase = MAINTAIN_CLEAR_DOWN;
        }
        return;
    }
    if (BMR(5) & 0x10) {
        if (vxd_dte_tx_buff_empty(0) || timer_expired(at_command_timer)) {
            at_reset_profile(at_pending_profile);
            at_com_result = AT_OK;
        }
        return;
    }
    if (!(BMR(7) & BMR7_COMMAND_LINE)) {
        return;
    }
    if (BMR(11) & 2) {
        if (((BMR(7) & BMR7_HOST_BYTE) && !(BMR(4) & 0x10) &&
             !(BMR(11) & BMR11_LEASED_LINE) &&
             ((SREG(14) & S14_ORIGINATE) || !(SREG(31) & 8))) ||
            !target_get_TD()) {
            BMR(11) &= 0xfd;
            BMR(5) &= 0xfe;
            at_com_result = AT_OK;
            return;
        } else if (!timer_expired(OFF_HOOK_TIM) || SREG(1)) {
            return;
        }
        BMR(11) &= 0xfd;
        if (!(BMR(5) & 1)) {
            command = 0;
            while (command != 'A' && command != 'D') {
#ifdef NONMATCHING
                /* this side fails where no A or D lies before the cursor, where
                 * the matching side's scan passes the buffer's start */
                if (ptr_CMD_ALT == CMD_BUF) {
                    at_com_result = AT_ERROR;
                    return;
                }
#endif
                command = *--ptr_CMD_ALT;
            }
            at_com_result = AT_CONTINUE;
            return;
        }
    }
    if (BMR(5) & 1) {
        if (BMR(5) & 2) {
            if (get_ring_state()) {
                return;
            }
            at_command_answer();
        } else if (BMR(5) & 4) {
            at_command_hang_up();
        } else if (BMR(5) & 8) {
            at_command_dial();
        }
        return;
    }
    if (BMR(16) & BMR16_RESUME_LINE) {
        BMR(16) &= ~BMR16_RESUME_LINE;
        at_com_result = AT_CONTINUE;
        return;
    }
    if (timer_expired(CMD_ECHO_TIM)) {
        if (CMD_B_PT > MODEM_COMMAND_LENGTH) {
            at_com_result = AT_ERROR;
            return;
        }
        ptr_CMD_ALT = CMD_BUF;
        BMR(7) &= ~BMR7_HOST_BYTE;
        if (ptr_CMD_ALT[1] != 'T' && ptr_CMD_ALT[1] != '/') {
            at_com_result = AT_SILENT;
            return;
        }
        /* @hack spelling: two increments, where ptr_CMD_ALT += 2 would do */
        ptr_CMD_ALT++;
        ptr_CMD_ALT++;
        at_com_result = AT_CONTINUE;
        set_dte_for_message();
    }
}

static void at_finish_command(Uint8 mode)
{
    switch (at_com_result) {
    case AT_OK:
        send_coded_message(RESULT_OK, NULL);
        break;
    case AT_ERROR:
        send_coded_message(RESULT_ERROR, NULL);
        SS41 = 0;
        break;
    case AT_NO_CARRIER:
        send_coded_message(RESULT_NO_CARRIER, NULL);
        SS41 = 0;
        break;
    }
    if (at_com_result != AT_WAIT_LINE && at_com_result != AT_WAIT_COMMAND) {
        BMR(6) &= ~BMR6_RUNNING_COMMAND;
        S3_THEN = SREG(3);
        v25terUpdateV25terPar();
        update_OIS();
        cmd_OIS();
        set_command_mode();
    }
    if (at_com_result != AT_WAIT_COMMAND) {
        if (SREG(27) != s27_copy || SREG(31) != s31_copy) {
            s27_copy = SREG(27);
            s31_copy = SREG(31);
            if (!(BMR(6) & BMR6_DATA) && at_com_result != AT_WAIT_LINE) {
                set_command_mode();
            }
        }
        if (SREG(31) & 0x10) {
            TCI_got_format(fixed_dte_speed, fixed_dte_format);
            if (!(BMR(6) & BMR6_DATA) && at_com_result != AT_WAIT_LINE) {
                set_command_mode();
            }
        }
        CMD_B_PT = 0;
        BMR(7) &= ~BMR7_COMMAND_LINE;
        BMR(7) &= ~BMR7_HOST_BYTE;
    }
    at_com_result = AT_CONTINUE;
}

static char at_dial_restricted(Uint8 *number)
{
    Uint8 skip = 0;
    Uint8 restriction;
    if (*number >= 'I' && *number <= 'Z') {
        skip = 1;
    }
    restriction = BL_Checking(number + skip);
    if (restriction == BL_BARRED || restriction == BL_DELAYED) {
        if (!(SREG(14) & S14_VERBOSE)) {
            send_message(AT_RESULT_29);
            message_byte_to_dte(SREG(3));
        } else if (restriction == BL_DELAYED) {
            BL_Dial_Delay();
        } else {
            send_cr_lf();
            send_message(AT_BLACKLISTED_TEXT);
            send_cr_lf();
        }
        return 1;
    }
    return 0;
}

void atcomResetBlackList(void)
{
    BL_Condition(BL_EVENT_CONNECTED);
}
