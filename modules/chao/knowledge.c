#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "chao/body_collision.h"
#include "chao/debug.h"
#include "chao/emotion.h"
#include "chao/knowledge.h"
#include "chao/life.h"
#include "chao/small_toys.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "chao/world.h"
#include "player/player.h"
#include "system/pad.h"
#include "system/task.h"

#define KNOWLEDGE(chao) (&CHAO_WORK(chao)->profile.knowledge)
#define LIKING_LIMIT 100
#define FEAR_LIMIT 200
#define TIME_APART_LIMIT 50000
#define KNOWN_PLAYERS 6
#define KNOWN_CHAO 20

/* the page's groups of rows, and the first row of each. the affection has a
 * row, and so does each character and each other Chao the Chao knows. the
 * rest run up to OTHER_ROWS_END. the cursor stops at LAST_ROW */
enum knowledge_group { AFFECTION_GROUP, PLAYER_GROUP, PEER_GROUP, OTHER_GROUP };
#define AFFECTION_ROW 0
#define PLAYER_ROWS 1
#define PEER_ROWS (PLAYER_ROWS + KNOWN_PLAYERS)
#define OTHER_ROWS (PEER_ROWS + KNOWN_CHAO)
#define OTHER_ROWS_END 147
#define LAST_ROW 145
/* @bug arithmetic: the page's row is the cursor's submode, a signed byte.
 * moving down from row 127 wraps it to -128, so rows 128 to LAST_ROW are
 * never reached (0x0c50fa20) */
#ifdef NONMATCHING
/* this side repairs the bug tagged above. it reads the byte unsigned, so the
 * rows past 127 are reached */
#define PAGE_ROW(cursor) (*(Uint8 *)&(cursor)->submode)
#else
#define PAGE_ROW(cursor) ((cursor)->submode)
#endif

int chao_knowledge_player(void)
{
    int player;
    /* @hack spelling: a case for each of the six characters, where a test of
     * the range would do */
    switch (player_character(0)) {
    case PLAYER_SONIC:
        player = PLAYER_SONIC;
        break;
    case PLAYER_SHADOW:
        player = PLAYER_SHADOW;
        break;
    case PLAYER_TAILS:
        player = PLAYER_TAILS;
        break;
    case PLAYER_EGGMAN:
        player = PLAYER_EGGMAN;
        break;
    case PLAYER_KNUCKLES:
        player = PLAYER_KNUCKLES;
        break;
    case PLAYER_ROUGE:
        player = PLAYER_ROUGE;
        break;
    default:
        player = PLAYER_SONIC;
        break;
    }
    return player;
}

Sint8 chao_player_liking(struct task *chao)
{
    struct chao_knowledge *knowledge = KNOWLEDGE(chao);
    Uint16 player = chao_knowledge_player();
    return knowledge->players[player].affinity;
}

void chao_remember_player(struct task *chao, int delta)
{
    struct chao_knowledge *knowledge = KNOWLEDGE(chao);
    Uint16 player = chao_knowledge_player();
    struct chao_player_relationship *relationship;
    int affinity;
    int fear;
    affinity = knowledge->players[player].affinity;
    affinity += delta;
    if (affinity > LIKING_LIMIT) {
        affinity = LIKING_LIMIT;
    } else if (affinity < -LIKING_LIMIT) {
        affinity = -LIKING_LIMIT;
    }
    relationship = &knowledge->players[player];
    relationship->affinity = affinity;
    if (delta > 0) {
        fear = relationship->fear;
        fear -= chao_tuning.fear_loss *
                (chao_emotion_get(chao, CHAO_AGGRESSION) + CHAO_TRAIT_SCALE) /
                CHAO_TRAIT_SCALE;
        fear -= chao_tuning.fear_loss *
                (chao_emotion_get(chao, CHAO_CALM) + CHAO_TRAIT_SCALE) /
                CHAO_TRAIT_SCALE;
        if (fear < 0) {
            fear = 0;
        } else if (fear > FEAR_LIMIT) {
            fear = FEAR_LIMIT;
        }
        relationship->fear = fear;
    }
    chao_meet_player(chao);
}

