#include <stdarg.h>
#include <stdio.h>

int vsprintf(char *s, const char *format, va_list arg)
{
    return _fmtout(PRINT_TO_STRING, NULL, s, format, arg);
}
