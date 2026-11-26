/* ENDING.PRS's init, exit and step, which its head names. the program shows
 * the staff roll of the hero or the dark story */
#include <sg_xpt.h>

#include "draw/fade.h"
#include "ending/ending.h"
#include "main.h"
#include "save/card.h"
#include "shared/staff_roll.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "system/flags.h"
#include "system/frame.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/sequence.h"
#include "system/task.h"
#include "vmu/lcd.h"

#define FRAME_BLANKS 1
#define FRAME_STEPS 1

static Sint32 pal_frames = 0;

int ending_step(void)
{
    fade_set_color(0);
    game_state.idle_dim = 0;
    pad_scan();
    task_step_all();
    if (game_state.display == DISPLAY_PAL) {
        /* @bug logic: under DISPLAY_PAL, no frame steps the tasks twice, as
         * one of every PAL_FRAMES does under frame.h's RUN_TASKS. nothing
         * counts pal_frames up: its one store writes 0 (MOV.L at
         * 0x8c50004e). the test for PAL_FRAMES then never holds (CMP/EQ #5
         * at 0x8c500044) */
        if (pal_frames == PAL_FRAMES) {
            pal_frames = 0;
            task_step_all();
        }
    }
    task_draw_all();
    if (!staff_roll_running) {
        sequence_advance();
        return OVERLAY_DONE;
    }
    if (pad_reset_pressed(0)) {
        return OVERLAY_RESET;
    }
    return OVERLAY_RUNNING;
}

void ending_init(void)
{
    frame_pacing_init(FRAME_BLANKS, FRAME_STEPS);
    task_init();
    sound_task_create(NULL);
    card_init();
    lcd_start();
    game_state.idle_dim = 0;
    if (game_flags[FLAG_SEQUENCE] == SEQUENCE_HERO) {
        staff_roll_load(SEQUENCE_HERO);
    } else {
        staff_roll_load(SEQUENCE_DARK);
    }
    staff_roll_start();
    music_play_next(game_flags[FLAG_SEQUENCE] == SEQUENCE_HERO
                        ? "t1_sonic.adx"
                        : "t1_shado.adx");
}

void ending_exit(void)
{
    card_exit();
    lcd_stop();
    fun_8c059fb2();
    sound_stop_all_and_unpause();
    sound_task_kill();
    game_state.idle_dim = 1;
    staff_roll_release();
    task_destroy_all();
}
