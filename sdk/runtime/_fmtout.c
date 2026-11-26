#include <ctype.h>
#include <errno.h>
#include <machine.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* the largest width or precision a directive takes */
#define LIMIT 512

/* where _fmtout's text goes, and count, the bytes written there so far */
struct output_cursor {
    int mode;
    void *destination;
    int count;
    int (*put)(int c, FILE *stream);
};

static void integer_text(long value, char *text, int conversion,
                         struct format_parts *parts);
static void emit_text(const char *text, int length,
                      struct output_cursor *output);
static void terminate_text(struct output_cursor *output);
static int output_error(struct output_cursor *output);

int _fmtout(int mode, int (*put)(int c, FILE *stream), void *destination,
            const char *format, va_list arguments)
{
    struct output_cursor output_storage;
    struct format_parts parts_storage;
    char buffer[128];
    char *buffer_start;
    int length;
    char zero;
    char *text;
    const char *end;
    int *count_target;
    int digit;
    int emitted;
    int segment;
    int fpscr;
    /* @hack spelling: output and parts reached through pointers, where the
     * two structs' own names would do */
    struct output_cursor *output = &output_storage;
    struct format_parts *parts = &parts_storage;

#ifdef NONMATCHING
    /* this side keeps the shipped result. GCC can't see that a store to text
     * and length comes before every read. %n and an invalid conversion store
     * neither, and write no text */
    text = buffer;
    length = 0;
#endif
    output->mode = PRINT_TO_STREAM;
    output->destination = NULL;
    output->count = 0;
    parts->flags = 0;
    parts->fill = 0;
    parts->unk_2 = 0;
    parts->width = 0;
    parts->precision = 0;
    parts->prefix_end = NULL;
    parts->digits = NULL;
    parts->suffix = NULL;
    parts->prefix_zeros = 0;
    parts->digit_zeros = 0;
    parts->suffix_zeros = 0;
    parts->padding = 0;
    if (mode == PRINT_TO_STREAM) {
        if (destination == NULL) {
            errno = PTRERR;
            return -1;
        }
        if (!(((FILE *)destination)->_ioflag1 & (_IOREAD | _IOWRITE | _IORW))) {
            errno = NOTOPN;
            return -1;
        }
        if (!(((FILE *)destination)->_ioflag1 & _IORW) &&
            (((FILE *)destination)->_ioflag1 & _IOREAD)) {
            errno = EBADF;
            return -1;
        }
        /* @hack redundant: a store of put to mode that never runs */
        if (0) {
            mode = (int)put;
        }
        output->put = put;
    }
    output->mode = mode;
    output->destination = destination;
    output->count = 0;
    parts->flags = 0;
    zero = '0';
    if (output_error(output)) {
        return -1;
    }
    /* @hack spelling: the %c and %% stores through buffer_start, a copy of
     * buffer's address, where buffer[0] would do */
    buffer_start = &buffer[0];
    /* @bug memory: reads the byte after the format's null, where the null cuts
     * a directive short, as after a trailing %. the step past the directive at
     * 0x8c0c96c0 steps past the null. this test then reads the byte after it at
     * 0x8c0c96e6, before it tests PRINT_STOP */
#ifdef NONMATCHING
    /* this side repairs the tag above: it tests PRINT_STOP first. every
     * directive that the format's null cuts short sets it */
    while (!(parts->flags & PRINT_STOP) && *format && !output_error(output)) {
#else
    while (*format && !output_error(output) && !(parts->flags & PRINT_STOP)) {
#endif
        if (*format == '%') {
            format++;
            parts->flags = 0;
            parts->prefix_end = buffer;
            parts->digits = buffer;
            parts->suffix = NULL;
            parts->prefix_zeros = 0;
            parts->digit_zeros = 0;
            parts->suffix_zeros = 0;
            parts->padding = 0;
            while (*format == '-' || *format == '+' || *format == ' ' ||
                   *format == '#') {
                switch (*format) {
                case '-':
                    parts->flags |= PRINT_LEFT;
                    break;
                case '+':
                    parts->flags |= PRINT_PLUS;
                    break;
                case ' ':
                    if (!(parts->flags & PRINT_PLUS)) {
                        parts->flags |= PRINT_SPACE;
                    }
                    break;
                case '#':
                    parts->flags |= PRINT_ALTERNATE;
                    break;
                }
                format++;
            }
            parts->fill = ' ';
            if (*format == '0') {
                parts->fill = '0';
                format++;
            }
            parts->width = 0;
            if (*format == '*') {
                parts->width = va_arg(arguments, int);
                if (parts->width < 0) {
                    parts->flags |= PRINT_LEFT;
                    /* @bug arithmetic: an INT_MIN width stays negative after
                     * NEG R3,R3 at 0x8c0c9156, and the signed test of the
                     * limit, CMP/GT R2,R1 at 0x8c0c915e, passes it */
                    parts->width = -(unsigned int)parts->width;
                }
                if (parts->width > LIMIT) {
                    parts->flags |= PRINT_STOP;
                    errno = ECSPEC;
                }
                format++;
            }
            if (isdigit(*format)) {
                parts->width = 0;
                while (isdigit(*format)) {
                    digit = *format - '0';
                    if ((LIMIT - digit) / 10 >= parts->width) {
                        parts->width = (parts->width * 10) + digit;
                    } else {
                        parts->flags |= PRINT_STOP;
                        errno = ECSPEC;
                    }
                    format++;
                }
            }
            parts->precision = -1;
            if (*format == '.') {
                if (*++format == '*') {
                    parts->precision = va_arg(arguments, int);
                    if (parts->precision < 0) {
                        parts->precision = -1;
                    }
                    if (parts->precision > LIMIT) {
                        parts->flags |= PRINT_STOP;
                        errno = ECSPEC;
                    }
                    format++;
                }
                if (isdigit(*format)) {
                    parts->precision = 0;
                    while (isdigit(*format)) {
                        digit = *format - '0';
                        if ((LIMIT - digit) / 10 >= parts->precision) {
                            parts->precision = (parts->precision * 10) + digit;
                        } else {
                            parts->flags |= PRINT_STOP;
                            errno = ECSPEC;
                        }
                        format++;
                    }
                }
            }
            parts->length = ' ';
            if (*format == 'h' || *format == 'l' || *format == 'L') {
                parts->length = (int)*format;
                format++;
            }
            switch (*format) {
            case 'd':
            case 'i':
            case 'o':
            case 'u':
            case 'x':
            case 'X': {
                long value;

                if (parts->length == 'l') {
                    value = va_arg(arguments, long);
                } else if (parts->length == 'h') {
                    if (*format == 'u' || *format == 'X' || *format == 'x' ||
                        *format == 'o') {
                        value = (unsigned short)va_arg(arguments, int);
                    } else {
                        value = (short)va_arg(arguments, int);
                    }
                } else {
                    if (*format == 'u' || *format == 'X' || *format == 'x' ||
                        *format == 'o') {
                        value = va_arg(arguments, unsigned int);
                    } else {
                        value = va_arg(arguments, int);
                    }
                }
                if (parts->precision == -1) {
                    parts->precision = 1;
                }
                integer_text(value, buffer, *format, parts);
                text = buffer;
                length = strlen(text);
                break;
            }
            case 'f':
            case 'e':
            case 'E':
            case 'g':
            case 'G':
                _xtostr(&arguments, buffer, *format, parts);
                /* @hack spelling: three statements through the local fpscr,
                 * where set_fpscr(get_fpscr() & ~FPSCR_PR) would do */
                fpscr = get_fpscr();
                fpscr = fpscr & ~FPSCR_PR;
                set_fpscr(fpscr);
                text = buffer;
                length = strlen(text);
                break;
            case 'c':
                *buffer_start = va_arg(arguments, int);
                text = buffer;
                length = 1;
                /* @hack redundant: digits and digit_zeros stored again, here
                 * and for %s and %%, though the directive's start set both */
                parts->digits = buffer;
                parts->digit_zeros = 0;
                /* the subtraction is unsigned, so an INT_MIN width wraps to
                 * INT_MAX as SHC's code does. %s and %% subtract alike */
                parts->padding = (int)((unsigned int)parts->width - 1);
                break;
            case 's':
                text = va_arg(arguments, char *);
                length = strlen(text);
                if (parts->precision != -1 && parts->precision < length) {
                    length = parts->precision;
                }
                parts->digits = buffer;
                parts->digit_zeros = 0;
                parts->padding = (int)((unsigned int)parts->width - length);
                break;
            case 'p': {
                long value = (long)va_arg(arguments, void *);

                if (parts->precision == -1) {
                    parts->precision = 1;
                }
                integer_text(value, buffer, 'x', parts);
                text = buffer;
                length = strlen(text);
                break;
            }
            case 'n':
                /* @hack spelling: the count stored through count_target,
                 * where a store through the fetched pointer itself would do */
                count_target = va_arg(arguments, int *);
                *count_target = output->count;
                break;
            case '%':
                *buffer_start = '%';
                text = buffer;
                length = 1;
                parts->digits = buffer;
                parts->digit_zeros = 0;
                parts->padding = (int)((unsigned int)parts->width - 1);
                break;
            default:
                errno = ECSPEC;
                parts->flags |= PRINT_STOP;
                break;
            }
            if (*format != 'n' && !(parts->flags & PRINT_STOP)) {
                /* @hack redundant: padding tested ahead of the loop, here and
                 * after the text, where the loop's own test would do */
                if (!(parts->flags & PRINT_LEFT) && parts->padding > 0) {
                    while (parts->padding > 0) {
                        emit_text(&parts->fill, 1, output);
                        parts->padding--;
                    }
                }
                if (parts->prefix_zeros == 0 && parts->digit_zeros == 0 &&
                    parts->suffix_zeros == 0) {
                    emit_text(text, length, output);
                } else {
                    segment = parts->prefix_end - text;
                    emitted = segment;
                    if (segment > 0) {
                        emit_text(text, segment, output);
                    }
                    while (parts->prefix_zeros > 0) {
                        emit_text(&zero, 1, output);
                        parts->prefix_zeros--;
                    }
                    segment = parts->digits - parts->prefix_end;
                    emitted += segment;
                    if (segment > 0) {
                        emit_text(parts->prefix_end, segment, output);
                    }
                    while (parts->digit_zeros > 0) {
                        emit_text(&zero, 1, output);
                        parts->digit_zeros--;
                    }
                    emit_text(parts->digits, length - emitted, output);
                    while (parts->suffix_zeros > 0) {
                        emit_text(&zero, 1, output);
                        parts->suffix_zeros--;
                    }
                }
                if ((parts->flags & PRINT_LEFT) && parts->padding > 0) {
                    while (parts->padding > 0) {
                        emit_text(&parts->fill, 1, output);
                        parts->padding--;
                    }
                }
            }
            format++;
        } else {
            for (end = format; *end != '\0' && *end != '%'; end++) {
            }
            emit_text(format, end - format, output);
            format = end;
        }
    }
    terminate_text(output);
    return output->count;
}

/* write value's text to text: its sign, space or 0x, its digits and a null.
 * conversion is an integer conversion's letter, or 'x' for %p. it sets
 * parts' digits, digit_zeros and padding for the text */
static void integer_text(long value, char *text, int conversion,
                         struct format_parts *parts)
{
    char *start;
    int digits;
    int total;
    int effective;
    int radix;
    unsigned long magnitude;
    char reversed[12];
    int i;
    int pending;

    start = text;
    /* @hack spelling: the null stored through start, where text, which holds
     * the same address, would do */
    *start = '\0';
#ifdef NONMATCHING
    /* this side keeps the shipped result. GCC can't see that the switch sets
     * radix on every path. each call passes a letter the switch takes */
    radix = 10;
#endif
    switch (conversion) {
    case 'd':
    case 'i':
    case 'u':
        radix = 10;
        break;
    case 'o':
        radix = 8;
        break;
    case 'x':
    case 'X':
        radix = 16;
        break;
    }
    if ((conversion == 'd' || conversion == 'i') && value < 0) {
        magnitude = -(unsigned long)value;
    } else {
        magnitude = value;
    }
    digits = 0;
    total = 0;
    if (magnitude == 0 && parts->precision != 0) {
        reversed[0] = '0';
        digits = 1;
    } else {
        while (magnitude > 0) {
            reversed[digits] = magnitude % radix;
            if (reversed[digits] <= 9) {
                reversed[digits] += '0';
            } else if (conversion == 'x') {
                reversed[digits] += 'a' - 10;
            } else {
                reversed[digits] += 'A' - 10;
            }
            /* @hack spelling: magnitude = magnitude / radix, where
             * magnitude /= radix would do */
            magnitude = magnitude / radix;
            digits++;
        }
    }
    if (parts->flags & PRINT_ALTERNATE) {
        switch (conversion) {
        case 'o':
            if (value != 0) {
                reversed[digits++] = '0';
            }
            break;
        case 'x':
        case 'X':
            if (value != 0) {
                *text++ = '0';
                *text++ = conversion;
                total = 2;
            }
            break;
        }
    }
    /* @hack spelling: pending, a copy of digits for the loop below, where
     * digits itself would do */
    pending = digits;
    total += digits;
    if ((conversion == 'd' || conversion == 'i') &&
        (parts->precision != 0 || value != 0)) {
        if (value >= 0) {
            if (parts->flags & PRINT_PLUS) {
                total++;
                *text++ = '+';
            } else if (parts->flags & PRINT_SPACE) {
                total++;
                *text++ = ' ';
            }
        } else {
            total++;
            *text++ = '-';
        }
    }
    if (*start == '+' || *start == '-' || *start == ' ') {
        parts->digits = start + 1;
    } else if (parts->flags & PRINT_ALTERNATE) {
        switch (conversion) {
        case 'X':
        case 'x':
            /* @bug memory: digits steps past a 0x even for 0, which has none:
             * ADD #2,R2 at 0x8c0c9894, stored at 0x8c0c9898. with a precision
             * above 1, _fmtout then writes the rest of the text as -1 bytes */
#ifdef NONMATCHING
            /* this side repairs the tag above: 0 has no 0x to step past */
            parts->digits = value != 0 ? start + 2 : start;
#else
            parts->digits = start + 2;
#endif
            break;
        case 'o':
            parts->digits = start + 1;
            break;
        /* @hack redundant: an empty default, where none would do */
        default:
            break;
        }
    } else {
        parts->digits = start;
    }
    if (parts->precision > total) {
        effective = parts->precision;
        parts->digit_zeros = parts->precision - total;
    } else {
        effective = total;
        parts->digit_zeros = 0;
    }
    if ((*start == '+' || *start == '-' || *start == ' ') &&
        parts->width > effective && parts->fill == '0') {
        parts->digit_zeros += parts->width - effective;
        parts->padding = 0;
    } else if (parts->width > effective) {
        parts->padding = parts->width - effective;
    } else {
        parts->padding = 0;
    }
    /* @hack spelling: i stepped in the subscript, where a step in the loop's
     * header would do */
    for (i = pending - 1; i >= 0;) {
        *text++ = reversed[i--];
    }
    *text = '\0';
}

/* write length bytes of text, and count them. a string takes them at once,
 * and a stream a byte at a time through put. where put returns -1, the call
 * ends without counting that byte */
static void emit_text(const char *text, int length,
                      struct output_cursor *output)
{
    int i;

    if (output->mode == PRINT_TO_STRING) {
        memcpy(output->destination, text, length);
        output->destination = (char *)output->destination + length;
        output->count += length;
    } else {
        for (i = 0; i < length; i++) {
            if (output->put(*text++, output->destination) == -1) {
                return;
            }
            output->count++;
        }
    }
}

static void terminate_text(struct output_cursor *output)
{
    if (output->mode == PRINT_TO_STRING) {
        *(char *)output->destination = '\0';
    }
}

/* the stream's _IOERR bit, or 0 for a string */
static int output_error(struct output_cursor *output)
{
    if (output->mode == PRINT_TO_STRING) {
        return 0;
    }
    return ((FILE *)output->destination)->_ioflag1 & _IOERR;
}
