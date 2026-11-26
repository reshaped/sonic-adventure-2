#include "ask_cells.h"
#include "ask_phrases.h"
#include "ask_dictionary.h"
#include "ask_grammar.h"
#include "ask_conversion.h"
#include "ask_editor.h"
#include "ask_input.h"
#include "ask_jis.h"
#include "ask_roman.h"
#include <sg_xpt.h>

/* what word_endings and word_phrases return where they can't know every
 * ending that follows a word. the reading may go on, or a position on the
 * way is unsearched */
#define INCOMPLETE_ENDINGS ((struct ask_ending *)-1)
#define INCOMPLETE_PHRASES ((struct ask_phrase *)-1)

struct ask_position hchar[ASK_POSITIONS];
int kd_mode;
int henkan_mode;
int ikkatsu_mode;
static struct ask_reading_match reading_match;
/* space the image holds for ASK_POSITIONS positions more and a word, which
 * nothing reads or writes */
#ifdef DREAMCAST
#ifdef NONMATCHING
#else
static struct ask_position spare_positions[ASK_POSITIONS];
static int spare_word;
#endif
#endif

#pragma section ATA
/* the word of no reading, whose phrases are endings alone */
static struct ask_match empty_word = {0, ASK_PLAIN_WORD, 0, -1, 0, NULL};
#pragma section

static void clear_position(struct ask_position *position);
static void free_position(struct ask_position *position, Bool keep);
static int joined_length(int first, int end);
static int raw_length(int first, int end);
static void find_endings(int first, int end);
static int find_words(int first, int end);
static struct ask_match *make_words(int first,
                                    struct ask_reading_match *result);
static void append_words(int first, struct ask_match *word);
static void add_raw_word(int first, int length, Uint8 kind);
static int find_phrases(int first, int end);
static struct ask_phrase *word_phrases(int first, int end,
                                       struct ask_match *word);
static void append_phrases(int first, struct ask_phrase *list);
static void keep_length(int first, int length);
static struct ask_fixed_word *find_fixed_word(char *text, int length,
                                              int *matched);
static struct ask_ending *word_endings(int first, int end, Uint8 kind,
                                       Uint8 form);
static int cannot_start(char code);
static int is_stop(char code);

void init_hchar(void)
{
    struct ask_position *position;
    for (position = hchar; position < &hchar[ASK_POSITIONS]; position++) {
        clear_position(position);
    }
}
static void clear_position(struct ask_position *position)
{
    position->ending = ASK_ENDING_UNSEARCHED;
    position->words = NULL;
    position->phrases = NULL;
    position->candidates = NULL;
    position->candidate_index = 0;
    position->fixed = 0;
    position->length = 0;
    position->first_length = 0;
    position->word_length = -1;
    position->ending_length = -1;
    position->selection = -1;
}
void free_hchar(int first, int end, int keep)
{
    /* @hack redundant: position declared register, where a plain local would
     * do */
    register struct ask_position *position;
    struct ask_position *limit;
    limit = &hchar[end];
    for (position = &hchar[first]; position < limit; position++) {
        free_position(position, keep);
    }
}
static void free_position(struct ask_position *position, Bool keep)
{
    if (!keep) {
        position->ending = ASK_ENDING_UNSEARCHED;
        position->ending_length = -1;
        if (position->words != ASK_NO_WORDS) {
            free_chmemlist(position->words);
        }
        position->words = NULL;
        position->word_length = -1;
        position->selection = -1;
        position->first_length = 0;
    }
    if (position->phrases != NULL && position->phrases != ASK_NO_PHRASES) {
        free_bsmemlist(position->phrases);
    }
    position->phrases = NULL;
    if (position->candidates != NULL) {
        free_khmemlist(position->candidates);
    }
    position->candidates = NULL;
    position->candidate_index = 0;
    position->fixed = 0;
    position->length = 0;
}

