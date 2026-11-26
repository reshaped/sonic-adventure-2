/* OTHERPRINT.PRS's init, exit and step, which its head names. the program
 * shows the logos or the picture whose textures are named tento_ending */
#include <sg_xpt.h>

#include "draw/fade.h"
#include "main.h"
#include "otherprint/logos.h"
#include "otherprint/otherprint.h"
#include "otherprint/tento_ending.h"
#include "save/card.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "system/frame.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "vmu/lcd.h"

#define FRAME_BLANKS 1
#define FRAME_STEPS 1

Bool otherprint_done = FALSE;

int otherprint_step(void)
{
    pad_scan();
    task_step_all();
    task_draw_all();
    if (otherprint_done) {
        return OVERLAY_DONE;
    }
    if (pad_reset_pressed(0)) {
        return OVERLAY_RESET;
    }
    return OVERLAY_RUNNING;
}

void otherprint_init(void)
{
    otherprint_done = FALSE;
    frame_pacing_init(FRAME_BLANKS, FRAME_STEPS);
    task_init();
    sound_task_create(NULL);
    fade_set_color(0);
    card_init();
    lcd_start();
    switch (game_state.other_print) {
    case OTHER_PRINT_LOGOS:
        logos_start();
        break;
    case OTHER_PRINT_ENDING:
        tento_ending_start();
        break;
    }
}

void otherprint_exit(void)
{
    card_exit();
    lcd_stop();
    fun_8c059fb2();
    sound_stop_all_and_unpause();
    sound_task_kill();
    task_destroy_all();
}
