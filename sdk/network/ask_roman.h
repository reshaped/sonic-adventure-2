#ifndef ASK_ROMAN_H
#define ASK_ROMAN_H
/* rmtype classifies each input byte for romaji composition. its low four
 * bits, ROMAN_KIND_MASK, hold a roman_kind. each bit above them is a flag of
 * its own, ROMAN_HEX to ROMAN_DIGIT */
enum roman_kind {
    ROMAN_A = 1,
    ROMAN_I,
    ROMAN_U,
    ROMAN_E,
    ROMAN_O,
    ROMAN_DAKUTEN,
    ROMAN_HANDAKUTEN,
    ROMAN_OTHER,
    ROMAN_PUNCTUATION,
    ROMAN_VOICED,
    ROMAN_HALF_VOICED,
    ROMAN_N,
    ROMAN_CONSONANT,
    ROMAN_Y = 15
};
#define ROMAN_KIND_MASK 15
#define ROMAN_HEX 0x10
#define ROMAN_PAREN 0x20
#define ROMAN_ALPHA 0x40
#define ROMAN_DIGIT 0x80
extern unsigned char rmtype[256];
int is_alphanum(unsigned char byte);
int is_num(unsigned char byte);
int is_alpha(unsigned char byte);
int is_paren(unsigned char byte);
#endif
