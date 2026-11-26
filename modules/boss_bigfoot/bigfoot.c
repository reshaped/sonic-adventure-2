/* Big Foot, the hero story's first boss, the task BossBigFootExec. the
 * stage's step creates it. it flies on three jets, charging back and forth
 * across the arena. it aims a laser on each charge but the first after a
 * take-off. it then lands to fire missiles from two banks of pods. each hit
 * on its capsule takes one of its power */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "boss_bigfoot/bigfoot.h"
#include "boss_bigfoot/model_art.h"
#include "boss_bigfoot/motion_art.h"
#include "camera/bossinit.h"
#include "camera/bosspoint.h"
#include "camera/stack.h"
#include "collision/collision.h"
#include "draw/chunk.h"
#include "draw/light.h"
#include "draw/motion.h"
#include "effect/burst.h"
#include "object/mover.h"
#include "player/player.h"
#include "shared/gun_mech/effect_art.h"
#include "shared/boss.h"
#include "shared/mech_boss.h"
#include "shared/mech_boss_camera.h"
#include "shared/mech_boss_capsule.h"
#include "shared/mech_boss_laser.h"
#include "shared/mech_boss_laser_beam.h"
#include "shared/mech_boss_missile.h"
#include "shared/mech_boss_sounds.h"
#include "shared/mech_boss_title.h"
#include "shared/mech_boss_voice.h"
#include "shared/node_aim.h"
#include "shared/rising_sound.h"
#include "shared/smoke_puff.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"
#include "vmu/vibration.h"

#define JETS 3

/* Joint positions are captured during the model traversal, in world and body
 * coordinates. */
static NJS_POINT3 bigfoot_pods_1[MECH_BOSS_PODS];
static NJS_POINT3 bigfoot_pods_0[MECH_BOSS_PODS];
static NJS_POINT3 bigfoot_jet_points[JETS];
static NJS_POINT3 bigfoot_body_pos;
static NJS_POINT3 bigfoot_foot_a_pos;
static NJS_POINT3 bigfoot_foot_b_pos;
static NJS_POINT3 bigfoot_turret_pos;
static NJS_POINT3 bigfoot_body_local;
static NJS_POINT3 bigfoot_foot_a_local;
static NJS_POINT3 bigfoot_foot_b_local;
static NJS_POINT3 bigfoot_laser_pos;
/* Ninja's matrix transfers require an eight-byte boundary. */
#ifdef NONMATCHING
/* on the Dreamcast, this side's NJS_MATRIX type gives the boundary, as
 * nj_render_types.h says */
#else
#pragma aligndata8(bigfoot_view_to_world)
#endif
static NJS_MATRIX bigfoot_view_to_world;
static NJS_MATRIX bigfoot_view_to_body;

/* Big Foot's modes, in its work's mode */
enum bigfoot_mode {
    MODE_INTRO = 1,
    MODE_TAKE_OFF = 2,
    MODE_CHARGE = 3, /* cross the arena past the player */
    MODE_COME_ROUND = 4, /* turn back toward the player */
    MODE_LAND = 5, /* fly toward the middle and land */
    MODE_FACE_PLAYER = 6,
    MODE_FIRE = 8, /* fire the missiles one by one */
    MODE_HIT_ON_GROUND = 9,
    MODE_DOWN = 10, /* the last hit landed on the ground */
    MODE_HIT_IN_AIR = 12,
    MODE_FALL = 13 /* the last hit landed in the air */
};

/* the entries of bigfoot_motions */
enum bigfoot_motion {
    MOTION_CROUCH = 0,
    MOTION_JUMP = 1,
    MOTION_LIFT = 2,
    MOTION_FLY = 3,
    MOTION_LEAN = 4,
    MOTION_STOMP = 5,
    MOTION_STAND = 6,
    MOTION_STEP = 7,
    MOTION_RECOVER = 10,
    MOTION_HIT = 12,
    MOTION_COLLAPSE = 13,
    MOTION_HIT_EMPTY = 14, /* a hit with no missiles held */
    MOTION_KNOCKED = 15
};
#define BIGFOOT_MOTIONS 16

/* the work's flags */
/* Big Foot stands on the ground. its body and shapes don't sway, and its
 * engines are silent */
#define BIGFOOT_LANDED 0x1
#define BIGFOOT_MISSILES 0x100 /* it holds missiles */
#define BIGFOOT_LASER 0x200 /* its charges aim the laser */
/* a landing's lean, or a fall's leveling, is done */
#define BIGFOOT_PITCHED 0x400
/* a hit in the air sends Big Foot on toward +x. the hit sets the flag on a move
 * toward +x, or at rest where x <= 0 */
#define BIGFOOT_HEADING_POS_X 0x800
/* no hit counts until the first take-off rises above GROUND_HEIGHT */
#define BIGFOOT_STARTING 0x1000

#define BIGFOOT_LEVEL 2 /* task_create's level */
#define FULL_POWER 4.0F /* the hits that beat Big Foot */
#define MISSILE_SLOTS 16
#define MISSILE_RADIUS 120.0F
#define MISSILE_SPEED 1.8F
#define BIGFOOT_SHAPES 11

/* LOW_SHAPE is the cylinder 40 below the body's point. a landing's approach
 * turns it on. a charge, or the righting after a hit in the air, turns it
 * off. UNDER_SHAPE lies under the body. a take-off turns it on, and the
 * landing that loads the missiles turns it off */
#define LOW_SHAPE 7
#define UNDER_SHAPE 8

/* the mover's gravity, and the lift each step in the air adds to cancel it */
#define GRAVITY (-0.05F)
#define LIFT (-GRAVITY)

/* how far the sway's phases move in a step in flight. a reel from a hit and a
 * fall move them by the REEL_ steps. a righting moves them by the RIGHTING_
 * steps */
#define SWAY_X_STEP 0x210
#define SWAY_Y_STEP 0x250
#define SWAY_Z_STEP 0x300
#define REEL_SWAY_X_STEP 0x420
#define REEL_SWAY_Y_STEP 0x500
#define REEL_SWAY_Z_STEP 0x600
#define RIGHTING_SWAY_X_STEP 0x300
#define RIGHTING_SWAY_Y_STEP 0x370
#define RIGHTING_SWAY_Z_STEP 0x400

/* each step, the steer's roll speed gains ROLL_GAIN of the gap to its aim. it
 * loses ROLL_DRAG of itself. its pitch speed does the same by PITCH_GAIN and
 * PITCH_DRAG */
#define ROLL_GAIN 0.0026F
#define ROLL_DRAG 0.045F
#define PITCH_GAIN 0.0035F
#define PITCH_DRAG 0.08F

/* the most the turret's aim turns about y and about z in a call */
#define TURRET_SPEED 32

/* below GROUND_HEIGHT, Big Foot counts as on the ground */
#define GROUND_HEIGHT 5.0F

/* the height Big Foot settles at as it takes off, or rights itself after a
 * hit. each step, its climb gains HOVER_PULL of the height left, which counts
 * at most HOVER_ERROR_LIMIT either way. it settles once the height left and
 * its climb both lie within HOVER_TOLERANCE of 0 */
#define HOVER_HEIGHT 10.0F
#define HOVER_PULL 0.02F
#define HOVER_ERROR_LIMIT 3.0F
#define HOVER_TOLERANCE 0.05F

/* a take-off raises a ring of dust every DUST_RING_STEPS after its jump,
 * until DUST_END_STEPS */
#define DUST_RING_STEPS 5
#define DUST_END_STEPS 15

/* a charge heads for player 0's z, held within CHARGE_LIMIT_Z of z = 0. once
 * within PASS_X of the player along x, it heads for BIGFOOT_ARENA_RADIUS on
 * the side it moves toward. it ends once past it */
#define CHARGE_LIMIT_Z 90.0F
#define PASS_X 10.0F

/* a turn back heads for player 0's z. where Big Foot lies past RETURN_Z from
 * the x axis, and the player farther out still, it heads inward of Big Foot's
 * own z instead. the turn ends within RETURN_X of x = 0, or within
 * HIT_RETURN_X after a hit in the air */
#define RETURN_Z 48.0F
#define RETURN_X 150.0F
#define HIT_RETURN_X 200.0F

/* a landing turns LOW_SHAPE on within LOW_SHAPE_RADIUS of the origin. it
 * stops accelerating within BRAKE_RADIUS, and brakes. it then leans forward
 * LEAN_PITCH and levels again, turning by LEAN_SPEED a step */
#define LOW_SHAPE_RADIUS 170.0F
#define BRAKE_RADIUS 90.0F
#define LEAN_PITCH 0x400
#define LEAN_SPEED 192

/* a fall crashes where the body comes down to CRASH_HEIGHT. while the body
 * lies at or below FALLEN_HEIGHT, each shape lies FALLEN_SHIFT farther along
 * the body's z */
#define CRASH_HEIGHT 11.5F
#define FALLEN_HEIGHT 13.5F
#define FALLEN_SHIFT 20.0F

/* how far the capsule lies above the body's point */
#define CAPSULE_HEIGHT 6.5F

#define INTRO_FOCUS_HEIGHT 38.0F

/* the firing launches a missile every SHOT_STEPS, and rests VOLLEY_REST_STEPS
 * after each volley. it closes the hatch CLOSE_STEPS before Big Foot turns to
 * face player 0 */
#define SHOT_STEPS 15
#define VOLLEY_REST_STEPS 45
#define CLOSE_STEPS 60

/* MOTION_STEP's frames at which each foot lands */
#define FOOT_A_FRAME 14.0F
#define FOOT_B_FRAME 29.0F

/* the steps between rumbles while Big Foot reels or collapses on the ground */
#define RUMBLE_STEPS 25

#define CHARGE_VOLUME 60
#define CHARGE_SOUND_STEPS 80
#define LAND_VOLUME 100
#define STEP_SOUND_STEPS 40
#define COLLAPSE_VOLUME 127
#define COLLAPSE_SOUND_STEPS 50

#define LAND_POWER 2
#define LAND_FREQ 45
#define LAND_TIME 1

#define FACE_STEPS 120
#define FACE_SLACK 256
#define FACE_TURN 128
#define FACE_REST_STEPS 40

/* Big Foot's own fields of its boss record, a struct boss_record of
 * shared/boss.h. MISSILES: the missiles it holds. CHARGES: the charges left
 * before it lands */
#define MISSILES(record) ((record)->unk_01)
#define CHARGES(record) ((record)->unk_02)

/* fields of Big Foot's mover that hold Big Foot's own state */
/* its turn speed about each axis, in degrees a step */
#define TURN(mover) ((mover)->home)
/* its squared speed across the ground at the last steer */
#define LAST_SPEED2(mover) ((mover)->unk_b4)
/* what bigfoot_jets scales each puff's speed by, where it takes a scale */
#define JET_SCALE(mover) ((mover)->radius)
/* the steps since a take-off's jump, while it raises dust, or 0 */
#define DUST_STEPS(mover) ((mover)->bottom)
/* the missiles held as each volley starts, which nothing reads */
#define VOLLEY_MISSILES(mover) ((mover)->bottom)
/* the speed of the collapse's motion */
#define COLLAPSE_SPEED(mover) ((mover)->bottom)

/* Big Foot's parts, in its boss record's own block */
struct bigfoot_parts {
    struct node_aim *turret_aim;
    struct task *missiles[MISSILE_SLOTS]; /* each riding its pod, or null */
    Uint8 unk_44[0xc];
    struct task *laser; /* the charge's laser spot, or null */
    struct task *capsule;
    struct angles capsule_ang;
    NJS_POINT3 capsule_pos;
    Float distance; /* from player 0 at the last step, or 0 */
};

/* where a shape's center lies: offset from *base, a point in the body's
 * frame. a null base stands for the frame's origin */
struct bigfoot_shape_place {
    const struct collision_shape *shape;
    NJS_POINT3 *base;
    NJS_VECTOR offset;
};

/* the pod a missile rides: bigfoot_pods_0[index], or bigfoot_pods_1[index]
 * where bank isn't 0 */
struct bigfoot_launch {
    Sint8 bank;
    Sint8 index;
};

static void bigfoot_start(struct task *task);
static void bigfoot_destroy(struct task *task);
static Angle bigfoot_steer(struct task *task, Float turn_acceleration);
static void bigfoot_jets(struct task *task, NJS_VECTOR *scale);
static void bigfoot_place_shapes(struct task *task);
static void bigfoot_place_capsule(struct task *task);
static void bigfoot_aim_camera(struct task *task);
static void bigfoot_end_hurt(struct task *task);
static void bigfoot_intro(struct task *task);
static void bigfoot_take_off(struct task *task);
static void bigfoot_charge(struct task *task);
static void bigfoot_come_round(struct task *task);
static void bigfoot_touch_down(struct task *task);
static void bigfoot_fire(struct task *task);
static void bigfoot_face_player(struct task *task);
static void bigfoot_hit_on_ground(struct task *task);
static void bigfoot_down(struct task *task);
static void bigfoot_fall(struct task *task);
static void bigfoot_hit_in_air(struct task *task);
static void bigfoot_step(struct task *task);
static void bigfoot_draw_shadows(struct task *task);
static void bigfoot_find_points(NJS_CNK_OBJECT *object);
static void bigfoot_start_points(struct task *task);
static void bigfoot_draw(struct task *task);

