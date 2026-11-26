/* the TV setting. its step drives tv_setting_draw.h's panel through
 * tv_setting_work, which menu_works.work_09 points at */
#include <sg_xpt.h>
#include <shinobi.h>

#include "advertise/menu_textures.h"
#include "advertise/menus.h"
#include "advertise/options.h"
#include "advertise/tv_setting.h"
#include "advertise/tv_setting_draw.h"
#include "sound/sound.h"
#include "system/game_state.h"
#include "system/pad.h"

struct tv_setting_work tv_setting_work;

/* the test runs TEST_STEPS steps, counting test_steps down. a step where
 * test_steps % TV_SETTING_SECOND_STEPS is TICK_STEP begins a second, and
 * plays MENU_SOUND_BACK as the second's tick */
#define TEST_STEPS (TV_SETTING_TEST_SECONDS * TV_SETTING_SECOND_STEPS)
#define TICK_STEP (TV_SETTING_SECOND_STEPS - 1)

/* menu_control.step. each wait below lasts until a step of the panel's
 * task moves nothing */
enum {
    STEP_OPEN, /* show the backdrop, with the cursor on the display's choice */
    STEP_SHOW_CHOICES, /* wait, then swing the choices in */
    STEP_LIGHT, /* wait, then light the cursor's choice and show its help */
    STEP_CHOOSE, /* move between the choices, and choose one */
    STEP_TEST_OPEN, /* wait, then show the test picture at 60 Hz */
    STEP_TEST, /* count test_steps down while nothing moves */
    STEP_TEST_CLOSE, /* wait, then change the display back and open again */
    STEP_CANCELED, /* wait, then leave */
    STEP_CHOSEN /* wait, change the display, then leave */
};

void tv_setting_start(void)
{
    menu_textures_select(MENU_TEXTURES_TV_SETTING);
    menu_control.step = STEP_OPEN;
}

static void tv_setting_hide(void)
{
    menu_works.work_09->shown = FALSE;
    menu_works.work_09->unk_04 = 0;
    menu_works.work_09->help_shown = FALSE;
}

Bool tv_setting_step(void)
{
    switch (menu_control.step) {
    case STEP_OPEN:
        menu_works.work_09->shown = TRUE;
        menu_works.work_09->backdrop.shown = TRUE;
        menu_control.step = STEP_SHOW_CHOICES;
        switch (game_state.display) {
        case DISPLAY_PAL:
            menu_works.work_09->cursor = TV_SETTING_50HZ;
            break;
        default:
            menu_works.work_09->cursor = TV_SETTING_60HZ;
            break;
        }
        break;
    case STEP_SHOW_CHOICES:
        if (menu_works.work_09->moving != 0) {
            break;
        }
        menu_works.work_09->choices.shown = TRUE;
        menu_control.step = STEP_LIGHT;
        break;
    case STEP_LIGHT:
        if (menu_works.work_09->moving != 0) {
            break;
        }
        menu_works.work_09->choices.bright = TRUE;
        menu_works.work_09->help_shown = TRUE;
        menu_control.step = STEP_CHOOSE;
        break;
    case STEP_CHOOSE:
        /* @hack type: pad_press_repeat[0] and pad_cancel_buttons cast to
         * Sint32, where the two uncast would do */
        if (((Sint32)pad_press_repeat[0] & (Sint32)pad_cancel_buttons) != 0) {
            tv_setting_hide();
            menu_control.step = STEP_CANCELED;
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
        } else if ((pad_press_repeat[0] & PDD_DGT_KL) != 0) {
            menu_works.work_09->cursor--;
            if (menu_works.work_09->cursor < 0) {
                menu_works.work_09->cursor = 0;
            } else {
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if ((pad_press_repeat[0] & PDD_DGT_KR) != 0) {
            menu_works.work_09->cursor++;
            if (menu_works.work_09->cursor > TV_SETTING_CHOICES - 1) {
                menu_works.work_09->cursor = TV_SETTING_CHOICES - 1;
            } else {
                sound_play(SOUND_CURSOR_ID, NULL, 0, 0);
            }
        } else if ((pad_press_repeat[0] & pad_choose_buttons) != 0) {
            switch (menu_works.work_09->cursor) {
            case TV_SETTING_TEST:
                menu_control.step = STEP_TEST_OPEN;
                tv_setting_hide();
                sound_play(SOUND_CHOOSE_ID, NULL, 0, 0);
                break;
            case TV_SETTING_50HZ:
                game_state.display = DISPLAY_PAL;
                tv_setting_hide();
                menu_control.step = STEP_CHOSEN;
                break;
            case TV_SETTING_60HZ:
                game_state.display = DISPLAY_NTSC;
                tv_setting_hide();
                menu_control.step = STEP_CHOSEN;
                break;
            }
        }
        break;
    case STEP_CANCELED:
        if (menu_works.work_09->moving != 0) {
            break;
        }
        if (game_state.display_pending != 0) {
            return TRUE;
        }
        options_start();
        menu_control.mode = MENU_MODE_OPTIONS;
        break;
    case STEP_CHOSEN:
        if (menu_works.work_09->moving == 0) {
            menu_textures_change_display(game_state.display);
            if (game_state.display_pending != 0) {
                return TRUE;
                /* @hack spelling: the else after a branch that returns, where
                 * the statements after the test would do */
            } else {
                options_start();
                menu_control.mode = MENU_MODE_OPTIONS;
            }
        }
        break;
    case STEP_TEST_OPEN:
        if (menu_works.work_09->moving == 0) {
            menu_works.work_09->shown = TRUE;
            menu_works.work_09->test.shown = TRUE;
            menu_works.work_09->test_steps = TEST_STEPS;
            menu_control.step = STEP_TEST;
            menu_textures_change_display(DISPLAY_NTSC);
        }
        break;
    case STEP_TEST:
        if (menu_works.work_09->moving != 0) {
            break;
        }
        menu_works.work_09->test_steps--;
        if (menu_works.work_09->test_steps % TV_SETTING_SECOND_STEPS ==
            TICK_STEP) {
            sound_play(MENU_SOUND_BACK, NULL, 0, 0);
        }
        if (menu_works.work_09->test_steps == 0) {
            menu_control.step = STEP_TEST_CLOSE;
            menu_works.work_09->shown = FALSE;
        }
        break;
    case STEP_TEST_CLOSE:
        if (menu_works.work_09->moving == 0) {
            menu_control.step = STEP_OPEN;
            menu_textures_change_display(game_state.display);
        }
        break;
    }
    return FALSE;
}

void tv_setting_open(void)
{
    menu_textures_select(MENU_TEXTURES_TV_SETTING);
    menu_control.step = STEP_OPEN;
    menu_control.mode = MENU_MODE_TV_SETTING;
}
