#include <string.h>

char *strpbrk(const char *string, const char *set)
{
    size_t pos;
    size_t candidate;

    for (pos = 0; string[pos] != '\0'; pos++) {
        candidate = 0;
        while (set[candidate] != '\0' && set[candidate] != string[pos]) {
            candidate++;
        }
        /* @hack spelling: set[candidate] tested again after the scan, where
         * a return inside the scan on a match would do */
        if (set[candidate] != '\0') {
            return (char *)&string[pos];
        }
    }
    return NULL;
}
