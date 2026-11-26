/* Garden pickups must either own everything needed by the first tick, or
 * leave their callers free to retry. Only that tick makes a pickup visible. */
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ninja.h>
#include <shinobi.h>

#include "chao/body_collision.h"
#include "chao/fruit.h"
#include "chao/garden.h"
#include "chao/garden_scene.h"
#include "chao/life.h"
#include "chao/pickup.h"
#include "chao/seed.h"
#include "chao/world.h"
#include "chao/world_slots.h"
#include "object/fall.h"
#include "object/mover.h"
#include "player/player.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/timer.h"

static int allocations, fail_at, live_blocks;
static struct task owners[GARDEN_ROW_SLOTS];
static struct task_work owner_work[GARDEN_ROW_SLOTS];
static NJS_POINT3 position = {17.5F, 2.25F, -5.0F};
static NJS_VECTOR velocity = {-1.0F, 3.5F, 0.25F};

struct pickup {
    struct task *(*create)(int, NJS_POINT3 *, Angle, NJS_VECTOR *);
    int category, level, kind, lower_id;
    Float carry_height, carry_radius;
};
static const struct pickup pickups[] = {
    {garden_fruit_create, GARDEN_OBJECT_FRUIT, 4, GARDEN_FRUIT_CALM, -125, 1.0F,
     2.0F},
    {garden_seed_create, GARDEN_OBJECT_SEED, 2, 4, -123, 0.2F, 0.35F},
    {garden_drop_headgear, GARDEN_OBJECT_HEADGEAR, 4, 2, -109, 0.0F, 1.4F}};

void *syMalloc(Uint32 size)
{
    void *block;
    if (++allocations == fail_at) {
        return NULL;
    }
    block = malloc(size);
    assert(block);
    memset(block, 0xa5, size);
    ++live_blocks;
    return block;
}

void *syCalloc(Uint32 count, Uint32 size)
{
    void *block = syMalloc(count * size);
    if (block) {
        memset(block, 0, count * size);
    }
    return block;
}

void syFree(void *block)
{
    assert(block && live_blocks > 0);
    free(block);
    --live_blocks;
}

void njMemCopy(void *dst, const void *src, Sint32 size)
{
    memcpy(dst, src, (size_t)size);
}
Float njScalor(NJS_VECTOR *v)
{
    return (Float)sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}
Float njSqrt(Float value)
{
    return (Float)sqrt(value);
}
int player_character(int slot)
{
    return PLAYER_EGGMAN;
}
void sound_detach_task(const struct task *task)
{
    assert(task != NULL);
}
void garden_age_reset(void)
{
}
void garden_enable_objects(void)
{
}
void profile_step_start(void (*step)(struct task *), const char *name)
{
    assert(task_current_call == step);
}
void profile_step_end(void (*step)(struct task *))
{
    (void)step;
}

static void check_owners(int category, int count)
{
    int i;
    for (i = 0; i < count; ++i) {
        assert(garden_find_object(category, i) == &owners[i]);
        assert(garden_active_object(category, i) == &owners[i]);
        assert(owners[i].force.p == garden_find_entry(category, i));
    }
}

static void check_unpublished(int category, int count)
{
    int i;
    assert(garden_object_count(category) == count);
    check_owners(category, count);
    assert(!garden_active_object(category, count));
    assert(!garden_active_entry(category, count));
    for (i = count; i < (int)garden_object_capacities[category]; ++i) {
        assert(!garden_find_object(category, i));
        assert(!garden_find_entry(category, i));
    }
}

static struct task *create(const struct pickup *pickup, NJS_VECTOR *speed)
{
    return pickup->create(pickup->kind, &position, 0x3456, speed);
}

static void check_pose(const struct pickup *pickup, struct task *task,
                       NJS_VECTOR *speed)
{
    NJS_VECTOR zero = {0};
    struct mover *mover = task->motion.p;
    struct task_work *work = task->work;
    assert(work && mover);
    assert(!memcmp(&work->pos, &position, sizeof(position)));
    assert(!memcmp(&mover->spd, speed ? speed : &zero, sizeof(zero)));
    assert(work->ang.x == pickup->kind);
    if (pickup->category != GARDEN_OBJECT_SEED) {
        assert(work->ang.y == 0x3456);
    }
}

