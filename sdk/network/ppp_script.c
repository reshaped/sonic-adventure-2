#include <access/aveppp.h>
#include <machine.h>
#include <sg_xpt.h>
#include <string.h>
#include "ppp_driver.h"
#include "ppp_fsm.h"
#include "ppp_link.h"
#include "ppp_system.h"
#ifdef NONMATCHING
/* this side's overflow tests below take SHRT_MAX and LONG_MAX from here */
#include <limits.h>
#endif
#ifdef DREAMCAST
#else
#include "ppp_script.h"
#endif

/* the commands, each spelled COMMAND_SPELLINGS ways in command_names */
enum { SCRIPT_DIAL, SCRIPT_SEND, SCRIPT_WAIT, SCRIPT_DELAY, SCRIPT_COMMANDS };
enum { COMMAND_SPELLINGS = 2 };
/* the macros, in macro_names' order, and the substitutions they stand for */
enum {
    MACRO_TELEPHONE_NUMBER,
    MACRO_LOGIN,
    MACRO_PASSWORD,
    MACRO_SIGNUP,
    MACRO_PERCENT,
    MACROS
};
enum { STRING_ARG, NUMBER_ARG };
enum { MAX_ARGUMENTS = 4, SCRIPT_BYTES = 256 };
enum { CONNECT_WINDOW_BYTES = 64, MS_PER_SECOND = 1000 };
struct command_arguments {
    int count;
    int kind[MAX_ARGUMENTS];
};
static const struct command_arguments argument_types[] = {
    {1, {STRING_ARG}},
    {1, {STRING_ARG}},
    {2, {STRING_ARG, NUMBER_ARG}},
    {1, {NUMBER_ARG}}};
#ifdef DREAMCAST
static Uint8 pull_word[4], pull_byte;
#endif
/* each macro's substitution, in an array of macro_names' size */
static const char *substitutions[MACROS + 1];
long PP_ConnectRate;
/* read_exact gives up wait_duration milliseconds after wait_began, and at
 * once where wait_duration is 0 */
static unsigned long wait_began, wait_duration;
struct ppp_statistics _ppp_info;
/* the line's arguments, each a text in strings or a number in numbers as
 * argument_types has it. one the line doesn't give is NULL */
/* @hack type: each argument a void *, where a union of a text and a number
 * would do */
static void *arguments[MAX_ARGUMENTS];
static char strings[MAX_ARGUMENTS][SCRIPT_BYTES];
static short numbers[MAX_ARGUMENTS];
#ifdef DREAMCAST
/* SHC places an uninitialized variable where it first meets its name.
 * ppp_script.h names PP_ConnectRate and _ppp_info, so it follows them */
#include "ppp_script.h"
#endif
/* each command's name in capitals and then in lowercase, in SCRIPT_DIAL's
 * order, then NULL */
static const char *command_names[(COMMAND_SPELLINGS * SCRIPT_COMMANDS) + 1];
/* the macros a text may hold, then NULL */
static const char *macro_names[MACROS + 1];
static short read_exact(Uint8 *bytes, short length);
static void flush_input(void);
static short next_line(const Uint8 *source, Uint8 *line, const Uint8 **next);
static short next_word(const Uint8 *source, Uint8 *word, const Uint8 **next);
static short spaces(const Uint8 *source);
static short command_number(const Uint8 *name);
static void parse_argument(int command, int argument, const Uint8 *text,
                           int *string_count, int *number_count);
static short dial(const AvepppTelephoneParam *telephone);
static short send_command(void);
static short wait_command(const AvepppTelephoneParam *telephone);
static short delay(const AvepppTelephoneParam *telephone);
static short wait_string(const Uint8 *text, short length, short timeout);
static short wait_connect(short timeout);

