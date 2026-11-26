/* the title screen, MENU_MODE_TITLE: the first menu. it waits for Start over
 * the alpha title, and plays a demo after MENU_IDLE_STEPS without it */
#include <sg_xpt.h>
#include <shinobi.h>

#include "advertise/alpha_title.h"
#include "advertise/file_select.h"
#include "advertise/menu_textures.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/new_menu_screen.h"
#include "advertise/title_screen.h"
#include "hud/hud.h"
#include "main.h"
#include "player/player.h"
#include "save/save.h"
#include "shared/card_warning.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/flags.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/sequence.h"
#include "system/task.h"

struct alpha_title_work alpha_title_work;

/* menu_control.step, while the title screen runs */
enum {
    STEP_SHOW, /* bring the backdrop in */
    STEP_FADE_IN, /* wait for the backdrop to fade in */
    STEP_WAIT, /* blink the prompt, and wait for Start */
    STEP_DEMO_FADE, /* wait for the backdrop to go, then ready a demo */
    STEP_CONTINUE, /* go on to MENU_MODE_NEW_MENU */
    STEP_CARD_WARNING /* wait for the card warning to end, or for a card */
};

/* the card warning's task */
static struct task *title_screen_warning;

static struct menu_demo demos[MENU_DEMOS] = {
    {STAGE_CITY_ESCAPE, PLAYER_SONIC},
    {STAGE_SAND_OCEAN, PLAYER_EGGMAN_MECH},
    {STAGE_WILD_CANYON, PLAYER_KNUCKLES},
    {STAGE_RADICAL_HIGHWAY, PLAYER_SHADOW},
    {STAGE_PRISON_LANE, PLAYER_TAILS_MECH},
    {STAGE_DRY_LAGOON, PLAYER_ROUGE},
};

void title_screen_start(void)
{
    menu_textures_select(MENU_TEXTURES_TITLE);
    menu_control.step = STEP_SHOW;
    music_play_name("title.adx");
}

void title_screen_hide(void)
{
    menu_works.alpha_title->shown = FALSE;
    menu_works.alpha_title->bright = FALSE;
}

void title_screen_continue(int which)
{
    /* @hack spelling: a switch of one case, where an if would do */
    switch (which) {
    case TITLE_SCREEN_CONTINUE:
        menu_control.step = STEP_CONTINUE;
        break;
    }
}

Bool title_screen_step(void)
{
    switch (menu_control.step) {
    case STEP_SHOW:
        menu_works.alpha_title->shown = TRUE;
        menu_works.alpha_title->bright = TRUE;
        menu_control.timer = 0;
        menu_control.step = STEP_FADE_IN;
        break;
    case STEP_FADE_IN:
        if (!menu_works.alpha_title->fading) {
            menu_control.step = STEP_WAIT;
        }
        break;
    case STEP_WAIT:
        menu_works.alpha_title->prompt =
            (game_state.steps >> MENU_PROMPT_SHIFT) & 1;
        if ((pad_press_repeat[0] & PDD_DGT_ST) && !pad_reset_pressed(0)) {
            menu_works.alpha_title->bright = FALSE;
            if (save_place.drive != SAVE_DRIVE_NONE) {
                new_menu_screen_start();
                menu_control.mode = MENU_MODE_NEW_MENU;
                sound_play(MENU_SOUND_START, NULL, 0, 0);
            } else if (file_select_card_present()) {
                file_select_return_mode = MENU_MODE_TITLE;
                file_select_start();
                menu_control.mode = MENU_MODE_FILE_SELECT;
                sound_play(MENU_SOUND_START, NULL, 0, 0);
            } else {
                title_screen_warning = save_warning_create();
                game_state.card_warning = SAVE_NO_ROOM;
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                menu_control.step = STEP_CARD_WARNING;
            }
        } else {
            menu_control.timer++;
            if (menu_control.timer > MENU_IDLE_STEPS) {
                menu_control.step = STEP_DEMO_FADE;
                menu_works.alpha_title->shown = FALSE;
            }
        }
        break;
    case STEP_DEMO_FADE:
        if (!menu_works.alpha_title->fading) {
            if (demos[game_state.next_demo].stage == MENU_NO_STAGE) {
                save_clear();
                menu_play_taiken_event();
            } else {
                stage_set_current(demos[game_state.next_demo].stage);
                game_state.two_players = 0;
                stage_set_character(0, demos[game_state.next_demo].character);
                sequence_start(SEQUENCE_NONE);
                game_state.next_mode = GAME_MODE_DEMO;
            }
            game_state.next_demo++;
            save_keep();
            flag_set(MENU_FLAG_SAVE_KEPT);
            /* @hack type: next_demo compared as a Uint32, where its own
             * Sint8 would do */
            if ((Uint32)game_state.next_demo >= MENU_DEMOS) {
                game_state.next_demo = 0;
            }
            lives_reset();
            game_state.demo = DEMO_ON;
            game_state.mission = MISSION_FIRST;
            return TRUE;
        }
        break;
    case STEP_CONTINUE:
        menu_works.alpha_title->bright = FALSE;
        new_menu_screen_start();
        menu_control.mode = MENU_MODE_NEW_MENU;
        sound_play(MENU_SOUND_START, NULL, 0, 0);
        break;
    case STEP_CARD_WARNING:
        /* the warning's task has destroyed itself */
        if (title_screen_warning->step == task_destroy) {
            sound_play(MENU_SOUND_START, NULL, 0, 0);
            new_menu_screen_start();
            menu_control.mode = MENU_MODE_NEW_MENU;
            save_set_drive(SAVE_DRIVE_REFUSED);
        } else if (file_select_card_present()) {
            file_select_return_mode = MENU_MODE_TITLE;
            file_select_start();
            menu_control.mode = MENU_MODE_FILE_SELECT;
            game_state.card_warning = SAVE_OK;
        }
        break;
    }
    return FALSE;
}
