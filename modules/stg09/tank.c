/* the tank, TANK in Prison Lane's object list. it stands on a spring with two
 * valves, and bursts once both valves are knocked off */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "effect/particle.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "shared/black_smoke.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "shared/flash.h"
#include "sound/sound.h"
#include "stg09/tank.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#include "stg09/tank/text.inc"

/* the tank's glow, in the work's mode. once a valve is off, the tank fades
 * from white to tank_bright. it then pulses between tank_bright and
 * tank_dark */
enum tank_mode { MODE_WHOLE, MODE_TO_BRIGHT, MODE_DIMMING, MODE_BRIGHTENING };

/* the tank's color, a Uint32 in the bits of the work's scl.z. FADE, in
 * scl.y, is the glow's progress from one color to the next, from 0 to 1 */
/* @hack type: the color read as a Uint32 through scl.z's address, where a
 * Uint32 member would do */
#define COLOR(work) (*(Uint32 *)&(work)->scl.z)
#define FADE(work) ((work)->scl.y)
#define WHITE 0xffffffff
/* a step of the glow adds BRIGHTEN_STEP to FADE as the tank turns toward
 * tank_bright, and DIM_STEP as it turns toward tank_dark */
#define BRIGHTEN_STEP 0.05F
#define DIM_STEPS 30.0F
#define DIM_STEP (1.0F / DIM_STEPS)

/* valve n lies VALVE_OUT from the tank's axis, and VALVE_HEIGHT * n above its
 * pos. valve 0 is on the tank's +x side, and valve 1 on its -x side. a
 * valve's work's tick holds its n */
#define VALVES 2
#define VALVE_OUT 15.0F
#define VALVE_HEIGHT 27.5F
#define VALVE_BIT(n) (1 << (n))
#define ALL_VALVES (VALVE_BIT(0) | VALVE_BIT(1))
#define VALVE(valve_work) ((valve_work)->tick)

/* the tank's record, in task->motion: the spring's offset, and its speed.
 * offset moves the node's child from its rest. the child draws |offset.y|
 * below its rest, so it never rises above it */
struct tank_spring {
    NJS_VECTOR spd;
    NJS_POINT3 offset;
};

#define SPRING(task) ((struct tank_spring *)(task)->motion.p)
#define SPRING_DAMPING_XZ 0.82F
#define SPRING_DAMPING_Y 0.8F
#define SPRING_STIFFNESS_X 0.63F
#define SPRING_STIFFNESS_Y 0.62F
#define SPRING_STIFFNESS_Z 0.68F
/* the kick to the spring's y speed as a valve comes off */
#define VALVE_KICK 1.5F

/* the pieces fly out at PIECE_SPEED plus up to PIECE_SPREAD at random, with
 * PIECE_LIFT up. their floor is the highest at or below the point
 * FLOOR_LOOK_HEIGHT above the tank's pos */
#define BURST_POINTS 20
#define BURST_SOUND 0x1022
#define PIECE_SPEED 0.9F
#define PIECE_SPREAD 0.5F
#define PIECE_LIFT 0.8F
#define FLOOR_LOOK_HEIGHT 1.0F
#define PIECE_FLAGS (DEBRIS_TRAIL | DEBRIS_LANDING_BURST | DEBRIS_VANISH)
#define VALVE_SOUND 0x1011
#define VALVE_DUST_SCALE 6.0F
#define VALVE_FLASH_SCALE 20.0F
#define VALVE_SPARKS 3
#define VALVE_FLASH_DEPTH 30.0F

#define TANK_PIECES 6
#define TANK_SHAPES 1
#define VALVE_SHAPES 1

static NJS_TEXLIST tank_texlist;
/* @hack type: the tank's node declared as an array, where the node alone
 * would do */
static NJS_CNK_OBJECT tank_object[1];
static NJS_TEXLIST tank_rim_texlist;
static NJS_TEXLIST tank_base_texlist;
static NJS_TEXLIST tank_side_texlist;
static NJS_CNK_MODEL tank_rim_model;
static NJS_CNK_MODEL tank_base_model;
static NJS_CNK_MODEL tank_side_model;

static struct debris_part tank_pieces[TANK_PIECES];
static Uint32 tank_dark;
static Uint32 tank_bright;
static Float smoke_chance;
static Float smoke_scale;
static Float smoke_scale_range;
static NJS_VECTOR smoke_spd;
static struct collision_shape tank_shapes[TANK_SHAPES];
static Float dat_8c578e94;
static struct collision_shape valve_shapes[VALVE_SHAPES];

