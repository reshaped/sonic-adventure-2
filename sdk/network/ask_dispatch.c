#include <sg_xpt.h>
#include "ask_session.h"

int func_mode;
int flush_mode;
int lock_mode;
int echo_lock;
Uint8 prev_yomi[80];
Uint8 prev_tango[80];
Uint8 *entry2code;
int entry2upd;

#include <apiask.h>
#include "ask_requests.h"
#include "ask_candidates.h"
#include "ask_conversion.h"
#include "ask_dictionary.h"
#include "ask_editor.h"
#include "ask_ime.h"
#include "ask_input.h"
#include "ask_jis.h"
#include "ask_pages.h"
#include "ask_phrases.h"
#include <string.h>
#include <machine.h>
#ifdef NONMATCHING
/* this side calls to_ucode and strcpy as they are, and ends the letters by
 * their own count, the plain forms the hack tags below give */
#define READING_CODE to_ucode
#define COPY_STRING strcpy
#define ROMAN_END (ASK_ROMAN_LETTERS + 512)
#else
#ifdef DREAMCAST
/* @hack type: to_ucode called as a function returning Uint16, where its own
 * Uint8 would do: the caller keeps the low byte alone */
#define READING_CODE ((Uint16 (*)(Uint16))to_ucode)
#else
#define READING_CODE to_ucode
#endif
/* @hack spelling: SHC's _builtin_strcpy, where strcpy would do */
#define COPY_STRING _builtin_strcpy
/* @hack spelling: the end of the letters as &wdsbuf[1], where
 * ASK_ROMAN_LETTERS + 512 would do */
#define ROMAN_END ((Uint16 *)&wdsbuf[1])
#endif

#ifdef NONMATCHING
/* this side reads each request through its struct, the plain form the hack tag
 * below gives */
typedef const void *ask_arguments;
#define COMMAND(args) (*(const int *)(args))
#define FIELD(args, record, member, type, index)                               \
    ((const struct record *)(args))->member
#else
/* @hack type: a request read as an array of the words after its command,
 * where a pointer to its struct, such as struct ask_string_pair, would do */
typedef const Sint32 *ask_arguments;
#define COMMAND(args) ((args)[-1])
#define FIELD(args, record, member, type, index) ((type)(args)[index])
#endif
/* where Shift-JIS would code hiragana VU, which it lacks, and the katakana VU
 * roman_to_kana writes for it */
#define SJIS_HIRAGANA_VU 0x82f2
#define SJIS_KATAKANA_VU 0x8394

static int roman_to_kana(ask_arguments args);
static int convert(ask_arguments args);
static int move_candidate(ask_arguments args);
static int move_group(ask_arguments args);
static int commit_all(ask_arguments args);
static int commit_first(ask_arguments args);
static int open_dictionary(ask_arguments args);
static int close_dictionary(ask_arguments args);
static int register_word(ask_arguments args);
static int set_dictionary_name(ask_arguments args);
static int extend_phrase(ask_arguments args);
static int shorten_phrase(ask_arguments args);
static int previous_phrase(ask_arguments args);
static int next_phrase(ask_arguments args);
static int change_form(ask_arguments args);
static int unsupported(ask_arguments args);
static int dictionary_category(int category);
static void show_selection(char *candidate, char *suffix);

#pragma section ATA
static int (*handlers[63])(ask_arguments) = {
    unsupported,      unsupported,    unsupported,     unsupported,
    unsupported,      unsupported,    unsupported,     unsupported,
    unsupported,      unsupported,    unsupported,     unsupported,
    unsupported,      unsupported,    roman_to_kana,   roman_to_kana,
    unsupported,      unsupported,    convert,         move_candidate,
    move_candidate,   move_group,     move_group,      commit_all,
    commit_first,     unsupported,    unsupported,     open_dictionary,
    close_dictionary, register_word,  register_word,   set_dictionary_name,
    extend_phrase,    shorten_phrase, previous_phrase, next_phrase,
    change_form,      change_form,    change_form,     change_form,
    change_form,      change_form,    change_form,     change_form,
    unsupported,      unsupported,    unsupported,     unsupported,
    unsupported,      unsupported,    unsupported,     unsupported,
    unsupported,      unsupported,    unsupported,     unsupported,
    unsupported,      unsupported,    unsupported,     unsupported,
    unsupported,      unsupported,    unsupported};