static const struct motion_table_entry bigfoot_motions[BIGFOOT_MOTIONS] = {
#include "boss_bigfoot/bigfoot/bigfoot_motions.inc"
};

/* clang-format off */
static const struct collision_shape bigfoot_shapes[BIGFOOT_SHAPES] = {
    {0, SHAPE_SPHERE, MECH_BOSS_PUSH,
     SHAPE_HURTS_OBJECTS | SHAPE_GUARD_LEVEL(3) | SHAPE_ATTACK_LEVEL(1),
     MECH_BOSS_SHAPE_FLAGS, {0.0F, 2.0F, -2.0F}, 14.0F, 0.0F, 0.0F, 0,
     {0, 0, 0}},
    {0, SHAPE_BOX, MECH_BOSS_PUSH, MECH_BOSS_HURTS | SHAPE_ATTACK_LEVEL(1),
     MECH_BOSS_SHAPE_FLAGS, {0.0F, 15.0F, -6.8F}, 22.0F, 8.0F, 8.0F, 0,
     {0, 0, 0}},
    {0, SHAPE_CYLINDER, MECH_BOSS_PUSH, MECH_BOSS_HURTS | SHAPE_ATTACK_LEVEL(1),
     MECH_BOSS_SHAPE_FLAGS, {0.0F, 0.0F, -12.0F}, 9.0F, 18.0F, 0.0F, 0,
     {0, 0, 0}},
    {0, SHAPE_CYLINDER, MECH_BOSS_PUSH, MECH_BOSS_HURTS | SHAPE_ATTACK_LEVEL(1),
     MECH_BOSS_SHAPE_FLAGS, {0.0F, 0.0F, -10.0F}, 9.0F, 18.0F, 0.0F, 0,
     {0, 0, 0}},
    {0, SHAPE_SPHERE, MECH_BOSS_PUSH, MECH_BOSS_HURTS | SHAPE_ATTACK_LEVEL(1),
     MECH_BOSS_SHAPE_FLAGS, {0.0F, 0.0F, 0.0F}, 9.5F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, MECH_BOSS_PUSH, MECH_BOSS_HURTS | SHAPE_ATTACK_LEVEL(1),
     MECH_BOSS_SHAPE_FLAGS, {0.0F, 0.0F, 0.0F}, 9.5F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, MECH_BOSS_PUSH, MECH_BOSS_HURTS | SHAPE_ATTACK_LEVEL(1),
     MECH_BOSS_SHAPE_FLAGS, {0.0F, -10.0F, -8.0F}, 17.0F, 0.0F, 0.0F, 0,
     {0, 0, 0}},
    {0, SHAPE_CYLINDER, MECH_BOSS_PUSH, MECH_BOSS_HURTS,
     MECH_BOSS_SHAPE_FLAGS | SHAPE_UNK_2000 | SHAPE_OFF | SHAPE_NO_OWN_HITS,
     {0.0F, -40.0F, -8.0F}, 12.0F, 25.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, MECH_BOSS_PUSH,
     SHAPE_HURTS_OBJECTS | SHAPE_GUARD_LEVEL(3) | SHAPE_ATTACK_LEVEL(3),
     MECH_BOSS_SHAPE_FLAGS, {0.0F, -5.0F, -8.0F}, 15.0F, 0.0F, 0.0F, 0,
     {0, 0, 0}},
    {0, SHAPE_CYLINDER2, MECH_BOSS_PUSH,
     MECH_BOSS_HURTS | SHAPE_ATTACK_LEVEL(1), MECH_BOSS_SHAPE_FLAGS,
     {0.0F, 2.0F, -2.0F}, 5.5F, 13.0F, 0.0F, 0,
     {0, QUARTER_TURN, -QUARTER_TURN}},
    {0, SHAPE_CYLINDER2, MECH_BOSS_PUSH,
     MECH_BOSS_HURTS | SHAPE_ATTACK_LEVEL(1), MECH_BOSS_SHAPE_FLAGS,
     {0.0F, 2.0F, -2.0F}, 5.5F, 15.0F, 0.0F, 0,
     {0, QUARTER_TURN, -QUARTER_TURN}},
};
/* clang-format on */

void bigfoot_create(NJS_POINT3 *pos, Angle ang_y)
{
    struct task *task;
    struct task_work *work;
    struct boss_record *record;
    struct bigfoot_parts *parts;

#ifdef NONMATCHING
    /* The landing state counts down both halves of the lean transition. */
    if (bigfoot_motions[MOTION_LEAN].link_frames < 3) {
        return;
    }
#endif
    task =
        task_create(TASK_WORK, BIGFOOT_LEVEL, bigfoot_step, "BossBigFootExec");
#ifdef NONMATCHING
    /* this side creates no boss where task_create fails. the matching side
     * writes through the null */
    if (task == NULL) {
        return;
    }
#endif
    work = task->work;
    record = boss_record_create(task);
#ifdef NONMATCHING
    /* this side kills the task where boss_record_create fails, whose null
     * record->own below writes through */
    if (record == NULL) {
        task_kill(task);
        return;
    }
#endif
    parts = syCalloc(1, sizeof(struct bigfoot_parts));
    if (parts == NULL) {
        /* @bug memory: leaks the record's block where the heap can't give the
         * parts. the task_kill at 0x8c500378 comes before the destructor is
         * set. task_destroy then frees the record and the mover, but not the
         * block that boss_record_create gave it at 0x8c500362 */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it frees the record's block
         * before the kill */
        boss_record_free_parts(task);
#endif
        task_kill(task);
        return;
    }
    record->own = parts;
    motion_init(record->block, bigfoot_motions);
    parts->turret_aim =
        node_aim_create(&bigfoot_turret_node, &bigfoot_turret_pos);
    /* @bug memory: leaves the turret's aim null where the heap can't give it.
     * the test at 0x8c500396 guards only the limits. node_aim_reset reads
     * through the aim at 0x8c53c426, from bigfoot_destroy and bigfoot_charge.
     * node_aim_set reads through it at 0x8c53c3ec, from bigfoot_charge.
     * node_aim_return reads through it at 0x8c53c38e, from bigfoot_down,
     * bigfoot_fall and bigfoot_hit_in_air */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the heap can't give the aim, it
     * frees the record's parts and kills the task, as it does where the parts
     * fail */
    if (parts->turret_aim == NULL) {
        boss_record_free_parts(task);
        task_kill(task);
        return;
    }
#endif
    if (parts->turret_aim != NULL) {
        struct node_aim_limits limits = {
            {0, TURRET_SPEED, TURRET_SPEED},
            {0, MECH_BOSS_LASER_TURN, MECH_BOSS_LASER_UPPER_TILT},
            {0, -MECH_BOSS_LASER_TURN, MECH_BOSS_LASER_LOWER_TILT}};

        node_aim_set_limits(parts->turret_aim, &limits);
    }
    /* @hack spelling: spec declared in a block of its own, where a local of
     * the function would do */
    {
        struct mech_boss_capsule_spec spec;

        spec.pos = &parts->capsule_pos;
        spec.ang = &parts->capsule_ang;
        spec.boss = MECH_BOSS_BIG_FOOT;
        spec.range = BIGFOOT_ARENA_RADIUS;
        parts->capsule = mech_boss_capsule_create(task, &spec);
    }
    work->pos = *pos;
    work->ang.y = ang_y;
    mover_set_gravity(task, GRAVITY);
    collision_init(task, bigfoot_shapes, BIGFOOT_SHAPES, COLLISION_ENEMY);
    /* @hack spelling: task->work read again, where work would do */
    task->work->collision->flags &= ~COLLISION_TARGET;
    bigfoot_start(task);
    pad_disable_player(0);
    task->draw = bigfoot_draw;
    task->destructor = bigfoot_destroy;
}

/* put Big Foot in MODE_INTRO, standing, with FULL_POWER and no missiles */
static void bigfoot_start(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct task_work *work = task->work;
    struct bigfoot_parts *parts = record->own;
    /* @hack spelling: control read before the rest, where record->block read
     * at its call would do */
    struct motion_control *control = record->block;
    /* @hack type: i a Sint8, where an int would do */
    Sint8 i;

    work->mode = MODE_INTRO;
    work->timer = 0;
    work->tick = 0;
    record->power = FULL_POWER;
    parts->capsule_pos = work->pos;
    bigfoot_body_pos = work->pos;
    work->id = WORK_ID_ATTACK;
    for (i = 0; i < MISSILE_SLOTS; i++) {
        parts->missiles[i] = NULL;
    }
    parts->laser = NULL;
    motion_set(control, MOTION_STAND);
    work->flags |= BIGFOOT_LANDED;
}

static void bigfoot_destroy(struct task *task)
{
    /* @hack redundant: work, which nothing reads */
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct bigfoot_parts *parts = record->own;

    node_aim_reset(parts->turret_aim);
    node_aim_free(parts->turret_aim);
    voices_stop();
    boss_record_free_parts(task);
}

/* turn Big Foot toward its mover's target, and sway it. turn_acceleration is
 * the most its turn speed gains in a step, in degrees a step. the body rolls
 * into the turn, and pitches as its speed changes. it returns twice the turn
 * left, which no caller reads */
static Angle bigfoot_steer(struct task *task, Float turn_acceleration)
{
    struct task_work *work = task->work;
    struct mover *mover = task->motion.p;
    Float turn;
    Float target_roll;
    Float target_pitch;
    Float speed_squared;
    Float speed_change;
    Float velocity_x;
    Float velocity_z;
    Angle angle;

    mover->sway_x += SWAY_X_STEP;
    mover->sway_y += SWAY_Y_STEP;
    mover->sway_z += SWAY_Z_STEP;
    angle = mover_target_heading(task);
    turn = -angle_difference(angle, work->ang.y);
    turn *= 2.0F;
    angle = turn;
    if (turn > 0.0F) {
        if (turn < QUARTER_TURN) {
            turn = njSin(angle);
        } else {
            turn = 1.0F;
        }
    } else {
        if (turn > -QUARTER_TURN) {
            turn = njSin(angle);
        } else {
            turn = -1.0F;
        }
    }
    TURN(mover).y +=
        (turn_acceleration * turn) - (TURN(mover).y * MECH_BOSS_TURN_DRAG);
    work->ang.y += (Angle)(TURN(mover).y * ANGLE_PER_DEGREE);
    target_roll = (-TURN(mover).y * MECH_BOSS_ROLL_PER_TURN) -
                  (work->ang.z * DEGREES_PER_ANGLE);
    TURN(mover).z += (target_roll * ROLL_GAIN) - (TURN(mover).z * ROLL_DRAG);
    work->ang.z += (Angle)(TURN(mover).z * ANGLE_PER_DEGREE);
    velocity_x = mover->spd.x;
    velocity_z = mover->spd.z;
    speed_squared = (velocity_x * velocity_x) + (velocity_z * velocity_z);
    speed_change = speed_squared - LAST_SPEED2(mover);
    LAST_SPEED2(mover) = speed_squared;
    target_pitch = (speed_change * MECH_BOSS_PITCH_PER_SPEED2) -
                   (work->ang.x * DEGREES_PER_ANGLE);
    TURN(mover).x += (target_pitch * PITCH_GAIN) - (TURN(mover).x * PITCH_DRAG);
    work->ang.x += (Angle)(TURN(mover).x * ANGLE_PER_DEGREE);
    return angle;
}

static struct bigfoot_shape_place bigfoot_shape_places[BIGFOOT_SHAPES] = {
    {&bigfoot_shapes[0], &bigfoot_body_local, {0.0F, 0.0F, -3.5F}},
    {&bigfoot_shapes[1], &bigfoot_body_local, {0.0F, 17.0F, -6.8F}},
    {&bigfoot_shapes[2], &bigfoot_foot_a_local, {-2.0F, 9.0F, -10.0F}},
    {&bigfoot_shapes[3], &bigfoot_foot_b_local, {2.0F, 9.0F, -10.0F}},
    {&bigfoot_shapes[4], &bigfoot_foot_a_local, {-3.0F, -9.0F, 0.0F}},
    {&bigfoot_shapes[5], &bigfoot_foot_b_local, {3.0F, -9.0F, 0.0F}},
    {&bigfoot_shapes[6], &bigfoot_body_local, {0.0F, -15.0F, -8.0F}},
    {&bigfoot_shapes[7], &bigfoot_body_local, {0.0F, -40.0F, -8.0F}},
    {&bigfoot_shapes[8], &bigfoot_body_local, {0.0F, -5.0F, -8.0F}},
    {&bigfoot_shapes[9], &bigfoot_body_local, {-12.0F, 2.0F, -2.0F}},
    {&bigfoot_shapes[10], &bigfoot_body_local, {12.0F, 2.0F, -2.0F}}};

