#ifndef ASK_EDITOR_H
#define ASK_EDITOR_H
#include <sg_xpt.h>

/* the forms trans_roman writes kana in. ASK_TEXT_ORIGINAL keeps each kana as
 * kana_buf holds it. ASK_TEXT_HIRAGANA and ASK_TEXT_KATAKANA join a
 * half-width dakuten or handakuten to the kana before it. ASK_TEXT_FULL_ROMAN
 * and ASK_TEXT_ROMAN write the letters typed for the kana instead, in full
 * width and as typed */
enum ask_text_form {
    ASK_TEXT_ORIGINAL,
    ASK_TEXT_HIRAGANA,
    ASK_TEXT_KATAKANA,
    ASK_TEXT_HALF_WIDTH,
    ASK_TEXT_FULL_WIDTH,
    ASK_TEXT_FULL_ROMAN,
    ASK_TEXT_ROMAN
};
extern int im_state, save_im_state;
extern char dic_name[128];
extern Uint8 yomi_buf[80];
extern int ektable, undo_cur_pos, undo_im_state, undo_length;
extern Uint8 tango_buf[80];
extern int learn_on;
void clear_prevwd(void);
int kwin_length(void);
int nwin_length(void);
void init_roman(void);
void init_edit0(void);
/* write kana_buf's count kana from first into output in form, then a NUL */
void trans_roman(Uint8 *output, int first, int count, int form);
void into_editing(int mode);
/* write the display cells of kana_buf's count kana from first into cells,
 * and return how many it wrote. the letters pending at pkana_buf take cells
 * too. with cursor nonzero, the cell at the cursor takes ASK_CURSOR_ATTRIBUTE.
 * it is the letter at cinpc_buf, the kana after the pending letters, or a blank
 * at the end */
int roman_makedisp(int first, int count, Uint16 *cells, int cursor);
#endif
