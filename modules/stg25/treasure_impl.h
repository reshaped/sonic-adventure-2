/* TREASURE's build, init and callbacks, which stg25/treasure.c compiles for
 * Death Chamber and stg26/treasure.c for Egg Quarters, under EGG_QUARTERS.
 * stg26/coffin.c compiles them for Egg Quarters' coffin, under EGG_QUARTERS
 * and QUARTERS_COFFIN. the includer defines TREASURE(name), and the chest's
 * art, shape, builds, built flag and table of steps. TREASURE_WAIT and
 * TREASURE_BROKEN name the two steps the table holds. coffin.c also defines
 * the settings of the coffin's break: coffin_break_volume, coffin_break_speed,
 * coffin_break_spread and coffin_break_rise */
#ifndef STG25_TREASURE_IMPL_H
#define STG25_TREASURE_IMPL_H
#include "collision/surface.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/object_builds.h"
#include "sound/sound.h"
#include "stg25/prop_stage.h"
#include "system/angle.h"
#include "system/task.h"

/* a chest's modes, in its work's mode, which index chamber_treasure_steps */
enum chamber_treasure_mode { MODE_WAIT, MODE_BROKEN };

/* the task's unk_3c holds the mode the chest takes at its next step, or
 * NO_NEXT_MODE */
#define NEXT_MODE(task) ((task)->unk_3c.l)
#define NO_NEXT_MODE (-1)

/* the state a break leaves in the placement. a chest whose placement holds
 * it starts broken */
#define BROKEN_STATE 1

/* the chest's flags in the collision world */
#define CHEST_WORLD (WORLD_SIZE_CLASS(2) | WORLD_SOLID)
/* a break's pieces vanish at the highest floor at or below FLOOR_LOOK_HEIGHT
 * above the break's origin */
#define FLOOR_LOOK_HEIGHT 1.0F
#define BREAK_SOUND 0x1011

/* the chest's copy collides only while a player lies within REACH of its pos.
 * each piece of a break flies straight away from the break's origin. it flies
 * at BREAK_SPEED, plus up to BREAK_SPREAD more at random. BREAK_RISE adds to
 * its upward speed. the coffin's origin lies BREAK_CENTER_HEIGHT above its pos,
 * and a chest's is its pos */
#ifdef QUARTERS_COFFIN
#define REACH 48.7F
#define BREAK_CENTER_HEIGHT 7.5F
#define BREAK_SPEED coffin_break_speed
#define BREAK_SPREAD coffin_break_spread
#define BREAK_RISE coffin_break_rise
#else
#define REACH 36.0F
#define BREAK_SPEED 0.8F
#define BREAK_SPREAD 0.4F
#define BREAK_RISE 0.7F
/* a play of the break's sound lasts BREAK_SOUND_STEPS + 1 updates, as
 * sound_play_at_for says */
#define BREAK_SOUND_PRIORITY 1
#define BREAK_VOLUME 127
#define BREAK_SOUND_STEPS 80
#endif

/* SHC expands each call of TREASURE(set_next_mode) under #pragma inline. the
 * image also holds a copy of the function, which SHC writes only for an
 * external one. no header declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void TREASURE(set_next_mode)(struct task *task, int mode);
/* NOLINTEND(misc-use-internal-linkage) */

static void chamber_treasure_destroy(struct task *task);
static void chamber_treasure_step(struct task *task);
static void chamber_treasure_draw(struct task *task);
static void chamber_treasure_take_mode(struct task *task);
static void chamber_treasure_break(struct task *task);

