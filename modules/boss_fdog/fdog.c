/* the Flying Dog, Rouge's boss in the dark story, the task BossFlyingDogExec.
 * the stage's step creates it. it flies on nine jets, charging back and forth
 * across the arena. it aims a laser on each charge but the first of a set. it
 * then stops in the air to fire missiles from two banks of pods. with three
 * hits or fewer left, each set of charges starts with its beam. the beam
 * fires at a sight that follows player 0. each hit on its capsule takes one
 * of its power */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "boss_fdog/fdog.h"
#include "boss_fdog/model_art.h"
#include "boss_fdog/motion_art.h"
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
#include "shared/mech_boss_beam_gun.h"
#include "shared/mech_boss_camera.h"
#include "shared/mech_boss_capsule.h"
#include "shared/mech_boss_laser.h"
#include "shared/mech_boss_laser_beam.h"
#include "shared/mech_boss_missile.h"
#include "shared/mech_boss_sight.h"
#include "shared/mech_boss_sounds.h"
#include "shared/mech_boss_title.h"
#include "shared/mech_boss_voice.h"
#include "shared/node_aim.h"
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

#define JETS 9

/* Joint positions are captured during the model traversal, in world and body
 * coordinates. */
static NJS_POINT3 fdog_body_pos;
static NJS_POINT3 fdog_pods_1[MECH_BOSS_PODS];
static NJS_POINT3 fdog_pods_0[MECH_BOSS_PODS];
static NJS_POINT3 fdog_jet_points[JETS];
static NJS_POINT3 fdog_turret_pos;
static NJS_POINT3 fdog_body_local;
static NJS_POINT3 fdog_laser_pos;
static NJS_POINT3 fdog_gun_pos;
static NJS_MATRIX fdog_view_to_world;
static NJS_MATRIX fdog_view_to_body;

/* the Flying Dog's modes, in its work's mode */
enum fdog_mode {
    MODE_INTRO = 1,
    MODE_CHOOSE = 2, /* plan the next set of charges, and any beam first */
    MODE_CHARGE = 3, /* cross the arena past the player */
    MODE_COME_ROUND = 4, /* turn back toward the player */
    MODE_STOP = 5, /* brake, then open the hatch and load the missiles */
    MODE_FACE_PLAYER = 6,
    MODE_FIRE = 8, /* fire the missiles one by one */
    MODE_BEAM = 9, /* fire the beam at a sight that follows the player */
    MODE_HIT = 10,
    MODE_FALL = 11 /* the last hit landed */
};

/* the entries of fdog_motions */
enum fdog_motion {
    MOTION_REST = 0,
    MOTION_OPEN = 1, /* the hatch opens */
    MOTION_CLOSE = 2,
    MOTION_HIT = 3,
    MOTION_FALL = 4
};
#define FDOG_MOTIONS 5

/* the work's flags */
/* the step crashed the Flying Dog where it came down below CRASH_HEIGHT */
#define FDOG_CRASHED 0x1
#define FDOG_MISSILES 0x100 /* it holds missiles */
#define FDOG_LASER 0x200 /* its charges aim the laser */
/* a stop's lean, or a fall's leveling, is done */
#define FDOG_PITCHED 0x400
/* a hit after the first sends the Flying Dog on toward +x. the hit sets the
 * flag on a move toward +x, or at rest where x <= 0 */
#define FDOG_HEADING_POS_X 0x800
/* camera 0's top mode is the one the step put there for player 0's glide */
#define FDOG_GLIDE_VIEW 0x1000
/* fdog_jets clears this flag, which no code of the object sets */
#define FDOG_UNK_2000 0x2000

#define FDOG_LEVEL 2 /* task_create's level */
#define START_HEIGHT 50.0F /* how far above fdog_create's pos it starts */
#define FULL_POWER 5.0F /* the hits that beat the Flying Dog */
#define MISSILE_SLOTS 12
#define MISSILE_RADIUS 200.0F
#define MISSILE_SPEED 2.6F
#define FDOG_SHAPES 7

/* the first LARGE_JETS jets lie at mech_boss_jet_vertices and puff
 * SMOKE_PUFF_LARGE. the rest lie at fdog_fast_jet_vertices and puff
 * SMOKE_PUFF_FAST */
#define LARGE_JETS 3

/* LOW_SHAPE is the cylinder 40 below the body's point. a stop's final
 * approach turns it on.
 * so do the firing, the turn to face player 0 and a beam. each of those three
 * turns it off as it ends. a hit turns it off, and the flight back after the
 * hit leaves it off */
#define LOW_SHAPE 3

/* the mover's gravity, and the lift each step adds to cancel it */
#define GRAVITY (-0.03F)
#define LIFT (-GRAVITY)

/* each step, the steer's roll speed gains ROLL_GAIN of the gap to its aim. it
 * loses ROLL_DRAG of itself. its pitch speed does the same by PITCH_GAIN and
 * PITCH_DRAG */
#define ROLL_GAIN 0.003F
#define ROLL_DRAG 0.04F
#define PITCH_GAIN 0.004F
#define PITCH_DRAG 0.03F

/* the most the turret's aim turns about y and about z in a call */
#define TURRET_SPEED 0x100

/* how far the sway's phases move in a step. a reel from a hit moves them
 * twice as far. so does a fall, until it settles them toward 0 over the
 * FALL_SETTLE_STEPS before the stage's clear */
#define SWAY_X_STEP 0x420
#define SWAY_Y_STEP 0x500
#define SWAY_Z_STEP 0x600

/* a charge heads for player 0's z, held within CHARGE_LIMIT_Z of z = 0.
 * once within PASS_X of the player along x, it heads for z = 0 instead */
#define CHARGE_LIMIT_Z 145.0F
#define PASS_X 40.0F

/* a turn back heads for player 0's x, and ends within RETURN_X of x = 0.
 * along z, it heads CROSS_Z past z = 0 on the far side. with no charge left,
 * it heads STOP_Z out on the player's side instead */
#define RETURN_X 260.0F
#define CROSS_Z 50.0F

/* a stop heads for STOP_X from x = 0 on the Flying Dog's side. it heads for
 * player 0's z, held within STOP_Z of z = 0. its final approach starts within
 * APPROACH_X of x = 0, and turns LOW_SHAPE on. it stops accelerating within
 * STOP_X, and brakes */
#define STOP_X 170.0F
#define STOP_Z 105.0F
#define APPROACH_X 250.0F
/* after a hit, the flight back heads for player 0's x. along z, it heads
 * STOP_Z out on the player's side. within RETURN_X of x = 0, it heads for the
 * point at that z STOP_RADIUS from the origin. it then approaches and brakes as
 * a stop does, but leaves LOW_SHAPE off */
#define STOP_RADIUS 160.0F
/* a stop leans forward STOP_LEAN and levels again, and the flight back after a
 * hit HIT_LEAN. each turns by LEAN_SPEED a step */
#define STOP_LEAN 0x300
#define HIT_LEAN 0x400
#define LEAN_SPEED 256

/* the step holds the Flying Dog within LIMIT_Z of z = 0 */
#define LIMIT_Z 125.0F

/* below CRASH_HEIGHT, the step stops the Flying Dog's descent. the first
 * time, it crashes it and sets FDOG_CRASHED. in a fall under FDOG_CRASHED,
 * each shape lies FALLEN_SHIFT farther along the body's z */
#define CRASH_HEIGHT 8.5F
#define FALLEN_SHIFT 25.0F

/* how far the capsule lies above the body's point */
#define CAPSULE_HEIGHT 7.0F
/* how far the cameras' focus lies above the work's pos */
#define FOCUS_HEIGHT 5.0F

/* the draw sways the body less than the shapes and the capsule do. it takes
 * these where they take MECH_BOSS_SWAY_X_ANGLE, MECH_BOSS_SWAY_Z_ANGLE and
 * MECH_BOSS_BOB_HEIGHT */
#define DRAW_SWAY_X_ANGLE (0.4F * ANGLE_PER_DEGREE)
#define DRAW_SWAY_Z_ANGLE (0.75F * ANGLE_PER_DEGREE)
#define DRAW_BOB_HEIGHT 0.5F

/* the opening repeats MECH_BOSS_SOUND_UNK_19 from INTRO_SOUND_START steps
 * before its end until INTRO_SOUND_STEPS before. each time, the sound lasts
 * INTRO_SOUND_STEPS */
#define INTRO_SOUND_START 420
#define INTRO_SOUND_STEPS 115

/* the firing launches a missile every SHOT_STEPS, and rests VOLLEY_REST_STEPS
 * after each volley */
#define SHOT_STEPS 12
#define VOLLEY_REST_STEPS 35

/* the sight moves at most SIGHT_SPEED a step. the gun tilts toward it only
 * within GUN_RANGE */
#define SIGHT_SPEED 1.0F
#define GUN_RANGE 150.0F
/* before each shot, a beam aims the gun at the sight for BEAM_AIM_STEPS. the
 * beam ends BEAM_END_STEPS after its last shot */
#define BEAM_AIM_STEPS 300
#define BEAM_END_STEPS 30

#define CHARGE_VOLUME 40
/* while one hit is left, the step repeats MECH_BOSS_SOUND_UNK_19 with a
 * duration of LAST_HIT_SOUND_STEPS */
#define LAST_HIT_SOUND_STEPS 105

#define FACE_STEPS 300
#define LATE_FACE_STEPS 60
#define FACE_SLACK 0x100
#define FACE_TURN 0x80
#define FACE_REST_STEPS 40
/* a fall's sway settles toward 0 over its last FALL_SETTLE_STEPS */
#define FALL_SETTLE_STEPS 60

