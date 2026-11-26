/* EMBLEMGET.PRS's init, exit and step, which its head names. the program
 * shows the emblem screen */
#include "emblemget/emblemget/text.inc"
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/fade.h"
#include "effect/particle.h"
#include "emblemget/emblemget.h"
#include "main.h"
#include "save/card.h"
#include "shared/emblem.h"
#include "shared/sparkle.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "system/frame.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"
#include "vmu/lcd.h"

#define FRAME_BLANKS 1
#define FRAME_STEPS 1

#include "shared/common_pvm_textures.h"

/* sparkle_texlist takes its textures from efftex_common's list by name */
static NJS_TEXLIST *shared_texlists[2] = {
#include "emblemget/emblemget/shared_texlists.inc"
};
static NJS_TEXLIST **shared_groups[2] = {
#include "emblemget/emblemget/shared_groups.inc"
};
static struct texture_pvm common_pvms[3] = {
#include "emblemget/emblemget/common_pvms.inc"
};

int emblemget_step(void)
{
    task_step_all();
    task_draw_all();
    if (!emblem_screen_is_open()) {
        game_state.next_mode = GAME_MODE_TO_MENU;
        return OVERLAY_DONE;
    }
    fade_draw();
    /* @bug logic: the step sees no reset held on the emblem screen. only
     * pad_scan makes pad_reset_pressed return 1, and nothing calls pad_scan
     * while the program runs. this call (JSR at 0x8c50004c) always returns 0 */
    if (pad_reset_pressed(0)) {
        return OVERLAY_RESET;
    }
    return OVERLAY_RUNNING;
}

void emblemget_init(void)
{
    /* @hack redundant: unused, a local that only sizes the frame */
    Uint8 unused[64];

    frame_pacing_init(FRAME_BLANKS, FRAME_STEPS);
    task_init();
    sound_task_create(NULL);
    particle_core_start();
    fade_set_black();
    card_init();
    lcd_start();
    texture_load_shared(common_pvms, shared_groups);
    texture_await_shared(common_pvms, shared_groups);
    emblem_start();
    game_state.idle_dim = 1;
    game_state.steps = 0;
}

void emblemget_exit(void)
{
    card_exit();
    lcd_stop();
    particle_core_stop();
    texture_release_shared(common_pvms, shared_groups);
    fun_8c059fb2();
    sound_stop_all_and_unpause();
    sound_task_kill();
    task_destroy_all();
}
