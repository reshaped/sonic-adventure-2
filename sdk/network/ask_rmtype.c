#include "ask_roman.h"

#define OTHER ROMAN_OTHER
#define FOUR_OTHER OTHER, OTHER, OTHER, OTHER
#define EIGHT_OTHER FOUR_OTHER, FOUR_OTHER
#define SIXTEEN_OTHER EIGHT_OTHER, EIGHT_OTHER
#define DIGIT (ROMAN_DIGIT | OTHER)
#define SYMBOL (ROMAN_ALPHA | OTHER)
#define PAREN (ROMAN_PAREN | OTHER)
#define PUNCT ROMAN_PUNCTUATION
#define HEX(k) (ROMAN_HEX | ROMAN_ALPHA | (k))
#define LETTER(k) (ROMAN_ALPHA | (k))
#define CONSONANT LETTER(ROMAN_CONSONANT)
#define ABCDEF                                                                 \
    HEX(ROMAN_A), HEX(ROMAN_CONSONANT), HEX(ROMAN_CONSONANT),                  \
        HEX(ROMAN_CONSONANT), HEX(ROMAN_E), HEX(ROMAN_CONSONANT)
#define A_TO_O                                                                 \
    ABCDEF, CONSONANT, CONSONANT, LETTER(ROMAN_I), CONSONANT, CONSONANT,       \
        CONSONANT, CONSONANT, LETTER(ROMAN_N), LETTER(ROMAN_O)
#define P_TO_Z                                                                 \
    CONSONANT, CONSONANT, CONSONANT, CONSONANT, CONSONANT, LETTER(ROMAN_U),    \
        CONSONANT, CONSONANT, CONSONANT, LETTER(ROMAN_Y), CONSONANT
#define VOICED ROMAN_VOICED
#define HALF ROMAN_HALF_VOICED

#pragma section ATA
/* clang-format off */
unsigned char rmtype[256] = {
    /* 0x00 */ SIXTEEN_OTHER,
    /* 0x10 */ SIXTEEN_OTHER,
    /* 0x20 */ OTHER, OTHER, PAREN, SYMBOL, SYMBOL, SYMBOL, SYMBOL, OTHER,
    /* 0x28 */ OTHER, PAREN, OTHER, OTHER, PUNCT, PUNCT, PUNCT, PUNCT,
    /* 0x30 */ DIGIT, DIGIT, DIGIT, DIGIT, DIGIT, DIGIT, DIGIT, DIGIT,
    /* 0x38 */ DIGIT, DIGIT, FOUR_OTHER, PAREN, OTHER,
    /* 0x40 */ OTHER, A_TO_O,
    /* 0x50 */ P_TO_Z, PUNCT, OTHER, ROMAN_PAREN | PUNCT, OTHER, OTHER,
    /* 0x60 */ OTHER, A_TO_O,
    /* 0x70 */ P_TO_Z, OTHER, OTHER, PAREN, OTHER, OTHER,
    /* 0x80 */ SIXTEEN_OTHER,
    /* 0x90 */ EIGHT_OTHER, OTHER, OTHER, PAREN, FOUR_OTHER, OTHER,
    /* 0xa0 */ SIXTEEN_OTHER,
    /* 0xb0 */ OTHER, OTHER, OTHER, VOICED, OTHER, OTHER, VOICED, VOICED,
    /* 0xb8 */ VOICED, VOICED, VOICED, VOICED, VOICED, VOICED, VOICED, VOICED,
    /* 0xc0 */ VOICED, VOICED, VOICED, VOICED, VOICED, OTHER, OTHER, OTHER,
    /* 0xc8 */ OTHER, OTHER, HALF, HALF, HALF, HALF, HALF, OTHER,
    /* 0xd0 */ EIGHT_OTHER,
    /* 0xd8 */ FOUR_OTHER, OTHER, OTHER, ROMAN_DAKUTEN, ROMAN_HANDAKUTEN,
    /* 0xe0 */ SIXTEEN_OTHER,
    /* 0xf0 */ SIXTEEN_OTHER,
};
/* clang-format on */
#pragma section
int is_alphanum(unsigned char byte)
{
    return rmtype[byte] & (ROMAN_ALPHA | ROMAN_DIGIT);
}
int is_num(unsigned char byte)
{
    return rmtype[byte] & ROMAN_DIGIT;
}
int is_alpha(unsigned char byte)
{
    return rmtype[byte] & ROMAN_ALPHA;
}
int is_paren(unsigned char byte)
{
    return rmtype[byte] & ROMAN_PAREN;
}