void henkan(int first, int end, int mode, int length)
{
    struct ask_position *position;
    struct ask_phrase *phrase;
    struct ask_position *follower;
    /* @hack redundant: index declared register, where a plain local would do */
    register int index;
    int next;
    int selected;
    int total;
    henkan_mode = mode;
    find_endings(first, end);
    if (henkan_mode == ASK_PARTIAL_READING && ikkatsu_mode) {
        return;
    }
    for (index = first; index < end;) {
        position = &hchar[index];
        if (position->length > 0) {
            index += position->length;
            continue;
        }
        if (position->words == NULL && find_words(index, end) == 0) {
            break;
        }
        if (position->phrases == NULL && find_phrases(index, end) == 0) {
            break;
        }
        for (phrase = position->phrases; phrase != NULL;
             phrase = phrase->next) {
            if ((next = phrase->length + index) >= end) {
                if (henkan_mode >= ASK_PARTIAL_READING) {
                    if (phrase->kind == ASK_STOP_WORD) {
                        henkan_mode = ASK_FREE_LENGTHS;
                    } else {
                        break;
                    }
                }
            } else {
                follower = &hchar[next];
                if (follower->words == NULL && find_words(next, end) == 0) {
                    break;
                }
                if (follower->phrases == NULL && find_phrases(next, end) == 0) {
                    break;
                }
            }
        }
        if (index == first) {
            if (henkan_mode == ASK_SCORED_LENGTH) {
                bs_prefer(index, end, length);
                hchar[first].fixed = 1;
                selected = length;
            } else if (henkan_mode == ASK_PLAIN_LENGTH) {
                hchar[first].fixed = 1;
                selected = length;
            } else {
                goto prefer;
            }
            goto chosen;
        }
        if (index == first + length && henkan_mode == ASK_PLAIN_LENGTH) {
            bs_prefix(index);
        }
    prefer:
        if ((selected = bs_prefer(index, end, -1)) == -1) {
            break;
        }
    chosen:
        keep_length(index, selected);
        first_kouho(index, selected);
        hchar[index].length = selected;
        index += selected;
    }
    for (index = first; index < end; index += selected) {
        position = &hchar[index];
        total = position->length;
        if (position->fixed) {
            selected = total;
            continue;
        }
        if ((selected = joined_length(index, end)) < 0) {
            return;
        }
        if (selected == 0) {
            selected = total;
        }
        if (selected == total) {
            position->fixed = 1;
            continue;
        }
        while (total < selected) {
            if ((phrase = position->phrases) != ASK_NO_PHRASES) {
                free_bsmemlist(phrase);
            }
            position->phrases = NULL;
            position->length = 0;
            free_khmemlist(position->candidates);
            position->candidates = NULL;
            position = &hchar[index + total];
            total += position->length;
        }
        if ((phrase = position->phrases) != ASK_NO_PHRASES) {
            free_bsmemlist(phrase);
        }
        position->phrases = NULL;
        position->length = 0;
        free_khmemlist(position->candidates);
        position->candidates = NULL;
        position = &hchar[index];
        position->length = selected;
        if ((phrase = alloc_mem()) != NULL) {
            phrase->length = selected;
            phrase->kind = ASK_RAW_WORD;
            phrase->form = 0;
            phrase->word = NULL;
            phrase->score = 0;
            phrase->preference = 0;
            phrase->next = NULL;
            position->phrases = phrase;
            position->fixed = 1;
            first_kouho(index, selected);
        }
    }
}

/* the length of the run of ASK_RAW_WORD phrases from first. where the run
 * ends before end, that length includes the phrase after the run. it returns
 * 0 where first's phrase is no ASK_RAW_WORD one. it returns -1 where a
 * position on the way has no phrase chosen */
