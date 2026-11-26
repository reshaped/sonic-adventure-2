/* the win and retry banners' task, which win.c and retry.c include. an
 * includer defines first:
 * - RESULT_CELL, the banner's cell;
 * - RESULT_SECOND_CELL, a cell drawn with it, where it has one;
 * - RESULT_Y, the screen y of the banner's center;
 * - RESULT_FRAMES, the frames the banner shows;
 * - RESULT_ASPECT, the banner's height for its width once grown;
 * - RESULT_HEIGHT(scale), its height for a width of scale below 1 */
#ifndef CHAOSTGRACE_RESULT_IMPL_H
#define CHAOSTGRACE_RESULT_IMPL_H
#include <ninja.h>
#include <sg_xpt.h>
#include "chaostgrace/banner_draw.h"
#include "chaostgrace/banners.h"
#include "chaostgrace/race.h"
#include "system/task.h"

enum { RESULT_INIT, RESULT_WAIT, RESULT_SHOW, RESULT_HIDE };
#define RESULT_DEPTH (-11.25F)

static void result_banner_step(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case RESULT_INIT:
        work->mode = RESULT_WAIT;
        work->submode = 0;
        work->timer = RACE_AWARD_DELAY;
        break;
    case RESULT_WAIT:
        if (--work->timer == 0) {
            race_banner_reset(task);
            work->mode = RESULT_SHOW;
            work->submode = 0;
            work->timer = RESULT_FRAMES;
        }
        break;
    case RESULT_SHOW:
        race_banner_expand(task);
        if (--work->timer == 0) {
            work->mode = RESULT_HIDE;
            work->submode = 0;
        }
        break;
    case RESULT_HIDE:
        work->scl.x -= BANNER_STEP;
        work->scl.y += BANNER_SWELL;
        if (work->scl.x < 0.0F) {
            task_kill(task);
        }
        break;
    }
}

static void result_banner_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_SPRITE sprite;

    BANNER_POSITION(sprite, RESULT_Y);
    if (work->scl.y < 1.0F) {
        sprite.sx = work->scl.y;
        sprite.sy = RESULT_HEIGHT(work->scl.y);
    } else {
        sprite.sx = 1.0F;
        sprite.sy = RESULT_ASPECT;
    }
    BANNER_MATERIAL(sprite, work->scl.x);
    switch (work->mode) {
    case RESULT_SHOW:
    case RESULT_HIDE:
        BANNER_DRAW_CELL(sprite, RESULT_CELL, RESULT_DEPTH);
#ifdef RESULT_SECOND_CELL
        BANNER_DRAW_CELL(sprite, RESULT_SECOND_CELL, RESULT_DEPTH);
#endif
        break;
    }
    njFogEnable();
}

static void result_banner_destroy(struct task *task)
{
}

#endif
