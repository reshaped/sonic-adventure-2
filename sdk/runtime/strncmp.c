#include <string.h>

int strncmp(const char *s1, const char *s2, size_t n)
{
    /* @hack spelling: p1 and p2, copies of s1 and s2, where s1 and s2
     * themselves would do */
    const char *p1 = s1;
    const char *p2 = s2;
    size_t i;

    if (n == 0) {
        return 0;
    }
    for (i = 0; i < n; ++i) {
        /* @hack spelling: p1[-1] read again in the test, where a local
         * holding the compared byte would do */
        if (*p1++ != *p2++ || p1[-1] == '\0') {
            break;
        }
    }
    return (unsigned char)p1[-1] - (unsigned char)p2[-1];
}
