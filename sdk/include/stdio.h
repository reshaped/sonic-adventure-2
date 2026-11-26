/* C runtime string formatting, as far as the sources use it */
#ifndef STDIO_H
#define STDIO_H
#include <stdarg.h>

/* the null pointer, defined by whichever header comes first */
#ifndef NULL
#define NULL ((void *)0)
#endif

/* sprintf writes the text format makes of the arguments to s, and a null
 * after it. it returns the bytes written, less the null. _fmtout writes the
 * text, and says where it departs from the C standard */
int sprintf(char *s, const char *format, ...);
/* sprintf, with its arguments read through arg */
int vsprintf(char *s, const char *format, va_list arg);

/* SHC's stream, as far as _fmtout reads it */
typedef struct _iobuf {
    unsigned char *_bufptr;
    long _bufcnt;
    unsigned char *_bufbase;
    long _buflen;
    char _ioflag1;
} FILE;

/* _ioflag1's bits */
#define _IOREAD 1 /* open to read, or an update stream reading */
#define _IOWRITE 2 /* open to write, or an update stream writing */
#define _IORW 4 /* open for update */
#define _IOERR 64 /* an I/O error */

/* _fmtout's modes: where its text goes */
enum { PRINT_TO_STREAM, PRINT_TO_STRING };

/* the formatter under sprintf and vsprintf. it writes the text format makes
 * of arguments to destination, and returns the bytes written, less any null.
 *
 * with PRINT_TO_STRING, destination is a string. the text goes there with a
 * null after it, and put isn't called.
 *
 * with PRINT_TO_STREAM, destination is a FILE. put takes the text a byte at
 * a time, with the FILE. a byte of 0x80 or more reaches put as a negative
 * char. _fmtout returns -1 at once for a FILE that is null, not open or open
 * only to read. errno then becomes PTRERR, NOTOPN or EBADF. it also
 * returns -1 at once where _IOERR is set. once _IOERR is set, the formatter
 * stops before the next directive or literal text.
 *
 * the formatter hands put its text in pieces. a piece is the literal text up
 * to a directive, or a single fill byte or zero. a stretch of a conversion's
 * text between its zeros is a piece too. where put returns -1, the formatter
 * drops that byte and the rest of its piece, uncounted.
 *
 * it departs from the C standard in these ways.
 * - %e, %E, %f, %g and %G take a float. -fpu=single passes a double as one.
 *   L changes nothing for them, or for an integer.
 * - they give 17 significant digits, then zeros. they round half up on those
 *   digits: %.2f of 0.125 is 0.13.
 * - %g takes %e where the value's exponent, before rounding, is below -4 or
 *   above the precision. its precision counts the digits after the point either
 *   way: %g of 123.456f is 123.456001.
 * - an infinity prints as its sign, and a NaN as '*'. each is repeated the
 *   larger of width and precision times. -0 prints as 0.
 * - with the space flag ahead of +, a float not below zero gets no sign. a
 *   float's sign or space doesn't count toward its width: %8.2f of -1.5 is
 *   "    -1.50".
 * - the 0 flag pads %c, %s and %% too. it pads on the side the - flag picks,
 *   ahead of a 0x, and even with a precision. %-05d of 42 is 42000. %#06x of
 *   255 is 000xff. %08.3d of 5 is 00000005. after a sign or space, the zeros
 *   follow it, even with -.
 * - an integer's precision counts its sign, space or 0x. %.3d of -5 is -05,
 *   and %#.5x of 255 is 0x0ff. %#.0o, %+.0d and % .0d of 0 have no text.
 * - %% takes flags and a width, as %c does. %s reads its string to the null,
 *   even with a precision. a '.' without digits leaves the precision unset. %p
 *   is %x of the address. %n stores an int, even for %hn.
 * - errno becomes ECSPEC for an invalid conversion or a flag after the 0 flag.
 *   it also becomes ECSPEC for a width or precision past 512. the output then
 *   ends before that directive. a %n with a width or precision past 512 still
 *   stores its count.
 *
 * it has three bugs.
 * - a * width of INT_MIN passes the limit of 512. it pads %c and %% with
 *   2^31 - 1 fill bytes. it pads a %s of nonzero length with 2^31 less that
 *   length. the count returned then wraps past INT_MAX.
 * - a directive the format's null cuts short, such as a trailing %, is invalid.
 *   the formatter then reads the byte after the null.
 * - %#x or %#X of 0 with a precision above 1 goes wrong. it writes 0, a null
 *   and the precision less one zeros. it then writes the rest of its text
 *   with a length of -1. a string takes that as a memcpy of (size_t)-1 bytes,
 *   which writes far past it. a stream takes it as nothing */
int _fmtout(int mode, int (*put)(int c, FILE *stream), void *destination,
            const char *format, va_list arguments);

/* format_parts' flags */
enum {
    PRINT_STOP = 1, /* the directive is invalid, and the output ends there */
    PRINT_LEFT = 2, /* the - flag */
    PRINT_ALTERNATE = 4, /* the # flag */
    PRINT_PLUS = 8, /* the + flag */
    PRINT_SPACE = 16, /* the space flag, unless + came first */
    PRINT_G = 32 /* %g or %G, which _xtostr sets */
};

/* a directive and the text of its conversion. the formatter writes padding
 * fill bytes ahead of the text, or after it under PRINT_LEFT. it writes the
 * text in three stretches: up to prefix_end, then up to digits, then the
 * rest. prefix_zeros, digit_zeros and suffix_zeros zeros follow the three in
 * turn */
struct format_parts {
    char flags;
    /* ' ', or '0' for the 0 flag. _xtostr sets an infinity's sign or a NaN's
     * '*' */
    char fill;
    short unk_2;
    int width;
    int precision; /* -1 where the directive gives none */
    int length; /* the length modifier, 'h', 'l' or 'L', or ' ' for none */
    char *prefix_end;
    char *digits;
    char *suffix; /* where suffix_zeros follow. nothing reads it */
    int prefix_zeros;
    int digit_zeros;
    int suffix_zeros;
    int padding;
};

/* write the text that conversion makes of the float at *arguments to text,
 * with a null. step *arguments past the float. conversion is 'e', 'E',
 * 'f', 'g' or 'G'. parts must give the directive, with prefix_end and digits
 * at text and its three zero counts 0.
 *
 * the call sets parts->prefix_end, parts->digits, the zero counts and
 * parts->padding for the text. it changes parts->precision and parts->flags.
 * a precision of -1 gives 6. an infinity or a NaN writes "", and sets
 * parts->fill to its sign or '*'. parts->padding then gets the larger of the
 * width and the precision.
 *
 * it sets FPSCR's PR for its double arithmetic. it clears PR and SZ before
 * each call it makes, and returns with both clear */
void _xtostr(va_list *arguments, char *text, int conversion,
             struct format_parts *parts);

#endif