static int check_start(const struct pickup *pickup, int occupied,
                       NJS_VECTOR *speed)
{
    struct task *task;
    struct collision_work *collision;
    struct garden_entry *entry;
    void (*init)(struct task *);
    int before = allocations;
    int used;

    task = create(pickup, speed);
    assert(task && task == task_levels[pickup->level] && task->next == task);
    assert(task->step && !task->draw);
    check_pose(pickup, task, speed);
    init = task->step;
    check_unpublished(pickup->category, occupied);
    used = allocations - before;
    assert(used >= 5); /* task, task work, mover, collision and its shapes */
    /* Even a loss of heap availability between constructor and first tick
     * cannot prevent the task from publishing its reserved slot. */
    fail_at = allocations + 1;
    task_step_level(pickup->level);
    assert(allocations == before + used);
    fail_at = 0;
    assert(task->step && task->step != init && task->draw && task->destructor);
    assert(task->work->mode == PICKUP_DROP && !task->work->submode);
    assert(task->work->id == FALL_KIND_NORMAL);
    check_pose(pickup, task, speed);
    collision = task->work->collision;
    assert(collision && collision->owner == task && collision->shapes);
    assert(collision->kind == COLLISION_OBJECT_MUTUAL);
    assert(collision->count == PICKUP_SHAPES);
    assert(collision->shapes[PICKUP_CONTACT].form == SHAPE_SPHERE);
    assert(collision->shapes[PICKUP_LOWER].id == pickup->lower_id);
    if (pickup->category == GARDEN_OBJECT_FRUIT) {
        assert(task->work->ang.z == 4);
    }
    entry = garden_find_entry(pickup->category, occupied);
    assert(entry && entry == task->force.p && entry->task == task);
    assert(entry == garden_active_entry(pickup->category, occupied));
    assert(entry->carry_height == pickup->carry_height);
    assert(entry->carry_radius == pickup->carry_radius);
    assert(garden_object_count(pickup->category) == occupied + 1);
    task_kill(task);
    task_step_level(pickup->level);
    assert(!task_levels[pickup->level] && !live_blocks);
    check_unpublished(pickup->category, occupied);
    return used;
}

static void allocation_failures(const struct pickup *pickup)
{
    int i, allocation_count;
    int category = pickup->category;
    owners[0].work = &owner_work[0];
    assert(garden_register_object(category, &owners[0], 0));
    allocation_count = check_start(pickup, 1, NULL);
    for (i = 1; i <= allocation_count; ++i) {
        fail_at = allocations + i;
        assert(!create(pickup, &velocity));
        assert(allocations == fail_at);
        assert(!task_levels[pickup->level] && !live_blocks);
        check_unpublished(category, 1);
        fail_at = 0;
        assert(check_start(pickup, 1, &velocity) == allocation_count);
    }
    assert(garden_unregister_object(&owners[0]));
    check_unpublished(category, 0);
}

static void capacity_and_teardown(const struct pickup *pickup)
{
    struct task *first, *second;
    int category = pickup->category;
    int count = garden_object_capacities[category];
    int i, held_blocks;

    for (i = 0; i < count; ++i) {
        owners[i].work = &owner_work[i];
        assert(garden_register_object(category, &owners[i], 0));
    }
    assert(!create(pickup, &velocity));
    assert(!live_blocks && !task_levels[pickup->level]);
    check_unpublished(category, count);
    assert(garden_unregister_object(&owners[--count]));
    first = create(pickup, &velocity);
    assert(first && !first->draw);
    held_blocks = live_blocks;
    assert(!create(pickup, NULL));
    assert(task_levels[pickup->level] == first && first->next == first);
    assert(live_blocks == held_blocks);
    /* A normal synchronous registration cannot take the reserved slot. */
    assert(!garden_register_object(category, &owners[count], 0));
    check_unpublished(category, count);
    task_destroy_level(pickup->level);
    assert(!live_blocks && !task_levels[pickup->level]);
    check_unpublished(category, count);
    check_start(pickup, count, &velocity);

    assert(garden_unregister_object(&owners[--count]));
    first = create(pickup, &velocity);
    second = create(pickup, NULL);
    assert(first && second && first != second);
    assert(!create(pickup, NULL));
    check_unpublished(category, count);
    task_step_level(pickup->level);
    assert(garden_find_object(category, count) == first);
    assert(garden_find_object(category, count + 1) == second);
    assert(garden_object_count(category) == count + 2);
    check_owners(category, count);
    task_destroy_level(pickup->level);
    assert(!live_blocks && !task_levels[pickup->level]);
    check_unpublished(category, count);
    while (count) {
        assert(garden_unregister_object(&owners[--count]));
    }
}

