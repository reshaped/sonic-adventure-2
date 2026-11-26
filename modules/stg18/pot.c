/* TSUBO in Dry Lagoon's object list: a pot that a Rouge's attack breaks into
 * six pieces */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/object_builds.h"
#include "sound/sound.h"
#include "stg18/lagoon_util.h"
#include "stg18/pot.h"
#include "stg18/pot/text.inc"
#include "system/angle.h"
#include "system/task.h"
#include "vmu/vibration.h"

/* the pot's modes, in its work's mode, which index pot_steps */
enum pot_mode { MODE_WAIT, MODE_BROKEN };

/* the work's submode holds the mode the pot takes at its next step, or
 * NO_NEXT_MODE */
#define NEXT_MODE(work) ((work)->submode)
#define NO_NEXT_MODE (-1)

/* the state a break leaves in the placement, so that the pot the placement
 * makes next starts broken */
#define BROKEN_STATE 1

#define POT_SHAPES 2
#define POT_PIECES 6
#define POT_BUILDS 1

/* the pieces end on the highest floor at or below FLOOR_LOOK_HEIGHT above
 * the pot's pos */
#define FLOOR_LOOK_HEIGHT 1.0F
#define BREAK_SOUND 7
#define VIBRATION_POWER 3
#define VIBRATION_FREQ 7

/* Large pot and its six broken pieces. */

static NJS_TEXNAME pot_texlist_names[2] = {
#include "stg18/pot/pot_texlist_names.inc"
};

static NJS_TEXLIST pot_texlist = {
#include "stg18/pot/pot_texlist.inc"
};

static Sint16 pot_polygons[327] = {
#include "stg18/pot/pot_polygons.inc"
};

static Sint32 pot_vertices[191] = {
#include "stg18/pot/pot_vertices.inc"
};

static NJS_CNK_MODEL pot_model = {
#include "stg18/pot/pot_model.inc"
};

static NJS_CNK_OBJECT pot_object = {
#include "stg18/pot/pot_object.inc"
};

static NJS_TEXNAME pot_pieces_texlist_names[2] = {
#include "stg18/pot/pot_pieces_texlist_names.inc"
};

static NJS_TEXLIST pot_pieces_texlist = {
#include "stg18/pot/pot_pieces_texlist.inc"
};

static Sint16 pot_piece_object_5_polygons[139] = {
#include "stg18/pot/pot_piece_object_5_polygons.inc"
};

static Sint32 pot_piece_object_5_vertices[91] = {
#include "stg18/pot/pot_piece_object_5_vertices.inc"
};

static NJS_CNK_MODEL pot_piece_model_5 = {
#include "stg18/pot/pot_piece_model_5.inc"
};

static NJS_CNK_OBJECT pot_piece_object_5 = {
#include "stg18/pot/pot_piece_object_5.inc"
};

static Sint16 pot_piece_object_4_polygons[45] = {
#include "stg18/pot/pot_piece_object_4_polygons.inc"
};

static Sint32 pot_piece_object_4_vertices[27] = {
#include "stg18/pot/pot_piece_object_4_vertices.inc"
};

static NJS_CNK_MODEL pot_piece_model_4 = {
#include "stg18/pot/pot_piece_model_4.inc"
};

static NJS_CNK_OBJECT pot_piece_object_4 = {
#include "stg18/pot/pot_piece_object_4.inc"
};

static Sint16 pot_piece_object_3_polygons[91] = {
#include "stg18/pot/pot_piece_object_3_polygons.inc"
};

static Sint32 pot_piece_object_3_vertices[63] = {
#include "stg18/pot/pot_piece_object_3_vertices.inc"
};

static NJS_CNK_MODEL pot_piece_model_3 = {
#include "stg18/pot/pot_piece_model_3.inc"
};

static NJS_CNK_OBJECT pot_piece_object_3 = {
#include "stg18/pot/pot_piece_object_3.inc"
};

static Sint16 pot_piece_object_1_polygons[91] = {
#include "stg18/pot/pot_piece_object_1_polygons.inc"
};

static Sint32 pot_piece_object_1_vertices[55] = {
#include "stg18/pot/pot_piece_object_1_vertices.inc"
};

static NJS_CNK_MODEL pot_piece_model_1 = {
#include "stg18/pot/pot_piece_model_1.inc"
};

static NJS_CNK_OBJECT pot_piece_object_1 = {
#include "stg18/pot/pot_piece_object_1.inc"
};

static Sint16 pot_piece_object_0_polygons[45] = {
#include "stg18/pot/pot_piece_object_0_polygons.inc"
};

static Sint32 pot_piece_object_0_vertices[31] = {
#include "stg18/pot/pot_piece_object_0_vertices.inc"
};

static NJS_CNK_MODEL pot_piece_model_0 = {
#include "stg18/pot/pot_piece_model_0.inc"
};

static NJS_CNK_OBJECT pot_piece_object_0 = {
#include "stg18/pot/pot_piece_object_0.inc"
};

static Sint16 pot_piece_object_2_polygons[117] = {
#include "stg18/pot/pot_piece_object_2_polygons.inc"
};

static Sint32 pot_piece_object_2_vertices[79] = {
#include "stg18/pot/pot_piece_object_2_vertices.inc"
};

static NJS_CNK_MODEL pot_piece_model_2 = {
#include "stg18/pot/pot_piece_model_2.inc"
};

static NJS_CNK_OBJECT pot_piece_object_2 = {
#include "stg18/pot/pot_piece_object_2.inc"
};