/* the word kind of each category */
static Uint8 dictionary_categories[32] = {
    1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 12, 13, 14, 19, 20,
    22, 24, 40, 27, 28, 26, 29, 30, 31, 31, 50, 32, 33, 34, 38};
static char candidate_label[8] = "  1.";
static char remaining_label[] = "  \x8e\x63      ";
#pragma section

int api_funcent(void *request)
{
    ask_arguments args = request;
#ifdef NONMATCHING
    /* this side reads the command at the request's start, and leaves args there
     * for FIELD's struct reads */
    int command = COMMAND(args);
#else
    Sint32 command = *args++;
#endif
    if (command <= 0 ||
        (Uint32)command > sizeof(handlers) / sizeof(handlers[0])) {
        return -1;
    }
    return handlers[command - 1](args);
}

static int roman_to_kana(ask_arguments args)
{
    Sint32 command = COMMAND(args);
    /* @hack redundant: input, output, letter and next declared register, where
     * plain locals would do */
    register Uint8 *input =
        (Uint8 *)FIELD(args, ask_string_pair, first, char *, 0);
    register Uint8 *output =
        (Uint8 *)FIELD(args, ask_string_pair, second, char *, 1);
    Uint16 attribute;
    register Uint16 *letter;
    register Uint16 *next;
    Uint16 code;
    Uint16 kana[2];
    int count;
    int produced;
    int consumed;
    if (command == ASK_ROMAN_TO_KATAKANA) {
        attribute = ASK_FULL_WIDTH | ASK_AS_KATAKANA;
    } else {
        attribute = ASK_FULL_WIDTH;
    }
    for (letter = ASK_ROMAN_LETTERS, count = 0; *input && letter < ROMAN_END;
         letter++, count++) {
        *letter = *input++ | attribute;
    }
    letter = ASK_ROMAN_LETTERS;
    while (count > 0) {
        consumed = to_roman((Sint16 *)letter, count, kana, &produced);
        if (consumed <= 0) {
            if (consumed != -1) {
                return letter - ASK_ROMAN_LETTERS + 1;
            }
            consumed = 1;
        }
        letter += consumed;
        count -= consumed;
        next = kana;
        while (produced-- != 0) {
            code = jis2sjis(*next++);
            if (code == SJIS_HIRAGANA_VU) {
                code = SJIS_KATAKANA_VU;
            }
            *output++ = code >> 8;
            *output++ = code;
        }
    }
    *output = 0;
    return 0;
}

static int convert(ask_arguments args)
{
    char *reading;
    char *candidate;
    char *suffix;
    if (func_mode == ASK_SESSION_CLOSED) {
        return -1;
    }
    reading = FIELD(args, ask_conversion, reading, char *, 0);
    candidate = FIELD(args, ask_conversion, candidate, char *, 1);
    suffix = FIELD(args, ask_conversion, suffix, char *, 2);
    if (reading != NULL) {
        if (func_mode >= ASK_SESSION_CONVERTING) {
            init_edit0();
        }
        if (add_kana_buf(reading) < 0) {
            return -1;
        }
    }
    if (kana_len <= 0) {
        *candidate = 0;
        *suffix = 0;
        return 0;
    }
    henkan(0, kana_len, ASK_FREE_LENGTHS, -1);
    cur_pos = 0;
    cur_len = bunsetu_len(cur_pos);
    init_kouho(0, 1);
    show_selection(candidate, suffix);
    func_mode = ASK_SESSION_SELECTING;
    return kh_count(hchar[0].candidates);
}

static void show_selection(char *candidate, char *suffix)
{
    Uint8 *text;
    text = select_subtostr(cur_pos, cur_len);
    COPY_STRING(candidate, (char *)text);
    text = select_subtostr(cur_pos + cur_len, kana_len - cur_pos - cur_len);
    COPY_STRING(suffix, (char *)text);
}

