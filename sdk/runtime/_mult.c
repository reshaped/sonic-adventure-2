#include <multiprec.h>
#include <string.h>

void _mult(const unsigned char *a, const unsigned char *b,
           unsigned char *product)
{
    short i;
    short j;
    unsigned short partial;
    unsigned char row[16];
    unsigned short term;

    memset(product, 0, 16);
    for (i = 7; i >= 0; --i) {
        if (b[i] != 0) {
            /* a times b[i] takes 9 bytes, at bytes i to i + 8 of the product.
             * row builds it at that place, where _add adds it in */
            memset(row, 0, 16);
            for (j = 7; j >= 0; --j) {
                term = a[j] * b[i];
                partial = *(row + i + j + 1);
                partial += term;
                _mvsto2b(row + i + j, &partial);
            }
            if (i == 7) {
                memcpy(product + i, row + i, 9);
            } else {
                _add(product + i, row + i, 9);
            }
        }
    }
}
