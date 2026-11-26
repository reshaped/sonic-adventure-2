/* the gas cylinder, BOMBE in Iron Gate's object list. it bursts into pieces at
 * an attack's touch. Prison Lane's, in stg09/gas_cylinder.c, scores before it
 * throws the pieces */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "effect/burst.h"
#include "hud/hud.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "shared/white_breath.h"
#include "stg11/gas_cylinder.h"
#include "system/angle.h"
#include "system/task.h"

#include "stg11/gas_cylinder/text.inc"

#define BURST_POINTS 20
#define BURST_SOUND_KIND 9
/* the pieces fly out at PIECE_SPEED plus up to PIECE_SPREAD at random, with
 * PIECE_LIFT up. their floor is the highest at or below the point
 * FLOOR_LOOK_HEIGHT above the cylinder's pos */
#define PIECE_SPEED 0.9F
#define PIECE_SPREAD 0.5F
#define PIECE_LIFT 0.7F
#define FLOOR_LOOK_HEIGHT 1.0F
#define PIECE_FLAGS (DEBRIS_TRAIL | DEBRIS_LANDING_BURST | DEBRIS_VANISH)

#define GAS_CYLINDER_PIECES 6
#define GAS_CYLINDER_SHAPES 1

static NJS_TEXLIST gas_cylinder_texlist;
/* @hack type: the cylinder's node declared as an array, where the node alone
 * would do */
static NJS_CNK_OBJECT gas_cylinder_object[1];
static NJS_TEXLIST gas_cylinder_top_texlist;
static NJS_CNK_MODEL gas_cylinder_top_model;
static NJS_TEXLIST gas_cylinder_side_texlist;
static NJS_CNK_MODEL gas_cylinder_side_model;
static NJS_TEXLIST gas_cylinder_base_texlist;
static NJS_CNK_MODEL gas_cylinder_base_model;

static struct debris_part gas_cylinder_pieces[GAS_CYLINDER_PIECES];
static struct collision_shape gas_cylinder_shapes[GAS_CYLINDER_SHAPES];

static void gas_cylinder_destroy(struct task *task);
static void gas_cylinder_step(struct task *task);
static void gas_cylinder_draw(struct task *task);

void iron_gate_gas_cylinder_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = gas_cylinder_draw;
    task->step = gas_cylinder_step;
    task->destructor = gas_cylinder_destroy;
    if (task->placement != NULL) {
        work->submode = placement_get_state(task);
    }
    collision_init(task, gas_cylinder_shapes, GAS_CYLINDER_SHAPES,
                   COLLISION_OBJECT);
}

static void gas_cylinder_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement != NULL) {
        placement_set_state(task, work->submode);
    }
}

static void gas_cylinder_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    if (placement_try_leave(task)) {
        return;
    }
    if (collision_next_attack(task) != NULL) {
        struct angles ang;
        NJS_VECTOR lift;
        Float floor;

        pos = work->pos;
        lift.x = 0.0F;
        lift.y = PIECE_LIFT;
        lift.z = 0.0F;
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
        floor =
            surface_probe_floor(pos.x, pos.y + FLOOR_LOOK_HEIGHT, pos.z, &ang);
        debris_break_flags(&pos, work->ang.y, gas_cylinder_pieces,
                           GAS_CYLINDER_PIECES, PIECE_SPEED, PIECE_SPREAD,
                           floor, &lift, PIECE_FLAGS);
        score_add(BURST_POINTS);
        object_sound_play(BURST_SOUND_KIND, &work->pos);
    } else {
        collision_register(task);
    }
}

static void gas_cylinder_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its SET editor
     * caption, which SHC still places in this source's constants */
    if (0) {
        (void)GAS_CYLINDER_EDITOR_ANGLE_Y;
    }
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&gas_cylinder_texlist);
    njCnkEasyDrawModel(gas_cylinder_object->model);
    njPopMatrix(1);
}

#include "stg11/fixtures/gas_cylinder.h"
#include "shared/prison_machinery/gas_cylinder_top.h"
#include "stg11/fixtures/gas_cylinder_side.h"
#include "shared/prison_machinery/gas_cylinder_base.h"

NJS_TEXLIST *iron_gate_gas_cylinder_texlists[8] = {
#include "stg11/gas_cylinder/iron_gate_gas_cylinder_texlists.inc"
};
/* the pieces the cylinder bursts into: its top, its base, and four quarters
 * of its side */
static struct debris_part gas_cylinder_pieces[GAS_CYLINDER_PIECES] = {
#include "stg11/gas_cylinder/gas_cylinder_pieces.inc"
};
static struct collision_shape gas_cylinder_shapes[GAS_CYLINDER_SHAPES] = {
#include "stg11/gas_cylinder/gas_cylinder_shapes.inc"
};
