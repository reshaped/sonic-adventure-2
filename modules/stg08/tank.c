/* TANK in Security Hall's object list: a tank whose valves a player's
 * attacks knock off, one a hit. smoke leaks where a valve is off, and the
 * tank's top flashes. the tank breaks once every valve is off. the work's
 * submode counts the valves off, and the placement's state keeps the count */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "shared/dust.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "effect/particle.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/black_smoke.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "sound/sound.h"
#include "stg08/fixture_art.h"
#include "stg08/machines.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the top's glow, in the work's mode. once a valve is off, the top fades
 * from WHITE to tank_bright. it then pulses between tank_bright and
 * tank_dark */
enum tank_mode { MODE_WHOLE, MODE_TO_BRIGHT, MODE_DIMMING, MODE_BRIGHTENING };

/* valve n lies VALVE_OUT from the tank's axis, and VALVE_HEIGHT * n above its
 * pos. valve 0 is on the tank's +x side, and valve 1 on its -x side */
#define VALVES 2
#define VALVE_OUT 15.0F
#define VALVE_HEIGHT 21.5F
/* the sounds, ids in sound list 1, which object_sound_play's kinds 0 and 3
 * play too */
#define VALVE_SOUND 0x1011
#define BURST_SOUND 0x1022
#define SOUND_PRIORITY 0
/* the top's tint while no valve is off. model_draw_tinted takes it as
 * COLOR_NONE, and draws the top as it is */
#define WHITE 0xffffffff
/* a step of the glow adds BRIGHTEN_STEP to FADE as the top turns toward
 * tank_bright, and DIM_STEP as it turns toward tank_dark */
#define BRIGHTEN_STEP 0.05F
#define DIM_STEPS 30.0F
#define DIM_STEP (1.0F / DIM_STEPS)
/* the pieces fly out at PIECE_SPEED plus up to PIECE_SPREAD at random, with
 * PIECE_LIFT up. their floor is the highest at or below the point
 * FLOOR_LOOK_HEIGHT above the tank's pos */
#define BURST_POINTS 20
#define PIECE_SPEED 0.9F
#define PIECE_SPREAD 0.5F
#define PIECE_LIFT 0.8F
#define FLOOR_LOOK_HEIGHT 1.0F
#define PIECE_FLAGS (DEBRIS_TRAIL | DEBRIS_LANDING_BURST | DEBRIS_VANISH)
/* the work's tick counts the steps since an attack last touched the tank, up
 * to TICK_MAX. a touch knocks a valve off only where the tick is above
 * HIT_GAP */
#define HIT_GAP 5
#define TICK_MAX 120
/* each step the spring's offset moves by its speed. the speed then becomes
 * SPRING_DAMPING times itself, less SPRING_STIFFNESS times the offset, by
 * each axis's constants */
#define SPRING_DAMPING_XZ 0.82F
#define SPRING_DAMPING_Y 0.8F
#define SPRING_STIFFNESS_X 0.63F
#define SPRING_STIFFNESS_Y 0.62F
#define SPRING_STIFFNESS_Z 0.68F
/* the kick to the spring's y speed as a valve comes off. nothing else kicks
 * the spring */
#define VALVE_KICK 1.5F

/* the tank's record, in task->motion: the spring's offset, and its speed.
 * offset moves the top from its rest. the top draws |offset.y| below its
 * rest, so it never rises above it */
struct tank_spring {
    NJS_VECTOR spd;
    NJS_POINT3 offset;
};

#define SPRING(task) ((struct tank_spring *)(task)->motion.p)
/* the top's tint, a Uint32 in the bits of the work's scl.z. FADE, in scl.y,
 * is the glow's progress from one color to the next, from 0 to 1. the tank
 * reads no scale */
#define COLOR(work) (*(Uint32 *)&(work)->scl.z)
#define FADE(work) ((work)->scl.y)

NJS_TEXLIST *security_tank_textures[] = {
    &security_tank_texlist,
    &security_tank_rim_texlist,
    &security_tank_base_texlist,
    &security_tank_side_texlist,
    &dust_texlist,
    NULL,
};

/* clang-format off */
static struct debris_part tank_pieces[] = {
    {&security_tank_rim_texlist, &security_tank_rim_model,
     {10.100001F, 21.150005F, 4.999999F}, {0, 0, 0}},
    {&security_tank_rim_texlist, &security_tank_rim_model,
     {5.2F, 21.150005F, -10.050003F}, {0, QUARTER_TURN, 0}},
    {&security_tank_rim_texlist, &security_tank_rim_model,
     {-9.650003F, 21.150005F, -5.200004F}, {0, HALF_TURN, 0}},
    {&security_tank_rim_texlist, &security_tank_rim_model,
     {-4.800003F, 21.150005F, 9.849993F}, {0, -QUARTER_TURN, 0}},
    {&security_tank_side_texlist, &security_tank_side_model,
     {-3.200001F, 10.099999F, 14.200004F}, {0, 0, 0}},
    {&security_tank_base_texlist, &security_tank_base_model,
     {0.0F, 2.099999F, 0.0F}, {0, 0, 0}},
};
/* clang-format on */
static Uint32 tank_dark = 0xff803838;
static Uint32 tank_bright = 0xffff5050;
/* each step, each valve off puffs smoke with a chance of smoke_chance, and
 * plays VALVE_SOUND with each puff. the puff leaves at smoke_spd, turned with
 * the valve's side of the tank. its scale is smoke_scale, and up to
 * smoke_scale_range more */
