#include "ask_input.h"
#include "ask_jis.h"
#include "ask_roman.h"
#include <sg_xpt.h>

Sint32 roman_japan;
Sint16 ic_flag;
Sint32 kana_buf_size;
Uint16 inpc_buf[ASK_INPUT_CELLS];
Uint16 *pinpc_buf, *qinpc_buf, *cinpc_buf, *einpc_buf;
struct ask_kana kana_buf[ASK_KANA_CELLS];
struct ask_kana *pkana_buf, *ekana_buf;
Sint32 kana_len;
Uint8 kana_ustr[ASK_READING_BYTES];
Uint8 *p_ustr, *e_ustr;
Uint16 meanbuf[ASK_DISPLAY_CELLS];
Uint8 outbuf[ASK_DISPLAY_CELLS];
Uint16 ud_inpc_buf[ASK_INPUT_CELLS];
Sint32 ud_inpc_len;
struct ask_kana ud_kana_buf[ASK_KANA_CELLS];
Uint8 ud_kana_ustr[ASK_READING_BYTES];
Sint32 ud_kana_len;

/* kana as ext_jis takes them: the cells of their JIS X 0208 codes. a kana's
 * cell is the same in hiragana and katakana */
#define KANA_N 0x73
#define KANA_SMALL_TSU 0x43
/* U in half-width katakana, as Shift-JIS codes it */
#define HALF_WIDTH_U 0xb3
/* the letters of the longest spellings, LTSU and XTSU */
#define MOST_LETTERS 4
/* the most display columns the reading in kana_buf takes */
#define READING_COLUMNS 79
/* the bytes of a vowel's kana in an rmtab row */
#define VOWEL_BYTES 2

static char *getrda2(const Sint16 *letters, const Sint16 *end);
static char *getrda1(const Sint16 *letters, const Sint16 *end);
enum pending_kana {
    PENDING_CONSONANT,
    PENDING_N,
    PENDING_VOICED,
    PENDING_HALF_VOICED
};

