/* Dry Lagoon's butterflies, whose first step heads each from where it starts
 * to its first place in the flock */
#include <assert.h>
#include <string.h>

#include <ninja.h>

#include "object/placement.h"
#include "stg18/butterflies.h"
#include "stg18/lagoon_util.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/task.h"
#include "tests/stages/object_world.h"

#define BUTTERFLIES 3

struct game_state game_state;

/* each butterfly's move as its first step heads it, and the point its draw
 * translates to after that step, in the flock's order */
static NJS_VECTOR moves[BUTTERFLIES];
static NJS_POINT3 drawn[BUTTERFLIES];
static int heads, draws;

void direction_angles_xy(Float x, Float y, Float z, Angle *ax, Angle *ay)
{
    assert(heads < BUTTERFLIES);
    moves[heads].x = x;
    moves[heads].y = y;
    moves[heads].z = z;
    heads++;
    *ax = *ay = 0;
}

void lagoon_model_draw_scaled(NJS_CNK_MODEL *model, Float scale)
{
    assert(draws < BUTTERFLIES);
    drawn[draws++] = translation;
}

/* the flock's models are never built here */
void lagoon_direct_draw_scaled(NJS_DIRECT_HEAD *model, Float scale)
{
    assert(FALSE);
}

/* a flock of BUTTERFLIES about (100, 50, 20) */
static void flock_start(struct task *task)
{
    task->work->pos.x = 100.0F;
    task->work->pos.y = 50.0F;
    task->work->pos.z = 20.0F;
    task->work->ang.x = 0x1000;
    task->work->ang.z = BUTTERFLIES;
    task->work->scl.x = 30.0F;
    butterflies_init(task);
}

int main(void)
{
    struct placement placement;
    struct task *flock;
    int i;

    memset(&placement, 0, sizeof(placement));
    task_init();
    flock = object_start(TASK_WORK, flock_start, &placement);
    assert(flock != NULL);
    task_step_level(OBJECT_LEVEL);
    flock->draw(flock);
    assert(heads == BUTTERFLIES && draws == BUTTERFLIES);
    for (i = 0; i < BUTTERFLIES; i++) {
        assert(moves[i].x == drawn[i].x - flock->work->pos.x);
        assert(moves[i].y == drawn[i].y - flock->work->pos.y);
        assert(moves[i].z == drawn[i].z - flock->work->pos.z);
    }
    task_destroy(flock);
    assert(heap_live == 0);
    return 0;
}
