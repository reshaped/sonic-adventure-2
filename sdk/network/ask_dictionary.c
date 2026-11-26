#include "ask_dictionary.h"
#include "ask_pages.h"
#include "ask_temp.h"
#include "ask_candidates.h"
#include <sg_xpt.h>
#include "ask_string.h"

/* the bytes of a key, a reading as a search takes it */
#define KEY_BYTES 80
#define RECORD_SIZE(p) ((Uint16)((p)[0] + ((p)[1] * 256)))
/* learning gives a word the next time as its frequency, from 1. at
 * TIME_LIMIT it clears the frequency of every word in the entries at ids.
 * the word then takes time 1 */
#define TIME_LIMIT 255
/* the bytes of a kanji in Shift-JIS */
#define KANJI_BYTES 2

/* a form with units writes at most MOST_DIGITS digits. it writes a large
 * unit every GROUP_PLACES places */
#define MOST_DIGITS 13
#define GROUP_PLACES 4
/* numeral_units' kanji, by index. the formal one, two and three follow
 * FORMAL_TEN. ten thousand, a hundred million and a trillion follow
 * THOUSAND */
enum numeral_unit { FORMAL_TEN = 0, TEN = 4, HUNDRED = 5, THOUSAND = 6 };
/* numeral_units' index in numerals */
#define UNIT_TABLE 2
int dic_fd;
int dic_rw;
int gaku_mode;
int suji_mode;
#pragma section ATA
/* the kanji numeral_unit indexes, KANJI_BYTES each */
/* clang-format off */
static Uint8 numeral_units[20] = {
    0x8f, 0x45, 0x88, 0xeb, 0x93, 0xf3, 0x8e, 0x51, 0x8f, 0x5c,
    0x95, 0x53, 0x90, 0xe7, 0x96, 0x9c, 0x89, 0xad, 0x92, 0x9b,
};
/* clang-format on */
static Uint8 full_digits[24];
static Uint8 kanji_digits[21];
/* the digits of ASK_FULL_WIDTH_DIGITS and ASK_KANJI_DIGITS, at those forms'
 * indexes, then numeral_units at UNIT_TABLE */
static Uint8 *numerals[3] = {full_digits, kanji_digits, numeral_units};
/* clang-format off */
static Uint8 full_digits[24] = {
    0x82, 0x4f, 0x82, 0x50, 0x82, 0x51, 0x82, 0x52, 0x82, 0x53,
    0x82, 0x54, 0x82, 0x55, 0x82, 0x56, 0x82, 0x57, 0x82, 0x58,
};
static Uint8 kanji_digits[21] = {
    0x81, 0x5a, 0x88, 0xea, 0x93, 0xf1, 0x8e, 0x4f, 0x8e, 0x6c,
    0x8c, 0xdc, 0x98, 0x5a, 0x8e, 0xb5, 0x94, 0xaa, 0x8b, 0xe3,
};
/* clang-format on */
#pragma section

static int longest_reading(Uint8 *key, Sint16 length, struct ask_lookup *match);
static int append_meanings(Uint8 *record, struct ask_synonym *meaning,
                           int count, int *frequency);
static int seen_meaning(struct ask_synonym *first,
                        struct ask_synonym *candidate);
static Uint8 *next_word(Uint8 *at, const Uint8 *end);
static int exact_reading(Uint8 *key, Sint16 length, struct ask_lookup *match);
static void best_word(Uint8 *record, int kind, int form, int *frequency,
                      struct ask_dictionary_word *output, int flags);
static void copy_word(struct ask_dictionary_word *output, Uint8 *word,
                      Uint8 *end, int id);
static void append_words(Uint8 *record, int kind, int form, int *count,
                         int *used, struct ask_dictionary_word *output,
                         int flags);
static Uint8 *insert_word(Uint8 *buffer, int frequency, int size, int used);
static void clear_times(Sint32 *ids, int count);
static Uint8 *end_page(Uint8 *page);
static void shift_page(Uint8 *first, Uint8 *end, int shift);
static int delete_word(Uint8 *cursor, struct ask_learn_word *word);
static int is_number(const Uint8 *reading);
static Uint8 *write_number(const Uint8 *reading, Sint16 length,
                           struct ask_dictionary_word *output, int mode);

int dic_open(char *name)
{
    int result;
    if (dic_fd != ASK_NO_DICTIONARY) {
        return ASK_DIC_ALREADY_OPEN;
    }
    if (*name == 0) {
        return ASK_DIC_INVALID;
    }
    set_dicname(name);
    if ((result = open_dic()) != 0) {
        if (result == -1) {
            return ASK_DIC_CANT_OPEN;
        } else {
            return ASK_DIC_OPEN_ERROR;
        }
    }
    if (read_head() == -1) {
        close_dic();
        return ASK_DIC_BAD_FILE;
    }
    if (read_index() == -1) {
        close_dic();
        return ASK_DIC_BAD_FILE;
    }
    init_page();
    init_temp();
    if (dic_rw == ASK_DIC_READ_ONLY_MODE) {
        return ASK_DIC_READ_ONLY;
    }
    return ASK_DIC_DONE;
}
int dic_close(void)
{
    if (dic_fd == ASK_NO_DICTIONARY) {
        return ASK_DIC_CLOSED;
    }
    flush_head();
    flush_temp();
    flush_pages();
    if (close_dic() == -1) {
        return ASK_DIC_BAD_FILE;
    }
    return ASK_DIC_DONE;
}

