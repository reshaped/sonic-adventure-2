/* A conversion steps through each phrase of its reading and back, past the
 * eight phrases the image's history holds. Each phrase it steps back to
 * shows the candidate chosen there. Where the library refuses a conversion,
 * the steps stop at history's end. */
#include <assert.h>
#include <string.h>

#include "chaostgkinder/composition.h"
/* romaji.h, which names the key commands, spells its tables' kana by
 * text.inc's names */
#include "chaostgkinder/composition/text.inc"
#include "chaostgkinder/romaji.h"
#include "chaostgkinder/widget.h"
#include "system/task.h"

/* the reading's phrases, more than the image's history holds */
#define PHRASES 12
/* the candidates of each phrase. candidate n is n letters long */
#define CANDIDATES 3
/* more steps than the port's history has room for: CODE_CAPACITY, 160 */
#define REFUSED_STEPS 200

/* the code the soft keyboard gives each pad key */
static Uint16 typed;
/* the phrase the library shows. chosen holds each phrase's candidate, from
 * 1, or 0 for its reading in katakana */
static int shown, chosen[PHRASES];
/* refusing makes the library refuse each call. steps counts the calls of
 * apiask_36_NextBunsetu */
static int refusing, steps;

int comkan_init(void)
{
    return 0;
}

void kinder_keyboard_start(void)
{
}

void kinder_keyboard_stop(void)
{
}

void kinder_keyboard_open(void)
{
}

void kinder_keyboard_close(void)
{
}

void kinder_keyboard_draw(void)
{
}

Uint16 kinder_keyboard_input(Uint16 key)
{
    return typed;
}

Uint8 kinder_key_modifiers(void)
{
    return 0;
}

struct task *task_create(Uint16 flags, int level,
                         void (*step)(struct task *task), const char *name)
{
    static struct task sentinel;
    return &sentinel;
}

void task_kill(struct task *task)
{
}

/* write the shown phrase's candidate, and a reading after it unless the
 * phrase is the last */
static void show_phrase(char *candidate, char *suffix)
{
    memset(candidate, 'a', chosen[shown]);
    candidate[chosen[shown]] = '\0';
    strcpy(suffix, shown < PHRASES - 1 ? "b" : "");
}

int apiask_19_Henkan(char *reading, char *candidate, char *suffix)
{
    if (refusing) {
        return -1;
    }
    shown = 0;
    show_phrase(candidate, suffix);
    return 0;
}

int apiask_21_NextKouho(char *candidate, char *suffix)
{
    if (refusing) {
        return 0;
    }
    chosen[shown] = chosen[shown] % CANDIDATES + 1;
    show_phrase(candidate, suffix);
    return chosen[shown];
}

int apiask_35_PrevBunsetu(char *candidate, char *suffix)
{
    assert(!refusing && shown > 0);
    shown--;
    show_phrase(candidate, suffix);
    return 0;
}

int apiask_36_NextBunsetu(char *candidate, char *suffix)
{
    steps++;
    if (refusing) {
        return -1;
    }
    assert(shown < PHRASES - 1);
    shown++;
    show_phrase(candidate, suffix);
    return 0;
}

int apiask_37_FirstHenkanToKata(char *candidate, char *suffix)
{
    if (refusing) {
        return -1;
    }
    chosen[shown] = 0;
    show_phrase(candidate, suffix);
    return 0;
}

/* the calls no key the test types makes */
int apiask_20_PrevKouho(char *candidate, char *suffix)
{
    assert(0);
    return 0;
}

int apiask_33_LongerKouho(char *candidate, char *suffix)
{
    assert(0);
    return 0;
}

int apiask_34_ShorterKouho(char *candidate, char *suffix)
{
    assert(0);
    return 0;
}

int apiask_38_FirstHenkanToHira(char *candidate, char *suffix)
{
    assert(0);
    return 0;
}

static void type(Uint16 code)
{
    typed = code;
    kinder_composition_input(WIDGET_PAD_A);
}

/* the letters of the candidates chosen before phrase */
static int letters_before(int phrase)
{
    int letters = 0;
    while (phrase-- > 0) {
        letters += phrase % CANDIDATES + 1;
    }
    return letters;
}

int main(void)
{
    int phrase;
    int i;

    kinder_kana_start();
    kinder_composition_start();
    kinder_composition_use_pad();
    type('a');
    type(KEY_CONVERT);
    assert(kinder_composition()->converting);
    /* each phrase takes a candidate of its own, and the last step finds no
     * phrase after it */
    for (phrase = 0; phrase < PHRASES; phrase++) {
        for (i = 0; i < phrase % CANDIDATES; i++) {
            type(KEY_CONVERT);
        }
        assert(shown == phrase);
        assert(chosen[phrase] == phrase % CANDIDATES + 1);
        assert(kinder_composition()->kana_length == letters_before(phrase));
        type(KEY_DOWN);
    }
    assert(steps == PHRASES - 1);
    for (phrase = PHRASES - 1; phrase > 0; phrase--) {
        type(KEY_UP);
        assert(shown == phrase - 1);
        assert(chosen[phrase - 1] == (phrase - 1) % CANDIDATES + 1);
        assert(kinder_composition()->kana_length == letters_before(phrase - 1));
    }
    type(KEY_UP);
    assert(shown == 0);

    /* Enter commits the conversion on its first phrase and leaves suffix
     * holding the reading after it */
    type('\n');
    assert(!kinder_composition()->converting);
    refusing = 1;
    steps = 0;
    type('a');
    type(KEY_CONVERT);
    for (i = 0; i < REFUSED_STEPS; i++) {
        type(KEY_DOWN);
    }
    assert(steps < REFUSED_STEPS);
    return 0;
}
