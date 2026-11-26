/* the colored torch: a flickering light on the players near it. it draws
 * nothing. Death Chamber's and Egg Quarters' object lists call it TAIMATSU,
 * and Pyramid Cave's calls it LIGHT.
 *
 * Death Chamber's image places this source as it stands. Egg Quarters'
 * copy, stg26/color_torch.c, includes it with COLOR_TORCH_HIDING_LIGHTS
 * defined. Pyramid Cave's copy, stg28/cave_color_torch.c, defines its own
 * COLOR_TORCH_THIRD_GREEN and COLOR_TORCH_THIRD_UNREAD, and its own
 * COLOR_TORCH_COLOR_LABEL and COLOR_TORCH_LAST_NAME */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/light.h"
#include "object/set.h"
#include "shared/color_torch.h"
#include "shared/object_light.h"
#include "system/game_state.h"
#include "system/task.h"

#ifdef COLOR_TORCH_HIDING_LIGHTS
#include "stg26/hiding_spot.h"
#endif

#define TORCH_PRESETS 3
/* the index of the task's work pointer among the task's words */
#define TASK_WORK_WORD 8

static void color_torch_step(struct task *task);
static void color_torch_destroy(struct task *task);
static void color_torch_draw(struct task *task);

/* the orange preset's green, and the third float of dat_color_torch_0's
 * third row */
#ifndef COLOR_TORCH_THIRD_GREEN
#define COLOR_TORCH_THIRD_GREEN 0.75F
#endif
#ifndef COLOR_TORCH_THIRD_UNREAD
#define COLOR_TORCH_THIRD_UNREAD 0.125F
#endif

/* the set editor's labels, and its name for each preset, which no code
 * reads. range_label's size starts color_label on a word boundary. only
 * Pyramid Cave's last name, ORANGE, matches its preset's color */
#ifndef COLOR_TORCH_COLOR_LABEL
#define COLOR_TORCH_COLOR_LABEL "<- COLOR(     )"
#define COLOR_TORCH_LAST_NAME "RED   "
#endif
static const char range_label[16] = "<- LIGHT RANGE";
static const char color_label[] = COLOR_TORCH_COLOR_LABEL;
static char *color_names[TORCH_PRESETS] = {"BLUE  ", "GREEN ",
                                           COLOR_TORCH_LAST_NAME};

/* the lights scl.z picks from: pink, pale yellow and orange.
 * object_light_add_near replaces each copy's dir */
static struct light color_torch_presets[TORCH_PRESETS] = {
    {{0.3F, 0.1F, 1.0F}, 100.0F, 0.3F, 1.0F, 0.75F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 100.0F, 0.3F, 1.0F, 1.0F, 0.5F},
    {{0.3F, 0.1F, 1.0F}, 100.0F, 0.3F, 1.0F, COLOR_TORCH_THIRD_GREEN, 0.25F},
};
/* four floats for each preset, which no code reads */
static Float dat_color_torch_0[TORCH_PRESETS][4] = {
    {0.25F, 0.1F, 0.125F, 0.333F},
    {0.25F, 0.1F, 0.333F, 0.125F},
    {0.25F, 0.333F, COLOR_TORCH_THIRD_UNREAD, 0.1F},
};

void color_torch_init(struct task *task)
{
    if (!placement_try_leave_by(task, task->work->scl.x * task->work->scl.x)) {
        task->draw = color_torch_draw;
        task->destructor = color_torch_destroy;
        task->step = color_torch_step;
    }
}

static void color_torch_destroy(struct task *task)
{
}

static void color_torch_step(struct task *task)
{
    struct task_work *work = task->work;
    struct light *light;
    int player;

    /* @hack type: the range test reads the work through its word in the
     * task where task->work would do */
#define RANGE (((struct task_work **)task)[TASK_WORK_WORD]->scl.x)
#ifdef COLOR_TORCH_HIDING_LIGHTS
    /* @hack redundant: a discarded read of the range */
#define RANGE2 ((void)RANGE, RANGE * RANGE)
#else
#define RANGE2 (RANGE * RANGE)
#endif
    if (!placement_try_leave_by(task, RANGE2)) {
        /* @hack spelling: presets, a local for color_torch_presets, where
         * the table read in place would do */
        struct light *presets = color_torch_presets;

#ifndef COLOR_TORCH_HIDING_LIGHTS
        /* @hack redundant: a block no path reaches */
        if (0) {
            if (task && task && task) { /* NOLINT(misc-redundant-expression) */
            }
        }
        /* @hack redundant: a block no path reaches */
        if (0) {
            if (presets) {
            }
        }
#endif
        /* @hack spelling: player's start above the mode test, where the
         * loop's own would do */
        player = 0;
        /* @hack redundant: a test of the work's mode no path makes */
        if (1) {
        } else if (task->work->mode < TORCH_PRESETS) {
        }
        for (; player < GAME_PLAYERS; player++) {
#ifdef COLOR_TORCH_HIDING_LIGHTS
            if (!hiding_light_contacts[player] &&
                !hiding_light_previous[player]) {
#endif
                /* @hack spelling: task->work read again, where work would
                 * do */
                light = object_light_add_near(
                    &presets[(int)njAbs(task->work->scl.z) % TORCH_PRESETS],
                    player, &work->pos, task->work->scl.x);
                if (light != NULL) {
                    object_light_jitter(light);
                }
#ifdef COLOR_TORCH_HIDING_LIGHTS
            }
#endif
        }
    }
}

static void color_torch_draw(struct task *task)
{
}