/* a puff from each jet, along its nozzle. scale scales each puff's speed, or
 * null leaves it as it is */
static void bigfoot_jets(struct task *task, NJS_VECTOR *scale)
{
    struct task_work *work = task->work;
    /* each jet's nozzle points along the body's up, turned about x by its
     * pitch */
    Angle pitches[JETS] = {(Angle)(-124 * ANGLE_PER_DEGREE),
                           (Angle)(180 * ANGLE_PER_DEGREE),
                           (Angle)(-124 * ANGLE_PER_DEGREE)};
    NJS_VECTOR dir;
    /* @hack redundant: unused, 128 bytes that nothing reads, which size
     * the frame */
    char unused[128];
    /* @hack type: i a Sint8, where an int would do */
    Sint8 i;
    Angle *pitch;

    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    /* @hack spelling: the pitches walked by a pointer, where pitches[i]
     * would do */
    for (i = 0, pitch = pitches; i < JETS; i++) {
        NJS_VECTOR up = {0.0F, 1.0F, 0.0F};
        njPushMatrixEx();
        njRotateX(NULL, *pitch);
        njCalcVector(NULL, &up, &dir);
        njPopMatrixEx();
        if (scale) {
            dir.x *= scale->x;
            dir.y *= scale->y;
            dir.z *= scale->z;
        }
        smoke_puff_create(task, SMOKE_PUFF_SMALL, &bigfoot_jet_points[i], &dir);
        pitch++;
    }
    njPopMatrix(1);
}

/* center each shape at its place, and register the collision. while Big
 * Foot flies, the registration takes its angles and height swayed */
static void bigfoot_place_shapes(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    /* @hack redundant: parts, which nothing reads */
    struct bigfoot_parts *parts = record->own;
    struct bigfoot_shape_place *place;
    /* @hack type: i a Sint8, where an int would do */
    Sint8 i;

    /* @hack spelling: i = i + 1, where ++i would do */
    for (place = bigfoot_shape_places, i = 0; i < BIGFOOT_SHAPES;
         ++place, i = i + 1) {
        /* @hack redundant: the loop's mover, which nothing reads */
        struct mover *mover = task->motion.p;
        NJS_POINT3 pos = {0.0F, 0.0F, 0.0F};
        if (work->mode == MODE_FALL && bigfoot_body_pos.y <= FALLEN_HEIGHT) {
            NJS_POINT3 offset = {0.0F, 0.0F, FALLEN_SHIFT};
            if (place->base) {
                njAddVector(&pos, place->base);
            }
            njAddVector(&pos, &place->offset);
            njAddVector(&pos, &offset);
        } else {
            if (place->base) {
                njAddVector(&pos, place->base);
            }
            njAddVector(&pos, &place->offset);
        }
        collision_set_shape_center(task, i, &pos);
    }
    {
#ifdef NONMATCHING
        /* the save and the restore both test BIGFOOT_LANDED, which
         * collision_register leaves as it was. so the restore runs only where
         * the save ran. GCC can't see that, so here the saves start set, which
         * changes no result */
        struct angles saved = work->ang;
        Float saved_y = work->pos.y;
#else
        struct angles saved;
        Float saved_y;
#endif
        if (!(work->flags & BIGFOOT_LANDED)) {
            struct mover *mover = task->motion.p;
            saved_y = work->pos.y;
            saved = work->ang;
            work->ang.x +=
                (Angle)(njSin(mover->sway_x) * MECH_BOSS_SWAY_X_ANGLE);
            work->ang.z +=
                (Angle)(njSin(mover->sway_z) * MECH_BOSS_SWAY_Z_ANGLE);
            work->pos.y += njSin(mover->sway_y) * MECH_BOSS_BOB_HEIGHT;
        }
        collision_register(task);
        if (!(work->flags & BIGFOOT_LANDED)) {
            work->pos.y = saved_y;
            work->ang.x = saved.x;
            work->ang.z = saved.z;
        }
    }
}

/* hold the capsule CAPSULE_HEIGHT above the body's point. it takes the
 * work's angles, swayed as bigfoot_place_shapes sways them, landed or not */
static void bigfoot_place_capsule(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct bigfoot_parts *parts = record->own;
    struct mover *mover = task->motion.p;

    parts->capsule_ang = work->ang;
    parts->capsule_ang.x +=
        (Angle)(njSin(mover->sway_x) * MECH_BOSS_SWAY_X_ANGLE);
    parts->capsule_ang.z +=
        (Angle)(njSin(mover->sway_z) * MECH_BOSS_SWAY_Z_ANGLE);
    parts->capsule_pos = bigfoot_body_pos;
    parts->capsule_pos.y += CAPSULE_HEIGHT;
}

/* put the cameras' focus at the work's pos, raised by the body point's y */
static void bigfoot_aim_camera(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    NJS_VECTOR offset;
    offset.x = 0.0F;
    offset.y = bigfoot_body_pos.y;
    offset.z = 0.0F;
    boss_record_set_focus(record, &work->pos, &offset);
}

/* end the hurt state. each shape and the capsule take back the levels they
 * start with */
static void bigfoot_end_hurt(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct bigfoot_parts *parts = record->own;
    struct mech_boss_levels levels[BIGFOOT_SHAPES];
    struct mech_boss_levels *entry = levels;
    const struct collision_shape *shape = bigfoot_shapes;
    /* @hack type: i a Sint8, where an int would do */
    Sint8 i;

    /* @hack spelling: the levels and the shapes walked by pointers, where an
     * index would do */
    for (i = 0; i < BIGFOOT_SHAPES; ++i) {
        entry->attack = shape->levels & SHAPE_ATTACK;
        entry->guard = (shape->levels & SHAPE_GUARD) >> SHAPE_GUARD_SHIFT;
        entry++;
        shape++;
    }
    mech_boss_set_levels(task, levels);
    mech_boss_capsule_reset(parts->capsule);
    work->flags &= ~WORK_HURT;
}

/* the opening. the camera turns about Big Foot as its title shows, and play
 * starts MECH_BOSS_INTRO_STEPS later with a take-off */
static void bigfoot_intro(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct motion_control *control = record->block;
    /* @hack spelling: offset declared in a block of its own, where a local of
     * the function would do */
    {
        NJS_VECTOR offset;
        offset.x = 0.0F;
        offset.y = INTRO_FOCUS_HEIGHT;
        offset.z = 0.0F;
        boss_record_set_focus(record, &work->pos, &offset);
    }
    switch (work->submode) {
    case 0: {
        /* @hack spelling: camera declared in a block of its own, where a
         * local of case 0's block would do */
        {
            struct camera_bossinit camera;
            camera.speed = 0.6666F;
            camera.ang = work->ang.y + 0xa000;
            camera.target = &record->focus;
            camera.start_dist = 100.0F;
            camera.dist = 60.0F;
            camera.turn = 0x471c;
            camera_bossinit_start(0, &camera);
        }
        pad_disable_player(0);
        player_work[0]->pos.x = -work->pos.x;
        player_work[0]->pos.y = 0.0F;
        player_work[0]->pos.z = 0.0F;
        motion_link(control, MOTION_STAND);
        work->flags |= BIGFOOT_LANDED;
        record->gauge = boss_gauge_create(NULL, &record->power);
        {
            NJS_POINT3 position = {370.0F, 280.0F, -1.5F};
            record->title =
                mech_boss_title_create(task, MECH_BOSS_BIG_FOOT, &position);
        }
        work->timer = MECH_BOSS_INTRO_STEPS;
        mech_boss_voice_create(task, MECH_BOSS_BIG_FOOT, &record->power);
        ++work->submode;
    } break;
    case 1:
        if (--work->timer == 0) {
            struct camera_bosspoint camera;
            camera_bossinit_end(0);
            camera.target = &record->focus;
            camera.dist = MECH_BOSS_CAMERA_DIST;
            camera.height = MECH_BOSS_CAMERA_HEIGHT;
            camera.part = MECH_BOSS_CAMERA_PART;
            camera.adjust = CAMERA_ADJUST_RELATIVE3;
            camera.collides = 1;
            camera.trigger_push = 0;
            camera.lift = MECH_BOSS_CAMERA_LIFT;
            camera.level = 0;
            camera_bosspoint_start(0, &camera);
            pad_enable_player(0);
            work->flags |= BIGFOOT_STARTING;
            task_kill(record->title);
            record->title = NULL;
            boss_messenger_summon_capped(0, MECH_BOSS_MESSAGES, task);
            work->mode = MODE_TAKE_OFF;
            work->submode = 0;
            work->tick = 0;
            work->timer = 0;
        }
        break;
    }
}

/* rise on the jets and settle at HOVER_HEIGHT. then set out on three
 * charges, the first without the laser */
static void bigfoot_take_off(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct mover *mover = task->motion.p;
    struct motion_control *control = record->block;
    Float low;

    switch (work->submode) {
    case 0:
        mover->target.y = 40.0F;
        mover->target.z = 0.0F;
        mover->sway_x = 0;
        mover->sway_y = 0;
        mover->sway_z = 0;
        JET_SCALE(mover) = 0.0F;
        DUST_STEPS(mover) = 0.0F;
        collision_shape_on(task, UNDER_SHAPE);
        rising_sound_start(task);
        ++work->submode;
        motion_link(control, MOTION_CROUCH);
        break;
    case 1: {
        NJS_VECTOR scale;
        JET_SCALE(mover) += 0.05F;
        if (JET_SCALE(mover) > 1.5F) {
            JET_SCALE(mover) = 1.5F;
        }
        scale.x = scale.y = scale.z = JET_SCALE(mover);
        bigfoot_jets(task, &scale);
    }
        if (motion_held(control)) {
            mover->acc.y = 0.8F;
            rising_sound_fade();
            motion_link(control, MOTION_JUMP);
            {
                struct mech_boss_ring spec = {
                    10, 5.0F, 8.0F, {0.2F, 0.05F, 0.2F}};
                mech_boss_ring(&work->pos, &spec);
            }
            if (work->flags & WORK_HURT) {
                bigfoot_end_hurt(task);
            }
            ++DUST_STEPS(mover);
            ++work->submode;
        }
        break;
    case 2:
        mover->acc.y *= 0.95F;
        mover->acc.y += -0.002F;
        mover->spd.y *= 0.9F;
        bigfoot_jets(task, NULL);
        if (control->index == MOTION_JUMP && motion_held(control)) {
            work->flags &= ~BIGFOOT_LANDED;
            motion_link(control, MOTION_LIFT);
        }
        if (mover->spd.y <= 0.0F) {
            ++work->submode;
        }
        break;
    case 3: {
        Float height_error;
        mover->sway_x += SWAY_X_STEP;
        mover->sway_y += SWAY_Y_STEP;
        if (control->index == MOTION_JUMP && motion_held(control)) {
            work->flags &= ~BIGFOOT_LANDED;
            motion_link(control, MOTION_LIFT);
        }
        mover_turn_to_player_by(task, 64, 0);
        bigfoot_jets(task, NULL);
        height_error = work->pos.y - HOVER_HEIGHT;
        mover->spd.y *= 0.8F;
        if (height_error > HOVER_ERROR_LIMIT) {
            height_error = HOVER_ERROR_LIMIT;
        } else if (height_error < -HOVER_ERROR_LIMIT) {
            height_error = -HOVER_ERROR_LIMIT;
        }
        mover->acc.y -= height_error * HOVER_PULL;
        if (height_error > -HOVER_TOLERANCE && height_error < HOVER_TOLERANCE) {
            Float vertical_speed = mover->spd.y;
            if (vertical_speed > -HOVER_TOLERANCE &&
                vertical_speed < HOVER_TOLERANCE) {
                work->pos.y = HOVER_HEIGHT;
                mover->spd.y = 0.0F;
                mover->acc.y = 0.0F;
                work->timer = 30;
                ++work->submode;
            }
        }
    } break;
    case 4:
        mover->sway_x += SWAY_X_STEP;
        mover->sway_y += SWAY_Y_STEP;
        mover->sway_z += SWAY_Z_STEP;
        if (work->pos.x > 0.0F) {
            mover->spd.x = -0.01F;
        } else {
            mover->spd.x = 0.01F;
        }
        if (motion_held(control)) {
            /* @hack spelling: CHARGES set to 2 and then counted up, where a
             * store of 3 would do */
            CHARGES(record) = 2;
            CHARGES(record) = CHARGES(record) + 1;
            work->mode = MODE_CHARGE;
            work->submode = 0;
            work->tick = 0;
            work->timer = 0;
            motion_link(control, MOTION_FLY);
            work->flags &= ~BIGFOOT_LASER;
        }
        bigfoot_jets(task, NULL);
        break;
    }
    if (!(work->flags & BIGFOOT_LANDED)) {
        mover->acc.y += LIFT;
    }
    /* @hack spelling: low, a local for 5, which the height test takes as
     * GROUND_HEIGHT and the second ring as its radius, where each constant in
     * place would do */
    low = GROUND_HEIGHT;
    if (work->flags & BIGFOOT_STARTING) {
        if (work->pos.y > low) {
            work->flags &= ~BIGFOOT_STARTING;
        }
    }
    if (DUST_STEPS(mover) > 0.0F) {
        if ((Sint16)DUST_STEPS(mover) % DUST_RING_STEPS == 0) {
            struct mech_boss_ring spec = {10, 5.0F, 8.0F, {0.2F, 0.05F, 0.1F}};
            mech_boss_ring(&work->pos, &spec);
            spec.radius = low;
            spec.count = 4;
            mech_boss_ring(&work->pos, &spec);
        }
        ++DUST_STEPS(mover);
        if (DUST_STEPS(mover) == DUST_END_STEPS) {
            DUST_STEPS(mover) = 0.0F;
        }
    }
    bigfoot_aim_camera(task);
}