/* the Flying Dog's own fields of its boss record, a struct boss_record of
 * shared/boss.h. MISSILES: the missiles it holds. SHOTS, the same field: the
 * beam's shots left. CHARGES: the charges left before it stops */
#define MISSILES(record) ((record)->unk_01)
#define SHOTS(record) ((record)->unk_01)
#define CHARGES(record) ((record)->unk_02)

/* fields of the Flying Dog's mover that hold the Flying Dog's own state */
/* its turn speed about each axis, in degrees a step */
#define TURN(mover) ((mover)->home)
/* its squared speed across the ground at the last steer */
#define LAST_SPEED2(mover) ((mover)->unk_b4)
/* the missiles held as each volley starts, which nothing reads. a charge's
 * start sets the field to 0 */
#define VOLLEY_MISSILES(mover) ((mover)->bottom)

/* the Flying Dog's parts, in its boss record's own block */
struct fdog_parts {
    struct node_aim *turret_aim;
    struct node_aim *gun_aim;
    struct task *missiles[MISSILE_SLOTS]; /* each riding its pod, or null */
    struct task *sight; /* a beam's sight, or null */
    Uint8 unk_3c[0xc];
    struct task *laser; /* the charge's laser spot, or null */
    struct task *capsule;
    struct task *beam_gun; /* a beam's gun, or null */
    struct angles capsule_ang;
    NJS_POINT3 capsule_pos;
    Float distance; /* from player 0 at the last step, or 0 */
};

/* where a shape's center lies: offset from *base, a point in the body's
 * frame. a null base stands for the frame's origin */
struct fdog_shape_place {
    const struct collision_shape *shape;
    NJS_POINT3 *base;
    NJS_VECTOR offset;
};

/* the pod a missile rides: fdog_pods_0[index], or fdog_pods_1[index] where
 * bank isn't 0 */
struct fdog_launch {
    Sint8 bank;
    Sint8 index;
};

static void fdog_start(struct task *task);
static void fdog_destroy(struct task *task);
/* NOLINTBEGIN(misc-use-internal-linkage) */
Angle fdog_steer(struct task *task, Float turn_acceleration);
/* NOLINTEND(misc-use-internal-linkage) */
/* SHC needs an external declaration for the two register calls in
 * fdog_stop. Both call the steering routine at 0x8c500604. */
#ifdef NONMATCHING
/* the steering routine by its own name: this side's objects don't define
 * fdog_steer_outside, which only symbols.txt places */
#define fdog_steer_outside fdog_steer
#else
Angle fdog_steer_outside(struct task *task, Float turn_acceleration);
#endif
static void fdog_jets(struct task *task, NJS_VECTOR *scale);
static void fdog_place_shapes(struct task *task);
static void fdog_place_capsule(struct task *task);
static void fdog_aim_camera(struct task *task);
static Bool fdog_is_flying(struct task *task);
static void fdog_end_hurt(struct task *task);
static void fdog_hover(struct task *task);
static void fdog_intro(struct task *task);
static void fdog_choose(struct task *task);
static void fdog_charge(struct task *task);
static void fdog_come_round(struct task *task);
static void fdog_stop(struct task *task);
static void fdog_fire(struct task *task);
static void fdog_face_player(struct task *task);
static void fdog_hit(struct task *task);
static void fdog_fall(struct task *task);
static void fdog_beam(struct task *task);
static void fdog_step(struct task *task);
static void fdog_draw_shadow(struct task *task);
static void fdog_find_points(NJS_CNK_OBJECT *object);
static void fdog_start_points(struct task *task);
static void fdog_draw(struct task *task);

static const struct motion_table_entry fdog_motions[FDOG_MOTIONS] = {
#include "boss_fdog/fdog/fdog_motions.inc"
};

/* clang-format off */
static const struct collision_shape fdog_shapes[FDOG_SHAPES] = {
    {0, SHAPE_SPHERE, MECH_BOSS_PUSH, MECH_BOSS_HURTS | SHAPE_ATTACK_LEVEL(1),
     MECH_BOSS_SHAPE_FLAGS, {0.0F, 2.0F, -2.0F}, 16.0F, 0.0F, 0.0F, 0,
     {0, 0, 0}},
    {0, SHAPE_SPHERE, MECH_BOSS_PUSH, MECH_BOSS_HURTS | SHAPE_ATTACK_LEVEL(1),
     MECH_BOSS_SHAPE_FLAGS, {5.0F, 17.0F, -8.0F}, 11.0F, 0.0F, 0.0F, 0,
     {0, 0, 0}},
    {0, SHAPE_SPHERE, MECH_BOSS_PUSH, MECH_BOSS_HURTS | SHAPE_ATTACK_LEVEL(1),
     MECH_BOSS_SHAPE_FLAGS, {-5.0F, 17.0F, -8.0F}, 11.0F, 0.0F, 0.0F, 0,
     {0, 0, 0}},
    {0, SHAPE_CYLINDER, MECH_BOSS_PUSH, MECH_BOSS_HURTS,
     MECH_BOSS_SHAPE_FLAGS | SHAPE_UNK_2000 | SHAPE_OFF | SHAPE_NO_OWN_HITS,
     {0.0F, -40.0F, -8.0F}, 12.0F, 25.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, MECH_BOSS_PUSH, MECH_BOSS_HURTS | SHAPE_ATTACK_LEVEL(3),
     MECH_BOSS_SHAPE_FLAGS, {0.0F, -5.0F, -8.0F}, 15.0F, 0.0F, 0.0F, 0,
     {0, 0, 0}},
    {0, SHAPE_SPHERE, MECH_BOSS_PUSH, MECH_BOSS_HURTS | SHAPE_ATTACK_LEVEL(3),
     MECH_BOSS_SHAPE_FLAGS, {0.0F, -5.0F, -8.0F}, 11.0F, 0.0F, 0.0F, 0,
     {0, 0, 0}},
    {0, SHAPE_SPHERE, MECH_BOSS_PUSH, MECH_BOSS_HURTS | SHAPE_ATTACK_LEVEL(3),
     MECH_BOSS_SHAPE_FLAGS, {0.0F, -5.0F, -8.0F}, 11.0F, 0.0F, 0.0F, 0,
     {0, 0, 0}},
};
/* clang-format on */

