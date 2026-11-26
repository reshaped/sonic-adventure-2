/* the fake title: a title and mode select in the menus' place. it runs
 * under the values of game_state.edition that game_state.h names for it. a
 * choice starts a sequence or a two-player game. MENU_IDLE_STEPS without
 * Start play a demo */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "advertise/fake_title.h"
#include "advertise/fake_title_draw.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "draw/fade.h"
#include "hud/hud.h"
#include "main.h"
#include "player/player.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/flags.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/sbinit.h"
#include "system/sequence.h"
#include "system/task.h"

/* fake_title_control.mode. nothing sets mode 2, and no case takes it */
enum {
    MODE_MUSIC = 0, /* start the title's music */
    MODE_WAIT = 1, /* blink the prompt, and wait for Start */
    MODE_MODES = 3, /* FAKE_TITLE_MODES's page */
    /* the lists' pages, by the row chosen on the modes */
    MODE_LIST_0 = 4,
    MODE_LIST_1 = 5,
    MODE_LIST_2 = 6,
    MODE_DEMO_FADE = 7, /* wait for the backdrop to go, then ready a demo */
    /* wait for the backdrop and the lists to go, then ready the choice */
    MODE_CHOSEN = 8,
    MODE_TENTO = 9, /* FAKE_TITLE_TENTO's page */
    MODE_TENTO_CHOSEN = 10
};

/* choice n below FIRST_VERSUS starts sequence SEQUENCE_CITY_ESCAPE + n.
 * choice FIRST_VERSUS + n plays row n's game of versus_games */
#define FIRST_VERSUS ((FAKE_TITLE_LIST_2 - FAKE_TITLE_LIST_0) * FAKE_TITLE_ROWS)
/* the game flag that picks the set of versus_games and of tento_games. each
 * two-player game flips it */
#define FLAG_VERSUS_SET 34
/* FAKE_TITLE_TENTO's last row, which plays a game of tento_games */
#define TENTO_VERSUS_ROW 2

/* @hack type: the fields as int, where Sint32 would do */
struct fake_title_control {
    Int mode;
    /* the steps spent in MODE_WAIT since the title started or a Cancel went
     * back to it */
    Int timer;
    /* a list's row chosen, counted across the lists: row + (page -
     * FAKE_TITLE_LIST_0) * FAKE_TITLE_ROWS */
    Int choice;
};
static struct fake_title_control fake_title_control;
static struct fake_title_backdrop fake_title_backdrop_work;
static struct fake_title_page fake_title_pages_work[FAKE_TITLE_PAGES];

/* a two-player game */
struct fake_title_versus {
    Sint32 stage;
    Sint32 character_0;
    Sint32 character_1;
    Sint32 split_screen;
};

#include "advertise/fake_title/text.inc"

static struct menu_demo demos[MENU_DEMOS] = {
    {STAGE_CITY_ESCAPE, PLAYER_SONIC},
    {STAGE_SAND_OCEAN, PLAYER_EGGMAN_MECH},
    {STAGE_WILD_CANYON, PLAYER_KNUCKLES},
    {STAGE_RADICAL_HIGHWAY, PLAYER_SHADOW},
    {STAGE_PRISON_LANE, PLAYER_TAILS_MECH},
    {STAGE_DRY_LAGOON, PLAYER_ROUGE},
};
/* by FLAG_VERSUS_SET, the games of FAKE_TITLE_LIST_2's rows */
/* clang-format off */
static struct fake_title_versus versus_games[2][FAKE_TITLE_ROWS] = {
    {{STAGE_GREEN_FOREST,    PLAYER_SONIC,      PLAYER_SHADOW,      1},
     {STAGE_WEAPONS_BED_2P,  PLAYER_TAILS_MECH, PLAYER_EGGMAN_MECH, 0},
     {STAGE_WILD_CANYON_2P,  PLAYER_KNUCKLES,   PLAYER_ROUGE,       1}},
    {{STAGE_RADICAL_HIGHWAY, PLAYER_SONIC,      PLAYER_SHADOW,      1},
     {STAGE_WEAPONS_BED_2P,  PLAYER_TAILS_MECH, PLAYER_EGGMAN_MECH, 0},
     {STAGE_WILD_CANYON_2P,  PLAYER_KNUCKLES,   PLAYER_ROUGE,       1}},
};
/* by FLAG_VERSUS_SET, the game of TENTO_VERSUS_ROW */
static struct fake_title_versus tento_games[2] = {
    {STAGE_GREEN_FOREST,    PLAYER_SONIC, PLAYER_SHADOW, 1},
    {STAGE_RADICAL_HIGHWAY, PLAYER_SONIC, PLAYER_SHADOW, 1},
};
/* clang-format on */
struct fake_title_works fake_title_works = {&fake_title_backdrop_work,
                                            fake_title_pages_work};
