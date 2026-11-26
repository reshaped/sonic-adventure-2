#include <string.h>

char *strchr(const char *s, int c)
{
    /* @hack spelling: scan, a copy of s, where s itself would do */
    const char *scan = s;

    /* testing c first finds a c of '\0' at the terminator, as C requires */
    while (*scan != (char)c) {
        if (*scan == '\0') {
            return NULL;
        }
        scan++;
    }
    return (char *)scan;
}
