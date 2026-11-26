/* the emblem results' menu. it drives the screen through
 * menu_works.work_19, emblem_results.h's emblem_results_work */
#include <shinobi.h>
#include <sg_xpt.h>

#include "advertise/boss_attack.h"
#include "advertise/emblem_results.h"
#include "advertise/emblem_results_menu.h"
#include "advertise/kart_race.h"
#include "advertise/menu_snapshot.h"
#include "advertise/menu_textures.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/new_menu_screen.h"
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

struct emblem_results_work emblem_results_work;

/* close the screen. nothing calls emblem_results_menu_close, and no header
 * declares it. it is external only because SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void emblem_results_menu_close(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* menu_control.step. STEP_OPENING and each step after STEP_CHOOSE wait for
 * the screen's fades to stop */
enum emblem_results_menu_step {
    STEP_OPEN = 0,
    STEP_OPENING = 1,
    STEP_CHOOSE = 2,
    STEP_BACK = 3,
    STEP_STAGE = 5,
    STEP_KART = 6,
    STEP_BOSS = 7,
    STEP_STORY = 8,
    STEP_CHAO = 9
};

#define OPEN_VOICES 19 /* voice_set_play's set */

void emblem_results_menu_start(void)
{
    menu_textures_select(MENU_TEXTURES_EMBLEM_RESULTS);
    menu_control.step = STEP_OPEN;
    emblem_results_gather();
    music_play_name("advsng_3.adx");
}

void emblem_results_menu_close(void)
{
    menu_works.work_19->open = FALSE;
    menu_works.work_19->unk_04 = 0;
}

