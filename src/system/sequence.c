/* sequences: the stages and events the game plays one after another */
#include <sg_xpt.h>

#include "event/mini_event.h"
#include "main.h"
#include "player/player.h"
#ifdef NONMATCHING
/* music_play_deferred, which this side's sequence_play_mini_event calls where
 * the event won't start */
#include "sound/adx.h"
#endif
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/flags.h"
#include "system/sequence.h"
#include "system/util.h"

/* a step's kinds. a STEP_EVENTS step names events, and a STEP_STAGE step a
 * stage and the character to play it. a STEP_ENDING step plays the ending
 * and names nothing. a STEP_TABLE_END step ends each sequence's table */
#define STEP_EVENTS 0
#define STEP_STAGE 1
#define STEP_TABLE_END 2
#define STEP_ENDING 3

#define NO_EVENTS {NO_EVENT, NO_EVENT, NO_EVENT, NO_EVENT}
/* SEQUENCE_TAIKEN's event, the game's first. game_bits marks an event seen
 * at the event's number */
#define FIRST_EVENT 0

struct sequence_step {
    Uint8 kind;
    Uint8 character; /* one of player.h's PLAYER_ characters */
    Uint8 stage;
    Sint16 events[SEQUENCE_EVENTS];
};

/* the mini event that sequence_play_mini_event starts in stage while
 * player_character(0) is character */
struct mini_event_trigger {
    Sint16 stage;
    Sint16 character;
    Sint16 mini_event;
};

/* clang-format off */
/* each sequence's steps, by its SEQUENCE_ name. they are not const, which
 * puts them in section D, where the image holds them */
