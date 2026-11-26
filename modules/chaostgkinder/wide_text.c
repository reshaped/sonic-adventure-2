#include <sg_xpt.h>
#include <stdarg.h>

#include "chaostgkinder/wide_text.h"

/* the least byte unpacking reads as the high byte of a two-byte code */
#define FIRST_LEAD 0x7f

Sint32 kinder_wide_length(const Uint16 *text)
{
    const Uint16 *end = text;
    while (*end) {
        end++;
    }
    return end - text;
}

void kinder_wide_copy(Uint16 *dst, const Uint16 *src)
{
    while ((*dst++ = *src++) != 0) {
    }
}

void kinder_wide_copy_n(Uint16 *dst, const Uint16 *src, Sint32 count)
{
    while (--count >= 0 && (*dst++ = *src++) != 0) {
    }
}

void kinder_wide_terminate(Uint16 *dst, const Uint16 *src, Sint32 count)
{
    while (--count >= 0 && (*dst++ = *src++) != 0) {
    }
    *dst = 0;
}

void kinder_text_unpack(Uint16 *dst, const char *src)
{
    Sint32 code;
    do {
        code = (Uint8)*src++;
        if (code >= FIRST_LEAD) {
            code = (code << 8) | (Uint8)*src++;
        }
        *dst++ = code;
    } while (code);
}

void kinder_text_unpack_n(Uint16 *dst, const char *src, Sint32 count)
{
    Sint32 code;
    while (--count >= 0 && (code = (Uint8)*src++) != 0) {
        if (code >= FIRST_LEAD) {
            code = (code << 8) | (Uint8)*src++;
        }
        *dst++ = code;
    }
    *dst = 0;
}

void kinder_text_pack(const Uint16 *src, char *dst)
{
    Sint32 code;
    do {
        code = *src++;
        if (code >> 8) {
            *dst++ = code >> 8;
        }
        *dst++ = code;
    } while (code);
}

void kinder_text_pack_n(const Uint16 *src, char *dst, Sint32 count)
{
    Sint32 code;
    while (--count >= 0 && (code = *src++) != 0) {
        if (code >> 8) {
            *dst++ = code >> 8;
        }
        *dst++ = code;
    }
    *dst = 0;
}

void kinder_int_to_text(Uint16 *text, Sint32 value)
{
    Sint32 digits;
    if (value < 0) {
        *text++ = '-';
        /* @bug arithmetic: writes the digits of the most negative Sint32 as
         * codes below '0'. negating it overflows, and each remainder is then
         * negative (0x0c9d095c, 0x0c9d0994) */
        value = -value;
    }
    if (!value) {
        *text++ = '0';
        *text = 0;
    } else {
        digits = value;
        while (digits) {
            digits /= 10;
            text++;
        }
        *text = 0;
        while (value) {
            *--text = (value % 10) + '0';
            value /= 10;
        }
    }
}

Sint32 kinder_text_to_int(const Uint16 *text)
{
    Sint32 value;
    Bool negative;
    Sint32 code;
    /* @hack spelling: the chained assignment of 0, where initializers would
     * do */
    negative = value = 0;
    if (*text == '-') {
        negative = TRUE;
        text++;
    }
    /* @hack redundant: the test for a 0 code, which the digit test fails
     * anyway */
    while ((code = *text++) != 0 && code >= '0' && code <= '9') {
        value = (value * 10) + code - '0';
    }
    if (negative) {
        value = -value;
    }
    return value;
}

void kinder_text_format(Uint16 *dst, const Uint16 *format, ...)
{
    /* @hack spelling: cursor and output, copies of format and dst, where the
     * parameters could step themselves */
    const Uint16 *cursor = format;
    Uint16 *output = dst;
    va_list args;
    Sint32 code;
    Sint32 character;
    const char *bytes;
    const Uint16 *wide;
    va_start(args, format);
    do {
        if ((code = *cursor++) != '%') {
            *output++ = code;
        } else {
            switch (*cursor++) {
            case '%':
                *output++ = '%';
                break;
            case 's':
                bytes = va_arg(args, char *);
                while ((character = *bytes++) != 0) {
                    *output++ = character;
                }
                break;
            case 'S':
                wide = va_arg(args, Uint16 *);
                while ((character = *wide++) != 0) {
                    *output++ = character;
                }
                break;
            }
        }
    } while (code);
    va_end(args);
}
