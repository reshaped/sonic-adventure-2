/* the sound test. its step drives sound_test_panel.h's panel through
 * menu_sound_test_work, which menu_works.work_10 points at */
#include <cri_adxt.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "advertise/menu_textures.h"
#include "advertise/menus.h"
#include "advertise/options.h"
#include "advertise/sound_test.h"
#include "advertise/sound_test_panel.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "system/pad.h"

struct menu_sound_test_work menu_sound_test_work;

#define IDLE_MUSIC "advsng_dummy.adx" /* the music while no song plays */

/* menu_control.step. each wait below lasts until a step of the panel's task
 * moves nothing */
enum {
    STEP_OPEN, /* show the backdrop, and play IDLE_MUSIC */
    STEP_SHOW_LISTS, /* wait, then show the disc and list 0 */
    STEP_LISTS_FADE, /* wait, then show the prompt */
    STEP_LISTS, /* choose a list of songs from list 0 */
    STEP_SONGS_OPEN, /* wait, then show the chosen list */
    STEP_SONGS_FADE, /* wait, then show the prompt */
    STEP_SONGS, /* choose a song */
    STEP_PLAYING, /* play it until cancel or its end */
    STEP_CLOSE /* wait, then go back to the options */
};

/* hide the panel. nothing calls menu_sound_test_close, and no header
 * declares it. it is external only because SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void menu_sound_test_close(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* the list of songs chosen, from 1, and whether a song the sound test
 * started plays */
static Sint32 menu_sound_test_open_list;
static Bool menu_sound_test_song_on;

static Bool menu_sound_test_song_ended(void);

void menu_sound_test_start(void)
{
    menu_textures_select(MENU_TEXTURES_SOUND_TEST);
    menu_control.step = STEP_OPEN;
    menu_sound_test_lists_build();
}

void menu_sound_test_close(void)
{
    menu_works.work_10->active = FALSE;
    menu_works.work_10->unk_04 = 0;
}