int to_roman(const Sint16 *letters, int count, Uint16 *kana, int *produced)
{
    Uint8 *classes;
    Uint8 next_kind;
    int initial_kind;
    int consumed;
    Uint16 pending;
    /* @hack redundant: cursor declared register, where a plain local would
     * do */
    register const Sint16 *cursor;
    Sint16 letter;
    Uint8 byte;
    char *reading;
#ifdef NONMATCHING
    /* this side produces nothing for no letters, which the read below would
     * take past */
    if (count <= 0) {
        *produced = 0;
        return 0;
    }
#endif
    byte = *letters;
    classes = rmtype;
    next_kind = classes[byte] & ROMAN_KIND_MASK;
    *produced = 1;
    if ((initial_kind = next_kind) <= ROMAN_PUNCTUATION) {
        if (initial_kind <= ROMAN_O) {
#ifdef DREAMCAST
            /* @hack redundant: a goto to the label right below it */
            goto standalone_vowel;
        standalone_vowel:
#endif
            reading = rmtab[0].vowel[0];
            consumed = 1;
            cursor = letters;
            goto select_vowel;
        }
        if (initial_kind <= ROMAN_OTHER || roman_japan == 0) {
            *kana = to_zenkaku(*letters);
        } else {
            *kana = to_zenkaku_spec(*letters);
        }
        return 1;
    } else {
        pending = PENDING_CONSONANT;
    }
    if (initial_kind <= ROMAN_N) {
        if (initial_kind == ROMAN_N) {
            /* @hack spelling: attribute, a copy of *letters, where *letters
             * itself would do */
            Uint16 attribute = *letters;
            *kana = ext_jis(KANA_N, attribute);
            pending = PENDING_N;
        } else {
            *kana = to_zenkaku(*letters);
            if (initial_kind == ROMAN_VOICED) {
                pending = PENDING_VOICED;
            } else {
                pending = PENDING_HALF_VOICED;
            }
        }
    }
    if (count == 1) {
        /* @hack spelling: result, a copy of pending, where pending itself
         * would do */
        int result = pending;
        if (result == PENDING_CONSONANT) {
            return 0;
        }
        return -1;
    }
    cursor = letters + 1;
    letter = *cursor;
    if ((Uint8)*letters == (Uint8)letter) {
        if (initial_kind == ROMAN_N) {
            return 2;
        }
        if (initial_kind == ROMAN_CONSONANT) {
            *kana = ext_jis(KANA_SMALL_TSU, *cursor);
            return 1;
        }
    }
    if ((Uint8)*letters == 'x' || (Uint8)*letters == 'X') {
        /* @hack redundant: cursor and letter set again to the second letter,
         * which they hold already */
        cursor = letters + 1;
        letter = *cursor;
        if ((Uint8)letter == 'n' || (Uint8)letter == 'N') {
            *kana = ext_jis(KANA_N, *letters);
            return 2;
        }
    }
    if (pending == PENDING_N && (Uint8)letter == '\'') {
        return 2;
    }
    byte = letter;
    next_kind = classes[byte] & ROMAN_KIND_MASK;
    if (next_kind == ROMAN_HANDAKUTEN && pending == PENDING_HALF_VOICED) {
        *kana += ASK_TO_HALF_VOICED;
        return 2;
    }
    if (next_kind == ROMAN_DAKUTEN && pending >= PENDING_VOICED) {
        *kana += ASK_TO_VOICED;
        /* VU's cell doesn't follow U's, so U's voiced form is set apart */
        if ((Uint8)*letters == HALF_WIDTH_U) {
            *kana = ext_jis(ASK_KANA_VU, *cursor);
        }
        return 2;
    }
    if (pending >= PENDING_VOICED) {
        return 1;
    }
    if (next_kind > ROMAN_O) {
        if (next_kind <= ROMAN_Y - 1 && pending == PENDING_N) {
            return 1;
        }
    }
    consumed = 2;
    for (;;) {
        if (next_kind <= ROMAN_O) {
            if (!(reading = getrda2(letters, cursor + 1))) {
                if (!(reading = getrda1(letters, cursor))) {
                    return -2;
                }
            select_vowel:
                reading += (next_kind - ROMAN_A) * VOWEL_BYTES;
            }
            *kana++ = ext_jis(*reading++, *cursor);
            if (*reading) {
                *kana = ext_jis(*reading, *cursor);
                *produced = 2;
            }
            return consumed;
        }
        if (consumed++ >= count) {
            break;
        }
        ++cursor;
        byte = *cursor;
        next_kind = classes[byte] & ROMAN_KIND_MASK;
        if ((next_kind >= ROMAN_DAKUTEN && next_kind <= ROMAN_N) ||
            consumed > MOST_LETTERS) {
            return -2;
        }
    }
    return 0;
}

static Uint8 ascii_upper(Uint8 byte)
{
    if (byte >= 'a' && byte <= 'z') {
        return byte - ('a' - 'A');
    }
    return byte;
}

/* look up the spelling from letters up to end in rmspec, the spellings rmtab
 * lacks. the spelling ends with its vowel, and its letters match in either
 * case. it returns the spelling's kana, or NULL where rmspec has none */
static char *getrda2(const Sint16 *letters, const Sint16 *end)
{
    int length;
    /* @hack redundant: spelling and cursor declared register, where plain
     * locals would do */
    register char *spelling;
    int remaining;
    register const Sint16 *cursor;
    int key_length;
    length = end - letters;
    for (spelling = rmspec; *spelling;) {
        key_length = (Uint8)*spelling++;
        if (length == key_length) {
            for (cursor = letters, remaining = length; remaining > 0;
                 remaining--, spelling++) {
                if ((Uint8)*spelling != ascii_upper(*cursor++)) {
                    break;
                }
            }
            if (remaining == 0) {
                return spelling;
            }
            spelling += remaining;
        } else {
            spelling += key_length;
        }
        while (*spelling++ != 0) {
        }
    }
    return NULL;
}

/* the kana for A of the rmtab row the consonants from letters up to end
 * spell. to_roman steps from it to the vowel's. row 0 takes no consonant.
 * any other row takes 1 or 2, which match in either case. it returns NULL
 * where no row has those consonants */
