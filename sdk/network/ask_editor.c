#include "ask_editor.h"
#include "ask_phrases.h"
#include "ask_input.h"
#include "ask_jis.h"
#include "ask_cells.h"
#include "ask_dictionary.h"
#include <sg_xpt.h>

/* the display cells of the candidate window and of the input window */
#define WINDOW_CELLS 72
/* U's cell in JIS X 0208, which VU's doesn't follow */
#define KANA_U 0x26
/* the Shift-JIS bytes of a full-width kana */
#define KANA_BYTES 2
/* the cell the cursor takes at the end of the input window */
#define CURSOR_CELLS 1

int im_state;
int save_im_state;
char dic_name[128];
Uint8 yomi_buf[80];
int ektable;
int undo_cur_pos;
int undo_im_state;
int undo_length;
Uint8 tango_buf[80];

int kwin_length(void)
{
    return WINDOW_CELLS;
}
int nwin_length(void)
{
    return WINDOW_CELLS;
}

void init_roman(void)
{
    pinpc_buf = qinpc_buf = cinpc_buf = einpc_buf = inpc_buf;
    pkana_buf = ekana_buf = kana_buf;
    p_ustr = e_ustr = kana_ustr;
    kana_len = 0;
}

void init_edit0(void)
{
    init_roman();
    free_hchar(0, ASK_POSITIONS, 0);
    clear_prevwd();
}

void trans_roman(Uint8 *output, int first, int count, int form)
{
    /* @hack redundant: kana, current, letter and previous declared register,
     * where plain locals would do */
    register struct ask_kana *kana = kana_buf;
    register struct ask_kana *current;
    struct ask_kana *limit;
    register Uint16 *letter;
    Uint16 code;
    register Uint16 previous;
    int start;
    int end;
    int i;
#ifdef NONMATCHING
    /* this side writes an empty text for a range outside kana_buf's kana_len
     * entries */
    if (first < 0 || count < 0 || first > kana_len ||
        count > kana_len - first) {
        *output = 0;
        return;
    }
#endif
    if (form >= ASK_TEXT_FULL_ROMAN) {
        start = 0;
        for (i = 0; i < first; i++, kana++) {
            start += kana->input_bytes;
        }
        end = start;
        for (i = 0; i < count; i++, kana++) {
            end += kana->input_bytes;
        }
        letter = &inpc_buf[start];
        for (; start < end; start++) {
            if (form == ASK_TEXT_FULL_ROMAN) {
                /* @hack spelling: the letter held in previous, where
                 * to_zenkaku could take it directly */
                previous = *letter++ | ASK_AS_KATAKANA;
                code = jis2sjis(to_zenkaku(previous));
                *output++ = code >> 8;
                *output++ = code;
            } else if (form == ASK_TEXT_ROMAN) {
                *output++ = *letter++;
            }
        }
    } else {
#ifdef DREAMCAST
        /* @hack redundant: a goto to the label right below it */
        goto convert;
    convert:
#endif
        previous = 0;
        current = kana + first;
        limit = current + count;
        for (; current < limit; current++) {
            code = current->code;
            if (form == ASK_TEXT_HIRAGANA) {
                if ((code & ASK_FIRST_BYTE) == 0) {
                    if (code == ASK_DAKUTEN && can_daku(previous)) {
                        output -= KANA_BYTES;
                        if (previous == (ASK_HIRAGANA | KANA_U)) {
                            code = ASK_HIRAGANA | ASK_KANA_VU;
                        } else {
                            code = previous + ASK_TO_VOICED;
                        }
                    } else if (code == ASK_HANDAKUTEN &&
                               can_handaku(previous)) {
                        output -= KANA_BYTES;
                        code = previous + ASK_TO_HALF_VOICED;
                    } else {
                        code = to_zenkaku(code);
                    }
                }
                if ((code & ASK_FIRST_BYTE) == ASK_KATAKANA) {
                    code &= ASK_HIRAGANA | 0xff;
                }
            } else if (form == ASK_TEXT_KATAKANA) {
                if ((code & ASK_FIRST_BYTE) == 0) {
                    if (code == ASK_DAKUTEN && can_daku(previous)) {
                        output -= KANA_BYTES;
                        if (previous == (ASK_KATAKANA | KANA_U)) {
                            code = ASK_KATAKANA | ASK_KANA_VU;
                        } else {
                            code = previous + ASK_TO_VOICED;
                        }
                    } else if (code == ASK_HANDAKUTEN &&
                               can_handaku(previous)) {
                        output -= KANA_BYTES;
                        code = previous + ASK_TO_HALF_VOICED;
                    } else {
                        code = to_zenkaku(code | ASK_AS_KATAKANA);
                    }
                }
                if ((code & ASK_FIRST_BYTE) == ASK_HIRAGANA) {
                    code |= ASK_KATAKANA;
                }
            } else if (form == ASK_TEXT_HALF_WIDTH) {
                output += to_hankaku(output, code);
                continue;
            } else if (form == ASK_TEXT_FULL_WIDTH) {
                if ((code & ASK_FIRST_BYTE) == 0) {
                    code = to_zenkaku(code | ASK_AS_KATAKANA);
                }
            }
            previous = code;
            if (code & ASK_FIRST_BYTE) {
                code = jis2sjis(code);
                *output++ = code >> 8;
            }
            *output++ = code;
#ifdef DREAMCAST
            /* @hack redundant: an unreachable store of ASK_AS_KATAKANA to
             * previous */
            if (0) {
                previous = ASK_AS_KATAKANA;
            }
            /* @hack redundant: the same store again, in a block of its own */
            if (0) {
                previous = ASK_AS_KATAKANA;
            }
            /* @hack redundant: an unreachable store of ASK_DAKUTEN to
             * previous */
            if (0) {
                previous = ASK_DAKUTEN;
            }
#endif
        }
    }
    *output = 0;
}