/* charge across the arena past player 0, out past BIGFOOT_ARENA_RADIUS along
 * x. under BIGFOOT_LASER, the laser aims at the player until Big Foot passes
 * it */
static void bigfoot_charge(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct mover *mover = task->motion.p;
    struct bigfoot_parts *parts = record->own;

    switch (work->submode) {
    case 0:
        if (work->flags & BIGFOOT_LASER) {
            {
                struct mech_boss_laser_spec spec;
                struct angles upper = {0, MECH_BOSS_LASER_TURN,
                                       MECH_BOSS_LASER_UPPER_TILT};
                struct angles lower = {0, -MECH_BOSS_LASER_TURN,
                                       MECH_BOSS_LASER_LOWER_TILT};
                spec.reach = MECH_BOSS_LASER_REACH;
                spec.upper = upper;
                spec.lower = lower;
                spec.step = MECH_BOSS_LASER_STEP;
                spec.origin = &bigfoot_laser_pos;
                parts->laser = mech_boss_laser_create(task, &spec);
            }
#ifdef NONMATCHING
            /* this side repairs the tag in the matching side: where the heap
             * can't give the laser, the charge goes on without it, as one
             * without BIGFOOT_LASER does */
            if (parts->laser == NULL) {
                work->flags &= ~BIGFOOT_LASER;
            } else {
                struct mech_boss_laser_beam_spec spec;

                spec.unk_00 = 2;
                spec.origin = &bigfoot_laser_pos;
                spec.target = &parts->laser->work->pos;
                spec.mover = mover;
                mech_boss_laser_beam_create(parts->laser, &spec);
                mech_boss_laser_set_mode(parts->laser, MECH_BOSS_LASER_AIM);
            }
#else
            {
                struct mech_boss_laser_beam_spec spec;
                spec.unk_00 = 2;
                spec.origin = &bigfoot_laser_pos;
                /* @bug memory: reads through parts->laser at 0x8c50145c,
                 * null where the heap couldn't give the laser. the tests of
                 * it under BIGFOOT_LASER, at 0x8c501544, 0x8c5015f8 and
                 * 0x8c50162a, come after that read */
                spec.target = &parts->laser->work->pos;
                spec.mover = mover;
                mech_boss_laser_beam_create(parts->laser, &spec);
            }
            mech_boss_laser_set_mode(parts->laser, MECH_BOSS_LASER_AIM);
#endif
        }
        work->collision->shapes[LOW_SHAPE].flags |= SHAPE_OFF;
        sound_play_at_for(MECH_BOSS_SOUND_ENGINE, MECH_BOSS_SECOND_KEY(task),
                          MECH_BOSS_SOUND_PRIORITY, CHARGE_VOLUME,
                          CHARGE_SOUND_STEPS, &work->pos);
        DUST_STEPS(mover) = 0.0F;
        ++work->submode;
    case 1:
        mover->target.x = player_work[0]->pos.x;
        if (player_work[0]->pos.z > CHARGE_LIMIT_Z) {
            mover->target.z = CHARGE_LIMIT_Z;
        } else if (player_work[0]->pos.z < -CHARGE_LIMIT_Z) {
            mover->target.z = -CHARGE_LIMIT_Z;
        } else {
            mover->target.z = player_work[0]->pos.z;
        }
        if (njAbs(player_work[0]->pos.x - work->pos.x) < PASS_X) {
            ++work->submode;
        }
        break;
    case 2:
        if (mover->spd.x > 0.0F) {
            mover->target.x = BIGFOOT_ARENA_RADIUS;
            if (work->flags & BIGFOOT_LASER) {
                if (player_work[0]->pos.x - work->pos.x < 0.0F) {
                    mech_boss_laser_set_mode(parts->laser,
                                             MECH_BOSS_LASER_REST);
                }
            }
            if (work->pos.x > BIGFOOT_ARENA_RADIUS) {
                if ((work->flags & BIGFOOT_LASER) && parts->laser) {
                    task_kill(parts->laser);
                    parts->laser = NULL;
                }
                node_aim_reset(parts->turret_aim);
                work->mode = MODE_COME_ROUND;
                work->submode = 0;
                --CHARGES(record);
            }
        } else {
            mover->target.x = -BIGFOOT_ARENA_RADIUS;
            if (work->flags & BIGFOOT_LASER) {
                if (player_work[0]->pos.x - work->pos.x > 0.0F) {
                    mech_boss_laser_set_mode(parts->laser,
                                             MECH_BOSS_LASER_REST);
                }
            }
            if (work->pos.x < -BIGFOOT_ARENA_RADIUS) {
                if ((work->flags & BIGFOOT_LASER) && parts->laser) {
                    task_kill(parts->laser);
                    parts->laser = NULL;
                }
                node_aim_reset(parts->turret_aim);
                work->mode = MODE_COME_ROUND;
                work->submode = 0;
                --CHARGES(record);
            }
        }
        break;
    }
    if (work->flags & BIGFOOT_LASER) {
        if (parts->laser) {
            struct angles angle;
            angle = *mech_boss_laser_angles(parts->laser);
            angle.z += MECH_BOSS_TURRET_TILT;
            node_aim_set(parts->turret_aim, &angle);
        }
        mover->acc.x += njSin(work->ang.y) * 0.1F;
        mover->acc.z += njCos(work->ang.y) * 0.1F;
        mover->spd.x *= 0.94F;
        mover->spd.z *= 0.94F;
        bigfoot_steer(task, 0.07F);
    } else {
        mover->acc.x += njSin(work->ang.y) * 0.1F;
        mover->acc.z += njCos(work->ang.y) * 0.1F;
        mover->spd.x *= 0.96F;
        mover->spd.z *= 0.96F;
        bigfoot_steer(task, 0.081F);
    }
    mover->acc.y += LIFT;
    bigfoot_aim_camera(task);
    bigfoot_jets(task, NULL);
    {
        NJS_VECTOR delta;
        vector_diff(&player_work[0]->pos, &work->pos, &delta);
        delta.y = 0.0F;
        if (njScalor(&delta) < MECH_BOSS_CAMERA_NEAR) {
            camera_bosspoint_set_level(0, 1);
        } else {
            camera_bosspoint_set_level(0, 0);
        }
        if (!(player_work[0]->flags & PLAYER_WORK_ON_GROUND)) {
            camera_bosspoint_set_level(0, 0);
        }
    }
}

/* turn back toward player 0. once within RETURN_X of x = 0, charge again
 * with the laser. where no charge is left, land instead */
static void bigfoot_come_round(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    /* @hack redundant: parts, which nothing reads */
    struct bigfoot_parts *parts = record->own;
    struct mover *mover = task->motion.p;

    switch (work->submode) {
    case 0: {
        /* @hack spelling: scale, a local for 0.9, where 0.9F would do */
        Float scale;
        mover->target.x = player_work[0]->pos.x;
        scale = 0.9F;
        if (work->pos.z > RETURN_Z && player_work[0]->pos.z > work->pos.z) {
            mover->target.z = work->pos.z * scale;
        } else if (work->pos.z < -RETURN_Z &&
                   player_work[0]->pos.z < work->pos.z) {
            mover->target.z = work->pos.z * scale;
        } else {
            mover->target.z = player_work[0]->pos.z;
        }
        ++work->submode;
    }
    case 1:
        if (work->pos.x > 0.0F) {
            if (mover->spd.x < 0.0F) {
                ++work->submode;
            }
        } else {
            if (mover->spd.x > 0.0F) {
                ++work->submode;
            }
        }
        break;
    case 2:
        if (njAbs(work->pos.x) < RETURN_X) {
            /* @hack type: next_submode, a Sint8 local for 0, where 0 stored
             * in place would do */
            Sint8 next_submode = 0;
            /* @hack spelling: the laser's flag set inside a select, where an
             * if and an else would do */
            work->mode = !CHARGES(record)
                             ? MODE_LAND
                             : (work->flags |= BIGFOOT_LASER, MODE_CHARGE);
            work->submode = next_submode;
        }
        break;
    }
    mover->acc.x += njSin(work->ang.y) * 0.09F;
    mover->acc.z += njCos(work->ang.y) * 0.09F;
    mover->spd.x *= 0.92F;
    mover->spd.z *= 0.92F;
    bigfoot_steer(task, 0.081F);
    mover->acc.y += LIFT;
    bigfoot_aim_camera(task);
    bigfoot_jets(task, NULL);
}

static struct bigfoot_launch bigfoot_launches[MISSILE_SLOTS] = {
    {0, 0}, {0, 2}, {1, 0}, {1, 2}, {0, 1}, {0, 3}, {1, 1}, {1, 3},
    {0, 0}, {0, 2}, {1, 0}, {1, 2}, {0, 1}, {0, 3}, {1, 1}, {1, 3}};

/* fly toward the middle, lean, and land. the landing loads no fewer missiles
 * the less power is left */
