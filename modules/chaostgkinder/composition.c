#include <apiask.h>
#include <apinecnf.h>
#include <sg_xpt.h>
#include <string.h>

#include "chaostgkinder/composition.h"
#include "chaostgkinder/composition/text.inc"
#include "chaostgkinder/kana.h"
#include "chaostgkinder/keyboard.h"
#include "chaostgkinder/romaji.h"
#include "chaostgkinder/wide_text.h"
#include "chaostgkinder/widget.h"
#include "system/task.h"

#define CODE_CAPACITY 160
#define KEY_ESCAPE 0x1b
#define KEY_DELETE 0x7f
enum kana_layout { HIRAGANA_LAYOUT = 0, KATAKANA_LAYOUT = 3 };
#define HIRAGANA_N 0x82f1
#define KATAKANA_N 0x8393
#define HIRAGANA_SMALL_TSU 0x82c1
#define KATAKANA_SMALL_TSU 0x8362
/* the phrases a conversion remembers its choices for */
#ifdef NONMATCHING
/* this side remembers a phrase for each code of kana, which holds the reading
 * a conversion takes. the conversion library's phrases hold at least one code
 * each, so every phrase of a conversion fits in history */
#define HISTORY_PHRASES CODE_CAPACITY
#else
#define HISTORY_PHRASES 8
#endif
/* the task level the sentinel runs at */
#define SENTINEL_LEVEL 2

struct conversion_step {
    Sint8 candidate, length;
};

static Bool use_romaji = TRUE;
static enum kana_layout layout = HIRAGANA_LAYOUT;
static Uint16 text[CODE_CAPACITY], kana[CODE_CAPACITY], roman[CODE_CAPACITY];
static Uint16 remaining_kana[CODE_CAPACITY], remaining_text[CODE_CAPACITY];
static struct kinder_composition state;
static Bool pad_mode;
static Sint32 limit;
static char reading[CODE_CAPACITY * 2], candidate[CODE_CAPACITY * 2];
static char suffix[CODE_CAPACITY * 2];
static struct conversion_step history[HISTORY_PHRASES];
static Sint32 phrase;
static Sint32 selection;
static struct task *composition_task;

static Uint16 keyboard_code(Uint16 key)
{
    Uint8 modifiers = key >> 8;
    Uint8 usage = key;
    Uint16 code = 0;

    if (usage < sizeof(plain_codes) / sizeof(plain_codes[0])) {
        /* @bug logic: a shifted key types its plain code. the shift test at
         * 0x0c9d97b8 never holds: kinder_composition_input, at 0x0c9da778,
         * passes only keys below WIDGET_PAD_UP, whose modifier byte is 0 */
        if (modifiers & KBD_SHIFT_KEYS) {
            code = shifted_codes[usage];
        } else {
            code = plain_codes[usage];
        }
    }
    return code;
}

static Uint16 input_code(Uint16 key)
{
    Uint16 code = 0;
    if (use_romaji || layout == HIRAGANA_LAYOUT) {
        code = keyboard_code(key);
    }
    return code;
}

