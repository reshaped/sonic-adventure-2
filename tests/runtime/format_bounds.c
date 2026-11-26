/* _fmtout with a directive that the format's null cuts short, and with %#x of
 * 0 at a precision above 1. each format lies in a heap block that ends at its
 * null, so that the address sanitizer stops a read past it. The SDK's headers
 * come first, as _fmtout's FILE and format_parts are theirs. */
#include <machine.h>
#undef get_fpscr
#undef set_fpscr
#define get_fpscr() 0
#define set_fpscr(value) ((void)(value))

#include "../../sdk/runtime/_fmtout.c"

#include <stdlib.h>

int printf(const char *format, ...);

/* the line of the first check that fails, or 0 */
static int failed;
#define CHECK(test)                                                            \
    do {                                                                       \
        if (!(test) && !failed) {                                              \
            failed = __LINE__;                                                 \
        }                                                                      \
    } while (0)

/* no check formats a float */
void _xtostr(va_list *arguments, char *text, int conversion,
             struct format_parts *parts)
{
    (void)arguments;
    (void)text;
    (void)conversion;
    (void)parts;
    __builtin_trap();
}

/* write the text format makes of the arguments to text, and return _fmtout's
 * count. format is copied first to a heap block of its own */
static int format_text(char *text, const char *format, ...)
{
    size_t size = strlen(format) + 1;
    char *copy = malloc(size);
    va_list arguments;
    int count;

    memcpy(copy, format, size);
    va_start(arguments, format);
    count = _fmtout(PRINT_TO_STRING, NULL, text, copy, arguments);
    va_end(arguments);
    free(copy);
    return count;
}

/* whether format, cut short by its null, writes only "ab" and sets errno to
 * ECSPEC */
static int cut_short(const char *format)
{
    char text[16];

    _errno = 0;
    return format_text(text, format, 1) == 2 && strcmp(text, "ab") == 0 &&
           _errno == ECSPEC;
}

int main(void)
{
    char text[16];

    CHECK(cut_short("ab%"));
    CHECK(cut_short("ab%-"));
    CHECK(cut_short("ab%5"));
    CHECK(cut_short("ab%."));
    CHECK(cut_short("ab%*"));
    CHECK(cut_short("ab%l"));
    /* 0 has no 0x, so the precision gives it zeros alone */
    CHECK(format_text(text, "%#.4x", 0) == 4 && strcmp(text, "0000") == 0);
    CHECK(format_text(text, "[%#.3X]", 0) == 5 && strcmp(text, "[000]") == 0);
    /* any other value keeps its 0x, which the precision counts */
    CHECK(format_text(text, "%#.5x", 255) == 5 && strcmp(text, "0x0ff") == 0);
    if (failed) {
        printf("tests/runtime/format_bounds.c:%d: check failed\n", failed);
        return 1;
    }
    return 0;
}
