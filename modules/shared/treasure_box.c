/* the treasure box, BOX in Wild Canyon's object list: a chest. a Knuckles or
 * a Rouge breaks it with an attack, and it stays broken */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/canyon_util.h"
#include "shared/debris.h"
#include "shared/treasure_box.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/port.h"
#include "system/task.h"
#include "vmu/vibration.h"

/* a box's modes, which index treasure_box_modes */
enum treasure_box_mode { MODE_WAIT, MODE_BROKEN };

/* the work's submode holds the mode the box takes at its next step, or
 * NO_NEXT_MODE */
#define NEXT_MODE(work) ((work)->submode)
#define NO_NEXT_MODE (-1)

/* the placement's state once the box is broken */
#define STATE_BROKEN 1

/* the world flags of the box's copy of treasure_box_collision */
#define BOX_WORLD (WORLD_SIZE_CLASS(2) | WORLD_SOLID)
/* world_collect_near_players takes the copy only while player 0 or 1 lies
 * within REACH of its pos */
#define REACH 36.0F

#define BOX_SHAPES 1
#define BOX_PIECES 5

/* a break throws the pieces at BREAK_SPEED, plus up to BREAK_SPREAD more at
 * random, and BREAK_RISE up. their floor is the highest at or below the point
 * FLOOR_LOOK_HEIGHT above the box's pos */
#define BREAK_SPEED 0.8F
#define BREAK_SPREAD 0.4F
#define BREAK_RISE 0.7F
#define FLOOR_LOOK_HEIGHT 1.0F
#define BREAK_SOUND 6
#define VIBRATION_POWER 3
#define VIBRATION_FREQ 7
#define VIBRATION_TIME 0
/* the evalflags that would keep an object's pos, ang and scl from applying */
#define EVAL_UNITS (NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL)

#include "shared/treasure_box/text.inc"
#include "shared/canyon_art/treasure_box.h"

/* no header declares it, yet it is external. treasure_box_wait takes it
 * inline, and the image holds its body too. SHC would leave out a static
 * one's body */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void treasure_box_set_next_mode(struct task *task, int mode);
/* NOLINTEND(misc-use-internal-linkage) */

static void treasure_box_destroy(struct task *task);
static void treasure_box_step(struct task *task);
static void treasure_box_draw(struct task *task);
static void treasure_box_take_mode(struct task *task);
static void treasure_box_wait(struct task *task);
static void treasure_box_break(struct task *task);
static void treasure_box_broken(struct task *task);

static struct debris_part treasure_box_parts[BOX_PIECES] = {
#include "shared/treasure_box/treasure_box_parts.inc"
};

NJS_TEXLIST *treasure_box_texlists[8] = {
#include "shared/treasure_box/treasure_box_texlists.inc"
};

/* the whole box's shape, the one collision_init takes, then three shapes that
 * nothing reads */
/* clang-format off */
static struct collision_shape treasure_box_shapes[] = {
    {0, SHAPE_BOX, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {0.0F, 9.0F, 0.0F}, 11.0F, 12.0F, 10.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000, {10.4342F, 2.596F, -1.0F}, 9.0F,
     4.0F, 1.0F, 0, {0, QUARTER_TURN, 0}},
    {0, SHAPE_BOX, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000, {-11.0919F, 0.0F, 2.7975F}, 5.0F,
     4.0F, 1.0F, 0, {0, QUARTER_TURN, 0}},
    {0, SHAPE_BOX, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000, {-0.3698F, 2.9165F, 7.6198F}, 11.5F,
     4.0F, 1.0F, 0, {0, 0, 0}},
};
/* clang-format on */

/* the volume of the break's sound */
static Sint32 treasure_box_volume = 100;

KEPT static void (*treasure_box_modes[])(struct task *task) = {
    treasure_box_wait,
    treasure_box_broken,
};

