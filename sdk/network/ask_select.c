#include "ask_session.h"
#include "ask_editor.h"
#include "ask_candidates.h"
#include "ask_phrases.h"
#include "ask_jis.h"
#include "ask_cells.h"
#include "ask_input.h"
#include "ask_dictionary.h"
#include <sg_xpt.h>
#include <string.h>

int learn_on;
int cur_pos;
int cur_len;
int cur_kh;
int sel_job;
static int reserved;

static int current_makedisp(int first, int length, Uint16 *cells);
static int first_makedisp(int first, int length, Uint16 *cells);
static void save_prevwd(int first, int length, union ask_candidate *candidate,
                        Bool fresh);
static struct ask_learn_word *raw_newwd(int first, int length,
                                        union ask_candidate *candidate);

int disp_select(void)
{
    Uint16 *cells;
    int first = 0;
    int total = 0;
    int length;
    int count;
    cells = meanbuf;
    while ((length = bunsetu_len(first)) != 0) {
        if (first == cur_pos) {
            count = current_makedisp(first, length, cells);
            cur_len = length;
            change_kind(cells, count, ASK_CURRENT_ATTRIBUTE);
        } else {
            count = first_makedisp(first, length, cells);
            change_kind(cells, count, ASK_CONVERTED_ATTRIBUTE);
        }
        cells += count;
        first += length;
        total += count;
    }
    return total;
}

static int current_makedisp(int first, int length, Uint16 *cells)
{
    union ask_candidate *candidate;
    if (sel_job == 1 || func_mode >= ASK_SESSION_SELECTING) {
        candidate = take_kouho(top_kh, gun_nkh);
    } else {
        candidate = take_kouho(hchar[first].candidates, 0);
    }
    return kouho_makedisp(first, length, candidate, cells);
}

static int first_makedisp(int first, int length, Uint16 *cells)
{
    return kouho_makedisp(first, length, hchar[first].candidates, cells);
}

/* drop the candidates of hchar[first] but one. under mode 1 during a
 * selection it keeps the one selected, and else the first */
void unify_khmem(int first, int mode)
{
    /* @hack redundant: candidate and next declared register, where plain locals
     * would do */
    register union ask_candidate *candidate;
    union ask_candidate *keep;
    register union ask_candidate *next;
    if (!(candidate = hchar[first].candidates)) {
        return;
    }
    if (mode == 1 && (sel_job == 1 || func_mode >= ASK_SESSION_SELECTING)) {
        keep = take_kouho(top_kh, gun_nkh);
        while (candidate != NULL) {
            if (candidate == keep) {
                break;
            }
            next = kh_followed(candidate);
            /* @bug memory: frees only the head cell of each candidate before
             * the one kept, at 0x0c9e986c. their continuation cells stay
             * taken until init_univmem rebuilds the pool */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: kh_skip ends the
             * candidate at its last cell, and free_khmemlist frees every cell
             * up to it */
            kh_skip(candidate);
            free_khmemlist(candidate);
#else
            free_mem(candidate);
#endif
            candidate = next;
        }
        hchar[first].candidates = candidate;
    }
#ifdef NONMATCHING
    /* this side frees nothing where no candidate is left, which kh_skip would
     * read through */
    if (!candidate) {
        return;
    }
#endif
    free_khmemlist(kh_skip(candidate));
}

Uint8 *select_tostr(void)
{
    meantosjis(meanbuf, outbuf, disp_select());
    return outbuf;
}

Uint8 *select_subtostr(int first, int length)
{
    int end = first + length;
    /* @hack redundant: cells declared register, where a plain local would do */
    register Uint16 *cells = meanbuf;
    int span;
    int count;
    for (; first < end && (span = bunsetu_len(first)) != 0; first += span) {
        if (first == cur_pos) {
            count = current_makedisp(first, span, cells);
        } else {
            count = first_makedisp(first, span, cells);
        }
        cells += count;
    }
    meantosjis(meanbuf, outbuf, cells - meanbuf);
    return outbuf;
}

