#include <multiprec.h>
#include <string.h>

/* the most digits _its writes */
#define PLACES 17

/* eights[place] is 8 times 10^place, most significant byte first. it is the
 * weight of the top bit of a digit in that place */
static const unsigned char eights[PLACES][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x20},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x40},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x38, 0x80},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x35, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x7a, 0x12, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x04, 0xc4, 0xb4, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x2f, 0xaf, 0x08, 0x00},
    {0x00, 0x00, 0x00, 0x01, 0xdc, 0xd6, 0x50, 0x00},
    {0x00, 0x00, 0x00, 0x12, 0xa0, 0x5f, 0x20, 0x00},
    {0x00, 0x00, 0x00, 0xba, 0x43, 0xb7, 0x40, 0x00},
    {0x00, 0x00, 0x07, 0x46, 0xa5, 0x28, 0x80, 0x00},
    {0x00, 0x00, 0x48, 0xc2, 0x73, 0x95, 0x00, 0x00},
    {0x00, 0x02, 0xd7, 0x98, 0x83, 0xd2, 0x00, 0x00},
    {0x00, 0x1c, 0x6b, 0xf5, 0x26, 0x34, 0x00, 0x00},
    {0x01, 0x1c, 0x37, 0x93, 0x7e, 0x08, 0x00, 0x00},
};

void _its(const unsigned char *value, char *digits, int n)
{
    int bit;
    int place;
    int digit;
    int remains;
    int weight;
    int skip;
    char *out;
    unsigned char rest[8];
    unsigned char step[8];

    remains = 1;
    out = digits + n;
    *out = '\0';
    memcpy(rest, value, 8);
    for (place = n - 1; place >= 0; place--) {
        /* rest is below 10^(place + 1), so its first skip bytes are 0, and so
         * are eights[place]'s. the arithmetic leaves them out. it keeps 2
         * bytes for each whole 5 of the place + 1 digits, and 2 more */
        skip = 8 - ((((place + 1) / 5) + 1) * 2);
        memcpy(step + skip, eights[place] + skip, 8 - skip);
        /* find the digit a bit at a time, from the top. subtract 8, 4, 2 and
         * 1 times 10^place in turn, wherever rest holds that much */
        digit = 0;
        weight = 8;
        for (bit = 0; bit < 4; bit++) {
            if (_umemcmp(rest + skip, step + skip, 8 - skip) >= 0) {
                remains = _sub(SUB_SIGNED, rest + skip, step + skip, 8 - skip);
                digit += weight;
            }
            weight /= 2;
            if (weight == 0) {
                break;
            }
            _rsfts(step + skip, 8 - skip, 1);
        }
        if (remains == 0) {
            /* a subtraction left rest 0, so every digit below is 0 */
            *--out = (char)(digit + '0');
            while (--out >= digits) {
                *out = '0';
            }
            return;
        }
        *--out = (char)(digit + '0');
    }
}
