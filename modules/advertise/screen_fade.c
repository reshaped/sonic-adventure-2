/* the screen fade: a cover over the whole screen. the menus fade it in and
 * out across a change of menu */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/screen_fade.h"
#include "system/task.h"

/* nothing calls it. it isn't static, since SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* destroy the task, where there is one */
void screen_fade_end(void);
/* NOLINTEND(misc-use-internal-linkage) */

#define TASK_LEVEL 2
#define OPAQUE 255
#define FADE_STEP 8
/* the steps for which the cover holds its alpha once shown is 0 */
#define HOLD_STEPS 30
#define CORNERS 4

struct screen_fade_state {
    Sint32 alpha; /* the cover's, from 0 to OPAQUE */
    Sint32 hold; /* the steps left before the cover fades out */
};
static struct screen_fade_state screen_fade_state;

static struct screen_fade_state state_start = {OPAQUE, HOLD_STEPS};
/* the screen's corners, a strip of two triangles that covers it. the draw
 * sets their color */
static NJS_POLYGON_VTX cover[CORNERS] = {
#include "advertise/screen_fade/cover.inc"
};
static struct task *fade_task = NULL;

static void screen_fade_step(struct task *task)
{
    screen_fade_work.fading = FALSE;
    if (screen_fade_work.shown) {
        screen_fade_state.hold = HOLD_STEPS;
        if (menu_approach_int(&screen_fade_state.alpha, OPAQUE, FADE_STEP)) {
            screen_fade_work.fading = TRUE;
        }
    } else if (screen_fade_state.hold) {
        screen_fade_state.hold--;
    } else if (menu_approach_int(&screen_fade_state.alpha, 0, FADE_STEP)) {
        screen_fade_work.fading = TRUE;
    }
}

static void screen_fade_draw(struct task *task)
{
    NJS_COLOR color;

    if (screen_fade_state.alpha) {
        color = screen_fade_work.color;
        color.argb.a = screen_fade_state.alpha;
        cover[0].col = color.color;
        cover[1].col = color.color;
        cover[2].col = color.color;
        cover[3].col = color.color;
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njDrawPolygon(cover, CORNERS, TRUE);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
    }
}

static void screen_fade_destroy(struct task *task)
{
    /* @hack redundant: the test of loaded before it is cleared, where the
     * store alone would do */
    if (screen_fade_work.loaded) {
        screen_fade_work.loaded = FALSE;
    }
}

void screen_fade_create(void)
{
    struct task *task;
    Uint8 *byte;
    int left;

    task =
        task_create(TASK_WORK, TASK_LEVEL, screen_fade_step, "fadeManagerExec");
    task->draw = screen_fade_draw;
    task->destructor = screen_fade_destroy;
    fade_task = task;
    byte = (Uint8 *)&screen_fade_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(screen_fade_work); left != 0; left--) {
        *byte++ = 0;
    }
    screen_fade_state = state_start;
    screen_fade_work.shown = TRUE;
}

void screen_fade_end(void)
{
    if (fade_task != NULL) {
        task_destroy(fade_task);
        fade_task = NULL;
    }
}

void screen_fade_load(void)
{
    /* @hack redundant: the test of loaded before it is set, where the store
     * alone would do */
    if (!screen_fade_work.loaded) {
        screen_fade_work.loaded = TRUE;
    }
}

void screen_fade_release(void)
{
    /* @hack redundant: the test of loaded before it is cleared, where the
     * store alone would do */
    if (screen_fade_work.loaded) {
        screen_fade_work.loaded = FALSE;
    }
}
