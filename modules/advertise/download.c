/* the download menu. its panel is the kart race menu's, menu_works.work_13
 * of kart_race_panel.h */
#include <sg_xpt.h>
#include <shinobi.h>

#include "advertise/download.h"
#include "advertise/download_panel.h"
#include "advertise/kart_race.h"
#include "advertise/kart_race_panel.h"
#include "advertise/menu_snapshot.h"
#include "advertise/menu_textures.h"
#include "advertise/menus.h"
#include "advertise/new_menu_screen.h"
#include "main.h"
#include "player/player.h"
#include "save/save.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "sound/voice_set.h"
#include "stage/stage.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/sequence.h"

struct download_panel_work download_panel_work;

/* hide the panel. nothing calls download_hide, and no header declares it.
 * it is external only because SHC leaves out a static function that nothing
 * calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void download_hide(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* menu_control.step. each wait below lasts until the panel's fades stop */
enum download_step {
    STEP_SHOW,
    STEP_READ, /* wait, then find the download files */
    STEP_LIST_IN, /* wait, then show the list's hint */
    STEP_LIST, /* choose a file */
    STEP_QUESTION, /* ask whether to play the file */
    STEP_QUESTION_IN, /* wait, then ask */
    STEP_NONE_FOUND, /* say that no file was found, and leave at a press */
    STEP_LEAVE, /* wait, then go back to the new menu */
    STEP_RACE /* wait, then end the menus for the race */
};

#define START_VOICES 21 /* voice_set_play's set */

void download_start(void)
{
    menu_textures_select(MENU_TEXTURES_DOWNLOAD);
    menu_control.step = STEP_SHOW;
    music_play_name("advsng_3.adx");
    voice_set_play(START_VOICES);
}

void download_hide(void)
{
    menu_works.work_13->shown = FALSE;
    menu_works.work_13->unk_04 = 0;
}

