/* the init and callbacks of Hidden Base's WALL and BLOCK, which stg23/wall.c
 * and stg23/block.c compile, BLOCK's under BASE_BLOCK. the includer defines
 * the art and the shape, LOOKS, the count of the art's looks, and HIT_DELAY.
 * its PROP_INIT and QUEUE_MODE name the init and the setter */
#ifndef STG23_BREAKABLE_IMPL_H
#define STG23_BREAKABLE_IMPL_H

#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "object/set.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/port.h"
#include "system/task.h"

/* the prop's damage, in the work's mode, which the placement's state keeps */
enum damage_mode { INTACT, FIRST_CRACK, SECOND_CRACK, DESTROYED };
/* the mode take_mode gives the prop at its next step, or NO_NEXT_MODE */
#define NEXT_MODE(work) ((work)->submode)
#define NO_NEXT_MODE (-1)
/* the steps since the last hit, up to HIT_DELAY. a cracked prop takes no hit
 * until TIMER reaches HIT_DELAY */
#define TIMER(task) ((task)->unk_3c.l)
#define LOOK(task) ((int)njAbs((task)->work->scl.z) % LOOKS)
/* the wall's look 0 is bright, and its other look dark */
#define BRIGHT_LOOK 0
/* whether an attack touched the prop's shape. it sets hit to the attack's
 * task */
#define ATTACK_HIT(work, hit)                                                  \
    (((work)->collision->flags & COLLISION_TOUCHED) &&                         \
     ((hit) = (work)->collision->hit->owner) && (hit)->work &&                 \
     (hit)->work->id == WORK_ID_ATTACK)
/* each hit raises dust of HIT_DUST_SCALE where the attack is, and plays
 * HIT_SOUND. the break plays BREAK_SOUND */
#define HIT_DUST_SCALE 12.0F
#define HIT_SOUND 2
#ifdef BASE_BLOCK
#define BREAK_SOUND 4
#else
#define BREAK_SOUND 3
#endif
#define SOUND_PRIORITY 0
#define PROP_SHAPES 1
/* the wall's init puts a copy of base_wall_collision in the collision world,
 * and keeps it in task->extra. the copy is WORLD_SOLID, and its
 * WORLD_SIZE_CLASS is 2 */
#define WALL_WORLD_FLAGS (WORLD_SIZE_CLASS(2) | WORLD_SOLID)
/* the eval flags the copy clears, so that the world reads its pos and ang */
#define WALL_EVAL_CLEARED (NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG)
/* the pieces' floor probe starts PROBE_LIFT above pos. the block's pieces
 * start FRAGMENT_HEIGHT above it */
#define PROBE_LIFT 1.0F
#define FRAGMENT_HEIGHT 5.0F
#define FRAGMENT_COUNT(parts) ((int)(sizeof(parts) / sizeof((parts)[0])))
#define FRAGMENT_FLAGS (DEBRIS_TRAIL | DEBRIS_VANISH)
/* the prop draws DRAW_LIFT above pos */
#define DRAW_LIFT 0.01F

static void prop_destroy(struct task *task);
static void prop_step(struct task *task);
static void prop_draw(struct task *task);
static void take_mode(struct task *task);
/* @hack redundant: QUEUE_MODE's external linkage, which no call needs, where
 * static would do. SHC would expand its every call in place and leave out the
 * body the image holds */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void QUEUE_MODE(struct task *task, int mode);
static void whole_step(struct task *task);
#ifndef BASE_BLOCK
static void wall_shatter(struct task *task);
#else
/* @hack redundant: block_break_sound's external linkage, though nothing calls
 * it, where static would do. SHC would leave out the body the image holds */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void block_break_sound(struct task *task);
#endif
static void cracked_step(struct task *task);
static void split_step(struct task *task);
static void destroyed_step(struct task *task);

static int hit_volume = {
#include "stg23/breakable_impl/hit_volume.inc"
};
static int break_volume = {
#include "stg23/breakable_impl/break_volume.inc"
};
typedef void (*masonry_damage_step)(struct task *task);

KEPT static masonry_damage_step steps[DESTROYED + 1] = {
#include "stg23/breakable_impl/steps.inc"
};
static Float fragment_speed = {
#include "stg23/breakable_impl/fragment_speed.inc"
};
static Float fragment_spread = {
#include "stg23/breakable_impl/fragment_spread.inc"
};
static Float fragment_rise = {
#include "stg23/breakable_impl/fragment_rise.inc"
};

