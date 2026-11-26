#include <apiask.h>
#include "ask_candidates.h"
#include "ask_cells.h"
#include "ask_conversion.h"
#include "ask_dictionary.h"
#include "ask_editor.h"
#include "ask_input.h"
#include "ask_jis.h"
#include "ask_phrases.h"
#include "sg_xpt.h"
#include "ask_session.h"
#include <string.h>

#ifdef NONMATCHING
/* this side defines wdsbuf as union ask_word_buffer, the plain form the hack
 * tag in ask_candidates.h gives */
union ask_word_buffer wdsbuf;
#else
struct ask_dictionary_word wdsbuf[1];
#endif
int kwin_len;
union ask_candidate *top_kh;
int gun_num, gun_nkh;
Uint8 *candidate_text_end;
/* the labels of a group of candidates. kouho_rest comes first, with the
 * count of those left at REST_COUNT. kouho_head comes before each candidate,
 * with its number at HEAD_NUMBER */
#ifdef NONMATCHING
/* this side's labels hold the byte the matching side's lack, which
 * set_dictionary_name copies in */
Uint8 kouho_head[5], kouho_rest[11];
#else
/* a byte short of the labels set_dictionary_name copies in */
Uint8 kouho_head[4], kouho_rest[10];
#endif
#pragma section ATA
static Uint8 blank[] = " ";
#pragma section

enum {
    GROUP_HEADER_WIDTH = 9, /* the cells before a group's first label */
    HEAD_NUMBER = 2,
    REST_COUNT = 5
};
/* the cells a group of candidates may fill while a session is open */
#define SESSION_WINDOW_CELLS 80
static void build_candidates(void);
static union ask_candidate *phrase_candidates(struct ask_phrase *phrase);
static void release_candidates(struct ask_candidate_list *list);
static union ask_candidate *unchanged_candidate(int length);
static union ask_candidate *copy_candidate(struct ask_dictionary_word *word,
                                           struct ask_match *match, int length,
                                           union ask_candidate **head,
                                           union ask_candidate **tail);
static int copy_characters(Uint8 *output, Uint8 *text, int room);
static union ask_candidate *raw_candidate(int first, int length, int mode);
static void merge_candidates(int first, struct ask_candidate_list *list);
static union ask_candidate *take_best(struct ask_candidate_list *lists);
static void remember_candidates(int first, union ask_candidate *candidate);
static void append_unique(int first, union ask_candidate **head,
                          union ask_candidate **tail,
                          union ask_candidate *candidate);
static int seen_text(Uint8 *text, int length);
static union ask_candidate *last_cell(union ask_candidate *candidate);
static int candidate_length(union ask_candidate *candidate);
static void draw_group(void);
static void write_count(int count, Uint8 *output);
static int draw_word(union ask_candidate *candidate, Uint16 *cells);
static int group_count(union ask_candidate *candidate);

void first_kouho(int first, int length)
{
    struct ask_position *position;
    /* @hack redundant: phrase declared register, where a plain local would
     * do */
    register struct ask_phrase *phrase;
    struct ask_phrase *best;
    struct ask_match *match;
    struct ask_match *word;
    Uint16 rank;
    Uint16 highest;
    union ask_candidate *head;
    union ask_candidate *tail;
    best = NULL;
    highest = 0;
    position = &hchar[first];
    if (position->candidates) {
        return;
    }
    phrase = position->phrases;
    if (phrase == ASK_NO_PHRASES) {
    unchanged:
        position->candidates = unchanged_candidate(length);
        return;
    }
    for (; phrase; phrase = phrase->next) {
        if (phrase->length == length) {
            word = phrase->word;
            rank = !word ? 0 : kh_priority(phrase, word->frequency);
            if (rank > highest || !best) {
                highest = rank;
                best = phrase;
            }
        }
    }
    if (!best) {
        goto unchanged;
    }
    match = best->word;
    if (!match || !match->length) {
        goto unchanged;
    }
    if (match->entry == -1) {
        if (match->kind != ASK_NUMBER_WORD) {
            goto unchanged;
        }
        if (dic_get1num(kana_ustr + first, match->length, ASK_NUMBER_RESULTS) >
            ASK_DIC_NONE) {
            goto found;
        }
        goto unchanged;
    } else {
        if (dic_get1wd(match->entry, match->kind, match->form,
                       ASK_WORD_RESULTS) <= ASK_DIC_NONE) {
            goto unchanged;
        }
    }
found:
    position->candidates =
        copy_candidate(ASK_WORD_RESULTS, match, match->length, &head, &tail);
}

