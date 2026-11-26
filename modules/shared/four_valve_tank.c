/* the four-valve tank, TANK in Eternal Engine's object list. it bursts once
 * all four of its valves are knocked off. stg09/tank.c is the same kind of
 * tank with two valves. this source also defines vibration_pulses_create,
 * which the capsule bomb calls too */
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
#include "shared/four_valve_tank.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the lid's tint, in the work's mode. once a valve is off, the lid fades from
 * white to four_valve_tank_bright. it then pulses between
 * four_valve_tank_bright and four_valve_tank_dark */
enum four_valve_tank_mode {
    MODE_WHOLE,
    MODE_TO_BRIGHT,
    MODE_DIMMING,
    MODE_BRIGHTENING
};

/* the lid's color, a Uint32 in the bits of the work's scl.z. FADE, in scl.y,
 * is the tint's progress from one color to the next, from 0 to 1 */
#define COLOR(work) (*(Uint32 *)&(work)->scl.z)
#define FADE(work) ((work)->scl.y)
#define WHITE 0xffffffff
#define FADE_STEP 0.05F
#define DIM_STEP (1.0F / 30.0F)

/* the valves stand about the tank's axis, a line along z VALVE_LIFT above
 * pos. valve n lies VALVE_OUT from the axis, and FIRST_VALVE_Z + n *
 * VALVE_SPACING along it from pos. about the axis, it sits ang.y plus n
 * quarter turns from the top. each valve keeps its n in its work's tick */
#define VALVES 4
#define VALVE_OUT 15.0F
#define VALVE_LIFT 13.5F
#define FIRST_VALVE_Z (-15.0F)
#define VALVE_SPACING 7.5F
#define VALVE(valve_work) ((valve_work)->tick)
/* the valves that are off, in the tank's submode. valve n's bit is set once
 * the valve is off */
#define OFF_VALVES(work) ((work)->submode)
#define VALVE_BIT(n) (1 << (n))
#define ALL_VALVES (VALVE_BIT(0) | VALVE_BIT(1) | VALVE_BIT(2) | VALVE_BIT(3))
/* OFF_VALVES as the tank's last step left them, in the tank's tick. a step
 * plays VALVE_SOUND where they differ. the tick starts at 0, so the first
 * step plays it where a placement kept a valve off */
#define LAST_OFF_VALVES(work) ((work)->tick)

/* the tank's record, in task->motion: the lid's offset from its rest, and
 * its speed. the lid rests LID_HEIGHT above pos, and draws |offset.y| above
 * that, so it never sinks below its rest. VALVE_KICK alone moves the spring,
 * along y, so offset.x and offset.z stay 0 */
struct four_valve_tank_spring {
    NJS_VECTOR spd;
    NJS_POINT3 offset;
};
#define SPRING(task) ((struct four_valve_tank_spring *)(task)->motion.p)
#define SPRING_DAMPING_XZ 0.82F
#define SPRING_DAMPING_Y 0.8F
#define SPRING_STIFFNESS_X 0.63F
#define SPRING_STIFFNESS_Y 0.62F
#define SPRING_STIFFNESS_Z 0.68F
/* the kick to the spring's y speed as a valve comes off */
#define VALVE_KICK 1.5F
#define LID_HEIGHT 13.5F

/* the pieces fly out at PIECE_SPEED plus up to PIECE_SPREAD at random, with
 * PIECE_LIFT up. their floor is the highest at or below the point
 * FLOOR_LOOK_HEIGHT above the tank's pos */
#define BURST_POINTS 20
#define PIECE_SPEED 0.9F
#define PIECE_SPREAD 0.5F
#define PIECE_LIFT 0.8F
#define FLOOR_LOOK_HEIGHT 1.0F
#define PIECE_FLAGS (DEBRIS_TRAIL | DEBRIS_LANDING_BURST | DEBRIS_VANISH)
#define BURST_SOUND 3
#define BURST_POWER 5
#define BURST_PULSES 2
#define BURST_INTERVAL 10
#define BURST_FREQ 55
#define VALVE_SOUND 2
#define VALVE_DUST_SCALE 6.0F
#define VALVE_FLASH_SCALE 20.0F
#define VALVE_SPARKS 3
#define VALVE_FLASH_DEPTH 30.0F