static Float smoke_chance = 0.07F;
static Float smoke_scale = 3.0F;
static Float smoke_scale_range = 1.0F;
static NJS_VECTOR smoke_spd = {0.15F, 0.24F, 0.0F};
/* clang-format off */
static struct collision_shape tank_shape = {
    0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    0, 0, {0.0F, 21.5F, 0.0F}, 15.0F, 21.5F, 0.0F, 0, {0, 0, 0}};
/* clang-format on */

static void tank_destroy(struct task *task);
static void tank_step(struct task *task);
static void tank_draw(struct task *task);

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
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y + (valve * HALF_TURN));
    njCalcPoint(NULL, &point, pos);
    if (spd != NULL) {
        njCalcVector(NULL, spd, spd);
    }
    njPopMatrix(1);
}

void security_tank_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct tank_spring));
    if (task->motion.p == NULL) {
        return;
    }
    task->draw = tank_draw;
    task->step = tank_step;
    task->destructor = tank_destroy;
    if (task->placement != NULL) {
        work->submode = placement_get_state(task);
    }
    collision_init(task, &tank_shape, 1, COLLISION_OBJECT);
    COLOR(work) = WHITE;
    FADE(work) = 0.0F;
}

static void tank_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement != NULL) {
        placement_set_state(task, work->submode);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void tank_step(struct task *task)
{
    struct task_work *work = task->work;
    struct collision_work *collision;
    NJS_POINT3 pos;
    /* @hack type: smoke, a puff's speed, also takes the floor's angles, where
     * a struct angles of their own would do */
    NJS_VECTOR smoke;
    NJS_VECTOR lift;
    Float floor;
    int valve;

    if (placement_try_leave(task)) {
        return;
    }
    if (work->submode >= VALVES) {
        pos = work->pos;
        lift.x = 0.0F;
        lift.y = PIECE_LIFT;
        lift.z = 0.0F;
        score_add(BURST_POINTS);
        floor = surface_probe_floor(pos.x, pos.y + FLOOR_LOOK_HEIGHT, pos.z,
                                    (struct angles *)&smoke);
        debris_break_flags(&pos, work->ang.y, tank_pieces,
                           sizeof(tank_pieces) / sizeof(tank_pieces[0]),
                           PIECE_SPEED, PIECE_SPREAD, floor, &lift,
                           PIECE_FLAGS);
        sound_play(BURST_SOUND, NULL, SOUND_PRIORITY, 0);
        if (task->placement) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
        return;
    }
    for (valve = 0; valve < work->submode; valve++) {
        if (smoke_chance > RANDOM_UNIT()) {
            smoke = smoke_spd;
            tank_valve_place(task, valve, &pos, &smoke);
            sound_play(VALVE_SOUND, NULL, SOUND_PRIORITY, 0);
            black_smoke_sinking_create(&pos, &smoke,
                                       smoke_scale +
                                           (RANDOM_UNIT() * smoke_scale_range));
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
    collision = work->collision;
    if (collision && (collision->flags & COLLISION_TOUCHED) &&
        player_attacks_task(task, collision->hit->owner)) {
        if (work->tick > HIT_GAP) {
            SPRING(task)->spd.y += VALVE_KICK;
            work->submode++;
        }
        work->tick = 0;
    } else if (work->tick < TICK_MAX) {
        work->tick++;
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

/* @hack spelling: njPushMatrix's argument from current_matrix, an inline
 * function that returns 0, where NULL passed in place would do */
#pragma inline(current_matrix)
static NJS_MATRIX *current_matrix(void)
{
    return 0;
}

static void tank_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 color;

    /* @hack spelling: a goto to the next line, where none would do */
    goto draw;
draw:
    color = COLOR(work);

    /* @hack redundant: a statement kept for its string, the set editor's
     * label for ang.y, which SHC places in this source's constants */
    (void)"<- ANGLE Y";

    njPushMatrix(current_matrix());
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&security_tank_texlist);
    njCnkEasyDrawModel(security_tank_object->model);
    njTranslate(NULL, SPRING(task)->offset.x, -njAbs(SPRING(task)->offset.y),
                SPRING(task)->offset.z);
    /* the top, the tank's child */
    model_draw_tinted(security_tank_object->child->model, color);
    njPopMatrix(1);
}
