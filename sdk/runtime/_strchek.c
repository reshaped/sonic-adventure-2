#include <errno.h>
#include <multiprec.h>

/* a conversion gives up this many bytes past a string's start, where the
 * string hasn't ended */
#define SCAN_LIMIT 512

int _strchek(const char *end, const char *start)
{
    /* @hack spelling: two nested tests, where one test with && would do */
    if (*end != 0) {
        if (end - start >= SCAN_LIMIT) {
            errno = ESTRN;
            return 1;
        }
    }
    return 0;
}
