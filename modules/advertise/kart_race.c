/* the kart race menu. its panel is menu_works.work_13 of
 * kart_race_panel.h, which the download menu shows too */
#include <sg_xpt.h>
#include <shinobi.h>

#include "advertise/emblem_results_menu.h"
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

/* hide the panel. nothing calls kart_race_hide, and no header declares it.
 * it is external only because SHC leaves out a static function that nothing
 * calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void kart_race_hide(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* menu_control.step. each wait below lasts until the panel's fades stop */
enum kart_race_step {
    STEP_SHOW,
    STEP_DRIVERS_IN, /* wait, then show the drivers */
    STEP_PREVIEW_IN, /* wait, then show the karts */
    STEP_DRIVERS, /* pick the drivers */
    STEP_COURSES_IN, /* wait, then show the courses' hint */
    STEP_COURSES, /* choose a course */
    STEP_READ, /* wait, then find the download files */
    /* wait, then note the device in each drive's socket for STEP_LIST */
    STEP_LIST_IN,
    STEP_LIST, /* choose a file, or go back to the courses */
    STEP_NONE_FOUND, /* say that no file was found, and go back at a press */
    STEP_LEAVE, /* wait, then leave the menu */
    STEP_RACE, /* wait, then end the menus for the race */
    STEP_RESUME /* count down, then go on at the course list */
};

/* voice_set_play's sets */
#define DRIVERS_VOICES 20
#define READ_VOICES 25
/* STEP_RESUME counts kart_race_resume_wait down from RESUME_WAIT, and goes
 * on at the step that finds it below 0 */
#define RESUME_WAIT 60
/* the drivers the first and second players start on. in a race of one, the
 * second player has NO_DRIVER */
#define FIRST_DRIVER 0
#define SECOND_DRIVER 1
#define NO_DRIVER (-1)

/* @hack type: the masks read as Sint32 through their second names, where
 * pad_choose_buttons and pad_cancel_buttons would do: each test asks only
 * whether a press shares a bit with the mask */
#ifdef NONMATCHING
/* this side reads the masks by their own names. its objects don't define the
 * signed names, which only symbols.txt places */
#define KART_CHOOSE_BUTTONS pad_choose_buttons
#define KART_CANCEL_BUTTONS pad_cancel_buttons
#else
#define KART_CHOOSE_BUTTONS pad_choose_buttons_signed
#define KART_CANCEL_BUTTONS pad_cancel_buttons_signed
#endif

#define FIRST_PLAYER 0
#define SECOND_PLAYER 1
#define PLAYERS 2

/* Set after the menu leaves because the second pad disconnected. */
static Bool kart_race_pad_lost;
static Sint32 kart_race_resume_wait;
/* Per-player confirmation of the selected driver. */
static Bool kart_race_ready[PLAYERS];

/* by driver, the character stage_set_character takes */
static Sint32 driver_characters[KART_RACE_DRIVERS] = {
    PLAYER_SONIC,  PLAYER_TAILS_MECH,  PLAYER_KNUCKLES,
    PLAYER_SHADOW, PLAYER_EGGMAN_MECH, PLAYER_ROUGE,
};

static void kart_race_leave(void);

void kart_race_start(void)
{
    menu_textures_select(MENU_TEXTURES_KART_RACE);
    menu_control.step = STEP_SHOW;
    music_play_name("advsng_4.adx");
    kart_race_pad_lost = FALSE;
}

void kart_race_hide(void)
{
    menu_works.work_13->shown = FALSE;
    menu_works.work_13->unk_04 = 0;
}

