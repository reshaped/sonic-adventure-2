/* the versus select. its step drives versus_select_draw.h's panel through
 * versus_select_work, which menu_works.work_04 points at */
#include <sg_xpt.h>
#include <shinobi.h>

#ifdef NONMATCHING
/* offsetof, by which this side's SELECTED_GAME_PAIR finds pairs in the work */
#include <stddef.h>
#endif

#include "advertise/menu_snapshot.h"
#include "advertise/menu_textures.h"
#include "advertise/menus.h"
#include "advertise/new_menu_screen.h"
#include "advertise/title_screen.h"
#include "advertise/versus_select.h"
#include "advertise/versus_select_draw.h"
#include "main.h"
#include "player/player.h"
#include "save/save.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "sound/voice_set.h"
#include "stage/stage.h"
#include "system/flags.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/port.h"
#include "system/sequence.h"
#include "system/text.h"

struct versus_select_work versus_select_work;

/* The original overlay exports this unused operation. The port calls it
 * when closing the panel without its arena roster. */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void versus_select_hide(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* versus_select_pad_lost is TRUE once the step has closed the menu for
 * player 1's lost pad. the next start clears it.
 * versus_select_resume_wait counts a resume's wait down, and the wait ends
 * on the step that finds it below 0. */
static Bool versus_select_pad_lost;
static Sint32 versus_select_resume_wait;

/* menu_control.step. each wait below but STEP_STAGE_CHOSEN's and the
 * resumes' lasts until a step of the panel's task moves nothing */
enum {
    STEP_OPEN, /* show the backdrop and the titles */
    STEP_OPENING, /* wait, then show the sides under the titles */
    STEP_TEAMS, /* swap the teams, or choose them */
    STEP_LIST_OPENING, /* wait, then show the help and find the looks open */
    STEP_GAMES, /* move along the list of games, and choose one */
    STEP_CLOSING, /* wait, then go back to the new menu */
    STEP_LIST_CLOSING, /* wait, then show the titles again */
    STEP_GAME_CHOSEN, /* wait, then play the game */
    STEP_STAGES_OPENING, /* wait, then show the help and find the looks open */
    STEP_STAGES, /* move along the list of stages, and choose one */
    STEP_STAGE_CHOSEN, /* wait for the screen fade, then play the stage */
    /* wait out versus_select_resume_wait, then go on at the list of games */
    STEP_RESUME_GAMES,
    /* wait it out, then go on at the list of stages */
    STEP_RESUME_STAGES
};

/* The random game and the list of stages have no character pair. */
#ifdef NONMATCHING
/* this side keeps the pairs the image reads for the random game and the
 * stages game: the work's bytes past pairs. it reads them through a byte
 * pointer into the whole work, where C leaves pairs[game] past the array
 * undefined */
#define SELECTED_GAME_PAIR(work)                                               \
    ((const Sint8 *)(work) + offsetof(struct versus_select_work, pairs) +      \
     ((work)->game * sizeof((work)->pairs[0])))
#else
#define SELECTED_GAME_PAIR(work) ((work)->pairs[(work)->game])
#endif

/* voice_set_play's sets as the versus select opens, and as each of its
 * lists opens */
#define OPEN_VOICES 6
#define GAMES_VOICES 7
#define STAGES_VOICES 3
#define RESUME_STEPS 60
/* the list of games' pairs, by their hero: Sonic and Shadow, Tails and
 * Eggman, Knuckles and Rouge. each pair has PAIR_GAMES random games, and
 * ANY_PAIR picks among every pair's */
#define SONIC_PAIR 0
#define TAILS_PAIR 1
#define KNUCKLES_PAIR 2
#define ANY_PAIR (-1)
#define PAIR_GAMES 3

/* a two-player game: its stage, team 0's character and team 1's, and its
 * game_state.split_screen */
struct versus_game {
    Sint32 stage;
    Sint32 character_0;
    Sint32 character_1;
    Sint32 split_screen;
};

/* the random games, PAIR_GAMES for each pair of the list of games, in the
 * pairs' order */
KEPT static struct versus_game random_games[PAIR_GAMES * VERSUS_SELECT_PAIRS] =
    {
#include "advertise/versus_select/random_games.inc"
};
/* the list of stages' games. versus_select_play_stage reads only their
 * stages and split_screen */
KEPT static struct versus_game stage_games[VERSUS_SELECT_STAGES] = {
#include "advertise/versus_select/stage_games.inc"
};

static void versus_select_play_random(int pair, Bool swapped);
static void versus_select_play_stage(int stage, int character_0,
                                     int character_1);
static void versus_select_set_costumes(void);
static void versus_select_find_looks(const Sint8 *pair);
static void versus_select_close_looks(void);
static void versus_select_clear_looks(void);
static void versus_select_next_look(int team);

/* the buttons of mask that either pad has pressed */
#define EITHER_PRESSED(mask)                                                   \
    /* @hack type: each pad's pad_press_repeat and the mask cast to Sint32,    \
     * where the two uncast would do */                                        \
    (((Sint32)pad_press_repeat[0] & (Sint32)(mask)) |                          \
     ((Sint32)pad_press_repeat[1] & (Sint32)(mask)))

void versus_select_start(void)
{
    struct versus_select_work *work;

    menu_textures_select(MENU_TEXTURES_VERSUS_SELECT);
    menu_control.step = STEP_OPEN;
    music_play_name("advsng_2.adx");
    /* @hack type: the work through menu_works_array, here and at each use
     * below, where menu_works would do */
    menu_works_array->work_04->english_names =
        game_state.text_language > TEXT_JAPANESE;
    /* @hack spelling: work set in the test, where a statement of its own
     * would do */
    if (work = menu_works_array->work_04, save_data.emblems == SAVE_EMBLEMS) {
        work->last_stage_open = TRUE;
    } else {
        work->last_stage_open = FALSE;
    }
    versus_select_pad_lost = FALSE;
}

void versus_select_hide(void)
{
    menu_works_array->work_04->shown = FALSE;
    menu_works_array->work_04->unk_04 = 0;
}

Bool versus_select_step(void)
{
    struct versus_select_work *work;
    /* @hack spelling: the resume's menu kept in a local across
     * menu_snapshot_save, where a store after it would do */
    int resume;

    if (pad_player_is_connected(1) == FALSE &&
        versus_select_pad_lost == FALSE) {
        menu_works_array->work_04->shown = FALSE;
        menu_works_array->work_04->unk_04 = 0;
        menu_control.step = STEP_CLOSING;
        sound_play(MENU_SOUND_BACK, NULL, 0, 0);
        game_state.match_wins[0] = 0;
        game_state.match_wins[1] = 0;
        versus_select_pad_lost = TRUE;
        return FALSE;
    }
    /* @hack redundant: stores of the left and right buttons and of the
     * choose mask that never run */
    if (0) {
        menu_control.timer = PDD_DGT_KL;
        menu_control.timer = PDD_DGT_KR;
        menu_control.timer = pad_choose_buttons;
    }
    /* @hack spelling: the work read again through the table after work
     * holds it, where work would do */
    work = menu_works_array->work_04;
    /* @hack redundant: a loop of stores that never runs */
    while (0) {
        menu_works_array->work_04->unk_04 = 0;
        menu_works_array->work_04->backdrop.shown = TRUE;
        menu_works_array->work_04->title.shown = TRUE;
    }
    /* @hack spelling: the cases in the order of their code, where the
     * steps' order would do */
#ifdef NONMATCHING
    /* Stage zero is the test map. Both arena lists begin with it only when
     * the selection data is absent. Follow the panel's usual close path. */
    if (random_games[0].stage == STAGE_BASIC_TEST &&
        stage_games[0].stage == STAGE_BASIC_TEST) {
        versus_select_hide();
        menu_control.step = STEP_CLOSING;
    }
#endif
    switch (menu_control.step) {
    case STEP_OPEN:
        work->shown = TRUE;
        menu_works_array->work_04->unk_04 = 1;
        menu_works_array->work_04->backdrop.shown = TRUE;
        menu_works_array->work_04->title.shown = TRUE;
        voice_set_play(OPEN_VOICES);
        menu_control.step = STEP_OPENING;
        break;
    case STEP_OPENING:
        if (work->moving != FALSE) {
            break;
        }
        menu_control.step = STEP_TEAMS;
        menu_works_array->work_04->title.bright = 1;
        menu_works_array->work_04->sides_shown = TRUE;
        menu_works_array->work_04->sides_help_shown = TRUE;
        break;
    case STEP_TEAMS:
        if ((pad_press_repeat[0] & PDD_DGT_KL) |
            (pad_press_repeat[0] & PDD_DGT_KR) |
            (pad_press_repeat[1] & PDD_DGT_KL) |
            (pad_press_repeat[1] & PDD_DGT_KR)) {
            work->swapped = !work->swapped;
            sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            game_state.match_wins[0] = 0;
            game_state.match_wins[1] = 0;
        } else if (EITHER_PRESSED(pad_choose_buttons)) {
            work->title.shown = FALSE;
            menu_works_array->work_04->sides_shown = FALSE;
            menu_works_array->work_04->sides_help_shown = FALSE;
            menu_works_array->work_04->title.bright = 0;
            menu_works_array->work_04->list.shown = TRUE;
            menu_control.step = STEP_LIST_OPENING;
            sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
            /* @hack type: pad_press_repeat[0] and pad_cancel_buttons cast to
             * Sint32, where the two uncast would do */
        } else if ((Sint32)pad_press_repeat[0] & (Sint32)pad_cancel_buttons) {
            work->shown = FALSE;
            menu_works_array->work_04->unk_04 = 0;
            menu_control.step = STEP_CLOSING;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            game_state.match_wins[0] = 0;
            game_state.match_wins[1] = 0;
        }
        break;
    case STEP_CLOSING:
        /* @hack spelling: an empty arm under the test of moving, here and in
         * STEP_LIST_CLOSING's case, where a test of !moving would do */
        if (work->moving != FALSE) {
        } else {
            new_menu_screen_start();
            new_menu_screen_reopen(NEW_MENU_REOPEN_VERSUS);
            menu_control.mode = MENU_MODE_NEW_MENU;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
        }
        break;
    case STEP_LIST_OPENING:
        if (work->moving != FALSE) {
            versus_select_close_looks();
            versus_select_clear_looks();
        } else {
            work->list.bright = 1;
            menu_works_array->work_04->games_help_shown = TRUE;
            menu_control.step = STEP_GAMES;
            /* @bug memory: for the random game and VERSUS_SELECT_STAGES_GAME,
             * which have no pair, this reads pairs[3] and pairs[4], past the
             * three pairs (0x8c509922). those bytes are the work's
             * games_help_shown and wins[0], and its wins[1] and unk_56[0].
             * read as characters, they open looks that the random game then
             * plays with */
            versus_select_find_looks(
                SELECTED_GAME_PAIR(menu_works_array->work_04));
            voice_set_play(GAMES_VOICES);
        }
        break;
    case STEP_GAMES:
        if (EITHER_PRESSED(PDD_DGT_KU)) {
            work->game--;
            if (menu_works_array->work_04->game < 0) {
                menu_works_array->work_04->game = 0;
            } else {
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
            /* @bug memory: reads past pairs, as STEP_LIST_OPENING does
             * (0x8c50999e) */
            versus_select_find_looks(
                SELECTED_GAME_PAIR(menu_works_array->work_04));
            versus_select_clear_looks();
        } else if (EITHER_PRESSED(PDD_DGT_KD)) {
            work->game++;
            if (menu_works_array->work_04->game > VERSUS_SELECT_STAGES_GAME) {
                menu_works_array->work_04->game = VERSUS_SELECT_STAGES_GAME;
            } else {
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
            /* @bug memory: reads past pairs, as STEP_LIST_OPENING does
             * (0x8c5099e6) */
            versus_select_find_looks(
                SELECTED_GAME_PAIR(menu_works_array->work_04));
            versus_select_clear_looks();
        } else if (EITHER_PRESSED(pad_choose_buttons)) {
            if (work->game != VERSUS_SELECT_STAGES_GAME) {
                resume = MENU_RESUME_VERSUS_GAMES;
                menu_snapshot_save();
                menu_resume_target = resume;
                menu_control.step = STEP_GAME_CHOSEN;
                title_screen_hide();
                menu_works_array->work_04->shown = FALSE;
                menu_works_array->work_04->list.bright = 0;
            } else {
                work->stages.shown = TRUE;
                menu_control.step = STEP_STAGES_OPENING;
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                versus_select_close_looks();
            }
            /* @bug logic: plays the choose sound a second time for the
             * list of stages, after its first at 0x8c509a34 (0x8c509a42) */
            sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
            menu_works_array->work_04->games_help_shown = FALSE;
        } else if (EITHER_PRESSED(pad_cancel_buttons)) {
            work->list.shown = FALSE;
            menu_works_array->work_04->list.bright = 0;
            menu_works_array->work_04->games_help_shown = FALSE;
            menu_control.step = STEP_LIST_CLOSING;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            versus_select_close_looks();
            versus_select_clear_looks();
        } else if ((pad_press_repeat[0] & PDD_DGT_KL) |
                   (pad_press_repeat[0] & PDD_DGT_KR)) {
            versus_select_next_look(work->swapped);
        } else if ((pad_press_repeat[1] & PDD_DGT_KL) |
                   (pad_press_repeat[1] & PDD_DGT_KR)) {
            versus_select_next_look(!work->swapped);
        }
        break;
    case STEP_LIST_CLOSING:
        if (work->moving != FALSE) {
        } else {
            work->title.shown = TRUE;
            menu_control.step = STEP_OPENING;
        }
        break;
    case STEP_STAGES_OPENING:
        if (work->moving != FALSE) {
            break;
        }
        menu_control.step = STEP_STAGES;
        menu_works_array->work_04->stages_help_shown = TRUE;
        versus_select_find_looks(
            menu_works_array->work_04
                ->stage_pairs[menu_works_array->work_04->stage]);
        versus_select_clear_looks();
        voice_set_play(STAGES_VOICES);
        break;
    case STEP_STAGES:
        if (EITHER_PRESSED(pad_cancel_buttons)) {
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            menu_works_array->work_04->stages.shown = FALSE;
            menu_works_array->work_04->stages.bright = 0;
            menu_works_array->work_04->stages_help_shown = FALSE;
            /* @bug logic: plays the back sound a second time, after its
             * first at 0x8c509b54 (0x8c509b72) */
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            menu_control.step = STEP_LIST_OPENING;
            versus_select_close_looks();
            versus_select_clear_looks();
        } else if (EITHER_PRESSED(PDD_DGT_KU)) {
            work->stage--;
            if (menu_works_array->work_04->stage < 0) {
                menu_works_array->work_04->stage = 0;
            } else {
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
            versus_select_find_looks(
                menu_works_array->work_04
                    ->stage_pairs[menu_works_array->work_04->stage]);
            versus_select_clear_looks();
        } else if (EITHER_PRESSED(PDD_DGT_KD)) {
            work->stage++;
            /* @bug logic: the cursor stops a stage short of
             * VERSUS_SELECT_LAST_STAGE (0x8c509c1e). the window then never
             * shows the last stage, and the tests of it, below and in the
             * panel's draw, never hold (0x8c509c60, 0x8c509cbc, 0x8c509cec) */
            if (menu_works_array->work_04->stage >
                VERSUS_SELECT_LAST_STAGE - 1) {
                menu_works_array->work_04->stage = VERSUS_SELECT_LAST_STAGE - 1;
            } else {
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
            versus_select_find_looks(
                menu_works_array->work_04
                    ->stage_pairs[menu_works_array->work_04->stage]);
            versus_select_clear_looks();
        } else if (EITHER_PRESSED(pad_choose_buttons)) {
            if (work->stage == VERSUS_SELECT_LAST_STAGE &&
                save_data.emblems != SAVE_EMBLEMS) {
                sound_play(MENU_SOUND_REFUSED, NULL, 0, 0);
            } else {
                resume = MENU_RESUME_VERSUS_STAGES;
                menu_snapshot_save();
                menu_resume_target = resume;
                menu_works_array->work_07->shown = TRUE;
                title_screen_hide();
                menu_control.step = STEP_STAGE_CHOSEN;
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
            }
        } else if ((pad_press_repeat[0] & PDD_DGT_KL) |
                   (pad_press_repeat[0] & PDD_DGT_KR)) {
            if (work->stage < VERSUS_SELECT_LAST_STAGE ||
                work->last_stage_open != FALSE) {
                versus_select_next_look(work->swapped);
            }
        } else if ((pad_press_repeat[1] & PDD_DGT_KL) |
                   (pad_press_repeat[1] & PDD_DGT_KR)) {
            if (work->stage < VERSUS_SELECT_LAST_STAGE ||
                work->last_stage_open != FALSE) {
                versus_select_next_look(!work->swapped);
            }
        }
        break;
    case STEP_GAME_CHOSEN:
        if (work->moving == FALSE) {
            switch (work->game) {
            case SONIC_PAIR:
                versus_select_play_random(SONIC_PAIR, work->swapped);
                break;
            case TAILS_PAIR:
                versus_select_play_random(TAILS_PAIR, work->swapped);
                break;
            case KNUCKLES_PAIR:
                versus_select_play_random(KNUCKLES_PAIR, work->swapped);
                break;
            case VERSUS_SELECT_RANDOM_GAME:
                versus_select_play_random(ANY_PAIR, work->swapped);
                break;
            }
            sequence_start(SEQUENCE_NONE);
            return TRUE;
        }
        break;
    case STEP_STAGE_CHOSEN:
        if (menu_works_array->work_07->fading != FALSE) {
            break;
        }
        if (work->swapped) {
            versus_select_play_stage(work->stage,
                                     work->stage_pairs[work->stage][1],
                                     work->stage_pairs[work->stage][0]);
        } else {
            versus_select_play_stage(work->stage,
                                     work->stage_pairs[work->stage][0],
                                     work->stage_pairs[work->stage][1]);
        }
        sequence_start(SEQUENCE_NONE);
        return TRUE;
        /* @hack redundant: a break after the return */
        break; /* NOLINT(clang-diagnostic-unreachable-code-break) */
    case STEP_RESUME_GAMES:
        if (versus_select_resume_wait-- < 0) {
            menu_control.step = STEP_GAMES;
        }
        break;
    case STEP_RESUME_STAGES:
        if (versus_select_resume_wait-- < 0) {
            menu_control.step = STEP_STAGES_OPENING;
        }
        break;
    }
    return FALSE;
}

/* set up one of pair's random games to play, or one of any pair's for
 * ANY_PAIR. game_state.steps picks it. while swapped is TRUE, player 0
 * takes team 1's character */
static void versus_select_play_random(int pair, Bool swapped)
{
    int game;

    switch (pair) {
    case SONIC_PAIR:
        game = game_state.steps % PAIR_GAMES;
        break;
    case TAILS_PAIR:
        game = (game_state.steps % PAIR_GAMES) + (PAIR_GAMES * TAILS_PAIR);
        break;
    case KNUCKLES_PAIR:
        game = (game_state.steps % PAIR_GAMES) + (PAIR_GAMES * KNUCKLES_PAIR);
        break;
    default:
        game = game_state.steps % (PAIR_GAMES * VERSUS_SELECT_PAIRS);
        break;
    }
    stage_set_current(random_games[game].stage);
    if (swapped) {
        stage_set_character(1, random_games[game].character_0);
        stage_set_character(0, random_games[game].character_1);
    } else {
        stage_set_character(0, random_games[game].character_0);
        stage_set_character(1, random_games[game].character_1);
    }
    versus_select_set_costumes();
    game_state.split_screen = random_games[game].split_screen;
    game_state.two_players = TWO_PLAYERS_3_POINTS;
    game_state.demo = 0;
    game_state.next_mode = GAME_MODE_STAGE;
}

/* set up stage of the list of stages to play. player 0 plays character_0,
 * and player 1 character_1. Tails and Eggman play as their mechs */
static void versus_select_play_stage(int stage, int character_0,
                                     int character_1)
{
    stage_set_current(stage_games[stage].stage);
    if (character_0 == PLAYER_TAILS) {
        character_0 = PLAYER_TAILS_MECH;
    }
    if (character_1 == PLAYER_TAILS) {
        character_1 = PLAYER_TAILS_MECH;
    }
    if (character_0 == PLAYER_EGGMAN) {
        character_0 = PLAYER_EGGMAN_MECH;
    }
    if (character_1 == PLAYER_EGGMAN) {
        character_1 = PLAYER_EGGMAN_MECH;
    }
    stage_set_character(0, character_0);
    stage_set_character(1, character_1);
    versus_select_set_costumes();
    game_state.split_screen = stage_games[stage].split_screen;
    game_state.two_players = TWO_PLAYERS_1_POINT;
    game_state.demo = 0;
    game_state.next_mode = GAME_MODE_STAGE;
}

/* set each player's costume and alt_character in game_state, from the look
 * of the player's team. under THEME_COSTUME_H and THEME_COSTUME_X, both
 * players wear the theme's costume */
static void versus_select_set_costumes(void)
{
    switch (game_state.theme) {
    case THEME_COSTUME_H:
        game_state.costume[0] = COSTUME_H;
        game_state.alt_character[0] = FALSE;
        game_state.costume[1] = COSTUME_H;
        game_state.alt_character[1] = FALSE;
        break;
    case THEME_COSTUME_X:
        game_state.costume[0] = COSTUME_X;
        game_state.alt_character[0] = FALSE;
        game_state.costume[1] = COSTUME_X;
        game_state.alt_character[1] = FALSE;
        break;
    default:
        if (versus_select_work.swapped) {
            switch (versus_select_work.looks[1]) {
            case VERSUS_SELECT_LOOK_ALT:
                game_state.costume[0] = COSTUME_NONE;
                game_state.alt_character[0] = TRUE;
                break;
            case VERSUS_SELECT_LOOK_COSTUME:
                game_state.costume[0] = COSTUME_PSO;
                game_state.alt_character[0] = FALSE;
                break;
            default:
                game_state.costume[0] = COSTUME_NONE;
                game_state.alt_character[0] = FALSE;
                break;
            }
            switch (versus_select_work.looks[0]) {
            case VERSUS_SELECT_LOOK_ALT:
                game_state.costume[1] = COSTUME_NONE;
                game_state.alt_character[1] = TRUE;
                break;
            case VERSUS_SELECT_LOOK_COSTUME:
                game_state.costume[1] = COSTUME_PSO;
                game_state.alt_character[1] = FALSE;
                break;
            default:
                game_state.costume[1] = COSTUME_NONE;
                game_state.alt_character[1] = FALSE;
                break;
            }
        } else {
            switch (versus_select_work.looks[0]) {
            case VERSUS_SELECT_LOOK_ALT:
                game_state.costume[0] = COSTUME_NONE;
                game_state.alt_character[0] = TRUE;
                break;
            case VERSUS_SELECT_LOOK_COSTUME:
                game_state.costume[0] = COSTUME_PSO;
                game_state.alt_character[0] = FALSE;
                break;
            default:
                game_state.costume[0] = COSTUME_NONE;
                game_state.alt_character[0] = FALSE;
                break;
            }
            switch (versus_select_work.looks[1]) {
            case VERSUS_SELECT_LOOK_ALT:
                game_state.costume[1] = COSTUME_NONE;
                game_state.alt_character[1] = TRUE;
                break;
            case VERSUS_SELECT_LOOK_COSTUME:
                game_state.costume[1] = COSTUME_PSO;
                game_state.alt_character[1] = FALSE;
                break;
            default:
                game_state.costume[1] = COSTUME_NONE;
                game_state.alt_character[1] = FALSE;
                break;
            }
        }
        break;
    }
}

/* set looks_open, by team, to the looks open to pair's characters, each a
 * PLAYER_ value. every look stays shut under THEME_COSTUME_H and
 * THEME_COSTUME_X */
static void versus_select_find_looks(const Sint8 *pair)
{
    int team;

    versus_select_close_looks();
    /* @hack spelling: a switch, where a test of the two values would do */
    switch (game_state.theme) {
    case THEME_COSTUME_X:
    case THEME_COSTUME_H:
        return;
    }
    for (team = 0; team < 2; team++) {
        switch (pair[team]) {
        case PLAYER_SONIC:
            if (game_flags[FLAG_RANKED(GROUP_SONIC)]) {
                versus_select_work.looks_open[team] |=
                    VERSUS_SELECT_LOOK_OPEN_COSTUME;
            }
            if (save_data.awards[SAVE_AWARD_SONIC_TOP]) {
                versus_select_work.looks_open[team] |=
                    VERSUS_SELECT_LOOK_OPEN_ALT;
            }
            break;
        case PLAYER_SHADOW:
            if (game_flags[FLAG_RANKED(GROUP_SHADOW)]) {
                versus_select_work.looks_open[team] |=
                    VERSUS_SELECT_LOOK_OPEN_COSTUME;
            }
            if (save_data.awards[SAVE_AWARD_SHADOW_TOP]) {
                versus_select_work.looks_open[team] |=
                    VERSUS_SELECT_LOOK_OPEN_ALT;
            }
            break;
        case PLAYER_TAILS:
        case PLAYER_TAILS_MECH:
            if (game_flags[FLAG_RANKED(GROUP_TAILS)]) {
                versus_select_work.looks_open[team] |=
                    VERSUS_SELECT_LOOK_OPEN_COSTUME;
            }
            if (save_data.awards[SAVE_AWARD_TAILS_TOP]) {
                versus_select_work.looks_open[team] |=
                    VERSUS_SELECT_LOOK_OPEN_ALT;
            }
            break;
        case PLAYER_EGGMAN:
        case PLAYER_EGGMAN_MECH:
            if (game_flags[FLAG_RANKED(GROUP_EGGMAN)]) {
                versus_select_work.looks_open[team] |=
                    VERSUS_SELECT_LOOK_OPEN_COSTUME;
            }
            if (save_data.awards[SAVE_AWARD_EGGMAN_TOP]) {
                versus_select_work.looks_open[team] |=
                    VERSUS_SELECT_LOOK_OPEN_ALT;
            }
            break;
        case PLAYER_KNUCKLES:
            if (game_flags[FLAG_RANKED(GROUP_KNUCKLES)]) {
                versus_select_work.looks_open[team] |=
                    VERSUS_SELECT_LOOK_OPEN_COSTUME;
            }
            if (save_data.awards[SAVE_AWARD_KNUCKLES_TOP]) {
                versus_select_work.looks_open[team] |=
                    VERSUS_SELECT_LOOK_OPEN_ALT;
            }
            break;
        case PLAYER_ROUGE:
            if (game_flags[FLAG_RANKED(GROUP_ROUGE)]) {
                versus_select_work.looks_open[team] |=
                    VERSUS_SELECT_LOOK_OPEN_COSTUME;
            }
            if (save_data.awards[SAVE_AWARD_ROUGE_TOP]) {
                versus_select_work.looks_open[team] |=
                    VERSUS_SELECT_LOOK_OPEN_ALT;
            }
            break;
        }
    }
}

static void versus_select_close_looks(void)
{
    versus_select_work.looks_open[0] = 0;
    versus_select_work.looks_open[1] = 0;
}

static void versus_select_clear_looks(void)
{
    versus_select_work.looks[0] = VERSUS_SELECT_LOOK_NORMAL;
    versus_select_work.looks[1] = VERSUS_SELECT_LOOK_NORMAL;
}

/* change team's look to the next open to it */
static void versus_select_next_look(int team)
{
    switch (versus_select_work.looks[team]) {
    case VERSUS_SELECT_LOOK_NORMAL:
        if (versus_select_work.looks_open[team] & VERSUS_SELECT_LOOK_OPEN_ALT) {
            versus_select_work.looks[team] = VERSUS_SELECT_LOOK_ALT;
            sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
        } else if (versus_select_work.looks_open[team] &
                   VERSUS_SELECT_LOOK_OPEN_COSTUME) {
            versus_select_work.looks[team] = VERSUS_SELECT_LOOK_COSTUME;
            sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
        }
        break;
    case VERSUS_SELECT_LOOK_ALT:
        /* @hack spelling: the sound in each arm, where one after the if
         * would do */
        if (versus_select_work.looks_open[team] &
            VERSUS_SELECT_LOOK_OPEN_COSTUME) {
            versus_select_work.looks[team] = VERSUS_SELECT_LOOK_COSTUME;
            sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
        } else {
            versus_select_work.looks[team] = VERSUS_SELECT_LOOK_NORMAL;
            sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
        }
        break;
    case VERSUS_SELECT_LOOK_COSTUME:
        versus_select_work.looks[team] = VERSUS_SELECT_LOOK_NORMAL;
        sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
        break;
    default:
        versus_select_work.looks[team] = VERSUS_SELECT_LOOK_NORMAL;
        break;
    }
}

void versus_select_resume(void)
{
    switch (menu_resume_target) {
    case MENU_RESUME_VERSUS_GAMES:
        menu_snapshot_restore();
        versus_select_start();
        menu_control.step = STEP_RESUME_GAMES;
        menu_control.mode = MENU_MODE_VERSUS_SELECT;
        break;
    case MENU_RESUME_VERSUS_STAGES:
        menu_snapshot_restore();
        versus_select_start();
        menu_control.step = STEP_RESUME_STAGES;
        menu_control.mode = MENU_MODE_VERSUS_SELECT;
        break;
    }
    versus_select_resume_wait = RESUME_STEPS;
}
