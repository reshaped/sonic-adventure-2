#include <ctype.h>

/* each character code's class bits. codes 0x80 to 0xff have none */
#define TWO(bits) bits, bits
#define FOUR(bits) TWO(bits), TWO(bits)
#define EIGHT(bits) FOUR(bits), FOUR(bits)

const unsigned char _ctype[256] = {
    /* 0x00 to 0x1f: tab to carriage return are white space too */
    EIGHT(_CNTRL), _CNTRL, FOUR(_CNTRL | _WHITE), _CNTRL | _WHITE, TWO(_CNTRL),
    EIGHT(_CNTRL), EIGHT(_CNTRL),
    /* 0x20 to 0x2f */
    _SPACE | _WHITE, _PUNCT, TWO(_PUNCT), FOUR(_PUNCT), EIGHT(_PUNCT),
    /* 0x30 to 0x3f */
    EIGHT(_DIGIT | _HEX), TWO(_DIGIT | _HEX), FOUR(_PUNCT), TWO(_PUNCT),
    /* 0x40 to 0x4f */
    _PUNCT, FOUR(_UPPER | _HEX), TWO(_UPPER | _HEX), _UPPER, EIGHT(_UPPER),
    /* 0x50 to 0x5f */
    EIGHT(_UPPER), TWO(_UPPER), _UPPER, _PUNCT, FOUR(_PUNCT),
    /* 0x60 to 0x6f */
    _PUNCT, FOUR(_LOWER | _HEX), TWO(_LOWER | _HEX), _LOWER, EIGHT(_LOWER),
    /* 0x70 to 0x7f */
    EIGHT(_LOWER), TWO(_LOWER), _LOWER, FOUR(_PUNCT), _CNTRL};
