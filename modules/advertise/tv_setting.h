/* the TV setting, MENU_MODE_TV_SETTING. it sets the display to 50 or 60 Hz.
 * tv_setting_draw.h's task draws it */
#ifndef ADVERTISE_TV_SETTING_H
#define ADVERTISE_TV_SETTING_H
#include <sg_xpt.h>

/* the choices, as tv_setting_work.cursor holds them, left to right */
enum tv_setting_choice {
    TV_SETTING_TEST, /* show the test picture at 60 Hz */
    TV_SETTING_50HZ, /* set game_state.display to DISPLAY_PAL */
    TV_SETTING_60HZ /* set game_state.display to DISPLAY_NTSC */
};
#define TV_SETTING_CHOICES 3

/* the test lasts TV_SETTING_TEST_SECONDS seconds of TV_SETTING_SECOND_STEPS
 * steps */
#define TV_SETTING_TEST_SECONDS 5
#define TV_SETTING_SECOND_STEPS 60

/* start the TV setting at its first step, and select its textures. it
 * leaves menu_control.mode to the caller */
void tv_setting_start(void);
/* start the TV setting as tv_setting_start does, under
 * MENU_MODE_TV_SETTING. menus_resume runs it while
 * game_state.display_pending is 1 */
void tv_setting_open(void);
/* the TV setting's step, under MENU_MODE_TV_SETTING. Left and Right move
 * the cursor over the choices. Choose on TV_SETTING_TEST shows the test
 * picture at 60 Hz for TV_SETTING_TEST_SECONDS seconds. the choices then
 * show again at the display's own rate. Choose on 50 or 60 Hz sets
 * game_state.display, and changes the display to it.
 *
 * that choice or Cancel leaves the TV setting once its panel stops moving.
 * the step then returns TRUE to end the menus where
 * game_state.display_pending is 1. otherwise it starts the options, and
 * sets menu_control.mode to MENU_MODE_OPTIONS. it returns FALSE on every
 * other step */
Bool tv_setting_step(void);

#endif