static int joined_length(int first, int end)
{
    /* @hack redundant: position declared register, where a plain local would
     * do */
    register struct ask_position *position = &hchar[first];
    int length;
    int total = 0;
    struct ask_phrase *phrase;
    while (first < end) {
        if ((length = position->length) == 0) {
            return -1;
        }
        phrase = position->phrases;
        if (phrase != NULL && phrase != ASK_NO_PHRASES &&
            phrase->kind != ASK_RAW_WORD) {
            break;
        }
        first += length;
        total += length;
        position += length;
    }
    if (total == 0 || first >= end) {
        return total;
    }
    return length + total;
}
/* the length of an ASK_RAW_WORD phrase at first. it runs from first up to the
 * next byte that can begin a phrase, or to end */
static int raw_length(int first, int end)
{
    int index;
    for (index = first + 1; index < end; index++) {
        if (!cannot_start(kana_ustr[index])) {
            break;
        }
    }
    return index - first;
}
static void find_endings(int first, int end)
{
    struct ask_position *position = &hchar[first];
    char *text = (char *)&kana_ustr[first];
    int remaining = end - first;
    int matched;
    struct ask_fixed_word *ending;
    for (; remaining > 0; remaining--, text++, position++) {
        if (position->ending == ASK_ENDING_UNSEARCHED) {
            if ((ending = find_fixed_word(text, remaining, &matched)) ==
                ASK_ENDING_UNSEARCHED) {
                break;
            }
            position->ending_length = matched;
            position->ending = ending;
        }
    }
}
/* find the words that begin at first. a run of digits, letters, katakana or
 * text with no reading makes a word of its own. it returns 1, or -1 where
 * none begins there. it returns 0 where first is end, or where more reading
 * could change them */
static int find_words(int first, int end)
{
    struct ask_position *position;
    char *text;
    char *next;
    unsigned int count;
    unsigned int shortest;
    int length;
    Uint8 kind;
    int index;
    if ((count = end - first) <= 0) {
        return 0;
    }
    text = (char *)&kana_ustr[first];
    position = &hchar[first];
    kind = 0;
    if (is_num(*text)) {
        for (shortest = 1, kind = ASK_NUMBER_WORD; shortest < count;
             shortest++) {
            if (!is_num(text[shortest])) {
                break;
            }
        }
        if (henkan_mode >= ASK_PARTIAL_READING && shortest >= count) {
            return 0;
        }
    } else if (is_alpha(*text)) {
        for (shortest = 1, kind = ASK_PLAIN_WORD; shortest < count;
             shortest++) {
            if (!is_alphanum(text[shortest])) {
                break;
            }
        }
        if (henkan_mode >= ASK_PARTIAL_READING && shortest >= count) {
            return 0;
        }
    } else if (is_paren(*text)) {
        if (henkan_mode >= ASK_PARTIAL_READING && count <= 1) {
            return 0;
        }
        shortest = count = 1;
        kind = ASK_PLAIN_WORD;
        goto exact;
    } else if (*text && *(Uint8 *)text < ASK_KANA_BASE) {
        if (henkan_mode >= ASK_PARTIAL_READING && count <= 1) {
            return 0;
        }
        shortest = count = 1;
        kind = ASK_SYMBOL_WORD;
        goto exact;
    } else if (*text == 0 && !(kana_buf[first].code & ASK_FIRST_BYTE)) {
        for (index = first + 1; index < end; index++) {
            if (kana_buf[index].code & ASK_FIRST_BYTE) {
                break;
            }
        }
        kind = ASK_PLAIN_WORD;
        goto raw;
    } else if (*text == 0 && is_kata(kana_buf[first].code, 0)) {
        for (index = first + 1; index < end; index++) {
            if (!is_kata(kana_buf[index].code, 1)) {
                break;
            }
        }
        kind = ASK_PLAIN_WORD;
    raw:
        if (henkan_mode >= ASK_PARTIAL_READING && index == end) {
            return 0;
        }
        add_raw_word(first, index - first, kind);
        return 1;
    } else if (*text == 0) {
        for (index = first + 1, text++; index < end; index++, text++) {
            if (*text) {
                break;
            }
        }
        if (is_jisknj(kana_buf[index - 1].code)) {
            kind = ASK_RAW_WORD;
        } else if (is_jiskig(kana_buf[index - 1].code)) {
            kind = ASK_SYMBOL_WORD;
        } else {
            kind = ASK_PLAIN_WORD;
        }
        goto raw;
    } else {
        shortest = 1;
    }
    for (index = 0, next = text; (unsigned int)index < count; index++) {
        if (*next++ == 0) {
            break;
        }
    }
    length = index;
    switch (dic_snssyn(text, length, &reading_match)) {
    case ASK_DIC_WHOLE:
        if (henkan_mode < ASK_PARTIAL_READING) {
            count = length;
            break;
        }
        return 0;
    case ASK_DIC_FOUND:
        if (kind && reading_match.length <= (int)shortest) {
            /* @hack type: the entry read through a Uint32 pointer, where
             * passing reading_match.entry would do */
            dic_freeentid(*(const Uint32 *)&reading_match.entry);
            position->word_length = reading_match.prefix_length;
            count = shortest;
        } else {
            append_words(first, make_words(first, &reading_match));
            position->word_length = reading_match.prefix_length;
            count = reading_match.length - 1;
        }
        break;
    case ASK_DIC_NONE:
    case ASK_DIC_INVALID:
    case ASK_DIC_CLOSED:
    case ASK_DIC_BAD_FILE:
    default:
        if (kind) {
            count = shortest;
            break;
        }
        position->words = ASK_NO_WORDS;
        return -1;
    }
exact:
    for (; count >= shortest; count--) {
        if (kind && count == shortest) {
            add_raw_word(first, shortest, kind);
        }
        if (dic_getsyn(text, count, &reading_match) == ASK_DIC_FOUND) {
            if (position->word_length == -1) {
                position->word_length = reading_match.prefix_length;
            }
            append_words(first, make_words(first, &reading_match));
        }
    }
    return 1;
}

