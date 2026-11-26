/* the kindergarten's text: strings of 16-bit codes, one to a glyph, ended by
 * a 0 code. a packed string holds the same text in bytes, ended by a NUL.
 * packing writes a code below 0x100 as one byte, and any other as two, its
 * high byte first. unpacking reads a byte below 0x7f as a code of its own. a
 * byte of 0x7f or more is a code's high byte. the byte after it is the
 * code's low byte. so a code unpacks as itself only below 0x7f or from
 * 0x7f00 up */
#ifndef CHAOSTGKINDER_WIDE_TEXT_H
#define CHAOSTGKINDER_WIDE_TEXT_H
#include <sg_xpt.h>

Sint32 kinder_wide_length(const Uint16 *text);
void kinder_wide_copy(Uint16 *dst, const Uint16 *src);
/* kinder_wide_copy_n copies codes until it has copied a 0 or count codes.
 * it leaves dst unended where src has count codes or more before its 0.
 * kinder_wide_terminate copies the same way, then writes a 0, so its dst
 * needs count + 1 codes */
void kinder_wide_copy_n(Uint16 *dst, const Uint16 *src, Sint32 count);
void kinder_wide_terminate(Uint16 *dst, const Uint16 *src, Sint32 count);
/* the _n forms unpack or pack at most count codes, then end dst. unpacking
 * needs count + 1 codes of dst, and packing 2 * count + 1 bytes */
void kinder_text_unpack(Uint16 *dst, const char *src);
void kinder_text_unpack_n(Uint16 *dst, const char *src, Sint32 count);
void kinder_text_pack(const Uint16 *src, char *dst);
void kinder_text_pack_n(const Uint16 *src, char *dst, Sint32 count);
/* text needs room for 12 codes: a -, ten digits and the 0 */
void kinder_int_to_text(Uint16 *text, Sint32 value);
/* the decimal number at the start of text, after an optional -. it reads up
 * to the first code that isn't a digit */
Sint32 kinder_text_to_int(const Uint16 *text);
/* write format into dst, ended by a 0. %% writes a % and %S copies a string
 * of codes. %s writes each char of a string as a code, without unpacking
 * it. a char is signed, so a byte of 0x80 or more writes a code from 0xff80.
 * any other conversion writes nothing. format must not end in a lone %, and
 * dst needs room for the result */
void kinder_text_format(Uint16 *dst, const Uint16 *format, ...);
#endif
