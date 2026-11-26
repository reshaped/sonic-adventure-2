/* the menus: the title screen and every menu after it, each a mode of
 * menu_control */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "advertise/alpha_title.h"
#include "advertise/boss_attack.h"
#include "advertise/boss_attack_panel.h"
#include "advertise/download.h"
#include "advertise/download_panel.h"
#include "advertise/emblem_results.h"
#include "advertise/emblem_results_menu.h"
#include "advertise/file_select.h"
#include "advertise/kart_race.h"
#include "advertise/kart_race_panel.h"
#include "advertise/main_menu.h"
#include "advertise/menu_snapshot.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/new_menu.h"
#include "advertise/new_menu_screen.h"
#include "advertise/options.h"
#include "advertise/options_draw.h"
#include "advertise/screen_effect.h"
#include "advertise/screen_fade.h"
#include "advertise/share_data.h"
#include "advertise/sound_test.h"
#include "advertise/sound_test_panel.h"
#include "advertise/stage_map.h"
#include "advertise/stage_map_menu.h"
#include "advertise/story_select.h"
#include "advertise/story_select_draw.h"
#include "advertise/synopsis.h"
#include "advertise/synopsis_screen.h"
#include "advertise/versus_select.h"
#include "advertise/versus_select_draw.h"
#include "advertise/title_screen.h"
#include "advertise/tutorial.h"
#include "advertise/tutorial_panel.h"
#include "advertise/tv_setting.h"
#include "advertise/tv_setting_draw.h"
#include "advertise/world_ranking.h"
#include "advertise/world_ranking_menu.h"
#include "draw/fade.h"
#include "hud/hud.h"
#include "main.h"
#include "save/save.h"
#include "sound/adx.h"
#include "system/flags.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/sbinit.h"
#include "system/task.h"

Bool menus_leave_game = FALSE;
struct menu_control menu_control;
/* Shared state in the order used by menu_snapshot and menu_textures. */
struct menu_works menu_works = {
    &alpha_title_work,   &main_menu_work,       &stage_map_work,
    &story_select_work,  &versus_select_work,   &new_menu_work,
    &synopsis_work,      &screen_fade_work,     &options_work,
    &tv_setting_work,    &menu_sound_test_work, &share_data_work,
    &file_select_work,   &kart_race_work,       &download_panel_work,
    &boss_attack_work,   &tutorial_work,        &screen_effect_work,
    &world_ranking_work, &emblem_results_work,
};
struct menu_set menus_set = {{0, 0, 0, 0}, &menu_works};

/* step the running menu, and watch the reset buttons. it returns TRUE if
 * the menus end */
static Bool menus_run(void)
{
    Bool done;

    done = FALSE;
    pad_scan();
    menu_color_step();
    switch (menu_control.mode) {
    case MENU_MODE_TITLE:
        done = title_screen_step();
        break;
    case MENU_MODE_NEW_MENU:
        done = new_menu_screen_step();
        break;
    case MENU_MODE_STORY_SELECT:
        done = story_select_step();
        break;
    case MENU_MODE_STAGE_MAP:
        done = stage_map_menu_step();
        break;
    case MENU_MODE_VERSUS_SELECT:
        done = versus_select_step();
        break;
    case MENU_MODE_SYNOPSIS:
        done = synopsis_screen_step();
        break;
    case MENU_MODE_OPTIONS:
        done = options_step();
        break;
    case MENU_MODE_TV_SETTING:
        done = tv_setting_step();
        break;
    case MENU_MODE_SOUND_TEST:
        done = menu_sound_test_step();
        break;
    case MENU_MODE_FILE_SELECT:
        done = file_select_step();
        break;
    case MENU_MODE_KART_RACE:
        done = kart_race_step();
        break;
    case MENU_MODE_DOWNLOAD:
        done = download_step();
        break;
    case MENU_MODE_BOSS_ATTACK:
        done = boss_attack_step();
        break;
    case MENU_MODE_TUTORIAL:
        done = tutorial_step();
        break;
    case MENU_MODE_UNK_0E:
        break;
    case MENU_MODE_WORLD_RANKING:
        done = world_ranking_menu_step();
        break;
    case MENU_MODE_EMBLEM_RESULTS:
        done = emblem_results_menu_step();
        break;
    case MENU_MODE_UNK_11:
        break;
    /* @hack redundant: a default that only breaks */
    default:
        break;
    }
    if (done) {
        if (game_state.lives[0] < LIVES_START) {
            lives_reset();
        }
        return TRUE;
    }
    if (pad_reset_pressed(0)) {
        if (menu_control.mode == MENU_MODE_TITLE) {
            sbExitSystem();
            syBtExit();
        } else {
            game_state.next_mode = GAME_MODE_TO_MENU;
            menu_resume_target = 0;
            game_state.skip_menus = 0;
            return TRUE;
        }
    }
    return FALSE;
}

void menus_init(void)
{
    Uint8 *byte;
    int left;

    if (flag_get(MENU_FLAG_SAVE_KEPT)) {
        save_restore();
        flag_clear(MENU_FLAG_SAVE_KEPT);
    }
    alpha_title_create();
    main_menu_create();
    stage_map_create();
    story_select_draw_create();
    versus_select_draw_create();
    new_menu_create();
    synopsis_create();
    screen_fade_create();
    options_draw_create();
    tv_setting_draw_create();
    menu_sound_test_panel_create();
    share_data_create();
    file_select_create();
    kart_race_panel_create();
    download_panel_create();
    boss_attack_panel_create();
    tutorial_panel_create();
    screen_effect_create();
    world_ranking_create();
    emblem_results_create();
    byte = (Uint8 *)&menu_control;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(menu_control); left != 0; left--) {
        *byte++ = 0;
    }
    music_fade_in(VOLUME_FULL);
    menus_resume();
}

int menus_step(void)
{
    enum overlay_result result;

    result = OVERLAY_RUNNING;
    if (!menu_control.fade_held) {
        fade_in_step();
    }
    game_state.idle_dim = 0;
    njFogDisable();
    if (menus_run()) {
        result = OVERLAY_DONE;
        if (menus_leave_game) {
            result = OVERLAY_BACK;
        }
    }
    task_step_all();
    task_draw_all();
    game_state.steps++;
    return result;
}

void menus_reset(void)
{
    main_menu_reset();
    stage_map_reset();
    story_select_draw_reset();
    versus_select_draw_reset();
    new_menu_reset();
    synopsis_reset();
    menu_sound_test_panel_reset();
    share_data_reset();
    kart_race_panel_reset();
    download_panel_reset();
    boss_attack_panel_reset();
    screen_effect_reset();
    emblem_results_reset();
}