/* @hack redundant: first, a parameter nothing reads */
static struct ask_match *make_words(int first, struct ask_reading_match *result)
{
    struct ask_match *head = NULL;
    struct ask_match *tail = NULL;
    /* @hack redundant: word and meaning declared register, where plain locals
     * would do */
    register struct ask_match *word;
    register struct ask_synonym *meaning;
    Sint16 length;
    Sint32 entry;
    int count;
    (void)first;
    length = result->length;
    entry = result->entry;
    count = result->count;
    meaning = result->meanings;
    while (count--) {
        /* @hack spelling: id, a copy of entry for each word, where entry
         * itself would do */
        Sint32 id = entry;
        if ((word = alloc_mem()) == NULL) {
            break;
        }
        word->length = length;
        if (head == NULL) {
            head = word;
        }
        word->kind = meaning->kind;
        word->form = meaning->form;
        word->entry = id;
        word->frequency = meaning->frequency;
        word->next = NULL;
        if (tail != NULL) {
            tail->next = word;
        }
        tail = word;
        meaning++;
    }
    return head;
}
static void append_words(int first, struct ask_match *word)
{
    struct ask_match **link;
    for (link = &hchar[first].words; *link != NULL; link = &(*link)->next) {
    }
    *link = word;
}
static void add_raw_word(int first, int length, Uint8 kind)
{
    struct ask_match *word;
    if ((word = alloc_mem()) == NULL) {
        return;
    }
    word->length = length;
    word->kind = kind;
    word->form = 0;
    word->entry = -1;
    word->frequency = 0;
    word->next = NULL;
    hchar[first].word_length = length + 1;
    append_words(first, word);
}
/* find the phrases that begin at first: each word's with the endings after
 * it, and endings alone. an ASK_RAW_WORD phrase stands where none is found. it
 * returns 1, 0 where more reading could change them, and -1 where no cell is
 * free */
