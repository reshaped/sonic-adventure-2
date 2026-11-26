/* the synopsis's screen, MENU_MODE_SYNOPSIS: synopsis.h's synopsis of the
 * story the story select chose, before the story plays */
#include <sg_xpt.h>

#include "advertise/menu_textures.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/story_select.h"
#include "advertise/synopsis.h"
#include "advertise/synopsis_screen.h"
#include "sound/adx.h"
#include "system/pad.h"

struct synopsis_work synopsis_work;

/* menu_control.step, while the screen runs */
enum {
    /* show the synopsis, or pass it over for a story from its beginning */
    STEP_SHOW,
    STEP_FADE_IN, /* wait for the backdrop, then scroll */
    STEP_SCROLL,
    STEP_BACK, /* wait for the cover, then go back to the story select */
    STEP_PLAY, /* wait for the cover, then play the story */
    /* wait for the cover and the story select to be still, then play the
     * story */
    STEP_PASS
};

/* only synopsis_screen_step calls it. it isn't static, since SHC would
 * expand those calls and leave the function out */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* hide the synopsis, and stop its scroll */
void synopsis_screen_hide(void);
/* NOLINTEND(misc-use-internal-linkage) */

void synopsis_screen_start(void)
{
    menu_textures_select(MENU_TEXTURES_SYNOPSIS);
    menu_control.step = STEP_SHOW;
}

void synopsis_screen_hide(void)
{
    menu_works.work_06->shown = FALSE;
    menu_works.work_06->scrolling = FALSE;
}

Bool synopsis_screen_step(void)
{
    switch (menu_control.step) {
    case STEP_SHOW:
        menu_works.work_06->shown = TRUE;
        if (synopsis_from_start()) {
            synopsis_screen_hide();
            menu_control.step = STEP_PASS;
        } else {
            menu_control.step = STEP_FADE_IN;
        }
        break;
    case STEP_FADE_IN:
        if (menu_works.work_06->fading) {
            break;
        }
        menu_works.work_06->scrolling = TRUE;
        menu_control.step = STEP_SCROLL;
        break;
    case STEP_SCROLL:
        /* @hack type: the pad's masks cast to int, where their own Uint32
         * would do */
        if (((int)pad_press_repeat[0] & (int)pad_cancel_buttons) != 0) {
            menu_control.step = STEP_BACK;
            synopsis_screen_hide();
            menu_works.work_07->shown = TRUE;
            /* @hack type: the pad's masks cast to int, where their own Uint32
             * would do */
        } else if (((int)pad_press_repeat[0] & (int)pad_choose_buttons) != 0 ||
                   menu_works.work_06->done) {
            menu_control.step = STEP_PLAY;
            synopsis_screen_hide();
            menu_works.work_07->shown = TRUE;
        }
        break;
    case STEP_BACK:
        if (menu_works.work_07->fading) {
            break;
        }
        menu_works.work_06->shown = FALSE;
        menu_works.work_06->scrolling = FALSE;
        story_select_start();
        menu_works.work_07->shown = FALSE;
        story_select_reopen(STORY_SELECT_REOPEN);
        menu_control.mode = MENU_MODE_STORY_SELECT;
        voices_stop();
        menu_restore_sequence_step();
        break;
    case STEP_PLAY:
        if (menu_works.work_07->fading) {
            break;
        }
        menu_to_sequence();
        return TRUE;
        /* @hack redundant: a break after the return */
        break;
    case STEP_PASS:
        if (menu_works.work_07->fading || menu_works.work_03->moving) {
            break;
        }
        menu_to_sequence();
        return TRUE;
    }
    return FALSE;
}
