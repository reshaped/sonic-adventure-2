#include <apinecnf.h>
#include <apiask.h>
#include <string.h>
#include "ask_session.h"

/* how show_candidate shows a candidate. candidate_action picks one by the
 * candidate's place, from 1 to candidate_count. the last two places hold the
 * reading in hiragana and in katakana, and the dictionary's candidates come
 * first */
enum candidate_action {
    SHOW_FIRST = 0,
    SHOW_HIRAGANA = 1,
    SHOW_KATAKANA = 2,
    SHOW_LAST = 3,
    STEP_BACK = 6,
    STEP_FORWARD = 7
};
#define READING_SIZE 41
/* the last places, which hold the reading in hiragana and in katakana */
#define READING_PLACES 2
static int candidate_count;
static int selected; /* the place of the candidate shown, or 0 for none */
static int direction;
/* @hack type: reading, katakana and hiragana as unsigned char, cast to char *
 * at each use, where char would do */
static unsigned char reading[READING_SIZE];
static unsigned char katakana[READING_SIZE];
static unsigned char hiragana[READING_SIZE];

#pragma section ATA
static char version[44];
char *ntgAskBuild = version;
static char main_name[8] = "Dummy";
static char user_name[4] = "";
static char version[44] = "\nntAsk Ver 1.08 Build:Nov 14 2000 10:38:18\n";
#pragma section

static int begins_hiragana(char *candidate)
{
    int length = strlen(candidate);
    int same = 0;
    if ((size_t)length <= strlen((char *)hiragana) && length > 0) {
        if (memcmp(hiragana, candidate, length) == 0) {
            same = 1;
        }
    }
    return same;
}

static int first_candidate(char *candidate, char *suffix)
{
    int result = apiask_37_FirstHenkanToKata(candidate, suffix);
    if (result >= 0) {
        direction = 0;
        result = apiask_21_NextKouho(candidate, suffix);
        if (begins_hiragana(candidate) == 1) {
            result = apiask_21_NextKouho(candidate, suffix);
            direction = 1;
        }
    }
    return result;
}

static int last_candidate(char *candidate, char *suffix)
{
    int result = apiask_37_FirstHenkanToKata(candidate, suffix);
    if (result >= 0) {
        direction = 1;
        result = apiask_20_PrevKouho(candidate, suffix);
        if (begins_hiragana(candidate) == 1) {
            result = apiask_20_PrevKouho(candidate, suffix);
            direction = 0;
        }
    }
    return result;
}

static int candidate_action(int place, int forward)
{
    int step = forward + STEP_BACK;
    int action;
    /* @hack spelling: a do-while that runs once and a while that jumps to
     * dictionary_choices, where an if and else on candidate_count would do */
    do {
        while (candidate_count > READING_PLACES) {
            goto dictionary_choices;
        }
        action = SHOW_KATAKANA;
        if (place == 1) {
            action = SHOW_HIRAGANA;
            break;
        }
        break;
    dictionary_choices:
        if (place == 1) {
            action = SHOW_FIRST;
            break;
        } else if (place == candidate_count) {
            action = SHOW_KATAKANA;
            break;
        } else if (place == candidate_count - 1) {
            action = SHOW_HIRAGANA;
            break;
        } else if (place == candidate_count - READING_PLACES) {
            action = SHOW_LAST;
            break;
        } else {
            action = step;
        }
    } while (0);
    return action;
}