static int find_phrases(int first, int end)
{
    struct ask_position *position = &hchar[first];
    struct ask_match *word;
    struct ask_phrase *list;
    if (position->words != ASK_NO_WORDS) {
        for (word = position->words; word != NULL; word = word->next) {
            if ((list = word_phrases(first, end, word)) == INCOMPLETE_PHRASES) {
                goto incomplete;
            } else if (list != NULL) {
                append_phrases(first, list);
            }
        }
    }
    if ((list = word_phrases(first, end, &empty_word)) == INCOMPLETE_PHRASES) {
    incomplete:
        if (position->phrases != NULL) {
            free_bsmemlist(position->phrases);
            position->phrases = NULL;
        }
        return 0;
#ifdef DREAMCAST
        /* @hack redundant: an assignment to list after the return, which
         * control never reaches */
        list = NULL;
#endif
    }
    if (list != NULL) {
        append_phrases(first, list);
    }
    if (position->phrases == NULL) {
        if ((list = alloc_mem()) == NULL) {
            return -1;
        }
        list->length = raw_length(first, end);
        list->kind = ASK_RAW_WORD;
        list->form = 0;
        list->word = NULL;
        list->score = 0;
        list->preference = 0;
        list->next = NULL;
        position->phrases = list;
    }
    return 1;
}

/* word's phrases at first. each is word and a chain of endings after it.
 * word alone is one too, where a phrase can end on word and begin after it.
 * it returns INCOMPLETE_PHRASES where word_endings can't know every
 * chain */
static struct ask_phrase *word_phrases(int first, int end,
                                       struct ask_match *word)
{
    struct ask_ending *endings;
    struct ask_ending *ending;
    struct ask_phrase *head = NULL;
    struct ask_phrase *tail = NULL;
    struct ask_phrase *phrase;
    int length;
    int next;
    length = word->length;
    next = first + length;
    if ((ending = word_endings(next, end, word->kind, word->form)) ==
        INCOMPLETE_ENDINGS) {
        return INCOMPLETE_PHRASES;
    }
    for (endings = ending; ending != NULL; ending = ending->next) {
        if ((phrase = alloc_mem()) == NULL) {
            break;
        }
        if (head == NULL) {
            head = phrase;
        }
        phrase->length = ending->length + length;
        phrase->kind = ending->word_kind;
        phrase->form = ending->word_form;
        phrase->word = word;
        phrase->score = 0;
        phrase->preference = 0;
        phrase->next = NULL;
        if (tail != NULL) {
            tail->next = phrase;
        }
        tail = phrase;
    }
    free_pwmemlist(endings);
    if (length > 0 && setu_end(word->kind, word->form) &&
        (next >= end || !cannot_start(kana_ustr[next]))) {
        if ((phrase = alloc_mem()) != NULL) {
            phrase->length = length;
            if (head == NULL) {
                head = phrase;
            }
            phrase->kind = word->kind;
            phrase->form = word->form;
            phrase->word = word;
            phrase->score = 0;
            phrase->preference = 0;
            phrase->next = NULL;
            if (tail != NULL) {
                tail->next = phrase;
            }
        }
    }
    return head;
}

/* put phrase into list after each phrase of its length or longer. it returns
 * the list, which runs longest first */