struct fake_title_set fake_title_set = {
    {{0, 0, 0, 0}, &fake_title_works}, 0, 0, 0, 0,
};
static NJS_TEXNAME texnames[12] = {
#include "advertise/fake_title/texnames.inc"
};
NJS_TEXLIST fake_title_texlist = {texnames,
                                  sizeof(texnames) / sizeof(texnames[0])};

/* the title's step. it returns TRUE once it has readied a game or a demo */
static Bool fake_title_run(void)
{
    int page;

    pad_scan();
    switch (fake_title_control.mode) {
    case MODE_MUSIC:
        fake_title_control.mode = MODE_WAIT;
        music_play_name("title.adx");
        break;
    case MODE_WAIT:
        fake_title_works.backdrop->shown = TRUE;
        fake_title_works.backdrop->bright = TRUE;
        if (!fake_title_works.backdrop->fading) {
            fake_title_works.backdrop->prompt =
                (game_state.steps >> MENU_PROMPT_SHIFT) & 1;
            if (pad_press_repeat[0] & PDD_DGT_ST) {
                fake_title_works.backdrop->bright = FALSE;
                fake_title_works.backdrop->dim = 0;
                sound_play(MENU_SOUND_START, NULL, 0, 0);
                music_play_name("advsng_1.adx");
                if (game_state.edition == EDITION_TENTO) {
                    fake_title_control.mode = MODE_TENTO;
                } else {
                    fake_title_control.mode = MODE_MODES;
                }
            }
        }
        ++fake_title_control.timer;
        if (fake_title_control.timer > MENU_IDLE_STEPS) {
            fake_title_control.mode = MODE_DEMO_FADE;
            fake_title_works.backdrop->shown = FALSE;
        }
        break;
    case MODE_MODES:
        fake_title_works.pages[FAKE_TITLE_MODES].shown = TRUE;
        if (!fake_title_works.pages[FAKE_TITLE_MODES].fading) {
            fake_title_works.pages[FAKE_TITLE_MODES].cursor = TRUE;
            /* @hack type: the pad's masks cast to int, where their own Uint32
             * would do */
            if ((int)pad_press_repeat[0] & (int)pad_choose_buttons) {
                fake_title_control.mode =
                    fake_title_works.pages[FAKE_TITLE_MODES].row + MODE_LIST_0;
                fake_title_works.pages[FAKE_TITLE_MODES].shown = FALSE;
                music_play_name("advsng_2.adx");
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                fake_title_works.backdrop->dim = 1;
            } else if (pad_press_repeat[0] & pad_cancel_buttons) {
                fake_title_control.mode = MODE_WAIT;
                fake_title_works.pages[FAKE_TITLE_MODES].shown = FALSE;
                fake_title_control.timer = 0;
                sound_play(MENU_SOUND_BACK, NULL, 0, 0);
                music_play_name("title.adx");
            } else if (pad_press_repeat[0] & PDD_DGT_KU) {
                --fake_title_works.pages[FAKE_TITLE_MODES].row;
                if (fake_title_works.pages[FAKE_TITLE_MODES].row < 0) {
                    fake_title_works.pages[FAKE_TITLE_MODES].row = 0;
                } else {
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
            } else if (pad_press_repeat[0] & PDD_DGT_KD) {
                ++fake_title_works.pages[FAKE_TITLE_MODES].row;
                if (fake_title_works.pages[FAKE_TITLE_MODES].row >
                    FAKE_TITLE_ROWS - 1) {
                    fake_title_works.pages[FAKE_TITLE_MODES].row =
                        FAKE_TITLE_ROWS - 1;
                } else {
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
            }
        }
        break;
    case MODE_LIST_0:
    case MODE_LIST_1:
    case MODE_LIST_2:
        page = fake_title_control.mode - MODE_MODES;
        fake_title_works.pages[page].shown = TRUE;
        if (!fake_title_works.pages[page].fading) {
            fake_title_works.pages[page].cursor = TRUE;
            /* @hack type: the pad's masks cast to int, where their own Uint32
             * would do */
            if ((int)pad_press_repeat[0] & (int)pad_choose_buttons) {
                fake_title_works.backdrop->shown = FALSE;
                fake_title_works.pages[page].shown = FALSE;
                fake_title_control.choice =
                    fake_title_works.pages[page].row +
                    ((fake_title_control.mode - MODE_LIST_0) * FAKE_TITLE_ROWS);
                fake_title_control.mode = MODE_CHOSEN;
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
            } else if (pad_press_repeat[0] & pad_cancel_buttons) {
                fake_title_control.mode = MODE_MODES;
                fake_title_works.pages[page].shown = FALSE;
                music_play_name("advsng_1.adx");
                sound_play(MENU_SOUND_BACK, NULL, 0, 0);
                fake_title_works.backdrop->dim = 0;
            } else if (pad_press_repeat[0] & PDD_DGT_KU) {
                --fake_title_works.pages[page].row;
                if (fake_title_works.pages[page].row < 0) {
                    fake_title_works.pages[page].row = 0;
                } else {
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
            } else if (pad_press_repeat[0] & PDD_DGT_KD) {
                ++fake_title_works.pages[page].row;
                if (fake_title_works.pages[page].row > FAKE_TITLE_ROWS - 1) {
                    fake_title_works.pages[page].row = FAKE_TITLE_ROWS - 1;
                } else {
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
            }
        }
        break;
    case MODE_DEMO_FADE:
        if (!fake_title_works.backdrop->fading) {
            if (demos[game_state.next_demo].stage == MENU_NO_STAGE) {
                menu_play_taiken_event();
            } else {
                stage_set_current(demos[game_state.next_demo].stage);
                game_state.two_players = 0;
                stage_set_character(0, demos[game_state.next_demo].character);
                game_state.next_mode = GAME_MODE_DEMO;
            }
            ++game_state.next_demo;
            /* @hack type: next_demo compared as a Uint32, where its own Sint8
             * would do */
            if ((Uint32)game_state.next_demo >= MENU_DEMOS) {
                game_state.next_demo = 0;
            }
            game_state.demo = DEMO_ON;
            return TRUE;
        }
        break;
    case MODE_CHOSEN:
        if (!fake_title_works.backdrop->fading &&
            !fake_title_works.pages[FAKE_TITLE_LIST_0].fading &&
            !fake_title_works.pages[FAKE_TITLE_LIST_1].fading &&
            !fake_title_works.pages[FAKE_TITLE_LIST_2].fading) {
            game_flags[UPGRADE_SONIC_SHOES] = 1;
            game_flags[UPGRADE_TWALKER_BOOSTER] = 1;
            game_flags[UPGRADE_KNUCKLES_CLAWS] = 1;
            game_flags[UPGRADE_SHADOW_SHOES] = 1;
            game_flags[UPGRADE_EWALKER_JETENGINE] = 1;
            game_flags[UPGRADE_ROUGE_NAILS] = 1;
            switch (fake_title_control.choice) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                menu_play_sequence(fake_title_control.choice +
                                   SEQUENCE_CITY_ESCAPE);
                break;
            default:
                stage_set_current(
                    versus_games[game_flags[FLAG_VERSUS_SET]]
                                [fake_title_control.choice - FIRST_VERSUS]
                                    .stage);
                stage_set_character(
                    0, versus_games[game_flags[FLAG_VERSUS_SET]]
                                   [fake_title_control.choice - FIRST_VERSUS]
                                       .character_0);
                stage_set_character(
                    1, versus_games[game_flags[FLAG_VERSUS_SET]]
                                   [fake_title_control.choice - FIRST_VERSUS]
                                       .character_1);
                game_state.split_screen =
                    versus_games[game_flags[FLAG_VERSUS_SET]]
                                [fake_title_control.choice - FIRST_VERSUS]
                                    .split_screen;
                game_state.two_players = TWO_PLAYERS_1_POINT;
                game_state.demo = 0;
                game_state.next_mode = GAME_MODE_STAGE;
                game_flags[FLAG_VERSUS_SET] = !game_flags[FLAG_VERSUS_SET];
                break;
            }
            return TRUE;
        }
        break;
    case MODE_TENTO:
        fake_title_works.pages[FAKE_TITLE_TENTO].refused[0] = FALSE;
        fake_title_works.pages[FAKE_TITLE_TENTO].refused[1] = FALSE;
        if (pad_player_is_connected(1)) {
            fake_title_works.pages[FAKE_TITLE_TENTO].refused[TENTO_VERSUS_ROW] =
                FALSE;
        } else {
            fake_title_works.pages[FAKE_TITLE_TENTO].refused[TENTO_VERSUS_ROW] =
                TRUE;
        }
        fake_title_works.pages[FAKE_TITLE_TENTO].shown = TRUE;
        if (!fake_title_works.pages[FAKE_TITLE_TENTO].fading) {
            fake_title_works.pages[FAKE_TITLE_TENTO].cursor = TRUE;
            /* @hack type: the pad's masks cast to int, where their own Uint32
             * would do */
            if ((int)pad_press_repeat[0] & (int)pad_choose_buttons) {
                if (!fake_title_works.pages[FAKE_TITLE_TENTO].refused
                         [fake_title_works.pages[FAKE_TITLE_TENTO].row]) {
                    fake_title_works.backdrop->shown = FALSE;
                    fake_title_works.pages[FAKE_TITLE_TENTO].shown = FALSE;
                    fake_title_control.mode = MODE_TENTO_CHOSEN;
                    sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                } else {
                    sound_play(MENU_SOUND_REFUSED, NULL, 0, 0);
                }
            } else if (pad_press_repeat[0] & pad_cancel_buttons) {
                fake_title_control.mode = MODE_WAIT;
                fake_title_works.pages[FAKE_TITLE_TENTO].shown = FALSE;
                fake_title_control.timer = 0;
                sound_play(MENU_SOUND_BACK, NULL, 0, 0);
                music_play_name("title.adx");
            } else if (pad_press_repeat[0] & PDD_DGT_KU) {
                --fake_title_works.pages[FAKE_TITLE_TENTO].row;
                if (fake_title_works.pages[FAKE_TITLE_TENTO].row < 0) {
                    fake_title_works.pages[FAKE_TITLE_TENTO].row = 0;
                } else {
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
            } else if (pad_press_repeat[0] & PDD_DGT_KD) {
                ++fake_title_works.pages[FAKE_TITLE_TENTO].row;
                if (fake_title_works.pages[FAKE_TITLE_TENTO].row >
                    FAKE_TITLE_ROWS - 1) {
                    fake_title_works.pages[FAKE_TITLE_TENTO].row =
                        FAKE_TITLE_ROWS - 1;
                } else {
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
            }
        }
        break;
    case MODE_TENTO_CHOSEN:
        /* @bug logic: the title ends with FAKE_TITLE_TENTO's page still
         * about half lit. the test waits on the backdrop and the three
         * lists' pages, not on this page (0x8c501386, 0x8c50138e and
         * 0x8c501396). the backdrop fades out from its dim level of 0.5 in
         * 15 steps, half the page's 30 */
        if (!fake_title_works.backdrop->fading &&
            !fake_title_works.pages[FAKE_TITLE_LIST_0].fading &&
            !fake_title_works.pages[FAKE_TITLE_LIST_1].fading &&
            !fake_title_works.pages[FAKE_TITLE_LIST_2].fading) {
            game_flags[UPGRADE_SONIC_SHOES] = 1;
            game_flags[UPGRADE_TWALKER_BOOSTER] = 1;
            game_flags[UPGRADE_KNUCKLES_CLAWS] = 1;
            game_flags[UPGRADE_SHADOW_SHOES] = 1;
            game_flags[UPGRADE_EWALKER_JETENGINE] = 1;
            game_flags[UPGRADE_ROUGE_NAILS] = 1;
            switch (fake_title_works.pages[FAKE_TITLE_TENTO].row) {
            case 0:
                menu_play_sequence(SEQUENCE_CITY_ESCAPE);
                break;
            case 1:
                menu_play_sequence(SEQUENCE_RADICAL_HIGHWAY);
                break;
            default:
                sequence_start(SEQUENCE_NONE);
                stage_set_current(
                    tento_games[game_flags[FLAG_VERSUS_SET]].stage);
                stage_set_character(
                    0, tento_games[game_flags[FLAG_VERSUS_SET]].character_0);
                stage_set_character(
                    1, tento_games[game_flags[FLAG_VERSUS_SET]].character_1);
                game_state.split_screen =
                    tento_games[game_flags[FLAG_VERSUS_SET]].split_screen;
                game_state.two_players = TWO_PLAYERS_1_POINT;
                game_state.demo = 0;
                game_state.next_mode = GAME_MODE_STAGE;
                game_flags[FLAG_VERSUS_SET] = !game_flags[FLAG_VERSUS_SET];
                break;
            }
            return TRUE;
        }
    }
    if (pad_reset_pressed(0)) {
        sbExitSystem();
        syBtExit();
    }
    return FALSE;
}

void fake_title_init(void)
{
    Uint8 *byte;
    int left;

    lives_reset();
    fake_title_draw_create();
    byte = (Uint8 *)&fake_title_control;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(fake_title_control); left != 0; left--) {
        *byte++ = 0;
    }
    byte = game_flags;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(game_flags); left != 0; left--) {
        *byte++ = 0;
    }
    byte = game_bits;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(game_bits); left != 0; left--) {
        *byte++ = 0;
    }
    music_fade_in(VOLUME_FULL);
}

int fake_title_step(void)
{
    enum overlay_result result;

    result = OVERLAY_RUNNING;
    fade_set_color(0);
    game_state.idle_dim = 0;
    if (fake_title_run()) {
        result = OVERLAY_DONE;
    }
    njFogDisable();
    task_step_all();
    task_draw_all();
    game_state.steps++;
    return result;
}