#define TANK_PIECES 15
#define TANK_SHAPES 1
#define VALVE_SHAPES 1

/* the pulses' task runs at PULSES_LEVEL. it keeps vibration_pulses_create's
 * arguments where the macros below name them. its work's timer counts each
 * interval's steps down, and each interval's first step starts a pulse */
#define PULSES_LEVEL 3
#define PULSE_PLAYER(work) ((work)->mode)
#define PULSES_LEFT(work) ((work)->submode)
#define PULSE_INTERVAL(work) ((work)->tick)
#define PULSE_FREQ(work) ((work)->ang.x)
#define PULSE_POWER(task) ((task)->unk_3c.l)

/* the set editor's label for the placement's ang.y */
static const char editor_angle_y[12] = "<- ANGLE Y";
static const char vibration_task_name[12] = "TaVibExec";

#include "shared/four_valve_tank/text.inc"

/* clang-format off */
#include "shared/tank_art/four_valve_tank.h"
#include "shared/tank_art/four_valve_tank_piece_1.h"
#include "shared/tank_art/four_valve_tank_piece_2.h"
#include "shared/tank_art/four_valve_tank_piece_3.h"
#include "shared/tank_art/four_valve_tank_piece_4.h"
/* clang-format on */

NJS_TEXLIST *four_valve_tank_texlists[7] = {
#include "shared/four_valve_tank/four_valve_tank_texlists.inc"
};
/* clang-format off */
static struct debris_part four_valve_tank_pieces[TANK_PIECES] = {
#include "shared/four_valve_tank/four_valve_tank_pieces.inc"
};
/* clang-format on */
static Uint32 four_valve_tank_dark = 0xff301010;
static Uint32 four_valve_tank_bright = 0xffff1e1e;
/* at each step, a valve that is off puffs smoke with a chance of
 * smoke_chance. a puff's size is smoke_scale plus up to smoke_scale_range at
 * random. its speed is smoke_spd, turned about z with the valve */
static Float smoke_chance = 0.07F;
static Float smoke_scale = 3.0F;
static Float smoke_scale_range = 1.0F;
static NJS_VECTOR smoke_spd = {0.15F, 0.24F, 0.0F};
/* the tank's capsule, laid along z by its quarter turn about x. collision_init
 * gives each tank's record its own copy */
/* clang-format off */
static struct collision_shape four_valve_tank_shapes[TANK_SHAPES] = {
    {0, SHAPE_CAPSULE,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 0.0F, 0.0F}, 25.0F, 15.0F, 0.0F, 0, {QUARTER_TURN, 0, 0}},
};
static Float dat_four_valve_tank_0 = 5.0F;
static struct collision_shape valve_shapes[VALVE_SHAPES] = {
    {10, SHAPE_SPHERE,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_UNK_800000, {0.0F, 0.0F, 0.0F}, 2.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static void four_valve_tank_destroy(struct task *task);
static void four_valve_tank_step(struct task *task);
static void four_valve_tank_draw(struct task *task);
static void valve_init(struct task *task);
static void valve_destroy(struct task *task);
static void valve_step(struct task *task);
static void vibration_pulses_step(struct task *task);

/* set *pos to where valve lies on task's tank. where spd isn't null, also
 * turn *spd to that valve's side */
static void valve_place(struct task *task, int valve, NJS_POINT3 *pos,
                        NJS_VECTOR *spd)
{
    struct task_work *work = task->work;
    NJS_POINT3 point;

    /* @hack spelling: the point's y and z set before its x, where x, y and
     * z in turn would do */
    point.y = VALVE_OUT;
    point.z = FIRST_VALVE_Z + (valve * VALVE_SPACING);
    point.x = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.y + (valve * QUARTER_TURN));
    njCalcPoint(NULL, &point, pos);
    if (spd != NULL) {
        njCalcVector(NULL, spd, spd);
    }
    njPopMatrixEx();
    pos->y += VALVE_LIFT;
}

void four_valve_tank_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *valve;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct four_valve_tank_spring));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = four_valve_tank_draw;
    task->step = four_valve_tank_step;
    task->destructor = four_valve_tank_destroy;
    if (task->placement != NULL) {
        OFF_VALVES(work) = placement_get_state(task);
    }
    for (i = 0; i < VALVES; i++) {
        if (!(OFF_VALVES(work) & VALVE_BIT(i))) {
            valve = task_create_child(TASK_WORK, valve_init, task);
            if (valve != NULL) {
                valve_place(task, i, &valve->work->pos, NULL);
                VALVE(valve->work) = i;
            }
        }
    }
    collision_init(task, four_valve_tank_shapes, TANK_SHAPES, COLLISION_OBJECT);
    COLOR(work) = WHITE;
    FADE(work) = 0.0F;
}

