/* Exercise the game constructors, scheduler, collision, mover and garden
 * registry. The body stub can fail after publishing a partially built shape,
 * as the game's node and morph target allocations can. */
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ninja.h>
#include <shinobi.h>

#include "chao/actor.h"
#include "chao/behavior.h"
#include "chao/body.h"
#include "chao/body_collision.h"
#include "chao/cocoon.h"
#include "chao/drive.h"
#include "chao/egg.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/garden.h"
#include "chao/garden_ball.h"
#include "chao/garden_scene.h"
#include "chao/genes.h"
#include "chao/icon.h"
#include "chao/life.h"
#include "chao/minimal.h"
#include "chao/motion.h"
#include "chao/parts.h"
#include "chao/perception.h"
#include "chao/race_chao.h"
#include "chao/residents.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "chao/world.h"
#include "chao/world_slots.h"
#include "collision/collision.h"
#include "draw/motion.h"
#include "hud/item_stock.h"
#include "object/chaos_drive.h"
#include "object/mover.h"
#include "player/player.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "system/task.h"

struct chao_tuning chao_tuning;
static struct task occupied[GARDEN_ROW_SLOTS * 2];
static struct task_work occupied_work[GARDEN_ROW_SLOTS * 2];
static struct chao_save save, untouched;
static struct task previous_ball;
static int allocations, live_blocks, fail_at;
static int current_area, pickup_sounds;
struct task_work *player_work[PLAYER_SLOTS];
struct player *player_records[PLAYER_SLOTS];
NJS_MATRIX _nj_unit_matrix_;

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
    --live_blocks;
    free(block);
}

Bool chao_build_body(struct task *task)
{
    struct chao_work *work = (struct chao_work *)task->work;
    work->shape.root = syCalloc(1, sizeof(*work->shape.root));
    if (!work->shape.root) {
        return FALSE;
    }
    work->shape.form = syCalloc(1, sizeof(*work->shape.form));
    return work->shape.form != NULL;
}