static void bigfoot_touch_down(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct mover *mover = task->motion.p;
    struct motion_control *control = record->block;
    struct bigfoot_parts *parts = record->own;

    switch (work->submode) {
    case 0:
        mover->target.x = 0.0F;
        mover->target.z = 0.0F;
        work->flags &= ~BIGFOOT_PITCHED;
        ++work->submode;
    case 1:
        mover->acc.x += njSin(work->ang.y) * 0.1F;
        mover->acc.z += njCos(work->ang.y) * 0.1F;
        mover->spd.x *= 0.93F;
        mover->spd.z *= 0.93F;
        bigfoot_steer(task, 0.07F);
        mover->acc.y += LIFT;
        bigfoot_jets(task, NULL);
        {
            Float x = work->pos.x;
            Float z = work->pos.z;
            if (njSqrt((x * x) + (z * z)) < LOW_SHAPE_RADIUS) {
                work->collision->shapes[LOW_SHAPE].flags &= ~SHAPE_OFF;
                ++work->submode;
            }
        }
        break;
    case 2:
        mover->acc.x += njSin(work->ang.y) * 0.07F;
        mover->acc.z += njCos(work->ang.y) * 0.07F;
        mover->spd.x *= 0.96F;
        mover->spd.z *= 0.96F;
        mover->sway_x += SWAY_X_STEP;
        mover->sway_y += SWAY_Y_STEP;
        mover->sway_z += SWAY_Z_STEP;
        bigfoot_steer(task, 0.07F);
        mover->acc.y += LIFT;
        bigfoot_jets(task, NULL);
        {
            Float x = work->pos.x;
            Float z = work->pos.z;
            if (njSqrt((x * x) + (z * z)) < BRAKE_RADIUS) {
                mover->acc.x = 0.0F;
                mover->acc.z = 0.0F;
                mover->aim_y = mover_player_heading(task, 0);
                mover->aim_x = work->ang.x - LEAN_PITCH;
                ++work->submode;
            }
        }
        break;
    case 3:
        mover->sway_x += SWAY_X_STEP;
        mover->sway_y += SWAY_Y_STEP;
        mover->sway_z += SWAY_Z_STEP;
        /* @hack redundant: a store of 0.005 to spd.x that no path reaches */
        if (0) {
            mover->spd.x = 0.005F;
        }
        if (njAbs(mover->spd.x) < 0.005F) {
            mover->spd.x = 0.0F;
        } else {
            mover->spd.x *= 0.5F;
        }
        if (njAbs(mover->spd.z) < 0.005F) {
            mover->spd.z = 0.0F;
        } else {
            mover->spd.z *= 0.5F;
        }
        if (!(work->flags & BIGFOOT_PITCHED)) {
            work->ang.x = angle_approach(work->ang.x, mover->aim_x, LEAN_SPEED);
            if (!angle_difference(work->ang.x, mover->aim_x)) {
                mover->aim_x = 0;
                work->flags |= BIGFOOT_PITCHED;
            }
        } else {
            work->ang.x = angle_approach(work->ang.x, mover->aim_x, LEAN_SPEED);
            if (!angle_difference(work->ang.x, mover->aim_x)) {
                motion_link(control, MOTION_LEAN);
                work->timer = bigfoot_motions[MOTION_LEAN].link_frames;
                ++work->submode;
            }
        }
        mover->acc.y += LIFT;
        bigfoot_jets(task, NULL);
        break;
    case 4:
        mover->sway_x += SWAY_X_STEP;
        mover->sway_y += SWAY_Y_STEP;
        mover->sway_z += SWAY_Z_STEP;
        work->ang.y = angle_approach(work->ang.y, mover->aim_y, 128);
        work->ang.x = angle_approach(work->ang.x, 0, 128);
        work->ang.z = angle_approach(work->ang.z, 0, 128);
        mover->acc.y += LIFT;
        bigfoot_jets(task, NULL);
        if (--work->timer < bigfoot_motions[MOTION_LEAN].link_frames * 0.5F) {
            ++work->submode;
        }
        break;
    case 5:
        mover->acc.y -= 0.0015F;
        mover->sway_x = angle_approach(mover->sway_x, 0, 128);
        mover->sway_y = angle_approach(mover->sway_y, 0, 128);
        mover->sway_z = angle_approach(mover->sway_z, 0, 128);
        work->ang.x = angle_approach(work->ang.x, 0, 256);
        work->ang.z = angle_approach(work->ang.z, 0, 256);
        work->ang.y = angle_approach(work->ang.y, mover->aim_y, 256);
        mover->acc.y += LIFT;
        bigfoot_jets(task, NULL);
        if (--work->timer == 0) {
            ++work->submode;
        }
        break;
    case 6:
        mover->acc.y += LIFT;
        work->ang.x = angle_approach(work->ang.x, 0, 256);
        work->ang.z = angle_approach(work->ang.z, 0, 256);
        work->ang.y = angle_approach(work->ang.y, mover->aim_y, 256);
        {
            NJS_VECTOR scale;
            JET_SCALE(mover) = 0.8F;
            scale.x = scale.y = scale.z = JET_SCALE(mover);
            bigfoot_jets(task, &scale);
        }
        if (mover->spd.y < -0.1F) {
            mover->acc.y += 0.005F;
        } else {
            mover->spd.y = 0.0F;
            if (!angle_distance(work->ang.y, mover->aim_y) &&
                work->ang.x == 0 && work->ang.z == 0) {
                /* @hack type: i a Sint8, where an int would do */
                Sint8 i;
                struct bigfoot_launch *launch;
                sound_play_at(MECH_BOSS_SOUND_HATCH, NULL, 0,
                              MECH_BOSS_HATCH_VOLUME, &work->pos);
                motion_link(control, MOTION_STOMP);
                /* @hack type: the power cast through a Sint8, where an int
                 * would do */
                switch ((Sint8)record->power) {
                case 4:
                    MISSILES(record) = 8;
                    break;
                case 3:
                    MISSILES(record) = 12;
                    break;
                case 2:
                    MISSILES(record) = 12;
                    break;
                case 1:
                    MISSILES(record) = 16;
                    break;
                }
                /* @hack spelling: a do-while under a test of the count, where
                 * a for would do */
                i = 0;
                launch = bigfoot_launches;
                if (MISSILES(record) > 0) {
                    do {
                        NJS_POINT3 *position;
                        if (!launch->bank) {
                            position = &bigfoot_pods_0[launch->index];
                        } else {
                            position = &bigfoot_pods_1[launch->index];
                        }
                        parts->missiles[i] = mech_boss_missile_create(
                            task, position, &work->ang, MISSILE_RADIUS);
                        ++launch;
                        ++i;
                    } while (i < MISSILES(record));
                }
                work->flags |= BIGFOOT_MISSILES;
                collision_shape_off(task, UNDER_SHAPE);
                ++work->submode;
            }
        }
        break;
    case 7: {
        NJS_VECTOR scale;
        JET_SCALE(mover) -= 0.05F;
        if (JET_SCALE(mover) < 0.0F) {
            JET_SCALE(mover) = 0.0F;
        }
        scale.x = scale.y = scale.z = JET_SCALE(mover);
        bigfoot_jets(task, &scale);
    }
        if (work->pos.y <= 0.0F) {
            work->ang.x = 0;
            work->ang.z = 0;
            TURN(mover).x = 0.0F;
            TURN(mover).y = 0.0F;
            TURN(mover).z = 0.0F;
            work->flags |= BIGFOOT_LANDED;
            mech_boss_camera_shake(&work->pos, MECH_BOSS_SHAKE_HEAVY);
            work->pos.y = 0.0F;
            sound_play_at(MECH_BOSS_SOUND_LAND, NULL, 0, LAND_VOLUME,
                          &work->pos);
            sound_pan_by_pos(MECH_BOSS_SOUND_LAND, NULL, NULL);
            {
                struct mech_boss_ring spec = {
                    10, 10.0F, 12.0F, {0.1F, 0.02F, 0.1F}};
                mech_boss_ring(&work->pos, &spec);
            }
            vibration_start(0, LAND_POWER, LAND_FREQ, LAND_TIME);
            ++work->submode;
        }
        break;
    case 8:
        if (motion_held(control)) {
            work->mode = MODE_FIRE;
            work->submode = 0;
            work->tick = 0;
            work->timer = 0;
        }
        break;
    }
    bigfoot_aim_camera(task);
}

/* launch the missiles at player 0 in volleys, then close the hatch and face
 * the player */
static void bigfoot_fire(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct mover *mover = task->motion.p;
    struct motion_control *control = record->block;
    struct bigfoot_parts *parts = record->own;

    switch (work->submode) {
    case 0:
        ++work->submode;
    case 1:
        work->timer = 1;
        VOLLEY_MISSILES(mover) = MISSILES(record);
        ++work->submode;
        break;
    case 2:
        if (--work->timer == 0) {
            Angle yaw =
                MECH_BOSS_MISSILE_YAW_SPREAD -
                (rand() * (2.0F * MECH_BOSS_MISSILE_YAW_SPREAD * RAND_SCALE));
            Angle turn =
                angle_difference(mover_player_heading(task, 0), work->ang.y);
            --MISSILES(record);
            yaw += turn;
            mech_boss_missile_launch(parts->missiles[MISSILES(record)], yaw,
                                     MISSILE_SPEED, player_work[0]->pos);
            parts->missiles[MISSILES(record)] = NULL;
            if (MISSILES(record) % MECH_BOSS_VOLLEY == 0) {
                work->timer = VOLLEY_REST_STEPS;
                work->submode = 3;
            } else {
                work->timer = SHOT_STEPS;
            }
        }
        break;
    case 3:
        if (--work->timer == 0) {
            if (MISSILES(record) == 0) {
                sound_play_at(MECH_BOSS_SOUND_HATCH, NULL, 0,
                              MECH_BOSS_HATCH_VOLUME, &work->pos);
                motion_link(control, MOTION_RECOVER);
                work->flags &= ~BIGFOOT_MISSILES;
                work->timer = CLOSE_STEPS;
                ++work->submode;
            } else {
                work->submode = 1;
            }
        }
        break;
    case 4:
        if (--work->timer == 0) {
            work->mode = MODE_FACE_PLAYER;
            work->submode = 0;
            work->tick = 0;
            work->timer = 0;
        }
        break;
    }
    bigfoot_aim_camera(task);
}

/* the engine's pitch bend for each unit Big Foot comes nearer player 0 in a
 * step */
static Sint32 bigfoot_pitch_scale = 2560;
/* step round on the ground to face player 0, turning by up to FACE_TURN a
 * step for FACE_STEPS. then rest FACE_REST_STEPS once the step's motion ends,
 * and take off again. it starts stepping once the heading to the player lies
 * more than FACE_SLACK from -QUARTER_TURN */
static void bigfoot_face_player(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct motion_control *control = record->block;

    switch (work->submode) {
    case 0:
        work->timer = FACE_STEPS;
        ++work->submode;
        break;
    case 1: {
        Angle diff = mover_player_heading(task, 0) + QUARTER_TURN;
        if (diff < -FACE_SLACK || diff > FACE_SLACK) {
            motion_link(control, MOTION_STEP);
            ++work->submode;
        }
    } break;
    case 2:
        /* @hack spelling: each test through a pointer set inside it, where
         * control->current read in place would do */
        if (control->index == MOTION_STEP) {
            {
                struct motion_play *state;
                if ((state = &control->current)->frame >= FOOT_A_FRAME &&
                    state->frame < FOOT_A_FRAME + 1.0F) {
                    sound_play_for(MECH_BOSS_SOUND_STEP, task,
                                   MECH_BOSS_SOUND_PRIORITY, 0,
                                   STEP_SOUND_STEPS);
                }
            }
            {
                struct motion_play *state;
                if ((state = &control->current)->frame >= FOOT_B_FRAME &&
                    state->frame < FOOT_B_FRAME + 1.0F) {
                    sound_play_for(
                        MECH_BOSS_SOUND_STEP, MECH_BOSS_SECOND_KEY(task),
                        MECH_BOSS_SOUND_PRIORITY, 0, STEP_SOUND_STEPS);
                }
            }
        }
        mover_turn_to_player_by(task, FACE_TURN, 0);
        if (--work->timer == 0) {
            ++work->submode;
        }
        break;
    case 3:
        /* @hack spelling: each test through a pointer set inside it, where
         * control->current read in place would do */
        if (control->index == MOTION_STEP) {
            {
                struct motion_play *state;
                if ((state = &control->current)->frame >= FOOT_A_FRAME &&
                    state->frame < FOOT_A_FRAME + 1.0F) {
                    sound_play_for(MECH_BOSS_SOUND_STEP, task,
                                   MECH_BOSS_SOUND_PRIORITY, 0,
                                   STEP_SOUND_STEPS);
                }
            }
            {
                struct motion_play *state;
                if ((state = &control->current)->frame >= FOOT_B_FRAME &&
                    state->frame < FOOT_B_FRAME + 1.0F) {
                    sound_play_for(
                        MECH_BOSS_SOUND_STEP, MECH_BOSS_SECOND_KEY(task),
                        MECH_BOSS_SOUND_PRIORITY, 0, STEP_SOUND_STEPS);
                }
            }
        }
        if (motion_ended(control)) {
            work->timer = FACE_REST_STEPS;
            motion_link(control, MOTION_STAND);
            ++work->submode;
        }
        break;
    case 4:
        if (--work->timer == 0) {
            work->mode = MODE_TAKE_OFF;
            work->submode = 0;
            work->tick = 0;
            work->timer = 0;
        }
        break;
    }
    bigfoot_aim_camera(task);
}

/* reel from a hit on the ground, close the hatch where it is open, and take
 * off again. the missiles still riding their pods are lost */
static void bigfoot_hit_on_ground(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct motion_control *control = record->block;
    struct bigfoot_parts *parts = record->own;
    /* @hack redundant: mover, which nothing reads */
    struct mover *mover = task->motion.p;

    switch (work->submode) {
    case 0:
        if (work->flags & BIGFOOT_MISSILES) {
            motion_link(control, MOTION_HIT);
        } else {
            motion_link(control, MOTION_HIT_EMPTY);
        }
        work->flags |= WORK_HURT;
        sound_play(MECH_BOSS_SOUND_HIT, NULL, 0, 0);
        sound_set_volume(NULL, MECH_BOSS_SOUND_HIT, MECH_BOSS_HIT_VOLUME);
        rising_sound_stop();
        work->timer = 60;
        work->tick = 0;
        ++work->submode;
    case 1:
        if (game_state.steps % RUMBLE_STEPS == 0) {
            vibration_start(0, MECH_BOSS_RUMBLE_POWER, MECH_BOSS_RUMBLE_FREQ,
                            MECH_BOSS_RUMBLE_TIME);
        }
        if (motion_held(control)) {
            if (--work->timer == 0) {
                if (work->flags & BIGFOOT_MISSILES) {
                    work->timer = 30;
                    sound_play_at(MECH_BOSS_SOUND_HATCH, NULL, 0,
                                  MECH_BOSS_HATCH_VOLUME, &work->pos);
                    motion_link(control, MOTION_RECOVER);
                    work->flags &= ~BIGFOOT_MISSILES;
                } else {
                    work->timer = 1;
                }
                ++work->submode;
            }
        }
        break;
    case 2:
        if (--work->timer == 0) {
            /* @hack type: i a Sint8, where an int would do */
            Sint8 i;
            for (i = MISSILE_SLOTS - 1; i >= 0; i--) {
                /* @hack spelling: mode, a local for the missile's mode, where
                 * the call tested in place would do */
                Sint8 mode = mech_boss_missile_mode(parts->missiles[i]);
                if (mode == MECH_BOSS_MISSILE_RIDING) {
                    task_kill(parts->missiles[i]);
                    parts->missiles[i] = NULL;
                }
            }
            work->mode = MODE_TAKE_OFF;
            work->submode = 0;
        }
        break;
    }
    bigfoot_aim_camera(task);
}