void init_kouho(int start, int reset)
{
    union ask_candidate *candidate;
    int count;
    if (reset == 1) {
        build_candidates();
    }
    candidate = hchar[cur_pos].candidates;
    if (func_mode > ASK_SESSION_CLOSED) {
        kwin_len = SESSION_WINDOW_CELLS;
    } else {
        kwin_len = kwin_length();
    }
    count = group_count(candidate);
#ifdef NONMATCHING
    /* this side clears the window for a candidate with no group, where the loop
     * below would never end */
    if (!count) {
        top_kh = NULL;
        gun_nkh = gun_num = 0;
        return;
    }
#endif
    while (start >= count) {
        start -= count;
        while (count--) {
            candidate = kh_followed(candidate);
        }
        if (!(count = group_count(candidate))) {
            init_kouho(0, 0);
            goto display;
        }
    }
    top_kh = candidate;
    gun_nkh = start;
    gun_num = count;
display:
    if (reset == 1 && func_mode == ASK_SESSION_CLOSED) {
        draw_group();
    }
}

static void build_candidates(void)
{
    struct ask_phrase *phrase;
    struct ask_candidate_list *head;
    struct ask_candidate_list *tail;
    struct ask_candidate_list *item;
    union ask_candidate *candidate;
    struct ask_position *position = &hchar[cur_pos];
    phrase = position->phrases;
    if (phrase == ASK_NO_PHRASES) {
        return;
    }
    head = NULL;
    tail = NULL;
    for (; phrase; phrase = phrase->next) {
        if (phrase->length != cur_len) {
            continue;
        }
        if (!(item = alloc_mem())) {
            release_candidates(head);
#ifdef NONMATCHING
            /* this side repairs the bug tagged below: it frees the lists and
             * returns */
            free_klmemlist(head);
            return;
#else
            /* @bug memory: merges freed cells into the candidates. the call
             * at 0x0c9ecd10 frees the first cell of each list, at 0x0c9ece9a,
             * and the branch after it still merges the lists */
            break;
#endif
        }
        candidate = phrase_candidates(phrase);
        item->candidates = candidate;
        item->phrase = phrase;
        item->priority =
            !candidate ? 0 : kh_priority(phrase, candidate->head.priority);
        item->next = NULL;
        if (!head) {
            head = tail = item;
        } else {
            tail->next = item;
            tail = item;
#ifdef DREAMCAST
            /* @hack redundant: an empty test of head, which control never
             * reaches */
            if (0) {
                if (head) {
                }
            }
#endif
        }
    }
#ifdef DREAMCAST
    /* @hack redundant: a goto to the label right below it */
    goto sort;
sort:
#endif
    merge_candidates(cur_pos, head);
    free_klmemlist(head);
}