Bool menu_sound_test_step(void)
{
    switch (menu_control.step) {
    case STEP_OPEN:
        menu_works.work_10->active = TRUE;
        menu_works.work_10->shown = TRUE;
        menu_works.work_10->bright = TRUE;
        menu_control.step = STEP_SHOW_LISTS;
        music_change(IDLE_MUSIC);
        break;
    case STEP_SHOW_LISTS:
        if (menu_works.work_10->moving != 0) {
            break;
        }
        menu_works.work_10->disc_shown = TRUE;
        menu_works.work_10->lists[0].shown = TRUE;
        menu_control.step = STEP_LISTS_FADE;
        break;
    case STEP_LISTS_FADE:
        if (menu_works.work_10->moving != 0) {
            break;
        }
        menu_control.step = STEP_LISTS;
        menu_works.work_10->prompt = TRUE;
        break;
    case STEP_LISTS:
        /* @hack type: pad_press_repeat[0] and the mask cast to Sint32, here
         * and in each test of pad_cancel_buttons or pad_choose_buttons
         * below, where the two uncast would do */
        if (((Sint32)pad_press_repeat[0] & (Sint32)pad_cancel_buttons) != 0) {
            menu_works.work_10->active = FALSE;
            menu_works.work_10->unk_04 = 0;
            menu_control.step = STEP_CLOSE;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
            menu_works.work_10->prompt = FALSE;
        } else if ((pad_press_repeat[0] & PDD_DGT_KU) != 0) {
            menu_works.work_10->lists[0].row--;
            if (menu_works.work_10->lists[0].row < 0) {
                menu_works.work_10->lists[0].row = 0;
            } else {
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if ((pad_press_repeat[0] & PDD_DGT_KD) != 0) {
            menu_works.work_10->lists[0].row++;
            if (menu_works.work_10->lists[0].row >=
                menu_works.work_10->lists[0].rows) {
                menu_works.work_10->lists[0].row =
                    menu_works.work_10->lists[0].rows - 1;
            } else {
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if (((Sint32)pad_press_repeat[0] & (Sint32)pad_choose_buttons) !=
                   0) {
            if (menu_works.work_10->lists[menu_works.work_10->lists[0].row + 1]
                    .rows != 0) {
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                menu_control.step = STEP_SONGS_OPEN;
                menu_sound_test_open_list =
                    menu_works.work_10->lists[0].row + 1;
                menu_works.work_10->lists[0].shown = FALSE;
                menu_works.work_10->prompt = FALSE;
            } else {
                sound_play(MENU_SOUND_REFUSED, NULL, 0, 0);
            }
        }
        break;
    case STEP_SONGS_OPEN:
        if (menu_works.work_10->moving == 0) {
            menu_works.work_10->lists[menu_sound_test_open_list].shown = TRUE;
            menu_control.step = STEP_SONGS_FADE;
        }
        break;
    case STEP_SONGS_FADE:
        /* @hack spelling: an empty arm under the test of moving, where a
         * test of !moving would do */
        if (menu_works.work_10->moving != 0) {
        } else {
            menu_control.step = STEP_SONGS;
            menu_works.work_10->prompt = TRUE;
            menu_sound_test_song_on = FALSE;
        }
        break;
    case STEP_SONGS:
        if (((Sint32)pad_press_repeat[0] & (Sint32)pad_cancel_buttons) != 0) {
            if (menu_sound_test_song_on) {
                menu_sound_test_song_on = FALSE;
                music_change(IDLE_MUSIC);
                /* @hack spelling: playing set through the name
                 * menu_sound_test_work, here and as a song starts or stops
                 * below, where menu_works.work_10 would do */
                menu_sound_test_work.playing = FALSE;
            } else {
                menu_works.work_10->lists[menu_sound_test_open_list].shown =
                    FALSE;
                menu_control.step = STEP_SHOW_LISTS;
                sound_play(MENU_SOUND_BACK, NULL, 0, 0);
                menu_works.work_10->prompt = FALSE;
            }
        } else if (((Sint32)pad_press_repeat[0] & (Sint32)pad_choose_buttons) !=
                   0) {
            music_play_name(menu_sound_test_song_name(
                menu_sound_test_open_list,
                menu_works.work_10->lists[menu_sound_test_open_list].row));
            menu_sound_test_work.playing = TRUE;
            menu_sound_test_song_on = TRUE;
            menu_control.step = STEP_PLAYING;
        } else if ((pad_press_repeat[0] & PDD_DGT_KU) != 0) {
            menu_works.work_10->lists[menu_sound_test_open_list].row--;
            if (menu_works.work_10->lists[menu_sound_test_open_list].row < 0) {
                menu_works.work_10->lists[menu_sound_test_open_list].row = 0;
            } else {
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if ((pad_press_repeat[0] & PDD_DGT_KD) != 0) {
            menu_works.work_10->lists[menu_sound_test_open_list].row++;
            if (menu_works.work_10->lists[menu_sound_test_open_list].row >=
                menu_works.work_10->lists[menu_sound_test_open_list].rows) {
                menu_works.work_10->lists[menu_sound_test_open_list].row =
                    menu_works.work_10->lists[menu_sound_test_open_list].rows -
                    1;
            } else {
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        }
        break;
    case STEP_PLAYING:
        if (((Sint32)pad_press_repeat[0] & (Sint32)pad_cancel_buttons) != 0 ||
            menu_sound_test_song_ended()) {
            if (menu_sound_test_song_on) {
                menu_sound_test_song_on = FALSE;
                music_change(IDLE_MUSIC);
                menu_sound_test_work.playing = FALSE;
                menu_control.step = STEP_SONGS;
            }
        }
        break;
    case STEP_CLOSE:
        if (menu_works.work_10->moving != 0) {
            break;
        }
        options_start();
        menu_control.mode = MENU_MODE_OPTIONS;
        break;
    }
    return FALSE;
}

/* TRUE once the music's ADXT handle has played to its end. FALSE without a
 * sound task or a handle */
static Bool menu_sound_test_song_ended(void)
{
    if (sound_streams != NULL && sound_streams->adxt != NULL) {
        if (ADXT_GetStat(sound_streams->adxt) == ADXT_STAT_PLAYEND) {
            return TRUE;
        }
    }
    return FALSE;
}