static int move_candidate(ask_arguments args)
{
    char *candidate;
    char *suffix;
    Sint32 command;
    int last;
    if (func_mode != ASK_SESSION_SELECTING) {
        /* @hack spelling: gotos to the returns at the end, where each return
         * in place would do */
        goto inactive;
    }
    command = COMMAND(args);
    candidate = FIELD(args, ask_string_pair, first, char *, 0);
    suffix = FIELD(args, ask_string_pair, second, char *, 1);
    switch (command) {
    case ASK_PREVIOUS_CANDIDATE:
        if (gun_nkh > 0) {
            gun_nkh--;
            break;
        }
        if (back_gun(0, 0) == 0) {
            init_kouho(0, 1);
            for (last = gun_num; next_gun(0, 0) != 0; last += gun_num) {
            }
            init_kouho(last - 1, 1);
        } else {
            gun_nkh = gun_num - 1;
        }
        break;
    case ASK_NEXT_CANDIDATE:
        if (gun_nkh < gun_num - 1) {
            gun_nkh++;
        } else if (next_gun(0, 0) == 0) {
            init_kouho(0, 1);
        }
        break;
    }
    show_selection(candidate, suffix);
    goto selected;
inactive:
    return 0;
selected:
    return gun_nkh + 1;
}

static int move_group(ask_arguments args)
{
    char *candidate;
    char *suffix;
    Sint32 command;
    Bool wrapped = FALSE;
    if (func_mode != ASK_SESSION_SELECTING) {
        return 0;
    }
    command = COMMAND(args);
    candidate = FIELD(args, ask_string_pair, first, char *, 0);
    suffix = FIELD(args, ask_string_pair, second, char *, 1);
    switch (command) {
    case ASK_PREVIOUS_GROUP:
        if (back_gun(0, 0) == 0) {
            wrapped = TRUE;
        }
        break;
    case ASK_NEXT_GROUP:
        if (next_gun(0, 0) == 0) {
            wrapped = TRUE;
        }
        break;
    }
    show_selection(candidate, suffix);
    if (wrapped) {
        return 0;
    }
    return gun_num;
}

static int commit_all(ask_arguments args)
{
    char *output;
    Uint8 *text;
    if (func_mode != ASK_SESSION_SELECTING) {
        return -1;
    }
    output = FIELD(args, ask_commit, text, char *, 0);
    wd_learn(0, kana_len);
    text = select_tostr();
    COPY_STRING(output, (char *)text);
    init_edit0();
    func_mode = ASK_SESSION_EDITING;
    return 0;
}

static int commit_first(ask_arguments args)
{
    Sint32 index;
    char *output;
    char *candidate;
    char *suffix;
    Sint16 *count;
    int bytes;
    int length;
    if (func_mode != ASK_SESSION_SELECTING) {
        return -1;
    }
    index = FIELD(args, ask_first_commit, index, int, 0);
    if (index <= 0 || index > gun_num) {
        return 0;
    }
    output = FIELD(args, ask_first_commit, text, char *, 1);
    /* @hack spelling: ask_first_commit's last three fields read last first,
     * where their own order would do */
    count = FIELD(args, ask_first_commit, count, Sint16 *, 4);
    suffix = FIELD(args, ask_first_commit, suffix, char *, 3);
    candidate = FIELD(args, ask_first_commit, candidate, char *, 2);
    gun_nkh = index - 1;
    /* @hack spelling: length, a copy of cur_len, where cur_len itself would
     * do */
    length = cur_len;
    COPY_STRING(output, (char *)select_subtostr(cur_pos, length));
    unify_khmem(cur_pos, 1);
    bytes = count_byte_kana_buf(cur_pos, cur_len);
    cur_pos += cur_len;
    if (cur_pos >= kana_len) {
        *candidate = 0;
        *suffix = 0;
        *count = 0;
        return bytes;
    }
    cur_len = bunsetu_len(cur_pos);
    init_kouho(0, 1);
    show_selection(candidate, suffix);
    *count = kh_count(hchar[cur_pos].candidates);
    return bytes;
}

static int open_dictionary(ask_arguments args)
{
    (void)args;
    if (lock_mode == 0) {
        return -1;
    }
    if (dic_open(dic_name) == ASK_DIC_CANT_OPEN) {
        return 1;
    }
    into_editing(0);
    func_mode = ASK_SESSION_EDITING;
    return 0;
}

static int close_dictionary(ask_arguments args)
{
    (void)args;
    if (lock_mode == 0) {
        return -1;
    }
    init_edit0();
    dic_close();
    func_mode = ASK_SESSION_CLOSED;
    return 0;
}