#define APPEND_CODES(dst, src, count)                                          \
    do {                                                                       \
        memcpy(&(dst)[state.dst##_length], src, (count) * sizeof(*(dst)));     \
        state.dst##_length += (count);                                         \
    } while (0)

static void compose_romaji(int flush)
{
    const struct roman_syllable *syllable;
    const char *spelling;
    const char *writing;
    int length;
    int start;
    int i;
    Uint16 preceding;
    Uint16 pending;

    for (syllable = syllables; syllable->roman; syllable++) {
        spelling = syllable->roman;
        writing = syllable->hiragana;
        if (layout == KATAKANA_LAYOUT && syllable->katakana) {
            writing = syllable->katakana;
        }
        length = strlen(spelling);
        start = state.roman_length - length;
        if (start < 0) {
            continue;
        }
        for (i = 0; i < length; i++) {
            if (roman[start + i] != spelling[i]) {
                goto next_syllable;
            }
        }
        if (start > 0) {
            preceding = roman[start - 1];
            /* @hack spelling: a store of each script's code, where one store
             * of the code chosen would do */
            if (preceding == 'n') {
                if (layout == KATAKANA_LAYOUT) {
                    kana[state.kana_length++] = KATAKANA_N;
                } else {
                    kana[state.kana_length++] = HIRAGANA_N;
                }
                start--;
            } else if (preceding == spelling[0]) {
                if (layout == KATAKANA_LAYOUT) {
                    kana[state.kana_length++] = KATAKANA_SMALL_TSU;
                } else {
                    kana[state.kana_length++] = HIRAGANA_SMALL_TSU;
                }
                start--;
            }
        }
        APPEND_CODES(kana, roman, start);
        kinder_text_unpack(&kana[state.kana_length], writing);
        state.kana_length += kinder_wide_length(&kana[state.kana_length]);
        state.roman_length = 0;
        return;
    next_syllable:;
    }

    if (flush) {
        /* @hack spelling: the last letter in pending, inside the length's
         * test, where one test of both would do */
        if (state.roman_length > 0) {
            pending = roman[state.roman_length - 1];
            if (pending == 'n') {
                if (layout == KATAKANA_LAYOUT) {
                    kana[state.kana_length++] = KATAKANA_N;
                    state.roman_length--;
                } else {
                    kana[state.kana_length++] = HIRAGANA_N;
                    state.roman_length--;
                }
            }
        }
        if (state.roman_length > 0) {
            APPEND_CODES(kana, roman, state.roman_length);
        }
        state.roman_length = 0;
    }
}

static void show_candidate(void)
{
    kinder_text_unpack(roman, candidate);
    state.roman_length = kinder_wide_length(roman);
    kinder_text_unpack(remaining_kana, suffix);
    state.remaining_kana_length = kinder_wide_length(remaining_kana);
}

static void begin_conversion(void)
{
    APPEND_CODES(kana, roman, state.roman_length);
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side: the conversion
     * takes remaining_kana */
    APPEND_CODES(kana, remaining_kana, state.remaining_kana_length);
#else
    /* @bug logic: a conversion takes the reading after the cursor from
     * suffix, not remaining_kana. suffix holds the last conversion's Shift
     * JIS bytes, not codes. begin_conversion copies it at 0x0c9d9a1c */
    APPEND_CODES(kana, suffix, state.remaining_kana_length);
#endif
    kana[state.kana_length] = 0;
    kinder_text_pack(kana, reading);
    apiask_19_Henkan(reading, candidate, suffix);
    apiask_37_FirstHenkanToKata(candidate, suffix);
    selection = apiask_21_NextKouho(candidate, suffix);
    state.kana_length = 0;
    phrase = 0;
}

enum kana_candidate { AS_KATAKANA = -2, AS_HIRAGANA = -1, FIRST_CANDIDATE = 1 };

static void select_hiragana(void)
{
    apiask_38_FirstHenkanToHira(candidate, suffix);
    selection = AS_HIRAGANA;
    show_candidate();
}

static void select_katakana(void)
{
    apiask_37_FirstHenkanToKata(candidate, suffix);
    selection = AS_KATAKANA;
    show_candidate();
}

static void next_phrase(void)
{
    if (suffix[0]) {
        /* @bug memory: a conversion past its eighth phrase writes past
         * history, into phrase, at 0x0c9d9b2a and 0x0c9d9b38. nothing bounds
         * phrase, and a reading can hold more phrases than history */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: next_phrase does nothing
         * once history is full. no conversion's phrases fill it. a
         * conversion the library refuses can leave an earlier one's suffix,
         * which then never empties */
        if (phrase >= HISTORY_PHRASES) {
            return;
        }
#endif
        history[phrase].candidate = selection;
        history[phrase].length = state.roman_length;
        phrase++;
        APPEND_CODES(kana, roman, state.roman_length);
        state.roman_length = 0;
        apiask_36_NextBunsetu(candidate, suffix);
        apiask_37_FirstHenkanToKata(candidate, suffix);
        selection = apiask_21_NextKouho(candidate, suffix);
        show_candidate();
    }
}

static void previous_phrase(void)
{
    if (phrase > 0) {
        phrase--;
        apiask_35_PrevBunsetu(candidate, suffix);
        apiask_37_FirstHenkanToKata(candidate, suffix);
        selection = apiask_21_NextKouho(candidate, suffix);
        selection = history[phrase].candidate;
        switch (selection) {
        case AS_HIRAGANA:
            apiask_38_FirstHenkanToHira(candidate, suffix);
            break;
        case AS_KATAKANA:
            apiask_37_FirstHenkanToKata(candidate, suffix);
            break;
        case FIRST_CANDIDATE:
            break;
        default:
            /* @hack spelling: a do loop with an empty body, where a while
             * loop would do */
            do {
            } while (apiask_21_NextKouho(candidate, suffix) != selection);
            break;
        }
        state.kana_length -= history[phrase].length;
        show_candidate();
    }
}

static void convert_whole_reading(void)
{
    begin_conversion();
    while (apiask_33_LongerKouho(candidate, suffix) > 0) {
    }
}

static void restore_reading(void)
{
    kinder_text_unpack(kana, reading);
    state.kana_length = kinder_wide_length(kana);
    state.roman_length = 0;
    state.remaining_kana_length = 0;
}

#define CLAMP_LENGTH(field, available)                                         \
    do {                                                                       \
        if (state.field > (available)) {                                       \
            state.field = (available);                                         \
        }                                                                      \
    } while (0)

static void terminate_spans(void)
{
    CLAMP_LENGTH(text_length, limit);
    CLAMP_LENGTH(kana_length, limit - state.text_length);
    CLAMP_LENGTH(roman_length, limit - state.text_length - state.kana_length);
    CLAMP_LENGTH(remaining_kana_length, limit - state.text_length -
                                            state.kana_length -
                                            state.roman_length);
    CLAMP_LENGTH(remaining_text_length,
                 limit - state.text_length - state.kana_length -
                     state.roman_length - state.remaining_kana_length);
    text[state.text_length] = 0;
    kana[state.kana_length] = 0;
    roman[state.roman_length] = 0;
    remaining_kana[state.remaining_kana_length] = 0;
    remaining_text[state.remaining_text_length] = 0;
}

#define MOVE_CODES(dst, src)                                                   \
    do {                                                                       \
        APPEND_CODES(dst, src, state.src##_length);                            \
        state.src##_length = 0;                                                \
    } while (0)

static void commit_selection(void)
{
    MOVE_CODES(text, kana);
    MOVE_CODES(text, roman);
    MOVE_CODES(text, remaining_kana);
}

static void flush_letters(void)
{
    MOVE_CODES(kana, roman);
}

static void insert_code(Uint16 code)
{
    if (state.roman_length < limit) {
        if (state.converting) {
            state.converting = FALSE;
            commit_selection();
        }
        roman[state.roman_length++] = code;
    }
}

/* @hack spelling: the pragma that inlines cycle_candidate, where a call
 * would do */
#ifdef NONMATCHING
#else
#pragma inline(cycle_candidate)
#endif
static void cycle_candidate(void)
{
    if (kinder_key_modifiers() & KBD_SHIFT_KEYS) {
        selection = apiask_20_PrevKouho(candidate, suffix);
        show_candidate();
    } else {
        selection = apiask_21_NextKouho(candidate, suffix);
        show_candidate();
    }
}

static void shorten_phrase(void)
{
    selection = apiask_34_ShorterKouho(candidate, suffix);
    show_candidate();
}

static void lengthen_phrase(void)
{
    selection = apiask_33_LongerKouho(candidate, suffix);
    show_candidate();
}

static void toggle_script(void)
{
    if (selection != AS_HIRAGANA) {
        select_hiragana();
    } else {
        select_katakana();
    }
}

static void convert_hiragana(void)
{
    state.converting = TRUE;
    convert_whole_reading();
    select_hiragana();
}

static void convert_katakana(void)
{
    state.converting = TRUE;
    convert_whole_reading();
    select_katakana();
}

#define HAS_READING                                                            \
    (state.kana_length || state.roman_length || state.remaining_kana_length)

#ifdef NONMATCHING
/* this side repairs the two bugs tagged in the matching side. a move counts its
 * codes' bytes, and the right arrow tests remaining_text_length */
#define MOVE_SIZE(count) ((count) * sizeof(Uint16))
#define CAN_MOVE_RIGHT state.remaining_text_length
#else
/* @bug logic: an arrow key moves half the codes of its span. its memmove,
 * at 0x0c9da3fa, 0x0c9da422, 0x0c9da46c and 0x0c9da498, takes a count of
 * codes as bytes */
#define MOVE_SIZE(count) (count)
/* @bug memory: at the text's end, the right arrow takes
 * remaining_text_length to -1, and memmove's count wraps. its test at
 * 0x0c9da474 reads text_length where remaining_text_length is meant */
#define CAN_MOVE_RIGHT state.text_length
#endif

static void edit_code(Uint16 code)
{
    if (state.converting) {
        switch (code) {
        case '\n':
            state.converting = FALSE;
            commit_selection();
            return;
        case KEY_ESCAPE:
            state.converting = FALSE;
            restore_reading();
            return;
        case ' ':
        case KEY_CONVERT:
            cycle_candidate();
            break;
        case KEY_HIRAGANA:
            select_hiragana();
            break;
        case KEY_KATAKANA:
            select_katakana();
            break;
        case KEY_NO_CONVERT:
            toggle_script();
            break;
        case KEY_UP:
            previous_phrase();
            return;
        case KEY_DOWN:
            next_phrase();
            return;
        case KEY_LEFT:
            shorten_phrase();
            break;
        case KEY_RIGHT:
            lengthen_phrase();
            break;
        default:
            state.converting = FALSE;
            commit_selection();
            edit_code(code);
            return;
        }
    } else {
        switch (code) {
        case '\b':
            if (HAS_READING) {
                if (state.roman_length) {
                    state.roman_length--;
                } else if (state.kana_length) {
                    state.kana_length--;
                }
            } else if (state.text_length) {
                state.text_length--;
            }
            break;
        case '\t':
        case '\n':
            if (HAS_READING) {
                commit_selection();
            } else {
                state.result = COMPOSE_ACCEPTED;
                kinder_composition_stop();
            }
            break;
        case KEY_ESCAPE:
            if (HAS_READING) {
                state.kana_length = 0;
                state.roman_length = 0;
                state.remaining_kana_length = 0;
            } else {
                state.result = COMPOSE_CANCELED;
                kinder_composition_stop();
            }
            break;
        case ' ':
        case KEY_CONVERT:
            if (HAS_READING) {
                state.converting = TRUE;
                compose_romaji(TRUE);
                begin_conversion();
                show_candidate();
            } else {
                insert_code(code);
            }
            break;
        case KEY_DELETE:
            if (HAS_READING) {
                if (state.remaining_kana_length) {
                    memmove(remaining_kana, remaining_kana + 1,
                            state.remaining_kana_length *
                                sizeof(*remaining_kana));
                    state.remaining_kana_length--;
                }
            } else if (state.remaining_text_length) {
                memmove(remaining_text, remaining_text + 1,
                        state.remaining_text_length * sizeof(*remaining_text));
                state.remaining_text_length--;
            }
            break;
        case KEY_LEFT:
            if (HAS_READING) {
                flush_letters();
                if (state.kana_length) {
                    memmove(remaining_kana + 1, remaining_kana,
                            MOVE_SIZE(state.remaining_kana_length++));
                    remaining_kana[0] = kana[--state.kana_length];
                }
            } else if (state.text_length) {
                memmove(remaining_text + 1, remaining_text,
                        MOVE_SIZE(state.remaining_text_length++));
                remaining_text[0] = text[--state.text_length];
            }
            break;
        case KEY_RIGHT:
            if (HAS_READING) {
                flush_letters();
                if (state.remaining_kana_length) {
                    kana[state.kana_length++] = remaining_kana[0];
                    memmove(remaining_kana, remaining_kana + 1,
                            MOVE_SIZE(--state.remaining_kana_length));
                }
            } else if (CAN_MOVE_RIGHT) {
                text[state.text_length++] = remaining_text[0];
                memmove(remaining_text, remaining_text + 1,
                        MOVE_SIZE(--state.remaining_text_length));
            }
            break;
        case KEY_DOWN:
            MOVE_CODES(kana, roman);
            MOVE_CODES(kana, remaining_kana);
            break;
        case KEY_UP:
            MOVE_CODES(kana, roman);
            MOVE_CODES(kana, remaining_kana);
            memcpy(remaining_kana, kana, state.kana_length * sizeof(*kana));
            state.remaining_kana_length = state.kana_length;
            state.kana_length = 0;
            break;
        case KEY_F1:
        case KEY_F2:
        case KEY_F3:
        case KEY_F4:
        case KEY_F5:
        case KEY_F8:
        case KEY_F9:
        case KEY_F10:
        case KEY_F11:
        case KEY_F12:
        case KEY_CAPS_LOCK:
        case KEY_SCROLL_LOCK:
        case KEY_UNTYPED:
        case KEY_PAUSE:
        case KEY_PRINT_SCREEN:
        case KEY_INSERT:
        case KEY_HOME:
        case KEY_END:
        case KEY_PAGE_UP:
        case KEY_PAGE_DOWN:
        case KEY_APPLICATION:
        case KEY_KANA:
            break;
        case KEY_HIRAGANA:
        case KEY_NO_CONVERT:
            if (HAS_READING) {
                convert_hiragana();
            }
            break;
        case KEY_KATAKANA:
            if (HAS_READING) {
                convert_katakana();
            }
            break;
        default:
            insert_code(code);
            break;
        }
        if (!state.converting) {
            if (!pad_mode && use_romaji) {
                compose_romaji(FALSE);
            } else {
                flush_letters();
            }
        }
    }
}

static void sentinel_destroy(struct task *task)
{
}

static void sentinel_draw(struct task *task)
{
    if (pad_mode) {
        kinder_keyboard_draw();
    }
}

static void sentinel_step(struct task *task)
{
    if (!task->destructor) {
        task->destructor = sentinel_destroy;
        task->draw = sentinel_draw;
    }
}

void kinder_kana_start(void)
{
    comkan_init();
    kinder_keyboard_start();
    state.text = text;
    state.kana = kana;
    state.roman = roman;
    state.remaining_kana = remaining_kana;
    state.remaining_text = remaining_text;
}

void kinder_kana_stop(void)
{
    MOVE_CODES(text, remaining_text);
    kinder_keyboard_stop();
}

struct kinder_composition *kinder_composition(void)
{
    return &state;
}

void kinder_composition_start(void)
{
    if (!composition_task) {
        composition_task =
            task_create(0, SENTINEL_LEVEL, sentinel_step, "SentinelExec");
        pad_mode = FALSE;
        limit = KINDER_COMPOSITION_LIMIT;
        state.result = COMPOSE_EDITING;
        state.converting = FALSE;
        state.text_length = 0;
        state.kana_length = 0;
        state.roman_length = 0;
        state.remaining_kana_length = 0;
        state.remaining_text_length = 0;
        terminate_spans();
    }
}

void kinder_composition_stop(void)
{
    if (composition_task) {
        task_kill(composition_task);
        composition_task = NULL;
    }
}

void kinder_composition_set_text(const Uint16 *initial)
{
    kinder_wide_terminate(text, initial, limit);
    state.text_length = kinder_wide_length(text);
}

void kinder_composition_use_pad(void)
{
    kinder_keyboard_open();
    pad_mode = TRUE;
}

void kinder_composition_set_limit(Sint32 count)
{
    limit = count;
}

void kinder_composition_input(Uint16 key)
{
    Uint16 code;
    if (key < WIDGET_PAD_UP) {
        if (pad_mode) {
            kinder_keyboard_close();
            pad_mode = FALSE;
        }
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((code = input_code(key)) != 0) {
            edit_code(code);
        }
    } else if (!pad_mode) {
        kinder_keyboard_open();
        pad_mode = TRUE;
    } else {
        /* @hack spelling: the assignment inside the test, as above */
        if ((code = kinder_keyboard_input(key)) != 0) {
            edit_code(code);
        }
    }
    terminate_spans();
}
