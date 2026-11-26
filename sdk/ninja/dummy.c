#include <ctype.h>
#include <nj_texlist.h>

void strupr(char *s)
{
    while (*s) {
        *s = toupper(*s);
        s++;
    }
    *s = '\0';
}
