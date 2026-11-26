#include <string.h>

char *strcat(char *s1, const char *s2)
{
    char *to = s1;
    /* @hack spelling: from, a copy of s2, where s2 itself would do */
    const char *from = s2;

    while (*to != 0) {
        to++;
    }
    /* @hack type: the cast to unsigned char in the test, where the copied
     * char tested as it is would do */
    while ((unsigned char)(*to++ = *from++) != 0) {
    }
    return s1;
}
