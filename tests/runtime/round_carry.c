/* _rnd's carry out of a number's first byte. each number lies in a heap block
 * of its own, so that the address sanitizer stops a read below its first
 * byte. */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../../sdk/runtime/_rnd.c"

/* round a copy of the size bytes at number to bits, and check that _rnd
 * returns carry and leaves the bytes at rounded */
static void check(const unsigned char *number, size_t size, int bits, int carry,
                  const unsigned char *rounded)
{
    unsigned char *copy = malloc(size);

    assert(copy != NULL);
    memcpy(copy, number, size);
    assert(_rnd(copy, bits) == carry);
    assert(memcmp(copy, rounded, size) == 0);
    free(copy);
}

int main(void)
{
    /* a carry out of the first byte clears every bit kept, and returns 1 */
    static const unsigned char ones[] = {0xff, 0xff, 0xc0};
    static const unsigned char ones_carried[] = {0x00, 0x00, 0xc0};
    static const unsigned char nibble[] = {0xf8};
    static const unsigned char nibble_carried[] = {0x08};
    /* a carry that stops in the first byte, and a round with no carry */
    static const unsigned char tie[] = {0xab, 0xf8};
    static const unsigned char tie_rounded[] = {0xac, 0x08};
    static const unsigned char up[] = {0xab, 0xcf};
    static const unsigned char up_rounded[] = {0xab, 0xdf};
    /* bits below half leave the number as it was */
    static const unsigned char down[] = {0x12, 0x34, 0x40};

    check(ones, sizeof(ones), 16, 1, ones_carried);
    check(nibble, sizeof(nibble), 4, 1, nibble_carried);
    check(tie, sizeof(tie), 12, 0, tie_rounded);
    check(up, sizeof(up), 12, 0, up_rounded);
    check(down, sizeof(down), 16, 0, down);
    return 0;
}