static void tank_destroy(struct task *task);
static void tank_step(struct task *task);
static void tank_draw(struct task *task);
static void tank_valve_init(struct task *task);
static void tank_valve_destroy(struct task *task);
static void tank_valve_step(struct task *task);

/* set *pos to where valve lies on task's tank. where spd isn't null, also
 * turn *spd to that valve's side */
static void tank_valve_place(struct task *task, int valve, NJS_POINT3 *pos,
                             NJS_VECTOR *spd)
{
    struct task_work *work = task->work;
    NJS_POINT3 point;

    point.x = VALVE_OUT;
    point.y = valve * VALVE_HEIGHT;
    point.z = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y + (valve * HALF_TURN));
    njCalcPoint(NULL, &point, pos);
    if (spd != NULL) {
        njCalcVector(NULL, spd, spd);
    }
    njPopMatrixEx();
}

void tank_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *valve;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct tank_spring));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = tank_draw;
    task->step = tank_step;
    task->destructor = tank_destroy;
    if (task->placement != NULL) {
        work->submode = placement_get_state(task);
    }
    for (i = 0; i < VALVES; i++) {
        if (!(work->submode & VALVE_BIT(i))) {
            valve = task_create_child(TASK_WORK, tank_valve_init, task);
            if (valve != NULL) {
                tank_valve_place(task, i, &valve->work->pos, NULL);
                VALVE(valve->work) = i;
            }
        }
    }
    collision_init(task, tank_shapes, TANK_SHAPES, COLLISION_OBJECT);
    COLOR(work) = WHITE;
    FADE(work) = 0.0F;
}

static void tank_destroy(struct task *task)
{
    struct task_work *work = task->work;

    task_destroy_children(task);
    if (task->placement != NULL) {
        placement_set_state(task, work->submode);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void tank_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    if (work->submode >= ALL_VALVES) {
        struct angles ang;
        NJS_VECTOR lift;
        Float floor;

        pos = work->pos;
        lift.x = 0.0F;
        lift.y = PIECE_LIFT;
        lift.z = 0.0F;
        floor =
            surface_probe_floor(pos.x, pos.y + FLOOR_LOOK_HEIGHT, pos.z, &ang);
        score_add(BURST_POINTS);
        debris_break_flags(&pos, work->ang.y, tank_pieces, TANK_PIECES,
                           PIECE_SPEED, PIECE_SPREAD, floor, &lift,
                           PIECE_FLAGS);
        sound_play(BURST_SOUND, NULL, 0, 0);
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
        return;
    }
    for (i = 0; i < VALVES; i++) {
        if ((work->submode & VALVE_BIT(i)) &&
            smoke_chance > (Float)rand() * RAND_SCALE) {
            NJS_VECTOR spd;

            spd = smoke_spd;
            tank_valve_place(task, i, &pos, &spd);
            black_smoke_sinking_create(
                &pos, &spd,
                smoke_scale + ((Float)rand() * RAND_SCALE * smoke_scale_range));
        }
    }
    if (work->submode != 0) {
        switch (work->mode) {
        case MODE_WHOLE:
            work->mode = MODE_TO_BRIGHT;
            FADE(work) = 0.0F;
            COLOR(work) = WHITE;
            break;
        case MODE_TO_BRIGHT:
            FADE(work) += BRIGHTEN_STEP;
            if (FADE(work) >= 1.0F) {
                FADE(work) = 0.0F;
                COLOR(work) = tank_bright;
                work->mode = MODE_DIMMING;
            } else {
                COLOR(work) = color_blend(WHITE, tank_bright, FADE(work));
            }
            break;
        case MODE_DIMMING:
            FADE(work) += DIM_STEP;
            if (FADE(work) >= 1.0F) {
                FADE(work) = 0.0F;
                COLOR(work) = tank_dark;
                work->mode = MODE_BRIGHTENING;
            } else {
                COLOR(work) = color_blend(tank_bright, tank_dark, FADE(work));
            }
            break;
        case MODE_BRIGHTENING:
            FADE(work) += BRIGHTEN_STEP;
            if (FADE(work) >= 1.0F) {
                FADE(work) = 0.0F;
                COLOR(work) = tank_bright;
                work->mode = MODE_DIMMING;
            } else {
                COLOR(work) = color_blend(tank_dark, tank_bright, FADE(work));
            }
            break;
        }
    }
    SPRING(task)->offset.x += SPRING(task)->spd.x;
    SPRING(task)->offset.y += SPRING(task)->spd.y;
    SPRING(task)->offset.z += SPRING(task)->spd.z;
    SPRING(task)->spd.x = (SPRING(task)->spd.x * SPRING_DAMPING_XZ) -
                          (SPRING(task)->offset.x * SPRING_STIFFNESS_X);
    SPRING(task)->spd.y = (SPRING(task)->spd.y * SPRING_DAMPING_Y) -
                          (SPRING(task)->offset.y * SPRING_STIFFNESS_Y);
    SPRING(task)->spd.z = (SPRING(task)->spd.z * SPRING_DAMPING_XZ) -
                          (SPRING(task)->offset.z * SPRING_STIFFNESS_Z);
    collision_register(task);
}

static void tank_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 color;

    /* @hack spelling: a goto to the next statement, where the statement
     * alone would do */
    goto draw;
draw:
    color = COLOR(work);
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&tank_texlist);
    njCnkEasyDrawModel(tank_object->model);
    njTranslate(NULL, SPRING(task)->offset.x, -njAbs(SPRING(task)->offset.y),
                SPRING(task)->offset.z);
    model_draw_tinted(tank_object->child->model, color);
    njPopMatrixEx();
}

