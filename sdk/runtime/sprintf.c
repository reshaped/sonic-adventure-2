#include <stdarg.h>
#include <stdio.h>

int sprintf(char *s, const char *format, ...)
{
    va_list arguments;
    int length;

    va_start(arguments, format);
    length = _fmtout(PRINT_TO_STRING, NULL, s, format, arguments);
    va_end(arguments);
    return length;
}