void chao_free_body(struct task *task)
{
    struct chao_work *work = (struct chao_work *)task->work;
    if (work->shape.form) {
        syFree(work->shape.form);
    }
    if (work->shape.root) {
        syFree(work->shape.root);
    }
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

Bool njPushMatrix(NJS_MATRIX *matrix)
{
    return TRUE;
}
Bool njPopMatrixEx(void)
{
    return TRUE;
}
void njRotateY(NJS_MATRIX *matrix, Angle angle)
{
}
void njCalcVector(NJS_MATRIX *matrix, NJS_VECTOR *source, NJS_VECTOR *result)
{
    *result = *source;
}

void sound_detach_task(const struct task *task)
{
    (void)task;
}

Sint8 sound_play_at(int id, const void *key, Sint8 priority, Sint16 volume,
                    const NJS_POINT3 *position)
{
    ++pickup_sounds;
    return 0;
}

struct trail_style chaos_drive_trail_style;

void njUnitMatrix(NJS_MATRIX *matrix)
{
    (void)matrix;
}

int player_character(int slot)
{
    return PLAYER_EGGMAN;
}

int chao_current_area(void)
{
    return current_area;
}

/* The game scales rand() as a 15-bit sample. */
int rand(void)
{
    return 1234;
}

void chao_learn_traits(struct chao_gene *gene,
                       const struct chao_profile *profile)
{
    gene->life[0] += profile->parameters.reincarnations;
}

int chao_emotion_get(const struct task *task, Uint32 kind)
{
    return 0;
}

int chao_motion_pose(struct task *task)
{
    return 0;
}
void chao_resist_displacement(struct task *task)
{
}
void chao_disable_body_shapes(struct task *task)
{
}
void chao_set_timed_eye(struct task *task, int eye, int frames)
{
}
void chao_set_timed_mouth(struct task *task, int mouth, int frames)
{
}
void chao_show_mark(struct task *task, Sint8 mark, int frames)
{
}
void chao_link_motion_frames(struct task *task, Sint32 index, Uint16 frames)
{
}
void jingle_play(const char *name)
{
    abort();
}

void garden_age_reset(void)
{
}
void garden_enable_objects(void)
{
}
void chao_new_gene(struct chao_gene *gene)
{
    (void)gene;
}
void chao_hatch_traits(struct task *task)
{
    (void)task;
}
void chao_perception_init(struct task *task)
{
    (void)task;
}
void chao_behavior_init(struct task *task)
{
    (void)task;
}
void chao_motion_init(struct task *task)
{
    (void)task;
}
void chao_icon_init(struct task *task)
{
    (void)task;
}
void chao_face_init(struct task *task)
{
    (void)task;
}
void chao_restore_animal_parts(struct task *task)
{
    (void)task;
}
void chao_behavior_destroy(struct task *task)
{
    (void)task;
}
void chao_unpack_racer(struct chao_parameters *parameters,
                       const struct garden_race_chao *racer)
{
    (void)racer;
}
NJS_MOTION *garden_minimal_held_motions[ANIMAL_KINDS];
NJS_MOTION *garden_minimal_walk_motions[ANIMAL_KINDS];
NJS_MOTION *garden_minimal_idle_motions[ANIMAL_KINDS];
NJS_MOTION *garden_minimal_fly_motions[ANIMAL_KINDS];
void motion_init(struct motion_control *control,
                 const struct motion_table_entry *table)
{
    (void)control;
}
void motion_link(struct motion_control *control, Sint32 index)
{
    (void)control;
}

static struct task *create(int category)
{
    NJS_POINT3 pos = {15.0F, 4.0F, -3.0F};
    NJS_VECTOR speed = {1.0F, -2.0F, 3.0F};
    struct task *old_ball = garden_ball;
    int floating = garden_ball_floating;

    switch (category) {
    case GARDEN_OBJECT_CHAO:
        return chao_create(NULL, NULL, NULL, &pos, 0, &save);
    case GARDEN_OBJECT_EGG:
        return chao_create_egg(NULL, &pos, EGG_RESIDENT, &save);
    case GARDEN_OBJECT_TOY:
        garden_ball_create(&pos, &speed);
        assert(garden_ball == old_ball && garden_ball_floating == floating);
        if (task_levels[2]) {
            assert(task_levels[2]->force.p && task_levels[2]->work->collision);
            assert(task_levels[2]->draw == NULL);
            task_levels[2]->step(task_levels[2]);
        }
        return task_levels[2];
    default:
        if (garden_minimal_create(0, &pos, 0, &speed)) {
            assert(task_levels[2]);
            return task_levels[2];
        }
        return NULL;
    }
}

/* Occupy real registry slots without scheduling the unrelated owners. The
 * first ordinary creation then receives a nonzero slot. */
static void reserve_slots(int category, int count)
{
    int slot;

    fail_at = 0;
    assert(garden_control_start());
    task_destroy(garden_controller);
    assert(!live_blocks);
    memset(occupied, 0, sizeof(occupied));
    assert(count <= (int)(sizeof(occupied) / sizeof(occupied[0])));
    for (slot = 0; slot < count; ++slot) {
        occupied[slot].work = &occupied_work[slot];
        assert(garden_register_object(category, &occupied[slot], 0));
    }
}

static void check_owners(int category, int count, int vacated)
{
    int slot;

    assert(garden_object_count(category) == count - (vacated >= 0));
    for (slot = 0; slot < count; ++slot) {
        if (slot == vacated) {
            assert(!garden_find_object(category, slot));
            continue;
        }
        assert(garden_find_object(category, slot) == &occupied[slot]);
        assert(occupied[slot].force.p == garden_find_entry(category, slot));
    }
    assert(!task_levels[2] && !live_blocks);
}

static int success(int category, int slots, int vacated)
{
    struct task *task;
    int next_slot = vacated < 0 ? slots : vacated;

    fail_at = 0;
    allocations = 0;
    save = untouched;
    task = create(category);
    assert(task && task == garden_find_object(category, next_slot));
    assert(task == task_levels[2] && task->next == task);
    assert(task->work->collision && task->motion.p);
    assert(garden_object_count(category) == slots + (vacated < 0));
    if (category == GARDEN_OBJECT_CHAO || category == GARDEN_OBJECT_EGG) {
        assert(memcmp(&save, &untouched, sizeof(save)));
        assert(task->work->tick == next_slot);
        if (category == GARDEN_OBJECT_CHAO) {
            assert(((struct mover *)task->motion.p)->floor_calls == next_slot);
        }
    } else if (category == GARDEN_OBJECT_TOY) {
        assert(garden_ball == task);
    }
    task_destroy(task);
    check_owners(category, slots, vacated);
    if (category == GARDEN_OBJECT_TOY) {
        assert(!garden_ball && !garden_ball_floating && !garden_ball_rolling);
    }
    return allocations;
}

static void exercise(int category)
{
    int i, count, slots;

    reserve_slots(category, 1);
    count = success(category, 1, -1);
    assert(count >= 5);
    for (i = 1; i <= count + 1; ++i) {
        slots = i > count ? (int)garden_object_capacities[category] : 1;
        reserve_slots(category, slots);
        allocations = 0;
        fail_at = i;
        save = untouched;
        garden_ball = &previous_ball;
        garden_ball_rolling = TRUE;
        garden_ball_floating = TRUE;
        assert(create(category) == NULL);
        check_owners(category, slots, -1);
        assert(!memcmp(&save, &untouched, sizeof(save)));
        assert(garden_ball == &previous_ball && garden_ball_rolling &&
               garden_ball_floating);
        if (i <= count) {
            assert(success(category, slots, -1) == count);
        } else {
            assert(garden_unregister_object(&occupied[0]));
            assert(success(category, slots, 0) == count);
        }
    }
}

/* This original entry point has no header or in-game caller. */
extern struct task *egg_create_shell_top(NJS_POINT3 *position);

static void shell_top_creation(void)
{
    NJS_POINT3 position = {4.0F, 2.0F, -1.0F};
    struct task *shell;
    struct mover *mover;
    int allocation;

    for (allocation = 1; allocation <= 3; ++allocation) {
        allocations = 0;
        fail_at = allocation;
        assert(egg_create_shell_top(&position) == NULL);
        assert(task_levels[2] == NULL && live_blocks == 0);

        fail_at = 0;
        shell = egg_create_shell_top(&position);
        assert(shell != NULL && task_levels[2] == shell);
        mover = shell->motion.p;
        assert(mover != NULL && mover->floor_bounce == 0.6F);
        assert(shell->work->pos.x == position.x &&
               shell->work->pos.y == position.y &&
               shell->work->pos.z == position.z);
        assert(shell->draw != NULL && shell->destructor != NULL);
        task_destroy(shell);
        assert(task_levels[2] == NULL && live_blocks == 0);
    }
}

static void unsaved_egg_outside_garden(void)
{
    NJS_POINT3 pos = {0};
    struct task *egg;
    struct chao_work *work;

    reserve_slots(GARDEN_OBJECT_EGG, 0);
    current_area = CHAO_AREA_LOBBY;
    egg = chao_create_egg(NULL, &pos, EGG_TRANSFERRED, NULL);
    assert(egg != NULL);
    work = (struct chao_work *)egg->work;
    assert(!work->save && work->profile.parameters.type == CHAO_EGG);
    task_destroy(egg);
    check_owners(GARDEN_OBJECT_EGG, 0, -1);
    current_area = CHAO_AREA_NEUTRAL;
}

static void reincarnation_retries(void)
{
    struct task *chao, *egg;
    struct chao_work *work, *egg_work;
    struct chao_profile profile;
    struct chao_gene gene;
    struct chao_save previous;
    int phase, blocks, capacity = garden_object_capacities[GARDEN_OBJECT_EGG];

    reserve_slots(GARDEN_OBJECT_EGG, capacity);
    chao = create(GARDEN_OBJECT_CHAO);
    assert(chao != NULL);
    work = (struct chao_work *)chao->work;
    work->profile.parameters.affection = 1;
    chao_tuning.rebirth_stress_limit = 1;
    garden_pack_chao(chao, &save);
    previous = save;
    gene = work->gene;
    profile = work->profile;
    assert(chao_die_behavior(chao) == BEHAVIOR_CONTINUE);
    phase = work->behavior.mode;
    blocks = live_blocks;
    /* The scheduler allocates a child task and its work separately. */
    fail_at = allocations + 1;
    assert(chao_die_behavior(chao) == BEHAVIOR_CONTINUE);
    assert(work->behavior.mode == phase && !work->cocoon && !chao->child);
    assert(live_blocks == blocks);
    assert(chao_die_behavior(chao) == BEHAVIOR_CONTINUE);
    fail_at = allocations + 2;
    assert(chao_die_behavior(chao) == BEHAVIOR_CONTINUE);
    assert(work->behavior.mode == phase && !work->cocoon && !chao->child);
    assert(live_blocks == blocks);
    fail_at = 0;
    assert(chao_die_behavior(chao) == BEHAVIOR_CONTINUE);
    assert(chao_die_behavior(chao) == BEHAVIOR_CONTINUE);
    assert(work->cocoon && work->cocoon == chao->child);
    phase = work->behavior.mode;
    blocks = live_blocks;
    assert(chao_die_behavior(chao) == BEHAVIOR_CONTINUE);
    assert(chao_die_behavior(chao) == BEHAVIOR_CONTINUE);
    assert(work->behavior.mode == phase && (work->flags & CHAO_VISIBLE));
    assert(work->save == &save && !memcmp(&previous, &save, sizeof(save)));
    assert(!memcmp(&gene, &work->gene, sizeof(gene)));
    assert(!memcmp(&profile, &work->profile, sizeof(profile)));
    assert(chao == garden_find_object(GARDEN_OBJECT_CHAO, 0));
    assert(live_blocks == blocks && chao->next == chao);

    assert(garden_unregister_object(&occupied[capacity - 1]));
    assert(chao_die_behavior(chao) == BEHAVIOR_CONTINUE);
    assert(chao_die_behavior(chao) == BEHAVIOR_CONTINUE);
    assert(work->behavior.mode != phase && !(work->flags & CHAO_VISIBLE));
    assert(!work->save && !chao->force.p);
    egg = garden_find_object(GARDEN_OBJECT_EGG, capacity - 1);
    assert(egg && egg != chao);
    egg_work = (struct chao_work *)egg->work;
    assert(egg_work->save == &save && egg_work->profile.parameters.reborn_egg);
    assert(((struct egg_state *)egg->extra.p)->origin == EGG_REBORN);
    assert(save.record.saved.parameters.type == CHAO_EGG);
    assert(save.record.saved.parameters.reincarnations == 1);
    assert(save.record.saved.gene.life[0] == gene.life[0] + 1);
    assert(work->gene.life[0] == egg_work->gene.life[0]);
    assert(work->profile.parameters.reincarnations == 1);
    task_destroy(chao);
    task_destroy(egg);
    check_owners(GARDEN_OBJECT_EGG, capacity - 1, -1);
}

static void stock_retries(int item)
{
    struct task_work player_task = {0};
    struct player player = {0};
    struct item_stock previous;
    struct task *manager, *pickup;
    int capacity = garden_object_capacities[GARDEN_OBJECT_MINIMAL];

    reserve_slots(GARDEN_OBJECT_MINIMAL, capacity);
    player_work[0] = &player_task;
    player_records[0] = &player;
    player.character = PLAYER_SONIC;
    pickup_sounds = 0;
    item_stock_clear_all();
    item_stock_push(0, 1);
    item_stock_push(0, item);
    item_stock_hold(0);
    item_stocks[TEAM_HERO].shift = 0.75F;
    item_stocks[TEAM_HERO].entries[STOCK_ITEMS + 1].timer = 7;
    previous = item_stocks[TEAM_HERO];
    assert(pickup_sounds == 2 && item_stock_peek(0) == item);
    garden_animals_start();
    manager = task_levels[2];
    assert(manager != NULL);
    manager->work->timer = 16;
    manager->step(manager);
    assert(manager->next == manager);
    assert(!memcmp(&previous, &item_stocks[TEAM_HERO], sizeof(previous)));
    assert(pickup_sounds == 2 && item_stock_peek(0) == item);

    assert(garden_unregister_object(&occupied[capacity - 1]));
    if (item >= ITEM_CHAOS_DRIVE) {
        /* The task, its work, mover, collision record and shape copy are
         * required. Each failed attempt leaves the same item for retry. */
        int i;
        int baseline = live_blocks;
        for (i = 1; i <= 5; ++i) {
            fail_at = allocations + i;
            manager->work->timer = 16;
            manager->step(manager);
            assert(live_blocks == baseline && manager->next == manager);
            assert(
                !memcmp(&previous, &item_stocks[TEAM_HERO], sizeof(previous)));
        }
        fail_at = 0;
    }
    manager->work->timer = 16;
    manager->step(manager);
    pickup = garden_find_object(GARDEN_OBJECT_MINIMAL, capacity - 1);
    assert(pickup && pickup->work->tick == item);
    assert(item_stocks[TEAM_HERO].count == 1 && item_stock_peek(0) == 1);
    assert(pickup_sounds == 2);
    task_destroy(pickup);
    task_destroy(manager);
    check_owners(GARDEN_OBJECT_MINIMAL, capacity - 1, -1);
    player_work[0] = NULL;
    player_records[0] = NULL;
}

static void drive_without_trails(void)
{
    NJS_POINT3 pos = {1.0F, 2.0F, 3.0F};
    NJS_VECTOR speed = {0.0F, 1.0F, 0.0F};
    struct task *drive;
    int allocation;

    /* The two optional trails each allocate a header and a point array. */
    for (allocation = 6; allocation <= 9; ++allocation) {
        reserve_slots(GARDEN_OBJECT_MINIMAL, 0);
        fail_at = allocations + allocation;
        assert(garden_drive_create(CHAOS_DRIVE_GREEN, &pos, &speed));
        drive = garden_find_object(GARDEN_OBJECT_MINIMAL, 0);
        assert(drive &&
               drive->work->tick == ITEM_CHAOS_DRIVE + CHAOS_DRIVE_GREEN);
        assert(drive->draw && drive->work->collision && drive->motion.p);
        assert(allocations >= fail_at);
        task_destroy(drive);
        check_owners(GARDEN_OBJECT_MINIMAL, 0, -1);
    }
    fail_at = 0;
}

int main(void)
{
    task_init();
    current_area = CHAO_AREA_NEUTRAL;
    memset(&untouched, 0x9d, sizeof(untouched));
    exercise(GARDEN_OBJECT_CHAO);
    exercise(GARDEN_OBJECT_EGG);
    exercise(GARDEN_OBJECT_TOY);
    exercise(GARDEN_OBJECT_MINIMAL);
    shell_top_creation();
    unsaved_egg_outside_garden();
    reincarnation_retries();
    stock_retries(2);
    stock_retries(ITEM_CHAOS_DRIVE + CHAOS_DRIVE_PURPLE);
    drive_without_trails();
    return 0;
}