Uint8 *TREASURE(build)(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, chamber_treasure_builds, CHAMBER_COLORS) ==
        CHAMBER_COLORS) {
        chamber_treasure_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void TREASURE(init)(struct task *task)
{
    /* @hack spelling: task->work read again for mode, where work would do */
    struct task_work *work = task->work;
    NJS_OBJECT *object;
#if defined(EGG_QUARTERS) && !defined(QUARTERS_COFFIN)
    NJS_POINT3 center;
#endif

    if (!placement_try_leave(task)) {
        task->draw = chamber_treasure_draw;
        task->step = chamber_treasure_step;
        task->destructor = chamber_treasure_destroy;
        if (placement_get_state(task)) {
#ifndef QUARTERS_COFFIN
            task->extra.p = NULL;
#endif
            task->work->mode = MODE_BROKEN;
        } else {
            object = world_pool_alloc();
            task->extra.p = object;
            /* @bug memory: copies the chest shape through null when the
             * world pool is empty. the writes at 0x8c506fea in Death Chamber
             * and 0x8c503c6c / 0x8c5045a6 in Egg Quarters use the allocation
             * unchecked. the destructors test the copy at 0x8c507080,
             * 0x8c503d20 and 0x8c50463e. the steps pass its pos to
             * player_near at 0x8c507238, 0x8c503ed8 and 0x8c5047f8 */
#ifdef NONMATCHING
            /* this side repairs the tag above: where the pool is empty, the
             * init destroys the chest, and set.c makes it again on a later
             * pass */
            if (object == NULL) {
                task_destroy(task);
                return;
            }
#endif
            *object = collision;
            object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
            object->scl[0] = object->scl[1] = object->scl[2] = 1.0F;
            object->ang[0] = 0;
            object->ang[1] = work->ang.y;
            object->ang[2] = 0;
            object->pos[0] = work->pos.x;
            object->pos[1] = work->pos.y;
            object->pos[2] = work->pos.z;
            world_add(CHEST_WORLD, task, object);
#if defined(EGG_QUARTERS) && !defined(QUARTERS_COFFIN)
            /* @hack redundant: a copy of the center raised above the chest,
             * which nothing reads */
            center = work->pos;
            center.y += 15.0F;
#endif
            collision_init(task, chamber_treasure_shapes, TREASURE_SHAPES,
                           COLLISION_OBJECT);
            task->work->mode = MODE_WAIT;
        }
        NEXT_MODE(task) = NO_NEXT_MODE;
    }
}

static void chamber_treasure_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void chamber_treasure_step(struct task *task)
{
    if (!placement_try_leave(task)) {
        chamber_treasure_take_mode(task);
        chamber_treasure_steps[task->work->mode](task);
    }
}

static void chamber_treasure_draw(struct task *task)
{
    /* @hack spelling: task->work read again for mode and the color, where
     * work would do */
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: a switch of one case, where an if would do */
    switch (task->work->mode) {
    case MODE_WAIT:
        njSetTexture(TREASURE(texlists)[CHAMBER_COLOR(task->work)]);
        if (chamber_treasure_built) {
            njDirectDrawObject(
                chamber_treasure_builds[CHAMBER_COLOR(task->work)].built);
        } else {
            njCnkEasyDrawModel(
                chamber_treasure_models[CHAMBER_COLOR(task->work)]);
        }
        break;
    }
    njPopMatrixEx();
}

static void chamber_treasure_take_mode(struct task *task)
{
    if (NEXT_MODE(task) != NO_NEXT_MODE) {
        task->work->mode = NEXT_MODE(task);
        NEXT_MODE(task) = NO_NEXT_MODE;
    }
}

#ifdef QUARTERS_COFFIN
#pragma inline(quarters_coffin_set_next_mode)
#elif defined(EGG_QUARTERS)
#pragma inline(quarters_treasure_set_next_mode)
#else
#pragma inline(chamber_treasure_set_next_mode)
#endif
void TREASURE(set_next_mode)(struct task *task, int mode)
{
    NEXT_MODE(task) = mode;
}

static void TREASURE_WAIT(struct task *task)
{
    struct task_work *work = task->work;
    struct task *player;
    NJS_OBJECT *object;

    /* @hack spelling: the hunter's test in an if of its own, where a third
     * operand of the && would do */
    if ((work->collision->flags & COLLISION_TOUCHED) &&
        (player = collision_next_player(task))) {
        if (STAGE_PROP(hunter_attacking)(player)) {
            chamber_treasure_break(task);
            TREASURE(set_next_mode)(task, MODE_BROKEN);
            return;
        }
    }
    object = task->extra.p;
    if (player_near((NJS_POINT3 *)object->pos, REACH)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
    collision_register(task);
}

static void chamber_treasure_break(struct task *task)
{
    /* @hack spelling: task->work read again for the color, where work would
     * do */
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR velocity;
    Float floor;

    pos = work->pos;
#ifdef QUARTERS_COFFIN
    /* @hack spelling: subtract the negative height to raise the origin,
     * where adding the height would do */
    pos.y -= -BREAK_CENTER_HEIGHT;
#endif
    velocity.x = 0.0F;
    velocity.y = BREAK_RISE;
    velocity.z = 0.0F;
    floor = surface_probe_floor(pos.x, pos.y + FLOOR_LOOK_HEIGHT, pos.z, &ang);
    debris_break_flags(&pos, work->ang.y,
                       chamber_treasure_parts[CHAMBER_COLOR(task->work)],
                       PIECES, BREAK_SPEED, BREAK_SPREAD, floor, &velocity,
                       DEBRIS_TRAIL | DEBRIS_VANISH);
#ifdef QUARTERS_COFFIN
    sound_play(BREAK_SOUND, NULL, 0, (Sint16)coffin_break_volume);
    placement_set_state(task, BROKEN_STATE);
#else
    placement_set_state(task, BROKEN_STATE);
    sound_play_at_for(BREAK_SOUND, work, BREAK_SOUND_PRIORITY, BREAK_VOLUME,
                      BREAK_SOUND_STEPS, &work->pos);
#endif
    placement_kill(task);
}

static void TREASURE_BROKEN(struct task *task)
{
}

#endif
