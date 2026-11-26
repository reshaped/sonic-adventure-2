#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <ninja.h>
#include <shinobi.h>

#include "enemy/enemy.h"
#include "object/placement.h"
#include "object/set.h"
#include "player/walker.h"
#include "sound/sound.h"
#include "stg21/stacked_platform.h"
#include "system/task.h"
#include "system/timer.h"

/* The two stage-specific art directories retain these model addresses. */
NJS_CNK_MODEL tier_bright_base, tier_bright_shell, tier_dark_base,
    tier_dark_shell;
NJS_TEXLIST tier_bright_base_texlist, tier_bright_shell_texlist;
NJS_TEXLIST tier_dark_base_texlist, tier_dark_shell_texlist;
NJS_CNK_MODEL tier_front_base, tier_front_shell, tier_side_base,
    tier_side_shell;
NJS_TEXLIST tier_front_base_texlist, tier_front_shell_texlist;
NJS_TEXLIST tier_side_base_texlist, tier_side_shell_texlist;

static int attempts, fail_at, live;

void *syMalloc(Uint32 size)
{
    void *block;

    if (++attempts == fail_at) {
        return NULL;
    }
    block = malloc(size);
    assert(block != NULL);
    memset(block, 0xa5, size);
    live++;
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

void syFree(void *block)
{
    assert(block != NULL && live > 0);
    live--;
    free(block);
}

void njMemCopy(void *dst, const void *src, Sint32 size)
{
    memcpy(dst, src, size);
}

Float njScalor(NJS_VECTOR *v)
{
    return (Float)sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

Float njSqrt(Float value)
{
    return (Float)sqrt(value);
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
void sound_stop_key(const void *key)
{
}
void lockon_release(struct enemy_work *enemy)
{
    abort(); /* These new targets have never acquired a lock. */
}

Bool placement_try_leave(struct task *task)
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

static int construct(struct placement *placement, int failed_allocation)
{
    struct task *stack =
        task_create(TASK_WORK, 2, stacked_platform_init, "DAI");
    struct task *tier;
    struct task *target;
    int index;

    assert(stack != NULL && !(placement->flags & PLACEMENT_LIVE));
    stack->placement = placement;
    placement->flags |= PLACEMENT_LIVE;
    stack->work->scl.z = 3.0F; /* four tiers */
    attempts = 0;
    fail_at = failed_allocation;
    task_step_level(2);
    fail_at = 0;
    if (failed_allocation) {
        assert(attempts == failed_allocation);
        assert(task_levels[2] == NULL && live == 0);
    } else {
        assert(stack == task_levels[2] && stack->work->tick == 4);
        tier = stack->child;
        for (index = 0; index < 4; index++) {
            int missing = (placement->state >> (index * 2)) & 3;
            int targets = 0;
            assert(tier != NULL && tier->work->ang.x == index);
            assert(((struct task **)stack->extra.p)[index] == tier);
            assert(tier->work->collision && tier->work->submode == missing);
            target = tier->child;
            if (target) {
                do {
                    assert(target->parent == tier && target->work->collision &&
                           target->motion.p);
                    targets++;
                    target = target->next;
                } while (target != tier->child);
            }
            assert(targets == 2 - !!(missing & 1) - !!(missing & 2));
            tier = tier->next;
        }
        assert(tier == stack->child);
        task_step_level(2);
        task_destroy(stack);
        assert(task_levels[2] == NULL && live == 0);
    }
    assert(!(placement->flags & PLACEMENT_LIVE));
    return attempts;
}

int main(void)
{
    struct placement placement;
    int count, failure;
    memset(&placement, 0, sizeof(placement));
    /* Whole, missing lower, missing upper, already broken. */
    placement.state = (Sint8)0xe4;
    task_init();
    count = construct(&placement, 0);
    for (failure = 1; failure <= count; failure++) {
        construct(&placement, failure);
        assert(placement.state == (Sint8)0xe4);
        assert(construct(&placement, 0) == count);
    }
    return 0;
}
