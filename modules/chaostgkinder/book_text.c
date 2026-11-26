#include <sg_xpt.h>

#include "chaostgkinder/book_text.h"

static const Uint16 empty_code[1] = {
#include "chaostgkinder/book_text/empty_code.inc"
};

/* slots 1 to 95 hold '!' to '~' and then the space. each lists its ASCII
 * code and its full-width Shift JIS code */
static const Uint16 ascii_codes[95][3] = {
#include "chaostgkinder/book_text/ascii_codes.inc"
};

/* slots 96 to 159 list the ISO 8859-1 codes 0xc0 to 0xff. 0xbf and 0xa1
 * stand in place of 0xd7 and 0xf7 */
static const Uint16 latin_codes[64][2] = {
#include "chaostgkinder/book_text/latin_codes.inc"
};

/* slots 160 to 253 list katakana and Japanese punctuation, by their Shift
 * JIS codes */
static const Uint16 japanese_codes[94][2] = {
#include "chaostgkinder/book_text/japanese_codes.inc"
};

static const Uint16 reserved_code[1] = {
#include "chaostgkinder/book_text/reserved_code.inc"
};

static const Uint16 escape_code[1] = {
#include "chaostgkinder/book_text/escape_code.inc"
};

typedef const Uint16 *slot_codes;
/* slot n of the alphabet lists the codes that encode as n, ended by a 0.
 * slots 0, 254 and 255 list none. so no code encodes as BOOK_TEXT_MARK, or
 * as 0, which ends the text */
#define ALPHABET_SLOTS 256
static const slot_codes alphabet[ALPHABET_SLOTS] = {
#include "chaostgkinder/book_text/alphabet.inc"
};

/* the slot that lists code, or 0 */
static Uint16 alphabet_slot(Uint16 code)
{
    int slot;
    int listed;
    const Uint16 *codes;
    /* @hack spelling: the bound slot <= ALPHABET_SLOTS - 1, where
     * slot < ALPHABET_SLOTS would do */
    for (slot = 0; slot <= ALPHABET_SLOTS - 1; slot++) {
        codes = alphabet[slot];
        /* @hack spelling: each code read into listed inside the loop's
         * test, where a loop over codes would do */
        while ((listed = *codes++) != 0) {
            if (listed == code) {
                return slot;
            }
        }
    }
    return 0;
}

Sint32 book_encode_text(Uint8 *dst, const Uint16 *src, Sint32 limit)
{
    Uint8 *out = dst;
    int code;
    int slot;
    while (--limit >= 0 && (code = *src++) != 0) {
        if ((code >> 8) == BOOK_TEXT_MARK) {
            if (--limit >= 0) {
                *out++ = BOOK_TEXT_MARK;
                *out++ = code;
            }
        } else {
            slot = alphabet_slot(code);
            if (slot) {
                *out++ = slot;
            }
        }
    }
    *out = 0;
    return out - dst;
}
