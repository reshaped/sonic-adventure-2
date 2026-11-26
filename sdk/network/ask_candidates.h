#ifndef ASK_CANDIDATES_H
#define ASK_CANDIDATES_H
#include "ask_cells.h"
#include <sg_xpt.h>
#include "ask_session.h"
/* a word as the dictionary returns it. wdsbuf is 1024 bytes of scratch. it
 * holds such words, or the letters of a romaji input. it holds the texts of
 * the candidates made so far too, or the ids a selection learns */
struct ask_dictionary_word {
    Uint16 id;
    Uint16 frequency;
    Uint8 form;
    Uint8 text[1019];
};
/* the offset of struct ask_dictionary_word's text, as offsetof would give
 * it */
#define ASK_WORD_TEXT 5
union ask_word_buffer {
    struct ask_dictionary_word word;
    Sint32 ids[256];
    Uint16 letters[512];
    Uint8 bytes[1024];
};
#ifdef NONMATCHING
/* this side gives wdsbuf its union's views by name, the plain form the hack tag
 * below gives */
extern union ask_word_buffer wdsbuf;
#define ASK_ROMAN_LETTERS wdsbuf.letters
#define ASK_LEARN_IDS wdsbuf.ids
#define ASK_WORD_RESULTS (&wdsbuf.word)
#define ASK_NUMBER_RESULTS ASK_WORD_RESULTS
#define ASK_WORD_BYTES wdsbuf.bytes
#else
/* @hack type: wdsbuf, one ask_dictionary_word in an array, read through
 * casts, where union ask_word_buffer would do */
extern struct ask_dictionary_word wdsbuf[1];
#define ASK_ROMAN_LETTERS ((Uint16 *)wdsbuf)
#define ASK_LEARN_IDS ((Sint32 *)wdsbuf)
#define ASK_WORD_RESULTS ((struct ask_dictionary_word *)&wdsbuf)
#define ASK_NUMBER_RESULTS wdsbuf
#define ASK_WORD_BYTES ((Uint8 *)&wdsbuf)
#endif
extern int kwin_len;
extern union ask_candidate *top_kh;
extern int gun_num, gun_nkh;
extern Uint8 *candidate_text_end;
extern Uint8 kouho_head[], kouho_rest[];
extern int cur_pos, cur_len, cur_kh, sel_job;
union ask_candidate *take_kouho(union ask_candidate *head, int count);
union ask_candidate *kh_skip(union ask_candidate *candidate);
union ask_candidate *kh_followed(union ask_candidate *candidate);
int kouho_makedisp(int first, int length, union ask_candidate *candidate,
                   Uint16 *cells);
int disp_select(void);
void unify_khmem(int first, int mode);
Uint8 *select_tostr(void);
Uint8 *select_subtostr(int first, int length);
void wd_learn(int first, int end);
#endif
