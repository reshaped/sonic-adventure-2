#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgentrance/menu.h"
#include "draw/screen.h"
#include "system/task.h"

static Float split_y = 317;
static Float bottom_y = 700;
static Float bottom_z = 0.1F;
static Float top_z = 1.0F / 60.5F;

enum dimmer_phase { DIMMER_IN, DIMMER_HOLD, DIMMER_OUT };
/* the work's tick holds the alpha. it rises by FADE_IN a frame to OPACITY,
 * and falls by FADE_OUT a frame once the menu leaves */
#define ALPHA(work) ((work)->tick)
#define OPACITY 104
#define FADE_IN 7
#define FADE_OUT 8

static void dimmer_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POLYGON_VTX vertices[4];
    Uint32 color;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    /* @hack redundant: color set to 0 before the alpha is or'ed in, where an
     * assignment of the alpha would do */
    color = 0;
    color |= (Uint32)ALPHA(work) << 24;
#define VERTEX(i, px, py, depth)                                               \
    vertices[i].x = (px);                                                      \
    vertices[i].y = (py);                                                      \
    vertices[i].z = (depth);                                                   \
    vertices[i].col = color
    VERTEX(0, 0, 0, top_z);
    VERTEX(1, 0, split_y, top_z);
    VERTEX(2, SCREEN_WIDTH, 0, top_z);
    VERTEX(3, SCREEN_WIDTH, split_y, top_z);
    njDrawPolygon(vertices, 4, TRUE);
    VERTEX(0, 0, split_y, top_z);
    VERTEX(1, 0, bottom_y, bottom_z);
    VERTEX(2, SCREEN_WIDTH, split_y, top_z);
    VERTEX(3, SCREEN_WIDTH, bottom_y, bottom_z);
    njDrawPolygon(vertices, 4, TRUE);
#undef VERTEX
}

static void dimmer_destroy(struct task *task)
{
    entrance_menu->dimmer = NULL;
}

static void dimmer_step(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case DIMMER_IN:
        if (ALPHA(work) < OPACITY) {
            ALPHA(work) += FADE_IN;
            entrance_menu->wait = TRUE;
        } else {
            ALPHA(work) = OPACITY;
            work->mode = DIMMER_HOLD;
            work->submode = 0;
        }
        break;
    case DIMMER_HOLD:
        break;
    case DIMMER_OUT:
        /* @hack spelling: > FADE_OUT - 1, where >= FADE_OUT would do */
        if (ALPHA(work) > FADE_OUT - 1) {
            ALPHA(work) -= FADE_OUT;
            entrance_menu->wait = TRUE;
        } else {
            task_kill(task);
        }
        break;
    }
    if (entrance_menu->request == ENTRANCE_MENU_LEAVE) {
        work->mode = DIMMER_OUT;
        work->submode = 0;
    }
}

void entrance_dimmer_create(struct task *parent)
{
    struct task *task;
    if (!entrance_menu->dimmer) {
        task = task_create_child(TASK_WORK, dimmer_step, parent);
        task->draw = dimmer_draw;
        task->destructor = dimmer_destroy;
        entrance_menu->dimmer = task;
    }
}
