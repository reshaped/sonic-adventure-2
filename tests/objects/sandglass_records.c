/* cave_sandglass_create where the heap has no record for the sandglass, and
 * where it gives each block as an earlier owner left it */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <ninja.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/sandglass.h"
#include "stg28/sandglass.h"
#include "stg28/sandglass_registry.h"
#include "system/matrix.h"
#include "system/task.h"

/* the task the create destroyed, the syMalloc call that fails, counting
 * from 1, and the blocks syMalloc has given and syFree not taken back */
static struct task *destroyed;
static int fail_at, calls, live_blocks;

void *syMalloc(Uint32 size)
{
    void *block;

    if (++calls == fail_at) {
        return NULL;
    }
    block = malloc(size);
    assert(block != NULL);
    /* what an earlier owner of the block left in it */
    memset(block, 0xa5, size);
    live_blocks++;
    return block;
}

void syFree(void *block)
{
    if (block != NULL) {
        live_blocks--;
        free(block);
    }
}

static struct task *new_task(void (*step)(struct task *task))
{
    struct task *task = calloc(1, sizeof(*task));

    assert(task != NULL);
    task->step = step;
    task->work = calloc(1, sizeof(*task->work));
    assert(task->work != NULL);
    return task;
}

struct task *task_create(Uint16 flags, int level,
                         void (*step)(struct task *task), const char *name)
{
    (void)flags;
    (void)level;
    (void)name;
    return new_task(step);
}

struct task *task_create_child(Uint16 flags, void (*step)(struct task *task),
                               struct task *parent)
{
    struct task *child = new_task(step);

    (void)flags;
    child->parent = parent;
    return child;
}

void task_destroy(struct task *task)
{
    assert(destroyed == NULL);
    destroyed = task;
    if (task->destructor != NULL) {
        task->destructor(task);
    }
}

void task_kill(struct task *task)
{
    (void)task;
}

Bool collision_init(struct task *task, const struct collision_shape *shapes,
                    int count, Uint8 kind)
{
    (void)task;
    (void)shapes;
    (void)count;
    (void)kind;
    return TRUE;
}

Bool placement_try_leave(struct task *task)
{
    (void)task;
    return FALSE;
}

struct task **cave_sandglass_register(struct task *task)
{
    (void)task;
    return NULL;
}

void cave_sandglass_unregister(struct task *task)
{
    (void)task;
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
    (void)m;
}

void njTranslate(NJS_MATRIX *m, Float x, Float y, Float z)
{
    (void)m;
    (void)x;
    (void)y;
    (void)z;
}

void njRotateX(NJS_MATRIX *m, Angle ang)
{
    (void)m;
    (void)ang;
}

void njRotateY(NJS_MATRIX *m, Angle ang)
{
    (void)m;
    (void)ang;
}

void matrix_get(NJS_MATRIX *matrix)
{
    memset(matrix, 0, sizeof(*matrix));
}

int main(void)
{
    NJS_POINT3 pos = {0.0F, 0.0F, 0.0F};
    struct task *task;

    /* the second block is the record. without it, the create destroys its
     * task, and the failed alloc has freed the matrix */
    fail_at = 2;
    assert(cave_sandglass_create(&pos, 0, 0, 1) == NULL);
    assert(destroyed != NULL && live_blocks == 0);

    /* the tumble's turn and the tick's timer start at 0, whatever the
     * record's block held */
    fail_at = 0;
    destroyed = NULL;
    task = cave_sandglass_create(&pos, 0, 0, 1);
    assert(task != NULL && destroyed == NULL);
    assert(SANDGLASS(task)->tumble_turn == 0);
    assert(SANDGLASS(task)->tick_timer == 0);
    return 0;
}