int dic_snssyn(const char *reading, Sint16 length,
               struct ask_reading_match *match)
{
    char key[KEY_BYTES];
    struct ask_lookup main;
    struct ask_lookup temporary;
    int frequency;
    int count;
    int main_result;
    int temporary_result;
    char *copy;
    Sint16 main_length;
    if (dic_fd == ASK_NO_DICTIONARY) {
        return ASK_DIC_CLOSED;
    }
#ifdef NONMATCHING
    /* this side refuses a length key can't hold with its NUL, and starts both
     * lookups empty, for a search that returns before it fills one */
    if (length < 0 || length >= KEY_BYTES) {
        return ASK_DIC_NONE;
    }
    main.row = 0;
    main.record = temporary.record = NULL;
#endif
    /* @hack spelling: copy, a second pointer to key, where key itself would
     * do */
    strncpy(copy = key, reading, length);
    key[length] = 0;
    if ((main_result = longest_reading((Uint8 *)key, length, &main)) ==
        ASK_DIC_WHOLE) {
        return ASK_DIC_WHOLE;
    }
    /* longest_reading shortens key as it searches */
    strncpy(copy, reading, length);
    key[length] = 0;
    if ((temporary_result = tmp_snssyn(key, length, &temporary)) ==
        ASK_DIC_WHOLE) {
        return ASK_DIC_WHOLE;
    }
    if (main_result == ASK_DIC_NONE && temporary_result == ASK_DIC_NONE) {
        return ASK_DIC_NONE;
    }
#ifdef DREAMCAST
    /* @hack redundant: an unreachable return of dic_fd through count */
    if (0) {
        count = dic_fd;
        return count;
    }
#endif
    match->found = 1;
    frequency = 0;
    match->prefix_length = main.prefix_length > temporary.prefix_length
                               ? main.prefix_length
                               : temporary.prefix_length;
    if ((main_length = main.length) > temporary.length) {
        match->length = main_length;
        match->count =
            append_meanings(main.record, match->meanings, 0, &frequency);
        temporary.offset = -1;
#ifdef DREAMCAST
        /* @hack redundant: an unreachable return of main_length */
        if (0) {
            return main_length;
        }
#endif
    } else if (main_length < temporary.length) {
        match->length = temporary.length;
        match->count =
            append_meanings(temporary.record, match->meanings, 0, &frequency);
        main.offset = -1;
    } else {
        match->length = main_length;
        count = append_meanings(main.record, match->meanings, 0, &frequency);
        match->count = append_meanings(temporary.record, match->meanings, count,
                                       &frequency);
    }
    if ((match->entry = set_entid_tab(main.row, main.offset, temporary.offset,
                                      frequency)) == -1) {
        return ASK_DIC_NONE;
    }
    return ASK_DIC_FOUND;
}

static int longest_reading(Uint8 *key, Sint16 length, struct ask_lookup *match)
{
    int row;
    Uint8 *best;
    Sint16 prefix;
    Sint16 best_length;
    Sint16 prefix_length;
    int search_length;
    Bool first;
    Uint8 *page;
    Uint8 *record;
    Uint8 *reading;
    int reading_length;
    Sint16 size;
    int common;
    int order;
    first = TRUE;
    match->length = match->prefix_length = 0;
    row = srch_page(key);
    if (page_fix(row, key) == 0) {
        return ASK_DIC_WHOLE;
    }
    prefix_length = 0;
    for (;;) {
#ifdef NONMATCHING
        /* this side stops where the row lies outside the main dictionary's
         * mainupper rows, which load_page would read past */
        if (row < 0 || row >= mainupper) {
            match->offset = -1;
            return ASK_DIC_NONE;
        }
#endif
        record = page = load_page(row);
        prefix = calc_pulen(row, key);
        best_length = 0;
        while (RECORD_SIZE(record)) {
            size = record[ASK_READING_LENGTH_AT];
            reading_length = size;
            reading = record + ASK_READING_AT;
            if ((common = (Sint16)ask_prefix(reading, key, reading_length)) >=
                prefix_length) {
                prefix_length = common + 1;
            }
            /* @hack spelling: search_length, a copy of length, where length
             * itself would do */
            search_length = length;
            if ((order = strncmp((char *)reading, (char *)key,
                                 reading_length)) == 0) {
                if (reading_length == search_length && first) {
                    return ASK_DIC_WHOLE;
                }
                best_length = size;
                best = record;
            } else if (order > 0) {
                if (strncmp((char *)reading, (char *)key, search_length) == 0 &&
                    first) {
                    return ASK_DIC_WHOLE;
                }
                break;
            } else {
                /* @hack redundant: an empty else */
            }
            record += RECORD_SIZE(record);
        }
        if (best_length > 0) {
            break;
        }
        if (prefix <= 1) {
            match->offset = -1;
            return ASK_DIC_NONE;
        }
        length = prefix - 1;
        key[length] = 0;
        row = srch_page(key);
        first = FALSE;
    }
    match->row = row;
    match->offset = best - page;
    match->length = best_length;
    match->prefix_length = prefix_length;
    match->record = best;
    return ASK_DIC_FOUND;
}