static struct ask_phrase *insert_phrase(struct ask_phrase *list,
                                        struct ask_phrase *phrase)
{
    struct ask_phrase *previous;
    struct ask_phrase *next;
    int length = phrase->length;
    if (list == NULL || list->length < length) {
        phrase->next = list;
        return phrase;
    }
    for (previous = list, next = list->next; next != NULL;
         previous = next, next = next->next) {
        if (next->length < length) {
            break;
        }
    }
    previous->next = phrase;
    phrase->next = next;
    return list;
}
static void append_phrases(int first, struct ask_phrase *list)
{
    struct ask_phrase *head = hchar[first].phrases;
    struct ask_phrase *phrase;
    struct ask_phrase *next;
    for (phrase = list; phrase != NULL; phrase = next) {
        next = phrase->next;
        head = insert_phrase(head, phrase);
    }
    hchar[first].phrases = head;
}
static void keep_length(int first, int length)
{
    struct ask_phrase **link = &hchar[first].phrases;
    struct ask_phrase *phrase;
    /* @hack spelling: a for (;;) that a goto enters at its test, where a
     * while loop would do */
    goto next;
    for (;;) {
        if (phrase->length == length) {
            link = &phrase->next;
        } else {
            *link = phrase->next;
            free_mem(phrase);
        }
    next:
        if ((phrase = *link) == NULL) {
            break;
        }
    }
}
int bunsetu_len(int first)
{
    struct ask_position *position;
    if (first >= kana_len) {
        return 0;
    }
    position = &hchar[first];
    if (im_state == 2 && !position->fixed) {
        return 0;
    }
    return position->length;
}
void save_fst_bslen(int first)
{
    struct ask_position *position;
    position = &hchar[first];
    if (position->first_length == 0 && position->fixed) {
        position->first_length = position->length;
    }
}

/* the ending whose reading text begins with, or NULL. the search runs back
 * from the last ending that begins with text's first kana. it returns
 * ASK_ENDING_UNSEARCHED where text ends inside a reading under
 * ASK_PARTIAL_READING. *matched gets one past the most bytes text shares
 * with a reading the search compared. it gets the found reading's length
 * where that is more */
