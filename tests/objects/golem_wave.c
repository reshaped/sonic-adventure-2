/* golem_wave_create where the heap can't give the wave's record: the call
 * destroys the wave's task and returns null. with the record, it returns the
 * task */
#include <assert.h>

#include "boss_golem/tactics.c"

struct golem *golem;
Float golem_floor_y;

static struct task wave_task;
static struct task_work wave_work;
static struct golem_wave wave;
static Bool refuse_wave;
static int destroys;

struct task *task_create_child(Uint16 flags, void (*step)(struct task *task),
                               struct task *parent)
{
    wave_task.work = &wave_work;
    return &wave_task;
}

void task_destroy(struct task *task)
{
    assert(task == &wave_task);
    destroys++;
}

void *syMalloc(Uint32 size)
{
    assert(size == sizeof(struct golem_wave));
    return refuse_wave ? NULL : &wave;
}

void golem_polar_xz(Angle ang, Float radius, Float *x, Float *z)
{
    *x = 0.0F;
    *z = radius;
}

Bool collision_init(struct task *task, const struct collision_shape *shapes,
                    int count, Uint8 kind)
{
    return TRUE;
}

int main(void)
{
    static struct golem state;
    static struct task parent;

    golem = &state;
    refuse_wave = TRUE;
    assert(golem_wave_create(&parent, FOOTHOLD_WHOLE) == NULL);
    assert(destroys == 1);
    refuse_wave = FALSE;
    assert(golem_wave_create(&parent, FOOTHOLD_WHOLE) == &wave_task);
    assert(destroys == 1 && wave_task.extra.p == &wave);
    return 0;
}