/* @hack redundant: record declared register, where a plain parameter would
 * do */
static int append_meanings(register Uint8 *record, struct ask_synonym *meaning,
                           int count, int *frequency)
{
    Uint8 *end;
    /* @hack redundant: at and first declared register, where plain locals
     * would do */
    register Uint8 *at;
    register struct ask_synonym *first;
#ifdef NONMATCHING
    /* this side takes no meaning from a missing record, and none past
     * ASK_MOST_MEANINGS */
    if (record == NULL || count >= ASK_MOST_MEANINGS) {
        return count;
    }
#endif
    end = record + RECORD_SIZE(record);
    at = record + record[ASK_READING_LENGTH_AT] + ASK_READING_AT;
    first = meaning;
    meaning += count;
    while (at < end) {
        meaning->kind = *at++;
        if ((meaning->frequency = *at++) > *frequency) {
            *frequency = meaning->frequency;
        }
        if (*at <= ASK_LAST_FORM) {
            meaning->form = *at++;
        } else {
            meaning->form = 0;
        }
        meaning->reserved = 0;
        if (!seen_meaning(first, meaning)) {
            meaning++;
            if (++count >= ASK_MOST_MEANINGS) {
                break;
            }
        }
        at = next_word(at, end);
    }
    return count;
}
static int seen_meaning(struct ask_synonym *first,
                        struct ask_synonym *candidate)
{
    for (; first < candidate; first++) {
        if (first->kind == candidate->kind && first->form == candidate->form) {
            if (first->frequency < candidate->frequency) {
                first->frequency = candidate->frequency;
            }
            return 1;
        }
    }
    return 0;
}
static Uint8 *next_word(Uint8 *at, const Uint8 *end)
{
    /* @hack spelling: a while on at alone that breaks at the kind, where a
     * while on both tests would do */
    while (at < end) {
        if (*at <= ASK_LAST_KIND) {
            break;
        }
        at += 2;
    }
    return at;
}

int dic_getsyn(const char *reading, Sint16 length,
               struct ask_reading_match *match)
{
    char key[KEY_BYTES];
    struct ask_lookup main;
    struct ask_lookup temporary;
    int frequency;
    int main_result;
    int temporary_result;
    if (dic_fd == ASK_NO_DICTIONARY) {
        return ASK_DIC_CLOSED;
    }
#ifdef NONMATCHING
    /* this side refuses a length key can't hold with its NUL, and starts both
     * lookups empty, for a search that returns before it fills one */
    if (length < 0 || length >= KEY_BYTES) {
        return ASK_DIC_NONE;
    }
    main.row = 0;
    main.record = temporary.record = NULL;
#endif
    strncpy(key, reading, length);
    key[length] = 0;
    main_result = exact_reading((Uint8 *)key, length, &main);
#ifdef DREAMCAST
    /* @hack redundant: an unreachable copy of reading's first byte */
    if (0) {
        key[0] = *reading;
    }
#endif
    temporary_result = tmp_getsyn(key, length, &temporary);
    if (main_result == ASK_DIC_NONE && temporary_result == ASK_DIC_NONE) {
        return ASK_DIC_NONE;
    }
    match->length = length;
    match->prefix_length = length;
    match->found = 1;
    match->count = 0;
    frequency = 0;
    if (main_result == ASK_DIC_FOUND) {
        match->count =
            append_meanings(main.record, match->meanings, 0, &frequency);
    }
    if (temporary_result == ASK_DIC_FOUND) {
        match->count = append_meanings(temporary.record, match->meanings,
                                       match->count, &frequency);
    }
    if ((match->entry = set_entid_tab(main.row, main.offset, temporary.offset,
                                      frequency)) == -1) {
        return ASK_DIC_NONE;
    }
    return ASK_DIC_FOUND;
}