static int register_word(ask_arguments args)
{
    struct ask_learn_word word;
    Uint8 *reading;
    char *writing;
    Uint8 *next;
    Uint16 code;
    Sint32 command;
    int length;
    int category;
    int result;
    if (im_state >= 2 || func_mode >= ASK_SESSION_CONVERTING) {
        return ASK_REGISTER_REFUSED;
    }
    command = COMMAND(args);
    if (FIELD(args, ask_registration, dictionary, int, 0) != 1 &&
        FIELD(args, ask_registration, dictionary, int, 0) != 0) {
        return ASK_REGISTER_BAD_DICTIONARY;
    }
    reading = (Uint8 *)FIELD(args, ask_registration, reading, char *, 1);
    writing = FIELD(args, ask_registration, word, char *, 2);
    category = FIELD(args, ask_registration, category, int, 3);
    if ((category = dictionary_category(category)) < 0) {
        return ASK_REGISTER_BAD_CATEGORY;
    }
    next = yomi_buf;
    length = 0;
    while (*reading) {
#ifdef NONMATCHING
        /* this side refuses a reading too long for yomi_buf, or one that ends
         * on half a character */
        if (length >= (int)sizeof(yomi_buf) - 1 || !reading[1]) {
            return ASK_REGISTER_BAD_READING;
        }
#endif
        code = (reading[0] << 8) | reading[1];
        if (!is_shift(code)) {
            return ASK_REGISTER_BAD_READING;
        }
        code = sjis2jis(code);
        if (((code = READING_CODE(code)) & 0xff) == 0) {
            return ASK_REGISTER_BAD_READING;
        }
        *next++ = code;
        length++;
        reading += 2;
    }
    *next = 0;
    word.reading = yomi_buf;
    word.length = length;
    word.reserved = 0;
    word.kind = category;
    word.form = 0;
    word.writing = (Uint8 *)writing;
    if (command == ASK_ADD_WORD) {
        if (dic_touroku(&word) == ASK_DIC_DONE) {
            return ASK_REGISTER_DONE;
        }
    } else if (command == ASK_DELETE_WORD) {
        result = dic_delete(&word);
        if (result == ASK_DIC_DONE) {
            return ASK_REGISTER_DONE;
        }
        if (result == ASK_DIC_NONE) {
            return ASK_REGISTER_MISSING;
        }
    }
    return ASK_REGISTER_REFUSED;
}

static int dictionary_category(int category)
{
    category--;
    if (category < 0 || category >= ASK_CATEGORIES) {
        return -1;
    }
    return dictionary_categories[category];
}

static int set_dictionary_name(ask_arguments args)
{
    int index;
    for (index = 0; index < (int)sizeof(prev_yomi); index++) {
        prev_yomi[index] = 0;
        prev_tango[index] = 0;
    }
#ifdef NONMATCHING
    /* this side refuses a dictionary name too long for dic_name */
    if (strlen(FIELD(args, ask_string_pair, first, char *, 0)) >=
        sizeof(dic_name)) {
        return -1;
    }
#endif
    COPY_STRING(dic_name, FIELD(args, ask_string_pair, first, char *, 0));
    func_mode = ASK_SESSION_CLOSED;
    flush_mode = 0;
    lock_mode = 1;
    echo_lock = 0;
    entry2code = ask_dictionary_header + ASK_HEADER_PREFIX_MAP;
    entry2upd = 0;
    dic_fd = ASK_NO_DICTIONARY;
    gaku_mode = 0;
    suji_mode = ASK_FULL_WIDTH_DIGITS;
    dic_rw = ASK_DIC_READ_WRITE_MODE;
    henkan_mode = ASK_FREE_LENGTHS;
    kd_mode = 1;
    ikkatsu_mode = 0;
    roman_japan = 1;
    ic_flag = ASK_FULL_WIDTH;
    im_state = 0;
    kana_buf_size = ASK_KANA_LIMIT;
    /* @bug memory: writes each label's NUL a byte past its array, at
     * 0x0c9e398e and 0x0c9e3998. kouho_head's lands on kouho_rest, whose copy
     * overwrites it, so draw_group's sstrtom of kouho_head reads on through
     * kouho_rest */
    COPY_STRING((char *)kouho_head, candidate_label);
    COPY_STRING((char *)kouho_rest, remaining_label);
    learn_on = 0;
    undo_cur_pos = 0;
    undo_im_state = 0;
    undo_length = 0;
    save_im_state = 0;
    first_init = 1;
    return 0;
}

