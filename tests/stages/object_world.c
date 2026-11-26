/* the heap, the collision world's pool, the collision records and the set, as
 * a stage object's test needs them, and the Ninja calls an object makes. no
 * matrix moves here: njCalcPoint copies its point, and njTranslateEx keeps
 * its vector in translation */
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <ninja.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/placement.h"
#include "object/set.h"
#include "player/player.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/timer.h"
#include "tests/stages/object_world.h"

/* the objects world_pool_alloc lends */
#define POOL_ENTRIES 4
/* the objects the world holds at once */
#define WORLD_ENTRIES 8
/* an Angle unit in radians */
#define ANGLE_RADIANS (2.0 * 3.14159265358979 / 65536.0)

int heap_live;
int heap_fail_at;
int pool_left;
int pool_lent;
int world_live;
Bool collision_fails;
NJS_POINT3 translation;

/* the allocations since object_start began its step */
static int heap_attempts;
static NJS_OBJECT pool[POOL_ENTRIES];
static Bool pool_taken[POOL_ENTRIES];
/* the world's objects in the order world_add put them in, each with its
 * task */
static struct {
    struct task *task;
    NJS_OBJECT *object;
} world[WORLD_ENTRIES];

void *syMalloc(Uint32 size)
{
    void *block;

    if (++heap_attempts == heap_fail_at) {
        return NULL;
    }
    block = malloc(size != 0 ? size : 1);
    assert(block != NULL);
    memset(block, 0xa5, size);
    heap_live++;
    return block;
}

void *syCalloc(Uint32 count, Uint32 size)
{
    void *block = syMalloc(count * size);

    if (block != NULL) {
        memset(block, 0, count * size);
    }
    return block;
}

/* a null block changes nothing, as the heap's own syFree */
void syFree(void *block)
{
    if (block != NULL) {
        assert(heap_live > 0);
        heap_live--;
        free(block);
    }
}

NJS_OBJECT *world_pool_alloc(void)
{
    int i;

    if (pool_left == 0) {
        return NULL;
    }
    for (i = 0; pool_taken[i]; i++) {
        assert(i + 1 < POOL_ENTRIES);
    }
    pool_left--;
    pool_lent++;
    pool_taken[i] = TRUE;
    memset(&pool[i], 0xa5, sizeof(pool[i]));
    pool[i].model = NULL;
    pool[i].child = NULL;
    pool[i].sibling = NULL;
    return &pool[i];
}

void world_pool_free(NJS_OBJECT *object)
{
    int i = object - pool;

    assert(i >= 0 && i < POOL_ENTRIES && pool_taken[i]);
    pool_taken[i] = FALSE;
    pool_lent--;
}

void world_add(Sint32 flags, struct task *task, NJS_OBJECT *object)
{
    assert(object != NULL && world_live < WORLD_ENTRIES);
    world[world_live].task = task;
    world[world_live].object = object;
    world_live++;
}

/* take out the last object task put in the world, as world.c's does */
void world_remove(struct task *task, NJS_OBJECT *object)
{
    int i;

    for (i = world_live - 1; i >= 0 && world[i].task != task; i--) {
    }
    if (i < 0) {
        return;
    }
    world_live--;
    for (; i < world_live; i++) {
        world[i] = world[i + 1];
    }
}

/* make a record of count shapes, with COLLISION_TAIL bytes of
 * COLLISION_TAIL_BYTE past them */
Bool collision_init(struct task *task, const struct collision_shape *shapes,
                    int count, Uint8 kind)
{
    struct collision_work *collision;
    Uint32 size = count * sizeof(struct collision_shape);

    if (collision_fails) {
        return FALSE;
    }
    collision = syCalloc(1, sizeof(struct collision_work));
    collision->shapes = syMalloc(size + COLLISION_TAIL);
    memcpy(collision->shapes, shapes, size);
    memset((char *)collision->shapes + size, COLLISION_TAIL_BYTE,
           COLLISION_TAIL);
    collision->count = count;
    collision->kind = kind;
    collision->owner = task;
    task->work->collision = collision;
    return TRUE;
}

