/* the pot's code. shared/ocean_pot.c compiles it for Sand Ocean, with
 * ocean_pot_pieces defined first. stg23/pot.c compiles it for Hidden Base,
 * with HIDDEN_BASE_POT and the bright and dark pots' art defined first */
#ifndef SHARED_OCEAN_POT_IMPL_H
#define SHARED_OCEAN_POT_IMPL_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "object/set.h"
#include "shared/debris.h"
#include "shared/ocean_pot.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"

/* the pot's modes, which index ocean_pot_steps */
enum ocean_pot_mode { MODE_WHOLE, MODE_BROKEN };
/* the work's submode holds the mode the pot takes at its next step, or
 * NO_NEXT_MODE */
#define NEXT_MODE(work) ((work)->submode)
#define NO_NEXT_MODE (-1)

#define POT_SHAPES 1
/* a break probes for the floor the pieces land on from FLOOR_LOOK_HEIGHT
 * above where they break */
#define FLOOR_LOOK_HEIGHT 1.0F
#ifdef HIDDEN_BASE_POT
enum pot_look { POT_BRIGHT, POT_DARK, POT_LOOKS };
#define LOOK(work) ((int)njAbs((work)->scl.z) % POT_LOOKS)
#define HALF_HEIGHT 10.0F
#define RADIUS 7.0F
/* the drawn pot's height above the work's pos */
#define DRAW_LIFT 0.01F
#define BREAK_SOUND 1
#else
#define HALF_HEIGHT 7.5F
#define RADIUS 5.0F
/* the pieces break from PIECE_LIFT above the work's pos */
#define PIECE_LIFT 2.0F
#define BREAK_SOUND 4
#endif
#define BREAK_SOUND_PRIORITY 0

/* nothing calls ocean_pot_set_next_mode, but the image holds it. it is
 * external because SHC leaves out a static function that nothing calls */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void ocean_pot_set_next_mode(struct task *task, Sint8 mode);

static void ocean_pot_destroy(struct task *task);
static void ocean_pot_step(struct task *task);
static void ocean_pot_draw(struct task *task);
static void ocean_pot_take_next_mode(struct task *task);
static void ocean_pot_whole_step(struct task *task);
static void ocean_pot_break(struct task *task);
static void ocean_pot_broken_step(struct task *task);

/* clang-format off */
static struct collision_shape ocean_pot_shape = {
    0, SHAPE_CYLINDER, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
    SHAPE_GUARD_LEVEL(1), SHAPE_HURT_NO_PUSH | SHAPE_HURTABLE,
    {0.0F, HALF_HEIGHT, 0.0F}, RADIUS, HALF_HEIGHT, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */
static Sint32 break_volume = 50;
static void (*ocean_pot_steps[])(struct task *task) = {ocean_pot_whole_step,
                                                       ocean_pot_broken_step};
/* each piece flies straight away from the pot at piece_speed, plus up to
 * piece_spread more at random. each also takes piece_rise along y */
static Float piece_speed = 0.7F;
static Float piece_spread = 0.35F;
static Float piece_rise = 0.25F;

void ocean_pot_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = ocean_pot_draw;
    task->step = ocean_pot_step;
    task->destructor = ocean_pot_destroy;
    collision_init(task, &ocean_pot_shape, POT_SHAPES, COLLISION_OBJECT);
    task->work->mode = MODE_WHOLE;
    NEXT_MODE(task->work) = NO_NEXT_MODE;
}

static void ocean_pot_destroy(struct task *task)
{
}

static void ocean_pot_step(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    ocean_pot_take_next_mode(task);
    ocean_pot_steps[task->work->mode](task);
}

static void ocean_pot_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
#ifdef HIDDEN_BASE_POT
    njTranslate(NULL, 0.0F, DRAW_LIFT, 0.0F);
#endif
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: task->work read again, where work would do */
    /* @hack spelling: a switch of one case, where an if would do */
    switch (task->work->mode) {
    case MODE_WHOLE:
#ifdef HIDDEN_BASE_POT
        njSetTexture(pot_textures[LOOK(task->work)]);
        njCnkEasyDrawModel(pot_models[LOOK(task->work)]);
#else
        njSetTexture(&ocean_pot_texlist);
        njCnkEasyDrawModel(&ocean_pot_model);
#endif
        break;
    }
    njPopMatrixEx();
}

static void ocean_pot_take_next_mode(struct task *task)
{
    if (NEXT_MODE(task->work) != NO_NEXT_MODE) {
        task->work->mode = NEXT_MODE(task->work);
        NEXT_MODE(task->work) = NO_NEXT_MODE;
    }
}

void ocean_pot_set_next_mode(struct task *task, Sint8 mode)
{
    NEXT_MODE(task->work) = mode;
}

static void ocean_pot_whole_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *hit;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((work->collision->flags & COLLISION_TOUCHED) &&
        (hit = work->collision->hit->owner) != NULL && hit->work != NULL &&
        hit->work->id == WORK_ID_ATTACK) {
        ocean_pot_break(task);
        return;
    }
    collision_register(task);
}

static void ocean_pot_break(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    struct angles tilt;
    NJS_VECTOR spd;
    Float floor;

    pos = work->pos;
#ifdef HIDDEN_BASE_POT
    /* @hack redundant: 0 taken from pos.y */
    pos.y -= 0.0F;
#else
    /* @hack spelling: -PIECE_LIFT taken from pos.y, where PIECE_LIFT added
     * would do */
    pos.y -= -PIECE_LIFT;
#endif
    spd.x = 0.0F;
    spd.y = piece_rise;
    spd.z = 0.0F;
    floor = surface_probe_floor(pos.x, pos.y + FLOOR_LOOK_HEIGHT, pos.z, &tilt);
#ifdef HIDDEN_BASE_POT
    /* @hack spelling: task->work read again, where work would do */
    if (LOOK(task->work) == POT_BRIGHT) {
        debris_break_flags(&pos, work->ang.y, bright_fragments,
                           OCEAN_POT_PIECES, piece_speed, piece_spread, floor,
                           &spd, DEBRIS_TRAIL | DEBRIS_VANISH);
    } else {
        debris_break_flags(&pos, work->ang.y, dark_fragments, OCEAN_POT_PIECES,
                           piece_speed, piece_spread, floor, &spd,
                           DEBRIS_TRAIL | DEBRIS_VANISH);
    }
#else
    debris_break_flags(&pos, work->ang.y, ocean_pot_pieces, OCEAN_POT_PIECES,
                       piece_speed, piece_spread, floor, &spd,
                       DEBRIS_TRAIL | DEBRIS_VANISH);
#endif
    sound_play_at(BREAK_SOUND, NULL, BREAK_SOUND_PRIORITY, break_volume,
                  &work->pos);
    placement_kill(task);
}

static void ocean_pot_broken_step(struct task *task)
{
}

#endif