Bool kart_race_step(void)
{
    /* the first player's cancel clears it. the second player's cancel leaves
     * only while it is TRUE */
    Bool may_cancel = TRUE;

    if (!pad_player_is_connected(SECOND_PLAYER) &&
        menu_works.work_13->two_players && !kart_race_pad_lost) {
        menu_works.work_13->shown = FALSE;
        menu_works.work_13->unk_04 = 0;
        menu_control.step = STEP_LEAVE;
        sound_play(MENU_SOUND_BACK, NULL, 0, 0);
        kart_race_pad_lost = TRUE;
        return FALSE;
    }
    switch (menu_control.step) {
    case STEP_SHOW:
        menu_works.work_13->shown = TRUE;
        menu_works.work_13->backdrop.shown = TRUE;
        menu_control.step = STEP_DRIVERS_IN;
        /* @hack spelling: download_backdrop, hint_shown, reading and
         * entry_count read and set through the name kart_race_work
         * throughout kart_race_step, where menu_works.work_13 would do */
        kart_race_work.download_backdrop = FALSE;
        break;
    case STEP_DRIVERS_IN:
        if (menu_works.work_13->fading) {
            break;
        }
        menu_works.work_13->drivers.shown = TRUE;
        if (menu_works.work_13->two_players) {
            menu_works.work_13->split = TRUE;
            menu_works.work_13->picks[FIRST_PLAYER].driver = FIRST_DRIVER;
            menu_works.work_13->picks[SECOND_PLAYER].driver = SECOND_DRIVER;
        } else {
            menu_works.work_13->split = FALSE;
            menu_works.work_13->picks[FIRST_PLAYER].driver = FIRST_DRIVER;
            menu_works.work_13->picks[SECOND_PLAYER].driver = NO_DRIVER;
        }
        menu_control.step = STEP_PREVIEW_IN;
        voice_set_play(DRIVERS_VOICES);
        break;
    case STEP_PREVIEW_IN:
        if (!menu_works.work_13->fading) {
            menu_works.work_13->preview_shown = TRUE;
            menu_control.step = STEP_DRIVERS;
            kart_race_ready[FIRST_PLAYER] = FALSE;
            kart_race_ready[SECOND_PLAYER] = FALSE;
            kart_race_work.hint_shown = TRUE;
        }
        break;
    case STEP_DRIVERS:
        if (!kart_race_ready[FIRST_PLAYER]) {
            if (pad_press_repeat[FIRST_PLAYER] & KART_CANCEL_BUTTONS) {
                menu_works.work_13->shown = FALSE;
                menu_works.work_13->unk_04 = 0;
                menu_control.step = STEP_LEAVE;
                sound_play(MENU_SOUND_BACK, NULL, 0, 0);
                may_cancel = FALSE;
            } else if (pad_press_repeat[FIRST_PLAYER] & PDD_DGT_KL) {
                if (menu_works.work_13->picks[FIRST_PLAYER].driver != 0) {
                    if (menu_works.work_13->picks[FIRST_PLAYER].driver - 1 ==
                        menu_works.work_13->picks[SECOND_PLAYER].driver) {
                        if (menu_works.work_13->picks[FIRST_PLAYER].driver >
                            1) {
                            menu_works.work_13->picks[FIRST_PLAYER].driver -= 2;
                            sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                        }
                    } else {
                        menu_works.work_13->picks[FIRST_PLAYER].driver--;
                        sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                    }
                }
            } else if (pad_press_repeat[FIRST_PLAYER] & PDD_DGT_KR) {
                if (menu_works.work_13->picks[FIRST_PLAYER].driver <
                    KART_RACE_DRIVERS - 1) {
                    if (menu_works.work_13->picks[FIRST_PLAYER].driver + 1 ==
                        menu_works.work_13->picks[SECOND_PLAYER].driver) {
                        if (menu_works.work_13->picks[FIRST_PLAYER].driver <
                            KART_RACE_DRIVERS - 2) {
                            menu_works.work_13->picks[FIRST_PLAYER].driver += 2;
                        }
                    } else {
                        menu_works.work_13->picks[FIRST_PLAYER].driver++;
                    }
                    /* @bug logic: the first player's move right plays the
                     * cursor's sound where the pick stays (0x8c51519e). the
                     * pick stays on the fifth driver while the second player
                     * has the sixth. the second player's move plays the sound
                     * only where the pick moves (0x8c5152e2, 0x8c5152fc) */
                    sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                }
            } else if (pad_press_repeat[FIRST_PLAYER] & KART_CHOOSE_BUTTONS) {
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                kart_race_ready[FIRST_PLAYER] = TRUE;
            }
        } else if (pad_press_repeat[FIRST_PLAYER] & KART_CANCEL_BUTTONS) {
            kart_race_ready[FIRST_PLAYER] = FALSE;
        }
        /* @hack spelling: two_players compared with TRUE, where its test
         * alone would do */
        if (menu_works.work_13->two_players == TRUE) {
            if (!kart_race_ready[SECOND_PLAYER]) {
                if ((pad_press_repeat[SECOND_PLAYER] & KART_CANCEL_BUTTONS) &&
                    may_cancel) {
                    menu_works.work_13->shown = FALSE;
                    menu_works.work_13->unk_04 = 0;
                    menu_control.step = STEP_LEAVE;
                    sound_play(MENU_SOUND_BACK, NULL, 0, 0);
                } else if (pad_press_repeat[SECOND_PLAYER] & PDD_DGT_KL) {
                    if (menu_works.work_13->picks[SECOND_PLAYER].driver != 0) {
                        if (menu_works.work_13->picks[SECOND_PLAYER].driver -
                                1 ==
                            menu_works.work_13->picks[FIRST_PLAYER].driver) {
                            if (menu_works.work_13->picks[SECOND_PLAYER]
                                    .driver > 1) {
                                menu_works.work_13->picks[SECOND_PLAYER]
                                    .driver -= 2;
                                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                            }
                        } else {
                            menu_works.work_13->picks[SECOND_PLAYER].driver--;
                            sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                        }
                    }
                } else if (pad_press_repeat[SECOND_PLAYER] & PDD_DGT_KR) {
                    if (menu_works.work_13->picks[SECOND_PLAYER].driver <
                        KART_RACE_DRIVERS - 1) {
                        if (menu_works.work_13->picks[SECOND_PLAYER].driver +
                                1 ==
                            menu_works.work_13->picks[FIRST_PLAYER].driver) {
                            if (menu_works.work_13->picks[SECOND_PLAYER]
                                    .driver < KART_RACE_DRIVERS - 2) {
                                menu_works.work_13->picks[SECOND_PLAYER]
                                    .driver += 2;
                                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                            }
                        } else {
                            menu_works.work_13->picks[SECOND_PLAYER].driver++;
                            sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
                        }
                    }
                } else if (pad_press_repeat[SECOND_PLAYER] &
                           KART_CHOOSE_BUTTONS) {
                    sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                    kart_race_ready[SECOND_PLAYER] = TRUE;
                }
            } else if (pad_press_repeat[SECOND_PLAYER] & KART_CANCEL_BUTTONS) {
                kart_race_ready[SECOND_PLAYER] = FALSE;
            }
        } else {
            kart_race_ready[SECOND_PLAYER] = TRUE;
        }
        /* @hack spelling: the ready flags compared with TRUE, where their
         * tests alone would do */
        if (kart_race_ready[FIRST_PLAYER] == TRUE &&
            kart_race_ready[SECOND_PLAYER] == TRUE) {
            menu_control.step = STEP_COURSES_IN;
            kart_race_work.hint_shown = FALSE;
            menu_works.work_13->drivers.shown = FALSE;
            menu_works.work_13->courses.shown = TRUE;
        }
        break;
    case STEP_COURSES_IN:
        if (menu_works.work_13->fading) {
            break;
        }
        menu_control.step = STEP_COURSES;
        kart_race_work.hint_shown = TRUE;
        break;
    case STEP_COURSES:
        if ((pad_press_repeat[FIRST_PLAYER] & KART_CANCEL_BUTTONS) ||
            ((pad_press_repeat[SECOND_PLAYER] & KART_CANCEL_BUTTONS) &&
             menu_works.work_13->two_players)) {
            menu_control.step = STEP_PREVIEW_IN;
            menu_works.work_13->drivers.shown = TRUE;
            menu_works.work_13->courses.shown = FALSE;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            kart_race_work.hint_shown = FALSE;
        } else if ((pad_press_repeat[FIRST_PLAYER] & PDD_DGT_KU) ||
                   ((pad_press_repeat[SECOND_PLAYER] & PDD_DGT_KU) &&
                    menu_works.work_13->two_players)) {
            if (menu_works.work_13->course != 0) {
                menu_works.work_13->course--;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if ((pad_press_repeat[FIRST_PLAYER] & PDD_DGT_KD) ||
                   ((pad_press_repeat[SECOND_PLAYER] & PDD_DGT_KD) &&
                    menu_works.work_13->two_players)) {
            if (menu_works.work_13->course < KART_RACE_DOWNLOADED) {
                menu_works.work_13->course++;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if ((pad_press_repeat[FIRST_PLAYER] & KART_CHOOSE_BUTTONS) ||
                   ((pad_press_repeat[SECOND_PLAYER] & KART_CHOOSE_BUTTONS) &&
                    menu_works.work_13->two_players)) {
            kart_race_work.hint_shown = FALSE;
            switch (menu_works.work_13->course) {
            case 0:
            case 1:
            case 2:
                sound_play(MENU_SOUND_RACE, NULL, 0, 0);
                menu_snapshot_save();
                menu_resume_target = MENU_RESUME_KART_RACE;
                menu_works.work_13->shown = FALSE;
                menu_works.work_13->unk_04 = 0;
                menu_control.step = STEP_RACE;
                menu_works.work_17->shown = FALSE;
                break;
            case KART_RACE_DOWNLOADED:
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                menu_works.work_13->courses.shown = FALSE;
                menu_works.work_13->preview_shown = FALSE;
                menu_control.step = STEP_READ;
                kart_race_work.hint_shown = TRUE;
                kart_race_work.reading = TRUE;
                voice_set_play(READ_VOICES);
                game_state.kart_character_karts = 1;
                break;
            }
        }
        break;
    case STEP_READ:
        if (menu_works.work_13->fading) {
            break;
        }
        kart_race_panel_set_list(
            kart_race_downloads,
            save_downloads_find(kart_race_downloads, DOWNLOAD_FIND_KINDS_3_4));
        if (kart_race_work.entry_count) {
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
        if (menu_works.work_13->fading) {
            break;
        }
        menu_control.step = STEP_LIST;
        save_check_sockets();
        break;
    case STEP_LIST:
        if ((pad_press_repeat[FIRST_PLAYER] & KART_CANCEL_BUTTONS) ||
            ((pad_press_repeat[SECOND_PLAYER] & KART_CANCEL_BUTTONS) &&
             menu_works.work_13->two_players) ||
            save_check_sockets()) {
            menu_works.work_13->list.shown = FALSE;
            menu_works.work_13->courses.shown = TRUE;
            menu_works.work_13->preview_shown = TRUE;
            menu_control.step = STEP_COURSES_IN;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            kart_race_work.hint_shown = FALSE;
        } else if (pad_press_repeat[FIRST_PLAYER] & PDD_DGT_KU) {
            if (menu_works.work_13->entry != 0) {
                menu_works.work_13->entry--;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if (pad_press_repeat[FIRST_PLAYER] & PDD_DGT_KD) {
            if (menu_works.work_13->entry <
                menu_works.work_13->entry_count - 1) {
                menu_works.work_13->entry++;
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if (pad_press_repeat[FIRST_PLAYER] & KART_CHOOSE_BUTTONS) {
            sound_play(MENU_SOUND_RACE, NULL, 0, 0);
            menu_works.work_13->list.shown = FALSE;
            menu_works.work_13->courses.shown = TRUE;
            menu_works.work_13->preview_shown = TRUE;
            menu_snapshot_save();
            menu_resume_target = MENU_RESUME_KART_RACE;
            menu_works.work_13->shown = FALSE;
            menu_works.work_13->unk_04 = 0;
            menu_control.step = STEP_RACE;
            menu_works.work_17->shown = FALSE;
        }
        break;
    case STEP_NONE_FOUND:
        if ((pad_press_repeat[FIRST_PLAYER] & KART_CHOOSE_BUTTONS) ||
            (pad_press_repeat[FIRST_PLAYER] & KART_CANCEL_BUTTONS) ||
            ((pad_press_repeat[SECOND_PLAYER] & KART_CHOOSE_BUTTONS) &&
             menu_works.work_13->two_players) ||
            ((pad_press_repeat[SECOND_PLAYER] & KART_CANCEL_BUTTONS) &&
             menu_works.work_13->two_players)) {
            menu_works.work_13->list.shown = FALSE;
            menu_works.work_13->courses.shown = TRUE;
            menu_works.work_13->preview_shown = TRUE;
            menu_works.work_13->question.shown = FALSE;
            menu_control.step = STEP_COURSES_IN;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            kart_race_work.hint_shown = FALSE;
        }
        break;
    case STEP_LEAVE:
        if (menu_works.work_13->fading) {
            break;
        }
        kart_race_leave();
        break;
    case STEP_RACE:
        if (menu_works.work_13->fading) {
            break;
        }
        stage_set_current(STAGE_CART_2);
        stage_set_character(
            FIRST_PLAYER,
            driver_characters[menu_works.work_13->picks[FIRST_PLAYER].driver]);
        if (menu_works.work_13->two_players) {
            stage_set_character(
                SECOND_PLAYER,
                driver_characters[menu_works.work_13->picks[SECOND_PLAYER]
                                      .driver]);
            game_state.split_screen = 1;
            game_state.two_players = 1;
        } else {
            game_state.split_screen = 0;
            game_state.two_players = 0;
        }
        game_state.demo = 0;
        game_state.mission = MISSION_FIRST;
        game_state.next_mode = GAME_MODE_STAGE;
        game_state.kart_course = menu_works.work_13->course;
        if (menu_works.work_13->course == KART_RACE_DOWNLOADED) {
            download_chosen = kart_race_downloads[menu_works.work_13->entry];
            save_download_set_drive(download_chosen.drive);
            save_download_set_slot(download_chosen.slot);
            save_download_read(DEMO_AREA);
        }
        sequence_start(SEQUENCE_NONE);
        return TRUE;
        /* @hack redundant: a break after the return, which no path reaches */
        break;
    case STEP_RESUME:
        if (kart_race_resume_wait-- < 0) {
            menu_control.step = STEP_COURSES_IN;
        }
        break;
    }
    return FALSE;
}

static void kart_race_leave(void)
{
    int group;

    if (menu_works.work_13->from_emblems) {
        emblem_results_menu_start();
        menu_control.mode = MENU_MODE_EMBLEM_RESULTS;
        /* @bug logic: a kart race the new menu opens later goes back to the
         * emblem results too. the store clears the boss attack's
         * from_emblems, work_15's (0x8c5158c4), where the test read the kart
         * race's (0x8c5158ac). the kart race's stays 1 until
         * kart_race_panel_reset */
        menu_works.work_15->from_emblems = FALSE;
    } else {
        new_menu_screen_start();
        if (!menu_works.work_13->two_players) {
            group = NEW_MENU_REOPEN_SINGLE;
        } else {
            group = NEW_MENU_REOPEN_VERSUS;
        }
        new_menu_screen_reopen(group);
        menu_control.mode = MENU_MODE_NEW_MENU;
    }
}

void kart_race_resume(void)
{
    menu_snapshot_restore();
    kart_race_start();
    menu_control.mode = MENU_MODE_KART_RACE;
    menu_control.step = STEP_RESUME;
    kart_race_resume_wait = RESUME_WAIT;
}
