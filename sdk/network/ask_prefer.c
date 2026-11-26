#include "ask_phrases.h"
#include "ask_cells.h"
#include "ask_dictionary.h"
#include <sg_xpt.h>

/* score_ending's word_form where the call gives none */
#define NO_FORM 0xff
/* what kh_priority adds to a rank the dictionary gives */
#define DICTIONARY_RANK 1000

static int score_phrase(struct ask_phrase *phrase, int first, int end);
static void score_followers(struct ask_phrase *phrase, int first, int end);
static int ignore_word(struct ask_phrase *phrase);
static int join_score(struct ask_phrase *phrase, struct ask_phrase *follower);
static int score_conjugation(Uint8 kind, Uint8 form, Uint8 next, int score);
static int score_word_pair(Uint8 kind, Uint8 next, int score);
static int score_particle(struct ask_phrase *phrase, Uint8 kind, Uint8 next,
                          int score);
static int score_ending(Uint8 ending_form, Uint8 kind, Uint8 word_form,
                        int score, int extra);

int bs_prefer(int first, int end, int length)
{
    struct ask_position *position;
    /* @hack redundant: phrase declared register, where a plain local would
     * do */
    register struct ask_phrase *phrase;
    struct ask_phrase *best;
    position = &hchar[first];
    for (phrase = position->phrases; phrase; phrase = phrase->next) {
        if (length < 0 || phrase->length == length) {
            if (score_phrase(phrase, first, end) == -1) {
                return -1;
            }
        }
    }
    phrase = position->phrases;
#ifdef NONMATCHING
    /* this side refuses a position with no phrase, which the scan below reads
     * through */
    if (!phrase) {
        return -1;
    }
#endif
    best = phrase;
    while ((phrase = phrase->next) != NULL) {
        if (phrase->score > best->score) {
            best = phrase;
        }
    }
    score_followers(best, first, end);
    return best->length;
}

/* @hack redundant: first, a parameter nothing reads */
static int pair_score(int first, struct ask_phrase *phrase,
                      struct ask_phrase *follower)
{
    int later;
    int earlier;
    int standalone;
    /* @hack spelling: score, a local that holds the sum, where returning the
     * sum would do */
    int score;
    (void)first;
    if (!follower) {
        later = phrase->length;
        earlier = 0;
        standalone = 1;
    } else {
        earlier = phrase->length;
        standalone = 0;
        later = follower->length;
    }
    score = phrase->preference + (earlier * 16) + (later * 17) +
            join_score(phrase, follower) + (standalone * 50);
    return score;
}

static int score_phrase(struct ask_phrase *phrase, int first, int end)
{
    /* @hack redundant: follower declared register, where a plain local would
     * do */
    register struct ask_phrase *follower;
    Uint16 best;
    Uint16 score;
    int length;
    if (ignore_word(phrase)) {
        phrase->score = 0;
        return 0;
    }
    length = phrase->length;
    if (phrase->kind == ASK_STOP_WORD || first + length >= end) {
        best = pair_score(first, phrase, NULL);
    } else {
        follower = hchar[first + length].phrases;
        if (!follower && henkan_mode >= ASK_PARTIAL_READING) {
            return -1;
        }
        for (best = 0; follower; follower = follower->next) {
            score = pair_score(first, phrase, follower);
            if (score > best) {
                best = score;
            }
        }
    }
    phrase->score = best;
    return 0;
}

/* prefer first's phrases of endings alone: those of henkan's empty word, an
 * ASK_PLAIN_WORD word of length 0 */
void bs_prefix(int first)
{
    struct ask_position *position;
    /* @hack redundant: phrase declared register, where a plain local would
     * do */
    register struct ask_phrase *phrase;
    struct ask_match *word;
    position = &hchar[first];
    for (phrase = position->phrases; phrase; phrase = phrase->next) {
        phrase->preference = 0;
        word = phrase->word;
        if (word && word->kind == ASK_PLAIN_WORD && word->length == 0) {
            phrase->preference = 10;
        }
    }
}