static int exact_reading(Uint8 *key, Sint16 length, struct ask_lookup *match)
{
    int row;
    Uint8 *page;
    Uint8 *record;
    Sint16 size;
    int reading_length;
    int order;
    if (length <= 2 && chk_entry2(key, length)) {
        match->offset = -1;
        return ASK_DIC_NONE;
    }
    row = srch_page(key);
#ifdef NONMATCHING
    /* this side stops where the row lies outside the main dictionary's
     * mainupper rows, which load_page would read past */
    if (row < 0 || row >= mainupper) {
        match->offset = -1;
        return ASK_DIC_NONE;
    }
#endif
    record = page = load_page(row);
    while (RECORD_SIZE(record)) {
#ifdef DREAMCAST
        /* @hack redundant: an unreachable return of size */
        if (0) {
            return size;
        }
#endif
        reading_length = size = record[ASK_READING_LENGTH_AT];
        if ((order = strncmp((char *)record + ASK_READING_AT, (char *)key,
                             reading_length)) == 0) {
            if (reading_length == length) {
                break;
            }
        } else if (order > 0) {
            match->offset = -1;
            return ASK_DIC_NONE;
        }
        record += RECORD_SIZE(record);
    }
    if (RECORD_SIZE(record) == 0) {
        match->offset = -1;
        return ASK_DIC_NONE;
    }
    match->row = row;
    match->offset = record - page;
    match->record = record;
    return ASK_DIC_FOUND;
}

int dic_get1wd(unsigned int entry, Uint8 kind, Uint8 form,
               struct ask_dictionary_word *output)
{
    int frequency;
    int row;
    int offset;
    int temporary;
    int time;
    Uint8 *record;
    if (dic_fd == ASK_NO_DICTIONARY) {
        return ASK_DIC_CLOSED;
    }
    if ((row = get_entid_tab(entry, &offset, &temporary, &time)) == -1) {
        return ASK_DIC_NONE;
    }
    frequency = -1;
    if (offset != -1) {
        record = load_page(row) + offset;
        best_word(record, kind, form, &frequency, output, 0);
    }
    if (temporary != -1) {
        record = load_temp(temporary);
        best_word(record, kind, form, &frequency, output, ASK_TEMP_WORD);
    }
    if (frequency == -1) {
        return ASK_DIC_NONE;
    }
    return ASK_DIC_FOUND;
}

static void best_word(Uint8 *record, int kind, int form, int *frequency,
                      struct ask_dictionary_word *output, int flags)
{
    Uint8 *end;
    Uint8 *at;
    Uint8 *best;
    int previous;
    int rank;
    int word_form;
    Uint8 *word;
    end = record + RECORD_SIZE(record);
    at = record + record[ASK_READING_LENGTH_AT] + ASK_READING_AT;
    previous = *frequency;
#ifdef NONMATCHING
    /* this side starts best at record. GCC can't see that best is set wherever
     * it is read: the copy runs only once a word has raised *frequency, and
     * that word set best */
    best = record;
#endif
    while (at < end) {
        word = at;
        rank = at[ASK_FREQUENCY_AT];
        at += ASK_FORM_AT;
        if (*at <= ASK_LAST_FORM) {
            word_form = *at++;
        } else {
            word_form = 0;
        }
        if (kind == *word && form == word_form && rank > *frequency) {
            best = word;
            *frequency = rank;
        }
        at = next_word(at, end);
    }
    if (*frequency > previous) {
        copy_word(output, best, end, (best - record) | flags);
    }
}
static void copy_word(struct ask_dictionary_word *output, Uint8 *word,
                      Uint8 *end, int id)
{
    output->id = id;
    output->frequency = word[ASK_FREQUENCY_AT];
    word += ASK_FORM_AT;
    output->form = 0;
    if (*word <= ASK_LAST_FORM) {
        word++;
    }
    getkbuf(output->text, word, end);
}

int dic_getallwd(unsigned int entry, Uint8 kind, Uint8 form,
                 struct ask_dictionary_word *output, int *count)
{
    int row;
    int found;
    int used;
    int offset;
    int temporary;
    Uint8 *record;
    if (dic_fd == ASK_NO_DICTIONARY) {
        return ASK_DIC_CLOSED;
    }
    /* @hack spelling: found takes get_entid_tab's time, which nothing reads,
     * where a local of its own would do */
    if ((row = get_entid_tab(entry, &offset, &temporary, &found)) == -1) {
        return ASK_DIC_NONE;
    }
    found = 0;
    used = 0;
    if (offset != -1) {
        record = load_page(row) + offset;
        append_words(record, kind, form, &found, &used, output, 0);
    }
    if (temporary != -1) {
        record = load_temp(temporary);
        append_words(record, kind, form, &found, &used, output, ASK_TEMP_WORD);
    }
    if ((*count = found) == 0) {
        return ASK_DIC_NONE;
    }
    return ASK_DIC_FOUND;
}

