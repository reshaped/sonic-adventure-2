#include "ask_jis.h"
#include "ask_roman.h"
#include "sg_xpt.h"

/* the full-width code of a byte that has none: a black square */
#define UNMAPPED 0x2223
#define FOUR_UNMAPPED UNMAPPED, UNMAPPED, UNMAPPED, UNMAPPED
#define EIGHT_UNMAPPED FOUR_UNMAPPED, FOUR_UNMAPPED
#define SIXTEEN_UNMAPPED EIGHT_UNMAPPED, EIGHT_UNMAPPED
#define EIGHT(first)                                                           \
    (first), (first) + 1, (first) + 2, (first) + 3, (first) + 4, (first) + 5,  \
        (first) + 6, (first) + 7
#define ALPHABET(first)                                                        \
    EIGHT(first), EIGHT((first) + 8), EIGHT((first) + 16), (first) + 24,       \
        (first) + 25
/* half_width's flags beside a half-width kana. VOICE_MARK marks a kana that
 * ASK_DAKUTEN follows, and HALF_VOICE_MARK one that ASK_HANDAKUTEN follows.
 * SMALL_FORM marks a kana that has a small form */
#define SMALL_FORM 0x2000
#define HALF_VOICE_MARK 0x4000
#define VOICE_MARK 0x8000
#define CAN_VOICE(kana) (kana), VOICE_MARK | (kana)
#define CAN_HALF_VOICE(kana) CAN_VOICE(kana), HALF_VOICE_MARK | (kana)
/* half_width holds the reading bytes from HALF_WIDTH_FIRST up to
 * HALF_WIDTH_END */
#define HALF_WIDTH_FIRST 0x98
#define HALF_WIDTH_END 0xf7

struct mark {
    Uint16 jis;
    Uint8 byte;
};
#pragma section ATA
/* the full-width JIS code of each byte. 0x1c to 0x1f are arrows, right,
 * left, up and down. 0xa1 to 0xdf are the half-width kana. 0xf1 to 0xf7 are
 * the kanji of yen, year, month, day, hour, minute and second */
/* clang-format off */
static Uint16 full_width[256] = {
    /* 0x00 */ SIXTEEN_UNMAPPED,
    /* 0x10 */ EIGHT_UNMAPPED, FOUR_UNMAPPED, 0x222a, 0x222b, 0x222c, 0x222d,
    /* 0x20 */ 0x2121, 0x212a, 0x2149, 0x2174, 0x2170, 0x2173, 0x2175, 0x2147,
    /* 0x28 */ 0x214a, 0x214b, 0x2176, 0x215c, 0x2124, 0x215d, 0x2125, 0x213f,
    /* 0x30 */ EIGHT(0x2330), 0x2338, 0x2339,
    /* 0x3a */ 0x2127, 0x2128, 0x2163, 0x2161, 0x2164, 0x2129, 0x2177,
    /* 0x41 */ ALPHABET(0x2341),
    /* 0x5b */ 0x214e, 0x216f, 0x214f, 0x2130, 0x2132, 0x2146,
    /* 0x61 */ ALPHABET(0x2361),
    /* 0x7b */ 0x2150, 0x2143, 0x2151, 0x2141, UNMAPPED,
    /* 0x80 */ SIXTEEN_UNMAPPED,
    /* 0x90 */ SIXTEEN_UNMAPPED,
    /* 0xa0 */ 0x2121, 0x2123, 0x2156, 0x2157, 0x2122, 0x2126, 0x2572, 0x2521,
    /* 0xa8 */ 0x2523, 0x2525, 0x2527, 0x2529, 0x2563, 0x2565, 0x2567, 0x2543,
    /* 0xb0 */ 0x213c, 0x2522, 0x2524, 0x2526, 0x2528, 0x252a, 0x252b, 0x252d,
    /* 0xb8 */ 0x252f, 0x2531, 0x2533, 0x2535, 0x2537, 0x2539, 0x253b, 0x253d,
    /* 0xc0 */ 0x253f, 0x2541, 0x2544, 0x2546, 0x2548, 0x254a, 0x254b, 0x254c,
    /* 0xc8 */ 0x254d, 0x254e, 0x254f, 0x2552, 0x2555, 0x2558, 0x255b, 0x255e,
    /* 0xd0 */ 0x255f, 0x2560, 0x2561, 0x2562, 0x2564, 0x2566, 0x2568, 0x2569,
    /* 0xd8 */ 0x256a, 0x256b, 0x256c, 0x256d, 0x256f, 0x2573, 0x212b, 0x212c,
    /* 0xe0 */ EIGHT_UNMAPPED, FOUR_UNMAPPED, 0x217c, 0x217b, UNMAPPED, UNMAPPED,
    /* 0xf0 */ UNMAPPED, 0x315f, 0x472f, 0x376e, 0x467c, 0x3b7e, 0x4a2c, 0x4943,
    /* 0xf8 */ EIGHT_UNMAPPED,
};
/* clang-format on */
static Uint8 special[] = ",-./Xx[]";
static Uint16 special_full[] = {0x2122, 0x213c, 0x2123, 0x2126,
                                0x2573, 0x2573, 0x2156, 0x2157};