static void score_followers(struct ask_phrase *phrase, int first, int end)
{
    struct ask_phrase *follower;
    int score;
    int length;
    length = phrase->length;
    if (phrase->kind == ASK_STOP_WORD || first + length >= end) {
        return;
    }
    /* @hack redundant: the return where hchar holds no follower, which the
     * loop's test covers */
    if (!(follower = hchar[first + length].phrases)) {
        return;
    }
    for (; follower; follower = follower->next) {
        follower->preference = 0;
        if (!ignore_word(follower) &&
            (score = join_score(phrase, follower)) > 0) {
            follower->preference = score;
        }
    }
}

static int ignore_word(struct ask_phrase *phrase)
{
    struct ask_match *word;
    int kind;
    if ((word = phrase->word) != NULL) {
        kind = word->kind;
        if (kind == ASK_RAW_WORD || kind == ASK_SYMBOL_WORD) {
            return 1;
        }
    }
    return 0;
}

static int join_score(struct ask_phrase *phrase, struct ask_phrase *follower)
{
    Uint8 kind;
    Uint8 next;
    Uint8 form;
    Uint8 ending_form;
    Uint8 word_kind;
    Uint8 next_form;
    int score;
    struct ask_match *following_word;
    struct ask_match *word;
    score = 0;
    ending_form = 0;
    form = 0;
    kind = phrase->kind;
    if (phrase->kind >= ASK_FORM_KINDS && phrase->kind <= ASK_LAST_KIND) {
        ending_form = phrase->form;
    } else {
        form = phrase->form;
    }
    if (!follower) {
        next = 0;
    } else if (!(following_word = follower->word)) {
        next = 0;
    } else {
        if ((next = following_word->kind) == ASK_RAW_WORD) {
            return 0;
        }
        if (next == ASK_SYMBOL_WORD ||
            (next == ASK_PLAIN_WORD && following_word->length > 0) ||
            (next == ASK_NUMBER_WORD && following_word->entry == -1)) {
            score = 20;
        }
    }
    word = phrase->word;
    if (word) {
        word_kind = word->kind;
        if (word_kind == ASK_PLAIN_WORD && word->length == 0) {
            score += 10;
        } else if (word_kind <= ASK_PLAIN_WORD ||
                   word_kind >= ASK_NUMBER_WORD) {
            score++;
        }
    }
    if (form || kind == 13) {
        return score_conjugation(kind, form, next, score);
    }
    if (kind == 45) {
        return score_ending(ending_form, next, NO_FORM, score, 0);
    }
    if (next >= 50 && next <= ASK_LAST_KIND) {
        if ((word = follower->word) != NULL) {
            next_form = word->form;
        } else {
            next_form = 0;
        }
        return score_ending(next_form, kind, form, score, 2);
    }
    if (kind >= 20 && kind < 0xc0) {
        return score_word_pair(kind, next, score);
    }
    if (kind >= 0xc0) {
        return score_particle(phrase, kind, next, score);
    }
    return score;
}

static int score_conjugation(Uint8 kind, Uint8 form, Uint8 next, int score)
{
    if (kind == 13 && form == 0) {
        if (next >= 1 && next <= 23) {
            return score + 3;
        }
        return score;
    } else if (form >= 4 && form <= 6 && next >= 1 && next <= 23) {
        if (kind == 0x8d || (kind == 0x8e && form == 6)) {
            return score + 10;
        } else if (kind <= 0x8c) {
            return score + 3;
        }
        return score;
    }
    if (form == 8 && next >= 20 && next <= 24) {
        if ((kind >= 0x80 && kind <= 0x8e) || kind == 0x92 || kind == 0x94 ||
            kind == 0xa0 || kind == 0xa2 || kind == 0xae || kind == 0xaf ||
            kind == 0xb1 || kind == 1 || (kind >= 10 && kind <= 12)) {
            return score + 15;
        }
        return score + 3;
    }
    return score;
}