static void append_words(Uint8 *record, int kind, int form, int *count,
                         int *used, struct ask_dictionary_word *output,
                         int flags)
{
    Uint8 *end;
    Uint8 *at;
    Uint8 *word;
    int frequency;
    int size;
    int word_form;
    struct ask_dictionary_word *slot;
    end = record + RECORD_SIZE(record);
    at = record + record[ASK_READING_LENGTH_AT] + ASK_READING_AT;
    while (at < end) {
        word = at;
        frequency = at[ASK_FREQUENCY_AT];
        at += ASK_FORM_AT;
        if (*at <= ASK_LAST_FORM) {
            /* @hack spelling: the read and the step as two statements, where
             * *at++ would do */
            word_form = *at;
            at++;
        } else {
            word_form = 0;
        }
        if (kind == *word && form == word_form) {
            size = getkbuflen(at, end) + (ASK_WORD_TEXT + 1);
            if (size & 1) {
                size++;
            }
#ifdef NONMATCHING
            /* this side stops where the word wouldn't fit in the room output
             * has past used */
            if (size > (int)sizeof(*output) - *used) {
                return;
            }
#endif
            slot = (struct ask_dictionary_word *)insert_word(
                (Uint8 *)output, frequency, size, *used);
            copy_word(slot, word, end, (word - record) | flags);
            *used += size;
            (*count)++;
        }
        at = next_word(at, end);
    }
}

static Uint8 *insert_word(Uint8 *buffer, int frequency, int size, int used)
{
    Uint8 *end;
    Uint8 *next;
    end = buffer + used;
    while (buffer < end) {
        if (((struct ask_dictionary_word *)buffer)->frequency < frequency) {
            break;
        }
        next = buffer + ASK_WORD_TEXT;
        while (*next++) {
        }
        if ((long)next & 1) {
            next++;
        }
        buffer = next;
    }
    if (buffer < end) {
#ifdef NONMATCHING
        /* this side moves the bytes by memmove. the backward loop ends on a
         * pointer before buffer, which C leaves undefined where buffer starts
         * its array */
        memmove(buffer + size, buffer, end - buffer);
#else
        for (next = end - 1; next >= buffer; next--) {
            next[size] = *next;
        }
#endif
    }
    return buffer;
}

int dic_learn(unsigned int entry, Uint16 synonym, Sint32 *ids, int count)
{
    int time;
    int temporary;
    int offset;
    int row;
    Uint8 *record;
    if (gaku_mode == 0) {
        return ASK_DIC_DONE;
    }
    if (dic_fd == ASK_NO_DICTIONARY) {
        return ASK_DIC_CLOSED;
    }
    /* @hack redundant: synonym >= 0, which an unsigned synonym always passes */
    if (entry == (unsigned int)-1 && synonym >= 0 &&
        synonym <= ASK_FORMAL_NUMERAL) {
        suji_mode = synonym;
        clear_times(ids, count);
        update_entid_rtime(ids, count, 0);
        return ASK_DIC_DONE;
    }
    if ((row = get_entid_tab(entry, &offset, &temporary, &time)) == -1) {
        return ASK_DIC_NONE;
    }
    if (synonym & ASK_TEMP_WORD) {
        if (temporary == -1) {
            return ASK_DIC_NONE;
        }
        record = load_temp(temporary);
    } else {
        if (offset == -1) {
            return ASK_DIC_NONE;
        }
        record = load_page(row) + offset;
    }
    time = get_maxtime(ids, count);
#ifdef NONMATCHING
    /* this side refuses a synonym past its record's end */
    if ((synonym & ~ASK_TEMP_WORD) + 1 >= RECORD_SIZE(record)) {
        return ASK_DIC_NONE;
    }
#endif
    record = record + (synonym & ~ASK_TEMP_WORD) + ASK_FREQUENCY_AT;
    if (*record && *record == time) {
        if (synonym & ASK_TEMP_WORD) {
            shift_temp(temporary);
            update_nowtmp();
        }
        return ASK_DIC_DONE;
    }
    if (++time == TIME_LIMIT) {
        clear_times(ids, count);
        time = 1;
    }
    *record = time;
    if (synonym & ASK_TEMP_WORD) {
        shift_temp(temporary);
        update_nowtmp();
    } else {
        update_nowpage();
    }
    update_entid_rtime(ids, count, time);
    return ASK_DIC_DONE;
}
static void clear_record_time(Uint8 *record)
{
    Uint8 *end;
    Uint8 *at;
    end = record + RECORD_SIZE(record);
    at = record + record[ASK_READING_LENGTH_AT] + ASK_READING_AT;
    while (at < end) {
        at[ASK_FREQUENCY_AT] = 0;
        at += ASK_FORM_AT;
        if (*at <= ASK_LAST_FORM) {
            at++;
        }
        at = next_word(at, end);
    }
}
static void clear_times(Sint32 *ids, int count)
{
    int offset;
    int temporary;
    int time;
    int i;
    int row;
    for (i = 0; i < count; i++) {
        if ((row = get_entid_tab(ids[i], &offset, &temporary, &time)) == -1) {
            continue;
        }
        if (offset != -1) {
            clear_record_time(load_page(row) + offset);
            update_nowpage();
        }
        if (temporary != -1) {
            clear_record_time(load_temp(temporary));
            update_nowtmp();
        }
    }
}
static int max_record_time(Uint8 *record)
{
    Uint8 *end;
    Uint8 *at;
    int latest = 0;
    end = record + RECORD_SIZE(record);
    at = record + record[ASK_READING_LENGTH_AT] + ASK_READING_AT;
    while (at < end) {
        if (latest < at[ASK_FREQUENCY_AT]) {
            latest = at[ASK_FREQUENCY_AT];
        }
        at += ASK_FORM_AT;
        if (*at <= ASK_LAST_FORM) {
            at++;
        }
        at = next_word(at, end);
    }
    return latest;
}