Uint8 chao_player_fear(struct task *chao)
{
    struct chao_knowledge *knowledge = KNOWLEDGE(chao);
    Uint16 player = chao_knowledge_player();
    return knowledge->players[player].fear;
}

void chao_fear_player(struct task *chao, int delta)
{
    struct chao_knowledge *knowledge = KNOWLEDGE(chao);
    Uint16 player = chao_knowledge_player();
    struct chao_player_relationship *relationship = &knowledge->players[player];
    int fear = relationship->fear;
    fear += delta *
            (CHAO_TRAIT_SCALE - chao_emotion_get(chao, CHAO_AGGRESSION)) /
            CHAO_TRAIT_SCALE;
    fear += delta * (CHAO_TRAIT_SCALE - chao_emotion_get(chao, CHAO_CALM)) /
            CHAO_TRAIT_SCALE;
    if (fear < 0) {
        fear = 0;
    } else if (fear > FEAR_LIMIT) {
        fear = FEAR_LIMIT;
    }
    relationship->fear = fear;
}

Uint16 chao_time_apart(struct task *chao)
{
    struct chao_knowledge *knowledge = KNOWLEDGE(chao);
    Uint16 player = chao_knowledge_player();
    return knowledge->players[player].time_apart;
}

Uint16 chao_meetings(struct task *chao)
{
    struct chao_knowledge *knowledge = KNOWLEDGE(chao);
    Uint16 player = chao_knowledge_player();
    return knowledge->players[player].meetings;
}

void chao_meet_player(struct task *chao)
{
    struct chao_knowledge *knowledge = KNOWLEDGE(chao);
    Uint16 player = chao_knowledge_player();
    struct chao_player_relationship *relationship = &knowledge->players[player];
    if (relationship->meetings < 0xffff) {
        ++relationship->meetings;
    }
    relationship->time_apart /= 3;
    relationship->time_apart *= 2;
}

void chao_age_memories(struct task *chao)
{
    struct chao_knowledge *knowledge = KNOWLEDGE(chao);
    int i;
    if (knowledge->timer++ >= chao_tuning.memory_interval) {
        knowledge->timer = 0;
        for (i = 0; i < KNOWN_PLAYERS; i++) {
            knowledge->players[i].time_apart += chao_tuning.time_apart_step;
            if (knowledge->players[i].time_apart > TIME_APART_LIMIT) {
                knowledge->players[i].time_apart = TIME_APART_LIMIT;
            }
        }
        for (i = 0; i < KNOWN_CHAO; i++) {
            knowledge->peers[i].time_apart += chao_tuning.time_apart_step;
            if (knowledge->peers[i].time_apart > TIME_APART_LIMIT) {
                knowledge->peers[i].time_apart = TIME_APART_LIMIT;
            }
        }
    }
}

