/* Run the race pickup lifecycle with the game's task ring, collision and
 * registry. The allocator can fail at any individual ownership transfer. */
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <shinobi.h>
#include "chao/behavior.h"
#include "chao/body_collision.h"
#include "chao/carry.h"
#include "chao/garden_field.h"
#include "chao/icon.h"
#include "chao/life.h"
#include "chao/model.h"
#include "chao/motion.h"
#include "chao/movement.h"
#include "chao/work.h"
#include "chao/world.h"
#include "chaostgrace/hunt_data.h"
#include "chaostgrace/movement.h"
#include "chaostgrace/obstacles.h"
#include "chaostgrace/props.h"
#include "chaostgrace/race.h"
#include "collision/collision.h"
#include "object/fall.h"
#include "object/mover.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/timer.h"
#include "system/util.h"

NJS_CNK_OBJECT scavenger_panel_a, scavenger_panel_b, scavenger_panel_c,
    scavenger_panel_d;
NJS_CNK_OBJECT scavenger_sign_a[1], scavenger_sign_b[1];
static int allocations, fail_at, blocks, wrong_sounds;
static chao_behavior_fn behavior;
static struct task owners[32], chao;
static struct chao_work racer;
static struct mover racer_motion;
static NJS_POINT3 position = {5.5F, 7.0F, -2.0F};
static NJS_VECTOR velocity = {0.25F, 0.8F, -0.1F};