Bool emblem_results_menu_step(void)
{
    switch (menu_control.step) {
    case STEP_OPEN:
        menu_works.work_19->open = TRUE;
        menu_works.work_19->title.shown = TRUE;
        menu_works.work_19->page_name.shown = TRUE;
        menu_works.work_19->time_window.shown = FALSE;
        menu_works.work_19->totals.shown = TRUE;
        menu_works.work_19->lines.shown = TRUE;
        menu_works.work_19->boxes.shown = TRUE;
        menu_works.work_19->message = EMBLEM_RESULTS_NO_MESSAGE;
        menu_works.work_19->ranking = FALSE;
        menu_control.step = STEP_OPENING;
        break;
    case STEP_OPENING:
        if (menu_works.work_19->fading) {
            break;
        }
        menu_control.step = STEP_CHOOSE;
        voice_set_play(OPEN_VOICES);
        break;
    case STEP_CHOOSE:
        emblem_results_pick_message();
        if (pad_press_repeat[0] & pad_cancel_buttons) {
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            menu_works.work_19->open = FALSE;
            menu_works.work_19->unk_04 = 0;
            menu_control.step = STEP_BACK;
            menu_works.work_19->message = EMBLEM_RESULTS_NO_MESSAGE;
        } else if (pad_press_repeat[0] & PDD_DGT_KL) {
            menu_works.work_19->line = 0;
            menu_works.work_19->page--;
            sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            if (menu_works.work_19->page == EMBLEM_RESULTS_LAST &&
                (save_data.awards[SAVE_AWARD_HERO_STORY] == 0 ||
                 save_data.awards[SAVE_AWARD_DARK_STORY] == 0)) {
                menu_works.work_19->page--;
            } else if (menu_works.work_19->page < 0) {
                menu_works.work_19->page = EMBLEM_RESULTS_OTHERS;
            }
        } else if (pad_press_repeat[0] & PDD_DGT_KR) {
            menu_works.work_19->line = 0;
            menu_works.work_19->page++;
            sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            if (menu_works.work_19->page == EMBLEM_RESULTS_LAST &&
                (save_data.awards[SAVE_AWARD_HERO_STORY] == 0 ||
                 save_data.awards[SAVE_AWARD_DARK_STORY] == 0)) {
                menu_works.work_19->page++;
            } else if (menu_works.work_19->page > EMBLEM_RESULTS_OTHERS) {
                menu_works.work_19->page = EMBLEM_RESULTS_HERO;
            }
        } else if (pad_press_repeat[0] & PDD_DGT_KU) {
            if (menu_works.work_19->line != 0) {
                menu_works.work_19->line--;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if (pad_press_repeat[0] & PDD_DGT_KD) {
            if (menu_works.work_19->line < menu_works.work_19->line_count - 1) {
                menu_works.work_19->line++;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if (pad_press_repeat[0] & pad_choose_buttons) {
            switch (emblem_results_choose()) {
            case EMBLEM_RESULTS_PLAY_STAGE:
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                menu_snapshot_save();
                menu_resume_target = MENU_RESUME_EMBLEM_RESULTS;
                menu_works.work_19->open = FALSE;
                menu_works.work_19->unk_04 = 0;
                menu_control.step = STEP_STAGE;
                /* @bug logic: plays the choose sound a second time, where
                 * each other choice plays it once (the calls at 0x8c51a7f4
                 * and 0x8c51a8da) */
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                break;
            case EMBLEM_RESULTS_PLAY_CHAO:
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                menu_snapshot_save();
                menu_resume_target = MENU_RESUME_EMBLEM_RESULTS;
                menu_works.work_19->open = FALSE;
                menu_works.work_19->unk_04 = 0;
                menu_control.step = STEP_CHAO;
                break;
            case EMBLEM_RESULTS_PLAY_KART:
                if (save_data.cart_stage[SAVE_CART_OTHERS]
                        .ranks[MISSION_FIRST] ||
                    save_data.cart_stage[SAVE_CART_ROUGE]
                        .ranks[MISSION_FIRST]) {
                    sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                    menu_works.work_19->open = FALSE;
                    menu_works.work_19->unk_04 = 0;
                    menu_control.step = STEP_KART;
                    menu_works.work_13->from_emblems = TRUE;
                    menu_works.work_13->two_players = FALSE;
                } else {
                    sound_play(MENU_SOUND_REFUSED, NULL, 0, 0);
                }
                break;
            case EMBLEM_RESULTS_PLAY_BOSS:
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                menu_works.work_19->open = FALSE;
                menu_works.work_19->unk_04 = 0;
                menu_control.step = STEP_BOSS;
                menu_works.work_15->from_emblems = TRUE;
                break;
            case EMBLEM_RESULTS_PLAY_STORY:
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                menu_works.work_19->open = FALSE;
                menu_works.work_19->unk_04 = 0;
                menu_control.step = STEP_STORY;
                menu_works.work_03->from_emblems = TRUE;
                break;
            default:
                sound_play(MENU_SOUND_REFUSED, NULL, 0, 0);
                break;
            }
        }
        break;
    case STEP_BACK:
        if (menu_works.work_19->fading) {
            break;
        }
        new_menu_screen_start();
        new_menu_screen_reopen(NEW_MENU_REOPEN_EXTRAS);
        menu_control.mode = MENU_MODE_NEW_MENU;
        break;
    case STEP_KART:
        if (menu_works.work_19->fading) {
            break;
        }
        kart_race_start();
        menu_control.mode = MENU_MODE_KART_RACE;
        break;
    case STEP_BOSS:
        if (menu_works.work_19->fading) {
            break;
        }
        boss_attack_start();
        menu_control.mode = MENU_MODE_BOSS_ATTACK;
        break;
    case STEP_STAGE:
        if (menu_works.work_19->fading) {
            break;
        }
        /* the menus end on this step, so the boss attack never runs.
         * boss_attack_start still selects its textures, plays its song and
         * sets menu_control.step */
        boss_attack_start();
        menu_control.mode = MENU_MODE_BOSS_ATTACK;
        sequence_start(SEQUENCE_NONE);
        return TRUE;
        /* @hack redundant: a break after the return, here and in STEP_STORY,
         * which no path reaches */
        break;
    case STEP_STORY:
        if (menu_works.work_19->fading) {
            break;
        }
        switch (menu_works.work_19->page) {
        case EMBLEM_RESULTS_HERO:
            menu_start_sequence_at(SEQUENCE_HERO,
                                   save_data.progress[SEQUENCE_HERO - 1]);
            menu_to_sequence();
            break;
        case EMBLEM_RESULTS_DARK:
            menu_start_sequence_at(SEQUENCE_DARK,
                                   save_data.progress[SEQUENCE_DARK - 1]);
            menu_to_sequence();
            break;
        case EMBLEM_RESULTS_LAST:
            menu_start_sequence_at(SEQUENCE_LAST,
                                   save_data.progress[SEQUENCE_LAST - 1]);
            menu_to_sequence();
            break;
        }
        return TRUE;
        break;
    case STEP_CHAO:
        if (menu_works.work_19->fading) {
            break;
        }
        sequence_start(SEQUENCE_NONE);
        stage_set_current(STAGE_CHAO);
        stage_set_character(0, PLAYER_SONIC);
        game_state.split_screen = 0;
        game_state.two_players = 0;
        game_state.demo = 0;
        game_state.next_mode = GAME_MODE_STAGE;
        game_state.chao_race_menu = CHAO_RACE_MENU_EMBLEMS;
        return TRUE;
    }
    return FALSE;
}

void emblem_results_menu_resume(void)
{
    menu_snapshot_restore();
    emblem_results_menu_start();
    menu_control.mode = MENU_MODE_EMBLEM_RESULTS;
    menu_control.step = STEP_OPENING;
}