Bool download_step(void)
{
    struct download_entry *entry;
    Sint32 i;
    Bool ranked;
    /* @hack spelling: the masks through pointers, where their own names
     * would do */
    /* @hack spelling: cancel's pointer declared before choose's, where
     * either order would do */
    Uint32 *cancel = &pad_cancel_buttons;
    Uint32 *choose = &pad_choose_buttons;

    /* @hack redundant: a block no path reaches, which reads the two
     * pointers twice */
    if (0) {
        if (choose && cancel) {
        }
        if (choose && cancel) {
        }
    }
    switch (menu_control.step) {
    case STEP_SHOW:
        menu_works.work_13->shown = TRUE;
        menu_works.work_13->backdrop.shown = TRUE;
        menu_control.step = STEP_READ;
        /* @hack spelling: download_backdrop, hint_shown and reading set
         * through the name kart_race_work throughout download_step, where
         * menu_works.work_13 would do */
        kart_race_work.download_backdrop = TRUE;
        /* note the device in each drive's socket. the steps below start the
         * menu again once one changes */
        save_check_sockets();
        break;
    case STEP_READ:
        if (menu_works.work_13->fading != 0) {
            break;
        }
        kart_race_work.hint_shown = TRUE;
        /* @bug logic: the hint never says that the memory cards are being
         * read. this call of download_step sets reading and clears it again
         * after the read, before the panel draws. the stores are at
         * 0x8c515adc and 0x8c515b1e. the kart race menu sets reading while
         * its courses fade out, before its read */
        kart_race_work.reading = TRUE;
        kart_race_panel_set_list(
            kart_race_downloads,
            save_downloads_find(kart_race_downloads,
                                DOWNLOAD_FIND_KINDS_1_TO_4));
        if (menu_works.work_13->entry_count != 0) {
            menu_works.work_13->list.shown = TRUE;
            menu_control.step = STEP_LIST_IN;
        } else {
            menu_works.work_13->question.shown = TRUE;
            menu_works.work_13->none_found = TRUE;
            menu_control.step = STEP_NONE_FOUND;
            sound_play(MENU_SOUND_NONE_FOUND, NULL, 0, 0);
        }
        kart_race_work.reading = FALSE;
        break;
    case STEP_LIST_IN:
        if (menu_works.work_13->fading != 0) {
            break;
        }
        menu_control.step = STEP_LIST;
        menu_works.work_13->hint_shown = TRUE;
        break;
    case STEP_LIST:
        if ((pad_press_repeat[0] & *choose) != 0) {
            menu_works.work_13->question.shown = TRUE;
            menu_works.work_13->list.shown = FALSE;
            menu_works.work_13->hint_shown = FALSE;
            menu_control.step = STEP_QUESTION_IN;
            sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
        } else if ((pad_press_repeat[0] & *cancel) != 0) {
            menu_works.work_13->shown = FALSE;
            menu_works.work_13->unk_04 = 0;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            menu_control.step = STEP_LEAVE;
        } else if ((pad_press_repeat[0] & PDD_DGT_KU) != 0) {
            if (menu_works.work_13->entry != 0) {
                menu_works.work_13->entry--;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if ((pad_press_repeat[0] & PDD_DGT_KD) != 0) {
            if (menu_works.work_13->entry <
                menu_works.work_13->entry_count - 1) {
                menu_works.work_13->entry++;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if (save_check_sockets() != 0) {
            menu_works.work_13->shown = FALSE;
            menu_works.work_13->unk_04 = 0;
            menu_control.step = STEP_SHOW;
        }
        break;
    case STEP_QUESTION_IN:
        if (menu_works.work_13->fading != 0) {
            break;
        }
        menu_control.step = STEP_QUESTION;
        menu_works.work_13->hint_shown = TRUE;
        break;
    case STEP_QUESTION:
        if ((pad_press_repeat[0] & *cancel) != 0) {
            menu_works.work_13->question.shown = FALSE;
            menu_works.work_13->list.shown = TRUE;
            menu_works.work_13->hint_shown = FALSE;
            menu_control.step = STEP_LIST_IN;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
        } else if ((pad_press_repeat[0] & PDD_DGT_KL) != 0) {
            if (menu_works.work_13->choice != KART_RACE_YES) {
                menu_works.work_13->choice = KART_RACE_YES;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if ((pad_press_repeat[0] & PDD_DGT_KR) != 0) {
            if (menu_works.work_13->choice == KART_RACE_YES) {
                menu_works.work_13->choice = KART_RACE_NO;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if ((pad_press_repeat[0] & *choose) != 0) {
            if (menu_works.work_13->choice != KART_RACE_YES) {
                menu_works.work_13->question.shown = FALSE;
                menu_works.work_13->list.shown = TRUE;
                menu_works.work_13->hint_shown = FALSE;
                /* @bug logic: the list comes back from the answer no
                 * without its hint. the answer goes straight to STEP_LIST,
                 * but a cancel goes through STEP_LIST_IN, which shows the
                 * hint again. the stores are at 0x8c515d2e and 0x8c515c88 */
                menu_control.step = STEP_LIST;
                sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            } else {
                entry = &kart_race_downloads[menu_works.work_13->entry];
                ranked = TRUE;
                /* a value above 0 names a stage, and the file needs a rank
                 * in that stage's first mission. STAGE_CART_1's ranks are in
                 * the two records of save_data.cart_stage, and a rank in
                 * either counts */
                for (i = 0; i < DOWNLOAD_VALUES; i++) {
                    if (entry->values[i] > 0) {
                        /* @hack spelling: a switch of one case and a
                         * default, where an if would do */
                        switch (entry->values[i]) {
                        case STAGE_CART_1:
                            if (!save_data.cart_stage[SAVE_CART_OTHERS]
                                     .ranks[MISSION_FIRST] &&
                                !save_data.cart_stage[SAVE_CART_ROUGE]
                                     .ranks[MISSION_FIRST]) {
                                ranked = FALSE;
                            }
                            break;
                        default:
                            if (save_data.stages[entry->values[i]]
                                    .ranks[MISSION_FIRST] == SAVE_RANK_NONE) {
                                ranked = FALSE;
                            }
                        }
                    }
                }
                if (ranked) {
                    sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                    switch (entry->kind) {
                    /* @hack redundant: cases of kinds 1 and 2 that only
                     * break, which the switch's end covers */
                    case DOWNLOAD_KIND_1:
                        break;
                    case DOWNLOAD_KIND_2:
                        break;
                    case DOWNLOAD_KIND_3:
                        stage_set_current(STAGE_CART_2);
                        stage_set_character(0, PLAYER_SONIC);
                        game_state.split_screen = 0;
                        game_state.two_players = 0;
                        game_state.demo = 0;
                        game_state.mission = MISSION_FIRST;
                        game_state.next_mode = GAME_MODE_STAGE;
                        game_state.kart_course = KART_RACE_DOWNLOADED;
                        download_chosen = *entry;
                        save_download_set_drive(entry->drive);
                        save_download_set_slot(entry->slot);
                        menu_control.step = STEP_RACE;
                        menu_works.work_13->shown = FALSE;
                        menu_works.work_13->unk_04 = 0;
                        game_state.kart_character_karts = 1;
                        menu_works.work_17->shown = FALSE;
                        break;
                    case DOWNLOAD_KIND_4:
                        stage_set_current(STAGE_CART_2);
                        stage_set_character(0, PLAYER_SONIC);
                        game_state.split_screen = 0;
                        game_state.two_players = 0;
                        game_state.demo = 0;
                        game_state.mission = MISSION_FIRST;
                        game_state.next_mode = GAME_MODE_STAGE;
                        game_state.kart_course = KART_RACE_DOWNLOADED;
                        download_chosen = *entry;
                        save_download_set_drive(entry->drive);
                        save_download_set_slot(entry->slot);
                        menu_control.step = STEP_RACE;
                        menu_works.work_13->shown = FALSE;
                        menu_works.work_13->unk_04 = 0;
                        game_state.kart_character_karts = 0;
                        menu_works.work_17->shown = FALSE;
                        break;
                    }
                } else {
                    sound_play(MENU_SOUND_REFUSED, NULL, 0, 0);
                }
            }
        } else if (save_check_sockets() != 0) {
            menu_works.work_13->shown = FALSE;
            menu_works.work_13->unk_04 = 0;
            menu_control.step = STEP_SHOW;
        }
        break;
    case STEP_NONE_FOUND:
        if ((pad_press_repeat[0] & *choose) != 0 ||
            (pad_press_repeat[0] & pad_cancel_buttons) != 0) {
            menu_works.work_13->shown = FALSE;
            menu_works.work_13->unk_04 = 0;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            menu_control.step = STEP_LEAVE;
        } else if (save_check_sockets() != 0) {
            menu_works.work_13->shown = FALSE;
            menu_works.work_13->unk_04 = 0;
            menu_control.step = STEP_SHOW;
        }
        break;
    case STEP_LEAVE:
        if (menu_works.work_13->fading != 0) {
            break;
        }
        new_menu_screen_start();
        new_menu_screen_reopen(NEW_MENU_REOPEN_EXTRAS);
        menu_control.mode = MENU_MODE_NEW_MENU;
        break;
    case STEP_RACE:
        if (menu_works.work_13->fading != 0) {
            break;
        }
        menu_snapshot_save();
        menu_resume_target = MENU_RESUME_DOWNLOAD;
        sequence_start(SEQUENCE_NONE);
        save_download_read(DEMO_AREA);
        return TRUE;
    }
    return FALSE;
}

void download_resume(void)
{
    menu_snapshot_restore();
    /* @bug logic: starts the kart race's music, advsng_4.adx, through
     * kart_race_start (0x8c51602a), where download_start plays the download
     * menu's advsng_3.adx */
    kart_race_start();
    menu_control.mode = MENU_MODE_DOWNLOAD;
    menu_control.step = STEP_SHOW;
    menu_works.work_17->shown = TRUE;
}