static char *getrda1(const Sint16 *letters, const Sint16 *end)
{
    int length;
    int initial;
    struct ask_roman_row *row;
    struct ask_roman_row *limit;
    int following;
    length = end - letters;
    if (!length) {
        return rmtab[0].vowel[0];
    }
    if (length != 1 && length != 2) {
        return NULL;
    }
    initial = (Uint8)*letters;
    if (initial >= 'a' && initial <= 'z') {
        initial -= 'a';
    } else if (initial >= 'A' && initial <= 'Z') {
        initial -= 'A';
    } else {
        return NULL;
    }
    row = prmtab[initial];
#ifdef NONMATCHING
    /* this side returns NULL for an initial prmtab has no row for */
    if (!row) {
        return NULL;
    }
#endif
    if (length == 1) {
        return row->vowel[0];
    }
    do {
        initial++;
        limit = prmtab[initial];
    } while (!limit);
    following = ascii_upper(letters[1]);
    for (row++; row < limit; row++) {
        if ((Uint8)row->consonant == following) {
            return row->vowel[0];
        }
    }
    return NULL;
}

static int kana_display_width(struct ask_kana *first, struct ask_kana *end);

/* @hack redundant: text declared register, where a plain parameter would do */
int add_kana_buf(register const char *text)
{
    Uint16 lead;
    int count;
    Uint8 *reading;
    struct ask_kana *kana;
    int width;
    Uint16 code;
    int byte;
    lead = 0;
    kana = pkana_buf;
    reading = p_ustr;
    count = kana_len;
    width = kana_display_width(kana_buf, pkana_buf);
    for (; *text; text++) {
        if (lead) {
            lead = lead << 8 | (Uint8)*text;
            if ((code = sjis2jis(lead)) != 0) {
                if (count >= ASK_KANA_LIMIT || width >= READING_COLUMNS - 1) {
                    return -1;
                }
                kana->code = code;
                kana->input_bytes = 2;
                kana++;
                *reading++ = to_ucode(code);
                count++;
                width += 2;
            }
            lead = 0;
        } else if (is_kanji(*text)) {
            lead = (Uint8)*text;
        } else {
            byte = (Uint8)*text;
            if ((byte >= 0x20 && byte < 0x7f) ||
                (byte >= ASK_HALF_WIDTH_KANA &&
                 byte < ASK_HALF_WIDTH_KANA_END)) {
                if (count >= ASK_KANA_LIMIT || width >= READING_COLUMNS) {
                    return -1;
                }
                count++;
                kana->code = (Uint8)*text;
                width++;
                kana->input_bytes = 1;
                kana++;
                /* @hack spelling: the store through reading and its step as
                 * statements of their own, where *reading++ = 0 would do */
                *reading = 0;
                reading = reading + 1;
                lead = 0;
            }
        }
    }
    pkana_buf = kana;
    ekana_buf = kana;
    p_ustr = reading;
    e_ustr = reading;
    kana_len = count;
    return count;
}

static int kana_display_width(struct ask_kana *first, struct ask_kana *end)
{
    int width;
    for (width = 0; first < end; first++) {
        if (first->code & ASK_FIRST_BYTE) {
            width += 2;
        } else {
            width += 1;
        }
    }
    return width;
}

int del_kana_buf(int bytes)
{
    int glyphs;
    struct ask_kana *kana;
    if (bytes > 0) {
#ifdef NONMATCHING
        /* this side walks back no further than kana_buf's start, and stops once
         * bytes are spent. the matching side's loop can end on a pointer before
         * kana_buf, which C leaves undefined */
        for (kana = pkana_buf, glyphs = 0; kana > kana_buf && bytes > 0;
             glyphs++) {
            bytes -= (--kana)->input_bytes;
        }
#else
        kana = pkana_buf - 1;
        glyphs = 1;
        /* @hack redundant: a goto to the loop right below it */
        goto scan;
    scan:
        /* @hack spelling: kana = kana - 1, where kana-- would do */
        for (; kana >= kana_buf; kana = kana - 1, glyphs++) {
            bytes -= kana->input_bytes;
            if (bytes <= 0) {
                break;
            }
        }
#endif
        if (bytes != 0) {
            return -1;
        }
        kana_len -= glyphs;
        pkana_buf -= glyphs;
        ekana_buf -= glyphs;
        p_ustr -= glyphs;
        e_ustr -= glyphs;
    }
    return kana_len;
}
int count_byte_kana_buf(int first, int count)
{
    struct ask_kana *kana;
    int bytes = 0;
#ifdef NONMATCHING
    /* this side refuses a range outside kana_buf's kana_len entries */
    if (first < 0 || first > kana_len || count > kana_len - first) {
        return -1;
    }
#endif
    for (kana = &kana_buf[first]; count > 0; count--, kana++) {
        bytes += kana->input_bytes;
    }
    return bytes;
}