/* the last hit landed on the ground. Big Foot collapses in bursts and smoke,
 * clears the stage, and poses for the winner */
static void bigfoot_down(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct mover *mover = task->motion.p;
    struct motion_control *control = record->block;
    struct bigfoot_parts *parts = record->own;
    /* @hack spelling: phase, a local for the timer modulo the period, where
     * the modulo tested in place would do */
    Sint16 phase;

    switch (work->submode) {
    case 0:
        task_destroy_children(task);
        motion_link(control, MOTION_HIT);
        mover->spd.x = 0.0F;
        mover->spd.y = 0.0F;
        mover->spd.z = 0.0F;
        mover->acc.x = 0.0F;
        mover->acc.y = 0.0F;
        mover->acc.z = 0.0F;
        work->timer = 90;
        {
            struct collision_shape *shape = collision_get_shape(task, 0);
            shape->radius = 35.0F;
            shape->center.x = 0.0F;
            shape->center.y = 40.0F;
            shape->center.z = -40.0F;
        }
        ++work->submode;
    case 1:
        if (work->timer > 30 && work->timer % 15 == 0) {
            vibration_start(0, MECH_BOSS_RUMBLE_POWER, MECH_BOSS_RUMBLE_FREQ,
                            MECH_BOSS_RUMBLE_TIME);
        }
        phase = work->timer % 5;
        if (phase == 0) {
            NJS_VECTOR dir = {0.0F, 0.5F, 0.0F};
            NJS_POINT3 pos;

            switch ((Sint8)(rand() * (5.0F * RAND_SCALE))) {
            case 0:
                pos = parts->capsule_pos;
                pos.x += 5.0F - (rand() * (10.0F * RAND_SCALE));
                pos.y += 5.0F - (rand() * (15.0F * RAND_SCALE));
                pos.z += 15.0F - (rand() * (1.0F * RAND_SCALE));
                break;
            case 1:
                pos = bigfoot_body_pos;
                pos.x += 15.0F - (rand() * (10.0F * RAND_SCALE));
                pos.y += 15.0F - (rand() * (10.0F * RAND_SCALE));
                pos.z += -5.0F - (rand() * (10.0F * RAND_SCALE));
                break;
            case 2:
                pos = bigfoot_body_pos;
                pos.x += -5.0F - (rand() * (10.0F * RAND_SCALE));
                pos.y += 15.0F - (rand() * (10.0F * RAND_SCALE));
                pos.z += -5.0F - (rand() * (10.0F * RAND_SCALE));
                break;
            case 3:
                pos = bigfoot_foot_a_pos;
                pos.x += 3.0F - (rand() * (1.5F * RAND_SCALE));
                pos.y += rand() * (10.0F * RAND_SCALE);
                break;
            case 4:
                pos = bigfoot_foot_b_pos;
                pos.x += 3.0F - (rand() * (1.5F * RAND_SCALE));
                pos.y += rand() * (10.0F * RAND_SCALE);
                break;
            }
            burst_create_wobbling(&pos, &dir, 5.0F);
        }
        if (work->timer-- == 0) {
            motion_link(control, MOTION_COLLAPSE);
            COLLAPSE_SPEED(mover) = 0.0F;
            motion_set_speed(control, COLLAPSE_SPEED(mover));
            work->timer = 240;
            ++work->submode;
        }
        break;
    case 2:
        COLLAPSE_SPEED(mover) += 0.006F;
        if (COLLAPSE_SPEED(mover) > 1.0F) {
            COLLAPSE_SPEED(mover) = 1.0F;
        }
        motion_set_speed(control, COLLAPSE_SPEED(mover));
        if (control->index == MOTION_COLLAPSE &&
            control->current.frame >= 14.0F && !motion_ended(control)) {

            sound_play_for(MECH_BOSS_SOUND_COLLAPSE, task,
                           MECH_BOSS_SOUND_PRIORITY, 0, COLLAPSE_SOUND_STEPS);
            sound_set_volume(task, MECH_BOSS_SOUND_COLLAPSE, COLLAPSE_VOLUME);
        }
        if (!motion_held(control) && game_state.steps % RUMBLE_STEPS == 0) {
            vibration_start(0, MECH_BOSS_RUMBLE_POWER, MECH_BOSS_RUMBLE_FREQ,
                            MECH_BOSS_RUMBLE_TIME);
        }
        phase = work->timer % 3;
        if (phase == 0) {

            NJS_VECTOR dir = {0.0F, 0.5F, 0.0F};
            NJS_POINT3 pos;
            NJS_VECTOR off;
            NJS_VECTOR base = {-1.0F, 0.0F, 0.0F};

            njPushMatrix(&_nj_unit_matrix_);
            njRotateY(NULL, work->ang.y + QUARTER_TURN);
            switch ((Sint8)(rand() * (5.0F * RAND_SCALE))) {
            case 0:
                pos = bigfoot_body_pos;
                off.x = -20.0F + (rand() * (5.0F * RAND_SCALE));
                off.y = 6.0F - (rand() * (10.0F * RAND_SCALE));
                off.z = 4.0F - (rand() * (2.0F * RAND_SCALE));
                break;
            case 1:
                pos = bigfoot_body_pos;
                off.x = -5.0F - (rand() * (10.0F * RAND_SCALE));
                off.y = 15.0F - (rand() * (10.0F * RAND_SCALE));
                off.z = 15.0F - (rand() * (10.0F * RAND_SCALE));
                break;
            case 2:
                pos = bigfoot_body_pos;
                off.x = -5.0F - (rand() * (10.0F * RAND_SCALE));
                off.y = 15.0F - (rand() * (10.0F * RAND_SCALE));
                off.z = -5.0F - (rand() * (10.0F * RAND_SCALE));
                break;
            case 3:
                pos = bigfoot_foot_a_pos;
                off.x = 3.0F - (rand() * (1.5F * RAND_SCALE));
                off.y = rand() * (10.0F * RAND_SCALE);
                off.z = 5.0F + (rand() * (5.0F * RAND_SCALE));
                break;
            case 4:
                pos = bigfoot_foot_b_pos;
                off.x = 3.0F - (rand() * (1.5F * RAND_SCALE));
                off.y = rand() * (5.0F * RAND_SCALE);
                off.z = 5.0F + (rand() * (5.0F * RAND_SCALE));
                break;
            }
            njTranslateV(NULL, &off);
            njCalcPoint(NULL, &base, &off);
            njPopMatrix(1);
            njAddVector(&pos, &off);
            burst_create_wobbling(&pos, &dir, 5.0F);
            {
                struct mech_boss_smoke smoke;
                NJS_POINT3 points[3] = {{-15.0F, 10.0F, -10.0F},
                                        {15.0F, 10.0F, -10.0F},
                                        {0.0F, 5.0F, 10.0F}};

                smoke.count = 3;
                smoke.ang = work->ang;
                smoke.pos = bigfoot_body_pos;
                mech_boss_smoke_points(&smoke, points);
            }
        }
        if (work->timer-- == 0) {
            stage_clear(0);
            pad_disable_player(0);
            ++work->submode;
        }
        break;
    case 3:
        ++work->timer;
        {
            NJS_POINT3 pos = {-40.0F, 0.0F, 0.0F};

            pos.y = work->pos.y;
            /* @hack spelling: the result compared with TRUE, where a test of it
             * would do */
            if (mech_boss_pose_for_winner(task, &pos) == TRUE) {
                work->ang.y = QUARTER_TURN;
            }
        }
        if (work->timer % 5 == 0) {
            struct mech_boss_smoke smoke;
            NJS_POINT3 points[5] = {{-15.0F, 30.0F, -10.0F},
                                    {15.0F, 30.0F, -10.0F},
                                    {0.0F, 10.0F, 10.0F},
                                    {-10.0F, 5.0F, 5.0F},
                                    {10.0F, 5.0F, 5.0F}};

            smoke.count = 5;
            smoke.ang = work->ang;
            smoke.pos = work->pos;
            mech_boss_smoke_points(&smoke, points);
        }
        break;
    }
    if (work->submode < 3 && game_state.steps % MECH_BOSS_SPARKS_STEPS == 0) {
        sound_play(MECH_BOSS_SOUND_SPARKS, NULL, 0, 0);
        sound_set_pitch(
            NULL, MECH_BOSS_SOUND_SPARKS,
            MECH_BOSS_SPARKS_PITCH +
                (rand() * (MECH_BOSS_SPARKS_PITCH_SPREAD * RAND_SCALE)));
        sound_set_volume(
            NULL, MECH_BOSS_SOUND_SPARKS,
            MECH_BOSS_SPARKS_VOLUME +
                (rand() * (MECH_BOSS_SPARKS_VOLUME_SPREAD * RAND_SCALE)));
        sound_set_pan(
            NULL, MECH_BOSS_SOUND_SPARKS,
            MECH_BOSS_SPARKS_PAN +
                (rand() * (MECH_BOSS_SPARKS_PAN_SPREAD * RAND_SCALE)));
    }
    bigfoot_aim_camera(task);
    node_aim_return(parts->turret_aim);
}

/* the last hit landed in the air. Big Foot sinks in bursts and smoke and
 * crashes, clears the stage, and poses for the winner */
static void bigfoot_fall(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct mover *mover = task->motion.p;
    struct bigfoot_parts *parts = record->own;
    /* @hack spelling: phase, a local for the timer modulo the period, where
     * the modulo tested in place would do */
    Sint16 phase;

    switch (work->submode) {
    case 0:
        mover->spd.x = 0.0F;
        mover->spd.y = 0.0F;
        mover->spd.z = 0.0F;
        mover->acc.x = 0.0F;
        mover->acc.y = 0.0F;
        mover->acc.z = 0.0F;
        work->flags &= ~BIGFOOT_PITCHED;
        {
            struct collision_shape *shape = collision_get_shape(task, 0);
            shape->radius = 30.0F;
        }
        task_destroy_children(task);
        work->timer = 240;
        ++work->submode;
    case 1:
        if (!(work->flags & BIGFOOT_PITCHED)) {
            work->ang.x = angle_approach(work->ang.x, 0, 0x100);
            if (work->ang.x == 0) {
                work->flags |= BIGFOOT_PITCHED;
            }
        }
        work->ang.z = angle_approach(work->ang.z, 0, 0x100);
        if (work->timer < 210.0F) {
            mover->acc.y -= 0.0125F;
        }
        if (bigfoot_body_pos.y < 20.0F) {
            work->ang.x = angle_approach(work->ang.x, -0x1000, 0x100);
        }
        if (work->timer < 30.0F) {
            mover->sway_x = angle_approach(mover->sway_x, 0, 0x80);
            mover->sway_y = angle_approach(mover->sway_y, 0, 0x80);
            mover->sway_z = angle_approach(mover->sway_z, 0, 0x80);
        } else {
            mover->sway_x += REEL_SWAY_X_STEP;
            mover->sway_y += REEL_SWAY_Y_STEP;
            mover->sway_z += REEL_SWAY_Z_STEP;
        }
        if (game_state.steps % MECH_BOSS_SPARKS_STEPS == 0) {
            sound_play(MECH_BOSS_SOUND_SPARKS, NULL, 0, 0);
            sound_set_pitch(
                NULL, MECH_BOSS_SOUND_SPARKS,
                MECH_BOSS_SPARKS_PITCH +
                    (rand() * (MECH_BOSS_SPARKS_PITCH_SPREAD * RAND_SCALE)));
            sound_set_volume(
                NULL, MECH_BOSS_SOUND_SPARKS,
                MECH_BOSS_SPARKS_VOLUME +
                    (rand() * (MECH_BOSS_SPARKS_VOLUME_SPREAD * RAND_SCALE)));
            sound_set_pan(
                NULL, MECH_BOSS_SOUND_SPARKS,
                MECH_BOSS_SPARKS_PAN +
                    (rand() * (MECH_BOSS_SPARKS_PAN_SPREAD * RAND_SCALE)));
            if (!(work->flags & BIGFOOT_LANDED)) {
                vibration_start(0, MECH_BOSS_RUMBLE_POWER,
                                MECH_BOSS_RUMBLE_FREQ, MECH_BOSS_RUMBLE_TIME);
            }
        }
        if (work->timer-- == 0) {
            stage_clear(0);
            pad_disable_player(0);
            ++work->submode;
        }
        break;
    case 2: {
        NJS_POINT3 pos = {-40.0F, 0.0F, 0.0F};

        pos.y = work->pos.y;
        /* @hack spelling: the result compared with TRUE, where a test of it
         * would do */
        if (mech_boss_pose_for_winner(task, &pos) == TRUE) {
            work->ang.y = QUARTER_TURN;
        }
    }
        ++work->timer;
        work->ang.x = angle_approach(work->ang.x, -0x1000, 0x100);
        break;
    }
    phase = work->timer % 5;
    if (phase == 0) {
        if (work->submode != 2) {
            NJS_VECTOR dir = {0.0F, 1.0F, 0.0F};
            NJS_POINT3 pos;

            pos = bigfoot_body_pos;
            pos.x += 10.0F - (rand() * (20.0F * RAND_SCALE));
            pos.y += 10.0F - (rand() * (20.0F * RAND_SCALE));
            pos.z += 10.0F - (rand() * (20.0F * RAND_SCALE));
            burst_create_wobbling(&pos, &dir, 5.0F);
        }
        {
            struct mech_boss_smoke smoke;
            NJS_POINT3 points[3] = {{-15.0F, 10.0F, -10.0F},
                                    {15.0F, 10.0F, -10.0F},
                                    {0.0F, 5.0F, 10.0F}};

            smoke.count = 3;
            smoke.ang = work->ang;
            smoke.pos = bigfoot_body_pos;
            mech_boss_smoke_points(&smoke, points);
        }
    }
    mover->acc.y += LIFT;
    bigfoot_aim_camera(task);
    node_aim_return(parts->turret_aim);
    if (bigfoot_body_pos.y < CRASH_HEIGHT) {
        /* @hack spelling: the mover read again from task, where mover
         * would do */
        struct mover *again = task->motion.p;
        if (again->spd.y < 0.0F) {
            /* @hack spelling: the height built in two locals, where one
             * statement would do */
            Float y = CRASH_HEIGHT;
            Float rise = work->pos.y - bigfoot_body_pos.y;
            y += rise;
            work->pos.y = y;
            again->spd.y = 0.0F;
            sound_play_for(MECH_BOSS_SOUND_CRASH, task,
                           MECH_BOSS_SOUND_PRIORITY, 0,
                           MECH_BOSS_CRASH_SOUND_STEPS);
            sound_set_volume(task, MECH_BOSS_SOUND_CRASH,
                             MECH_BOSS_CRASH_VOLUME);
            mech_boss_camera_shake(&work->pos, MECH_BOSS_SHAKE_HEAVY);
            vibration_start(0, MECH_BOSS_CRASH_POWER, MECH_BOSS_CRASH_FREQ,
                            MECH_BOSS_CRASH_TIME);
        }
    }
}

