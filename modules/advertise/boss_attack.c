/* the boss attack: MENU_MODE_BOSS_ATTACK's menu, whose courses each play a
 * sequence of bosses. boss_attack_panel.h's panel draws it */
#include <shinobi.h>
#include <sg_xpt.h>

#include "advertise/boss_attack.h"
#include "advertise/boss_attack_panel.h"
#include "advertise/emblem_results_menu.h"
#include "advertise/menu_snapshot.h"
#include "advertise/menu_textures.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/new_menu_screen.h"
#include "hud/hud.h"
#include "main.h"
#include "save/save.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/sequence.h"

struct boss_attack_work boss_attack_work;

/* menu_control.step, while the boss attack runs */
enum boss_attack_step {
    STEP_SHOW,
    STEP_FADE_IN,
    STEP_CHOOSE,
    STEP_BACK, /* wait for the panel to fade out, then go back */
    STEP_START /* wait for the panel to fade out, then start the course */
};

/* only boss_attack_step calls it. it isn't static, since SHC would expand
 * that call and leave the function out */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* fade the panel out */
void boss_attack_hide(void);
/* NOLINTEND(misc-use-internal-linkage) */

void boss_attack_start(void)
{
    menu_textures_select(MENU_TEXTURES_BOSS_ATTACK);
    menu_control.step = STEP_SHOW;
    music_play_name("advsng_4.adx");
}

void boss_attack_hide(void)
{
    menu_works.work_15->shown = FALSE;
    menu_works.work_15->unk_04 = 0;
}

Bool boss_attack_step(void)
{
    struct boss_attack_work *work;

    /* @hack spelling: the work read in the switch's expression, where a
     * statement of its own before the switch would do */
    switch (work = menu_works.work_15, menu_control.step) {
    case STEP_SHOW:
        work->shown = TRUE;
        menu_control.step = STEP_FADE_IN;
        break;
    case STEP_FADE_IN:
        /* @hack spelling: the break while the panel fades, here and in
         * STEP_BACK, where an if around what follows would do */
        if (work->fading) {
            break;
        }
        menu_control.step = STEP_CHOOSE;
        break;
    case STEP_CHOOSE:
        /* @hack type: the press and the buttons each cast to an int, here
         * and for Choose, where their Uint32s would do */
        if ((int)pad_press_repeat[0] & (int)pad_cancel_buttons) {
            work->shown = FALSE;
            /* @hack spelling: menu_works' work read again for unk_04, where
             * work would do */
            menu_works.work_15->unk_04 = 0;
            menu_control.step = STEP_BACK;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
        } else if (pad_press_repeat[0] & PDD_DGT_KU) {
            if (work->course) {
                work->course--;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if (pad_press_repeat[0] & PDD_DGT_KD) {
            if (work->course == BOSS_COURSE_DARK) {
                if (save_data
                        .awards[BOSS_ATTACK_FIRST_AWARD + BOSS_COURSE_ALL]) {
                    work->course++;
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
            } else if (work->course == BOSS_COURSE_HERO) {
                work->course++;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if ((int)pad_press_repeat[0] & (int)pad_choose_buttons) {
            if (save_data.awards[work->course + BOSS_ATTACK_FIRST_AWARD]) {
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                menu_resume_target = MENU_RESUME_BOSS_ATTACK;
                menu_snapshot_save();
                boss_attack_hide();
                menu_control.step = STEP_START;
                /* @hack spelling: menu_works' work read again for the
                 * course, here and below, where work would do */
                boss_times_clear(menu_works.work_15->course);
            } else {
                sound_play(MENU_SOUND_REFUSED, NULL, 0, 0);
            }
        }
        break;
    /* @hack spelling: STEP_START's case before STEP_BACK's, where their
     * order would do */
    case STEP_START:
        if (!work->fading) {
            game_state.split_screen = 0;
            game_state.two_players = 0;
            game_state.demo = 0;
            game_state.mission = MISSION_FIRST;
            /* menu_play_sequence overwrites it with GAME_MODE_SEQUENCE */
            game_state.next_mode = GAME_MODE_STAGE;
            menu_play_sequence(menu_works.work_15->course +
                               SEQUENCE_HERO_BOSSES);
            return TRUE;
        }
        break;
    case STEP_BACK:
        if (work->fading) {
            break;
        }
        if (work->from_emblems) {
            emblem_results_menu_start();
            menu_control.mode = MENU_MODE_EMBLEM_RESULTS;
            /* @hack spelling: menu_works' work read again, where work would
             * do */
            menu_works.work_15->from_emblems = FALSE;
        } else {
            new_menu_screen_start();
            new_menu_screen_reopen(NEW_MENU_REOPEN_SINGLE);
            menu_control.mode = MENU_MODE_NEW_MENU;
        }
        break;
    }
    return FALSE;
}

void boss_attack_resume(void)
{
    menu_snapshot_restore();
    boss_attack_start();
    menu_control.mode = MENU_MODE_BOSS_ATTACK;
    /* @hack redundant: the step set again, as boss_attack_start set it */
    menu_control.step = STEP_SHOW;
}