static union ask_candidate *phrase_candidates(struct ask_phrase *phrase)
{
    int count;
    struct ask_match *match = phrase->word;
    union ask_candidate *head;
    union ask_candidate *tail;
    union ask_candidate *first;
    union ask_candidate *last;
    /* @hack redundant: word declared register, where a plain local would do */
    register Uint8 *word;
    if (!match || !match->length) {
    unchanged:
        head = unchanged_candidate(phrase->length);
        return head;
    }
    if (match->entry == -1) {
        if (match->kind != ASK_NUMBER_WORD) {
            goto unchanged;
        }
        if (dic_getallnum(kana_ustr + cur_pos, match->length,
                          ASK_NUMBER_RESULTS, &count) > ASK_DIC_NONE) {
            goto found;
        }
        goto unchanged;
    }
    if (dic_getallwd(match->entry, match->kind, match->form, ASK_WORD_RESULTS,
                     &count) <= ASK_DIC_NONE) {
        goto unchanged;
    }
found:
    word = ASK_WORD_BYTES;
    head = tail = NULL;
    while (count > 0) {
        if (!copy_candidate((struct ask_dictionary_word *)word, match,
                            match->length, &first, &last)) {
            free_khmemlist(head);
            return NULL;
        }
        if (!head) {
            head = first;
        } else {
#ifdef DREAMCAST
            /* @hack redundant: a goto to the label right below it */
            goto append;
        append:
#endif
            tail->head.next = first;
        }
        tail = last;
        word += ASK_WORD_TEXT;
        while (*word++) {
        }
        if ((Uint32)word & 1) {
            word++;
        }
        count--;
    }
    return head;
}

/* the matching side frees only the first cell of each list's candidates */
static void release_candidates(struct ask_candidate_list *list)
{
    for (; list; list = list->next) {
#ifdef NONMATCHING
        /* this side frees every cell of each list's candidates */
        free_khmemlist(list->candidates);
#else
        free_mem(list->candidates);
#endif
    }
}
static union ask_candidate *unchanged_candidate(int length)
{
    union ask_candidate *candidate;
    if ((candidate = alloc_mem()) != NULL) {
        candidate->head.flags = ASK_CANDIDATE_RAW;
        candidate->head.text[0] = 0;
        candidate->head.length = length;
        candidate->head.form = 0;
        candidate->head.match = NULL;
        candidate->head.id = ASK_NO_ID;
        candidate->head.next = NULL;
    }
    return candidate;
}
/* word's text as a candidate for match, which takes length bytes of the
 * reading. the text fills as many cells as it needs. *head and *tail get the
 * first cell and the last. it returns *head, or NULL where no cell is free */
static union ask_candidate *copy_candidate(struct ask_dictionary_word *word,
                                           struct ask_match *match, int length,
                                           union ask_candidate **head,
                                           union ask_candidate **tail)
{
    /* @hack redundant: text declared register, where a plain local would do */
    register Uint8 *text = word->text;
    int remaining = strlen((char *)text);
    int copied;
    union ask_candidate *cell;
    union ask_candidate *next;
    if (!(cell = alloc_mem())) {
        return NULL;
    }
    cell->head.flags = 0;
    copied =
        copy_characters(cell->head.text, text, sizeof(cell->head.text) - 1);
    remaining -= copied;
#ifdef DREAMCAST
    /* @hack redundant: an empty test of length, which control never
     * reaches */
    if (0) {
        if (length) {
        }
    }
#endif
    cell->head.length = length;
    cell->head.form = word->form;
    cell->head.match = match;
    cell->head.id = word->id;
    cell->head.priority = word->frequency;
    cell->head.next = NULL;
    *head = cell;
    text += copied;
    while (remaining > 0) {
        if (!(next = alloc_mem())) {
            free_khmemlist(*head);
            return NULL;
        }
        cell->head.flags |= ASK_CANDIDATE_CONTINUES;
        next->head.flags = ASK_CANDIDATE_TAIL;
        copied = copy_characters(next->continuation.text, text,
                                 sizeof(next->continuation.text) - 1);
        remaining -= copied;
        next->head.next = NULL;
        cell->head.next = next;
        text += copied;
        cell = next;
    }
    *tail = cell;
    return *head;
}
/* copy text's whole characters into output, room bytes at most, then a NUL.
 * it returns the bytes copied */
/* @hack redundant: output and text declared register, where plain parameters
 * would do */