static void four_valve_tank_destroy(struct task *task)
{
    struct task_work *work = task->work;

    task_destroy_children(task);
    if (task->placement != NULL) {
        placement_set_state(task, OFF_VALVES(work));
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void four_valve_tank_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    if (OFF_VALVES(work) >= ALL_VALVES) {
        struct angles ang;
        NJS_VECTOR lift;
        Float floor;

        pos = work->pos;
        lift.x = 0.0F;
        lift.y = PIECE_LIFT;
        lift.z = 0.0F;
        floor =
            surface_probe_floor(pos.x, pos.y + FLOOR_LOOK_HEIGHT, pos.z, &ang);
        debris_break_flags(&pos, work->ang.y, four_valve_tank_pieces,
                           TANK_PIECES, PIECE_SPEED, PIECE_SPREAD, floor, &lift,
                           PIECE_FLAGS);
        score_add(BURST_POINTS);
        object_sound_play(BURST_SOUND, &work->pos);
        vibration_pulses_create(0, BURST_POWER, BURST_PULSES, BURST_INTERVAL,
                                BURST_FREQ);
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
        return;
    }
    for (i = 0; i < VALVES; i++) {
        if ((OFF_VALVES(work) & VALVE_BIT(i)) &&
            rand() * RAND_SCALE < smoke_chance) {
            NJS_VECTOR spd;

            spd = smoke_spd;
            valve_place(task, i, &pos, &spd);
            black_smoke_sinking_create(
                &pos, &spd,
                smoke_scale + (rand() * smoke_scale_range * RAND_SCALE));
        }
    }
    if (OFF_VALVES(work) != 0) {
        switch (work->mode) {
        case MODE_WHOLE:
            work->mode = MODE_TO_BRIGHT;
            FADE(work) = 0.0F;
            COLOR(work) = WHITE;
            break;
        case MODE_TO_BRIGHT:
            FADE(work) += FADE_STEP;
            if (FADE(work) >= 1.0F) {
                FADE(work) = 0.0F;
                COLOR(work) = four_valve_tank_bright;
                work->mode = MODE_DIMMING;
            } else {
                COLOR(work) =
                    color_blend(WHITE, four_valve_tank_bright, FADE(work));
            }
            break;
        case MODE_DIMMING:
            FADE(work) += DIM_STEP;
            if (FADE(work) >= 1.0F) {
                FADE(work) = 0.0F;
                COLOR(work) = four_valve_tank_dark;
                work->mode = MODE_BRIGHTENING;
            } else {
                COLOR(work) = color_blend(four_valve_tank_bright,
                                          four_valve_tank_dark, FADE(work));
            }
            break;
        case MODE_BRIGHTENING:
            FADE(work) += FADE_STEP;
            if (FADE(work) >= 1.0F) {
                FADE(work) = 0.0F;
                COLOR(work) = four_valve_tank_bright;
                work->mode = MODE_DIMMING;
            } else {
                COLOR(work) = color_blend(four_valve_tank_dark,
                                          four_valve_tank_bright, FADE(work));
            }
            break;
        }
    }
    collision_register(task);
    if (LAST_OFF_VALVES(work) != OFF_VALVES(work)) {
        object_sound_play(VALVE_SOUND, &work->pos);
    }
    LAST_OFF_VALVES(work) = OFF_VALVES(work);
    SPRING(task)->offset.x += SPRING(task)->spd.x;
    SPRING(task)->offset.y += SPRING(task)->spd.y;
    SPRING(task)->offset.z += SPRING(task)->spd.z;
    SPRING(task)->spd.x = (SPRING(task)->spd.x * SPRING_DAMPING_XZ) -
                          (SPRING(task)->offset.x * SPRING_STIFFNESS_X);
    SPRING(task)->spd.y = (SPRING(task)->spd.y * SPRING_DAMPING_Y) -
                          (SPRING(task)->offset.y * SPRING_STIFFNESS_Y);
    SPRING(task)->spd.z = (SPRING(task)->spd.z * SPRING_DAMPING_XZ) -
                          (SPRING(task)->offset.z * SPRING_STIFFNESS_Z);
}

static void four_valve_tank_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 color;

    /* @hack redundant: a jump to the statement that follows it */
    goto draw;
draw:
    color = COLOR(work);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&four_valve_tank_texlist);
    njCnkEasyDrawModel(four_valve_tank_object->model);
    njTranslate(NULL, SPRING(task)->offset.x,
                njAbs(SPRING(task)->offset.y) + LID_HEIGHT,
                SPRING(task)->offset.z);
    model_draw_tinted(four_valve_tank_object->child->model, color);
    njPopMatrixEx();
}

