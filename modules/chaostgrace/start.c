/* the start banner, and the steps by which every race banner fades in and
 * grows */
#include <ninja.h>
#include <sg_xpt.h>
#include "chao/race.h"
#include "chaostgrace/banner_draw.h"
#include "chaostgrace/banners.h"
#include "chaostgrace/race.h"
#include "system/task.h"

/* the start banner's modes. it shows READY for work->tick frames, a beat of
 * its race group's music. it then starts the race, shows START as long
 * again, and fades */
enum { START_INIT = 0, START_READY = 4, START_SHOW = 5, START_HIDE = 6 };
#define START_Y 220.0F

static Float start_depth = -1.15F;

void race_banner_reset(struct task *task)
{
    struct task_work *work = task->work;

    work->scl.x = 0.0F;
    work->scl.y = BANNER_FIRST_SCALE;
}

void race_banner_expand(struct task *task)
{
    struct task_work *work = task->work;

    work->scl.x += BANNER_STEP;
    if (work->scl.x > 1.0F) {
        work->scl.x = 1.0F;
    }
    if (work->scl.y < 1.0F) {
        work->scl.y += BANNER_STEP;
    } else {
        work->scl.y += BANNER_SWELL;
    }
}

static void start_banner_step(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case START_INIT:
        switch (garden_race_group()) {
        case RACE_BEGINNER:
            work->tick = RACE_BEGINNER_BEAT;
            break;
        case RACE_JEWEL:
            work->tick = RACE_JEWEL_BEAT;
            break;
        case RACE_CHALLENGE:
            work->tick = RACE_CHALLENGE_BEAT;
            break;
        case RACE_DARK_COURSES:
            work->tick = RACE_DARK_BEAT;
            break;
        case RACE_HERO_COURSES:
            work->tick = RACE_HERO_BEAT;
            break;
        case RACE_MULTI:
            work->tick = RACE_MULTI_BEAT;
            break;
        }
        work->timer = work->tick;
        race_banner_reset(task);
        work->mode = START_READY;
        work->submode = 0;
        break;
    case START_READY:
        race_banner_expand(task);
        if (--work->timer == 0) {
            work->timer = work->tick;
            race_banner_reset(task);
            race_set_mode(RACE_RUNNING);
            race_release_entrants();
            work->mode = START_SHOW;
            work->submode = 0;
        }
        break;
    case START_SHOW:
        race_banner_expand(task);
        if (--work->timer == 0) {
            work->mode = START_HIDE;
            work->submode = 0;
        }
        break;
    case START_HIDE:
        work->scl.x -= BANNER_STEP;
        work->scl.y += BANNER_SWELL;
        if (work->scl.x < 0.0F) {
            task_kill(task);
        }
        break;
    }
}

static void start_banner_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_SPRITE sprite;

    BANNER_POSITION(sprite, START_Y);
    sprite.sx = work->scl.y;
    sprite.sy = work->scl.y;
    BANNER_MATERIAL(sprite, work->scl.x);
    switch (work->mode) {
    case START_READY:
        BANNER_DRAW_CELL(sprite, BANNER_READY_LEFT, start_depth);
        BANNER_DRAW_CELL(sprite, BANNER_READY_RIGHT, start_depth);
        break;
    case START_SHOW:
    case START_HIDE:
        BANNER_DRAW_CELL(sprite, BANNER_START_LEFT, start_depth);
        BANNER_DRAW_CELL(sprite, BANNER_START_RIGHT, start_depth);
        break;
    }
    njFogEnable();
}

static void start_banner_destroy(struct task *task)
{
}

void race_start_banner_create(void)
{
    struct task *task = task_create(TASK_WORK, 2, start_banner_step,
                                    "ALR_ADV_StartManagerExecutor");

    task->draw = start_banner_draw;
    task->destructor = start_banner_destroy;
}
