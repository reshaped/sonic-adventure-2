/* the frame timer, on Shinobi's general-purpose timer, and the task profiler
 * that reads it */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "system/frame.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/timer.h"

/* the times count thousandths of a vertical blank. VBLANK is a vertical
 * blank, and MILLISECOND a millisecond */
#define VBLANK 1000
#define MILLISECOND 60.0F
#define MILLISECONDS_PER_MICROSECOND 0.001F
/* a tick of the timer, in microseconds. timer_start clocks the timer at
 * SYD_TMR_CLOCK_P4, a quarter of the 50 MHz peripheral clock */
#define TICK_MICROSECONDS 0.08F
/* a tick, as the times count it */
#define TICK (TICK_MICROSECONDS * MILLISECONDS_PER_MICROSECOND * MILLISECOND)

/* @hack redundant: the profiler's code, which never runs while PROFILE_ON is
 * FALSE */
#define PROFILE_ON FALSE
#define PROFILE_ROWS 8
#define PROFILE_LAST_ROW (PROFILE_ROWS - 1)
/* profile_mode's values. PROFILE_TABLE times the frame's steps and draws
 * that fall in the table's rows. each row keeps a function and its time.
 * PROFILE_WATCH times every call of one step and of one draw. for each it
 * keeps the longest time, the total and the number of calls */
#define PROFILE_TABLE 0
#define PROFILE_WATCH 1
/* the color profile_print goes back to after each heading. SHC dropped the
 * load of it with the test of profile_mode, so the image doesn't hold it.
 * this value stands in for it */
#define PROFILE_TEXT_COLOR 0xffc0c0c0

/* timer_draw_bar's sizes, in pixels: half the bar's height, half the
 * scale's, and the length of each of the scale's four parts */
#define BAR_HALF_HEIGHT 5
#define SCALE_HALF_HEIGHT 6
#define SCALE_PART 50
/* the depths of the bar and of its scale */
#define BAR_DEPTH 0.5F
#define SCALE_DEPTH 0.51F

/* PROFILE_TABLE or PROFILE_WATCH. SHC dropped every read of it, and the
 * load of its address in timer_start. the image holds a word of 0 ahead of
 * profile_first, and nothing points at that word. the initializer puts
 * profile_mode there, since the bss below has no gap for a byte */
static Sint8 profile_mode = PROFILE_TABLE;
/* the index, among the frame's steps or its draws, of the one that takes the
 * table's first row */
static Sint32 profile_first = 0;

/* the timer's count at the mark, and as the last render ended */
static Uint32 timer_marked;
static Uint32 timer_rendered;
/* the steps and the draws the frame has made, so the index of the next */
static Sint32 profile_step_index;
static Sint32 profile_draw_index;
/* the table: each row's step and draw, and the time each took */
static void (*profile_table_steps[PROFILE_ROWS])(struct task *task);
static Uint32 profile_table_step_times[PROFILE_ROWS];
static void (*profile_table_draws[PROFILE_ROWS])(struct task *task);
static Uint32 profile_table_draw_times[PROFILE_ROWS];
/* the step and the draw that PROFILE_WATCH times. for each, the longest of
 * the frame's times, their total, and the calls timed */
static void (*profile_watched_step)(struct task *task);
static void (*profile_watched_draw)(struct task *task);
static Uint32 profile_step_max;
static Uint32 profile_step_total;
static Uint32 profile_step_runs;
static Uint32 profile_draw_max;
static Uint32 profile_draw_total;
static Uint32 profile_draw_runs;
/* the name of the first task the frame found with the watched step, or
 * null */
static const char *profile_watched_name;

static void timer_end_of_render(void);

/* no other object calls these, and nothing calls timer_elapsed or
 * profile_print. SHC leaves out a static function that nothing calls, and one
 * whose every call it expands in place. it expands timer_frame_end's call of
 * timer_set_render_time */
/* @hack redundant: external linkage, which no call needs, where static would
 * do */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Float timer_elapsed(void);