static int copy_characters(register Uint8 *output, register Uint8 *text,
                           int room)
{
    int total = room;
    while (*text && room > 0) {
        if (is_kanji(*text)) {
            if (room <= 1) {
                break;
            }
            *output++ = *text++;
            room--;
        }
        *output++ = *text++;
        room--;
    }
    *output = 0;
    return total - room;
}
static union ask_candidate *raw_candidate(int first, int length, int mode)
{
    union ask_candidate *head;
    union ask_candidate *tail;
    ASK_NUMBER_RESULTS->id = ASK_NO_ID;
    ASK_NUMBER_RESULTS->frequency = 0;
    ASK_NUMBER_RESULTS->form = 0;
    trans_roman(ASK_NUMBER_RESULTS->text, first, length, mode);
    return copy_candidate(ASK_WORD_RESULTS, NULL, length, &head, &tail);
}
void khmem_raw(int mode)
{
    struct ask_position *position = &hchar[cur_pos];
    free_khmemlist(position->candidates);
    position->candidates = raw_candidate(cur_pos, cur_len, mode);
}

static void merge_candidates(int first, struct ask_candidate_list *list)
{
    struct ask_position *position = &hchar[first];
    union ask_candidate *head;
    union ask_candidate *tail;
    union ask_candidate *candidate;
    head = position->candidates;
    tail = last_cell(head);
    remember_candidates(first, head);
    while ((candidate = take_best(list)) != NULL) {
        append_unique(first, &head, &tail, candidate);
    }
    if ((candidate = unchanged_candidate(cur_len)) != NULL) {
        append_unique(first, &head, &tail, candidate);
    }
    position->candidates = head;
}
/* take the first candidate of the list whose priority is highest, or NULL
 * where every list is empty */
static union ask_candidate *take_best(struct ask_candidate_list *lists)
{
    struct ask_candidate_list *best = NULL;
    struct ask_candidate_list *list = lists;
    Uint16 rank;
    /* @hack redundant: candidate declared register, where a plain local would
     * do */
    register union ask_candidate *candidate;
    union ask_candidate *next;
    for (; list; list = list->next) {
        if (list->candidates && (!best || list->priority > rank)) {
            best = list;
            rank = list->priority;
        }
    }
    if (!best) {
        return NULL;
    }
    candidate = best->candidates;
    if (!(next = best->candidates = kh_skip(candidate))) {
        best->priority = 0;
    } else {
        /* @hack type: the priority promoted to int by a unary plus, where
         * assigning it would do */
        best->priority = +kh_priority(best->phrase, next->head.priority);
    }
    return candidate;
}
/* record the text of candidate and each after it in wdsbuf, from
 * ASK_WORD_BYTES to candidate_text_end. each text is a length byte, then its
 * bytes */
/* @hack redundant: candidate declared register, where a plain parameter would
 * do */
static void remember_candidates(int first,
                                register union ask_candidate *candidate)
{
    int length;
    candidate_text_end = ASK_WORD_BYTES;
    while (candidate) {
        length = kouho_makedisp(first, cur_len, candidate, meanbuf);
#ifdef NONMATCHING
        /* this side converts into outbuf, and copies in only a text wdsbuf has
         * room for, where the matching side converts into wdsbuf unchecked */
        length = meantosjis(meanbuf, outbuf, length);
        if ((int)sizeof(wdsbuf) - (candidate_text_end - ASK_WORD_BYTES) <=
            length) {
            break;
        }
        memcpy(candidate_text_end + 1, outbuf, length);
#else
        length = meantosjis(meanbuf, candidate_text_end + 1, length);
#endif
        *candidate_text_end++ = length;
        candidate_text_end += length;
        candidate = kh_followed(candidate);
    }
}
/* append candidate to the list from *head to *tail, and record its text
 * where wdsbuf has room. a candidate whose text is recorded already is freed
 * instead */