static NJS_CNK_OBJECT pot_shards = {
#include "stg18/pot/pot_shards.inc"
};

/* the image holds pot_set_next_mode's body, though SHC expands each call
 * under #pragma inline. a static function under the pragma gets no body. so
 * this one is external, though no header declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void pot_set_next_mode(struct task *task, int mode);
/* NOLINTEND(misc-use-internal-linkage) */

static void pot_destroy(struct task *task);
static void pot_step(struct task *task);
static void pot_draw(struct task *task);
static void pot_take_mode(struct task *task);
static void pot_wait(struct task *task);
static void pot_break(struct task *task);
static void pot_broken(struct task *task);

/* clang-format off */
static struct debris_part pot_parts[POT_PIECES] = {
    {&pot_pieces_texlist, &pot_piece_model_0, {0.0F, 19.24F, 0.0F}, {0, 0, 0}},
    {&pot_pieces_texlist, &pot_piece_model_1, {-4.745F, 4.61F, -0.19500001F},
     {0, 0, 0}},
    {&pot_pieces_texlist, &pot_piece_model_2, {3.9259999F, 9.54F, 3.7309997F},
     {0, 0, 0}},
    {&pot_pieces_texlist, &pot_piece_model_3, {3.51F, 8.75F, 4.758F},
     {0, 0, 0}},
    {&pot_pieces_texlist, &pot_piece_model_4, {-0.19500001F, 0.79F, -4.758F},
     {0, 0, 0}},
    {&pot_pieces_texlist, &pot_piece_model_5,
     {-4.3289995F, 13.68F, -0.19500001F}, {0, 0, 0}},
};
/* clang-format on */
NJS_TEXLIST *pot_texlists[] = {&pot_texlist, &pot_pieces_texlist, NULL};
/* clang-format off */
static struct collision_shape pot_shapes[POT_SHAPES] = {
    {0, SHAPE_BOX, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {0.0F, 20.0F, 0.0F}, 15.0F, 15.0F, 10.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000, {0.0F, 10.0F, 0.0F}, 10.0F, 10.0F,
     0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static Sint32 pot_volume = 25;
static Bool pot_built = FALSE;
static struct model_build pot_builds[POT_BUILDS] = {
    {MODEL_BUILD_ALONE, &pot_texlist, &pot_object, NULL},
};
static void (*pot_steps[])(struct task *task) = {
    pot_wait,
    pot_broken,
};
/* a break throws the pieces at pot_speed, plus up to pot_spread more at
 * random, and pot_rise up */
static Float pot_speed = 0.7F;
static Float pot_spread = 0.35F;
static Float pot_rise = 0.3F;

Uint8 *pot_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, pot_builds, POT_BUILDS) == POT_BUILDS) {
        pot_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void pot_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's order
     * would set the step first */
    task->draw = pot_draw;
    task->step = pot_step;
    task->destructor = pot_destroy;
    if (placement_get_state(task)) {
        task->work->mode = MODE_BROKEN;
    } else {
        collision_init(task, pot_shapes, POT_SHAPES, COLLISION_OBJECT);
        task->work->mode = MODE_WAIT;
    }
    NEXT_MODE(task->work) = NO_NEXT_MODE;
}

static void pot_destroy(struct task *task)
{
}

static void pot_step(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    pot_take_mode(task);
    pot_steps[task->work->mode](task);
}

static void pot_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: task->work read again for mode, where work would do */
    /* @hack spelling: a switch of one case, where an if would do */
    switch (task->work->mode) {
    case MODE_WAIT:
        njSetTexture(&pot_texlist);
        if (pot_built) {
            njDirectDrawObject(pot_builds[0].built);
        } else {
            njCnkEasyDrawModel(&pot_model);
        }
        break;
    }
    njPopMatrixEx();
}

static void pot_take_mode(struct task *task)
{
    if (NEXT_MODE(task->work) != NO_NEXT_MODE) {
        task->work->mode = NEXT_MODE(task->work);
        NEXT_MODE(task->work) = NO_NEXT_MODE;
    }
}

#pragma inline(pot_set_next_mode)
void pot_set_next_mode(struct task *task, int mode)
{
    NEXT_MODE(task->work) = mode;
}

static void pot_wait(struct task *task)
{
    struct task_work *work = task->work;
    struct task *player;

    /* @hack spelling: the Rouge's test in an if of its own, where a third
     * operand of the && would do */
    if ((work->collision->flags & COLLISION_TOUCHED) &&
        (player = collision_next_player(task))) {
        if (rouge_attacking(player)) {
            pot_break(task);
            pot_set_next_mode(task, MODE_BROKEN);
            sound_play(BREAK_SOUND, task, 0, 0);
            sound_set_volume(task, SOUND_ANY_ID, pot_volume);
            vibration_start(player_slot_of(player), VIBRATION_POWER,
                            VIBRATION_FREQ, 0);
            return;
        }
    }
    collision_register(task);
}

static void pot_break(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR spd;
    Float floor;

    pos = work->pos;
    spd.x = 0.0F;
    spd.y = pot_rise;
    spd.z = 0.0F;
    floor = surface_probe_floor(pos.x, pos.y + FLOOR_LOOK_HEIGHT, pos.z, &ang);
    debris_break_flags(&pos, work->ang.y, pot_parts, POT_PIECES, pot_speed,
                       pot_spread, floor, &spd, DEBRIS_TRAIL | DEBRIS_VANISH);
    placement_set_state(task, BROKEN_STATE);
}

static void pot_broken(struct task *task)
{
}
