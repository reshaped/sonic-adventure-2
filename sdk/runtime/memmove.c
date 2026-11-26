#include <string.h>

#ifdef NONMATCHING
/* this side defines no memmove here. memcpy.c names its shipped memcpy
 * memmove */
#else
void *memmove(void *s1, const void *s2, size_t n)
{
    size_t i;

    if (s1 != s2) {
        if (s1 < s2) {
            char *to = s1;
            const char *from = s2;

            for (i = 0; i < n; i++) {
                *to++ = *from++;
            }
        } else {
            char *to = (char *)s1 + n;
            const char *from = (const char *)s2 + n;

            for (i = 0; i < n; i++) {
                *--to = *--from;
            }
        }
    }
    return s1;
}
#endif