static void append_unique(int first, union ask_candidate **head,
                          union ask_candidate **tail,
                          union ask_candidate *candidate)
{
    int length;
    length = kouho_makedisp(first, cur_len, candidate, meanbuf);
    length = meantosjis(meanbuf, outbuf, length);
    if (seen_text(outbuf, length)) {
        free_khmemlist(candidate);
        return;
    }
#ifdef NONMATCHING
    /* this side compares the room left in wdsbuf, where the matching side forms
     * a pointer that can lie past it */
    if ((int)sizeof(wdsbuf) - (candidate_text_end - ASK_WORD_BYTES) > length) {
#else
    if (candidate_text_end + length + 1 <= (Uint8 *)(&wdsbuf + 1)) {
#endif
        *candidate_text_end++ = length;
        strncpy((char *)candidate_text_end, (char *)outbuf, length);
        candidate_text_end += length;
    }
    if (!*head) {
        *head = *tail = candidate;
    } else {
        (*tail)->head.next = candidate;
    }
    *tail = last_cell(candidate);
}
static int seen_text(Uint8 *text, int length)
{
    Uint8 *entry;
    int size;
    for (entry = ASK_WORD_BYTES; entry < candidate_text_end; entry += size) {
        size = *entry++;
        if (size == length && !strncmp((char *)entry, (char *)text, size)) {
            return 1;
        }
    }
    return 0;
}

union ask_candidate *kh_skip(union ask_candidate *candidate)
{
    union ask_candidate *next;
    while (candidate->head.flags & ASK_CANDIDATE_CONTINUES) {
        candidate = candidate->head.next;
    }
    next = candidate->head.next;
    candidate->head.next = NULL;
    return next;
}
union ask_candidate *kh_followed(union ask_candidate *candidate)
{
    while (candidate->head.flags & ASK_CANDIDATE_CONTINUES) {
        candidate = candidate->head.next;
    }
    return candidate->head.next;
}
static union ask_candidate *last_cell(union ask_candidate *candidate)
{
    union ask_candidate *next;
    if (!candidate) {
        return NULL;
    }
    for (;;) {
        next = candidate->head.next;
        if (!next) {
            break;
        }
        candidate = next;
    }
    return candidate;
}
int kh_count(union ask_candidate *candidate)
{
    /* @hack redundant: count declared register, where a plain local would do */
    register int count = 0;
    while (candidate) {
        candidate = kh_followed(candidate);
        count++;
    }
    return count;
}
/* the cells candidate takes in a group. those are its text's, and
 * ASK_WIDE_CELLS for each kana of the reading past its own */
static int candidate_length(union ask_candidate *candidate)
{
    int length = cur_len;
    if (candidate->head.flags == ASK_CANDIDATE_RAW) {
        return length * ASK_WIDE_CELLS;
    }
    length = (length - candidate->head.length) * ASK_WIDE_CELLS;
    while (candidate->head.flags & ASK_CANDIDATE_CONTINUES) {
        length += strlen((char *)candidate->continuation.text);
        candidate = candidate->head.next;
    }
    return length + strlen((char *)candidate->continuation.text);
}
union ask_candidate *take_kouho(union ask_candidate *head, int count)
{
    while (count--) {
        head = kh_followed(head);
    }
    return head;
}
static void draw_group(void)
{
    union ask_candidate *candidate;
    /* @hack redundant: index declared register, where a plain local would do */
    register int index;
    int length;
    int used;
    /* @hack type: kouho_rest and kouho_head written through Uint8 pointers
     * to them, where indexing them would do */
    write_count(kh_count(top_kh) - gun_nkh - 1,
                ((Uint8 *)&kouho_rest) + REST_COUNT);
    sstrtom(meanbuf, kouho_rest, ASK_PLAIN_ATTRIBUTE);
    candidate = top_kh;
    for (used = GROUP_HEADER_WIDTH, index = 0; index < gun_num; index++) {
        ((Uint8 *)&kouho_head)[HEAD_NUMBER] = index + '1';
        used += sstrtom(meanbuf + used, kouho_head, ASK_PLAIN_ATTRIBUTE);
        length = kouho_makedisp(cur_pos, cur_len, candidate, meanbuf + used);
        if (index == gun_nkh) {
            change_kind(meanbuf + used, length, ASK_CURRENT_ATTRIBUTE);
        } else {
            change_kind(meanbuf + used, length, ASK_PLAIN_ATTRIBUTE);
        }
        used += length;
        if (!(candidate = kh_followed(candidate))) {
            break;
        }
    }
    while (used < kwin_len) {
        sstrtom(meanbuf + used, blank, ASK_PLAIN_ATTRIBUTE);
        used++;
    }
}
/* write count as three digits, with blanks for its leading zeros. a count
 * below 0 is 0 */
static void write_count(int count, Uint8 *output)
{
    int digit;
    int padding;
    /* @hack spelling: padding set in each branch, where setting it once
     * before the test would do */
    if (count < 0) {
        count = 0;
        padding = ' ';
    } else {
        padding = ' ';
    }
    digit = count / 100;
    if (digit) {
        *output = digit + '0';
    } else {
        *output = padding;
    }
    output++;
    count %= 100;
    digit = count / 10;
    if (digit || output[-1] != ' ') {
        *output = digit + '0';
    } else {
        *output = padding;
    }
    output++;
    count %= 10;
    *output = count + '0';
}
int kouho_makedisp(int first, int length, union ask_candidate *candidate,
                   Uint16 *cells)
{
    int used;
    if (!candidate || (candidate->head.flags & ASK_CANDIDATE_RAW)) {
        return roman_makedisp(first, length, cells, 0);
    }
    used = draw_word(candidate, cells);
    cells += used;
    used += roman_makedisp(first + candidate->head.length,
                           length - candidate->head.length, cells, 0);
    return used;
}
static int draw_word(union ask_candidate *candidate, Uint16 *cells)
{
    union ask_candidate *cell = candidate;
    int used = 0;
    for (;;) {
        used += sstrtom(cells + used, cell->continuation.text,
                        ASK_CONVERTED_ATTRIBUTE);
        if (!(cell->head.flags & ASK_CANDIDATE_CONTINUES)) {
            break;
        }
        cell = cell->head.next;
    }
    return used;
}
/* how many candidates from candidate a group shows. it shows as many as
 * kwin_len has room for, up to ASK_MOST_IN_GROUP, and at least 1. 0 for
 * none */
static int group_count(union ask_candidate *candidate)
{
    int length;
    int count;
    int available;
    union ask_candidate *next;
    if (!candidate) {
        return 0;
    }
    available = kwin_len - GROUP_HEADER_WIDTH;
    for (count = length = 0, next = candidate;
         count < ASK_MOST_IN_GROUP && next;) {
        if ((length += candidate_length(next) + 3) > available) {
            break;
        }
        count++;
        next = kh_followed(next);
    }
    if (!count) {
        return 1;
    }
    return count;
}
int next_gun(int display, int wrap)
{
    union ask_candidate *old = top_kh;
    int count;
    top_kh = take_kouho(top_kh, gun_num);
    if (!(count = group_count(top_kh))) {
        if (!wrap) {
            top_kh = old;
            return 0;
        }
        init_kouho(0, 0);
    } else {
        gun_num = count;
    }
    gun_nkh = 0;
    if (display == 1) {
        draw_group();
    }
    return 1;
}
int back_gun(int display, int wrap)
{
    union ask_candidate *old = top_kh;
    int count = gun_num;
    init_kouho(0, 0);
    if (old == top_kh) {
        if (!wrap) {
            top_kh = old;
            gun_num = count;
            gun_nkh = 0;
            return 0;
        }
        old = NULL;
    }
    for (;;) {
        if (take_kouho(top_kh, gun_num) == old) {
            break;
        }
        next_gun(0, 1);
    }
    if (display == 1) {
        draw_group();
    }
    return 1;
}
int next_kouho(int display, int wrap)
{
    if (gun_nkh + 1 >= gun_num) {
        return next_gun(display, wrap);
    } else {
        gun_nkh++;
        if (display == 1) {
            draw_group();
        }
        return 1;
    }
}
int back_kouho(int display, int wrap)
{
    if (!gun_nkh) {
        if (!back_gun(0, wrap)) {
            return 0;
        }
        gun_nkh = gun_num - 1;
    } else {
        gun_nkh--;
    }
    if (display == 1) {
        draw_group();
    }
    return 1;
}