short PP_GoScript(const Uint8 *script, AvepppOpenParam *parameters)
{
    Uint8 line[SCRIPT_BYTES];
    Uint8 word[SCRIPT_BYTES];
    const Uint8 *line_at;
    const Uint8 *word_at;
    int string_count;
    int number_count;
    int argument;
    int command;
    short result;
    /* @hack spelling: parameters read through a copy, options, where
     * parameters itself would do */
    AvepppOpenParam *options = parameters;
#ifdef NONMATCHING
    /* this side refuses a missing script, options or telephone, which the
     * matching side reads through */
    if (!script || !options || !options->telephone) {
        return AvepppErrorScript;
    }
#endif
    substitutions[MACRO_TELEPHONE_NUMBER] = options->telephone_number;
    substitutions[MACRO_LOGIN] = options->login;
    substitutions[MACRO_PASSWORD] = options->password;
    substitutions[MACRO_SIGNUP] = options->signup;
    substitutions[MACRO_PERCENT] = "%";
    flush_input();
    line_at = script;
    for (;;) {
        arguments[0] = arguments[1] = arguments[2] = arguments[3] = NULL;
#ifdef NONMATCHING
        /* this side fails a script with a line too long for line, for which
         * next_line returns -1 */
        result = next_line(line_at, line, &line_at);
        if (result < 0) {
            return AvepppErrorScript;
        }
        if (!result) {
            break;
        }
#else
        if (!next_line(line_at, line, &line_at)) {
            break;
        }
#endif
        word_at = line;
        if (!next_word(line, word, &word_at)) {
            continue;
        }
        if ((command = command_number(word)) == -1) {
            flush_input();
            return AvepppErrorScript;
        }
        string_count = 0;
        number_count = 0;
        for (argument = 0; argument < argument_types[command].count;
             argument++) {
            if (!next_word(word_at, word, &word_at)) {
                break;
            }
            parse_argument(command, argument, word, &string_count,
                           &number_count);
        }
#ifdef NONMATCHING
        /* this side fails a command but SCRIPT_DELAY that has no first
         * argument, which the commands below read */
        if (command != SCRIPT_DELAY && !arguments[0]) {
            return AvepppErrorScript;
        }
#endif
        switch (command) {
        case SCRIPT_DIAL:
            result = dial(options->telephone);
            break;
        case SCRIPT_SEND:
            result = send_command();
            break;
        case SCRIPT_WAIT:
            result = wait_command(options->telephone);
            break;
        case SCRIPT_DELAY:
            result = delay(options->telephone);
            break;
        default:
            result = AvepppErrorScript;
            break;
        }
        if (result) {
            flush_input();
            return result;
        }
    }
    flush_input();
    return 0;
}
static short read_exact(Uint8 *bytes, short length)
{
    Uint16 available;
    /* @hack type: an array of one, where a char would do */
    char byte[1];
    int i = 0;
    while (i < length) {
        PP_direct_pk_stat(&available);
        if (available && !PP_direct_pk_recv(byte, 1)) {
            bytes[i++] = *byte;
            continue;
        }
        if (PPP_CloseRequest || !wait_duration ||
            PP_GetTimeLow() - wait_began > wait_duration) {
            wait_duration = 0;
            return -1;
        }
        PP_OS_CriticalUnLock();
        PP_OS_Yield();
        PP_OS_CriticalLock();
    }
    return 0;
}
static void flush_input(void)
{
    Uint16 count;
    /* @hack type: an array of one, where a char would do */
    char byte[1];
    do {
        if (PP_direct_pk_stat(&count) || !count) {
            break;
        }
    } while (!PP_direct_pk_recv(byte, 1));
}
static short next_line(const Uint8 *source, Uint8 *line, const Uint8 **next)
{
    int c;
#ifdef NONMATCHING
    /* the last byte line has room for before its NUL, where this side's copy
     * below stops */
    const Uint8 *end = line + SCRIPT_BYTES - 1;
#endif
    if (!*source) {
        return 0;
    }
    while ((c = *source++) != 0) {
        if (c == '\n') {
            source++;
            break;
        }
#ifdef NONMATCHING
        /* this side fails a line too long for line */
        if (line == end) {
            return -1;
        }
#endif
        *line++ = c;
    }
    source--;
    *line = 0;
    *next = source;
    return 1;
}
static short next_word(const Uint8 *source, Uint8 *word, const Uint8 **next)
{
    int c;
    int escaped;
    int digits;
    int value;
    source += spaces(source);
    if (!*source) {
        return 0;
    }
    if (*source == '"') {
        escaped = 0;
        digits = 0;
        value = 0;
        source++;
        while ((c = *source++) != 0 && c != '\n') {
            if (digits) {
                if (c >= '0' && c <= '9') {
                    value = (value * 16) + c - '0';
                    if (!--digits) {
#ifdef NONMATCHING
                        /* this side repairs the bug tagged below: a \x escape
                         * writes the byte its two digits make */
                        *word++ = value;
#else
                        /* @bug logic: a \x escape writes its second digit,
                         * not the byte its two digits make, here and below
                         * (0x0c9e221e) */
                        *word++ = c;
#endif
                    }
                    continue;
                }
                if (c >= 'a' && c <= 'f') {
#ifdef NONMATCHING
                    /* this side gives a letter its value, the plain form the
                     * hack tag below gives, for the store that repairs the bug
                     * tagged above */
                    value = (value * 16) + c - 'a' + 10;
#else
                    /* @hack spelling: a letter's value less '0' - 10, where
                     * 'a' - 10 would do: the stores write c, and no code
                     * reads value */
                    value = (value * 16) + c - ('0' - 10);
#endif
                    if (!--digits) {
#ifdef NONMATCHING
                        /* this side repairs the bug tagged above for a last
                         * digit that is a letter */
                        *word++ = value;
#else
                        *word++ = c;
#endif
                    }
                    continue;
                } else {
                    digits = 0;
                    continue;
                }
            } else if (escaped) {
                if (c == 'r') {
                    c = '\r';
                    *word++ = c;
                } else if (c == 'n') {
                    c = '\n';
                    *word++ = c;
                } else if (c == '\\') {
                    c = '\\';
                    *word++ = c;
                } else if (c == '"') {
                    c = '"';
                    *word++ = c;
                } else if (c == 'x') {
                    digits = 2;
                    value = 0;
                }
                escaped = 0;
                continue;
            } else if (c == '\\') {
                escaped = 1;
                continue;
            } else if (c == '"') {
                source++;
                break;
            }
            *word++ = c;
        }
    } else {
        while ((c = *source++) != 0 && c != '\n') {
            if (c == ' ' || c == ',' || c == '"') {
                break;
            }
            *word++ = c;
        }
    }
    source--;
    *word = 0;
    source += spaces(source);
    if (*source == ',') {
        source++;
    }
    *next = source;
    return 1;
}
static short spaces(const Uint8 *source)
{
    int count = 0;
    while (*source++ == ' ') {
        count++;
    }
    return count;
}
static short command_number(const Uint8 *name)
{
    int i = 0;
    while (command_names[i]) {
#ifdef NONMATCHING
        /* this side calls the C library's strcmp. machine.h gives GCC SHC's
         * _builtin_strcmp on the console alone */
        if (!strcmp((const char *)name, command_names[i])) {
#else
        if (!_builtin_strcmp((const char *)name, command_names[i])) {
#endif
            break;
        }
        i++;
    }
    if (!command_names[i]) {
        return -1;
    }
    return i / COMMAND_SPELLINGS;
}
static void parse_argument(int command, int argument, const Uint8 *text,
                           int *string_count, int *number_count)
{
    int i;
    int value;
    int index;
    short *slot;
    if (argument_types[command].kind[argument] == STRING_ARG) {
        index = *string_count;
#ifdef NONMATCHING
        /* this side calls the C library's strcpy. machine.h gives GCC SHC's
         * _builtin_strcpy on the console alone */
        strcpy(strings[index], (const char *)text);
#else
        _builtin_strcpy(strings[index], (const char *)text);
#endif
        arguments[argument] = strings[index];
        index++;
        *string_count = index;
    }
    if (argument_types[command].kind[argument] == NUMBER_ARG) {
        index = *number_count;
        for (i = 0, value = 0; text[i]; i++) {
#ifdef NONMATCHING
            /* this side takes 0 for an argument that isn't all digits, or whose
             * value would pass SHRT_MAX, which the short it lands in can't
             * hold */
            if (text[i] < '0' || text[i] > '9' ||
                value > (SHRT_MAX - (text[i] - '0')) / 10) {
                value = 0;
                break;
            }
#endif
            value *= 10;
            value += text[i] - '0';
        }
        slot = &numbers[index++];
        *slot = value;
        arguments[argument] = slot;
        *number_count = index;
    }
}
#ifdef NONMATCHING
/* this side repairs the bug tagged below: a substitution goes in at length,
 * right after the bytes before it */

/* append source to text's length bytes with each macro expanded, and end it
 * with a NUL. the result is text's new length. it is -1 where a macro's
 * substitution is NULL, or where the text would pass SCRIPT_BYTES */
static int append_expanded(char text[SCRIPT_BYTES], int length,
                           const char *source)
{
    const char *value;
    size_t used;
    int i;
    while (*source) {
        for (i = 0; macro_names[i]; i++) {
            if (!strncmp(source, macro_names[i], strlen(macro_names[i]))) {
                break;
            }
        }
        if (!macro_names[i]) {
            if (length >= SCRIPT_BYTES - 1) {
                return -1;
            }
            text[length++] = *source++;
        } else {
            value = substitutions[i];
            if (!value) {
                return -1;
            }
            used = strlen(value);
            if (used >= (size_t)(SCRIPT_BYTES - length)) {
                return -1;
            }
            memcpy(text + length, value, used);
            length += (int)used;
            source += strlen(macro_names[i]);
        }
    }
    text[length] = 0;
    return length;
}
#else
/* one step of expanding arguments[0] into text. it appends the substitution
 * of the macro at position, or the byte there where none starts. length and
 * position move past what the step writes and reads. each command runs the
 * steps in a loop of its own */
/* @bug uninitialized: a macro after a copied byte lands at the first zero
 * byte that follows in text, past bytes no write set. the copy writes no NUL
 * for strcat to find (0x0c9e2446, 0x0c9e255e, 0x0c9e265a) */
#define EXPAND_MACRO(text, length, position, match)                            \
    for ((match) = 0; macro_names[match]; (match)++) {                         \
        if (!strncmp(macro_names[match], (char *)arguments[0] + (position),    \
                     strlen(macro_names[match]))) {                            \
            break;                                                             \
        }                                                                      \
    }                                                                          \
    if (!macro_names[match]) {                                                 \
        (text)[length] = ((char *)arguments[0])[position];                     \
        (length)++;                                                            \
        (position)++;                                                          \
    } else {                                                                   \
        strcat((text) + (length), substitutions[match]);                       \
        (length) += strlen(substitutions[match]);                              \
        (position) += strlen(macro_names[match]);                              \
    }
#endif
static short dial(const AvepppTelephoneParam *telephone)
{
    char text[SCRIPT_BYTES];
    int length;
#ifdef NONMATCHING
    /* this side builds the command by append_expanded, which fails where the
     * command passes SCRIPT_BYTES, and refuses a missing or overlong outside
     * number. the matching side writes past text for either */
    strcpy(text, telephone->dialtype == 0 ? "ATDT" : "ATDP");
    if (telephone->outside_line) {
        if (!telephone->outside_number || strlen(telephone->outside_number) >
                                              SCRIPT_BYTES - sizeof("ATDT\r")) {
            return AvepppErrorScript;
        }
        strcat(text, telephone->outside_number);
    }
    length = append_expanded(text, strlen(text), arguments[0]);
    if (length < 0 ||
        (length == SCRIPT_BYTES - 1 && text[length - 1] != '\r')) {
        return AvepppErrorScript;
    }
#else
    int i;
    int match;
    text[0] = 0;
    if (telephone->dialtype == 0) {
        _builtin_strcpy(text, "ATDT");
    } else {
        _builtin_strcpy(text, "ATDP");
    }
    if (telephone->outside_line) {
        strcat(text, telephone->outside_number);
    }
    length = strlen(text);
    i = 0;
    while (((char *)arguments[0])[i]) {
        EXPAND_MACRO(text, length, i, match)
    }
#endif
    if (length > 0 && text[length - 1] != '\r') {
        text[length++] = '\r';
    }
    text[length] = 0;
    flush_input();
    if (PP_direct_pk_send(text, strlen(text))) {
        return AvepppErrorScript;
    }
    return wait_connect(telephone->timeout);
}
static short send_command(void)
{
    char text[SCRIPT_BYTES];
    int length;
#ifdef NONMATCHING
    /* this side expands the argument by append_expanded, which fails where the
     * text passes SCRIPT_BYTES. the matching side's EXPAND_MACRO writes past
     * text */
    length = append_expanded(text, 0, arguments[0]);
    if (length < 0) {
        return AvepppErrorScript;
    }
#else
    int i;
    int match;
    text[0] = 0;
    i = length = 0;
    while (((char *)arguments[0])[i]) {
        EXPAND_MACRO(text, length, i, match)
    }
#endif
    text[length] = 0;
    flush_input();
    if (PP_direct_pk_send(text, strlen(text))) {
        return AvepppErrorScript;
    }
    return 0;
}
static short wait_command(const AvepppTelephoneParam *telephone)
{
    char text[SCRIPT_BYTES];
    int length;
    short timeout;
    short *argument;
#ifdef NONMATCHING
    /* this side expands the argument by append_expanded, which fails where the
     * text passes SCRIPT_BYTES. the matching side's EXPAND_MACRO writes past
     * text */
    length = append_expanded(text, 0, arguments[0]);
    if (length < 0) {
        return AvepppErrorScript;
    }
#else
    int i;
    int match;
    text[0] = 0;
    length = 0;
    i = 0;
    while (((char *)arguments[0])[i]) {
        EXPAND_MACRO(text, length, i, match)
    }
#endif
    text[length] = 0;
    if (!arguments[1]) {
        timeout = telephone->timeout;
    } else {
        argument = arguments[1];
        timeout = *argument;
    }
    if (wait_string((Uint8 *)text, strlen(text), timeout)) {
        return AvepppErrorScript;
    }
    return 0;
}
static short delay(const AvepppTelephoneParam *telephone)
{
    unsigned long started;
    unsigned long duration;
    short result;
    started = PP_GetTimeLow();
    result = 0;
    if (!arguments[0]) {
        duration = telephone->timeout * MS_PER_SECOND;
    } else {
        duration = *(short *)arguments[0] * MS_PER_SECOND;
    }
    for (;;) {
        if (PPP_CloseRequest) {
            result = AvepppErrorScript;
            break;
        }
        if (PP_GetTimeLow() - started > duration) {
            break;
        }
        PP_OS_CriticalUnLock();
        PP_OS_Yield();
        PP_OS_CriticalLock();
    }
    return result;
}
static short wait_string(const Uint8 *text, short length, short timeout)
{
    Uint8 window[SCRIPT_BYTES];
    wait_began = PP_GetTimeLow();
    wait_duration = timeout * MS_PER_SECOND;
    if ((unsigned long)length > sizeof(window)) {
        length = sizeof(window);
    }
    if (read_exact(window, length)) {
        return -1;
    }
    /* @hack type: the comparison narrowed to a short, where its int would do */
    while ((short)PP_strnicmp(window, text, length)) {
        memmove(window, window + 1, length - 1);
        if (read_exact(window + length - 1, 1)) {
            return -1;
        }
    }
    return 0;
}
/* whether the last bytes of window, an array, are word's, matched in either
 * case */
/* @hack type: the comparison narrowed to a short, where its int would do */
#define ENDS_WITH(window, word)                                                \
    (!(short)PP_strnicmp((window) + (sizeof(window) - (sizeof(word) - 1)),     \
                         (const Uint8 *)(word), sizeof(word) - 1))
static short wait_connect(short timeout)
{
    Uint8 window[CONNECT_WINDOW_BYTES];
    Uint8 *position;
    int i;
    int c;
    wait_began = PP_GetTimeLow();
    wait_duration = timeout * MS_PER_SECOND;
    memset(window, 0, sizeof(window));
    for (;;) {
        memmove(window, window + 1, sizeof(window) - 1);
        if (read_exact(window + (sizeof(window) - 1), 1)) {
            return AvepppErrorScript;
        }
        if (ENDS_WITH(window, "CONNECT")) {
            break;
        }
        if (ENDS_WITH(window, "BUSY")) {
            return AvepppErrorBusy;
        }
        if (ENDS_WITH(window, "ERROR")) {
            return AvepppErrorBusy;
        }
        if (ENDS_WITH(window, "DELAYED")) {
            return AvepppErrorBusy;
        }
        if (ENDS_WITH(window, "NO CARRIER")) {
            return AvepppErrorNoDialTone;
        }
        if (ENDS_WITH(window, "NO DIALTONE")) {
            return AvepppErrorNoDialTone;
        }
    }
    PP_ConnectRate = 0;
    for (i = 0; (unsigned int)i < sizeof(window) - 1; i++) {
        if (read_exact(window + i, 1)) {
            return AvepppErrorScript;
        }
        if (window[i] == '\r') {
            break;
        }
    }
    window[i] = 0;
    position = window + spaces(window);
    for (;;) {
        c = *position++;
        if (c < '0' || c > '9') {
            break;
        }
#ifdef NONMATCHING
        /* this side fails a rate that would pass LONG_MAX */
        if (PP_ConnectRate > (LONG_MAX - (c - '0')) / 10) {
            return AvepppErrorScript;
        }
#endif
        PP_ConnectRate = (PP_ConnectRate * 10) + c - '0';
    }
    return 0;
}
void get_ppp_info(void)
{
    struct ppp_driver_statistics driver;
    short i;
    short status;
    status = PP_GetPPPStatus(&driver);
    if (status) {
        return;
    }
    for (i = 0; i < (short)sizeof(_ppp_info.local_ip); i++) {
        _ppp_info.local_ip[i] = driver.local_ip[i];
        _ppp_info.peer_ip[i] = driver.peer_ip[i];
        _ppp_info.local_dns1[i] = driver.local_dns1[i];
        _ppp_info.local_dns2[i] = driver.local_dns2[i];
        _ppp_info.peer_dns1[i] = driver.peer_dns1[i];
        _ppp_info.peer_dns2[i] = driver.peer_dns2[i];
    }
    _ppp_info.octets_sent = driver.octets_sent;
    _ppp_info.packets_sent = driver.packets_sent;
    for (i = 0; i < (short)PPP_PROTOCOLS; i++) {
        _ppp_info.control_sent[i] = driver.control_sent[i];
        _ppp_info.control_received[i] = driver.control_received[i];
    }
    _ppp_info.send_status = driver.send_status;
    _ppp_info.receive_status = driver.receive_status;
    _ppp_info.octets_received = driver.octets_received;
    _ppp_info.packets_received = driver.packets_received;
    _ppp_info.unknown = driver.unknown;
    _ppp_info.discarded = driver.discarded;
    _ppp_info.overflow = driver.overflow;
    _ppp_info.receive_errors = driver.receive_errors;
    _ppp_info.receive_allocation_errors = driver.receive_allocation_errors;
}
static const char *command_names[(COMMAND_SPELLINGS * SCRIPT_COMMANDS) + 1] = {
    "DIAL", "dial", "SEND", "send", "WAIT", "wait", "DELAY", "delay", NULL};
static const char *macro_names[MACROS + 1] = {"%t", "%u", "%p",
                                              "%i", "%%", NULL};