/* reel from a hit in the air, and settle at HOVER_HEIGHT. fly on the way Big
 * Foot was heading, out past BIGFOOT_ARENA_RADIUS. then turn back to charge
 * with the laser */
static void bigfoot_hit_in_air(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct task_work *work = task->work;
    struct mover *mover = task->motion.p;
    struct bigfoot_parts *parts = record->own;
    struct motion_control *control = record->block;
    /* @hack spelling: limit_x, a local for BIGFOOT_ARENA_RADIUS, where the
     * constant would do */
    Float limit_x = BIGFOOT_ARENA_RADIUS;
    /* @hack spelling: acceleration_step, a local for 0.09, where 0.09F would
     * do */
    Float acceleration_step = 0.09F;
    /* @hack spelling: turn, a pointer to angle_approach, where angle_approach
     * called by name would do */
    Angle (*turn)(Angle, Angle, Angle) = angle_approach;
    /* @hack spelling: scale, a local for 0.8, where 0.8F would do */
    Float scale = 0.8F;

    switch (work->submode) {
    case 0:
        if (mover->spd.x == 0.0F) {
            if (work->pos.x > 0.0F) {
                work->flags &= ~BIGFOOT_HEADING_POS_X;
            } else {
                work->flags |= BIGFOOT_HEADING_POS_X;
            }
        } else {
            if (mover->spd.x > 0.0F) {
                work->flags |= BIGFOOT_HEADING_POS_X;
            } else {
                work->flags &= ~BIGFOOT_HEADING_POS_X;
            }
        }
        {
            NJS_VECTOR dir;
            Float len;
            dir = mover->spd;
            len = njScalor(&dir);
            if (len > 1.0F) {
                njUnitVector(&dir);
            }
            if (len > 0.01F) {
                mover->spd.x = -dir.x;
                mover->spd.y = -0.03F;
                mover->spd.z = -dir.z;
            } else {
                mover->spd.x = 0.0F;
                mover->spd.y = -0.03F;
                mover->spd.z = 0.0F;
            }
        }
        if (control->index != MOTION_FLY) {
            motion_link(control, MOTION_KNOCKED);
        }
        sound_play(MECH_BOSS_SOUND_HIT, NULL, 0, 0);
        sound_set_volume(NULL, MECH_BOSS_SOUND_HIT, MECH_BOSS_HIT_VOLUME);
        vibration_start(0, MECH_BOSS_RUMBLE_POWER, MECH_BOSS_RUMBLE_FREQ,
                        MECH_BOSS_RUMBLE_TIME);
        if (parts->laser) {
            task_kill(parts->laser);
            /* @bug memory: kills the laser but leaves parts->laser set.
             * bigfoot_end_hurt clears WORK_HURT in submode 2, and a capsule
             * hit in submodes 3 to 5 then brings Big Foot back here. the
             * task_kill at 0x8c503544 then writes to the freed task. the
             * charge nulls the pointer after each of its kills, at 0x8c501552
             * and 0x8c501606 */
#ifdef NONMATCHING
            /* this side repairs the tag above: it nulls the pointer, as the
             * charge does after its kills */
            parts->laser = NULL;
#endif
        }
        work->timer = 120;
        work->flags |= WORK_HURT;
        ++work->submode;
        break;
    case 1:
        mover->sway_x += REEL_SWAY_X_STEP;
        mover->sway_y += REEL_SWAY_Y_STEP;
        mover->sway_z += REEL_SWAY_Z_STEP;
        mover->spd.x *= 0.98F;
        mover->spd.y *= 0.98F;
        mover->spd.z *= 0.98F;

        work->ang.x = turn(work->ang.x, 0, 0x80);
        work->ang.z = turn(work->ang.z, 0, 0x80);
        {
            NJS_VECTOR scale;
            scale.x = scale.y = scale.z = 0.5F;
            bigfoot_jets(task, &scale);
        }
        if (mover->spd.y < 0.005F) {
            mover->spd.y = 0.0F;
        }
        if (work->timer-- == 0) {
            mover->spd.x = 0.0F;
            mover->spd.y = 0.0F;
            mover->spd.z = 0.0F;
            mover->acc.y = 0.5F;
            if (control->index != MOTION_FLY) {
                motion_link(control, MOTION_KNOCKED);
            }
            ++work->submode;
        }
        break;
    case 2:
        mover->sway_x += RIGHTING_SWAY_X_STEP;
        mover->sway_y += RIGHTING_SWAY_Y_STEP;
        mover->sway_z += RIGHTING_SWAY_Z_STEP;
        work->ang.x = turn(work->ang.x, 0, 0x80);
        work->ang.z = turn(work->ang.z, 0, 0x80);
        bigfoot_jets(task, NULL);
        {
            Float dy = work->pos.y - HOVER_HEIGHT;
            mover->spd.y *= scale;
            if (dy > HOVER_ERROR_LIMIT) {
                dy = HOVER_ERROR_LIMIT;
            } else if (dy < -HOVER_ERROR_LIMIT) {
                dy = -HOVER_ERROR_LIMIT;
            }
            mover->acc.y -= dy * HOVER_PULL;
            if (dy > -HOVER_TOLERANCE && dy < HOVER_TOLERANCE) {
                dy = mover->spd.y;
                if (dy > -HOVER_TOLERANCE && dy < HOVER_TOLERANCE) {
                    work->pos.y = HOVER_HEIGHT;
                    mover->spd.y = 0.0F;
                    mover->acc.y = 0.0F;
                    if (work->flags & BIGFOOT_HEADING_POS_X) {
                        mover->target.x = limit_x;
                    } else {
                        mover->target.x = -limit_x;
                    }
                    mover->target.z = 0.0F;
                    bigfoot_end_hurt(task);
                    work->collision->shapes[LOW_SHAPE].flags |= SHAPE_OFF;
                    ++work->submode;
                }
            }
        }
        break;
    case 3:
        mover->acc.x += njSin(work->ang.y) * 0.1F;
        mover->acc.z += njCos(work->ang.y) * 0.1F;
        mover->spd.x *= 0.94F;
        mover->spd.z *= 0.94F;
        bigfoot_steer(task, 0.081F);
        bigfoot_jets(task, NULL);
        if (njAbs(work->pos.x) > limit_x) {
            mover->target.x = player_work[0]->pos.x;
            if (work->pos.z > RETURN_Z && player_work[0]->pos.z > work->pos.z) {
                mover->target.z = work->pos.z * scale;
            } else if (work->pos.z < -RETURN_Z &&
                       player_work[0]->pos.z < work->pos.z) {
                mover->target.z = work->pos.z * scale;
            } else {
                mover->target.z = player_work[0]->pos.z;
            }
            work->flags &= ~WORK_HURT;
            ++work->submode;
        }
        break;
    case 4:
        mover->acc.x += njSin(work->ang.y) * acceleration_step;
        mover->acc.z += njCos(work->ang.y) * acceleration_step;
        mover->spd.x *= 0.92F;
        mover->spd.z *= 0.92F;
        bigfoot_jets(task, NULL);
        bigfoot_steer(task, 0.07F);
        if (work->pos.x > 0.0F) {
            if (mover->spd.x < 0.0F) {
                ++work->submode;
            }
        } else {
            if (mover->spd.x > 0.0F) {
                ++work->submode;
            }
        }
        break;
    case 5:
        mover->acc.x += njSin(work->ang.y) * acceleration_step;
        mover->acc.z += njCos(work->ang.y) * acceleration_step;
        mover->spd.x *= 0.92F;
        mover->spd.z *= 0.92F;
        bigfoot_jets(task, NULL);
        bigfoot_steer(task, 0.07F);
        if (njAbs(work->pos.x) < HIT_RETURN_X) {
            /* @hack type: the power cast through a Sint8, where an int would
             * do */
            switch ((Sint8)record->power) {
            case 4:
                CHARGES(record) = 2;
                break;
            case 3:
            case 2:
                CHARGES(record) = 3;
                break;
            case 1:
                CHARGES(record) = 4;
                break;
            }
            work->flags &= ~BIGFOOT_PITCHED;
            work->flags |= BIGFOOT_LASER;
            work->mode = MODE_CHARGE;
            work->submode = 0;
            work->tick = 0;
            work->timer = 0;
        }
        break;
    }
    mover->acc.y += LIFT;
    bigfoot_aim_camera(task);
    node_aim_return(parts->turret_aim);
}

/* Big Foot's step. a hit on its capsule throws player 0 and takes one of its
 * power. hits count only while it isn't hurt or starting. the last hit beats
 * it: on the ground below GROUND_HEIGHT, and in the air above. player 0
 * rolling into it is thrown too */
