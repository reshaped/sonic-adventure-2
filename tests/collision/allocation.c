/* Allocation failures must not detach the record already owned by a task. */
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <shinobi.h>

#include "collision/collision.h"
#include "shared/common_object.h"
#include "system/task.h"

static int live_blocks;
static int fail_after;

void *syCalloc(Uint32 count, Uint32 size)
{
    void *block;

    if (fail_after && --fail_after == 0) {
        return NULL;
    }
    block = calloc(count, size);
    assert(block != NULL);
    live_blocks++;
    return block;
}

void syFree(void *block)
{
    assert(block != NULL && live_blocks > 0);
    free(block);
    live_blocks--;
}

void njMemCopy(void *dst, const void *src, Sint32 count)
{
    memcpy(dst, src, (size_t)count);
}

Float njScalor(NJS_VECTOR *v)
{
    return (Float)sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

Float njSqrt(Float value)
{
    return (Float)sqrt(value);
}

static void check_failures(struct task *task, struct collision_shape *shape)
{
    struct collision_work *previous = task->work->collision;
    struct collision_shape *previous_shapes =
        previous ? previous->shapes : NULL;
    int before = live_blocks;
    int allocation;

    for (allocation = 1; allocation <= 2; allocation++) {
        fail_after = allocation;
        assert(!collision_init(task, shape, 1, COLLISION_OBJECT));
        assert(task->work->collision == previous && live_blocks == before);
        fail_after = allocation;
        assert(!collision_init_attack_sphere(task, 8.0F, SHAPE_HURTABLE));
        assert(task->work->collision == previous && live_blocks == before);
        if (previous) {
            assert(previous->owner == task &&
                   previous->shapes == previous_shapes);
            assert(previous_shapes->radius == shape->radius);
        }
    }
}

/* Both pickup constructors instantiate holdable_setup.h. Only their draw
 * callbacks and the per-level model and texture selection differ here. */
static struct task attack;
static struct task_work attack_work;
static struct body body;
static NJS_CNK_MODEL model;
static NJS_TEXLIST textures[3];

static void idle(struct task *task)
{
    (void)task;
}
#define holdable_draw idle
#define holdable_step idle
#define holdable_destroy idle
#define HOLDABLE_SETUP static void setup_holdable
#include "shared/holdable_setup.h"

Bool placement_try_leave(struct task *task)
{
    (void)task;
    return FALSE;
}
void body_init(struct task *task)
{
    task->force.p = &body;
}
void task_destroy(struct task *task)
{
    (void)task;
    abort();
}
NJS_POINT3 **switch_presser_add(NJS_POINT3 *position)
{
    (void)position;
    return NULL;
}

struct task *task_create_child(Uint16 fields, void (*step)(struct task *),
                               struct task *parent)
{
    assert(fields == TASK_WORK);
    attack.work = &attack_work;
    attack.step = step;
    parent->child = &attack;
    return &attack;
}

static void check_holdable_shapes(int mode, Sint32 hold_turn)
{
    struct task task = {0};
    struct task_work work = {0};
    struct task_motion motion = {0};
    struct collision_work *record;

    task.work = &work;
    task.motion.p = &motion;
    setup_holdable(&task, textures, &model, hold_turn, mode);
    assert(task.extra.p == &model && task.draw == idle &&
           motion.hold_turn == hold_turn);
    record = work.collision;
    assert(record->kind == COLLISION_ITEM && record->count == 2);
    assert(record->shapes[0].form == SHAPE_CYLINDER);
    assert(record->shapes[1].form == SHAPE_CYLINDER);
    assert((record->shapes[1].push != 0) == ((mode & HOLDABLE_PUSHES) != 0));
    assert(attack_work.collision->kind == COLLISION_ATTACK);
    assert(attack_work.collision->count == 1);
    collision_free(&attack);
    collision_free(&task);
    assert(live_blocks == 0);
}

int main(void)
{
    struct task task = {0};
    struct task_work work = {0};
    struct collision_shape shape = {0};
    struct collision_work *record;

    check_holdable_shapes(OBJECT_MAY_LEAVE, 0x300);
    check_holdable_shapes(OBJECT_MAY_LEAVE | HOLDABLE_PUSHES, 0x1800);
    task.work = &work;
    shape.form = SHAPE_SPHERE;
    shape.radius = 8.0F;
    check_failures(&task, &shape);

    assert(collision_init_in_place(&task, &shape, 1, COLLISION_OBJECT));
    assert(work.collision->shapes == &shape && live_blocks == 1);
    check_failures(&task, &shape);
    collision_free(&task);
    assert(work.collision == NULL && live_blocks == 0);

    assert(collision_init(&task, &shape, 1, COLLISION_ENEMY));
    record = work.collision;
    assert(record->owner == &task && record->kind == COLLISION_ENEMY);
    assert(record->count == 1 && record->shapes != &shape);
    assert(record->shapes->radius == shape.radius &&
           record->radius > shape.radius);
    assert((record->flags & COLLISION_TARGET) &&
           (record->flags & COLLISION_OWN_SHAPES));
    assert(live_blocks == 2);
    check_failures(&task, &shape);
    collision_free(&task);
    assert(work.collision == NULL && live_blocks == 0);

    assert(collision_init_attack_swept(&task, shape.radius, SHAPE_HURTABLE));
    record = work.collision;
    assert(record->owner == &task && record->kind == COLLISION_ATTACK);
    assert(record->count == 1 && record->shapes->form == SHAPE_SWEPT_SPHERE);
    assert(record->shapes->flags == (SHAPE_HURTABLE | SHAPE_UNK_40));
    assert(record->shapes->radius == shape.radius && record->radius == 0.0F);
    collision_free(&task);
    assert(work.collision == NULL && live_blocks == 0);
    return 0;
}
