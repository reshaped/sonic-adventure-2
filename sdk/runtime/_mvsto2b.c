#include <multiprec.h>

void _mvsto2b(unsigned char *bytes, const unsigned short *half)
{
    int i;

    for (i = 0; i < 2; i++) {
        bytes[i] = *half >> ((1 - i) * 8);
    }
}