static int extend_phrase(ask_arguments args)
{
    char *candidate;
    char *suffix;
    if (func_mode != ASK_SESSION_SELECTING) {
        return -1;
    }
    candidate = FIELD(args, ask_string_pair, first, char *, 0);
    suffix = FIELD(args, ask_string_pair, second, char *, 1);
    if (cur_pos + cur_len >= kana_len) {
        return 0;
    }
    save_fst_bslen(cur_pos);
    free_hchar(cur_pos, kana_len, 1);
    cur_len++;
    henkan(cur_pos, kana_len, ASK_SCORED_LENGTH, cur_len);
    init_kouho(0, 1);
    show_selection(candidate, suffix);
    return kh_count(hchar[cur_pos].candidates);
}

static int shorten_phrase(ask_arguments args)
{
    char *candidate;
    char *suffix;
    if (func_mode != ASK_SESSION_SELECTING) {
        return -1;
    }
    candidate = FIELD(args, ask_string_pair, first, char *, 0);
    suffix = FIELD(args, ask_string_pair, second, char *, 1);
    if (cur_len <= 1) {
        return 0;
    }
    save_fst_bslen(cur_pos);
    free_hchar(cur_pos, kana_len, 1);
    cur_len--;
    henkan(cur_pos, kana_len, ASK_SCORED_LENGTH, cur_len);
    init_kouho(0, 1);
    show_selection(candidate, suffix);
    return kh_count(hchar[cur_pos].candidates);
}

static int previous_phrase(ask_arguments args)
{
    char *candidate;
    char *suffix;
    int first;
    int length;
    if (func_mode != ASK_SESSION_SELECTING) {
        return -1;
    }
    candidate = FIELD(args, ask_string_pair, first, char *, 0);
    suffix = FIELD(args, ask_string_pair, second, char *, 1);
    if (cur_pos == 0) {
        return 0;
    }
#ifdef NONMATCHING
    /* this side returns for a negative cur_pos, and starts length at 0 */
    if (cur_pos < 0) {
        return 0;
    }
    length = 0;
#endif
    unify_khmem(cur_pos, 0);
    for (first = 0; first < cur_pos; first += length) {
        length = bunsetu_len(first);
#ifdef NONMATCHING
        /* this side returns where a phrase has no length, on which the loop
         * would never end */
        if (length <= 0) {
            return 0;
        }
#endif
        if (first + length >= cur_pos) {
            break;
        }
    }
    cur_pos = first;
    cur_len = length;
    init_kouho(0, 1);
    show_selection(candidate, suffix);
    return kh_count(hchar[cur_pos].candidates);
}

static int next_phrase(ask_arguments args)
{
    char *candidate;
    char *suffix;
    if (func_mode != ASK_SESSION_SELECTING) {
        return -1;
    }
    candidate = FIELD(args, ask_string_pair, first, char *, 0);
    suffix = FIELD(args, ask_string_pair, second, char *, 1);
    if (cur_len + cur_pos >= kana_len) {
        return 0;
    }
    unify_khmem(cur_pos, 0);
    cur_pos += cur_len;
    cur_len = bunsetu_len(cur_pos);
    init_kouho(0, 1);
    show_selection(candidate, suffix);
    return kh_count(hchar[cur_pos].candidates);
}

static int change_form(ask_arguments args)
{
    char *candidate;
    char *suffix;
    Sint32 command;
    int form;
    if (func_mode != ASK_SESSION_SELECTING) {
        return -1;
    }
    command = COMMAND(args);
    candidate = FIELD(args, ask_string_pair, first, char *, 0);
    suffix = FIELD(args, ask_string_pair, second, char *, 1);
    switch (command) {
    case ASK_FIRST_KATAKANA:
        form = ASK_TEXT_KATAKANA;
        break;
    case ASK_FIRST_HIRAGANA:
        form = ASK_TEXT_HIRAGANA;
        break;
    case ASK_FIRST_HALF_WIDTH:
        form = ASK_TEXT_HALF_WIDTH;
        break;
    case ASK_FIRST_FULL_WIDTH:
        form = ASK_TEXT_FULL_WIDTH;
        break;
#ifdef NONMATCHING
    /* handlers sends commands 41 to 44 here too, which no case gives a form.
     * no apiask_ function sends them */
    default:
        return -1;
#endif
    }
    khmem_raw(form);
    init_kouho(0, 1);
    show_selection(candidate, suffix);
    return 0;
}

static int unsupported(ask_arguments args)
{
    (void)args;
    return -1;
}
