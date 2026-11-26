#include <multiprec.h>

void _add(unsigned char *to, const unsigned char *from, int n)
{
    int i;
    int sum;
    int carry = 0;

    for (i = n - 1; i >= 0; i--) {
        sum = to[i] + from[i] + carry;
        to[i] = (unsigned char)sum;
        carry = sum / 256;
    }
}