int dic_touroku(struct ask_learn_word *word)
{
    char *reading;
    char key[KEY_BYTES];
    Uint8 *page;
    Uint8 *end;
    Uint8 *record;
    Sint16 size;
    Bool fresh;
    int length;
    int order;
    fresh = TRUE;
    if (dic_fd == ASK_NO_DICTIONARY) {
        return ASK_DIC_CLOSED;
    }
    if (dic_rw == ASK_DIC_READ_ONLY_MODE) {
        return ASK_DIC_READ_ONLY;
    }
#ifdef NONMATCHING
    /* this side refuses a reading length outside 1 to KEY_BYTES - 1, which key
     * can't hold with its NUL */
    if (word->length < 1 || word->length >= KEY_BYTES) {
        return ASK_DIC_FULL;
    }
#endif
    /* @hack spelling: reading, a second pointer to key, and &key[0] below,
     * where key itself would do */
    strncpy(reading = key, (char *)word->reading, word->length);
    key[word->length] = 0;
    page = load_page(srch_page((Uint8 *)&key[0]));
    end = end_page(page);
    for (record = page; record < end; record += RECORD_SIZE(record)) {
        size = record[ASK_READING_LENGTH_AT];
        if ((order = strncmp((char *)record + ASK_READING_AT, reading, size)) ==
            0) {
            if (size == word->length) {
                fresh = FALSE;
                break;
            }
        } else if (order > 0) {
            break;
        }
    }
    length = fresh ? newwdlen(word) : updwdlen(word);
#ifdef DREAMCAST
    /* @hack redundant: an unreachable return of *end */
    if (0) {
        return *end;
    }
#endif
#ifdef NONMATCHING
    /* this side compares lengths, where the matching side forms page - length,
     * a pointer that can lie before the page */
    if (length > ASK_PAGE_BYTES - ASK_END_MARK - (end - page)) {
        return ASK_DIC_FULL;
    }
#else
    if (end > page - length + (ASK_PAGE_BYTES - ASK_END_MARK)) {
        return ASK_DIC_FULL;
    }
#endif
    if (fresh) {
        shift_page(record, end + ASK_END_MARK, length);
        set_record(record, length, word, 0);
        if (word->length <= 2) {
            set_entry2((Uint8 *)key, word->length);
        }
    } else {
        shift_page(record + RECORD_SIZE(record), end + ASK_END_MARK, length);
        upd_record(record, length, word, max_record_time(record) + 1);
    }
    init_entid_tab();
    update_nowpage();
    return ASK_DIC_DONE;
}
static Uint8 *end_page(Uint8 *page)
{
    int size;
    while ((size = RECORD_SIZE(page)) != 0) {
        page += size;
    }
    return page;
}
static void shift_page(Uint8 *first, Uint8 *end, int shift)
{
#ifdef NONMATCHING
    /* this side moves the bytes by memmove. the matching side's backward loop
     * ends on a pointer before first, which C leaves undefined where first
     * starts its array */
    if (first < end && shift) {
        memmove(first + shift, first, end - first);
    }
#else
    Uint8 *byte;
    if (shift > 0) {
        for (byte = end - 1; first <= byte; byte--) {
            byte[shift] = *byte;
        }
    } else if (shift < 0) {
        for (byte = first; byte < end; byte++) {
            byte[shift] = *byte;
        }
    }
#endif
}