static void bigfoot_step(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct mover *mover = task->motion.p;
    struct motion_control *control = record->block;
    struct bigfoot_parts *parts = record->own;
    /* @hack spelling: zero, a local for 0, where 0.0F would do */
    Float zero;

    if (game_state.time_stopped) {
        return;
    }
    if (game_state.mini_event_running) {
        /* @hack spelling: record->block read again, where control would do */
        motion_step(record->block);
        return;
    }
    /* @hack redundant: a test of mover that no path reaches */
    if (0) {
        if (mover) {
            ;
        }
    }
    zero = 0.0F;
    /* the hover's sound fades as Big Foot speeds up. the engine's sound grows
     * with its speed across the ground. the engine's pitch rises as Big Foot
     * comes nearer player 0 */
    if (!(work->flags & BIGFOOT_LANDED) && work->mode != MODE_FALL) {
        Float speed = njScalor(&mover->spd);
        Float distance = vector_distance(&work->pos, &player_work[0]->pos);
        Float ground_speed;
        Sint16 volume;
        int bend;

        volume = 15.0F - (speed * 10.0F);
        if (volume < -40) {
            volume = -40;
        }
        sound_play_at_for(MECH_BOSS_SOUND_HOVER, task, MECH_BOSS_SOUND_PRIORITY,
                          volume, MECH_BOSS_ENGINE_SOUND_STEPS, &work->pos);
        {
            /* @hack spelling: rate, a copy of zero, where zero would do */
            Float rate = zero;
            bend = (int)(rate * 1536.0F) - 4096;
        }
        if (bend > 4096) {
            bend = 4096;
        }
        sound_set_pitch(task, MECH_BOSS_SOUND_HOVER, bend);
        {
            Float x = mover->spd.x;
            Float z = mover->spd.z;
            ground_speed = njSqrt((x * x) + (z * z));
        }
        volume = (ground_speed * 60.0F) - 20.0F;
        if (volume > 120) {
            volume = 120;
        }
        if (volume > -100) {
            sound_play_at_for(MECH_BOSS_SOUND_ENGINE, task,
                              MECH_BOSS_SOUND_PRIORITY, volume,
                              MECH_BOSS_ENGINE_SOUND_STEPS, &work->pos);
            if (parts->distance != 0.0F) {
                {
                    Float delta = parts->distance - distance;
                    bend = bigfoot_pitch_scale * delta;
                }
                if (bend > MECH_BOSS_ENGINE_BEND) {
                    bend = MECH_BOSS_ENGINE_BEND;
                } else if (bend < -MECH_BOSS_ENGINE_BEND) {
                    bend = -MECH_BOSS_ENGINE_BEND;
                }
                sound_set_pitch(task, MECH_BOSS_SOUND_ENGINE, bend);
            }
            sound_pan_by_pos(MECH_BOSS_SOUND_ENGINE, task, NULL);
        }
        parts->distance = distance;
    } else {
        parts->distance = zero;
    }
    switch (work->mode) {
    case MODE_INTRO:
        bigfoot_intro(task);
        break;
    case MODE_TAKE_OFF:
        bigfoot_take_off(task);
        break;
    case MODE_CHARGE:
        bigfoot_charge(task);
        break;
    case MODE_COME_ROUND:
        bigfoot_come_round(task);
        break;
    case MODE_LAND:
        bigfoot_touch_down(task);
        break;
    case MODE_FACE_PLAYER:
        bigfoot_face_player(task);
        break;
    case MODE_FIRE:
        bigfoot_fire(task);
        break;
    case MODE_HIT_ON_GROUND:
        bigfoot_hit_on_ground(task);
        break;
    case MODE_DOWN:
        bigfoot_down(task);
        break;
    case MODE_HIT_IN_AIR:
        bigfoot_hit_in_air(task);
        break;
    case MODE_FALL:
        bigfoot_fall(task);
        break;
    }
    motion_step(control);
    if (!game_state.cleared) {
        mover_step(task);
    }
    if (work->mode != MODE_FALL) {
        if (work->pos.y < 0.0F) {
            /* @hack spelling: the mover read again from task, where mover
             * would do */
            struct mover *again = task->motion.p;
            if (again->spd.y < 0.0F) {
                work->pos.y = zero;
                again->spd.y = zero;
                /* @hack redundant: an empty else */
            } else {
            }
        }
    }
    if (work->mode != MODE_DOWN && work->mode != MODE_FALL) {
        /* @hack spelling: spd declared once for both arms, where each arm
         * could declare its own */
        NJS_VECTOR spd;

        bigfoot_place_capsule(task);
        if (collision_hurt(parts->capsule) && !(work->flags & WORK_HURT) &&
            !(work->flags & BIGFOOT_STARTING)) {
            struct angles ang = {0, 0, 0};

            spd.x = MECH_BOSS_THROW_X;
            spd.y = MECH_BOSS_THROW_Y;
            spd.z = zero;
            ang.y = work->ang.y + HALF_TURN;
            player_launch_speed(0, &spd, &ang, MECH_BOSS_LAUNCH_LOCK_STEPS);
            if (work->mode != MODE_HIT_ON_GROUND && work->mode != MODE_DOWN) {
                record->power--;
                mech_boss_guard_all(task);
                mech_boss_guard_all(parts->capsule);
                if (record->power > 0.0F) {
                    work->mode = (work->flags & BIGFOOT_LANDED)
                                     ? MODE_HIT_ON_GROUND
                                     : MODE_HIT_IN_AIR;
                    work->submode = 0;
                    work->tick = 0;
                    work->timer = 0;
                } else {
                    work->flags &= ~WORK_HURT;
                    if (work->pos.y < GROUND_HEIGHT) {
                        work->mode = MODE_DOWN;
                        work->submode = 0;
                        work->tick = 0;
                        work->timer = 0;
                    } else {
                        work->mode = MODE_FALL;
                        work->submode = 0;
                        work->tick = 0;
                        work->timer = 0;
                    }
                }
            }
        } else if (collision_next_player(task)) {
            struct angles ang = {0, 0, 0};

            if ((player_work[0]->flags & PLAYER_WORK_BALL) &&
                !(work->flags & WORK_HURT)) {
                spd.x = -1.0F;
                spd.y = 1.0F;
                spd.z = zero;
                ang.y = player_work[0]->ang.y + HALF_TURN;
                player_launch_speed(0, &spd, &ang, MECH_BOSS_LAUNCH_LOCK_STEPS);
            }
        }
        if (work->flags & WORK_HURT) {
            mech_boss_count(&work->tick, MECH_BOSS_BLINK_STEPS);
        }
    }
    bigfoot_place_shapes(task);
}

/* draw the body's shadow and the feet's, as modifier volumes */
static void bigfoot_draw_shadows(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: the reads of record, control, parts and mover, which
     * nothing uses */
    struct boss_record *record = task->extra.p;
    struct motion_control *control = record->block;
    struct bigfoot_parts *parts = record->own;
    struct mover *mover = task->motion.p;
    /* @hack redundant: unused, 156 bytes that nothing reads, which size the
     * frame */
    NJS_POINT3 unused[13];

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateV(NULL, &bigfoot_body_pos);
    njRotateY(NULL, work->ang.y + QUARTER_TURN);
    njCnkModDrawObject(&mech_boss_body_shadow);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslateV(NULL, &bigfoot_foot_a_pos);
    njRotateY(NULL, work->ang.y + QUARTER_TURN);
    njCnkModDrawObject(&mech_boss_foot_shadow);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslateV(NULL, &bigfoot_foot_b_pos);
    njRotateY(NULL, work->ang.y + QUARTER_TURN);
    njRotateY(NULL, HALF_TURN);
    njCnkModDrawObject(&mech_boss_foot_shadow);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

/* the motion callback. keep where the pods, the jets and five of the nodes it
 * draws lie. it keeps each in the world, and some in the body's frame too */
static void bigfoot_find_points(NJS_CNK_OBJECT *object)
{
    NJS_POINT3 point;
    NJS_POINT3 origin = {0.0F, 0.0F, 0.0F};

    if (object == &bigfoot_body_node) {
        njCalcPoint(NULL, &mech_boss_pods_1_vertices[0].point, &point);
        njCalcPoint(&bigfoot_view_to_world, &point, &bigfoot_pods_1[0]);
        njCalcPoint(NULL, &mech_boss_pods_1_vertices[1].point, &point);
        njCalcPoint(&bigfoot_view_to_world, &point, &bigfoot_pods_1[1]);
        njCalcPoint(NULL, &mech_boss_pods_1_vertices[2].point, &point);
        njCalcPoint(&bigfoot_view_to_world, &point, &bigfoot_pods_1[2]);
        njCalcPoint(NULL, &mech_boss_pods_1_vertices[3].point, &point);
        njCalcPoint(&bigfoot_view_to_world, &point, &bigfoot_pods_1[3]);
        njCalcPoint(NULL, &mech_boss_pods_0_vertices[0].point, &point);
        njCalcPoint(&bigfoot_view_to_world, &point, &bigfoot_pods_0[0]);
        njCalcPoint(NULL, &mech_boss_pods_0_vertices[1].point, &point);
        njCalcPoint(&bigfoot_view_to_world, &point, &bigfoot_pods_0[1]);
        njCalcPoint(NULL, &mech_boss_pods_0_vertices[2].point, &point);
        njCalcPoint(&bigfoot_view_to_world, &point, &bigfoot_pods_0[2]);
        njCalcPoint(NULL, &mech_boss_pods_0_vertices[3].point, &point);
        njCalcPoint(&bigfoot_view_to_world, &point, &bigfoot_pods_0[3]);
        njCalcPoint(NULL, &mech_boss_jet_vertices[0].point, &point);
        njCalcPoint(&bigfoot_view_to_world, &point, &bigfoot_jet_points[0]);
        njCalcPoint(NULL, &mech_boss_jet_vertices[1].point, &point);
        njCalcPoint(&bigfoot_view_to_world, &point, &bigfoot_jet_points[1]);
        njCalcPoint(NULL, &mech_boss_jet_vertices[2].point, &point);
        njCalcPoint(&bigfoot_view_to_world, &point, &bigfoot_jet_points[2]);
        njCalcPoint(NULL, &origin, &point);
        njCalcPoint(&bigfoot_view_to_world, &point, &bigfoot_body_pos);
        njCalcPoint(&bigfoot_view_to_body, &point, &bigfoot_body_local);
    }
    if (object == &bigfoot_foot_b_node) {
        /* @hack redundant: offset, a local nothing reads, whose initializer
         * stays in the constants */
        NJS_POINT3 offset = {-1.0F, 0.0F, 0.0F};

        njCalcPoint(NULL, &origin, &point);
        njCalcPoint(&bigfoot_view_to_body, &point, &bigfoot_foot_b_local);
        njCalcPoint(&bigfoot_view_to_world, &point, &bigfoot_foot_b_pos);
    }
    if (object == &bigfoot_foot_a_node) {
        njCalcPoint(NULL, &origin, &point);
        njCalcPoint(&bigfoot_view_to_world, &point, &bigfoot_foot_a_pos);
        njCalcPoint(&bigfoot_view_to_body, &point, &bigfoot_foot_a_local);
    }
    if (object == &bigfoot_turret_node) {
        njCalcPoint(NULL, &origin, &point);
        njCalcPoint(&bigfoot_view_to_world, &point, &bigfoot_turret_pos);
    }
    if (object == &bigfoot_laser_node) {
        njCalcPoint(NULL, &origin, &point);
        njCalcPoint(&bigfoot_view_to_world, &point, &bigfoot_laser_pos);
    }
}

/* keep the matrix from the view to the world, and have the draw call
 * bigfoot_find_points at each node */
static void bigfoot_start_points(struct task *task)
{
    struct boss_record *record = task->extra.p;
    /* @hack redundant: the reads of parts and work, which nothing uses */
    struct bigfoot_parts *parts = record->own;
    struct task_work *work = task->work;

    njGetMatrix(&bigfoot_view_to_world);
    njInvertMatrix(&bigfoot_view_to_world);
    njCnkSetMotionCallback(bigfoot_find_points);
}

/* draw Big Foot in BIGFOOT_LIGHT, swayed while it flies. it draws in
 * MECH_BOSS_HURT_LIGHT for the first half of each hurt blink. a hurt draw then
 * hands Ninja back the stage's light, the one light_select_after_draw set */
static void bigfoot_draw(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct task_work *work = task->work;
    struct mover *mover = task->motion.p;
    /* @hack spelling: control read after mover, where its declaration first
     * would do */
    struct motion_control *control = record->block;

    njSetTexture(&grobo_texlist);
    njPushMatrixEx();
    bigfoot_start_points(task);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    if (!(work->flags & BIGFOOT_LANDED)) {
        njTranslate(NULL, 0.0F,
                    MECH_BOSS_PIVOT_HEIGHT +
                        (njSin(mover->sway_y) * MECH_BOSS_BOB_HEIGHT),
                    0.0F);
        njRotateZ(NULL, work->ang.z + (Angle)(njSin(mover->sway_z) *
                                              MECH_BOSS_SWAY_Z_ANGLE));
        njRotateX(NULL, work->ang.x + (Angle)(njSin(mover->sway_x) *
                                              MECH_BOSS_SWAY_X_ANGLE));
    } else {
        njTranslate(NULL, 0.0F, MECH_BOSS_PIVOT_HEIGHT, 0.0F);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
    }
    njTranslate(NULL, 0.0F, -MECH_BOSS_PIVOT_HEIGHT, 0.0F);
    njGetMatrix(&bigfoot_view_to_body);
    njInvertMatrix(&bigfoot_view_to_body);
    njRotateY(NULL, QUARTER_TURN);
    if (work->flags & WORK_HURT) {
        if (work->tick < MECH_BOSS_BLINK_STEPS / 2) {
            light_apply(MECH_BOSS_HURT_LIGHT);
        } else {
            light_apply(BIGFOOT_LIGHT);
        }
    } else {
        light_apply(BIGFOOT_LIGHT);
    }
    draw_control_set(DRAW_CONTROL_SHADOW);
    motion_draw_simple(&bigfoot_object, control);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    if (work->flags & WORK_HURT) {
        light_apply(light_after_draw);
    }
    njCnkSetMotionCallback(NULL);
    njPopMatrixEx();
    bigfoot_draw_shadows(task);
}