/* the half-width form of each reading byte from HALF_WIDTH_FIRST: the marks,
 * then the hiragana */
/* clang-format off */
static Uint16 half_width[HALF_WIDTH_END - HALF_WIDTH_FIRST] = {
    /* 0x98 */ 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xb0, ASK_DAKUTEN, ASK_HANDAKUTEN,
    /* 0xa0 */ ' ',
    /* 0xa1 */ 0xa7, SMALL_FORM | 0xb1, 0xa8, SMALL_FORM | 0xb2,
    /* 0xa5 */ 0xa9, SMALL_FORM | 0xb3, 0xaa, SMALL_FORM | 0xb4,
    /* 0xa9 */ 0xab, SMALL_FORM | 0xb5,
    /* 0xab */ CAN_VOICE(0xb6), CAN_VOICE(0xb7), CAN_VOICE(0xb8),
    /* 0xb1 */ CAN_VOICE(0xb9), CAN_VOICE(0xba),
    /* 0xb5 */ CAN_VOICE(0xbb), CAN_VOICE(0xbc), CAN_VOICE(0xbd),
    /* 0xbb */ CAN_VOICE(0xbe), CAN_VOICE(0xbf),
    /* 0xbf */ CAN_VOICE(0xc0), CAN_VOICE(0xc1), 0xaf, SMALL_FORM | 0xc2,
    /* 0xc5 */ VOICE_MARK | 0xc2, CAN_VOICE(0xc3), CAN_VOICE(0xc4),
    /* 0xca */ 0xc5, 0xc6, 0xc7, 0xc8, 0xc9,
    /* 0xcf */ CAN_HALF_VOICE(0xca), CAN_HALF_VOICE(0xcb), CAN_HALF_VOICE(0xcc),
    /* 0xd8 */ CAN_HALF_VOICE(0xcd), CAN_HALF_VOICE(0xce),
    /* 0xde */ 0xcf, 0xd0, 0xd1, 0xd2, 0xd3,
    /* 0xe3 */ 0xac, SMALL_FORM | 0xd4, 0xad, SMALL_FORM | 0xd5, 0xae,
    /* 0xe8 */ SMALL_FORM | 0xd6,
    /* 0xe9 */ 0xd7, 0xd8, 0xd9, 0xda, 0xdb,
    /* 0xee */ 0xdc, SMALL_FORM | 0xdc, 0xb2, 0xb4, 0xa6, 0xdd,
    /* 0xf4 */ VOICE_MARK | 0xb3, 0xb6, 0xb9,
};
/* clang-format on */
static struct mark marks[] = {
    {0x2121, ' '},  {0x2122, 0x9b}, {0x2123, 0x98}, {0x2124, ','},
    {0x2125, '.'},  {0x2126, 0x9c}, {0x2127, ':'},  {0x2128, ';'},
    {0x2129, '?'},  {0x212a, '!'},  {0x212b, 0x9e}, {0x212c, 0x9f},
    {0x2130, '^'},  {0x2132, '_'},  {0x213c, 0x9d}, {0x213f, '/'},
    {0x2141, '~'},  {0x2143, '|'},  {0x2146, 0x60}, {0x2147, 0x27},
    {0x2149, '"'},  {0x214a, '('},  {0x214b, ')'},  {0x214e, '['},
    {0x214f, ']'},  {0x2150, '{'},  {0x2151, '}'},  {0x2156, 0x99},
    {0x2157, 0x9a}, {0x215c, '+'},  {0x215d, '-'},  {0x2161, '='},
    {0x2163, '<'},  {0x2164, '>'},  {0x216f, 0x5c}, {0x2170, '$'},
    {0x2173, '%'},  {0x2174, '#'},  {0x2175, '&'},  {0x2176, '*'},
    {0x2177, '@'},  {0x222a, 0x1c}, {0x222b, 0x1d}, {0x222c, 0x1e},
    {0x222d, 0x1f}};