#ifdef BASE_BLOCK
void PROP_INIT(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = prop_draw;
        task->step = prop_step;
        task->destructor = prop_destroy;
        collision_init(task, &shape, PROP_SHAPES, COLLISION_OBJECT);
        task->work->mode = placement_get_state(task);
        NEXT_MODE(task->work) = NO_NEXT_MODE;
        TIMER(task) = 0;
    }
}
#else
/* @hack redundant: task declared register, where a plain parameter would
 * do */
void PROP_INIT(register struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (!placement_try_leave(task)) {
        /* @hack redundant: zero declared register, where a plain local would
         * do */
        /* @hack spelling: zero, a local for 0, where 0 in place would do */
        register int zero = 0;
        task->draw = prop_draw;
        task->step = prop_step;
        task->destructor = prop_destroy;
        if (placement_get_state(task) != DESTROYED) {
            object = world_pool_alloc();
            task->extra.p = object;
            /* @bug memory: writes base_wall_collision through null where
             * world_pool_alloc has none to lend (0x8c505300). prop_destroy
             * and split_step test task->extra for null (0x8c5053a4 and
             * 0x8c5059dc) */
#ifdef NONMATCHING
            /* this side repairs the tag above: where the pool has no object,
             * the init destroys the wall, and set.c makes it again on a later
             * pass */
            if (object == NULL) {
                task_destroy(task);
                return;
            }
#endif
            *object = base_wall_collision;
            object->evalflags &= ~WALL_EVAL_CLEARED;
            object->scl[0] = object->scl[1] = object->scl[2] = 1.0F;
            object->ang[0] = zero;
            object->ang[1] = task->work->ang.y;
            object->ang[2] = zero;
            object->pos[0] = work->pos.x;
            object->pos[1] = work->pos.y;
            object->pos[2] = work->pos.z;
            world_add(WALL_WORLD_FLAGS, task, object);
            collision_init(task, &shape, PROP_SHAPES, COLLISION_OBJECT);
        }
        task->work->mode = placement_get_state(task);
        NEXT_MODE(task->work) = NO_NEXT_MODE;
        TIMER(task) = zero;
    }
}
#endif

static void prop_destroy(struct task *task)
{
#ifndef BASE_BLOCK
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
#endif
}

static void prop_step(struct task *task)
{
    if (!placement_try_leave(task)) {
        take_mode(task);
        steps[task->work->mode](task);
    }
}

static void prop_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njTranslate(NULL, 0.0F, DRAW_LIFT, 0.0F);
    njRotateY(NULL, work->ang.y);
    switch (task->work->mode) {
    case INTACT:
        njSetTexture(whole_textures[LOOK(task)]);
        njCnkEasyDrawModel(whole_models[LOOK(task)]);
        break;
    case FIRST_CRACK:
        njSetTexture(cracked_textures[LOOK(task)]);
        njCnkEasyDrawModel(cracked_models[LOOK(task)]);
#ifndef BASE_BLOCK
        njSetTexture(frame_textures[LOOK(task)]);
        njCnkEasyDrawModel(frame_models[LOOK(task)]);
#endif
        break;
    case SECOND_CRACK:
        njSetTexture(split_textures[LOOK(task)]);
        njCnkEasyDrawModel(split_models[LOOK(task)]);
#ifndef BASE_BLOCK
        njSetTexture(frame_textures[LOOK(task)]);
        njCnkEasyDrawModel(frame_models[LOOK(task)]);
        break;
    case DESTROYED:
        njSetTexture(frame_textures[LOOK(task)]);
        njCnkEasyDrawModel(frame_models[LOOK(task)]);
#endif
        break;
    }
    njPopMatrixEx();
}

static void take_mode(struct task *task)
{
    if (NEXT_MODE(task->work) != NO_NEXT_MODE) {
        task->work->mode = NEXT_MODE(task->work);
        placement_set_state(task, task->work->mode);
        NEXT_MODE(task->work) = NO_NEXT_MODE;
    }
}

void QUEUE_MODE(struct task *task, int mode)
{
    NEXT_MODE(task->work) = mode;
}

static void whole_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *hit;
    NJS_VECTOR speed;

    if (ATTACK_HIT(work, hit)) {
        speed.x = 0.0F;
        speed.y = 0.0F;
        speed.z = 0.0F;
        dust_create(&hit->work->pos, &speed, HIT_DUST_SCALE);
        sound_play_at(HIT_SOUND, NULL, SOUND_PRIORITY, hit_volume, &work->pos);
        QUEUE_MODE(task, FIRST_CRACK);
        TIMER(task) = 0;
        return;
    }
    collision_register(task);
}