void fdog_create(NJS_POINT3 *pos, Angle ang_y)
{
    struct task *task;
    struct task_work *work;
    struct boss_record *record;
    struct fdog_parts *parts;

#ifdef NONMATCHING
    /* The missile hatch needs its timed opening transition. */
    if (fdog_motions[MOTION_OPEN].link_frames < 1) {
        return;
    }
#endif
    task = task_create(TASK_WORK, FDOG_LEVEL, fdog_step, "BossFlyingDogExec");
#ifdef NONMATCHING
    /* this side creates no boss where task_create fails. the matching side
     * writes through the null */
    if (task == NULL) {
        return;
    }
#endif
    work = task->work;
    record = boss_record_create(task);
    /* @bug memory: frees the record and the mover twice where the heap
     * can't give the record's block. boss_record_create frees both at
     * 0x8c500362, but leaves them in task's extra and motion. the task_kill
     * at 0x8c50036e leaves the task to task_destroy, which frees them again */
    if (record == NULL) {
        task_kill(task);
        return;
    }
    parts = boss_record_alloc_own(record, sizeof(struct fdog_parts));
    if (parts == NULL) {
        /* @bug memory: leaks the record's block where the heap can't give the
         * parts. the task_kill at 0x8c500386 comes before the destructor is
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
    motion_init(record->block, fdog_motions);
    parts->turret_aim = node_aim_create(&fdog_turret_node, &fdog_turret_pos);
    /* @bug memory: leaves the turret's aim null where the heap can't give it.
     * the test at 0x8c5003a2 guards only the limits. node_aim_reset reads
     * through the aim at 0x8c531b86, from fdog_destroy and fdog_charge.
     * node_aim_set reads through it at 0x8c531b4c, from fdog_charge.
     * node_aim_return reads through it at 0x8c531aee, from fdog_hit and
     * fdog_fall */
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
    parts->gun_aim = node_aim_create(&fdog_gun_node, &fdog_gun_pos);
    /* @bug memory: leaves the gun's aim null where the heap can't give it.
     * the test at 0x8c5003e8 guards only the limits. node_aim_reset reads
     * through the aim at 0x8c531b86, from fdog_destroy and fdog_beam.
     * node_aim_point reads through it at 0x8c5319da, from fdog_beam.
     * node_aim_return reads through it at 0x8c531aee, from fdog_hit,
     * fdog_fall and fdog_beam */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the heap can't give the aim, it
     * frees the turret's aim and the record's parts, and kills the task */
    if (parts->gun_aim == NULL) {
        node_aim_free(parts->turret_aim);
        boss_record_free_parts(task);
        task_kill(task);
        return;
    }
#endif
    if (parts->gun_aim != NULL) {
        struct node_aim_limits limits = {
            {0, 0x80, 0x80}, {0, 0x1400, 0x3000}, {0, -0x1400, -0xc00}};

        node_aim_set_limits(parts->gun_aim, &limits);
    }
    work->pos = *pos;
    work->pos.y += START_HEIGHT;
    work->ang.y = ang_y;
    fdog_start(task);
    mover_set_gravity(task, GRAVITY);
    collision_init(task, fdog_shapes, FDOG_SHAPES, COLLISION_ENEMY);
    /* @hack spelling: task->work read again, where work would do */
    task->work->collision->flags &= ~COLLISION_TARGET;
    /* @hack spelling: spec declared in a block of its own, where a local of
     * the function would do */
    {
        struct mech_boss_capsule_spec spec;

        spec.pos = &parts->capsule_pos;
        spec.ang = &parts->capsule_ang;
        spec.boss = MECH_BOSS_FLYING_DOG;
        spec.range = FDOG_ARENA_HALF_X;
        parts->capsule = mech_boss_capsule_create(task, &spec);
    }
    pad_disable_player(0);
    task->draw = fdog_draw;
    task->destructor = fdog_destroy;
}

/* put the Flying Dog in MODE_INTRO, at rest, with FULL_POWER. it holds no
 * missile, laser, sight or beam gun */
static void fdog_start(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct task_work *work = task->work;
    struct motion_control *control = record->block;
    struct fdog_parts *parts = record->own;
    /* @hack type: i a Sint8, where an int would do */
    Sint8 i;

    work->mode = MODE_INTRO;
    work->timer = 0;
    work->tick = 0;
    record->power = FULL_POWER;
    parts->capsule_pos = work->pos;
    fdog_body_pos = work->pos;
    work->flags &= ~FDOG_CRASHED;
    work->id = WORK_ID_ATTACK;
    for (i = 0; i < MISSILE_SLOTS; i++) {
        parts->missiles[i] = NULL;
    }
    parts->laser = NULL;
    parts->sight = NULL;
    parts->beam_gun = NULL;
    motion_set(control, MOTION_REST);
}

/* @hack redundant: task declared register, where a plain parameter would do */
static void fdog_destroy(register struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct fdog_parts *parts = record->own;

    sound_stop_key(task);
    sound_stop_key(MECH_BOSS_SECOND_KEY(task));
    node_aim_reset(parts->turret_aim);
    node_aim_free(parts->turret_aim);
    node_aim_reset(parts->gun_aim);
    node_aim_free(parts->gun_aim);
    voices_stop();
    boss_record_free_parts(task);
}

/* turn the Flying Dog toward its mover's target. turn_acceleration is the
 * most its turn speed gains in a step, in degrees a step. the body rolls into
 * the turn, and pitches as its speed changes. it returns twice the turn left,
 * which no caller reads */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* @hack type: external linkage, which the link needs for
 * fdog_steer_outside, where static would do without that name */
Angle fdog_steer(struct task *task, Float turn_acceleration)
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
/* NOLINTEND(misc-use-internal-linkage) */

static struct fdog_shape_place fdog_shape_places[FDOG_SHAPES] = {
    {&fdog_shapes[0], &fdog_body_local, {0.0F, 2.0F, -2.0F}},
    {&fdog_shapes[1], &fdog_body_local, {13.0F, 17.0F, -8.0F}},
    {&fdog_shapes[2], &fdog_body_local, {-13.0F, 17.0F, -8.0F}},
    {&fdog_shapes[3], &fdog_body_local, {0.0F, -40.0F, -8.0F}},
    {&fdog_shapes[4], &fdog_body_local, {0.0F, -5.0F, -8.0F}},
    {&fdog_shapes[5], &fdog_body_local, {12.0F, 2.0F, -10.0F}},
    {&fdog_shapes[6], &fdog_body_local, {-12.0F, 2.0F, -10.0F}}};

/* each jet's nozzle points along the body's up, turned about x by the jet's
 * pitch. the jet's roll then turns it about z. both are in degrees */
static Sint32 jet_pitch[JETS] = {-124, 180, -124, 0, -108, 0, 0, -108, 0};
static Sint32 jet_roll[JETS] = {0, 0, 0, 147, 0, 147, -147, 0, -147};
static Float dat_8c54f85c = 0.8F;

/* a puff from each jet, along its nozzle. scale scales each puff's speed, or
 * null leaves it as it is */
static void fdog_jets(struct task *task, NJS_VECTOR *scale)
{
    struct task_work *work = task->work;
    Sint32 *pitch;
    Sint32 *roll;
    NJS_POINT3 *point;
    NJS_VECTOR dir;
    /* @hack redundant: unused, 88 bytes that nothing reads, which size the
     * frame */
    Sint32 unused[22];
    /* @hack type: i a Sint8, where an int would do */
    Sint8 i;

    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    /* @hack spelling: the pitches, the rolls and the jets' points walked by
     * pointers, where an index would do */
    pitch = jet_pitch;
    roll = jet_roll;
    point = fdog_jet_points;
    for (i = 0; i < JETS; i++) {
        NJS_VECTOR up = {0.0F, 1.0F, 0.0F};
        njPushMatrixEx();
        njRotateZ(NULL, (Angle)(*roll * ANGLE_PER_DEGREE));
        njRotateX(NULL, (Angle)(*pitch * ANGLE_PER_DEGREE));
        njCalcVector(NULL, &up, &dir);
        njPopMatrix(1);
        if (scale) {
            dir.x *= scale->x;
            dir.y *= scale->y;
            dir.z *= scale->z;
        }
        if (i < LARGE_JETS) {
            smoke_puff_create(task, SMOKE_PUFF_LARGE, point, &dir);
        } else {
            smoke_puff_create(task, SMOKE_PUFF_FAST, point, &dir);
        }
        pitch++;
        roll++;
        point++;
    }
    njPopMatrix(1);
    work->flags &= ~FDOG_UNK_2000;
}

/* center each shape at its place, and register the collision. the
 * registration takes the work's angles and height swayed */
static void fdog_place_shapes(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: the reads of record and parts, which nothing uses */
    struct boss_record *record = task->extra.p;
    struct fdog_parts *parts = record->own;
    struct fdog_shape_place *place;
    /* @hack type: i a Sint8, where an int would do */
    Sint8 i;

    place = fdog_shape_places;
    for (i = 0; i < FDOG_SHAPES; i++) {
        NJS_POINT3 pos = {0.0F, 0.0F, 0.0F};
        if (work->mode == MODE_FALL && (work->flags & FDOG_CRASHED)) {
            NJS_POINT3 offset = {0.0F, 0.0F, FALLEN_SHIFT};
            if (place->base) {
                njAddVector(&pos, place->base);
            }
            njAddVector(&pos, &place->offset);
            njAddVector(&pos, &offset);
            /* @bug logic: pops Ninja's matrix stack, which nothing here
             * pushed. the pop at 0x8c500c72 runs for each shape at each step
             * of a fall under FDOG_CRASHED */
            njPopMatrixEx();
        } else {
            if (place->base) {
                njAddVector(&pos, place->base);
            }
            njAddVector(&pos, &place->offset);
        }
        collision_set_shape_center(task, i, &pos);
        place++;
    }
    {
        struct mover *mover = task->motion.p;
        struct angles saved;
        Float saved_y;
        /* @hack spelling: sin, a pointer to njSin, where njSin called by
         * name would do */
        Float (*sin)(Angle);

        saved_y = work->pos.y;
        saved = work->ang;
        sin = njSin;
        work->ang.x += (Angle)(sin(mover->sway_x) * MECH_BOSS_SWAY_X_ANGLE);
        work->ang.z += (Angle)(sin(mover->sway_z) * MECH_BOSS_SWAY_Z_ANGLE);
        work->pos.y += sin(mover->sway_y) * MECH_BOSS_BOB_HEIGHT;
        collision_register(task);
        work->pos.y = saved_y;
        work->ang.x = saved.x;
        work->ang.z = saved.z;
        /* @hack redundant: a read of the mover's spd.y into saved_y, which
         * nothing reads after */
        saved_y = mover->spd.y;
    }
}

/* hold the capsule CAPSULE_HEIGHT above the body's point. it takes the
 * work's angles, swayed as fdog_place_shapes sways them */
static void fdog_place_capsule(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct fdog_parts *parts = record->own;
    struct mover *mover = task->motion.p;

    parts->capsule_pos = fdog_body_pos;
    parts->capsule_pos.y += CAPSULE_HEIGHT;
    parts->capsule_ang = work->ang;
    parts->capsule_ang.x +=
        (Angle)(njSin(mover->sway_x) * MECH_BOSS_SWAY_X_ANGLE);
    parts->capsule_ang.z +=
        (Angle)(njSin(mover->sway_z) * MECH_BOSS_SWAY_Z_ANGLE);
}

/* put the cameras' focus FOCUS_HEIGHT above the work's pos */
static void fdog_aim_camera(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    NJS_VECTOR offset;

    offset.x = 0.0F;
    offset.y = FOCUS_HEIGHT;
    offset.z = 0.0F;
    boss_record_set_focus(record, &work->pos, &offset);
}

static Bool fdog_is_flying(struct task *task)
{
    struct task_work *work = task->work;

    if (work->mode == MODE_FALL) {
        return FALSE;
    }
    return TRUE;
}

/* end the hurt state. each shape and the capsule take back the levels they
 * start with */
static void fdog_end_hurt(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct fdog_parts *parts = record->own;
    struct mech_boss_levels levels[FDOG_SHAPES];
    struct mech_boss_levels *entry = levels;
    const struct collision_shape *shape = fdog_shapes;
    /* @hack type: i a Sint8, where an int would do */
    Sint8 i;

    /* @hack spelling: the levels and the shapes walked by pointers, where an
     * index would do */
    for (i = 0; i < FDOG_SHAPES; ++i) {
        entry->attack = shape->levels & SHAPE_ATTACK;
        entry->guard = (shape->levels & SHAPE_GUARD) >> SHAPE_GUARD_SHIFT;
        entry++;
        shape++;
    }
    mech_boss_set_levels(task, levels);
    mech_boss_capsule_reset(parts->capsule);
    /* @hack spelling: task->work read here, where a local of the function
     * would do */
    task->work->flags &= ~WORK_HURT;
    /* @hack redundant: task->extra read into record again, which nothing
     * reads after */
    record = task->extra.p;
}

/* the step while a mini event runs. the Flying Dog hovers, sways and puffs,
 * and the capsule follows it */
static void fdog_hover(struct task *task)
{
    struct mover *mover = task->motion.p;
    struct boss_record *record = task->extra.p;
    struct motion_control *control = record->block;

    mover->acc.y += LIFT;
    mover->sway_x += SWAY_X_STEP;
    mover->sway_y += SWAY_Y_STEP;
    mover->sway_z += SWAY_Z_STEP;
    /* @hack redundant: a read of the record's block, which nothing uses */
    (void)record->block;
    motion_step(control);
    mover_step(task);
    fdog_jets(task, NULL);
    fdog_place_capsule(task);
}

/* the opening. the camera turns about the Flying Dog as its title shows, and
 * play starts MECH_BOSS_INTRO_STEPS later */
static void fdog_intro(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;

    fdog_aim_camera(task);
    switch (work->submode) {
    case 0:
        /* @hack spelling: camera declared in a block of its own, where a
         * local of the function would do */
        {
            struct camera_bossinit camera;
            camera.speed = 0.6666F;
            camera.ang = work->ang.y + 0x9000;
            camera.target = &record->focus;
            camera.start_dist = 100.0F;
            camera.dist = 60.0F;
            camera.turn = 0x38e3;
            camera_bossinit_start(0, &camera);
        }
        record->gauge = boss_gauge_create(NULL, &record->power);
        pad_disable_player(0);
        player_work[0]->pos.x = 0.0F;
        player_work[0]->pos.y = 0.0F;
        player_work[0]->pos.z = 0.0F;
        {
            NJS_POINT3 position = {350.0F, 250.0F, -1.5F};
            record->title =
                mech_boss_title_create(task, MECH_BOSS_FLYING_DOG, &position);
        }
        work->timer = MECH_BOSS_INTRO_STEPS;
        mech_boss_voice_create(task, MECH_BOSS_FLYING_DOG, &record->power);
        ++work->submode;
        break;
    case 1:
        if (--work->timer == 0) {
            struct camera_bosspoint camera;
            camera_bossinit_end(0);
            camera.target = &record->focus;
            camera.dist = MECH_BOSS_CAMERA_DIST;
            camera.height = MECH_BOSS_CAMERA_HEIGHT;
            camera.part = MECH_BOSS_CAMERA_PART;
            camera.adjust = CAMERA_ADJUST_RELATIVE3;
            camera.collides = 0;
            camera.trigger_push = 0;
            camera.lift = MECH_BOSS_CAMERA_LIFT;
            camera.level = 0;
            camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_RELATIVE2);
            camera_bosspoint_start(0, &camera);
            task_kill(record->title);
            /* @hack redundant: reads of the record's gauge and power,
             * which nothing uses */
            (void)record->gauge;
            (void)record->power;
            boss_messenger_summon_capped(0, MECH_BOSS_MESSAGES, task);
            pad_enable_player(0);
            work->mode = MODE_CHOOSE;
            work->submode = 0;
            work->tick = 0;
            work->timer = 0;
        }
        break;
    }
    if (work->timer < INTRO_SOUND_START && work->timer > INTRO_SOUND_STEPS) {
        sound_play_repeating(MECH_BOSS_SOUND_UNK_19, task,
                             MECH_BOSS_SOUND_PRIORITY, 0, INTRO_SOUND_STEPS);
    }
    fdog_aim_camera(task);
}