static int show_candidate(int action, char *candidate, char *suffix)
{
    int result;
    switch (action) {
    case SHOW_FIRST:
        result = first_candidate(candidate, suffix);
        break;
    case SHOW_HIRAGANA:
        strcpy(candidate, (char *)hiragana);
        *suffix = 0;
        result = 1;
        break;
    case SHOW_KATAKANA:
        strcpy(candidate, (char *)katakana);
        *suffix = 0;
        result = 1;
        break;
    case SHOW_LAST:
        result = last_candidate(candidate, suffix);
        break;
    case STEP_BACK:
        result = apiask_20_PrevKouho(candidate, suffix);
        if (begins_hiragana(candidate) == 1) {
            direction = 0;
            result = apiask_20_PrevKouho(candidate, suffix);
        }
        break;
    case STEP_FORWARD:
        result = apiask_21_NextKouho(candidate, suffix);
        if (begins_hiragana(candidate) == 1) {
            direction = 1;
            result = apiask_21_NextKouho(candidate, suffix);
        }
        break;
#ifdef NONMATCHING
    /* candidate_action gives no other action */
    default:
        return -1;
#endif
    }
    return result;
}

int comkan_init(void)
{
    int result;
    func_mode = ASK_SESSION_CLOSED;
    lock_mode = 1;
    candidate_count = 0;
    selected = 0;
    result = apiask_32_SetDicName(main_name, user_name);
    if (!result) {
        result = apiask_28_OpenDic();
    }
    if (result > 0) {
        result = -1;
    }
    return result;
}