static int score_word_pair(Uint8 kind, Uint8 next, int score)
{
    if (kind >= 20 && kind <= 24) {
        if (next >= 20 && next <= 24) {
            return score + 3;
        }
        return score;
    }
    if (kind == ASK_NUMBER_WORD && next == ASK_NUMBER_WORD) {
        return score + 15;
    }
    if (kind == 33 || kind == 38 || kind == 39) {
        return score + 20;
    }
    if (kind == 32 || kind == 34) {
        return score + 15;
    }
    if (kind == 27 && next == 28) {
        return score + 20;
    }
    if (kind == 26 && next == 26) {
        return score + 5;
    }
    return score;
}

static int score_particle(struct ask_phrase *phrase, Uint8 kind, Uint8 next,
                          int score)
{
    struct ask_match *word;
    Uint8 word_kind;
    int joined;
    joined = 0;
    if ((word = phrase->word) != NULL) {
        word_kind = word->kind;
        if ((word_kind >= 20 && word_kind <= 25) || word_kind == 50) {
            joined++;
            if (word->length + 1 == phrase->length) {
                joined++;
            }
        }
    }
    if (kind == 0xc7) {
        return next >= 1 && next <= ASK_LAST_CONJUGATING ? score + 30
                                                         : score + 25;
    }
    if (kind == 0xc8) {
        if (next == 0) {
            return score;
        }
        return joined == 2 ? score + 20 : score + 5;
    }
    if (kind == 0xc1 || kind == 0xe1) {
        return joined == 2 ? score + 5 : score;
    }
    if (kind == 0xc4 || kind == 0xc5 || (kind >= 0xce && kind <= 0xd1)) {
        if ((next >= 1 && next <= ASK_LAST_CONJUGATING) ||
            (next >= 22 && next <= 23)) {
            return score + 5;
        }
        return score;
    }
    if (kind == 0xc2) {
        if (next != 0 && joined == 2) {
            score += 3;
        }
        return score;
    }
    if (kind >= 0xfc && kind <= 0xfd && next >= 1 && next <= 23) {
        return score + 18;
    }
    if ((kind == 0xfa || kind == 0xfe) && next >= 20 && next <= 24) {
        return score + 18;
    }
    return score;
}

static int score_ending(Uint8 ending_form, Uint8 kind, Uint8 word_form,
                        int score, int extra)
{
    int bonus = 0;
    switch (ending_form) {
    case 0:
        if ((kind >= 20 && kind <= 25) || kind == 50) {
            bonus = 15;
        }
        break;
    case 1:
        if (kind == 26) {
            bonus = 20;
        }
        break;
    case 2:
        if (kind == 27 || kind == 28) {
            bonus = 15;
        }
        break;
    case 3:
        if (kind == ASK_NUMBER_WORD || kind == 56) {
            bonus = 20;
        }
        break;
    case 4:
        if (kind == 22) {
            bonus = 20;
        }
        break;
    case 5:
        if (word_form == NO_FORM) {
            if (kind >= 1 && kind <= ASK_LAST_CONJUGATING) {
                bonus = 15;
            }
        } else if ((kind >= 0x80 && kind <= 0x8b && word_form == 4) ||
                   (kind == 13 && word_form == 0)) {
            bonus = 15;
        }
        break;
    case 6:
        if (kind >= 20 && kind <= 25) {
            bonus = 15;
        } else if (kind == ASK_NUMBER_WORD || kind == 56) {
            bonus = 20;
        }
        break;
    case 7:
        if (kind == 29) {
            bonus = 15;
        }
        break;
    case 8:
        if ((kind >= 20 && kind <= 25) ||
            (kind >= 1 && kind <= ASK_LAST_CONJUGATING)) {
            bonus = 20;
        }
        break;
    }
    if (!bonus) {
        return score;
    }
    return extra + score + bonus;
}

Uint16 kh_priority(struct ask_phrase *phrase, Uint16 rank)
{
    /* @hack spelling: priority, a local that holds the result, where
     * returning each value would do */
    Uint16 priority;
    if (rank) {
        priority = rank + DICTIONARY_RANK;
    } else {
        priority = phrase->score;
    }
    return priority;
}
