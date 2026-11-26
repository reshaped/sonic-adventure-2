#include <string.h>

char *strncat(char *s1, const char *s2, size_t n)
{
    char *to = s1;
    /* @hack spelling: from, a copy of s2, where s2 itself would do */
    const char *from = s2;
    size_t i;

    while (*to != 0) {
        to++;
    }
    for (i = 0; i < n; ++i) {
        /* @hack type: the cast to unsigned char in the test, where the
         * copied char tested as it is would do */
        if ((unsigned char)(*to++ = *from++) == 0) {
            break;
        }
    }
    /* i reaches n only where none of the n bytes copied was a terminator */
    if (i >= n) {
        *to = 0;
    }
    return s1;
}
