#include <ctype.h>
#include <errno.h>
#include <multiprec.h>
#include <stdlib.h>

/* LONG_MAX and LONG_MIN, the results of an overflow. the SDK has no
 * limits.h to define them */
#define LARGEST_LONG 0x7fffffffL
#define SMALLEST_LONG (-LARGEST_LONG - 1)

long strtol(const char *nptr, char **endptr, int base)
{
    long result = 0;
    int negative = 0;
    const char *start = nptr;
    char last;
    unsigned long value;
    int digit;
    int overflow;

    while (isspace(*nptr)) {
        ++nptr;
        if (_strchek(nptr, start)) {
            return 0;
        }
    }
    if (*nptr == '+' || *nptr == '-') {
        if (*nptr == '-') {
            negative = 1;
        }
        ++nptr;
        if (_strchek(nptr, start)) {
            return 0;
        }
    }
    if (base == 0) {
        if (*nptr == '0') {
            ++nptr;
            if (_strchek(nptr, start)) {
                return 0;
            }
            if (tolower(*nptr) == 'x') {
                ++nptr;
                if (_strchek(nptr, start)) {
                    return 0;
                }
                base = 16;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
    } else if (base == 16 && *nptr == '0') {
        ++nptr;
        if (tolower(*nptr) == 'x') {
            ++nptr;
        }
        if (_strchek(nptr, start)) {
            return 0;
        }
    }
    if (base >= 2 && base <= 36) {
        if (base > 10) {
            last = (char)(base + ('a' - 11));
        } else {
            last = (char)(base + ('0' - 1));
        }
        if (isalnum(*nptr) && tolower(*nptr) <= last) {
            while (*nptr == '0') {
                ++nptr;
                if (_strchek(nptr, start)) {
                    return 0;
                }
            }
            overflow = 0;
            value = 0;
            while (isalnum(*nptr) && tolower(*nptr) <= last) {
                if (!overflow) {
                    if (isdigit(*nptr)) {
                        digit = *nptr - '0';
                    } else {
                        digit = (tolower(*nptr) - 'a') + 10;
                    }
                    if (value >
                        (((unsigned long)LARGEST_LONG + negative) - digit) /
                            base) {
                        overflow = 1;
                    } else {
                        value = (value * base) + digit;
                    }
                }
                ++nptr;
                if (_strchek(nptr, start)) {
                    return 0;
                }
            }
            if (!overflow) {
                if (negative) {
                    value = -value;
                }
                result = value;
            } else {
                errno = ERANGE;
                if (!negative) {
                    result = LARGEST_LONG;
                } else {
                    result = SMALLEST_LONG;
                }
            }
        }
        if (tolower(*nptr) == 'u') {
            ++nptr;
            if (tolower(*nptr) == 'l') {
                ++nptr;
            }
        }
        if (tolower(*nptr) == 'l') {
            ++nptr;
            if (tolower(*nptr) == 'u') {
                ++nptr;
            }
        }
        if (_strchek(nptr, start)) {
            return 0;
        }
    } else {
        errno = ECBASE;
    }
    if (endptr) {
        *endptr = (char *)nptr;
    }
    return result;
}