void *syMalloc(Uint32 size)
{
    void *block;
    if (++allocations == fail_at) {
        return NULL;
    }
    block = malloc(size);
    assert(block);
    memset(block, 0xa5, size);
    ++blocks;
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
    assert(block && blocks > 0);
    free(block);
    --blocks;
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
Float njSin(Angle a)
{
    return (Float)sin(a / ANGLE_PER_RADIAN);
}
Float njCos(Angle a)
{
    return (Float)cos(a / ANGLE_PER_RADIAN);
}
Angle angle_approach(Angle from, Angle to, Angle step)
{
    return to;
}
int rand(void)
{
    return 1234;
}
void sound_detach_task(const struct task *task)
{
}
void profile_step_start(void (*step)(struct task *), const char *name)
{
}
void profile_step_end(void (*step)(struct task *))
{
}
Sint8 sound_play(int id, const void *key, Sint8 priority, Sint16 volume)
{
    assert(id == 4);
    ++wrong_sounds;
    return 0;
}
Sint8 sound_play_at_for(int id, const void *key, Sint8 priority, Sint16 volume,
                        Sint16 duration, const NJS_POINT3 *pos)
{
    return 0;
}
void chao_link_motion(struct task *task, Sint32 index)
{
}
void chao_link_walk_motion(struct task *task)
{
}
void chao_show_mark(struct task *task, Sint8 mark, int frames)
{
}
void chao_resist_displacement(struct task *task)
{
}
void chao_slow_on_floor(struct task *task, Float scale)
{
}
int chao_skill_points(const struct task *task, Uint16 skill)
{
    return 15000;
}
void chao_set_behavior(struct task *task, chao_behavior_fn next)
{
    assert(task == &chao);
    behavior = next;
}
int race_run(struct task *task)
{
    return BEHAVIOR_CONTINUE;
}
void race_run_forward(struct task *task)
{
}
int garden_turn_to_object(struct task *task, struct task *other, int step)
{
    return 0;
}
Bool garden_follow_chao(struct task *item)
{
    return FALSE;
}
void garden_animate_texture(NJS_CNK_MODEL *mesh, int texture)
{
}
struct task *garden_create_timed_field(struct task *task, int kind,
                                       NJS_POINT3 *pos, Float radius,
                                       int frames)
{
    return NULL;
}
static struct task field;
struct task *garden_timed_enemy_field(int kind, NJS_POINT3 *pos, Float radius,
                                      int frames)
{
    assert(kind == 170);
    return &field;
}
struct task *garden_hit_kind(struct task *task, int shape, Uint8 kind)
{
    return kind == 170 ? &field : NULL;
}

static int task_count(int level)
{
    int count = 0;
    struct task *task = task_levels[level];
    if (task) {
        do {
            ++count;
            task = task->next;
        } while (task != task_levels[level]);
    }
    return count;
}

static void reserve(int category, int count)
{
    int i;
    assert(!garden_object_count(category));
    for (i = 0; i < count; ++i) {
        assert(garden_register_object(category, &owners[i], 0));
    }
}

static void release(int count)
{
    while (count) {
        assert(garden_unregister_object(&owners[--count]));
    }
}

static void immediate_failures(void)
{
    int i;
    for (i = 1; i <= 3; ++i) {
        fail_at = allocations + i;
        assert(!race_fruit_create(&position, 0x3200, &velocity));
        assert(!task_levels[4] && !blocks);
        fail_at = allocations + i;
        assert(!race_scavenger_item_create(2, &position, 0x3200, &velocity));
        assert(!task_levels[4] && !blocks);
    }
    fail_at = 0;
}

/* The tree's private fruit pointer outlives the scheduled initializer. */
static void tree_drop(int failure)
{
    struct task *tree, *fruit;
    struct collision_work *collision;
    int count =
        failure == 3 ? garden_object_capacities[GARDEN_OBJECT_FRUIT] : 1;
    int before;
    reserve(GARDEN_OBJECT_FRUIT, count);
    race_trees_reset();
    tree = task_create(TASK_WORK, 2, race_tree_init, "tree");
    tree->work->pos = position;
    tree->work->ang.y = 0x4000;
    tree->step(tree);
    racer.race_target = tree;
    tree->work->mode = 1;
    tree->work->timer = 0;
    before = blocks;
    tree->step(tree);
    fruit = task_levels[4];
    assert(fruit && !fruit->draw && !race_tree_fruit(&chao));
    assert(fruit->work->pos.y == position.y + 30.0F);
    assert(((struct mover *)fruit->motion.p)->spd.x == 0.2F);
    if (failure == 1 || failure == 2) {
        fail_at = allocations + failure;
    }
    if (failure == 4) {
        task_destroy(fruit); /* teardown before the first scheduled tick */
    } else {
        task_step_level(4);
    }
    fail_at = 0;
    if (failure) {
        assert(!task_levels[4] && blocks == before && !race_tree_fruit(&chao));
        behavior = NULL;
        racer.behavior.mode = 1;
        race_shake_tree(&chao);
        assert(behavior == race_run && !racer.race_target);
    } else {
        collision = fruit->work->collision;
        assert(fruit->draw && collision && collision->owner == fruit);
        assert(collision->count == 2 && fruit->work->ang.z == 4);
        assert(race_tree_fruit(&chao) == fruit);
        assert(garden_find_object(GARDEN_OBJECT_FRUIT, count) == fruit);
        task_destroy(fruit);
        assert(!race_tree_fruit(&chao) && blocks == before);
    }
    assert(garden_object_count(GARDEN_OBJECT_FRUIT) == count);
    task_destroy(tree);
    release(count);
    assert(!blocks);
}

/* Search only advances after its ordinary interval; let the real scheduler
 * initialize any replacements in between two such checks. */
static int search_frames(int limit)
{
    int i, result;
    for (i = 0; i < limit; ++i) {
        result = race_find_answer_item(&chao);
        task_step_level(4);
        if (result != BEHAVIOR_CONTINUE || behavior == race_run) {
            return TRUE;
        }
    }
    return FALSE;
}

static void reset_search(void)
{
    ((struct garden_entry *)chao.force.p)->target = NULL;
    memset(&racer.behavior, 0, sizeof(racer.behavior));
    racer.race_choice = race_scavenger_target;
    behavior = NULL;
}

static void toy_failure(int failure)
{
    struct task *toy;
    int index = SCAVENGER_ITEMS - 1;
    int count =
        failure == 3 ? garden_object_capacities[GARDEN_OBJECT_TOY] - index : 0;
    int i, before;
    reserve(GARDEN_OBJECT_TOY, count);
    race_scavenger_start();
    for (i = 0; i < index; ++i) {
        toy = race_scavenger_items[i];
        toy->step(toy);
        assert(toy->force.p);
        toy->work->tick = (race_scavenger_target + 1) % SCAVENGER_LANES;
    }
    toy = race_scavenger_items[index];
    assert(toy && !toy->draw && !toy->force.p);
    before = blocks;
    if (failure >= 4) {
        /* The rack can also lose a replacement before its first tick. */
        task_destroy(toy);
        fail_at = allocations + failure - 3;
        race_respawn_scavenger_item(index);
    } else if (failure == 1 || failure == 2) {
        fail_at = allocations + failure;
    }
    task_step_level(4);
    fail_at = 0;
    assert(!race_scavenger_items[index] && task_count(4) == index);
    assert(blocks == before - 3); /* its task, work and mover */
    if (count) {
        assert(garden_unregister_object(&owners[--count]));
    }
    reset_search();
    assert(search_frames(250));
    toy = garden_target_object(&chao);
    assert(toy && toy->work->ang.x == index && !race_scavenger_items[index]);
    assert(toy->work->pos.x == scavenger_item_positions[index].x);
    assert(toy->work->pos.z == scavenger_item_positions[index].z);
    assert(toy->work->collision->count == 1 && toy->force.p && toy->draw);
    /* A search cannot replace a claimed toy. Its release creates the new
     * occupant, then the destructor of its predecessor runs the next frame. */
    reset_search();
    search_frames(70);
    assert(task_count(4) == SCAVENGER_ITEMS && !race_scavenger_items[index]);
    toy->work->mode = 3; /* ITEM_HELD */
    toy->step(toy);
    assert(race_scavenger_items[index] && race_scavenger_items[index] != toy);
    task_step_level(4);
    assert(task_count(4) == SCAVENGER_ITEMS && race_scavenger_items[index]);
    assert(race_scavenger_items[index]->force.p);
    task_destroy_level(4);
    task_destroy_level(2);
    assert(!race_scavenger_items[index] && !blocks);
    release(count);
}

static void unavailable_rack(void)
{
    int i, count = garden_object_capacities[GARDEN_OBJECT_TOY];
    reserve(GARDEN_OBJECT_TOY, count);
    race_scavenger_start();
    task_step_level(4);
    reset_search();
    assert(search_frames(500));
    assert(behavior == race_run && !garden_target_object(&chao));
    assert(!task_levels[4]);
    for (i = 0; i < SCAVENGER_ITEMS; ++i) {
        assert(!race_scavenger_items[i]);
    }
    task_destroy_level(2);
    release(count);
    assert(!blocks);
}

static void panel_setup(void)
{
    int i;
    /* The question uses a task and work. Each lane also owns a collision
     * record and its shape. A partial set of judging lanes cannot operate. */
    for (i = 1; i <= 2 + 4 * SCAVENGER_LANES; ++i) {
        fail_at = allocations + i;
        race_scavenger_start();
        fail_at = 0;
        if (i <= 2) {
            assert(task_count(2) == SCAVENGER_LANES);
            assert(task_count(4) == SCAVENGER_ITEMS);
            task_step_level(4);
            reset_search();
            assert(search_frames(100) && garden_target_object(&chao));
            task_destroy_level(4);
            task_destroy_level(2);
        } else {
            assert(!task_levels[2] && !task_levels[4] && !blocks);
            reset_search();
            assert(search_frames(500) && behavior == race_run);
        }
        assert(!blocks && !garden_object_count(GARDEN_OBJECT_TOY));
    }
}

static void answer_penalty(int failure)
{
    struct task *panel, *toy, *penalty;
    int before;
    race_scavenger_start();
    task_step_level(4);
    assert(task_count(4) == SCAVENGER_ITEMS);
    toy = race_scavenger_items[0];
    toy->work->tick = (race_scavenger_target + 1) % SCAVENGER_LANES;
    assert(race_join_scavenger_lane(&chao));
    assert(garden_pair_objects(&chao, toy));
    panel = task_levels[2]->next; /* first lane follows the question panel */
    panel->work->timer = 1801;
    panel->step(panel);
    race_scavenger_submitted[0] = TRUE;
    panel->step(panel);
    panel->work->timer = 61;
    before = blocks;
    wrong_sounds = 0;
    behavior = NULL;
    fail_at = failure ? allocations + failure : 0;
    panel->step(panel);
    fail_at = 0;
    assert(wrong_sounds == 1 && panel->work->mode == 3);
    if (failure) {
        assert(behavior == race_run && blocks == before);
        assert(task_count(4) == SCAVENGER_ITEMS);
    } else {
        assert(!behavior && task_count(4) == SCAVENGER_ITEMS + 1);
        penalty = task_levels[4]->prev;
        assert(!strcmp(penalty->name, scavenger_penalty_name));
        assert(penalty->draw && penalty->motion.p && penalty->work->collision);
        assert(penalty->work->pos.y == panel->work->pos.y + 30.0F);
        assert(penalty->work->ang.y ==
               (Angle)(1234 * (FULL_TURN * RAND_SCALE)));
        penalty->step(penalty);
        assert(((struct mover *)penalty->motion.p)->spd.y == -0.05F);
        assert(penalty->work->pos.y < panel->work->pos.y + 30.0F);
    }
    panel->work->timer = 149;
    panel->step(panel);
    assert(!race_scavenger_occupant(0));
    task_destroy_level(4);
    task_destroy_level(2);
    assert(!blocks && !garden_object_count(GARDEN_OBJECT_TOY));
}

int main(void)
{
    int i;
    for (i = 0; i < SCAVENGER_ITEMS; i++) {
        scavenger_item_positions[i].x = 10.0F + i;
        scavenger_item_positions[i].z = -20.0F - i;
    }
    task_init();
    chao.work = &racer.base;
    chao.motion.p = &racer_motion;
    assert(garden_register_object(GARDEN_OBJECT_CHAO, &chao, 0));
    immediate_failures();
    for (i = 0; i <= 4; ++i) {
        tree_drop(i);
    }
    for (i = 1; i <= 6; ++i) {
        toy_failure(i);
    }
    unavailable_rack();
    panel_setup();
    for (i = 0; i <= 5; ++i) {
        answer_penalty(i);
    }
    assert(garden_unregister_object(&chao));
    return 0;
}