static struct ask_fixed_word *find_fixed_word(char *text, int length,
                                              int *matched)
{
    struct ask_fixed_word *word;
    struct ask_fixed_word *first;
    /* @hack redundant: reading and next declared register, where plain locals
     * would do */
    register Uint8 *reading;
    register Uint8 *next;
    Uint8 code;
    int best;
    int count;
    int common;
    int remaining;
    best = 1;
    if ((code = *text) < ASK_KANA_BASE) {
        *matched = 1;
        return NULL;
    }
    code -= ASK_KANA_BASE;
    text++;
#ifdef NONMATCHING
    /* this side refuses a code past pwordmap's buckets, and walks down to first
     * without forming the pointer before it, which C leaves undefined where
     * first starts pword */
    if (code >= ASK_ENDING_BUCKETS) {
        *matched = 1;
        return NULL;
    }
    first = &pword[pwordmap[code]];
    word = &pword[pwordmap[code + 1]];
    for (length--; word > first;) {
        word--;
#else
    word = &pword[pwordmap[code + 1]] - 1;
    first = &pword[pwordmap[code]];
    for (length--; word >= first; word--) {
#endif
        reading = &pluswd[word->reading];
        count = *reading++ - 1;
        next = (Uint8 *)text;
        common = 1;
        remaining = length;
        for (;;) {
            if (count == 0) {
                if (common > best) {
                    best = common;
                }
                *matched = best;
                return word;
            }
            if (remaining == 0) {
                if (henkan_mode < ASK_PARTIAL_READING) {
                    break;
                }
                return ASK_ENDING_UNSEARCHED;
            }
            common++;
            if (*reading != *next) {
                if (common > best) {
                    best = common;
                }
                break;
            }
            next++;
            reading++;
            remaining--;
            count--;
        }
    }
    *matched = best;
    return NULL;
}

/* the chains of endings at first that can follow a word of kind in form.
 * kind 0 stands for any word. a stop mark there is a chain of its own,
 * where a phrase can end on the word. it returns INCOMPLETE_ENDINGS where the
 * reading may go on, or a position on the way is unsearched */
static struct ask_ending *word_endings(int first, int end, Uint8 kind,
                                       Uint8 form)
{
    /* @hack redundant: word declared register, where a plain local would do */
    register struct ask_fixed_word *word;
    struct ask_ending *head = NULL;
    struct ask_ending *list;
    struct ask_ending *ending;
    int length;
    int next;
    if (first >= end) {
        if (henkan_mode >= ASK_PARTIAL_READING) {
            return INCOMPLETE_ENDINGS;
        }
        return NULL;
    }
    if ((word = hchar[first].ending) == ASK_ENDING_UNSEARCHED) {
        return INCOMPLETE_ENDINGS;
    }
    if (word != NULL) {
        goto check;
    }
    if (is_stop(kana_ustr[first])) {
        if (kind == 0 || setu_end(kind, form)) {
            ending = alloc_mem();
#ifdef NONMATCHING
            /* this side returns NULL where no cell is free */
            if (ending == NULL) {
                return NULL;
            }
#endif
            ending->length = 1;
            ending->word_kind = ASK_STOP_WORD;
            ending->kind = ASK_STOP_WORD;
            ending->word_form = 0;
            ending->form = 0;
            ending->next = NULL;
            return ending;
        }
    }
    while (word != NULL) {
    check:
        if (setu_end(word->kind, word->form) &&
            (kind == 0 || goku_connect(kind, form, word->kind))) {
            length = pluswd[word->reading];
            next = first + length;
            if (next >= end || !cannot_start(kana_ustr[next])) {
                ending = alloc_mem();
#ifdef NONMATCHING
                /* this side frees the endings so far and returns NULL where no
                 * cell is free */
                if (ending == NULL) {
                    free_pwmemlist(head);
                    return NULL;
                }
#endif
                ending->length = length;
                ending->kind = ending->word_kind = word->kind;
                ending->form = ending->word_form = word->form;
                ending->next = head;
                head = ending;
            }
        }
        if (!word->back) {
            break;
        }
        word -= word->back;
    }
    for (word = hchar[first].ending; word != NULL; word -= word->back) {
        if (kind == 0 || goku_connect(kind, form, word->kind)) {
            length = pluswd[word->reading];
            if ((list = word_endings(first + length, end, word->kind,
                                     word->form)) == INCOMPLETE_ENDINGS) {
                free_pwmemlist(head);
                return INCOMPLETE_ENDINGS;
            }
            if (list != NULL) {
                for (ending = list;; ending = ending->next) {
                    ending->length += length;
                    ending->kind = word->kind;
                    ending->form = word->form;
                    if (!ending->next) {
                        break;
                    }
                }
                ending->next = head;
                head = list;
            }
        }
        if (!word->back) {
            break;
        }
    }
    return head;
}

/* whether a phrase can't begin with the reading byte code. those are the
 * long vowel mark, a small kana, WO and N. any can begin one under
 * ASK_SCORED_LENGTH and ASK_PLAIN_LENGTH */
static int cannot_start(char code)
{
    if (henkan_mode == ASK_SCORED_LENGTH || henkan_mode == ASK_PLAIN_LENGTH) {
        return 0;
    }
    switch ((Uint8)code) {
    case 0x9d:
    case 0xa1:
    case 0xa3:
    case 0xa5:
    case 0xa7:
    case 0xa9:
    case 0xc3:
    case 0xe3:
    case 0xe5:
    case 0xe7:
    case 0xee:
    case 0xf2:
    case 0xf3:
        return 1;
    }
    return 0;
}
/* whether the reading byte code ends a phrase: an ASCII space, !, comma,
 * full stop, :, ; or ?, or an ideographic full stop, comma or middle dot */
static int is_stop(char code)
{
    if ((Uint8)code >= ASK_KANA_BASE) {
        return 0;
    }
    switch ((Uint8)code) {
    case ' ':
    case '!':
    case ',':
    case '.':
    case ':':
    case ';':
    case '?':
    case 0x98:
    case 0x9b:
    case 0x9c:
        return 1;
    }
    return 0;
}