Float timer_set_render_time(void);
void profile_print(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* restart the timer at 0 with the mark there, and set Ninja's end-of-render
 * callback. under PROFILE_ON it then serves the profiler's controls, and
 * clears what the profiler kept of the last frame */
static void timer_start(void)
{
    void (**call)(struct task *task);
    Uint32 *time;
    int i;

    syTmrGenStop();
    timer_marked = 0;
    syTmrGenSetClock(SYD_TMR_CLOCK_P4);
    /* @hack spelling: timer_marked read back, where 0 would do */
    syTmrGenSetCount(timer_marked);
    syTmrGenStart();
    njSetEORCallBackFunction(timer_end_of_render);
    if (PROFILE_ON) {
        /* the controls, on player 0's pad. under PROFILE_TABLE, up and down
         * scroll the table a row through the frame's calls. a scroll needs
         * its direction both held and in pad_press_repeat. right watches the
         * step and the draw of the table's first row. under PROFILE_WATCH,
         * left goes back to the table */
        switch (profile_mode) {
        case PROFILE_TABLE:
            if ((player_pads[0]->on & PDD_DGT_KU) &&
                (pad_press_repeat[0] & PDD_DGT_KU)) {
                if (profile_first != 0) {
                    profile_first--;
                }
            } else if ((player_pads[0]->on & PDD_DGT_KD) &&
                       (pad_press_repeat[0] & PDD_DGT_KD)) {
                if (profile_first != -1) {
                    profile_first++;
                }
            }
            if (player_pads[0]->press & PDD_DGT_KR) {
                profile_mode = PROFILE_WATCH;
                profile_watched_step = profile_table_steps[0];
                profile_watched_draw = profile_table_draws[0];
            }
            break;
        case PROFILE_WATCH:
            if (player_pads[0]->press & PDD_DGT_KL) {
                profile_mode = PROFILE_TABLE;
            }
            break;
        }
        profile_step_index = 0;
        profile_draw_index = 0;
        /* @hack spelling: each column cleared through a pointer, counting
         * down, where an index counting up would do */
        call = profile_table_steps;
        for (i = PROFILE_ROWS; i != 0; i--) {
            *call++ = NULL;
        }
        time = profile_table_step_times;
        for (i = PROFILE_ROWS; i != 0; i--) {
            *time++ = 0;
        }
        call = profile_table_draws;
        for (i = PROFILE_ROWS; i != 0; i--) {
            *call++ = NULL;
        }
        time = profile_table_draw_times;
        for (i = PROFILE_ROWS; i != 0; i--) {
            *time++ = 0;
        }
        profile_step_max = 0;
        profile_step_total = 0;
        profile_step_runs = 0;
        profile_draw_max = 0;
        profile_draw_total = 0;
        profile_draw_runs = 0;
        profile_watched_name = NULL;
    }
}

/* set game_state.frame_time to the time since timer_start, and move the
 * mark to now. returns the time */
static Float timer_mark_frame_time(void)
{
    /* @hack spelling: the mark and count set in one expression, where a
     * statement for each would do */
    Uint32 count = timer_marked = syTmrGenGetCount();

    return game_state.frame_time = count * TICK;
}

/* the time since timer_start */
Float timer_elapsed(void)
{
    return syTmrGenGetCount() * TICK;
}

Float timer_lap(void)
{
    Uint32 count = syTmrGenGetCount();
    Uint32 ticks = count - timer_marked;

    timer_marked = count;
    return ticks * TICK;
}

void timer_mark(void)
{
    timer_marked = syTmrGenGetCount();
}

/* set game_state.render_time to the time from timer_start to the end of the
 * last render, and return it */
Float timer_set_render_time(void)
{
    return game_state.render_time = timer_rendered * TICK;
}

/* the callback Ninja calls as each render ends. it notes the timer's count
 * for timer_set_render_time, then steps the tasks of level 6 */
static void timer_end_of_render(void)
{
    timer_rendered = syTmrGenGetCount();
    task_step_level_6();
}

void profile_step_start(void (*step)(struct task *task), const char *name)
{
    Sint32 row;

    /* @hack spelling: the hook's work as the test's else, where it would do
     * after the test */
    if (!PROFILE_ON) {
        return;
    } else {
        switch (profile_mode) {
        case PROFILE_TABLE:
            row = profile_step_index - profile_first;
            /* @hack spelling: the return as the else of the row's test,
             * where a test for a row outside the table would do ahead of
             * the store */
            if (row >= 0 && row <= PROFILE_LAST_ROW) {
                profile_table_steps[row] = step;
            } else {
                return;
            }
            break;
        /* @hack spelling: no break ends this case, the switch's last,
         * where one would do */
        case PROFILE_WATCH:
            /* @hack spelling: the name's test as the else of a test that
             * returns, where a test of its own would do */
            if (profile_watched_step != step) {
                return;
            } else if (profile_watched_name == NULL) {
                profile_watched_name = name;
            }
        }
        timer_mark();
    }
}

void profile_step_end(void (*step)(struct task *task))
{
    Sint32 row;
    Uint32 time;

    if (!PROFILE_ON) {
        return;
    }
    switch (profile_mode) {
    case PROFILE_TABLE:
        row = profile_step_index - profile_first;
        if (row >= 0 && row <= PROFILE_LAST_ROW) {
            profile_table_step_times[row] = (Uint32)timer_lap();
        }
        break;
    case PROFILE_WATCH:
        if (profile_watched_step == step) {
            time = (Uint32)timer_lap();
            if (time > profile_step_max) {
                profile_step_max = time;
            }
            profile_step_total += time;
            profile_step_runs++;
        }
        break;
    }
    profile_step_index++;
}

void profile_draw_start(void (*draw)(struct task *task))
{
    Sint32 row;

    /* @hack spelling: the hook's work as the test's else, where it would do
     * after the test */
    if (!PROFILE_ON) {
        return;
    } else {
        switch (profile_mode) {
        case PROFILE_TABLE:
            row = profile_draw_index - profile_first;
            /* @hack spelling: the return as the else of the row's test,
             * where a test for a row outside the table would do ahead of
             * the store */
            if (row >= 0 && row <= PROFILE_LAST_ROW) {
                profile_table_draws[row] = draw;
            } else {
                return;
            }
            break;
        case PROFILE_WATCH:
            if (profile_watched_draw != draw) {
                return;
            }
            break;
        }
        timer_mark();
    }
}

void profile_draw_end(void (*draw)(struct task *task))
{
    Sint32 row;
    Uint32 time;

    if (!PROFILE_ON) {
        return;
    }
    switch (profile_mode) {
    case PROFILE_TABLE:
        row = profile_draw_index - profile_first;
        if (row >= 0 && row <= PROFILE_LAST_ROW) {
            profile_table_draw_times[row] = (Uint32)timer_lap();
        }
        break;
    case PROFILE_WATCH:
        if (profile_watched_draw == draw) {
            time = (Uint32)timer_lap();
            if (time > profile_draw_max) {
                profile_draw_max = time;
            }
            profile_draw_total += time;
            profile_draw_runs++;
        }
        break;
    }
    profile_draw_index++;
}

/* print the table under PROFILE_TABLE: each row's step and draw, as
 * addresses, with their times. under PROFILE_WATCH, print the watched step
 * and draw with what the profiler keeps of each. then print the name of the
 * watched step's first task, or Unidentified.
 *
 * SHC dropped the loads of the print functions' addresses with the test of
 * profile_mode. so the image doesn't say which functions profile_print calls.
 * their arguments are those of njPrintColor, njPrintC, njPrintH and njPrintD */
void profile_print(void)
{
    Sint32 row;
    const char *name;

    /* @hack spelling: the prints as the test's else, where they would do
     * after the test */
    if (!PROFILE_ON) {
        return;
    } else {
        switch (profile_mode) {
        case PROFILE_TABLE:
            njPrintColor(0xff308068);
            njPrintC(NJM_LOCATION(2, 13), " <Exec>  <Cnt>  <Disp>  <Cnt>");
            njPrintColor(PROFILE_TEXT_COLOR);
            njPrintC(NJM_LOCATION(1, 14), ">");
            for (row = 0; row < PROFILE_ROWS; row++) {
                njPrintH(NJM_LOCATION(2, row + 14),
                         (Uint32)profile_table_steps[row], 8);
                njPrintD(NJM_LOCATION(11, row + 14),
                         profile_table_step_times[row], 5);
                njPrintH(NJM_LOCATION(17, row + 14),
                         (Uint32)profile_table_draws[row], 8);
                njPrintD(NJM_LOCATION(26, row + 14),
                         profile_table_draw_times[row], 5);
            }
            break;
        case PROFILE_WATCH:
            if (profile_watched_step != NULL) {
                njPrintColor(0xffc0c0a0);
                njPrintC(NJM_LOCATION(2, 13), " <Exec>  < Max > <Total>");
                njPrintColor(PROFILE_TEXT_COLOR);
                njPrintC(NJM_LOCATION(1, 14), "<");
                njPrintH(NJM_LOCATION(2, 14), (Uint32)profile_watched_step, 8);
                njPrintD(NJM_LOCATION(12, 14), profile_step_max, 5);
                njPrintD(NJM_LOCATION(20, 14), profile_step_total, 5);
            }
            if (profile_watched_draw != NULL) {
                njPrintColor(0xffc0a0a0);
                njPrintC(NJM_LOCATION(2, 15), " <Disp>  < Max > <Total>");
                njPrintColor(PROFILE_TEXT_COLOR);
                njPrintH(NJM_LOCATION(2, 16), (Uint32)profile_watched_draw, 8);
                njPrintD(NJM_LOCATION(12, 16), profile_draw_max, 5);
                njPrintD(NJM_LOCATION(20, 16), profile_draw_total, 5);
            }
            njPrintColor(0xffa0c0c0);
            njPrintC(NJM_LOCATION(2, 17), "<Exec Loops> <Disp Loops>");
            njPrintColor(PROFILE_TEXT_COLOR);
            njPrintD(NJM_LOCATION(2, 18), profile_step_runs, 5);
            njPrintD(NJM_LOCATION(20, 18), profile_draw_runs, 5);
            njPrintColor(0xffc0a0c0);
            name = profile_watched_name;
            if (name == NULL) {
                name = "Unidentified";
            }
            njPrintC(NJM_LOCATION(2, 19), name);
            njPrintColor(PROFILE_TEXT_COLOR);
            break;
        }
    }
}

/* draw time as a bar from x rightward, centered on y, in color, an ARGB
 * color. a time that isn't negative takes a pixel for each 10. a vertical
 * blank is thus SCALE_PART * 2 pixels. with the bar goes a translucent scale
 * of four SCALE_PART parts, each in a color of its own */
static void timer_draw_bar(Uint32 x, Uint32 y, Uint32 color, Sint32 time)
{
    NJS_POLYGON_VTX strip[4];
    Sint32 end = x + ((time >> 1) / 5);

    strip[0].col = strip[1].col = strip[2].col = strip[3].col = color;
    strip[0].x = strip[1].x = end;
    strip[2].x = strip[3].x = x;
    strip[1].y = strip[3].y = y + BAR_HALF_HEIGHT;
    strip[0].y = strip[2].y = y - BAR_HALF_HEIGHT;
    strip[0].z = strip[1].z = strip[2].z = strip[3].z = BAR_DEPTH;
    njDrawPolygon(strip, 4, FALSE);
    strip[0].col = strip[1].col = strip[2].col = strip[3].col = 0x704418b0;
    strip[0].x = strip[1].x = x + SCALE_PART;
    strip[1].y = strip[3].y = y + SCALE_HALF_HEIGHT;
    strip[0].y = strip[2].y = y - SCALE_HALF_HEIGHT;
    strip[0].z = strip[1].z = strip[2].z = strip[3].z = SCALE_DEPTH;
    njDrawPolygon(strip, 4, TRUE);
    strip[0].col = strip[1].col = strip[2].col = strip[3].col = 0x60b02040;
    strip[2].x = strip[3].x = x + (SCALE_PART * 2);
    njDrawPolygon(strip, 4, TRUE);
    strip[0].col = strip[1].col = strip[2].col = strip[3].col = 0x5840f038;
    strip[0].x = strip[1].x = x + (SCALE_PART * 3);
    njDrawPolygon(strip, 4, TRUE);
    strip[0].col = strip[1].col = strip[2].col = strip[3].col = 0x50b0b048;
    strip[2].x = strip[3].x = x + (SCALE_PART * 4);
    njDrawPolygon(strip, 4, TRUE);
}

void timer_frame_start(void)
{
    timer_start();
    timer_mark();
}

void timer_frame_end(void)
{
    timer_mark_frame_time();
    timer_set_render_time();
    /* the image keeps the nine labels of a display, SH-4: to POLY:. it also
     * keeps the seven registers the function saves for the display, and 8 bytes
     * of its frame. nothing shows what the display printed beside each label.
     * the lines below stand in. they print the frame's times beside SH-4: and
     * CLX2:, and draw them as bars. those calls also keep timer_draw_bar, which
     * nothing else calls, in the object */
    /* @hack redundant: a debug display that never runs */
    if (0) {
        Sint32 frame = (Sint32)game_state.frame_time;
        Sint32 draw = (Sint32)game_state.draw_time;
        Sint32 step = frame - draw;
        Sint32 steps = profile_step_index;
        Sint32 draws = profile_draw_index;
        Sint32 render;

        njPrintC(NJM_LOCATION(1, 17), "SH-4:");
        njPrintD(NJM_LOCATION(7, 17), frame, 5);
        njPrintD(NJM_LOCATION(13, 17), step, 5);
        njPrintC(NJM_LOCATION(1, 18), "CLX2:");
        render = (Sint32)game_state.render_time;
        njPrintD(NJM_LOCATION(7, 18), render, 5);
        if (frame > VBLANK) {
            timer_draw_bar(120, 276, 0xc0ff4040, frame);
        } else {
            timer_draw_bar(120, 276, 0xc040ff40, frame);
        }
        timer_draw_bar(120, 284, 0xc0ffff40, step);
        timer_draw_bar(120, 292, 0xc0ff40ff, draw);
        timer_draw_bar(120, 300, 0xc04040ff, render);
        njPrintC(NJM_LOCATION(1, 19), "SIZE:");
        njPrintC(NJM_LOCATION(1, 20), "FREE:");
        njPrintC(NJM_LOCATION(1, 21), "TBLK:");
        njPrintC(NJM_LOCATION(1, 22), "TEX :");
        njPrintC(NJM_LOCATION(1, 23), "TMAX:");
        njPrintC(NJM_LOCATION(1, 24), "VERT:");
        njPrintC(NJM_LOCATION(1, 25), "POLY:");
        njPrintD(NJM_LOCATION(7, 26), steps, 5);
        njPrintD(NJM_LOCATION(13, 26), draws, 5);
        njPrintD(NJM_LOCATION(19, 26), game_state.total_steps, 8);
    }
}
