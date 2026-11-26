/* the GOAL banner, which shows for GOAL_FRAMES frames and then fades out */
#include <ninja.h>
#include "chaostgrace/banner_draw.h"
#include "chaostgrace/banners.h"
#include "system/task.h"

#define GOAL_FRAMES 180
#define GOAL_Y 220.0F
#define GOAL_DEPTH (-1.15F)
enum { GOAL_INIT, GOAL_SHOW, GOAL_HIDE };

static void goal_banner_step(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case GOAL_INIT:
        race_banner_reset(task);
        work->mode = GOAL_SHOW;
        work->submode = 0;
        work->timer = GOAL_FRAMES;
        break;
    case GOAL_SHOW:
        race_banner_expand(task);
        if (--work->timer == 0) {
            work->mode = GOAL_HIDE;
            work->submode = 0;
        }
        break;
    case GOAL_HIDE:
        work->scl.x -= BANNER_STEP;
        work->scl.y += BANNER_SWELL;
        if (work->scl.x < 0.0F) {
            task_kill(task);
        }
        break;
    }
}

static void goal_banner_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_SPRITE sprite;

    BANNER_POSITION(sprite, GOAL_Y);
    if (work->scl.y < 1.0F) {
        sprite.sx = work->scl.y;
        sprite.sy = work->scl.y;
    } else {
        sprite.sx = 1.0F;
        sprite.sy = 1.0F;
    }
    BANNER_MATERIAL(sprite, work->scl.x);
    switch (work->mode) {
    case GOAL_SHOW:
    case GOAL_HIDE:
        BANNER_DRAW_CELL(sprite, BANNER_GOAL, GOAL_DEPTH);
        break;
    }
    njFogEnable();
}

static void goal_banner_destroy(struct task *task)
{
}

void race_goal_banner_create(void)
{
    struct task *task = task_create(TASK_WORK, 2, goal_banner_step,
                                    "ALR_ADV_GoalManagerExecutor");

    task->draw = goal_banner_draw;
    task->destructor = goal_banner_destroy;
}
