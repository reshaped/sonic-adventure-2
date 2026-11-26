/* the stage map's menu. its step drives stage_map.h's map through
 * stage_map_work, which menu_works.work_02 points at */
#include <sg_xpt.h>
#include <shinobi.h>

#include "advertise/menu_snapshot.h"
#include "advertise/menu_textures.h"
#include "advertise/menus.h"
#include "advertise/new_menu_screen.h"
#include "advertise/stage_map.h"
#include "advertise/stage_map_cursor.h"
#include "advertise/stage_map_menu.h"
#include "main.h"
#include "player/player.h"
#include "save/save.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "sound/voice_set.h"
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/sequence.h"

struct stage_map_work stage_map_work;

/* hide the stage map. nothing calls stage_map_menu_hide, and no header
 * declares it. it is external only because SHC leaves out a static
 * function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void stage_map_menu_hide(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* menu_control.step. each wait below lasts until a step of the map's task
 * moves nothing */
enum {
    STEP_SHOW, /* show the map, with its backdrop */
    STEP_OPEN, /* wait, then show the grid and the summary */
    STEP_GRID, /* move the cursor, and choose a stage */
    STEP_MISSIONS, /* choose a mission */
    STEP_BACK, /* wait, then go back to the new menu */
    STEP_START /* wait, then ready the game and end the menus */
};

/* voice_set_play's sets as the grid opens, and as Chao World's missions
 * open */
#define GRID_VOICES 3
#define CHAO_VOICES 2

void stage_map_menu_start(void)
{
    menu_textures_select(MENU_TEXTURES_STAGE_MAP);
    menu_control.step = STEP_SHOW;
    music_play_name("advsng_4.adx");
    /* Chao World's choices name the mechs, and stage_map_start plays each
     * as its pilot. so a visit by either counts for both */
    /* @hack spelling: each mark ORed into itself, where |= would do */
    save_data.played[PLAYER_TAILS] =
        save_data.played[PLAYER_TAILS] | save_data.played[PLAYER_TAILS_MECH];
    save_data.played[PLAYER_TAILS_MECH] =
        save_data.played[PLAYER_TAILS] | save_data.played[PLAYER_TAILS_MECH];
    save_data.played[PLAYER_EGGMAN] =
        save_data.played[PLAYER_EGGMAN] | save_data.played[PLAYER_EGGMAN_MECH];
    save_data.played[PLAYER_EGGMAN_MECH] =
        save_data.played[PLAYER_EGGMAN] | save_data.played[PLAYER_EGGMAN_MECH];
}

void stage_map_menu_hide(void)
{
    menu_works.work_02->shown = FALSE;
    menu_works.work_02->unk_04 = 0;
}

