#include <sg_xpt.h>

#include "atcom_ex.h"
#include "messages.h"

static Uint8 reserved_work[84];

static char parameter_digit(char character, Uint8 kind);
static char parameter_alnum(char character);
static Uint32 parameter_number(char *text, Uint8 kind);

Uint8 read_parameter_fv(Uint32 *value, char **cursor, char kind)
{
    char *text;
    char digits[20];
    Uint8 count;
    Uint8 result;

    /* @hack redundant: the test of cursor, *cursor and value, whose return
     * only GCC's build keeps */
    if (!cursor || !*cursor || !value) {
        /* @bug memory: the image goes on past its test of cursor, *cursor and
         * value for null, at 0x0caf765c to 0x0caf7674, so it reads through a
         * null: *cursor at 0x0caf7678, and **cursor at 0x0caf767c */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: a null cursor, *cursor or
         * value returns AT_FIELD_MISSING */
        return AT_FIELD_MISSING;
#endif
    }
    text = *cursor;
    while (*text == ' ') {
        text++;
    }
    if (parameter_digit(*text, kind)) {
        for (count = 0; parameter_digit(*text, kind) && count < 8; count++) {
            digits[count] = *text++;
        }
        digits[count] = 0;
        while (*text == ' ') {
            text++;
        }
        if (*text == ',') {
            text++;
        }
        *value = parameter_number(digits, kind);
        result = AT_FIELD_VALUE;
    } else if (*text == ',') {
        text++;
        result = AT_FIELD_EMPTY;
    } else {
        result = AT_FIELD_MISSING;
    }
    *cursor = text;
    return result;
}

Uint8 read_string(char *buffer, char **cursor, Uint8 width)
{
    char *text;
    Uint8 count;
    Uint8 result;

    /* @hack redundant: the test of cursor, *cursor and buffer, whose return
     * only GCC's build keeps */
    if (!cursor || !*cursor || !buffer) {
        /* @bug memory: the image goes on past its test of cursor, *cursor and
         * buffer for null, at 0x0caf7712 to 0x0caf7726, so it reads through a
         * null: *cursor at 0x0caf772a, and **cursor at 0x0caf772e */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: a null cursor, *cursor or
         * buffer returns AT_FIELD_MISSING */
        return AT_FIELD_MISSING;
#endif
    }
    text = *cursor;
    while (*text == ' ') {
        text++;
    }
    if (parameter_alnum(*text)) {
        for (count = 0; count < width; count++) {
            if (parameter_alnum(*text)) {
                *buffer = *text++;
            } else {
                *buffer = ' ';
            }
            buffer++;
        }
        *buffer = 0;
        while (*text == ' ') {
            text++;
        }
        if (*text == ',') {
            text++;
        }
        result = AT_FIELD_VALUE;
    } else if (*text == ',') {
        text++;
        result = AT_FIELD_EMPTY;
    } else {
        result = AT_FIELD_MISSING;
    }
    *cursor = text;
    return result;
}

static char parameter_digit(char character, Uint8 kind)
{
    if ((kind == 'd' || kind == 'z' || kind == 'l' || kind == 'n') &&
        character >= '0' && character <= '9') {
        return 1;
    } else if ((kind == 'h' || kind == 'm' || kind == 's') &&
               ((character >= '0' && character <= '9') ||
                (character >= 'A' && character <= 'F'))) {
        return 1;
    }
    return 0;
}

static char parameter_alnum(char character)
{
    if ((character >= 'A' && character <= 'Z') ||
        (character >= 'a' && character <= 'z') ||
        (character >= '0' && character <= '9')) {
        return 1;
    }
    return 0;
}

static Uint32 parameter_number(char *text, Uint8 kind)
{
    Uint32 value;
    Uint32 shifted;

    while (*text == ' ' || *text == '\t') {
        text++;
    }
    if (kind == 'd' || kind == 'z' || kind == 'l' || kind == 'n') {
        value = 0;
        while (parameter_digit(*text, 'd')) {
            value = (value * 10) + *text++ - '0';
        }
        /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
    } else if (kind == 'h' || kind == 'm' || kind == 's') {
        value = 0;
        while (parameter_digit(*text, 'h')) {
            shifted = value * 16;
            if (parameter_digit(*text, 'd')) {
                value = shifted + *text++ - '0';
            } else {
                value = shifted + *text++ - 'A' + 10;
            }
        }
    } else {
        send_message("invalid type of number ");
        /* @bug uninitialized: any other kind returns value unset
         * (0x0caf790a). read_parameter_fv calls parameter_number only after
         * parameter_digit takes a digit, which it does for no other kind */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: any other kind returns
         * kind */
        return kind;
#endif
    }
    return value;
}