Bool collision_tail_intact(const struct task *task)
{
    const struct collision_work *collision = task->work->collision;
    const Uint8 *tail = (const Uint8 *)(collision->shapes + collision->count);
    int i;

    for (i = 0; i < COLLISION_TAIL; i++) {
        if (tail[i] != COLLISION_TAIL_BYTE) {
            return FALSE;
        }
    }
    return TRUE;
}

void collision_free(struct task *task)
{
    struct collision_work *collision = task->work->collision;

    if (collision != NULL) {
        syFree(collision->shapes);
        syFree(collision);
        task->work->collision = NULL;
    }
}

Bool collision_register(struct task *task)
{
    return task->work->collision != NULL;
}

void collision_fit_radius(struct task *task)
{
}

Bool placement_try_leave(struct task *task)
{
    return FALSE;
}

Bool placement_try_leave_by(struct task *task, Float range)
{
    return FALSE;
}

Sint8 placement_get_state(struct task *task)
{
    return task->placement->state;
}

void placement_set_state(struct task *task, Sint8 state)
{
    task->placement->state = state;
}

int placement_used(struct task *task)
{
    return task->placement->flags & PLACEMENT_USED;
}

void placement_set_used(struct task *task)
{
    task->placement->flags |= PLACEMENT_USED;
}

void placement_kill(struct task *task)
{
    task_kill(task);
}

int player_near(NJS_POINT3 *pos, Float range)
{
    return FALSE;
}

void profile_step_start(void (*step)(struct task *task), const char *name)
{
}

void profile_step_end(void (*step)(struct task *task))
{
}

void sound_detach_task(const struct task *task)
{
}

Bool njPushMatrix(NJS_MATRIX *m)
{
    return TRUE;
}

Bool njPopMatrix(Uint32 n)
{
    return TRUE;
}

Bool njPushMatrixEx(void)
{
    return TRUE;
}

Bool njPopMatrixEx(void)
{
    return TRUE;
}

void njUnitMatrix(NJS_MATRIX *m)
{
}

void njTranslate(NJS_MATRIX *m, Float x, Float y, Float z)
{
}

void njTranslateV(NJS_MATRIX *m, NJS_VECTOR *v)
{
}

void njTranslateEx(NJS_VECTOR *v)
{
    translation = *v;
}

void njRotateX(NJS_MATRIX *m, Angle ang)
{
}

void njRotateY(NJS_MATRIX *m, Angle ang)
{
}

void njRotateZ(NJS_MATRIX *m, Angle ang)
{
}

void njRotateEx(Angle *ang, Sint32 lv)
{
}

void njCalcPoint(NJS_MATRIX *m, NJS_POINT3 *ps, NJS_POINT3 *pd)
{
    *pd = *ps;
}

void njGetTranslation(NJS_MATRIX *m, NJS_POINT3 *p)
{
    p->x = p->y = p->z = 0.0F;
}

Sint32 njSetTexture(NJS_TEXLIST *texlist)
{
    return 0;
}

Float njSin(Angle n)
{
    return (Float)sin(n * ANGLE_RADIANS);
}

Float njCos(Angle n)
{
    return (Float)cos(n * ANGLE_RADIANS);
}

Float njSqrt(Float n)
{
    return (Float)sqrt(n);
}

struct task *object_start(Uint16 flags, void (*init)(struct task *task),
                          struct placement *placement)
{
    int fail_at = heap_fail_at;
    struct task *task;

    heap_fail_at = 0;
    task = task_create(flags, OBJECT_LEVEL, init, "TEST");
    assert(task != NULL);
    task->placement = placement;
    placement->flags |= PLACEMENT_LIVE;
    heap_attempts = 0;
    heap_fail_at = fail_at;
    task_step_level(OBJECT_LEVEL);
    heap_fail_at = 0;
    if (task_levels[OBJECT_LEVEL] == NULL) {
        return NULL;
    }
    assert(task_levels[OBJECT_LEVEL] == task);
    return task;
}