static void valve_init(struct task *task)
{
    task->step = valve_step;
    task->destructor = valve_destroy;
    collision_init(task, valve_shapes, VALVE_SHAPES, COLLISION_ITEM);
    enemy_work_alloc(task);
}

static void valve_destroy(struct task *task)
{
    enemy_destroy(task);
}

static void valve_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *tank = task->parent;
    struct task_work *tank_work = tank->work;
    NJS_VECTOR spd;
    NJS_POINT3 pos;

    if (work->flags & WORK_HURT) {
        OFF_VALVES(tank_work) |= VALVE_BIT(VALVE(work));
        spd.x = 0.0F;
        spd.y = 0.0F;
        spd.z = 0.0F;
        pos = work->pos;
        dust_create(&pos, &spd, VALVE_DUST_SCALE);
        flash_manager_create_sparks_depth(pos.x, pos.y, pos.z,
                                          VALVE_FLASH_SCALE, VALVE_SPARKS,
                                          VALVE_FLASH_DEPTH);
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

static void vibration_pulses_step(struct task *task)
{
    struct task_work *work = task->work;

    if (work->timer == PULSE_INTERVAL(work)) {
        vibration_start(PULSE_PLAYER(work), PULSE_POWER(task), PULSE_FREQ(work),
                        VIBRATION_UNTIMED);
    }
    work->timer--;
    if (work->timer == 0) {
        work->timer = PULSE_INTERVAL(work);
        PULSES_LEFT(work)--;
        /* @hack spelling: a return, and the call in its else, where the call
         * under a test for 0 would do */
        if (PULSES_LEFT(work) != 0) {
            return;
        } else {
            task_kill(task);
        }
    }
}

void vibration_pulses_create(int player, int power, int pulses, int interval,
                             int freq)
{
    struct task *task;
    struct task_work *work;

    task = task_create(TASK_WORK, PULSES_LEVEL, vibration_pulses_step,
                       vibration_task_name);
    if (task != NULL) {
        work = task->work;
        PULSES_LEFT(work) = pulses;
        PULSE_INTERVAL(work) = interval;
        work->timer = PULSE_INTERVAL(work);
        PULSE_POWER(task) = power;
        PULSE_PLAYER(work) = player;
        PULSE_FREQ(work) = freq;
    }
}
