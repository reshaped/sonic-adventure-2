#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "chao/body_collision.h"
#include "chao/face.h"
#include "chao/garden_scene.h"
#include "chao/garden_toys.h"
#include "chao/icon.h"
#include "chao/motion.h"
#include "chao/parts.h"
#include "chao/small_toys.h"
#include "chao/toy_art.h"
#include "chao/work.h"
#include "chao/world.h"
#include "chao/world_slots.h"
#include "object/mover.h"
#include "sound/sound.h"
#include "system/task.h"

static int allocations, fail_at, live_blocks, reading_motions;
static struct chao_work reader_work;
static struct task reader;
static NJS_POINT3 position = {3.0F, 4.0F, 5.0F};

void *syMalloc(Uint32 size)
{
    void *block;
    if (++allocations == fail_at) {
        return NULL;
    }
    block = malloc(size);
    assert(block);
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

void njMemCopy(void *dest, const void *source, Sint32 bytes)
{
    memcpy(dest, source, (size_t)bytes);
}

Float njScalor(NJS_VECTOR *v)
{
    return (Float)sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

Float njSqrt(Float value)
{
    return (Float)sqrt(value);
}

void sound_detach_task(const struct task *task)
{
    (void)task;
}

int rand(void)
{
    return 0;
}

struct motion_table_entry garden_horse_clips[5];
NJS_CNK_OBJECT chao_rattle, chao_stick, chao_doll, chao_toy_car,
    chao_pogo_stick;
NJS_CNK_OBJECT chao_broom, bubble_bottle, bubble_wand;
NJS_TEXLIST garden_toy_texlist;

void chao_link_motion(struct task *chao, Sint32 index)
{
    if (index == 337) {
        ++reading_motions;
    }
}

Bool chao_motion_held(struct task *chao)
{
    return FALSE;
}

void chao_set_timed_eye(struct task *chao, int eye, int frames)
{
}
void chao_equip_node(struct task *chao, int node, NJS_CNK_OBJECT *object,
                     NJS_TEXLIST *textures)
{
}
void chao_equip_hand_11(struct task *chao, NJS_CNK_OBJECT *object,
                        NJS_TEXLIST *textures)
{
}
void chao_resist_displacement(struct task *chao)
{
}
void chao_enable_push(struct task *chao)
{
}
void chao_allow_displacement(struct task *chao)
{
}
void chao_enable_body_shapes(struct task *chao)
{
}
void chao_fade_props(struct task *chao)
{
}
void chao_clear_emotion_mark(struct task *chao)
{
}
void chao_restore_mouth(struct task *chao)
{
}
void chao_restore_eyes(struct task *chao)
{
}

static struct task *make_horse(void)
{
    struct task *horse;
    garden_rocking_horse_create(&position, 0x3200);
    horse = task_levels[3];
    if (horse) {
        assert(!horse->draw && !horse->force.p && !horse->destructor);
        assert(horse->work->pos.y == position.y &&
               horse->work->ang.y == 0x3200);
        horse->step(horse);
    }
    return task_levels[3];
}

static void check_horse(void)
{
    struct task *horse = make_horse();
    struct motion_control *motion;
    assert(horse && horse->draw && horse->destructor && horse->work->collision);
    motion = horse->extra.p;
    assert(motion && motion->table == garden_horse_clips && motion->index == 4);
    assert(horse->motion.p &&
           ((struct mover *)horse->motion.p)->gravity == -0.05F);
    assert(garden_find_object(GARDEN_OBJECT_TOY, 0) == horse);
    task_destroy(horse);
    assert(!task_levels[3] && !live_blocks &&
           !garden_object_count(GARDEN_OBJECT_TOY));
}

static void horses(void)
{
    struct task occupied[GARDEN_ROW_SLOTS * 2] = {{0}};
    int i, count, capacity = (int)garden_object_capacities[GARDEN_OBJECT_TOY];
    check_horse();
    count = allocations;
    assert(count == 6);
    for (i = 1; i <= count; ++i) {
        fail_at = allocations + i;
        assert(!make_horse());
        assert(!garden_object_count(GARDEN_OBJECT_TOY) && !live_blocks);
        check_horse();
    }
    for (i = 0; i < capacity; ++i) {
        assert(garden_register_object(GARDEN_OBJECT_TOY, &occupied[i], 0));
    }
    assert(!make_horse() && !live_blocks);
    for (i = 0; i < capacity; ++i) {
        assert(garden_find_object(GARDEN_OBJECT_TOY, i) == &occupied[i]);
        garden_unregister_object(&occupied[i]);
    }
    check_horse();
}

static int blow(void)
{
    reader_work.behavior.mode = 1;
    reader_work.behavior.timer = 69;
    return chao_blow_bubbles(&reader);
}

static void bubbles(void)
{
    int i;
    struct task *bubble;
    struct mover *mover;
    reader_work.shape.hand_11 = position;
    for (i = 1; i <= 3; ++i) {
        fail_at = allocations + i;
        assert(blow() == BEHAVIOR_CONTINUE);
        assert(!live_blocks && !task_levels[4]);
        assert(blow() == BEHAVIOR_CONTINUE);
        bubble = task_levels[4];
        assert(bubble && bubble->draw && bubble->work->pos.x == position.x);
        mover = bubble->motion.p;
        assert(mover && mover->gravity == 0.0F && mover->spd.y == 0.1F);
        task_destroy(bubble);
        assert(!live_blocks && !task_levels[4]);
    }
}

static void books(void)
{
    chao_behavior_fn read = small_toy_actions[CHAO_BOOK_TOY];
    struct task *old, *book;
    int i, blocks, motions;

    reader.work->pos = position;
    reader.work->ang.y = 0x4200;
    reader_work.behavior.mode = 0;
    for (i = 1; i <= 2; ++i) {
        fail_at = allocations + i;
        assert(read(&reader) == BEHAVIOR_CONTINUE);
        assert(!reader_work.behavior.mode && !reader_work.book);
        assert(!task_levels[2] && !live_blocks && !reading_motions);
    }
    assert(read(&reader) == BEHAVIOR_CONTINUE);
    old = reader_work.book;
    assert(old && old == task_levels[2] && old->work->scl.y == 0.0F);
    old->step(old);
    assert(old->work->scl.y == 0.1F);
    reader_work.behavior.mode = 0;
    blocks = live_blocks;
    motions = reading_motions;
    for (i = 1; i <= 2; ++i) {
        fail_at = allocations + i;
        assert(read(&reader) == BEHAVIOR_CONTINUE);
        assert(!reader_work.behavior.mode && reader_work.book == old);
        assert(old->step != task_destroy && reading_motions == motions);
        assert(live_blocks == blocks);
    }
    assert(read(&reader) == BEHAVIOR_CONTINUE);
    book = reader_work.book;
    assert(book && book != old && old->step == task_destroy);
    assert(book->work->ang.y == 0x4200 && book->work->pos.x == position.x);
    old->step(old);
    assert(reader_work.book == book && book == task_levels[2]);

    reader_work.behavior.timer = 0;
    assert(read(&reader) == BEHAVIOR_FINISH);
    book->step(book);
    assert(book->step == task_destroy);
    book->step(book);
    assert(!reader_work.book && !task_levels[2] && !live_blocks);
    assert(read(&reader) == BEHAVIOR_FINISH);

    reader_work.behavior.mode = 0;
    assert(read(&reader) == BEHAVIOR_CONTINUE);
    reader_work.behavior.mode = 2;
    task_destroy(reader_work.book);
    assert(!reader_work.book && !task_levels[2] && !live_blocks);
    assert(read(&reader) == BEHAVIOR_FINISH);

    reader_work.behavior.mode = 0;
    assert(read(&reader) == BEHAVIOR_CONTINUE);
    book = reader_work.book;
    chao_set_behavior(&reader, read);
    assert(!reader_work.book && book->step == task_destroy);
    assert(read(&reader) == BEHAVIOR_CONTINUE);
    old = reader_work.book;
    book->step(book);
    assert(reader_work.book == old);

    /* The real actor destructor calls this hook before the reader is freed. */
    chao_behavior_destroy(&reader);
    assert(!reader_work.book && old->step == task_destroy);
    reader.work = NULL;
    old->step(old);
    assert(!task_levels[2] && !live_blocks);
}

int main(void)
{
    task_init();
    reader.work = &reader_work.base;
    horses();
    bubbles();
    books();
    return 0;
}