void treasure_box_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (!half_try_leave(task) && !placement_try_leave(task)) {
        task->draw = treasure_box_draw;
        task->step = treasure_box_step;
        task->destructor = treasure_box_destroy;
        /* @bug uninitialized: reads the state of a box without a placement
         * too, which placement_get_state leaves undefined. at the call at
         * 0x2e of the object's code, R0 still holds placement_try_leave's
         * FALSE, which EXTS.B R0,R0 at 0x32 takes as the state. such a box
         * starts whole */
#ifdef NONMATCHING
        /* this side keeps the tag's result without the undefined read. a box
         * without a placement starts whole, as in the image */
        if (task->placement != NULL && placement_get_state(task)) {
#else
        if (placement_get_state(task)) {
#endif
            task->extra.p = NULL;
            task->work->mode = MODE_BROKEN;
            /* @hack spelling: a goto past the else, where the else alone
             * would do */
            goto ready;
        } else {
            object = world_pool_alloc();
            task->extra.p = object;
            *object = treasure_box_collision;
            object->evalflags &= ~EVAL_UNITS;
            object->scl[0] = object->scl[1] = object->scl[2] = 1.0F;
            object->ang[0] = 0;
            object->ang[1] = task->work->ang.y;
            object->ang[2] = 0;
            object->pos[0] = work->pos.x;
            object->pos[1] = work->pos.y;
            object->pos[2] = work->pos.z;
            world_add(BOX_WORLD, task, object);
            collision_init(task, treasure_box_shapes, BOX_SHAPES,
                           COLLISION_OBJECT);
            task->work->mode = MODE_WAIT;
        }
    ready:
        NEXT_MODE(task->work) = NO_NEXT_MODE;
    }
}

static void treasure_box_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void treasure_box_step(struct task *task)
{
    /* @hack redundant: work, a local whose address is taken, which nothing
     * uses */
    struct task_work *work = task->work;

    (void)&work;
    if (!half_try_leave(task) && !placement_try_leave(task)) {
        treasure_box_take_mode(task);
        treasure_box_modes[task->work->mode](task);
    }
}

static void treasure_box_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    switch (task->work->mode) {
    case MODE_WAIT:
        njSetTexture(&treasure_box_texlist);
        njCnkEasyDrawModel(&treasure_box_model);
        break;
    case MODE_BROKEN:
        /* @hack spelling: the last case without its break, where a break
         * would do */
        njSetTexture(&treasure_box_broken_texlist);
        njCnkEasyDrawModel(&treasure_box_broken_model);
    }
    njPopMatrixEx();
}

static void treasure_box_take_mode(struct task *task)
{
    if (NEXT_MODE(task->work) != NO_NEXT_MODE) {
        task->work->mode = NEXT_MODE(task->work);
        NEXT_MODE(task->work) = NO_NEXT_MODE;
    }
}

#pragma inline(treasure_box_set_next_mode)
void treasure_box_set_next_mode(struct task *task, int mode)
{
    NEXT_MODE(task->work) = mode;
}

static void treasure_box_wait(struct task *task)
{
    NJS_OBJECT *object = task->extra.p;
    struct task_work *work = task->work;
    struct task *player;

    if (player_near((NJS_POINT3 *)object->pos, REACH)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
    /* @hack spelling: the hunter's test in an if of its own, where a third
     * operand of the && would do */
    if ((work->collision->flags & COLLISION_TOUCHED) &&
        (player = collision_next_player(task))) {
        if (hunter_attacking(player)) {
            treasure_box_break(task);
            treasure_box_set_next_mode(task, MODE_BROKEN);
            vibration_start(player_slot_of(player), VIBRATION_POWER,
                            VIBRATION_FREQ, VIBRATION_TIME);
            return;
        }
    }
    collision_register(task);
}

static void treasure_box_break(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR spd;
    Float floor;

    pos = work->pos;
    spd.x = 0.0F;
    spd.y = BREAK_RISE;
    spd.z = 0.0F;
    floor = surface_probe_floor(pos.x, pos.y + FLOOR_LOOK_HEIGHT, pos.z, &ang);
    debris_break_flags(&pos, work->ang.y, treasure_box_parts, BOX_PIECES,
                       BREAK_SPEED, BREAK_SPREAD, floor, &spd,
                       DEBRIS_TRAIL | DEBRIS_VANISH);
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
    placement_set_state(task, STATE_BROKEN);
    sound_play(BREAK_SOUND, task, 0, 0);
    sound_set_volume(task, SOUND_ANY_ID, treasure_box_volume);
}

static void treasure_box_broken(struct task *task)
{
}