/* plan the next set of charges by the power left: four, or five with one hit
 * left. with three hits or fewer left, fire the beam first. the set's first
 * charge aims no laser */
static void fdog_choose(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct task_work *work = task->work;

    /* @hack redundant: a jump to the next statement */
    goto choose;
choose:
    /* @hack type: the power cast through a Sint8, where an int would do */
    switch ((Sint8)record->power) {
    case 5:
    case 4:
        CHARGES(record) = 4;
        SHOTS(record) = 0;
        break;
    case 3:
    case 2:
        CHARGES(record) = 4;
        SHOTS(record) = 1;
        break;
    case 1:
        CHARGES(record) = 5;
        SHOTS(record) = 3;
        break;
    }
    work->flags &= ~FDOG_LASER;
    if (SHOTS(record) == 0) {
        work->mode = MODE_CHARGE;
        work->submode = 0;
    } else {
        work->mode = MODE_BEAM;
        work->submode = 0;
    }
}

/* charge across the arena past player 0, out past FDOG_ARENA_HALF_X along x.
 * under FDOG_LASER, the laser aims at the player until the Flying Dog passes
 * it */
static void fdog_charge(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct mover *mover = task->motion.p;
    struct fdog_parts *parts = record->own;

    switch (work->submode) {
    case 0:
        if (work->flags & FDOG_LASER) {
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
                spec.origin = &fdog_laser_pos;
                parts->laser = mech_boss_laser_create(task, &spec);
            }
#ifdef NONMATCHING
            /* this side repairs the tag in the matching side: where the heap
             * can't give the laser, the charge goes on without it, as one
             * without FDOG_LASER does */
            if (parts->laser == NULL) {
                work->flags &= ~FDOG_LASER;
            } else {
                struct mech_boss_laser_beam_spec spec;

                spec.unk_00 = 2;
                spec.origin = &fdog_laser_pos;
                spec.target = &parts->laser->work->pos;
                spec.mover = mover;
                mech_boss_laser_beam_create(parts->laser, &spec);
                mech_boss_laser_set_mode(parts->laser, MECH_BOSS_LASER_AIM);
            }
#else
            {
                struct mech_boss_laser_beam_spec spec;
                spec.unk_00 = 2;
                spec.origin = &fdog_laser_pos;
                /* @bug memory: reads through parts->laser at 0x8c501476,
                 * null where the heap couldn't give the laser. the test of
                 * it under FDOG_LASER, at 0x8c50161a, comes after that read.
                 * the charge also sets the laser's mode and kills it
                 * untested */
                spec.target = &parts->laser->work->pos;
                spec.mover = mover;
                mech_boss_laser_beam_create(parts->laser, &spec);
            }
            mech_boss_laser_set_mode(parts->laser, MECH_BOSS_LASER_AIM);
#endif
        }
        VOLLEY_MISSILES(mover) = 0.0F;
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
            mover->target.z = 0.0F;
            ++work->submode;
        }
        break;
    case 2:
        if (mover->spd.x > 0.0F) {
            mover->target.x = FDOG_ARENA_HALF_X;
            if (work->flags & FDOG_LASER) {
                if (player_work[0]->pos.x - work->pos.x < 0.0F) {
                    mech_boss_laser_set_mode(parts->laser,
                                             MECH_BOSS_LASER_REST);
                }
            }
            if (work->pos.x > FDOG_ARENA_HALF_X) {
                if (work->flags & FDOG_LASER) {
                    task_kill(parts->laser);
                    parts->laser = NULL;
                }
                node_aim_reset(parts->turret_aim);
                work->mode = MODE_COME_ROUND;
                work->submode = 0;
                --CHARGES(record);
            }
        } else {
            mover->target.x = -FDOG_ARENA_HALF_X;
            if (work->flags & FDOG_LASER) {
                if (player_work[0]->pos.x - work->pos.x > 0.0F) {
                    mech_boss_laser_set_mode(parts->laser,
                                             MECH_BOSS_LASER_REST);
                }
            }
            if (work->pos.x < -FDOG_ARENA_HALF_X) {
                if (work->flags & FDOG_LASER) {
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
    sound_play_at_for(MECH_BOSS_SOUND_ENGINE, MECH_BOSS_SECOND_KEY(task),
                      MECH_BOSS_SOUND_PRIORITY, CHARGE_VOLUME,
                      MECH_BOSS_ENGINE_SOUND_STEPS, &work->pos);
    if (work->flags & FDOG_LASER) {
        if (parts->laser) {
            struct angles angle;
            angle = *mech_boss_laser_angles(parts->laser);
            angle.z += MECH_BOSS_TURRET_TILT;
            node_aim_set(parts->turret_aim, &angle);
        }
        mover->acc.x += njSin(work->ang.y) * 0.1F;
        mover->acc.z += njCos(work->ang.y) * 0.1F;
        mover->spd.x *= 0.96F;
        mover->spd.z *= 0.96F;
        fdog_steer(task, 0.068F);
    } else {
        mover->acc.x += njSin(work->ang.y) * 0.12F;
        mover->acc.z += njCos(work->ang.y) * 0.12F;
        mover->spd.x *= 0.97F;
        mover->spd.z *= 0.97F;
        fdog_steer(task, 0.075F);
    }
    fdog_aim_camera(task);
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
 * with the laser. where no charge is left, stop instead */
static void fdog_come_round(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct mover *mover = task->motion.p;

    switch (work->submode) {
    case 0:
        mover->target.x = player_work[0]->pos.x;
        if (CHARGES(record)) {
            if (work->pos.z > 0.0F) {
                mover->target.z = -CROSS_Z;
            } else {
                mover->target.z = CROSS_Z;
            }
        } else {
            if (player_work[0]->pos.z > 0.0F) {
                mover->target.z = STOP_Z;
            } else {
                mover->target.z = -STOP_Z;
            }
        }
        ++work->submode;
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
            sound_stop(MECH_BOSS_SOUND_HOVER, task);
            if (CHARGES(record) == 0) {
                work->mode = MODE_STOP;
                work->submode = 0;
            } else {
                work->flags |= FDOG_LASER;
                work->mode = MODE_CHARGE;
                work->submode = 0;
            }
        }
        break;
    }
    mover->acc.x += njSin(work->ang.y) * 0.11F;
    mover->acc.z += njCos(work->ang.y) * 0.11F;
    mover->spd.x *= 0.94F;
    mover->spd.z *= 0.94F;
    fdog_steer(task, 0.075F);
    fdog_aim_camera(task);
}

static struct fdog_launch fdog_launches[MISSILE_SLOTS] = {
    {0, 0}, {0, 2}, {1, 0}, {1, 2}, {0, 1}, {0, 3},
    {1, 1}, {1, 3}, {0, 0}, {0, 2}, {1, 0}, {1, 2}};

/* fly in toward STOP_X, then brake and lean. open the hatch, and load a
 * missile into each of the MISSILE_SLOTS. with one hit left, load none, and
 * choose again */
static void fdog_stop(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct mover *mover = task->motion.p;
    struct motion_control *control = record->block;
    struct fdog_parts *parts = record->own;

    /* @hack redundant: two stores to pos.x, of 0 and 1, that no path reaches */
    if (0) {
        work->pos.x = 0.0F;
        work->pos.x = 1.0F;
    }
    switch (work->submode) {
    case 0:
        if (work->pos.x > 0.0F) {
            mover->target.x = STOP_X;
        } else {
            mover->target.x = -STOP_X;
        }
        if (player_work[0]->pos.z > STOP_Z) {
            mover->target.z = STOP_Z;
        } else if (player_work[0]->pos.z < -STOP_Z) {
            mover->target.z = -STOP_Z;
        } else {
            mover->target.z = player_work[0]->pos.z;
        }
        work->flags &= ~FDOG_PITCHED;
        ++work->submode;
    case 1:
        mover->acc.x += njSin(work->ang.y) * 0.12F;
        mover->acc.z += njCos(work->ang.y) * 0.12F;
        mover->spd.x *= 0.97F;
        mover->spd.z *= 0.97F;
        /* @hack spelling: the steer named through its second name, where
         * its own name would do */
        fdog_steer_outside(task, 0.068F);
        if (njAbs(work->pos.x) < APPROACH_X) {
            work->collision->shapes[LOW_SHAPE].flags &= ~SHAPE_OFF;
            ++work->submode;
        }
        break;
    case 2: {
        /* @hack spelling: gain, a local for 0.07, where 0.07F would do */
        Float gain = 0.07F;
        mover->acc.x += njSin(work->ang.y) * gain;
        /* @bug logic: accelerates along z by the heading's sine, at
         * 0x8c501a9c, where case 1 takes its cosine, at 0x8c501a1e */
        mover->acc.z += njSin(work->ang.y) * gain;
        mover->spd.x *= 0.96F;
        mover->spd.z *= 0.96F;
        /* @hack spelling: the steer named through its second name, where
         * its own name would do */
        fdog_steer_outside(task, 0.068F);
        if (njAbs(work->pos.x) < STOP_X) {
            mover->acc.x = 0.0F;
            mover->acc.z = 0.0F;
            mover->aim_y = mover_player_heading(task, 0);
            work->flags &= ~FDOG_PITCHED;
            mover->aim_x = work->ang.x - STOP_LEAN;
            ++work->submode;
        }
    } break;
    case 3:
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
        if (!(work->flags & FDOG_PITCHED)) {
            work->ang.x = angle_approach(work->ang.x, mover->aim_x, LEAN_SPEED);
            if (!angle_difference(work->ang.x, mover->aim_x)) {
                mover->aim_x = 0;
                work->flags |= FDOG_PITCHED;
            }
        } else {
            work->ang.x = angle_approach(work->ang.x, mover->aim_x, LEAN_SPEED);
            if (!angle_difference(work->ang.x, mover->aim_x)) {
                if (record->power > 1.0F) {
                    /* @hack type: i a Sint8, where an int would do */
                    Sint8 i;
                    struct fdog_launch *launch;
                    MISSILES(record) = MISSILE_SLOTS;
                    /* @hack spelling: a do-while under a test of the count,
                     * where a for would do */
                    i = 0;
                    launch = fdog_launches;
                    if (MISSILES(record) > 0) {
                        do {
                            NJS_POINT3 *position;
                            if (!launch->bank) {
                                position = &fdog_pods_0[launch->index];
                            } else {
                                position = &fdog_pods_1[launch->index];
                            }
                            parts->missiles[i] = mech_boss_missile_create(
                                task, position, &work->ang, MISSILE_RADIUS);
                            ++launch;
                            ++i;
                        } while (i < MISSILES(record));
                    }
                    motion_link(control, MOTION_OPEN);
                    sound_play_at(MECH_BOSS_SOUND_HATCH, NULL, 0,
                                  MECH_BOSS_HATCH_VOLUME, &work->pos);
                    work->timer = fdog_motions[MOTION_OPEN].link_frames;
                    work->flags |= FDOG_MISSILES;
                } else {
                    work->timer = 1;
                }
                ++work->submode;
            }
        }
        break;
    case 4:
        work->ang.x = angle_approach(work->ang.x, 0, 256);
        work->ang.z = angle_approach(work->ang.z, 0, 256);
        if (--work->timer == 0) {
            work->ang.z = 0;
            if (record->power > 1.0F) {
                work->mode = MODE_FIRE;
            } else {
                SHOTS(record) = 3;
                work->mode = MODE_CHOOSE;
            }
            work->submode = 0;
            work->tick = 0;
            work->timer = 0;
        }
        break;
    }
    fdog_aim_camera(task);
}

/* launch the missiles at player 0 in volleys. then close the hatch, turning
 * toward the player */
static void fdog_fire(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct mover *mover = task->motion.p;
    struct motion_control *control = record->block;
    struct fdog_parts *parts = record->own;

    switch (work->submode) {
    case 0:
        ++work->submode;
    case 1:
        work->timer = 1;
        VOLLEY_MISSILES(mover) = MISSILES(record);
        work->collision->shapes[LOW_SHAPE].flags &= ~SHAPE_OFF;
        ++work->submode;
        break;
    case 2:
        if (--work->timer == 0) {
            /* yaw turns the curve's middle by the turn to player 0. it adds
             * up to 2048 either way at random, since rand() runs to 32767 */
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
                motion_link(control, MOTION_CLOSE);
                work->flags &= ~FDOG_MISSILES;
                work->timer = fdog_motions[MOTION_CLOSE].link_frames;
                sound_play_at(MECH_BOSS_SOUND_HATCH, NULL, 0,
                              MECH_BOSS_HATCH_VOLUME, &work->pos);
                ++work->submode;
            } else {
                work->submode = 1;
            }
        }
        break;
    case 4:
        mover_turn_to_player_by(task, 0x200, 0);
        if (--work->timer == 0) {
            work->mode = MODE_FACE_PLAYER;
            work->submode = 0;
            work->tick = 0;
            work->timer = 0;
            work->collision->shapes[LOW_SHAPE].flags |= SHAPE_OFF;
        }
        break;
    }
    fdog_aim_camera(task);
}

/* turn toward player 0 by up to FACE_TURN a step. the turn lasts FACE_STEPS,
 * or LATE_FACE_STEPS with three hits or fewer left. then rest FACE_REST_STEPS
 * once the motion ends, and choose again. the turn starts once the heading
 * to the player lies more than FACE_SLACK from -QUARTER_TURN */
static void fdog_face_player(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct motion_control *control = record->block;

    switch (work->submode) {
    case 0:
        if (record->power > 3.0F) {
            work->timer = FACE_STEPS;
        } else {
            work->timer = LATE_FACE_STEPS;
        }
        work->collision->shapes[LOW_SHAPE].flags &= ~SHAPE_OFF;
        ++work->submode;
        break;
    case 1: {
        Angle diff = mover_player_heading(task, 0) + QUARTER_TURN;
        if (diff < -FACE_SLACK || diff > FACE_SLACK) {
            ++work->submode;
        }
    } break;
    case 2:
        mover_turn_to_player_by(task, FACE_TURN, 0);
        if (--work->timer == 0) {
            ++work->submode;
        }
        break;
    case 3:
        if (motion_ended(control)) {
            work->timer = FACE_REST_STEPS;
            ++work->submode;
        }
        break;
    case 4:
        if (--work->timer == 0) {
            work->collision->shapes[LOW_SHAPE].flags |= SHAPE_OFF;
            work->mode = MODE_CHOOSE;
            work->submode = 0;
            work->tick = 0;
            work->timer = 0;
        }
        break;
    }
    fdog_aim_camera(task);
}

/* reel from a hit, and close the hatch where it is open. the missiles still
 * riding their pods are lost. after the first hit, choose again. after a
 * later one, fly on the way the Flying Dog was heading, out past
 * FDOG_ARENA_HALF_X. then turn back, fly in and lean as a stop does, and choose
 * again */
static void fdog_hit(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct mover *mover = task->motion.p;
    struct motion_control *control = record->block;
    struct fdog_parts *parts = record->own;

    switch (work->submode) {
    case 0:
        motion_link(control, MOTION_HIT);
        work->flags |= WORK_HURT;
        work->timer = 90;
        work->tick = 0;
        vibration_start(0, MECH_BOSS_RUMBLE_POWER, MECH_BOSS_RUMBLE_FREQ,
                        MECH_BOSS_RUMBLE_TIME);
        if (mover->spd.x == 0.0F) {
            if (work->pos.x > 0.0F) {
                work->flags &= ~FDOG_HEADING_POS_X;
            } else {
                work->flags |= FDOG_HEADING_POS_X;
            }
        } else if (mover->spd.x > 0.0F) {
            work->flags |= FDOG_HEADING_POS_X;
        } else {
            work->flags &= ~FDOG_HEADING_POS_X;
        }
        mover->spd.x = 0.0F;
        mover->spd.z = 0.0F;
        mover->acc.x = 0.0F;
        mover->acc.z = 0.0F;
        if (parts->laser) {
            task_kill(parts->laser);
            parts->laser = NULL;
        }
        if (parts->sight) {
            task_kill(parts->sight);
            parts->sight = NULL;
        }
        if (parts->beam_gun) {
            task_kill(parts->beam_gun);
            parts->beam_gun = NULL;
        }
        sound_play(MECH_BOSS_SOUND_HIT, NULL, 0, 0);
        sound_set_volume(NULL, MECH_BOSS_SOUND_HIT, MECH_BOSS_HIT_VOLUME);
        work->collision->shapes[LOW_SHAPE].flags |= SHAPE_OFF;
        ++work->submode;
    case 1:
        if (motion_held(control) && --work->timer == 0) {
            if (work->flags & FDOG_MISSILES) {
                work->timer = fdog_motions[MOTION_CLOSE].link_frames;
                motion_link(control, MOTION_CLOSE);
                work->flags &= ~FDOG_MISSILES;
            } else {
                work->timer = 1;
            }
            ++work->submode;
        }
        mover->sway_x += SWAY_X_STEP;
        mover->sway_y += SWAY_Y_STEP;
        mover->sway_z += SWAY_Z_STEP;
        break;
    case 2:
        if (--work->timer == 0) {
            /* @hack type: i a Sint8, where an int would do */
            Sint8 i;
            for (i = 0; i < MISSILE_SLOTS; ++i) {
                /* @hack spelling: mode, a local for the missile's mode, where
                 * the call tested in place would do */
                Sint8 mode = mech_boss_missile_mode(parts->missiles[i]);
                if (mode == MECH_BOSS_MISSILE_RIDING) {
                    task_kill(parts->missiles[i]);
                    parts->missiles[i] = NULL;
                }
            }
            fdog_end_hurt(task);
            /* @hack redundant: WORK_HURT cleared again, which fdog_end_hurt
             * has cleared */
            work->flags &= ~WORK_HURT;
            if (record->power >= FULL_POWER - 1.0F) {
                work->collision->shapes[LOW_SHAPE].flags |= SHAPE_OFF;
                work->mode = MODE_CHOOSE;
                work->submode = 0;
            } else {
                if (work->flags & FDOG_HEADING_POS_X) {
                    mover->target.x = FDOG_ARENA_HALF_X;
                } else {
                    mover->target.x = -FDOG_ARENA_HALF_X;
                }
                mover->target.z = 0.0F;
                ++work->submode;
            }
        }
        break;
    case 3:
        sound_play_at_for(MECH_BOSS_SOUND_ENGINE, MECH_BOSS_SECOND_KEY(task),
                          MECH_BOSS_SOUND_PRIORITY, CHARGE_VOLUME,
                          MECH_BOSS_ENGINE_SOUND_STEPS, &work->pos);
        mover->acc.x += njSin(work->ang.y) * 0.12F;
        mover->acc.z += njCos(work->ang.y) * 0.12F;
        mover->spd.x *= 0.97F;
        mover->spd.z *= 0.97F;
        fdog_steer(task, 0.075F);
        if (njAbs(work->pos.x) > FDOG_ARENA_HALF_X) {
            mover->target.x = player_work[0]->pos.x;
            if (player_work[0]->pos.z > 0.0F) {
                mover->target.z = STOP_Z;
            } else {
                mover->target.z = -STOP_Z;
            }
            ++work->submode;
        }
        break;
    case 4:
        if (work->pos.x > 0.0F) {
            if (mover->spd.x < 0.0F) {
                ++work->submode;
            }
        } else {
            if (mover->spd.x > 0.0F) {
                ++work->submode;
            }
        }
        mover->acc.x += njSin(work->ang.y) * 0.115F;
        mover->acc.z += njCos(work->ang.y) * 0.115F;
        mover->spd.x *= 0.94F;
        mover->spd.z *= 0.94F;
        fdog_steer(task, 0.075F);
        break;
    case 5:
        mover->acc.x += njSin(work->ang.y) * 0.115F;
        mover->acc.z += njCos(work->ang.y) * 0.115F;
        mover->spd.x *= 0.96F;
        mover->spd.z *= 0.96F;
        fdog_steer(task, 0.075F);
        if (njAbs(work->pos.x) < RETURN_X) {
            if (player_work[0]->pos.z > 0.0F) {
                mover->target.z = STOP_Z;
            } else {
                mover->target.z = -STOP_Z;
            }
            if (work->pos.x > 0.0F) {
                mover->target.x = njSqrt((STOP_RADIUS * STOP_RADIUS) -
                                         (mover->target.z * mover->target.z));
            } else {
                mover->target.x = -njSqrt((STOP_RADIUS * STOP_RADIUS) -
                                          (mover->target.z * mover->target.z));
            }
            work->flags &= ~FDOG_PITCHED;
            ++work->submode;
        }
    case 6:
        mover->acc.x += njSin(work->ang.y) * 0.12F;
        mover->acc.z += njCos(work->ang.y) * 0.12F;
        mover->spd.x *= 0.97F;
        mover->spd.z *= 0.97F;
        fdog_steer(task, 0.068F);
        if (njAbs(work->pos.x) < APPROACH_X) {
            work->collision->shapes[LOW_SHAPE].flags |= SHAPE_OFF;
            ++work->submode;
        }
        break;
    case 7:
        mover->acc.x += njSin(work->ang.y) * 0.07F;
        /* @bug logic: accelerates along z by the heading's sine, at
         * 0x8c50264e, as fdog_stop does at 0x8c501a9c */
        mover->acc.z += njSin(work->ang.y) * 0.07F;
        mover->spd.x *= 0.96F;
        mover->spd.z *= 0.96F;
        fdog_steer(task, 0.068F);
        if (njAbs(work->pos.x) < STOP_X) {
            mover->acc.x = 0.0F;
            mover->acc.z = 0.0F;
            mover->aim_y = mover_player_heading(task, 0);
            work->flags &= ~FDOG_PITCHED;
            mover->aim_x = work->ang.x - HIT_LEAN;
            ++work->submode;
        }
        break;
    case 8:
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
        work->ang.z = angle_approach(work->ang.z, 0, 256);
        if (!(work->flags & FDOG_PITCHED)) {
            work->ang.x = angle_approach(work->ang.x, mover->aim_x, LEAN_SPEED);
            if (!angle_difference(work->ang.x, mover->aim_x)) {
                mover->aim_x = 0;
                work->flags |= FDOG_PITCHED;
            }
        } else {
            work->ang.x = angle_approach(work->ang.x, mover->aim_x, LEAN_SPEED);
            if (!angle_difference(work->ang.x, mover->aim_x)) {
                work->ang.z = 0;
                work->mode = MODE_CHOOSE;
                work->submode = 0;
                work->tick = 0;
                work->timer = 0;
            }
        }
        break;
    }
    node_aim_return(parts->turret_aim);
    node_aim_return(parts->gun_aim);
    fdog_aim_camera(task);
}

/* the last hit landed. the Flying Dog sinks in bursts and smoke, clears the
 * stage, and poses for the winner */
static void fdog_fall(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct mover *mover = task->motion.p;
    struct motion_control *control = record->block;
    struct fdog_parts *parts = record->own;
    /* @hack spelling: phase, a local for the timer modulo the period, where
     * the modulo tested in place would do */
    Sint16 phase;

    switch (work->submode) {
    case 0:
        motion_link(control, MOTION_FALL);
        mover->spd.x = 0.0F;
        mover->spd.y = 0.0F;
        mover->spd.z = 0.0F;
        mover->acc.x = 0.0F;
        mover->acc.y = 0.0F;
        mover->acc.z = 0.0F;
        task_destroy_children(task);
        work->timer = 240;
        {
            struct collision_shape *shape = collision_get_shape(task, 0);
            shape->radius = 30.0F;
        }
        work->flags &= ~FDOG_PITCHED;
        ++work->submode;
    case 1:
        if (!(work->flags & FDOG_PITCHED)) {
            work->ang.x = angle_approach(work->ang.x, 0, 0x100);
            if (work->ang.x == 0) {
                work->flags |= FDOG_PITCHED;
            }
        }
        work->ang.z = angle_approach(work->ang.z, 0, 0x100);
        mover->acc.y -= 0.0075F;
        if (work->pos.y < 30.0F) {
            work->ang.x = angle_approach(work->ang.x, -0x1000, 0x80);
        }
        if (work->timer < FALL_SETTLE_STEPS) {
            mover->sway_x = angle_approach(mover->sway_x, 0, 0x80);
            mover->sway_y = angle_approach(mover->sway_y, 0, 0x80);
            mover->sway_z = angle_approach(mover->sway_z, 0, 0x80);
        } else {
            mover->sway_x += 2 * SWAY_X_STEP;
            mover->sway_y += 2 * SWAY_Y_STEP;
            mover->sway_z += 2 * SWAY_Z_STEP;
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
            if (!(work->flags & FDOG_CRASHED)) {
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
    case 2:
        ++work->timer;
        {
            NJS_POINT3 pos = {-85.0F, 0.0F, -40.0F};

            pos.y = work->pos.y;
            /* @hack spelling: the result compared with TRUE, where a test of
             * it would do */
            if (mech_boss_pose_for_winner(task, &pos) == TRUE) {
                work->ang.y = 0xa000;
            }
        }
        break;
    }
    phase = work->timer % 5;
    if (phase == 0) {
        if (work->submode != 2) {
            NJS_VECTOR dir = {0.0F, 1.0F, 0.0F};
            NJS_POINT3 pos;

            pos = fdog_body_pos;
            pos.x += 10.0F - (rand() * (20.0F * RAND_SCALE));
            pos.y += 20.0F - (rand() * (40.0F * RAND_SCALE));
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
            smoke.pos = fdog_body_pos;
            mech_boss_smoke_points(&smoke, points);
        }
    }
    mover->acc.y += LIFT;
    fdog_aim_camera(task);
    node_aim_return(parts->turret_aim);
    node_aim_return(parts->gun_aim);
}

/* follow player 0 with the sight. turn toward the sight, and aim the gun at
 * it for BEAM_AIM_STEPS before each shot. it fires one shot, or three with
 * one hit left. BEAM_END_STEPS after the last, set out on the charges */
static void fdog_beam(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct mover *mover = task->motion.p;
    struct fdog_parts *parts = record->own;

    switch (work->submode) {
    case 0:
        if (record->power > 1.0F) {
            SHOTS(record) = 1;
        } else {
            SHOTS(record) = 3;
        }
        work->timer = BEAM_AIM_STEPS;
        if (work->flags & FDOG_GLIDE_VIEW) {
            camera_remove_mode(0, camera_top(0));
            work->flags &= ~FDOG_GLIDE_VIEW;
        }
        mech_boss_camera_far(task, 0);
        parts->sight =
            mech_boss_sight_create(task, &player_work[0]->pos, SIGHT_SPEED);
        parts->beam_gun =
            mech_boss_beam_gun_create(task, &fdog_gun_pos, &work->ang);
        /* @bug memory: reads through parts->sight at 0x8c502cb8, null where
         * the heap couldn't give the sight. the shot reads through it again
         * at 0x8c502d06, and hands it to mech_boss_sight_lock at 0x8c502cfa,
         * untested. the beam also hides and kills it untested. fdog_hit and
         * fdog_step test it before they kill it */
        /* @bug memory: hands parts->beam_gun, null where the heap couldn't
         * give the gun, to mech_boss_beam_gun_fire at 0x8c502d0a and to
         * mech_boss_beam_gun_charge at 0x8c502d30, untested. the beam also
         * kills it untested. fdog_hit tests it before it kills it */
#ifdef NONMATCHING
        /* this side repairs the tags above: where the heap can't give the
         * sight or the gun, the beam kills the one it made and tries again
         * at its next step */
        if (parts->sight == NULL || parts->beam_gun == NULL) {
            if (parts->sight != NULL) {
                task_kill(parts->sight);
                parts->sight = NULL;
            }
            if (parts->beam_gun != NULL) {
                task_kill(parts->beam_gun);
                parts->beam_gun = NULL;
            }
            break;
        }
#endif
        work->collision->shapes[LOW_SHAPE].flags &= ~SHAPE_OFF;
        ++work->submode;
    case 1:
        mover->target = parts->sight->work->pos;
        mover_turn_to_target_by(task, 0x200);
        node_aim_point(task, parts->gun_aim, GUN_RANGE, &mover->target);
        if (--work->timer == 0) {
            ++work->submode;
        }
        break;
    case 2:
        mech_boss_beam_gun_fire(parts->beam_gun, &parts->sight->work->pos,
                                mech_boss_sight_lock(parts->sight));
        ++work->submode;
        break;
    case 3:
        if (--SHOTS(record) == 0) {
            work->timer = BEAM_END_STEPS;
            ++work->submode;
        } else {
            mech_boss_beam_gun_charge(parts->beam_gun);
            work->timer = BEAM_AIM_STEPS;
            work->submode = 1;
        }
        break;
    case 4:
        node_aim_return(parts->gun_aim);
        mech_boss_sight_hide_unlocked(parts->sight);
        if (--work->timer == 0) {
            task_kill(parts->beam_gun);
            parts->beam_gun = NULL;
            task_kill(parts->sight);
            parts->sight = NULL;
            if (work->flags & FDOG_GLIDE_VIEW) {
                camera_remove_mode(0, camera_top(0));
                work->flags &= ~FDOG_GLIDE_VIEW;
            }
            mech_boss_camera_near(0);
            node_aim_reset(parts->gun_aim);
            work->collision->shapes[LOW_SHAPE].flags |= SHAPE_OFF;
            work->mode = MODE_CHARGE;
            work->submode = 0;
            work->tick = 0;
            work->timer = 0;
        }
        break;
    }
}

/* the engine's pitch bend for each unit the Flying Dog comes nearer player 0
 * in a step */
static Sint32 fdog_pitch_scale = 2048;

/* the Flying Dog's step. a hit on its capsule throws player 0 and takes one
 * of its power. a hit counts only while the Flying Dog isn't hurt, and
 * outside MODE_HIT and MODE_FALL. the last hit beats it. player 0 touching
 * it is thrown too */
static void fdog_step(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct task_work *work = task->work;
    struct mover *mover = task->motion.p;
    struct motion_control *control = record->block;
    struct fdog_parts *parts = record->own;
    /* @hack spelling: zero, a local for 0, where 0.0F would do */
    Float zero;

    if (game_state.time_stopped) {
        return;
    }
    if (game_state.mini_event_running) {
        fdog_hover(task);
        return;
    }
    /* @hack redundant: a store of 120 to the timer that no path reaches */
    if (0) {
        work->timer = 120;
    }
    zero = 0.0F;
    /* the hover's sound fades as the Flying Dog speeds up. the engine's sound
     * grows with its speed across the ground. the engine's pitch rises as the
     * Flying Dog comes nearer player 0 */
    if (!(work->flags & FDOG_CRASHED) && work->mode != MODE_FALL) {
        Float speed = njScalor(&mover->spd);
        Float distance = vector_distance(&work->pos, &player_work[0]->pos);
        Float ground_speed;
        Sint16 volume;
        int bend;

        volume = 15.0F - (speed * 10.0F);
        if (volume < -20) {
            volume = -20;
        }
        sound_play_at_for(MECH_BOSS_SOUND_HOVER, task, MECH_BOSS_SOUND_PRIORITY,
                          volume, MECH_BOSS_ENGINE_SOUND_STEPS, &work->pos);
        {
            /* @hack spelling: rate, a copy of zero, where zero would do */
            Float rate = zero;
            bend = (int)(rate * 8192.0F) - 8192;
        }
        if (bend > 16383) {
            bend = 16383;
        }
        sound_set_pitch(task, MECH_BOSS_SOUND_HOVER, bend);
        {
            Float x = mover->spd.x;
            Float z = mover->spd.z;
            /* @hack redundant: a test of x that no path reaches */
            if (0) {
                if (x) {
                    ;
                }
            }
            ground_speed = njSqrt((x * x) + (z * z));
        }
        volume = -127.0F + (ground_speed * 60.0F);
        if (volume > 120) {
            volume = 120;
        }
        if (volume > -100) {
            sound_play_at_for(MECH_BOSS_SOUND_ENGINE, task,
                              MECH_BOSS_SOUND_PRIORITY, volume,
                              MECH_BOSS_ENGINE_SOUND_STEPS, &work->pos);
            if (parts->distance != 0.0F) {
                Float delta = parts->distance - distance;
                bend = fdog_pitch_scale * delta;
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
        fdog_intro(task);
        break;
    case MODE_CHOOSE:
        fdog_choose(task);
        break;
    case MODE_CHARGE:
        fdog_charge(task);
        break;
    case MODE_COME_ROUND:
        fdog_come_round(task);
        break;
    case MODE_STOP:
        fdog_stop(task);
        break;
    case MODE_FACE_PLAYER:
        fdog_face_player(task);
        break;
    case MODE_FIRE:
        fdog_fire(task);
        break;
    case MODE_BEAM:
        fdog_beam(task);
        break;
    case MODE_HIT:
        fdog_hit(task);
        break;
    case MODE_FALL:
        fdog_fall(task);
        break;
    }
    if (work->mode != MODE_FALL) {
        mover->acc.y += LIFT;
        mover->sway_x += SWAY_X_STEP;
        mover->sway_y += SWAY_Y_STEP;
        mover->sway_z += SWAY_Z_STEP;
    }
    motion_step(control);
    mover_step(task);
    if (work->pos.z > LIMIT_Z) {
        work->pos.z = LIMIT_Z;
    } else if (work->pos.z < -LIMIT_Z) {
        work->pos.z = -LIMIT_Z;
    }
    if (work->pos.y < CRASH_HEIGHT) {
        /* @hack spelling: the mover read again from task, where mover
         * would do */
        struct mover *again = task->motion.p;
        if (again->spd.y < 0.0F) {
            work->pos.y = CRASH_HEIGHT;
            again->spd.y = zero;
        }
        if (!(work->flags & FDOG_CRASHED)) {
            struct mech_boss_ring spec = {
                15, 10.0F, 12.0F, {0.1F, 0.02F, 0.1F}};
            mech_boss_ring(&work->pos, &spec);
            mech_boss_camera_shake(&work->pos, MECH_BOSS_SHAKE_HEAVY);
            sound_play_for(MECH_BOSS_SOUND_CRASH, task,
                           MECH_BOSS_SOUND_PRIORITY, 0,
                           MECH_BOSS_CRASH_SOUND_STEPS);
            sound_set_volume(task, MECH_BOSS_SOUND_CRASH,
                             MECH_BOSS_CRASH_VOLUME);
            work->flags |= FDOG_CRASHED;
            vibration_start(0, MECH_BOSS_CRASH_POWER, MECH_BOSS_CRASH_FREQ,
                            MECH_BOSS_CRASH_TIME);
        }
    }
    if (work->mode == MODE_FALL) {
        fdog_place_shapes(task);
        return;
    }
    if (record->power == 1.0F) {
        sound_play_repeating(MECH_BOSS_SOUND_UNK_19, task,
                             MECH_BOSS_SOUND_PRIORITY, 0, LAST_HIT_SOUND_STEPS);
    }
    if (fdog_is_flying(task)) {
        if (player_work[0]->mode == PLAYER_MODE_HUNTER_GLIDE &&
            !(work->flags & FDOG_GLIDE_VIEW)) {
            work->flags |= FDOG_GLIDE_VIEW;
            camera_set_mode(0, camera_top(0) + 1, CAMERA_MODE_KNUKLES);
            /* @bug logic: sets the adjust of the slot above the glide's mode,
             * not of the mode. camera_set_mode puts the mode in the slot
             * above camera 0's top, at 0x8c503216, and the mode becomes the
             * top. camera_top(0) + 1 then names the slot above it, which
             * camera_set_adjust refuses, at 0x8c503226. so the glide's mode
             * keeps the adjust it copied from the mode below */
            camera_set_adjust(0, camera_top(0) + 1, CAMERA_ADJUST_RELATIVE1);
        } else if (player_work[0]->mode != PLAYER_MODE_HUNTER_GLIDE &&
                   (work->flags & FDOG_GLIDE_VIEW)) {
            work->flags &= ~FDOG_GLIDE_VIEW;
            camera_remove_mode(0, camera_top(0));
        }
    }
    fdog_jets(task, NULL);
    fdog_place_capsule(task);
    if (collision_hurt(parts->capsule) && !(work->flags & WORK_HURT)) {
        NJS_VECTOR spd;
        struct angles ang = {0, 0, 0};

        spd.x = MECH_BOSS_THROW_X;
        spd.y = MECH_BOSS_THROW_Y;
        spd.z = zero;
        ang.y = work->ang.y + HALF_TURN;
        player_launch_speed(0, &spd, &ang, MECH_BOSS_LAUNCH_LOCK_STEPS);
        if (work->mode != MODE_HIT && work->mode != MODE_FALL) {
            record->power--;
            mech_boss_guard_all(task);
            mech_boss_guard_all(parts->capsule);
            /* @hack redundant: a test of mover that no path reaches */
            if (0) {
                if (mover) {
                    ;
                }
            }
            if (parts->sight) {
                task_kill(parts->sight);
                parts->sight = NULL;
            }
            if (parts->laser) {
                task_kill(parts->laser);
                parts->laser = NULL;
            }
            if (record->power > 0.0F) {
                work->mode = MODE_HIT;
            } else {
                work->flags &= ~WORK_HURT;
                if (work->flags & FDOG_GLIDE_VIEW) {
                    work->flags &= ~FDOG_GLIDE_VIEW;
                    camera_remove_mode(0, camera_top(0));
                }
                if (work->mode == MODE_BEAM) {
                    mech_boss_camera_near(0);
                }
                work->mode = MODE_FALL;
            }
            work->submode = 0;
            work->tick = 0;
            work->timer = 0;
        }
    } else if (collision_next_player(task)) {
        NJS_VECTOR spd;
        struct angles ang = {0, 0, 0};

        spd.x = -3.0F;
        spd.y = 1.0F;
        spd.z = zero;
        ang.y = player_work[0]->ang.y + HALF_TURN;
        player_launch_speed(0, &spd, &ang, MECH_BOSS_LAUNCH_LOCK_STEPS);
    }
    if (work->flags & WORK_HURT) {
        mech_boss_count(&work->tick, MECH_BOSS_BLINK_STEPS);
    }
    fdog_place_shapes(task);
}

/* draw the body's shadow, as a modifier volume */
static void fdog_draw_shadow(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: the reads of record, parts and mover, which nothing
     * uses */
    struct boss_record *record = task->extra.p;
    struct fdog_parts *parts = record->own;
    /* @hack redundant: unused, 156 bytes that nothing reads, which size the
     * frame */
    NJS_POINT3 unused[13];
    struct mover *mover = task->motion.p;

    /* @bug logic: leaves DRAW_CONTROL_SHADOW set after the set at
     * 0x8c503472. fdog_draw clears it after its own set, at 0x8c5038ce */
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateV(NULL, &fdog_body_pos);
    njRotateY(NULL, work->ang.y + QUARTER_TURN);
    njCnkModDrawObject(&mech_boss_body_shadow);
    njPopMatrixEx();
}

/* the motion callback. keep where the pods, the jets and four of the nodes it
 * draws lie. it keeps each in the world, and some in the body's frame too */
static void fdog_find_points(NJS_CNK_OBJECT *object)
{
    NJS_POINT3 point;
    NJS_POINT3 origin = {0.0F, 0.0F, 0.0F};

    if (object == &fdog_body_node) {
        /* @hack spelling: pods, a local for mech_boss_pods_1_vertices, where
         * the array named in place would do */
        struct chunk_vertex_vn *pods = mech_boss_pods_1_vertices;

        njCalcPoint(NULL, &pods[0].point, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_pods_1[0]);
        njCalcPoint(NULL, &pods[1].point, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_pods_1[1]);
        njCalcPoint(NULL, &pods[2].point, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_pods_1[2]);
        njCalcPoint(NULL, &pods[3].point, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_pods_1[3]);
        /* @hack redundant: two tests of pods that no path reaches */
        if (0) {
            if (pods) {
                ;
            }
            if (pods) {
                ;
            }
        }
        njCalcPoint(NULL, &mech_boss_pods_0_vertices[0].point, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_pods_0[0]);
        njCalcPoint(NULL, &mech_boss_pods_0_vertices[1].point, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_pods_0[1]);
        njCalcPoint(NULL, &mech_boss_pods_0_vertices[2].point, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_pods_0[2]);
        njCalcPoint(NULL, &mech_boss_pods_0_vertices[3].point, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_pods_0[3]);
        njCalcPoint(NULL, &mech_boss_jet_vertices[0].point, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_jet_points[0]);
        njCalcPoint(NULL, &mech_boss_jet_vertices[1].point, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_jet_points[1]);
        njCalcPoint(NULL, &mech_boss_jet_vertices[2].point, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_jet_points[2]);
        njCalcPoint(NULL, &fdog_fast_jet_vertices[0].point, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_jet_points[3]);
        njCalcPoint(NULL, &fdog_fast_jet_vertices[1].point, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_jet_points[4]);
        njCalcPoint(NULL, &fdog_fast_jet_vertices[2].point, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_jet_points[5]);
        njCalcPoint(NULL, &fdog_fast_jet_vertices[3].point, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_jet_points[6]);
        njCalcPoint(NULL, &fdog_fast_jet_vertices[4].point, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_jet_points[7]);
        njCalcPoint(NULL, &fdog_fast_jet_vertices[5].point, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_jet_points[8]);
        njCalcPoint(NULL, &origin, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_body_pos);
        njCalcPoint(&fdog_view_to_body, &point, &fdog_body_local);
    }
    if (object == &fdog_turret_node) {
        njCalcPoint(NULL, &origin, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_turret_pos);
    }
    if (object == &fdog_laser_node) {
        njCalcPoint(NULL, &origin, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_laser_pos);
    }
    if (object == &fdog_gun_node) {
        njCalcPoint(NULL, &origin, &point);
        njCalcPoint(&fdog_view_to_world, &point, &fdog_gun_pos);
    }
}

/* keep the matrix from the view to the world, and have the draw call
 * fdog_find_points at each node */
static void fdog_start_points(struct task *task)
{
    /* @hack redundant: the reads of record, parts and mover, which nothing
     * uses */
    struct boss_record *record = task->extra.p;
    struct fdog_parts *parts = record->own;
    struct mover *mover = task->motion.p;

    njGetMatrix(&fdog_view_to_world);
    njInvertMatrix(&fdog_view_to_world);
    njCnkSetMotionCallback(fdog_find_points);
}

/* draw the Flying Dog, swayed. it draws in MECH_BOSS_HURT_LIGHT for the first
 * half of each hurt blink. it then hands Ninja back the stage's light, the
 * one light_select_after_draw set */
static void fdog_draw(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct task_work *work = task->work;
    struct mover *mover = task->motion.p;
    struct motion_control *control = record->block;

    njSetTexture(&grobo_texlist);
    njPushMatrixEx();
    fdog_start_points(task);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njTranslate(NULL, 0.0F,
                MECH_BOSS_PIVOT_HEIGHT +
                    (njSin(mover->sway_y) * DRAW_BOB_HEIGHT),
                0.0F);
    njRotateZ(NULL,
              work->ang.z + (Angle)(njSin(mover->sway_z) * DRAW_SWAY_Z_ANGLE));
    njRotateX(NULL,
              work->ang.x + (Angle)(njSin(mover->sway_x) * DRAW_SWAY_X_ANGLE));
    njTranslate(NULL, 0.0F, -MECH_BOSS_PIVOT_HEIGHT, 0.0F);
    njGetMatrix(&fdog_view_to_body);
    njInvertMatrix(&fdog_view_to_body);
    njRotateY(NULL, QUARTER_TURN);
    if ((work->flags & WORK_HURT) && work->tick < MECH_BOSS_BLINK_STEPS / 2) {
        light_apply(MECH_BOSS_HURT_LIGHT);
    }
    draw_control_set(DRAW_CONTROL_SHADOW);
    motion_draw_simple(&fdog_object, control);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    if ((work->flags & WORK_HURT) && work->tick < MECH_BOSS_BLINK_STEPS / 2) {
        light_apply(light_after_draw);
    }
    njCnkSetMotionCallback(NULL);
    njPopMatrixEx();
    fdog_draw_shadow(task);
}