int comkan_henkan(unsigned char *output, int capacity, unsigned char *input,
                  int input_length, int backward)
{
    unsigned char candidate[READING_SIZE];
    unsigned char suffix[READING_SIZE];
    int result = -1;
    int count;
    int steps;
    int best_step;
    int best_count;
    int action;
    int place;
    int length;
#ifdef NONMATCHING
    /* this side refuses an output with no room, and empties the output for a
     * missing input or one too long for reading */
    count = 0;
    if (capacity <= 0 || !output) {
        return -1;
    }
    if (!input || input_length < 0 || input_length >= READING_SIZE) {
        output[0] = 0;
        candidate_count = selected = 0;
        return -1;
    }
#endif
    if (input_length < READING_SIZE) {
#ifdef NONMATCHING
        /* this side copies input_length bytes, since input needn't end in a
         * NUL */
        memcpy(reading, input, input_length);
        reading[input_length] = 0;
#else
        strcpy((char *)reading, (char *)input);
#endif
        candidate[0] = 0;
        suffix[0] = 0;
        result = apiask_19_Henkan((char *)reading, (char *)candidate,
                                  (char *)suffix);
        if (result > 0) {
            count = result;
            if (*suffix != 0) {
                best_step = 0;
                steps = 0;
                while (*suffix != 0 && result >= 0) {
                    result = apiask_33_LongerKouho((char *)candidate,
                                                   (char *)suffix);
                    steps++;
                    if (result > 1) {
                        best_step = steps;
                        best_count = result;
                    }
                }
                apiask_38_FirstHenkanToHira((char *)candidate, (char *)suffix);
                strcpy((char *)hiragana, (char *)candidate);
                result = apiask_37_FirstHenkanToKata((char *)candidate,
                                                     (char *)suffix);
                strcpy((char *)katakana, (char *)candidate);
                if (best_step > 0) {
                    if (best_step < steps) {
                        for (; best_step != steps; best_step++) {
                            result = apiask_34_ShorterKouho((char *)candidate,
                                                            (char *)suffix);
                        }
                        count = result;
                    } else {
                        count = best_count;
                        result = count;
                    }
                } else {
                    result = apiask_19_Henkan(
                        (char *)reading, (char *)candidate, (char *)suffix);
                }
            } else {
                apiask_38_FirstHenkanToHira((char *)candidate, (char *)suffix);
                strcpy((char *)hiragana, (char *)candidate);
                apiask_37_FirstHenkanToKata((char *)candidate, (char *)suffix);
                strcpy((char *)katakana, (char *)candidate);
                result = count;
            }
        }
    }
    selected = 1;
    candidate_count = 0;
    direction = 0;
    /* @hack spelling: strlen of &katakana[0] and of &reading[0], where
     * katakana and reading would do */
    if (strlen((char *)hiragana) == strlen((char *)katakana) &&
        memcmp(hiragana, katakana, strlen((char *)&katakana[0])) == 0) {
        result = 0;
        if (strlen((char *)hiragana) != strlen((char *)reading) ||
            memcmp(hiragana, reading, strlen((char *)&reading[0])) != 0) {
            strcpy((char *)hiragana, (char *)reading);
        }
    }
    if (result > 0) {
        apiask_37_FirstHenkanToKata((char *)candidate, (char *)suffix);
        place = apiask_20_PrevKouho((char *)candidate, (char *)suffix);
        if (place != ((count - 1) % ASK_MOST_IN_GROUP) + 1) {
            count++;
        }
        if (count == 1) {
            count++;
        }
        candidate_count = count;
        if (backward == 0) {
            action = candidate_action(selected, 1);
        } else {
            selected = candidate_count;
            action = candidate_action(selected, 0);
        }
        /* @hack spelling: &candidate[0] and &suffix[0], where candidate and
         * suffix would do */
        show_candidate(action, (char *)&candidate[0], (char *)&suffix[0]);
        length = strlen((char *)candidate) + strlen((char *)suffix);
        result = -1;
#ifdef NONMATCHING
        /* this side repairs the bug tagged below: the candidate needs capacity
         * for its NUL */
        if (length < capacity) {
#else
        /* @bug memory: writes the candidate's NUL past output when its length
         * is capacity, at 0x0c9e01ee. comkan_nextkouho and comkan_prevkouho
         * refuse that length */
        if (length <= capacity) {
#endif
            strcpy((char *)output, (char *)candidate);
            if (*suffix != 0) {
                strcat((char *)output, (char *)suffix);
            }
            result = candidate_count;
        }
    }
    if (result <= 0) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged below: the input needs capacity for
         * its NUL. it copies reading, whose NUL the copy above wrote */
        if (result == 0 && capacity > input_length && input_length > 0) {
            strcpy((char *)output, (char *)reading);
#else
        /* @bug memory: writes the NUL of an input capacity bytes long past
         * output, at 0x0c9e0218. the test admits an input_length equal to
         * capacity */
        if (result == 0 && capacity >= input_length && input_length > 0) {
            strcpy((char *)output, (char *)input);
#endif
            result = 1;
            selected = 1;
            candidate_count = 1;
        } else {
            output[0] = 0;
            selected = 0;
            result = -1;
        }
    }
    return result;
}

int comkan_nextkouho(unsigned char *output, int capacity)
{
    char candidate[READING_SIZE];
    char suffix[READING_SIZE];
    int result = -1;
    int action;
    int length;
    if (selected > 0 && candidate_count > 0) {
        selected++;
        if (selected > candidate_count) {
            selected = 1;
        }
        action = candidate_action(selected, 1);
        result = show_candidate(action, candidate, suffix);
    }
    if (result > 0) {
        length = strlen(candidate) + strlen(suffix);
        if (length >= capacity) {
            result = -1;
        } else {
            strcpy((char *)output, candidate);
            if (suffix[0]) {
                strcat((char *)output, suffix);
            }
            result = selected;
        }
    }
    return result;
}

int comkan_prevkouho(unsigned char *output, int capacity)
{
    char candidate[READING_SIZE];
    char suffix[READING_SIZE];
    int result = -1;
    int action;
    int length;
    if (selected > 0 && candidate_count > 0) {
        selected--;
        if (selected < 1) {
            selected = candidate_count;
        }
        action = candidate_action(selected, 0);
        result = show_candidate(action, candidate, suffix);
    }
    if (result > 0) {
        length = strlen(candidate) + strlen(suffix);
        if (length >= capacity) {
            result = -1;
        } else {
            strcpy((char *)output, candidate);
            if (suffix[0]) {
                strcat((char *)output, suffix);
            }
            result = selected;
        }
    }
    return result;
}