int dic_delete(struct ask_learn_word *word)
{
    char *reading;
    char key[KEY_BYTES];
    Uint8 *page;
    Uint8 *end;
    Uint8 *record;
    Sint16 length;
    Bool missing;
    int removed;
    int order;
    missing = TRUE;
    if (dic_fd == ASK_NO_DICTIONARY) {
        return ASK_DIC_CLOSED;
    }
    if (dic_rw == ASK_DIC_READ_ONLY_MODE) {
        return ASK_DIC_READ_ONLY;
    }
#ifdef NONMATCHING
    /* this side refuses a reading length outside 1 to KEY_BYTES - 1, which key
     * can't hold with its NUL */
    if (word->length < 1 || word->length >= KEY_BYTES) {
        return ASK_DIC_NONE;
    }
#endif
    /* @hack spelling: reading, a second pointer to key, where key itself
     * would do */
    strncpy(reading = key, (char *)word->reading, word->length);
    key[word->length] = 0;
    page = load_page(srch_page((Uint8 *)key));
    end = end_page(page);
    for (record = page; record < end; record += RECORD_SIZE(record)) {
        length = record[ASK_READING_LENGTH_AT];
        if ((order = strncmp((char *)record + ASK_READING_AT, reading,
                             length)) == 0) {
            if (length == word->length) {
                missing = FALSE;
                break;
            }
        } else if (order > 0) {
            break;
        }
    }
    if (missing) {
        return ASK_DIC_NONE;
    }
    if ((removed = delete_word(record, word)) == 0) {
        return ASK_DIC_NONE;
    }
    if (removed == -1) {
        shift_page(record + RECORD_SIZE(record), end + ASK_END_MARK,
                   -RECORD_SIZE(record));
    } else {
        shift_page(record + RECORD_SIZE(record) + removed, end + ASK_END_MARK,
                   -removed);
    }
    init_entid_tab();
    update_nowpage();
    return ASK_DIC_DONE;
}
/* remove word, by its kind and writing, from the record at cursor, and
 * return the bytes it took. it returns 0 where the record lacks the word.
 * where it is the record's only word, delete_word removes nothing and
 * returns -1 */
static int delete_word(Uint8 *cursor, struct ask_learn_word *word)
{
    Uint8 writing[128];
    int writing_length;
    Uint8 *reading;
    Uint8 *start;
    Uint8 *text;
    Uint8 *end;
    Uint8 *record;
    int size;
    int removed;
#ifdef NONMATCHING
    /* this side refuses a writing too long for writing to hold with its NUL */
    if (setkbuflen(word->writing) >= (int)sizeof(writing)) {
        return 0;
    }
#endif
    setkbuf(word->writing, writing);
    writing_length = setkbuflen(word->writing);
    writing[writing_length] = 0;
    record = cursor;
    size = RECORD_SIZE(cursor);
    end = record + size;
    /* @hack redundant: cursor stepped to the reading's length, which the loop
     * sets again */
    reading = cursor += ASK_READING_LENGTH_AT;
    for (cursor = record + *reading + ASK_READING_AT;
         start = cursor, cursor < end;) {
        cursor += ASK_FORM_AT;
        if (*cursor <= ASK_LAST_FORM) {
            cursor++;
        }
        text = cursor;
        cursor = next_word(cursor, end);
        removed = cursor - text;
        if (*start == word->kind && removed == writing_length &&
            strncmp((char *)text, (char *)writing, removed) == 0) {
            break;
        }
    }
#ifdef DREAMCAST
    /* @hack redundant: an unreachable store of removed */
    if (0) {
        record[0] = removed;
    }
#endif
    if ((removed = cursor - start) <= 0) {
        return removed;
    }
    if (start == record + *reading + ASK_READING_AT && cursor == end) {
        return -1;
    }
    shift_page(cursor, end, -removed);
    size -= removed;
    record[0] = size % 256;
    record[1] = size / 256;
    return removed;
}

int dic_tmptouroku(struct ask_learn_word *word)
{
    char key[KEY_BYTES];
    if (gaku_mode == 0) {
        return ASK_DIC_DONE;
    }
    if (dic_fd == ASK_NO_DICTIONARY) {
        return ASK_DIC_CLOSED;
    }
#ifdef NONMATCHING
    /* this side refuses a reading length outside 1 to KEY_BYTES - 1, which key
     * can't hold with its NUL */
    if (word->length < 1 || word->length >= KEY_BYTES) {
        return ASK_DIC_NONE;
    }
#endif
    strncpy(key, (char *)word->reading, word->length);
    if (word->kind == ASK_RAW_WORD || word->kind == ASK_SYMBOL_WORD) {
        word->kind = 39;
    }
    key[word->length] = 0;
    tmp_touroku((Uint8 *)key, word, 1);
    return ASK_DIC_DONE;
}

