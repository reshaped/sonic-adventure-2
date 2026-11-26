/* A bubble that reaches the surface: bubbles_step frees it, and writes no
 * freed memory. Its wobble still takes its three rand calls. */
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "effect/bubbles.c"

struct task *player_tasks[PLAYER_SLOTS];
struct task_work *player_work[PLAYER_SLOTS];
struct player *player_records[PLAYER_SLOTS];
static struct task_work player;
static struct player record;
static struct task_work work;
static struct task task;
static struct bubble_params params[PARAM_SETS];
static int rand_calls;
static int kills;

/* each roll is 0, below any wobble_rate over 0, so the wobble runs */
int rand(void)
{
    rand_calls++;
    return 0;
}

void *syMalloc(Uint32 size)
{
    return malloc(size);
}

void syFree(void *ptr)
{
    free(ptr);
}

void task_kill(struct task *killed)
{
    kills++;
}

Float njUnitVector(NJS_VECTOR *v)
{
    return 0.0F;
}

Float njScalor(NJS_VECTOR *v)
{
    return sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

Float njSqrt(Float n)
{
    return sqrtf(n);
}

int main(void)
{
    struct bubble *bubble = syMalloc(sizeof(*bubble));

    player_work[0] = &player;
    player_records[0] = &record;
    record.anim.frame = BREATH_FRAME + 1.0F;
    params[SET_CALM].wobble_rate = 0.5F;
    params[SET_CALM].wobble = 1.0F;
    task.work = &work;
    task.force.p = (struct task_force *)params;
    bubble->surface = 0.0F;
    bubble->pos.x = 1000.0F;
    bubble->pos.y = 10.0F;
    bubble->pos.z = 0.0F;
    bubble->spd.x = bubble->spd.y = bubble->spd.z = 0.0F;
    bubble->ang = 0;
    bubble->ang_spd = 1;
    bubble->size = bubble->max_size = 1.0F;
    bubble->prev = bubble->next = NULL;
    task.motion.p = (struct task_motion *)bubble;
    bubbles_step(&task);
    assert(task.motion.p == NULL && kills == 0);
    assert(rand_calls == 3);
    return 0;
}
