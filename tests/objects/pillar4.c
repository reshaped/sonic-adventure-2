/* pillar4_init where world_pool_alloc has no object to lend: the init destroys
 * the pillar, which clears its placement's PLACEMENT_LIVE. a pillar made again
 * for the placement, as the object list's pass makes one, stands in the
 * world */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <ninja.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/placement.h"
#include "player/player.h"
#include "shared/ocean_util.h"
#include "shared/pillar4.h"
#include "system/task.h"
#include "system/util.h"

#define LEVEL 2

NJS_CNK_MODEL pillar4_model;
NJS_OBJECT pillar4_object;
NJS_TEXLIST pillar4_texlist;

static NJS_OBJECT pooled;
static int pool_empty, lent, added, live, near_calls;

void *syMalloc(Uint32 size)
{
    void *block = malloc(size);

    assert(block != NULL);
    memset(block, 0xa5, size);
    live++;
    return block;
}

void *syCalloc(Uint32 count, Uint32 size)
{
    void *block = syMalloc(count * size);

    memset(block, 0, count * size);
    return block;
}

void syFree(void *block)
{
    assert(block != NULL && live > 0);
    live--;
    free(block);
}

void profile_step_start(void (*step)(struct task *), const char *name)
{
}

void profile_step_end(void (*step)(struct task *))
{
}

void sound_detach_task(const struct task *task)
{
}

Bool placement_try_leave(struct task *task)
{
    return FALSE;
}

NJS_OBJECT *world_pool_alloc(void)
{
    if (pool_empty) {
        return NULL;
    }
    assert(!lent);
    lent = 1;
    return &pooled;
}

void world_pool_free(NJS_OBJECT *object)
{
    assert(object == &pooled && lent);
    lent = 0;
}

void world_add(Sint32 flags, struct task *task, NJS_OBJECT *object)
{
    assert(flags == WORLD_SOLID && object == &pooled &&
           task->extra.p == object);
    added++;
}

void world_remove(struct task *task, NJS_OBJECT *object)
{
    assert(object == &pooled && added > 0);
    added--;
}

int player_near(NJS_POINT3 *pos, Float range)
{
    assert(lent && pos == (NJS_POINT3 *)pooled.pos);
    near_calls++;
    return 1;
}

/* the fixture makes no collision record and draws nothing */
void collision_free(struct task *task)
{
    abort();
}

void draw_control_set(Uint32 bits)
{
    abort();
}

void draw_control_clear(Uint32 bits)
{
    abort();
}

Bool njPushMatrixEx(void)
{
    abort();
}

Bool njPopMatrixEx(void)
{
    abort();
}

void njTranslateEx(NJS_VECTOR *v)
{
    abort();
}

void njRotateY(NJS_MATRIX *m, Angle ang)
{
    abort();
}

Sint32 njSetTexture(NJS_TEXLIST *texlist)
{
    abort();
}

void ocean_model_draw_scaled(NJS_CNK_MODEL *model, Float scale)
{
    abort();
}

/* the pass's part: a task for the placement, which it marks PLACEMENT_LIVE */
static struct task *place(struct placement *placement)
{
    struct task *task = task_create(TASK_WORK, LEVEL, pillar4_init, "PILLAR4");

    assert(task != NULL && !(placement->flags & PLACEMENT_LIVE));
    task->placement = placement;
    placement->flags |= PLACEMENT_LIVE;
    return task;
}

int main(void)
{
    struct placement placement;
    struct task *pillar;

    memset(&placement, 0, sizeof(placement));
    task_init();

    pool_empty = 1;
    place(&placement);
    task_step_level(LEVEL);
    assert(task_levels[LEVEL] == NULL && live == 0);
    assert(!(placement.flags & PLACEMENT_LIVE) && added == 0);

    pool_empty = 0;
    pillar = place(&placement);
    task_step_level(LEVEL);
    assert(task_levels[LEVEL] == pillar && pillar->extra.p == &pooled);
    assert(pillar->step != pillar4_init && added == 1);
    task_step_level(LEVEL);
    assert(near_calls == 1 && (pillar->work->flags & WORLD_ADDED));
    task_destroy(pillar);
    assert(task_levels[LEVEL] == NULL && live == 0 && !lent && added == 0);
    assert(!(placement.flags & PLACEMENT_LIVE));
    return 0;
}