static void shared_seed_slots(void)
{
    struct task toys[GARDEN_ROW_SLOTS + 1] = {{0}};
    struct task *seed;
    int i;

    seed = create(&pickups[1], NULL);
    assert(seed);
    for (i = 0; i <= GARDEN_ROW_SLOTS; ++i) {
        assert(garden_register_object(GARDEN_OBJECT_TOY, &toys[i], 0));
    }
    /* Toy slot 16 aliases the seed's reservation at seed slot 0. */
    assert(!garden_find_object(GARDEN_OBJECT_TOY, GARDEN_ROW_SLOTS));
    assert(garden_find_object(GARDEN_OBJECT_SEED, 1) ==
           &toys[GARDEN_ROW_SLOTS]);
    assert(!garden_active_object(GARDEN_OBJECT_SEED, 0));
    task_step_level(2);
    assert(garden_find_object(GARDEN_OBJECT_SEED, 0) == seed);
    assert(garden_active_object(GARDEN_OBJECT_SEED, 0) == seed);
    assert(garden_object_count(GARDEN_OBJECT_SEED) == 1);
    assert(garden_object_count(GARDEN_OBJECT_TOY) == GARDEN_ROW_SLOTS + 1);
    task_destroy(seed);
    for (i = 0; i <= GARDEN_ROW_SLOTS; ++i) {
        assert(garden_unregister_object(&toys[i]));
    }
    assert(!live_blocks);
}

static void replaced_garden(const struct pickup *pickup)
{
    struct task *old, *stale, *current, *second;

    assert(garden_control_start());
    old = create(pickup, NULL);
    stale = create(pickup, NULL);
    assert(old && stale);
    assert(garden_control_start());
    current = create(pickup, &velocity);
    second = create(pickup, NULL);
    assert(current && second);
    /* The replacement world has reused the old reservations. A late
     * destructor and a late first tick must both leave its claims intact. */
    task_destroy(old);
    check_unpublished(pickup->category, 0);
    task_step_level(pickup->level);
    assert(garden_find_object(pickup->category, 0) == current);
    assert(garden_find_object(pickup->category, 1) == second);
    assert(task_levels[pickup->level] == current);
    assert(current->next == second && second->next == current);
    assert(garden_object_count(pickup->category) == 2);
    task_destroy_level(pickup->level);
    task_destroy(garden_controller);
    assert(!live_blocks);
}

static void earlier_vacancy(const struct pickup *pickup)
{
    int category = pickup->category;
    struct task *task;

    assert(garden_register_object(category, &owners[0], 0));
    assert(garden_register_object(category, &owners[1], 0));
    task = create(pickup, NULL);
    assert(task && garden_unregister_object(&owners[0]));
    task_step_level(pickup->level);
    /* The reservation guarantees capacity. As in the retail first tick,
     * registration still takes the first slot available on that frame. */
    assert(garden_find_object(category, 0) == task);
    assert(garden_find_object(category, 1) == &owners[1]);
    assert(garden_register_object(category, &owners[0], 0));
    assert(garden_find_object(category, 2) == &owners[0]);
    assert(garden_object_count(category) == 3);
    assert(garden_unregister_object(&owners[0]));
    assert(garden_unregister_object(&owners[1]));
    task_destroy(task);
    assert(!live_blocks);
    check_unpublished(category, 0);
}

int main(void)
{
    size_t i;
    task_init();
    assert(garden_control_start());
    task_destroy(garden_controller);
    for (i = 0; i < sizeof(pickups) / sizeof(*pickups); ++i) {
        allocation_failures(&pickups[i]);
        capacity_and_teardown(&pickups[i]);
        replaced_garden(&pickups[i]);
        earlier_vacancy(&pickups[i]);
    }
    shared_seed_slots();
    return 0;
}