Bool stage_map_menu_step(void)
{
    struct stage_map_work *menu;
    /* @hack spelling: the work in a second local, where menu would do */
    struct stage_map_work *selection;
    /* @hack spelling: the cancel buttons through a local pointer, where their
     * own name would do */
    Uint32 *cancel = &pad_cancel_buttons;

    /* @hack spelling: the work read into its locals inside the switch's
     * test, where a statement of its own would do */
    switch (selection = menu = menu_works.work_02, menu_control.step) {
    case STEP_SHOW:
        menu->shown = TRUE;
        /* @hack spelling: the work read again through menu_works, here and
         * after, where menu would do */
        menu_works.work_02->backdrop.shown = TRUE;
        menu_control.step = STEP_OPEN;
        break;
    case STEP_OPEN:
        if (menu->moving) {
            break;
        }
        menu_control.step = STEP_GRID;
        menu_works.work_02->grid.shown = TRUE;
        menu_works.work_02->grid.bright = TRUE;
        menu_works.work_02->summary.shown = TRUE;
        menu_works.work_02->summary.bright = 1;
        voice_set_play(GRID_VOICES);
        break;
    case STEP_GRID:
        if (menu->backdrop.moving == FALSE) {
            /* @bug logic: Choose pressed with Cancel is taken too, and the
             * step it sets replaces STEP_BACK. the menu then takes the pad
             * on the missions with the map hidden, or Green Hill starts.
             * the cancel's branch at 0x8c506b6a falls through to the pad's
             * tests at 0x8c506b7e. the missions' own cancel ends its step,
             * at 0x8c506cf8 */
            if (pad_press_repeat[0] & *cancel) {
                menu->shown = FALSE;
                menu_works.work_02->unk_04 = 0;
                menu_control.step = STEP_BACK;
                sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            }
            if (pad_press_repeat[0] & PDD_DGT_KU) {
                if (stage_map_cursor_move(STAGE_MAP_UP)) {
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
            } else if (pad_press_repeat[0] & PDD_DGT_KD) {
                if (stage_map_cursor_move(STAGE_MAP_DOWN)) {
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
            } else if (pad_press_repeat[0] & PDD_DGT_KL) {
                if (stage_map_cursor_move(STAGE_MAP_LEFT)) {
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
            } else if (pad_press_repeat[0] & PDD_DGT_KR) {
                if (stage_map_cursor_move(STAGE_MAP_RIGHT)) {
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
            } else if (pad_press_repeat[0] & pad_choose_buttons) {
                if (stage_map_cell_lit(menu_works.work_02->cursor)) {
                    sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                    if (stage_map_cursor_stage() == STAGE_GREEN_HILL) {
                        menu_snapshot_save();
                        menu_resume_target = MENU_RESUME_STAGE_MAP;
                        menu_works.work_02->shown = FALSE;
                        menu_works.work_02->unk_04 = 0;
                        menu_control.step = STEP_START;
                    } else {
                        menu_works.work_02->summary.shown = FALSE;
                        menu_works.work_02->grid.bright = FALSE;
                        menu_works.work_02->missions.shown = TRUE;
                        menu_works.work_02->missions.bright = TRUE;
                        menu_works.work_02->mission = MISSION_FIRST;
                        menu_control.step = STEP_MISSIONS;
                        if (stage_map_cursor_stage() == STAGE_CHAO) {
                            voice_set_play(CHAO_VOICES);
                        }
                    }
                } else {
                    sound_play(MENU_SOUND_REFUSED, NULL, 0, 0);
                }
            }
        }
        break;
    case STEP_MISSIONS:
        if (pad_press_repeat[0] & *cancel) {
            menu_control.step = STEP_GRID;
            menu_works.work_02->summary.shown = TRUE;
            menu_works.work_02->grid.bright = TRUE;
            menu_works.work_02->missions.shown = FALSE;
            menu_works.work_02->missions.bright = FALSE;
            menu_works.work_02->mission = MISSION_FIRST;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
        } else if (pad_press_repeat[0] & PDD_DGT_KL) {
            if (selection->mission) {
                selection->mission--;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if (pad_press_repeat[0] & PDD_DGT_KR) {
            if (selection->mission < selection->mission_count - 1) {
                selection->mission++;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else {
            /* @hack spelling: the choice's test as a break out of the last
             * else, where an else if would do */
            if (!(pad_press_repeat[0] & pad_choose_buttons)) {
                break;
            }
            switch (stage_map_mission_open()) {
            /* @hack spelling: STAGE_MAP_CHAO's case before STAGE_MAP_KART's,
             * where their order would do */
            case STAGE_MAP_STAGE:
            case STAGE_MAP_CHAO:
            case STAGE_MAP_KART:
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                /* @hack spelling: the work by its own name, where menu would
                 * do */
                stage_map_work.missions.bright = FALSE;
                menu_snapshot_save();
                menu_resume_target = MENU_RESUME_STAGE_MAP;
                menu_works.work_02->shown = FALSE;
                menu_works.work_02->unk_04 = 0;
                menu_control.step = STEP_START;
                break;
            case STAGE_MAP_LOCKED:
                sound_play(MENU_SOUND_REFUSED, NULL, 0, 0);
                break;
            }
        }
        break;
    /* @hack spelling: STEP_START's case before STEP_BACK's, where the steps'
     * order would do */
    case STEP_START:
        if (menu->moving) {
            break;
        }
        switch (stage_map_start()) {
        case STAGE_MAP_STAGE:
        case STAGE_MAP_KART:
        case STAGE_MAP_CHAO:
            sequence_start(SEQUENCE_NONE);
            return TRUE;
        /* @hack redundant: a default that only breaks */
        default:
            break;
        }
        break;
    case STEP_BACK:
        if (menu->moving) {
            break;
        }
        new_menu_screen_start();
        new_menu_screen_reopen(NEW_MENU_REOPEN_SINGLE);
        menu_control.mode = MENU_MODE_NEW_MENU;
        break;
    }
    return FALSE;
}

void stage_map_menu_resume(void)
{
    menu_snapshot_restore();
    stage_map_menu_start();
    menu_control.mode = MENU_MODE_STAGE_MAP;
    menu_control.step = STEP_SHOW;
    menu_works.work_02->missions.shown = FALSE;
}
