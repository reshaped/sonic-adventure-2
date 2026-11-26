/* the menus' snapshot and their resume. the snapshot copies every menu's
 * work into the main executable's data, which lasts while the program is
 * gone */
#include <sg_xpt.h>

#include "advertise/alpha_title.h"
#include "advertise/boss_attack.h"
#include "advertise/download.h"
#include "advertise/emblem_results_menu.h"
#include "advertise/kart_race.h"
#include "advertise/menu_snapshot.h"
#include "advertise/menus.h"
#include "advertise/new_menu_screen.h"
#include "advertise/stage_map_menu.h"
#include "advertise/title_screen.h"
#include "advertise/tv_setting.h"
#include "advertise/versus_select.h"
#include "main.h"
#include "system/game_state.h"

static Sint32 snapshot_size = sizeof(struct menu_snapshot);

void menus_resume(void)
{
    if (game_state.display_pending) {
        tv_setting_open();
    } else {
        switch (menu_resume_target) {
        case MENU_RESUME_VERSUS_GAMES:
        case MENU_RESUME_VERSUS_STAGES:
            versus_select_resume();
            break;
        case MENU_RESUME_STAGE_MAP:
            stage_map_menu_resume();
            break;
        case MENU_RESUME_KART_RACE:
            kart_race_resume();
            break;
        case MENU_RESUME_DOWNLOAD:
            download_resume();
            break;
        case MENU_RESUME_BOSS_ATTACK:
            boss_attack_resume();
            break;
        case MENU_RESUME_EMBLEM_RESULTS:
            emblem_results_menu_resume();
            break;
        case MENU_RESUME_VERSUS:
            new_menu_screen_resume();
            break;
        default:
            title_screen_start();
            menu_control.mode = MENU_MODE_TITLE;
            break;
        }
    }
    menu_resume_target = 0;
    menu_works.work_07->shown = FALSE;
}

void menu_snapshot_restore(void)
{
    /* @hack spelling: the snapshot through a local pointer, where its own
     * name would do */
    const struct menu_snapshot *snapshot = &menu_snapshot;

    *menu_works.alpha_title = snapshot->alpha_title;
    *menu_works.work_01 = snapshot->work_01;
    *menu_works.work_02 = snapshot->work_02;
    *menu_works.work_03 = snapshot->work_03;
    *menu_works.work_04 = snapshot->work_04;
    *menu_works.work_05 = snapshot->work_05;
    *menu_works.work_06 = snapshot->work_06;
    *menu_works.work_07 = snapshot->work_07;
    *menu_works.work_08 = snapshot->work_08;
    *menu_works.work_09 = snapshot->work_09;
    *menu_works.work_10 = snapshot->work_10;
    *menu_works.work_11 = snapshot->work_11;
    *menu_works.work_12 = snapshot->work_12;
    *menu_works.work_13 = snapshot->work_13;
    *menu_works.work_14 = snapshot->work_14;
    *menu_works.work_15 = snapshot->work_15;
    *menu_works.work_16 = snapshot->work_16;
    *menu_works.work_17 = snapshot->work_17;
    *menu_works.work_18 = snapshot->work_18;
    *menu_works.work_19 = snapshot->work_19;
}

void menu_snapshot_save(void)
{
    /* @hack spelling: the snapshot through a local pointer, where its own
     * name would do */
    struct menu_snapshot *snapshot = &menu_snapshot;

    snapshot->alpha_title = *menu_works.alpha_title;
    snapshot->work_01 = *menu_works.work_01;
    snapshot->work_02 = *menu_works.work_02;
    snapshot->work_03 = *menu_works.work_03;
    snapshot->work_04 = *menu_works.work_04;
    snapshot->work_05 = *menu_works.work_05;
    snapshot->work_06 = *menu_works.work_06;
    snapshot->work_07 = *menu_works.work_07;
    snapshot->work_08 = *menu_works.work_08;
    snapshot->work_09 = *menu_works.work_09;
    snapshot->work_10 = *menu_works.work_10;
    snapshot->work_11 = *menu_works.work_11;
    snapshot->work_12 = *menu_works.work_12;
    snapshot->work_13 = *menu_works.work_13;
    snapshot->work_14 = *menu_works.work_14;
    snapshot->work_15 = *menu_works.work_15;
    snapshot->work_16 = *menu_works.work_16;
    snapshot->work_17 = *menu_works.work_17;
    snapshot->work_18 = *menu_works.work_18;
    snapshot->work_19 = *menu_works.work_19;
    snapshot->alpha_title.loaded = FALSE;
    snapshot->work_01.loaded = FALSE;
    snapshot->work_02.loaded = FALSE;
    snapshot->work_03.loaded = FALSE;
    snapshot->work_04.loaded = FALSE;
    snapshot->work_05.loaded = FALSE;
    snapshot->work_06.loaded = FALSE;
    snapshot->work_07.loaded = FALSE;
    snapshot->work_08.loaded = FALSE;
    snapshot->work_09.loaded = FALSE;
    snapshot->work_10.loaded = FALSE;
    snapshot->work_11.loaded = FALSE;
    snapshot->work_12.loaded = FALSE;
    snapshot->work_13.loaded = FALSE;
    snapshot->work_14.loaded = FALSE;
    snapshot->work_15.loaded = FALSE;
    snapshot->work_16.loaded = FALSE;
    snapshot->work_17.loaded = FALSE;
    snapshot->work_18.loaded = FALSE;
    snapshot->work_19.loaded = FALSE;
}
