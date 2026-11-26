/* JIS and Shift-JIS codes, and the display cells ASK draws text in. a code
 * holds its first byte in bits 8 to 15. a JIS code's row and cell each run
 * from 0x21 to 0x7e */
#ifndef ASK_JIS_H
#define ASK_JIS_H
#include <sg_xpt.h>

/* a display cell holds a byte of a character in bits 0 to 7. the byte's
 * kind takes the 4 bits from ASK_KIND_SHIFT, and an attribute the bits from
 * ASK_ATTRIBUTE_SHIFT. a one-byte character takes one cell of
 * ASK_BYTE_KIND. a two-byte character's first byte takes a cell of an odd
 * kind. its second byte takes a cell of the next kind */
#define ASK_KIND_SHIFT 8
#define ASK_KIND_MASK 15
#define ASK_ATTRIBUTE_SHIFT 12
enum {
    ASK_BYTE_KIND = 0,
    ASK_NARROW_KIND = 3, /* Shift-JIS, one column wide */
    ASK_JIS_KIND = 5,
    ASK_WIDE_KIND = 7 /* Shift-JIS, two columns wide */
};
/* the cells a two-byte character takes */
#define ASK_WIDE_CELLS 2
enum {
    ASK_PLAIN_ATTRIBUTE = 0,
    ASK_READING_ATTRIBUTE = 4, /* a kana of the reading */
    ASK_CONVERTED_ATTRIBUTE = 6,
    ASK_CURRENT_ATTRIBUTE = 7, /* the phrase or candidate chosen */
    ASK_CURSOR_ATTRIBUTE = 8
};

/* a reading, as kana_ustr holds it, takes a byte for each character, which
 * to_ucode gives. a hiragana's is ASK_KANA_BASE plus its JIS cell less 0x20:
 * its cell plus ASK_CELL_TO_READING */
#define ASK_KANA_BASE 0xa0
#define ASK_CELL_TO_READING (ASK_KANA_BASE - 0x20)

/* the first byte of a two-byte code, a JIS code's row. it is 0 in a one-byte
 * code. ASK_MARKS to ASK_KATAKANA are rows of JIS X 0208 */
#define ASK_FIRST_BYTE 0xff00
#define ASK_MARKS 0x2100
#define ASK_ALPHANUMERIC 0x2300
#define ASK_HIRAGANA 0x2400
#define ASK_KATAKANA 0x2500
/* a kana's voiced form follows it in JIS X 0208, and its half-voiced form
 * follows that */
#define ASK_TO_VOICED 1
#define ASK_TO_HALF_VOICED 2
/* JIS X 0208 has no hiragana VU. ASK gives it the cell of katakana VU, and
 * setmean draws it as katakana */
#define ASK_KANA_VU 0x74
/* Shift-JIS codes the half-width kana in one byte, from ASK_HALF_WIDTH_KANA
 * up to ASK_HALF_WIDTH_KANA_END. ASK_DAKUTEN and ASK_HANDAKUTEN are the
 * voiced and half-voiced marks among them */
#define ASK_HALF_WIDTH_KANA 0xa0
#define ASK_HALF_WIDTH_KANA_END 0xe0
#define ASK_DAKUTEN 0xde
#define ASK_HANDAKUTEN 0xdf
/* a letter's attributes. to_zenkaku and ext_jis make katakana of a letter
 * with ASK_AS_KATAKANA set, and hiragana of one without. roman_makedisp
 * shows a letter with ASK_FULL_WIDTH set in full width */
#define ASK_AS_KATAKANA 0x100
#define ASK_FULL_WIDTH 0x4000

int is_jis(Uint16 code);
/* whether byte begins a two-byte Shift-JIS character */
int is_kanji(Uint8 byte);
/* whether code is a two-byte Shift-JIS character */
int is_shift(Uint16 code);
/* write code's cells, and return how many: 1 or 2. code is a Shift-JIS or
 * JIS character, or else a byte */
int setmean(Uint16 *cells, Uint16 code, Uint16 attribute);
/* set the attribute of count cells to attribute */
void change_kind(Uint16 *cells, int count, Uint16 attribute);
/* the columns Shift-JIS code takes on the display */
int shiftlen(Uint16 code);
/* write the cells of the NUL-terminated Shift-JIS text, and return how
 * many */
int sstrtom(Uint16 *cells, const Uint8 *text, Uint16 attribute);
/* the reading byte of JIS code. a letter's or digit's is its ASCII, and a
 * mark's is the byte srch_ucode gives. a hiragana's is as ASK_KANA_BASE
 * says, and any other's is 0 */
Uint8 to_ucode(Uint16 code);
/* whether JIS code is a katakana, or with include_long_vowel the long vowel
 * mark too */
int is_kata(Uint16 code, int include_long_vowel);
/* whether JIS code lies in the kanji rows, from 0x30 */
int is_jisknj(Uint16 code);
/* whether JIS code lies in the rows before the kanji and isn't a katakana */
int is_jiskig(Uint16 code);
/* write count cells as Shift-JIS text, then a NUL, and return the bytes
 * before the NUL. a JIS pair becomes its Shift-JIS character. a second
 * byte's cell must follow its first byte's: meantosjis never returns from
 * one alone */
int meantosjis(const Uint16 *cells, Uint8 *text, int count);
/* the JIS code of Shift-JIS code, or 0 for a second byte out of range */
Uint16 sjis2jis(Uint16 code);
/* the Shift-JIS code of JIS code, or 0 for one out of range */
Uint16 jis2sjis(Uint16 code);
/* the full-width JIS code of code's low byte, or a black square for a byte
 * that has none. a kana is hiragana unless code holds ASK_AS_KATAKANA */
Uint16 to_zenkaku(Uint16 code);
/* the same for the keys romaji input reads as kana or marks: , - . / X x [
 * and ]. it returns 0 for any other */
Uint16 to_zenkaku_spec(Uint16 code);
/* the kana of JIS cell byte: katakana where attribute holds ASK_AS_KATAKANA,
 * and else hiragana */
Uint16 ext_jis(Uint8 byte, Uint16 attribute);
/* write the half-width form of JIS code into text, or a space for a code that
 * has none. it returns the bytes written: 2 where ASK_DAKUTEN or
 * ASK_HANDAKUTEN follows a kana, and else 1 */
int to_hankaku(Uint8 *text, Uint16 code);
/* whether the kana of JIS or Shift-JIS code has a voiced form, and for
 * can_handaku a half-voiced one */
int can_daku(Uint16 code);
int can_handaku(Uint16 code);
/* whether the hiragana of JIS cell byte has a small form */
int can_small(Uint8 byte);
/* the reading byte of JIS code, a mark, or 0 for none */
Uint8 srch_ucode(Uint16 code);
#endif