static void tank_valve_init(struct task *task)
{
    task->step = tank_valve_step;
    task->destructor = tank_valve_destroy;
    collision_init(task, valve_shapes, VALVE_SHAPES, COLLISION_ITEM);
    enemy_work_alloc(task);
}

static void tank_valve_destroy(struct task *task)
{
    enemy_destroy(task);
}

static void tank_valve_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *tank = task->parent;
    struct task_work *tank_work = tank->work;
    NJS_VECTOR spd;
    NJS_POINT3 pos;

    if (work->flags & WORK_HURT) {
        tank_work->submode |= VALVE_BIT(VALVE(work));
        spd.x = 0.0F;
        spd.y = 0.0F;
        spd.z = 0.0F;
        pos = work->pos;
        dust_create(&pos, &spd, VALVE_DUST_SCALE);
        flash_manager_create_sparks_depth(pos.x, pos.y, pos.z,
                                          VALVE_FLASH_SCALE, VALVE_SPARKS,
                                          VALVE_FLASH_DEPTH);
        sound_play(VALVE_SOUND, NULL, 0, 0);
        SPRING(tank)->spd.y += VALVE_KICK;
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
    } else {
        collision_register(task);
    }
}

#include "shared/prison_machinery/tank_textures.h"
#include "stg09/fixtures/tank_cap.h" /* IWYU pragma: keep -- tank_object's child */
#include "shared/prison_machinery/tank.h"
#include "shared/prison_machinery/tank_rim.h"
#include "shared/prison_machinery/tank_base.h"
#include "shared/prison_machinery/tank_side.h"

NJS_TEXLIST *tank_texlists[6] = {
#include "stg09/tank/tank_texlists.inc"
};
/* the tank's pieces: four quarters of its rim, a piece of its side, and its
 * base */
static struct debris_part tank_pieces[TANK_PIECES] = {
#include "stg09/tank/tank_pieces.inc"
};
static Uint32 tank_dark = 0xff803838;
static Uint32 tank_bright = 0xffff5050;
/* at each step, a valve that is off puffs smoke with a chance of
 * smoke_chance. a puff's size is smoke_scale plus up to smoke_scale_range at
 * random. its speed is smoke_spd, turned to the valve's side */
static Float smoke_chance = 0.07F;
static Float smoke_scale = 3.0F;
static Float smoke_scale_range = 1.0F;
static NJS_VECTOR smoke_spd = {0.15F, 0.24F, 0.0F};
/* clang-format off */
static struct collision_shape tank_shapes[TANK_SHAPES] = {
    {0, SHAPE_CYLINDER,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 27.5F, 0.0F}, 15.0F, 29.5F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static Float dat_8c578e94 = 5.0F;
/* clang-format off */
static struct collision_shape valve_shapes[VALVE_SHAPES] = {
    {10, SHAPE_SPHERE,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_UNK_800000, {0.0F, 0.0F, 0.0F}, 2.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