void chao_debug_knowledge(struct task *page)
{
    struct task_work *cursor = page->work;
    struct task *chao = garden_active_object(GARDEN_OBJECT_CHAO, cursor->ang.x);
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_parameters *parameters = &work->profile.parameters;
    Sint32 first_row[4] = {AFFECTION_ROW, PLAYER_ROWS, PEER_ROWS, OTHER_ROWS};
    int group;
    int delta;
    /* @hack redundant: a read of the affection that nothing uses */
    (void)parameters->affection;
    garden_enable_objects();
    if (player_pads[CHAO_DEBUG_PAD]->press & PDD_DGT_TB) {
        garden_disable_objects();
    }
    if (PAGE_ROW(cursor) == AFFECTION_ROW) {
        group = AFFECTION_GROUP;
    } else if (PAGE_ROW(cursor) < PEER_ROWS) {
        group = PLAYER_GROUP;
    } else if (PAGE_ROW(cursor) < OTHER_ROWS) {
        group = PEER_GROUP;
        /* NOLINTNEXTLINE(clang-diagnostic-tautological-*) */
    } else if (PAGE_ROW(cursor) < OTHER_ROWS_END) {
        group = OTHER_GROUP;
    }
#ifdef NONMATCHING
    else {
        /* GCC's row stays from 0 to LAST_ROW, so this branch never runs. it
         * leaves no path to the code below with group unset */
        return;
    }
#endif
    if (pad_press_repeat[CHAO_DEBUG_PAD] & PDD_DGT_KU) {
        if (PAGE_ROW(cursor) > AFFECTION_ROW) {
            PAGE_ROW(cursor)--;
        }
    }
    if (pad_press_repeat[CHAO_DEBUG_PAD] & PDD_DGT_KD) {
        /* NOLINTNEXTLINE(clang-diagnostic-tautological-*) */
        if (PAGE_ROW(cursor) < LAST_ROW) {
            PAGE_ROW(cursor)++;
        }
    }
    if (!(player_pads[CHAO_DEBUG_PAD]->on & PDD_DGT_TA)) {
        if (pad_press_repeat[CHAO_DEBUG_PAD] & PDD_DGT_KL) {
            if (!group) {
                group = OTHER_GROUP;
            } else {
                --group;
            }
            PAGE_ROW(cursor) = first_row[group];
        }
        if (pad_press_repeat[CHAO_DEBUG_PAD] & PDD_DGT_KR) {
            if (group == OTHER_GROUP) {
                group = AFFECTION_GROUP;
            } else {
                ++group;
            }
            PAGE_ROW(cursor) = first_row[group];
        }
    } else {
        if (pad_press_repeat[CHAO_DEBUG_PAD] & PDD_DGT_KL) {
            delta = -1;
        } else if (pad_press_repeat[CHAO_DEBUG_PAD] & PDD_DGT_KR) {
            delta = 1;
        } else {
            delta = 0;
        }
        switch (group) {
        case AFFECTION_GROUP:
            chao_player_affection(chao, delta, 0.0F);
            break;
        case PLAYER_GROUP:
            chao_remember_player(chao, delta);
            break;
        case PEER_GROUP:
        case OTHER_GROUP:
            break;
        }
    }
    /* @hack redundant: an empty switch on the group */
    switch (group) {
    default:
        break;
    }
    switch (group) {
    case AFFECTION_GROUP:
        njPrintColor(0xdd66ff66);
        njPrintC(NJM_LOCATION(1, 3), "USER");
        njPrintColor(0xffffffff);
        njPrintD(NJM_LOCATION(9, 3), parameters->affection, 3);
        chao_debug_gauge(14, 3, parameters->affection * 50 / 139, 0xff55ff55,
                         0xff008800);
        break;
    case PLAYER_GROUP:
    case PEER_GROUP:
    case OTHER_GROUP:
        break;
    }
}

#define KNOWLEDGE_BITS(learn, forget, knows, field)                            \
    void learn(struct task *chao, int bit)                                     \
    {                                                                          \
        struct chao_knowledge *knowledge = KNOWLEDGE(chao);                    \
        knowledge->field |= 1 << bit;                                          \
    }                                                                          \
    void forget(struct task *chao)                                             \
    {                                                                          \
        struct chao_knowledge *knowledge = KNOWLEDGE(chao);                    \
        knowledge->field = 0;                                                  \
    }                                                                          \
    Bool knows(struct task *chao, int bit)                                     \
    {                                                                          \
        struct chao_knowledge *knowledge = KNOWLEDGE(chao);                    \
        if (knowledge->field & (1 << bit)) {                                   \
            return TRUE;                                                       \
        }                                                                      \
        return FALSE;                                                          \
    }

KNOWLEDGE_BITS(chao_learn_art, chao_forget_art, chao_knows_art, art)
KNOWLEDGE_BITS(chao_learn_dance, chao_forget_dances, chao_knows_dance, dance)
KNOWLEDGE_BITS(chao_learn_song, chao_forget_songs, chao_knows_song, song)
KNOWLEDGE_BITS(chao_learn_instrument, chao_forget_instruments,
               chao_knows_instrument, instruments)
KNOWLEDGE_BITS(chao_add_small_toy, chao_clear_small_toys, chao_has_small_toy,
               small_toys)
KNOWLEDGE_BITS(fun_0c50fd60, fun_0c50fd74, fun_0c50fd82, unk_06)
