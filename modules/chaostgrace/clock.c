/* the race's clock. a beginner or jewel race whose course has a record
 * shows the record's time too */
#include <ninja.h>
#include <sg_xpt.h>
#include "chao/race.h"
#include "chao/race_record.h"
#include "chaostgrace/banner_draw.h"
#include "chaostgrace/hud_art.h"
#include "chaostgrace/race.h"
#include "draw/lines.h"
#include "system/game_state.h"
#include "system/task.h"

static struct garden_race_time best_time;
static Bool show_best;
static NJS_POINT2 time_position = {
#include "chaostgrace/clock/time_position.inc"
};
static NJS_POINT2 best_position = {
#include "chaostgrace/clock/best_position.inc"
};

enum {
    CLOCK_TIME = 18,
    CLOCK_DIGITS = 19,
    CLOCK_TIME_SEPARATOR = 29,
    CLOCK_BEST = 30,
    CLOCK_BEST_SEPARATOR = 41
};

#define HUNDREDTHS_PER_SECOND 100
/* each time's digits and separators stand this far right of the time's x */
#define MINUTES_X 66.0F
#define FIRST_SEPARATOR_X 89.0F
#define SECONDS_X 102.0F
#define SECOND_SEPARATOR_X 126.0F
#define HUNDREDTHS_X 137.0F
#define DIGIT_WIDTH 13.0F
/* how far the times move down once the results show */
#define RESULTS_DROP 26.0F
#define CLOCK_DEPTH (-1.15F)

static void clock_step(struct task *task)
{
    if (race_mode() == RACE_CLOSED) {
        task_kill(task);
    }
}

/* @hack spelling: a value's ones as value - TENS(value) * 10, where
 * value % 10 would do */
#define TENS(value) ((value) / 10)
#define ONES(value) ((value) - (TENS(value) * 10))

