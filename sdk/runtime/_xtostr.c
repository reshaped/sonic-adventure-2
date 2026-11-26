#include <multiprec.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* the most digits of value a conversion gives. any digit it writes past
 * them is a 0 */
#define SIGNIFICANT_DIGITS 17
/* the precision a conversion takes where the directive gives none */
#define DEFAULT_PRECISION 6
/* %g writes its value as %e below this decimal exponent, or above the
 * precision */
#define LEAST_FIXED_EXPONENT (-4)

static void round_digits(char *digits, int position);

void _xtostr(va_list *arguments, char *text, int conversion,
             struct format_parts *parts)
{
    long double value;
    int scale;
    signed char sign;
    unsigned char magnitude[8];
    char digits[32];
    char *begin;
    char *lead;
    char *low;
    char *high;
    char *sign_end;
    char *end;
    char swap;
    char sign_flags;
    int status;
    int length;
    int count;
    int position;
    int exponent;
    int source;
    int skip;
    int i;
    int j;

#ifdef NONMATCHING
    /* this side keeps the shipped result. a float argument arrives promoted to
     * double, which -m4-single-only makes 4 bytes. GCC compiles va_arg of a
     * float to a trap */
    value = va_arg(*arguments, double);
#else
    value = va_arg(*arguments, float);
#endif
    /* @hack redundant: a square of value that never runs */
    if (0) {
        value = value * value;
    }
    if (parts->precision == -1) {
        parts->precision = DEFAULT_PRECISION;
    }
    begin = text;
    digits[0] = '0';
    if (value != 0.0) {
        status = _xti(value, SIGNIFICANT_DIGITS, magnitude, &scale, &sign);
        if (status != XTI_FINITE) {
            *text = '\0';
            if (parts->precision > parts->width) {
                parts->padding = parts->precision;
            } else {
                parts->padding = parts->width;
            }
            switch (status) {
            case XTI_PLUS_INFINITY:
                parts->fill = '+';
                break;
            case XTI_MINUS_INFINITY:
                parts->fill = '-';
                break;
            default:
                parts->fill = '*';
                break;
            }
            return;
        }
        _its(magnitude, digits + 1, SIGNIFICANT_DIGITS);
    } else {
        sign = 1;
        digits[1] = '\0';
    }
    /* _its writes the least significant digit first. a run of zeros at
     * digits[1] is then the integer's trailing zeros. the loop drops them,
     * and raises scale by their count */
    for (i = 1; digits[i] == '0'; i++) {
    }
    if (i > 1) {
        length = strlen(digits + i);
        scale += i - 1;
        for (j = 1; j <= length; j++, i++) {
            digits[j] = digits[i];
        }
        digits[j] = '\0';
    }
    /* @hack spelling: lead assigned inside strlen's argument, where a
     * statement of its own would do */
    length = strlen(lead = digits + 1);
    for (i = 0, high = lead + length - 1, low = lead; i < length / 2; i++) {
        swap = *low;
        *low = *high;
        *high = swap;
        high--;
        low++;
    }
    if (length == 0) {
        *lead = '0';
        length = 1;
        scale = 0;
    }
    if (conversion == 'g' || conversion == 'G') {
        parts->flags |= PRINT_G;
        exponent = scale + length - 1;
        if (exponent < LEAST_FIXED_EXPONENT || exponent > parts->precision) {
            conversion -= 'g' - 'e';
        } else {
            conversion = 'f';
        }
    }
    if (sign == 1) {
        sign_flags = parts->flags & (PRINT_PLUS | PRINT_SPACE);
        switch (sign_flags) {
        case PRINT_PLUS:
            *text++ = '+';
            break;
        case PRINT_SPACE:
            *text++ = ' ';
            break;
        }
    } else {
        *text++ = '-';
    }
    /* count is the text's length as the width counts it, without a sign or
     * space. sign_end is where such a sign ends */
    sign_end = begin + 1;
    if (conversion == 'f') {
        if (scale >= 0) {
            for (i = 0, low = lead; i < length; i++) {
                *text++ = *low++;
            }
            parts->digits = text;
            parts->digit_zeros = scale;
            count = 0;
            if ((!(parts->flags & PRINT_G) && parts->precision > 0) ||
                (parts->flags & PRINT_ALTERNATE)) {
                *text++ = '.';
                count++;
            }
            /* @hack redundant: PRINT_G tested again where the || has ruled it
             * out, here and in the three identical tests below */
            if (!(parts->flags & PRINT_G) ||
                ((parts->flags & PRINT_G) != 0 &&
                 (parts->flags & PRINT_ALTERNATE))) {
                parts->suffix = text;
                parts->suffix_zeros = parts->precision;
                count += parts->precision;
            }
            count += scale;
            count += length;
            if ((*begin == '+' || *begin == '-' || *begin == ' ') &&
                parts->width > count && parts->fill == '0') {
                parts->prefix_end = sign_end;
                parts->prefix_zeros = parts->width - count;
                parts->padding = 0;
            } else if (parts->width > count) {
                parts->padding = parts->width - count;
            } else {
                parts->padding = 0;
            }
        } else {
            scale += length;
            position = scale + parts->precision;
            if (position < length && position >= 0) {
                round_digits(digits, position + 1);
            }
            if (scale > 0) {
                if (digits[0] == '0') {
                    source = 1;
                    skip = 1;
                } else {
                    length++;
                    source = 0;
                    skip = 0;
                    scale++;
                }
                count = 0;
                while (scale > 0) {
                    *text++ = digits[source];
                    source++;
                    scale--;
                    count++;
                }
                *text++ = '.';
                count++;
                while (source - skip < length && parts->precision > 0) {
                    *text++ = digits[source++];
                    parts->precision--;
                    count++;
                }
                /* @hack redundant: the else if's test of text[-1] ahead of the
                 * loop, which repeats the loop's own test, here and in the two
                 * identical tests below */
                if (!(parts->flags & PRINT_G) ||
                    ((parts->flags & PRINT_G) != 0 &&
                     (parts->flags & PRINT_ALTERNATE))) {
                    count += parts->precision;
                    parts->digits = text;
                    parts->digit_zeros = parts->precision;
                } else if (text[-1] == '0') {
                    while (text[-1] == '0') {
                        text--;
                        count--;
                    }
                }
                if (!(parts->flags & PRINT_ALTERNATE) && text[-1] == '.' &&
                    (parts->digit_zeros == 0 || (parts->flags & PRINT_G))) {
                    count = count - parts->digit_zeros - 1;
                    text--;
                    parts->digit_zeros = 0;
                    parts->digits = begin;
                }
                if ((*begin == '+' || *begin == '-' || *begin == ' ') &&
                    parts->width > count && parts->fill == '0') {
                    parts->prefix_end = sign_end;
                    parts->prefix_zeros = parts->width - count;
                    parts->padding = 0;
                } else if (parts->width > count) {
                    parts->padding = parts->width - count;
                } else {
                    parts->padding = 0;
                }
                if (parts->digits == begin) {
                    parts->digits = parts->prefix_end;
                }
            } else {
                if (digits[0] == '0') {
                    source = 1;
                    skip = 1;
                } else {
                    source = 0;
                    skip = 0;
                    scale++;
                    length++;
                }
                if (scale > 0) {
                    source = 1;
                    *text++ = digits[0];
                } else {
                    *text++ = '0';
                }
                count = 1;
                *text++ = '.';
                count++;
                if (scale < 0) {
                    parts->digits = text;
                    if (parts->precision > -scale) {
                        parts->digit_zeros = -scale;
                        count -= scale;
                    } else {
                        parts->digit_zeros = parts->precision;
                        count += parts->precision;
                    }
                    parts->precision += scale;
                }
                while (source - skip < length && parts->precision > 0) {
                    *text++ = digits[source];
                    count++;
                    source++;
                    parts->precision--;
                }
                if (!(parts->flags & PRINT_G) ||
                    ((parts->flags & PRINT_G) != 0 &&
                     (parts->flags & PRINT_ALTERNATE))) {
                    if (parts->precision > 0) {
                        parts->suffix = text;
                        parts->suffix_zeros = parts->precision;
                        count += parts->precision;
                    }
                } else if (text[-1] == '0') {
                    while (text[-1] == '0') {
                        text--;
                        count--;
                    }
                }
                if (!(parts->flags & PRINT_ALTERNATE) && text[-1] == '.' &&
                    ((parts->digit_zeros == 0 && parts->suffix_zeros == 0) ||
                     (parts->flags & PRINT_G))) {
                    count =
                        count - parts->digit_zeros - parts->suffix_zeros - 1;
                    parts->suffix_zeros = 0;
                    parts->digit_zeros = 0;
                    text--;
                    parts->digits = begin;
                    parts->suffix = NULL;
                }
                if ((*begin == '+' || *begin == '-' || *begin == ' ') &&
                    parts->width > count && parts->fill == '0') {
                    parts->prefix_end = sign_end;
                    parts->prefix_zeros = parts->width - count;
                    parts->padding = 0;
                } else if (parts->width > count) {
                    parts->padding = parts->width - count;
                } else {
                    parts->padding = 0;
                }
                if (parts->digits == begin) {
                    parts->digits = parts->prefix_end;
                }
            }
        }
    } else {
        if (parts->precision + 1 < length) {
            round_digits(digits, parts->precision + 2);
        }
        if (digits[0] == '0') {
            skip = 1;
        } else {
            skip = 0;
            scale--;
            length++;
        }
        scale += length - skip;
        *text++ = digits[skip];
        source = skip;
        count = 1;
        /* @hack redundant: a copy of skip to count that never runs */
        if (0) {
            count = skip;
        }
        source++;
        count++;
        *text++ = '.';
        while (source <= length && parts->precision > 0) {
            *text++ = digits[source];
            count++;
            source++;
            parts->precision--;
        }
        if (!(parts->flags & PRINT_G) || ((parts->flags & PRINT_G) != 0 &&
                                          (parts->flags & PRINT_ALTERNATE))) {
            parts->digits = text;
            parts->digit_zeros = parts->precision;
            count += parts->precision;
        } else if (text[-1] == '0') {
            while (text[-1] == '0') {
                text--;
                count--;
            }
        }
        if (!(parts->flags & PRINT_ALTERNATE) && text[-1] == '.' &&
            (parts->digit_zeros == 0 || (parts->flags & PRINT_G))) {
            count = count - parts->digit_zeros - 1;
            text--;
            parts->digits = begin;
            parts->digit_zeros = 0;
        }
        if (conversion == 'e') {
            *text++ = 'e';
        } else {
            *text++ = 'E';
        }
        count++;
        if (scale >= 0) {
            *text++ = '+';
        } else {
            *text++ = '-';
            scale = -scale;
        }
        count++;
        if (scale >= 100) {
            text += 2;
            count += 3;
        } else {
            *text++ = '0';
            *text = '0';
            count += 2;
        }
        /* the loop writes the exponent's digits from the last, where text
         * stands */
        end = text + 1;
        while (scale > 0) {
            *text-- = (char)((scale % 10) + '0');
            scale = scale / 10;
        }
        text = end;
        if ((*begin == '+' || *begin == '-' || *begin == ' ') &&
            parts->width > count && parts->fill == '0') {
            parts->prefix_end = sign_end;
            parts->prefix_zeros = parts->width - count;
            parts->padding = 0;
        } else if (parts->width > count) {
            parts->padding = parts->width - count;
        } else {
            parts->padding = 0;
        }
        if (parts->digits == begin) {
            parts->digits = parts->prefix_end;
        }
    }
    *text = '\0';
}

/* round half up at digits[position], the first digit dropped. a carry can
 * reach digits[0], the spare '0' ahead of the digits */
static void round_digits(char *digits, int position)
{
    if (digits[position] > '4') {
        digits[position] = '0';
        position--;
        digits[position]++;
        for (; position > 0; position--) {
            if (digits[position] > '9') {
                digits[position] -= 10;
                /* @hack spelling: the carry stored through digits + position
                 * - 1, where digits[position - 1] would do */
                *(digits + position - 1) += 1;
            }
        }
        /* @hack redundant: a pass at digits[1] after the loop, whose test
         * never holds, since digits[1] is a digit by then */
        if (digits[1] > '9') {
            digits[1] -= 10;
            digits[0]++;
        }
    }
}