static struct sequence_step hero_steps[] = {
    {STEP_EVENTS,    0,                 0,                     {FIRST_EVENT, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_SONIC,      STAGE_CITY_ESCAPE,     NO_EVENTS},
    {STEP_STAGE,     PLAYER_SONIC,      STAGE_BOSS_BIGFOOT,    NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                     {2, 3, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_KNUCKLES,   STAGE_WILD_CANYON,     NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                     {4, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_TAILS_MECH, STAGE_VERSUS_MECH_1,   NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                     {5, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_TAILS_MECH, STAGE_PRISON_LANE,     NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                     {6, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_SONIC,      STAGE_METAL_HARBOR,    NO_EVENTS},
    {STEP_STAGE,     PLAYER_SONIC,      STAGE_VERSUS_SONIC_1,  NO_EVENTS},
    {STEP_STAGE,     PLAYER_SONIC,      STAGE_GREEN_FOREST,    NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                     {409, 50, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_KNUCKLES,   STAGE_PUMPKIN_HILL,    NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                     {51, 411, 11, NO_EVENT}},
    {STEP_STAGE,     PLAYER_TAILS_MECH, STAGE_MISSION_STREET,  NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                     {52, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_KNUCKLES,   STAGE_AQUATIC_MINE,    NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                     {14, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_TAILS_MECH, STAGE_CART_1,          NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                     {15, 16, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_TAILS_MECH, STAGE_HIDDEN_BASE,     NO_EVENTS},
    {STEP_STAGE,     PLAYER_SONIC,      STAGE_PYRAMID_CAVE,    NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                     {17, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_KNUCKLES,   STAGE_DEATH_CHAMBER,   NO_EVENTS},
    {STEP_STAGE,     PLAYER_KNUCKLES,   STAGE_BOSS_BIGBOGY,    NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                     {19, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_SONIC,      STAGE_BOSS_GOLEM,      NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                     {420, 21, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_TAILS_MECH, STAGE_ETERNAL_ENGINE,  NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                     {22, 53, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_KNUCKLES,   STAGE_METEOR_HERD,     NO_EVENTS},
    {STEP_STAGE,     PLAYER_KNUCKLES,   STAGE_VERSUS_KNUCKLES, NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                     {24, 54, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_SONIC,      STAGE_CRAZY_GADGET,    NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                     {25, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_TAILS_MECH, STAGE_VERSUS_MECH_2,   NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                     {26, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_SONIC,      STAGE_FINAL_RUSH,      NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                     {27, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_SONIC,      STAGE_VERSUS_SONIC_2,  NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                     {28, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_ENDING,    0,                 0,                     NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                     {429, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_TABLE_END, 0,                 0,                     {0, 0, 0, 0}},
};

static struct sequence_step dark_steps[] = {
    {STEP_EVENTS,    0,                  0,                     {100, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_EGGMAN_MECH, STAGE_IRON_GATE,       NO_EVENTS},
    {STEP_EVENTS,    0,                  0,                     {101, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_SHADOW,      STAGE_BOSS_HOTSHOT,    NO_EVENTS},
    {STEP_EVENTS,    0,                  0,                     {102, 103, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_ROUGE,       STAGE_DRY_LAGOON,      NO_EVENTS},
    {STEP_EVENTS,    0,                  0,                     {150, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_EGGMAN_MECH, STAGE_SAND_OCEAN,      NO_EVENTS},
    {STEP_EVENTS,    0,                  0,                     {105, 106, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_SHADOW,      STAGE_RADICAL_HIGHWAY, NO_EVENTS},
    {STEP_EVENTS,    0,                  0,                     {107, 151, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_ROUGE,       STAGE_EGG_QUARTERS,    NO_EVENTS},
    {STEP_EVENTS,    0,                  0,                     {109, 152, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_EGGMAN_MECH, STAGE_LOST_COLONY,     NO_EVENTS},
    {STEP_EVENTS,    0,                  0,                     {111, 112, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_EGGMAN_MECH, STAGE_WEAPONS_BED,     NO_EVENTS},
    {STEP_EVENTS,    0,                  0,                     {113, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_EGGMAN_MECH, STAGE_VERSUS_MECH_1,   NO_EVENTS},
    {STEP_STAGE,     PLAYER_ROUGE,       STAGE_SECURITY_HALL,   NO_EVENTS},
    {STEP_STAGE,     PLAYER_ROUGE,       STAGE_BOSS_FDOG,       NO_EVENTS},
    {STEP_EVENTS,    0,                  0,                     {116, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_SHADOW,      STAGE_WHITE_JUNGLE,    NO_EVENTS},
    {STEP_STAGE,     PLAYER_SHADOW,      STAGE_VERSUS_SONIC_1,  NO_EVENTS},
    {STEP_EVENTS,    0,                  0,                     {118, 409, 119, NO_EVENT}},
    {STEP_EVENTS,    0,                  0,                     {153, 411, 120, 154}},
    {STEP_STAGE,     PLAYER_ROUGE,       STAGE_CART_1,          NO_EVENTS},
    {STEP_STAGE,     PLAYER_SHADOW,      STAGE_SKY_RAIL,        NO_EVENTS},
    {STEP_EVENTS,    0,                  0,                     {122, 123, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_EGGMAN_MECH, STAGE_BOSS_GOLEM_E,    NO_EVENTS},
    {STEP_EVENTS,    0,                  0,                     {524, 124, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_ROUGE,       STAGE_MAD_SPACE,       NO_EVENTS},
    {STEP_STAGE,     PLAYER_ROUGE,       STAGE_VERSUS_KNUCKLES, NO_EVENTS},
    {STEP_EVENTS,    0,                  0,                     {126, 127, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_EGGMAN_MECH, STAGE_COSMIC_WALL,     NO_EVENTS},
    {STEP_EVENTS,    0,                  0,                     {128, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_EGGMAN_MECH, STAGE_VERSUS_MECH_2,   NO_EVENTS},
    {STEP_EVENTS,    0,                  0,                     {129, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_SHADOW,      STAGE_FINAL_CHASE,     NO_EVENTS},
    {STEP_EVENTS,    0,                  0,                     {130, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_SHADOW,      STAGE_VERSUS_SONIC_2,  NO_EVENTS},
    {STEP_EVENTS,    0,                  0,                     {131, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_ENDING,    0,                  0,                     NO_EVENTS},
    {STEP_EVENTS,    0,                  0,                     {532, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_TABLE_END, 0,                  0,                     {0, 0, 0, 0}},
};

static struct sequence_step last_steps[] = {
    {STEP_EVENTS,    0,                 0,                 {200, 201, 602, 203}},
    {STEP_STAGE,     PLAYER_TAILS_MECH, STAGE_RELAY_TAILS, NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                 {204, 205, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_SHADOW,     STAGE_BOSS_LAST1,  NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                 {206, 207, NO_EVENT, 0}},
    {STEP_STAGE,     PLAYER_SONIC,      STAGE_BOSS_LAST2,  NO_EVENTS},
    {STEP_EVENTS,    0,                 0,                 {208, 210, 211, NO_EVENT}},
    {STEP_TABLE_END, 0,                 0,                 {0, 0, 0, 0}},
};

static struct sequence_step taiken_event_steps[] = {
    {STEP_EVENTS,    0, 0, {400, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_TABLE_END, 0, 0, {0, 0, 0, 0}},
};

static struct sequence_step taiken_steps[] = {
    {STEP_EVENTS,    0,            0,                 {FIRST_EVENT, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_SONIC, STAGE_CITY_ESCAPE, NO_EVENTS},
    {STEP_TABLE_END, 0,            0,                 {0, 0, 0, 0}},
};

static struct sequence_step city_escape_steps[] = {
    {STEP_STAGE,     PLAYER_SONIC, STAGE_CITY_ESCAPE, NO_EVENTS},
    {STEP_EVENTS,    0,            0,                 {532, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_TABLE_END, 0,            0,                 {0, 0, 0, 0}},
};

static struct sequence_step prison_lane_steps[] = {
    {STEP_STAGE,     PLAYER_TAILS_MECH, STAGE_PRISON_LANE, NO_EVENTS},
    {STEP_TABLE_END, 0,                 0,                 {0, 0, 0, 0}},
};

static struct sequence_step wild_canyon_steps[] = {
    {STEP_EVENTS,    0,               0,                 {3, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_KNUCKLES, STAGE_WILD_CANYON, NO_EVENTS},
    {STEP_TABLE_END, 0,               0,                 {0, 0, 0, 0}},
};

static struct sequence_step radical_highway_steps[] = {
    {STEP_STAGE,     PLAYER_SHADOW, STAGE_RADICAL_HIGHWAY, NO_EVENTS},
    {STEP_EVENTS,    0,             0,                     {429, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_TABLE_END, 0,             0,                     {0, 0, 0, 0}},
};

static struct sequence_step sand_ocean_steps[] = {
    {STEP_STAGE,     PLAYER_EGGMAN_MECH, STAGE_SAND_OCEAN, NO_EVENTS},
    {STEP_TABLE_END, 0,                  0,                {0, 0, 0, 0}},
};

static struct sequence_step dry_lagoon_steps[] = {
    {STEP_EVENTS,    0,            0,                {103, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_STAGE,     PLAYER_ROUGE, STAGE_DRY_LAGOON, NO_EVENTS},
    {STEP_TABLE_END, 0,            0,                {0, 0, 0, 0}},
};

static struct sequence_step hero_bosses_steps[] = {
    {STEP_STAGE,     PLAYER_SONIC,      STAGE_BOSS_BIGFOOT,    NO_EVENTS},
    {STEP_STAGE,     PLAYER_TAILS_MECH, STAGE_VERSUS_MECH_1,   NO_EVENTS},
    {STEP_STAGE,     PLAYER_SONIC,      STAGE_VERSUS_SONIC_1,  NO_EVENTS},
    {STEP_STAGE,     PLAYER_KNUCKLES,   STAGE_BOSS_BIGBOGY,    NO_EVENTS},
    {STEP_STAGE,     PLAYER_SONIC,      STAGE_BOSS_GOLEM,      NO_EVENTS},
    {STEP_STAGE,     PLAYER_KNUCKLES,   STAGE_VERSUS_KNUCKLES, NO_EVENTS},
    {STEP_STAGE,     PLAYER_TAILS_MECH, STAGE_VERSUS_MECH_2,   NO_EVENTS},
    {STEP_STAGE,     PLAYER_SONIC,      STAGE_VERSUS_SONIC_2,  NO_EVENTS},
    {STEP_TABLE_END, 0,                 0,                     {0, 0, 0, 0}},
};

static struct sequence_step dark_bosses_steps[] = {
    {STEP_STAGE,     PLAYER_SHADOW,      STAGE_BOSS_HOTSHOT,    NO_EVENTS},
    {STEP_STAGE,     PLAYER_EGGMAN_MECH, STAGE_VERSUS_MECH_1,   NO_EVENTS},
    {STEP_STAGE,     PLAYER_ROUGE,       STAGE_BOSS_FDOG,       NO_EVENTS},
    {STEP_STAGE,     PLAYER_SHADOW,      STAGE_VERSUS_SONIC_1,  NO_EVENTS},
    {STEP_STAGE,     PLAYER_EGGMAN_MECH, STAGE_BOSS_GOLEM_E,    NO_EVENTS},
    {STEP_STAGE,     PLAYER_ROUGE,       STAGE_VERSUS_KNUCKLES, NO_EVENTS},
    {STEP_STAGE,     PLAYER_EGGMAN_MECH, STAGE_VERSUS_MECH_2,   NO_EVENTS},
    {STEP_STAGE,     PLAYER_SHADOW,      STAGE_VERSUS_SONIC_2,  NO_EVENTS},
    {STEP_TABLE_END, 0,                  0,                     {0, 0, 0, 0}},
};

static struct sequence_step all_bosses_steps[] = {
    {STEP_STAGE,     PLAYER_SONIC,       STAGE_BOSS_BIGFOOT,    NO_EVENTS},
    {STEP_STAGE,     PLAYER_TAILS_MECH,  STAGE_VERSUS_MECH_1,   NO_EVENTS},
    {STEP_STAGE,     PLAYER_SONIC,       STAGE_VERSUS_SONIC_1,  NO_EVENTS},
    {STEP_STAGE,     PLAYER_KNUCKLES,    STAGE_BOSS_BIGBOGY,    NO_EVENTS},
    {STEP_STAGE,     PLAYER_SONIC,       STAGE_BOSS_GOLEM,      NO_EVENTS},
    {STEP_STAGE,     PLAYER_KNUCKLES,    STAGE_VERSUS_KNUCKLES, NO_EVENTS},
    {STEP_STAGE,     PLAYER_TAILS_MECH,  STAGE_VERSUS_MECH_2,   NO_EVENTS},
    {STEP_STAGE,     PLAYER_SONIC,       STAGE_VERSUS_SONIC_2,  NO_EVENTS},
    {STEP_STAGE,     PLAYER_SHADOW,      STAGE_BOSS_HOTSHOT,    NO_EVENTS},
    {STEP_STAGE,     PLAYER_EGGMAN_MECH, STAGE_VERSUS_MECH_1,   NO_EVENTS},
    {STEP_STAGE,     PLAYER_ROUGE,       STAGE_BOSS_FDOG,       NO_EVENTS},
    {STEP_STAGE,     PLAYER_SHADOW,      STAGE_VERSUS_SONIC_1,  NO_EVENTS},
    {STEP_STAGE,     PLAYER_EGGMAN_MECH, STAGE_BOSS_GOLEM_E,    NO_EVENTS},
    {STEP_STAGE,     PLAYER_ROUGE,       STAGE_VERSUS_KNUCKLES, NO_EVENTS},
    {STEP_STAGE,     PLAYER_EGGMAN_MECH, STAGE_VERSUS_MECH_2,   NO_EVENTS},
    {STEP_STAGE,     PLAYER_SHADOW,      STAGE_VERSUS_SONIC_2,  NO_EVENTS},
    {STEP_STAGE,     PLAYER_SHADOW,      STAGE_BOSS_LAST1,      NO_EVENTS},
    {STEP_STAGE,     PLAYER_SONIC,       STAGE_BOSS_LAST2,      NO_EVENTS},
    {STEP_TABLE_END, 0,                  0,                     {0, 0, 0, 0}},
};

static struct sequence_step menu_event_steps[] = {
    {STEP_EVENTS,    0, 0, {350, NO_EVENT, NO_EVENT, NO_EVENT}},
    {STEP_TABLE_END, 0, 0, {0, 0, 0, 0}},
};

/* the mini events sequence_play_mini_event can start. leaving out const puts
 * this table in section D too */
static struct mini_event_trigger mini_event_triggers[] = {
    {STAGE_BOSS_BIGFOOT,    PLAYER_SONIC,       1},
    {STAGE_VERSUS_SONIC_1,  PLAYER_SONIC,       7},
    {STAGE_GREEN_FOREST,    PLAYER_SONIC,       8},
    {STAGE_PUMPKIN_HILL,    PLAYER_KNUCKLES,    10},
    {STAGE_AQUATIC_MINE,    PLAYER_KNUCKLES,    13},
    {STAGE_BOSS_BIGBOGY,    PLAYER_KNUCKLES,    18},
    {STAGE_VERSUS_KNUCKLES, PLAYER_KNUCKLES,    23},
    {STAGE_SAND_OCEAN,      PLAYER_EGGMAN_MECH, 104},
    {STAGE_EGG_QUARTERS,    PLAYER_ROUGE,       108},
    {STAGE_LOST_COLONY,     PLAYER_EGGMAN_MECH, 110},
    {STAGE_SECURITY_HALL,   PLAYER_ROUGE,       114},
    {STAGE_BOSS_FDOG,       PLAYER_ROUGE,       115},
    {STAGE_VERSUS_SONIC_1,  PLAYER_SHADOW,      117},
    {STAGE_SKY_RAIL,        PLAYER_SHADOW,      121},
    {STAGE_VERSUS_KNUCKLES, PLAYER_ROUGE,       125},
};
/* clang-format on */
#define TRIGGERS ARRAY_COUNT(mini_event_triggers)

Sint32 sequence_events[SEQUENCE_EVENTS];

static void apply_step(const struct sequence_step *step);

#ifdef NONMATCHING
static void apply_sequence_step(const struct sequence_step *steps, Uint32 count)
{
    Uint32 index = game_flags[FLAG_SEQUENCE_STEP];

    /* a step restored from a save can lie past its table */
    if (index >= count) {
        game_state.mode = GAME_MODE_TO_MENU;
        return;
    }
    apply_step(&steps[index]);
}
#define APPLY_STEPS(steps)                                                     \
    apply_sequence_step(steps, sizeof(steps) / sizeof((steps)[0]))
#else
#define APPLY_STEPS(steps) apply_step(&(steps)[game_flags[FLAG_SEQUENCE_STEP]])
#endif

void sequence_start(int sequence)
{
    game_flags[FLAG_SEQUENCE] = sequence;
    game_flags[FLAG_SEQUENCE_STEP] = 0;
    /* @hack spelling: the copy taken from FLAG_SEQUENCE_STEP just set to 0,
     * where a store of 0 would do */
    game_flags[FLAG_SEQUENCE_STEP_COPY] = game_flags[FLAG_SEQUENCE_STEP];
    game_flags[FLAG_MINI_EVENT_UNPLAYED] = 1;
    if (sequence != SEQUENCE_NONE) {
        game_state.mission = MISSION_FIRST;
    }
    if (sequence == SEQUENCE_TAIKEN && game_bits[FIRST_EVENT] != 0) {
        sequence_advance();
    }
}

void sequence_apply_step(void)
{
    switch (game_flags[FLAG_SEQUENCE]) {
    case SEQUENCE_HERO:
        APPLY_STEPS(hero_steps);
        break;
    case SEQUENCE_DARK:
        APPLY_STEPS(dark_steps);
        break;
    case SEQUENCE_LAST:
        APPLY_STEPS(last_steps);
        break;
    case SEQUENCE_TAIKEN_EVENT:
        APPLY_STEPS(taiken_event_steps);
        break;
    case SEQUENCE_TAIKEN:
        APPLY_STEPS(taiken_steps);
        break;
    case SEQUENCE_CITY_ESCAPE:
        APPLY_STEPS(city_escape_steps);
        break;
    case SEQUENCE_PRISON_LANE:
        APPLY_STEPS(prison_lane_steps);
        break;
    case SEQUENCE_WILD_CANYON:
        APPLY_STEPS(wild_canyon_steps);
        break;
    case SEQUENCE_RADICAL_HIGHWAY:
        APPLY_STEPS(radical_highway_steps);
        break;
    case SEQUENCE_SAND_OCEAN:
        APPLY_STEPS(sand_ocean_steps);
        break;
    case SEQUENCE_DRY_LAGOON:
        APPLY_STEPS(dry_lagoon_steps);
        break;
    case SEQUENCE_HERO_BOSSES:
        APPLY_STEPS(hero_bosses_steps);
        break;
    case SEQUENCE_DARK_BOSSES:
        APPLY_STEPS(dark_bosses_steps);
        break;
    case SEQUENCE_ALL_BOSSES:
        APPLY_STEPS(all_bosses_steps);
        break;
    case SEQUENCE_MENU_EVENT:
        APPLY_STEPS(menu_event_steps);
        /* @hack spelling: no break, so the last case runs out of the
         * switch */
    }
    game_flags[FLAG_SEQUENCE_STEP_COPY] = game_flags[FLAG_SEQUENCE_STEP];
}

void sequence_advance(void)
{
    game_flags[FLAG_SEQUENCE_STEP]++;
    game_flags[FLAG_MINI_EVENT_UNPLAYED] = 1;
}

void sequence_play_mini_event(void)
{
    int stage;
    int character;
    int i;

    if (game_flags[FLAG_MINI_EVENT_UNPLAYED] == 0) {
        return;
    }
    stage = stage_current_id();
    character = player_character(0);
    if (game_flags[FLAG_SEQUENCE] == SEQUENCE_NONE) {
        return;
    }
    for (i = 0; (Uint32)i < TRIGGERS; i++) {
        if (mini_event_triggers[i].stage == stage &&
            mini_event_triggers[i].character == character) {
            mini_event_load(mini_event_triggers[i].mini_event);
#ifdef NONMATCHING
            /* where mini_event_start refuses the event, this side plays the
             * music the event deferred */
            if (mini_event_start() < 0) {
                music_play_deferred();
            }
#else
            mini_event_start();
#endif
            game_flags[FLAG_MINI_EVENT_UNPLAYED] = 0;
            return;
        }
    }
}

Bool sequence_mini_event_due(void)
{
    int stage;
    int character;
    int i;

    /* @hack spelling: the rest of the function inside the test, where an
     * early return would do */
    if (game_flags[FLAG_MINI_EVENT_UNPLAYED] != 0) {
        stage = stage_current_id();
        character = player_character(0);
        if (game_flags[FLAG_SEQUENCE] == SEQUENCE_NONE) {
            return FALSE;
        }
        for (i = 0; (Uint32)i < TRIGGERS; i++) {
            if (mini_event_triggers[i].stage == stage &&
                mini_event_triggers[i].character == character) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

static void apply_step(const struct sequence_step *step)
{
    /* @hack spelling: record, a pointer to game_state, where game_state
     * written directly would do */
    struct game_state *record = &game_state;

    /* @hack spelling: the STEP_ENDING case ahead of the STEP_TABLE_END case,
     * not in the kinds' order */
    switch (step->kind) {
    case STEP_EVENTS:
        /* @hack spelling: the events copied one by one, where a loop over
         * them would do */
        sequence_events[0] = step->events[0];
        sequence_events[1] = step->events[1];
        sequence_events[2] = step->events[2];
        sequence_events[3] = step->events[3];
        record->mode = GAME_MODE_EVENT;
        record->next_mode = GAME_MODE_SEQUENCE;
        break;
    case STEP_STAGE:
        /* a relay stage past the first goes on with the relay */
        switch (step->stage) {
        case STAGE_RELAY_SONIC:
        case STAGE_RELAY_EGGMAN:
        case STAGE_RELAY_ROUGE:
        case STAGE_RELAY_KNUCKLES:
            record->mode = GAME_MODE_STAGE_AGAIN;
            break;
        default:
            record->mode = GAME_MODE_STAGE;
            break;
        }
        record->split_screen = 0;
        record->two_players = 0;
        record->demo = 0;
        stage_set_character(0, step->character);
        stage_set_current(step->stage);
        break;
    case STEP_ENDING:
        /* @hack spelling: the stores joined by &&, which runs the second
         * since GAME_MODE_ENDING isn't 0, where two statements would do */
        (record->mode = GAME_MODE_ENDING) &&
            (record->next_mode = GAME_MODE_SEQUENCE);
        break;
    case STEP_TABLE_END:
        record->mode = GAME_MODE_TO_MENU;
        break;
    }
}