#pragma section

Uint16 to_zenkaku(Uint16 code)
{
    Uint16 full = full_width[code & 0xff];
    if ((full & ASK_FIRST_BYTE) == ASK_KATAKANA && !(code & ASK_AS_KATAKANA)) {
        return (full & 0xff) | ASK_HIRAGANA;
    }
    return full;
}
Uint16 to_zenkaku_spec(Uint16 code)
{
    Uint8 *byte = special;
    Uint16 *full = special_full;
    while (*byte) {
        if (*byte == (code & 0xff)) {
            if ((*full & ASK_FIRST_BYTE) == ASK_KATAKANA &&
                !(code & ASK_AS_KATAKANA)) {
                return (*full & 0xff) | ASK_HIRAGANA;
            }
            return *full;
        }
        byte++;
        full++;
    }
    return 0;
}
Uint16 ext_jis(Uint8 byte, Uint16 attribute)
{
    return ((attribute & ASK_AS_KATAKANA) + ASK_HIRAGANA) | byte;
}
int to_hankaku(Uint8 *text, Uint16 code)
{
    Uint8 byte;
    Uint16 form;
    switch (code & ASK_FIRST_BYTE) {
    case ASK_ALPHANUMERIC:
    case 0:
        *text = code;
        return 1;
    case ASK_KATAKANA:
        code &= ASK_HIRAGANA | 0xff;
        /* fall through */
    case ASK_HIRAGANA:
    case ASK_MARKS:
        byte = to_ucode(code);
        if (byte < HALF_WIDTH_FIRST || byte >= HALF_WIDTH_END) {
            *text = byte;
            return 1;
        }
        form = half_width[byte - HALF_WIDTH_FIRST];
        *text++ = form;
        if (form & VOICE_MARK) {
            *text = ASK_DAKUTEN;
            return 2;
        } else if (form & HALF_VOICE_MARK) {
            *text = ASK_HANDAKUTEN;
            return 2;
        } else {
            return 1;
        }
    default:
        *text = ' ';
        break;
    }
    return 1;
}
int can_daku(Uint16 code)
{
    Uint8 text[2];
    if (is_shift(code)) {
        code = sjis2jis(code);
    }
    if (to_hankaku(text, code) != 1) {
        return 0;
    }
    return (rmtype[text[0]] & ROMAN_KIND_MASK) == ROMAN_VOICED;
}
int can_handaku(Uint16 code)
{
    Uint8 text[2];
    if (is_shift(code)) {
        code = sjis2jis(code);
    }
    if (to_hankaku(text, code) != 1) {
        return 0;
    }
    return (rmtype[text[0]] & ROMAN_KIND_MASK) == ROMAN_HALF_VOICED;
}
int can_small(Uint8 byte)
{
    /* byte is a hiragana's JIS cell */
    if (byte <= 0x20 || byte >= HALF_WIDTH_END - ASK_CELL_TO_READING) {
        return 0;
    }
    return half_width[byte - (HALF_WIDTH_FIRST - ASK_CELL_TO_READING)] &
           SMALL_FORM;
}
Uint8 srch_ucode(Uint16 code)
{
    struct mark *entry;
    for (entry = marks; entry < marks + (sizeof(marks) / sizeof(marks[0]));
         entry++) {
        if (entry->jis == code) {
            return entry->byte;
        } else if (entry->jis > code) {
            break;
        }
    }
    return 0;
}