#ifdef BASE_BLOCK
void block_break_sound(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR speed;

    pos = work->pos;
    /* @hack redundant: the stores to speed, which nothing reads */
    speed.x = 0.0F;
    speed.y = 0.7F;
    speed.z = 0.0F;
    surface_probe_floor(pos.x, pos.y + PROBE_LIFT, pos.z, &ang);
    sound_play_at(BREAK_SOUND, NULL, SOUND_PRIORITY, break_volume, &work->pos);
}
#else
static void wall_shatter(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR speed;
    Float floor;

    pos = work->pos;
    speed.x = 0.0F;
    speed.y = fragment_rise;
    speed.z = 0.0F;
    floor = surface_probe_floor(pos.x, pos.y + PROBE_LIFT, pos.z, &ang);
    if (LOOK(task) == BRIGHT_LOOK) {
        debris_break_flags(&pos, work->ang.y, bright_fragments,
                           FRAGMENT_COUNT(bright_fragments), fragment_speed,
                           fragment_spread, floor, &speed, FRAGMENT_FLAGS);
    } else {
        debris_break_flags(&pos, work->ang.y, dark_fragments,
                           FRAGMENT_COUNT(dark_fragments), fragment_speed,
                           fragment_spread, floor, &speed, FRAGMENT_FLAGS);
    }
    sound_play_at(BREAK_SOUND, NULL, SOUND_PRIORITY, break_volume, &work->pos);
}
#endif

static void cracked_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *hit;
    NJS_VECTOR speed;
#ifdef BASE_BLOCK
    NJS_POINT3 pos;
    struct angles ang;
    Float floor;
#endif

    if (TIMER(task) < HIT_DELAY) {
        TIMER(task)++;
    } else if (ATTACK_HIT(work, hit)) {
#ifdef BASE_BLOCK
        floor = surface_probe_floor(work->pos.x, work->pos.y + PROBE_LIFT,
                                    work->pos.z, &ang);
        pos = work->pos;
        pos.y += FRAGMENT_HEIGHT;
#endif
        speed.x = 0.0F;
        speed.y = 0.0F;
        speed.z = 0.0F;
        dust_create(&hit->work->pos, &speed, HIT_DUST_SCALE);
#ifdef BASE_BLOCK
        speed.x = 0.0F;
        speed.y = fragment_rise;
        speed.z = 0.0F;
        debris_break_flags(&pos, work->ang.y, cracked_fragments,
                           FRAGMENT_COUNT(cracked_fragments), fragment_speed,
                           fragment_spread, floor, &speed, FRAGMENT_FLAGS);
#endif
        sound_play_at(HIT_SOUND, NULL, SOUND_PRIORITY, hit_volume, &work->pos);
        QUEUE_MODE(task, SECOND_CRACK);
        TIMER(task) = 0;
        return;
    }
    collision_register(task);
}

static void split_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *hit;
    NJS_VECTOR speed;
#ifdef BASE_BLOCK
    NJS_POINT3 pos;
    struct angles ang;
    Float floor;
#endif

    if (TIMER(task) < HIT_DELAY) {
        TIMER(task)++;
    } else if (ATTACK_HIT(work, hit)) {
#ifdef BASE_BLOCK
        floor = surface_probe_floor(work->pos.x, work->pos.y + PROBE_LIFT,
                                    work->pos.z, &ang);
#endif
        speed.x = 0.0F;
        speed.y = 0.0F;
        speed.z = 0.0F;
        dust_create(&hit->work->pos, &speed, HIT_DUST_SCALE);
#ifdef BASE_BLOCK
        sound_play_at(BREAK_SOUND, NULL, SOUND_PRIORITY, break_volume,
                      &work->pos);
        pos = work->pos;
        pos.y += FRAGMENT_HEIGHT;
        speed.x = 0.0F;
        speed.y = fragment_rise;
        speed.z = 0.0F;
        debris_break_flags(&pos, work->ang.y, split_fragments,
                           FRAGMENT_COUNT(split_fragments), fragment_speed,
                           fragment_spread, floor, &speed, FRAGMENT_FLAGS);
#else
        wall_shatter(task);
#endif
        QUEUE_MODE(task, DESTROYED);
        TIMER(task) = 0;
#ifndef BASE_BLOCK
        if (task->extra.p) {
            world_remove(task, task->extra.p);
            world_pool_free(task->extra.p);
        }
        task->extra.p = NULL;
#endif
        return;
    }
    collision_register(task);
}

static void destroyed_step(struct task *task)
{
}

#endif
