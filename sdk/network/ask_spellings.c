#include "ask_input.h"

/* the romaji spellings. each initial consonant has a row. a row follows it for
 * each letter that may come next, such as the Y of KYA. consonant is that
 * letter, or 0 in the initial's own row. row 0 spells the vowels alone.
 * vowel[v] spells a, i, u, e and o in turn, as one kana or two. each kana is
 * the low byte of its JIS X 0208 code, the same for hiragana and katakana.
 * prmtab[n] is the nth letter's first row, from 0 for A, or NULL for a vowel.
 * prmtab[26] ends Z's rows */
#pragma section ATA
struct ask_roman_row rmtab[49] = {
    {0, {"\"", "$", "&", "(", "*"}},
    {0, {"P", "S", "V", "Y", "\\"}}, /* B */
    {'Y', {{'S', 'c'}, {'S', '#'}, {'S', 'e'}, {'S', '\''}, {'S', 'g'}}},
    {0, {"+", "7", "/", ";", "3"}}, /* C */
    {'H', {{'A', 'c'}, "A", {'A', 'e'}, {'A', '\''}, {'A', 'g'}}},
    {'Y', {{'A', 'c'}, {'A', '#'}, {'A', 'e'}, {'A', '\''}, {'A', 'g'}}},
    {0, {"@", "B", "E", "G", "I"}}, /* D */
    {'H', {{'G', 'c'}, {'G', '#'}, {'G', 'e'}, {'G', '\''}, {'G', 'g'}}},
    {'Y', {{'B', 'c'}, {'B', '#'}, {'B', 'e'}, {'B', '\''}, {'B', 'g'}}},
    {0, {{'U', '!'}, {'U', '#'}, "U", {'U', '\''}, {'U', ')'}}}, /* F */
    {'Y', {{'U', 'c'}, {'U', '#'}, {'U', 'e'}, {'U', '\''}, {'U', 'g'}}},
    {0, {",", ".", "0", "2", "4"}}, /* G */
    {'W', {{'0', '!'}, {'0', '#'}, "0", {'0', '\''}, {'0', ')'}}},
    {'Y', {{'.', 'c'}, {'.', '#'}, {'.', 'e'}, {'.', '\''}, {'.', 'g'}}},
    {0, {"O", "R", "U", "X", "["}}, /* H */
    {'Y', {{'R', 'c'}, {'R', '#'}, {'R', 'e'}, {'R', '\''}, {'R', 'g'}}},
    {0, {{'8', 'c'}, "8", {'8', 'e'}, {'8', '\''}, {'8', 'g'}}}, /* J */
    {'Y', {{'8', 'c'}, {'8', '#'}, {'8', 'e'}, {'8', '\''}, {'8', 'g'}}},
    {0, {"+", "-", "/", "1", "3"}}, /* K */
    {'W', {{'/', 'n'}, {'/', '#'}, "/", {'/', '\''}, {'/', ')'}}},
    {'Y', {{'-', 'c'}, {'-', '#'}, {'-', 'e'}, {'-', '\''}, {'-', 'g'}}},
    {0, {"!", "#", "%", "'", ")"}}, /* L */
    {'Y', {"c", "#", "e", "'", "g"}},
    {0, {"^", "_", "`", "a", "b"}}, /* M */
    {'Y', {{'_', 'c'}, {'_', '#'}, {'_', 'e'}, {'_', '\''}, {'_', 'g'}}},
    {0, {"J", "K", "L", "M", "N"}}, /* N */
    {'Y', {{'K', 'c'}, {'K', '#'}, {'K', 'e'}, {'K', '\''}, {'K', 'g'}}},
    {0, {"Q", "T", "W", "Z", "]"}}, /* P */
    {'Y', {{'T', 'c'}, {'T', '#'}, {'T', 'e'}, {'T', '\''}, {'T', 'g'}}},
    {0, {{'/', '!'}, {'/', '#'}, "/", {'/', '\''}, {'/', ')'}}}, /* Q */
    {'Y', {{'/', 'c'}, {'/', '#'}, {'/', 'e'}, {'/', '\''}, {'/', 'g'}}},
    {0, {"i", "j", "k", "l", "m"}}, /* R */
    {'Y', {{'j', 'c'}, {'j', '#'}, {'j', 'e'}, {'j', '\''}, {'j', 'g'}}},
    {0, {"5", "7", "9", ";", "="}}, /* S */
    {'H', {{'7', 'c'}, "7", {'7', 'e'}, {'7', '\''}, {'7', 'g'}}},
    {'Y', {{'7', 'c'}, {'7', '#'}, {'7', 'e'}, {'7', '\''}, {'7', 'g'}}},
    {0, {"?", "A", "D", "F", "H"}}, /* T */
    {'H', {{'F', 'c'}, {'F', '#'}, {'F', 'e'}, {'F', '\''}, {'F', 'g'}}},
    {'S', {{'D', '!'}, {'D', '#'}, "D", {'D', '\''}, {'D', ')'}}},
    {'Y', {{'A', 'c'}, {'A', '#'}, {'A', 'e'}, {'A', '\''}, {'A', 'g'}}},
    {0, {{'t', '!'}, {'t', '#'}, "t", {'t', '\''}, {'t', ')'}}}, /* V */
    {0, {"o", {'&', '#'}, "&", {'&', '\''}, "r"}}, /* W */
    {'H', {{'&', '!'}, {'&', '#'}, "&", {'&', '\''}, {'&', ')'}}},
    {0, {"!", "#", "%", "'", ")"}}, /* X */
    {'Y', {"c", "#", "e", "'", "g"}},
    {0, {"d", "$", "f", {'$', '\''}, "h"}}, /* Y */
    {0, {"6", "8", ":", "<", ">"}}, /* Z */
    {'Y', {{'8', 'c'}, {'8', '#'}, {'8', 'e'}, {'8', '\''}, {'8', 'g'}}}};

struct ask_roman_row *prmtab[27] = {
    NULL,       &rmtab[1],  &rmtab[3], &rmtab[6],  NULL,       &rmtab[9],
    &rmtab[11], &rmtab[14], NULL,      &rmtab[16], &rmtab[18], &rmtab[21],
    &rmtab[23], &rmtab[25], NULL,      &rmtab[27], &rmtab[29], &rmtab[31],
    &rmtab[33], &rmtab[36], NULL,      &rmtab[40], &rmtab[41], &rmtab[43],
    &rmtab[45], &rmtab[46], &rmtab[48]};

/* spellings outside rmtab. each is a length byte, its letters, then its kana
 * as rmtab gives them and a 0. a length of 0, the string's own NUL, ends the
 * list. L and X both mark a small kana: LTSU and XTSU spell a small tsu */
char rmspec[] = "\004LTSUC\0"
                "\003LTUC\0"
                "\003LWAn\0"
                "\004XTSUC\0"
                "\003XTUC\0"
                "\003XWAn\0";
#pragma section