void into_editing(int mode)
{
    learn_on = dic_getgaku();
    init_univmem();
    init_hchar();
    init_edit0();
    if (mode == 1) {
        kana_buf_size = (nwin_length() - CURSOR_CELLS) / ASK_WIDE_CELLS;
        im_state = 1;
    }
}

int roman_makedisp(int first, int count, Uint16 *cells, int cursor)
{
    /* @hack redundant: kana and cell declared register, where plain locals
     * would do */
    register struct ask_kana *kana;
    struct ask_kana *limit;
    register Uint16 *cell;
    Uint16 *letter;
    Uint16 code;
    Bool marked = FALSE;
    int width;
    cell = cells;
#ifdef NONMATCHING
    /* this side draws nothing for a range that starts outside kana_buf's cells,
     * and cuts one that runs past them */
    if (first < 0 || count < 0 || first >= ASK_KANA_CELLS) {
        return 0;
    }
    if (count > ASK_KANA_CELLS - first) {
        count = ASK_KANA_CELLS - first;
    }
#endif
    for (kana = &kana_buf[first], limit = &kana_buf[first + count];
         kana < limit; kana++) {
        if (kana == pkana_buf) {
            for (letter = qinpc_buf; letter < pinpc_buf; letter++) {
                if (*letter & ASK_FULL_WIDTH) {
                    code = to_zenkaku(*letter);
                } else {
                    code = (Uint8)*letter;
                }
                if (cursor && letter == cinpc_buf) {
                    width = setmean(cell, code, ASK_CURSOR_ATTRIBUTE);
                    cursor = 0;
                } else {
                    width = setmean(cell, code, ASK_READING_ATTRIBUTE);
                }
                cell += width;
            }
            marked = TRUE;
        }
        if (kana >= ekana_buf) {
            break;
        }
        if (cursor && marked) {
            width = setmean(cell, kana->code, ASK_CURSOR_ATTRIBUTE);
            cursor = 0;
        } else {
            width = setmean(cell, kana->code, ASK_READING_ATTRIBUTE);
        }
        cell += width;
        marked = FALSE;
    }
    if (cursor) {
        width = setmean(cell, ' ', ASK_CURSOR_ATTRIBUTE);
        cell += width;
    }
    return cell - cells;
}