int dic_newlearn(struct ask_learn_word *word, Sint32 *ids, int count)
{
    char key[KEY_BYTES];
    int time;
    if (gaku_mode == 0) {
        return ASK_DIC_DONE;
    }
    if (dic_fd == ASK_NO_DICTIONARY) {
        return ASK_DIC_CLOSED;
    }
#ifdef NONMATCHING
    /* this side refuses a reading length outside 1 to KEY_BYTES - 1, which key
     * can't hold with its NUL */
    if (word->length < 1 || word->length >= KEY_BYTES) {
        return ASK_DIC_NONE;
    }
#endif
    strncpy(key, (char *)word->reading, word->length);
    key[word->length] = 0;
    if (is_number((Uint8 *)key)) {
        word->kind = ASK_NUMBER_WORD;
    }
    if ((time = get_maxtime(ids, count) + 1) == TIME_LIMIT) {
        clear_times(ids, count);
        time = 1;
    }
    tmp_touroku((Uint8 *)key, word, time);
    update_entid_rtime(ids, count, time);
    return ASK_DIC_DONE;
}
static int is_number(const Uint8 *reading)
{
    while (*reading) {
        if (*reading < '0' || *reading > '9') {
            return 0;
        }
        reading++;
    }
    return 1;
}
int dic_freeentid(unsigned int entry)
{
    free_entid_tab(entry);
    return ASK_DIC_DONE;
}
int dic_setgaku(int mode)
{
    if (mode == 1 || mode == 0) {
        gaku_mode = mode;
        return ASK_DIC_DONE;
    }
    return ASK_DIC_INVALID;
}
int dic_getgaku(void)
{
    return gaku_mode;
}

int dic_get1num(const Uint8 *reading, Sint16 length,
                struct ask_dictionary_word *output)
{
    if (dic_fd == ASK_NO_DICTIONARY) {
        return ASK_DIC_CLOSED;
    }
    if (!write_number(reading, length, output, suji_mode)) {
        write_number(reading, length, output, ASK_FULL_WIDTH_DIGITS);
    }
    return ASK_DIC_FOUND;
}

static Uint8 *write_number(const Uint8 *reading, Sint16 length,
                           struct ask_dictionary_word *output, int mode)
{
    Uint8 *cursor;
    Uint8 *group;
    int i;
    int place;
    int position;
    int digit;
    int table;
    int unit;
#ifdef NONMATCHING
    /* this side refuses a length outside 1 to KEY_BYTES - 1, and a mode past
     * ASK_FORMAL_NUMERAL, which indexes numerals */
    if (length <= 0 || length >= KEY_BYTES || mode < 0 ||
        mode > ASK_FORMAL_NUMERAL) {
        return NULL;
    }
#endif
    if (mode >= ASK_KANJI_NUMERAL && length > MOST_DIGITS) {
        return NULL;
    }
    output->id = mode;
    output->frequency = 0;
    output->form = 0;
    group = cursor = output->text;
    for (i = 0; i < length; i++) {
        digit = reading[i] - '0';
#ifdef NONMATCHING
        /* this side refuses a reading that isn't all digits, which index
         * numerals */
        if (digit < 0 || digit > 9) {
            return NULL;
        }
#endif
        if (mode < ASK_KANJI_NUMERAL) {
            *cursor++ = numerals[mode][digit * KANJI_BYTES];
            *cursor++ = numerals[mode][(digit * KANJI_BYTES) + 1];
            continue;
        }
        place = length - 1 - i;
        position = place % GROUP_PLACES;
        /* kanji numerals leave out the one of ten and of a hundred. a form
         * with units finds one to three at numerals[mode - 1] */
        if (digit != 0 && !(mode == ASK_KANJI_NUMERAL && digit == 1 &&
                            (position == 1 || position == 2))) {
            table = digit >= 1 && digit <= 3 ? mode - 1 : ASK_KANJI_DIGITS;
            *cursor++ = numerals[table][digit * KANJI_BYTES];
            *cursor++ = numerals[table][(digit * KANJI_BYTES) + 1];
        }
        if (place == 0 || (position == 0 && group == cursor) ||
            (position != 0 && digit == 0)) {
            continue;
        }
        if (position == 0) {
            unit = (place / GROUP_PLACES) + THOUSAND;
        } else if (position == 1) {
            unit = mode == ASK_KANJI_NUMERAL ? TEN : FORMAL_TEN;
        } else {
            unit = position + (HUNDRED - 2);
        }
        *cursor++ = numerals[UNIT_TABLE][unit * KANJI_BYTES];
        *cursor++ = numerals[UNIT_TABLE][(unit * KANJI_BYTES) + 1];
        if (position == 0) {
            group = cursor;
        }
    }
    if (cursor == output->text) {
        return NULL;
    }
    *cursor = 0;
    return cursor + 1;
}

int dic_getallnum(const Uint8 *reading, Sint16 length,
                  struct ask_dictionary_word *output, int *count)
{
    Uint8 *next;
    int mode;
    if (dic_fd == ASK_NO_DICTIONARY) {
        return ASK_DIC_CLOSED;
    }
    *count = 0;
    if ((next = write_number(reading, length, output, suji_mode)) != NULL) {
        output = (struct ask_dictionary_word *)next;
        (*count)++;
    }
    for (mode = 0; mode < ASK_NUMBER_FORMS; mode++) {
        if (mode == suji_mode) {
            continue;
        }
        if ((next = write_number(reading, length, output, mode)) != NULL) {
            output = (struct ask_dictionary_word *)next;
            (*count)++;
        }
    }
    return ASK_DIC_FOUND;
}
int dic_clearmem(void)
{
    return ASK_DIC_DONE;
}