static void clock_draw(struct task *task)
{
    NJS_SPRITE sprite;
    struct garden_race_time time;
    Float x;
    Float y;
    Float depth;
    Uint16 minutes;
    Uint16 seconds;
    Uint16 hundredths;
    int cell;

    /* @hack redundant: a call of race_mode under if (0), which never runs */
    if (0) {
        race_mode();
    }
    if (race_mode() == RACE_CLOSED) {
        return;
    }
    sprite.p.z = 0.0F;
    sprite.ang = 0;
    sprite.sy = sprite.sx = 1.0F;
    /* @hack spelling: the material set call by call, where
     * BANNER_MATERIAL(sprite, 1.0F) would do */
    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    constant_material_set(1.0F, 1.0F, 1.0F, 1.0F);
    sprite.tlist = &race_hud_texlist;
    sprite.tanim = RACE_HUD_CELLS;

    x = time_position.x;
    y = time_position.y;
    if (race_mode() >= RACE_SHOW_RESULTS) {
        y += RESULTS_DROP;
    }
    constant_material_set(1.0F, 1.0F, 1.0F, 0.0F);
    sprite.p.x = x;
    sprite.p.y = y;
    depth = CLOCK_DEPTH;
    BANNER_DRAW_CELL(sprite, CLOCK_TIME, depth);

    constant_material_set(1.0F, 1.0F, 1.0F, 1.0F);
    sprite.p.x = x + FIRST_SEPARATOR_X;
    BANNER_DRAW_CELL(sprite, CLOCK_TIME_SEPARATOR, depth);
    sprite.p.x = x + SECOND_SEPARATOR_X;
    BANNER_DRAW_CELL(sprite, CLOCK_TIME_SEPARATOR, depth);
    garden_race_get_time(&time);
    minutes = time.minutes;
    seconds = time.seconds;
    hundredths = time.frames * HUNDREDTHS_PER_SECOND / STEPS_PER_SECOND;
    sprite.p.x = x + MINUTES_X;
    sprite.p.y = y;
    cell = TENS(minutes) + CLOCK_DIGITS;
    BANNER_DRAW_CELL(sprite, cell, depth);
    sprite.p.x += DIGIT_WIDTH;
    sprite.p.y = y;
    cell = ONES(minutes) + CLOCK_DIGITS;
    BANNER_DRAW_CELL(sprite, cell, depth);
    sprite.p.x = x + SECONDS_X;
    sprite.p.y = y;
    cell = TENS(seconds) + CLOCK_DIGITS;
    BANNER_DRAW_CELL(sprite, cell, depth);
    sprite.p.x += DIGIT_WIDTH;
    sprite.p.y = y;
    cell = ONES(seconds) + CLOCK_DIGITS;
    BANNER_DRAW_CELL(sprite, cell, depth);
    sprite.p.x = x + HUNDREDTHS_X;
    sprite.p.y = y;
    cell = TENS(hundredths) + CLOCK_DIGITS;
    BANNER_DRAW_CELL(sprite, cell, depth);
    sprite.p.x += DIGIT_WIDTH;
    sprite.p.y = y;
    cell = ONES(hundredths);
    cell += CLOCK_DIGITS;
    BANNER_DRAW_CELL(sprite, cell, depth);

    x = best_position.x;
    y = best_position.y;
    if (race_mode() >= RACE_SHOW_RESULTS) {
        y += RESULTS_DROP;
    }
    constant_material_set(1.0F, 0.4F, 0.4F, 1.0F);
    sprite.sx = 1.0F;
    sprite.sy = 1.0F;
    if (show_best) {
        sprite.p.x = x;
        sprite.p.y = y;
        BANNER_DRAW_CELL(sprite, CLOCK_BEST, depth);
        constant_material_set(1.0F, 0.6F, 0.6F, 0.6F);
        /* the race's time again, which nothing below reads */
        garden_race_get_time(&time);
        minutes = best_time.minutes;
        seconds = best_time.seconds;
        hundredths =
            best_time.frames * HUNDREDTHS_PER_SECOND / STEPS_PER_SECOND;
        sprite.p.x = x + FIRST_SEPARATOR_X;
        sprite.p.y = y;
        BANNER_DRAW_CELL(sprite, CLOCK_BEST_SEPARATOR, depth);
        sprite.p.x = x + SECOND_SEPARATOR_X;
        sprite.p.y = y;
        BANNER_DRAW_CELL(sprite, CLOCK_BEST_SEPARATOR, depth);
        sprite.p.x = x + MINUTES_X;
        sprite.p.y = y;
        cell = TENS(minutes) + CLOCK_DIGITS;
        BANNER_DRAW_CELL(sprite, cell, depth);
        sprite.p.x += DIGIT_WIDTH;
        sprite.p.y = y;
        cell = ONES(minutes) + CLOCK_DIGITS;
        BANNER_DRAW_CELL(sprite, cell, depth);
        sprite.p.x = x + SECONDS_X;
        sprite.p.y = y;
        cell = TENS(seconds);
        cell += CLOCK_DIGITS;
        BANNER_DRAW_CELL(sprite, cell, depth);
        sprite.p.x += DIGIT_WIDTH;
        sprite.p.y = y;
        cell = ONES(seconds) + CLOCK_DIGITS;
        BANNER_DRAW_CELL(sprite, cell, depth);
        sprite.p.x = x + HUNDREDTHS_X;
        sprite.p.y = y;
        cell = TENS(hundredths) + CLOCK_DIGITS;
        BANNER_DRAW_CELL(sprite, cell, depth);
        sprite.p.x += DIGIT_WIDTH;
        sprite.p.y = y;
        cell = ONES(hundredths);
        cell += CLOCK_DIGITS;
        BANNER_DRAW_CELL(sprite, cell, depth);
    }
    constant_material_set(1.0F, 1.0F, 1.0F, 1.0F);
    njFogEnable();
}

static void clock_destroy(struct task *task)
{
}

void race_clock_create(void)
{
    struct task *task;
    struct garden_race_record record;

    task = task_create(TASK_WORK, 2, clock_step, "ALR_ADV_TimeManagerExecutor");
    task->draw = clock_draw;
    task->destructor = clock_destroy;
    if (garden_race_group() <= RACE_JEWEL) {
        garden_race_get_record(garden_race_event(), &record);
        best_time = record.time;
        if (best_time.minutes == 0 && best_time.seconds == 0 &&
            best_time.frames == 0) {
            show_best = FALSE;
        } else {
            show_best = TRUE;
        }
    }
}