static void kh_learn(int first, int length, union ask_candidate *candidate,
                     struct ask_phrase *phrases)
{
    struct ask_match *match;
    struct ask_learn_word *new_word;
    /* @hack redundant: id declared register, where a plain local would do */
    register Sint32 *id;
    Sint32 previous;
    Sint32 entry;
#ifdef NONMATCHING
    /* this side refuses a missing candidate, and repairs the bug tagged below:
     * it compares id with ASK_NO_ID, which the Uint16 can hold */
    if (!candidate) {
        return;
    }
    if (candidate->head.id == ASK_NO_ID) {
#else
    /* @bug logic: a candidate of no dictionary word is never learned as a new
     * word. its Uint16 id, zero-extended at 0x0c9e994a, never equals the -1
     * that 0x0c9e994c compares it with */
    /* NOLINTNEXTLINE(clang-diagnostic-tautological-*) */
    if (candidate->head.id == -1) {
#endif
        match = NULL;
    } else if (!(match = candidate->head.match)) {
        return;
    }
    id = ASK_LEARN_IDS;
    if (!phrases || phrases == ASK_NO_PHRASES) {
        if (match) {
            *id++ = match->entry;
        }
    } else {
        previous = -1;
        do {
            if (phrases->word) {
                entry = phrases->word->entry;
                if (entry != -1 && entry != previous) {
#ifdef NONMATCHING
                    /* this side stops at the end of ASK_LEARN_IDS's 256
                     * entries */
                    if (id == ASK_LEARN_IDS + 256) {
                        break;
                    }
#endif
                    *id++ = entry;
                    previous = entry;
                }
            }
            phrases = phrases->next;
        } while (phrases != NULL);
    }
    if (!match) {
        new_word = raw_newwd(first, length, candidate);
        dic_newlearn(new_word, ASK_LEARN_IDS, id - ASK_LEARN_IDS);
    } else {
        dic_learn(match->entry, candidate->head.id, ASK_LEARN_IDS,
                  id - ASK_LEARN_IDS);
    }
}

static void prev_learn(union ask_candidate *candidate)
{
    struct ask_learn_word word;
    struct ask_match *match;
    if (prev_yomi[0] != 0) {
        word.reading = prev_yomi;
        word.length = strlen((char *)prev_yomi);
        word.reserved = 0;
        if (!candidate || !(match = candidate->head.match)) {
            word.kind = ASK_RAW_WORD;
            word.form = 0;
        } else {
            word.kind = match->kind;
            word.form = match->form;
        }
        word.writing = prev_tango;
        dic_tmptouroku(&word);
    }
}

void wd_learn(int first, int end)
{
    struct ask_position *position;
    union ask_candidate *candidate;
    int length;
    for (; first < end && (length = bunsetu_len(first)) != 0; first += length) {
        position = &hchar[first];
        if (first == cur_pos &&
            (sel_job == 1 || func_mode >= ASK_SESSION_SELECTING)) {
            candidate = take_kouho(top_kh, gun_nkh);
        } else {
            candidate = take_kouho(position->candidates, 0);
        }
        if (im_state == 3 || func_mode > ASK_SESSION_CONVERTING) {
            /* @hack spelling: the gotos to join_previous and back to learn,
             * where an else if that ends in continue would do */
            if (position->first_length &&
                position->first_length != position->length) {
                save_prevwd(first, length, candidate, TRUE);
            } else {
                goto join_previous;
            }
        }
    learn:
        kh_learn(first, length, candidate, position->phrases);
        continue;
    join_previous:
        if (prev_yomi[0] == 0) {
            goto learn;
        }
        save_prevwd(first, length, candidate, FALSE);
        prev_learn(candidate);
        clear_prevwd();
    }
}

void clear_prevwd(void)
{
    prev_yomi[0] = 0;
}

static void save_prevwd(int first, int length, union ask_candidate *candidate,
                        Bool fresh)
{
    Uint8 *reading = prev_yomi;
    Uint8 *writing = prev_tango;
    if (fresh == FALSE) {
        reading += strlen((char *)prev_yomi);
        writing += strlen((char *)prev_tango);
        length = candidate->head.length;
    }
#ifdef NONMATCHING
    /* this side refuses a range outside the reading, and a reading too long for
     * the room prev_yomi has left */
    if (length < 0 || first < 0 || first > ASK_READING_BYTES ||
        length > ASK_READING_BYTES - first ||
        length >= (prev_yomi + sizeof(prev_yomi)) - reading) {
        clear_prevwd();
        return;
    }
#endif
    strncpy((char *)reading, (char *)&kana_ustr[first], length);
    reading[length] = 0;
    if (strlen((char *)reading) != (unsigned int)length) {
        clear_prevwd();
        return;
    }
#ifdef NONMATCHING
    /* this side converts into text first, and copies it only where prev_tango
     * has room */
    {
        Uint8 text[ASK_DISPLAY_CELLS + 1];
        int bytes = meantosjis(
            meanbuf, text, kouho_makedisp(first, length, candidate, meanbuf));
        if (bytes < 0 || bytes >= (prev_tango + sizeof(prev_tango)) - writing) {
            clear_prevwd();
            return;
        }
        memcpy(writing, text, bytes + 1);
    }
#else
    meantosjis(meanbuf, writing,
               kouho_makedisp(first, length, candidate, meanbuf));
#endif
}

static struct ask_learn_word *raw_newwd(int first, int length,
                                        union ask_candidate *candidate)
{
    static struct ask_learn_word new_word;
    (void)reserved;
    strncpy((char *)yomi_buf, (char *)&kana_ustr[first], length);
    yomi_buf[length] = 0;
    meantosjis(meanbuf, tango_buf,
               kouho_makedisp(first, length, candidate, meanbuf));
    new_word.reading = yomi_buf;
    new_word.length = length;
    new_word.reserved = 0;
    new_word.kind = ASK_PLAIN_WORD;
    new_word.form = 0;
    new_word.writing = tango_buf;
    return &new_word;
}
