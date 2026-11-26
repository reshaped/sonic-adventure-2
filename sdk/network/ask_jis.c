#include "ask_jis.h"
#include "sg_xpt.h"

#define JIS_FIRST 0x21
#define JIS_LAST 0x7e
#define SJIS_LEAD 0x81
#define SJIS_LAST 0xfc
/* a Shift-JIS trail byte runs from SJIS_TRAIL to SJIS_LAST, but for
 * SJIS_NO_TRAIL */
#define SJIS_TRAIL 0x40
#define SJIS_NO_TRAIL 0x7f
#define LONG_VOWEL 0x213c
/* the first codes of the rows is_jiskig and is_jisknj take */
#define SYMBOL_ROWS 0x2120
#define KANJI_ROWS 0x3020

int is_jis(Uint16 code)
{
    /* @hack redundant: row and cell declared register, where plain locals would
     * do */
    register Uint8 row = code >> 8;
    register Uint8 cell = code;
    return row >= JIS_FIRST && row <= JIS_LAST && cell >= JIS_FIRST &&
           cell <= JIS_LAST;
}
int is_kanji(Uint8 byte)
{
    if (byte < SJIS_LEAD || byte > SJIS_LAST ||
        (byte >= ASK_HALF_WIDTH_KANA && byte < ASK_HALF_WIDTH_KANA_END)) {
        return 0;
    }
    return 1;
}
int is_shift(Uint16 code)
{
    /* @hack redundant: lead and trail declared register, where plain locals
     * would do */
    register Uint8 lead = code >> 8;
    register Uint8 trail = code;
    if (!is_kanji(lead)) {
        return 0;
    }
    if (trail < SJIS_TRAIL || trail > SJIS_LAST || trail == SJIS_NO_TRAIL) {
        return 0;
    }
    return 1;
}
int setmean(Uint16 *cells, Uint16 code, Uint16 attribute)
{
    int flags = attribute << ASK_ATTRIBUTE_SHIFT;
    if (is_shift(code)) {
        if (shiftlen(code) == 1) {
            flags |= ASK_NARROW_KIND << ASK_KIND_SHIFT;
        } else {
            flags |= ASK_WIDE_KIND << ASK_KIND_SHIFT;
        }
    } else if (is_jis(code)) {
        flags |= ASK_JIS_KIND << ASK_KIND_SHIFT;
        if (code == (ASK_HIRAGANA | ASK_KANA_VU)) {
            code = ASK_KATAKANA | ASK_KANA_VU;
        }
    } else {
        *cells = (Uint8)code | flags;
        return 1;
    }
    *cells++ = (code >> 8) | flags;
    flags += 1 << ASK_KIND_SHIFT;
    flags |= (Uint8)code;
    *cells = flags;
    return 2;
}
void change_kind(Uint16 *cells, int count, Uint16 attribute)
{
    int flags = attribute << ASK_ATTRIBUTE_SHIFT;
    while (count--) {
        *cells = (*cells & ((1 << ASK_ATTRIBUTE_SHIFT) - 1)) | flags;
        cells++;
    }
}
int shiftlen(Uint16 code)
{
    switch (code & ASK_FIRST_BYTE) {
    case 0x8000:
    case 0x8500:
        return 1;
    case 0x8600:
        if ((Uint8)code < 0x9e) {
            return 1;
        }
    }
    return 2;
}
int sstrtom(Uint16 *cells, const Uint8 *text, Uint16 attribute)
{
    Uint16 *out = cells;
    int length;
    while (*text) {
        if (*text < 0x80 ||
            (*text >= ASK_HALF_WIDTH_KANA && *text < ASK_HALF_WIDTH_KANA_END)) {
            length = setmean(out, *text++, attribute);
            out += length;
        } else {
#ifdef NONMATCHING
            /* this side stops at a first byte whose second is the NUL */
            if (!text[1]) {
                break;
            }
#endif
            length = setmean(out, (text[0] << 8) | text[1], attribute);
            out += length;
            text += 2;
        }
    }
    return out - cells;
}
Uint8 to_ucode(Uint16 code)
{
    Uint8 low;
    if (code > 0x20 && code < 0x7f) {
        return 0;
    }
    low = code & 0x7f;
    switch (code & ASK_FIRST_BYTE) {
    case ASK_ALPHANUMERIC:
        return low;
    case ASK_HIRAGANA:
        return low | ASK_CELL_TO_READING;
    case ASK_KATAKANA:
        return 0;
    default:
        return srch_ucode(code);
    }
}
int is_kata(Uint16 code, int include_long_vowel)
{
    if (include_long_vowel && code == LONG_VOWEL) {
        return 1;
    }
    if ((code & ASK_FIRST_BYTE) == ASK_KATAKANA) {
        return 1;
    }
    return 0;
}
int is_jisknj(Uint16 code)
{
    if (code >= KANJI_ROWS) {
        return 1;
    }
    return 0;
}
int is_jiskig(Uint16 code)
{
    if (code >= SYMBOL_ROWS && code < KANJI_ROWS) {
        if (!is_kata(code, 0)) {
            return 1;
        }
    }
    return 0;
}
int meantosjis(const Uint16 *cells, Uint8 *text, int count)
{
    /* @hack redundant: i and out declared register, where plain locals would
     * do */
    register int i;
    register Uint8 *out;
    Uint16 kind;
    Uint16 code;
    /* @hack spelling: first, a local that holds 1, where 1 would do */
    int first = 1;
    for (i = 0, out = text; i < count;) {
        kind = (*cells >> ASK_KIND_SHIFT) & ASK_KIND_MASK;
        if (!kind) {
            /* @hack spelling: out stepped in a statement of its own, where
             * *out++ would do */
            *out = *cells++;
            out = out + 1;
            i++;
            continue;
        }
        if (kind & first) {
            code = *cells++ << 8;
            i++;
#ifdef NONMATCHING
            /* this side stops after a first byte's cell that ends the text.
             * the matching side reads the cell past it */
            if (i == count) {
                break;
            }
#endif
            if (((*cells >> ASK_KIND_SHIFT) & ASK_KIND_MASK) == kind + first) {
                code |= *cells++ & 0xff;
                i++;
                if (is_jis(code)) {
                    code = jis2sjis(code);
                }
                *out++ = code >> 8;
                *out++ = code;
            }
#ifdef NONMATCHING
        } else {
            /* a second byte's cell alone, which the matching side never
             * steps past */
            cells++;
            i++;
#endif
        }
    }
    *out = 0;
    return out - text;
}
Uint16 sjis2jis(Uint16 code)
{
    Uint8 lead = code >> 8;
    Uint8 trail = code;
    /* @hack redundant: row declared register, where a plain local would do */
    register int row;
    if (trail < SJIS_TRAIL || trail > SJIS_LAST || trail == SJIS_NO_TRAIL) {
        return 0;
    }
    /* each lead byte holds two JIS rows. the odd row's cells take trails
     * from 0x40, and the even row's from 0x9f */
    if (lead >= 0xe0) {
        lead -= 0x40;
    }
    row = lead - SJIS_LEAD;
    row = (row * 2) + JIS_FIRST;
    if (trail >= 0x9f) {
        row++;
        trail -= 0x7e;
    } else if (trail >= 0x80) {
        trail -= 0x20;
    } else {
        trail -= 0x1f;
    }
    return ((Uint8)row << 8) | trail;
}
Uint16 jis2sjis(Uint16 code)
{
    Uint8 row = code >> 8;
    Uint8 cell = code;
    if (code < 0x2121 || code > 0x7e7e) {
        return 0;
    }
    /* @hack spelling: cell tested against JIS_LAST + 1, where cell >
     * JIS_LAST would do */
    if (cell < JIS_FIRST || cell >= JIS_LAST + 1) {
        return 0;
    }
    cell += 0x1f;
    if (row & 1) {
        row >>= 1;
    } else {
        row >>= 1;
        row--;
        cell += 0x5e;
    }
    if (cell >= 0x7f) {
        cell++;
    }
    if (row < 0x2f) {
        row += 0x71;
    } else {
        row += 0xb1;
    }
    return (row << 8) | cell;
}
