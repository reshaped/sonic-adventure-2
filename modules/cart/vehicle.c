/* the karts: each racer's step and draw. the step drives a player's kart by its
 * pad, and a CPU kart toward its gates */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "camera/helpers.h"
#include "camera/probe.h"
#include "cart/kart_art.h"
#include "cart/course.h"
#include "cart/objects.h"
#include "cart/pads.h"
#include "cart/race.h"
#include "cart/track.h"
#include "cart/vehicle.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "effect/powder.h"
#include "draw/lines.h"
#include "shared/dust.h"
#include "player/player.h"
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

#ifdef NONMATCHING
/* port_stop, by which this side stops the game where the heap has no kart */
#include "engine/dc/crash.h"
#endif

#define KART_PAD(kart) cart_pad_history[cart_pad_frame][(kart)->player]
/* RADIANS(radians), but by a Float product */
#define KART_ANGLE(radians) ((Angle)((radians) * ANGLE_PER_RADIAN_F))
enum vehicle_mode {
    DRIVE,
    STRAIGHTEN,
    ENTER_DRIFT,
    DRIFT,
    SPIN_OUT,
    STOPPED,
    DEBUG_MOVE,
    CPU_DRIVE,
    STORY_CLEAR,
    STORY_APPROACH,
    FINISHED
};
/* DEBUG_MOVE moves the kart by the pad, DEBUG_SPEED a step */
#define DEBUG_SPEED 3.0F
/* the bits of a kart's flags, which only check_motion_flags reads, to no
 * effect. DRIFT_READY says a drift may start */
#define DRIFT_READY 1
#define SPINNING 2
#define DRIFTING 4

/* a kart beyond DRAW_RANGE of the camera isn't drawn. one beyond NEAR_RANGE
 * draws its second model, where it has one */
#define DRAW_RANGE 3000.0F
#define NEAR_RANGE 200.0F
/* the model and the lamps stand at MODEL_Y from the kart's position along y.
 * a player's marker stands MARKER_RISE above the model, on the other player's
 * screen. the shadow stands SHADOW_LIFT above shadow_height */
#define MODEL_Y (-5.0F)
#define MARKER_RISE 30
#define MARKER_SCALE 10
#define SHADOW_LIFT 0.01F
/* the download kart's shadow is shadow_object widened DOWNLOAD_SHADOW_SCALE
 * times along x and z */
#define DOWNLOAD_SHADOW_SCALE 5
/* a story race loops near its end. a kart whose gate passes
 * cart_gate_count - LOOP_END_GATES moves back LOOP_LENGTH along x. its gate
 * goes back to cart_gate_count - LOOP_START_GATES. under MISSION_RINGS the
 * kart moves to gate 0 instead, at (RESTART_X, RESTART_Y, RESTART_Z) */
#define LOOP_END_GATES 20
#define LOOP_START_GATES 25
#define LOOP_LENGTH 3200.0F
#define RESTART_X 30.0F
#define RESTART_Y 6.0F
#define RESTART_Z (-50.0F)
/* a CPU kart drives toward one of a gate's points, its lane. it starts
 * toward START_LANE, where a fallen player's kart comes back too */
#define START_LANE 1

/* a kart that lands after more than LANDING_STEPS steps in the air raises
 * dust of DUST_SCALE. a player's kart also plays LAND_SOUND */
#define PROBE_RADIUS 6.0F
#define LANDING_STEPS 30
#define DUST_SCALE 3.0F
#define LAND_SOUND 26
/* a wall is a surface whose normal's y lies within WALL_SLOPE of 0. it
 * pushes a player's kart WALL_PUSH along its normal, and plays CRASH_SOUND. a
 * wall after WALL_COOLDOWN steps with none plays a story voice, and under
 * MISSION_TIMED ends the race */
#define WALL_SLOPE 0.2F
#define WALL_PUSH 3.0F
#define WALL_COOLDOWN 180
#define CRASH_SOUND 18
#define TAILS_WALL_VOICE 0x687
#define ROUGE_WALL_VOICE 0x50b
/* a push keeps PUSH_DECAY of itself a step. the kart tilts toward the road's
 * slope by TILT_STEP a step */
#define PUSH_DECAY 0.9F
#define TILT_STEP ((Angle)ANGLE_PER_DEGREE)
/* the wheels turn once in each WHEEL_TRAVEL units driven */
#define WHEEL_TRAVEL 18
#define WHEEL_SPIN ((Float)FULL_TURN / WHEEL_TRAVEL)
/* the brake lets go after AIR_BRAKE_STEPS steps in the air. a kart slower
 * than FACING_SPEED along both x and z keeps its heading */
#define AIR_BRAKE_STEPS 10
#define FACING_SPEED 0.5F

/* DRIVE steers by the mean of STEER_SAMPLES records of the stick, as
 * sum_stick takes them. at a full stick of 127, DRIVE turns the kart a degree
 * a step. STRAIGHTEN turns it a degree and a half, and DRIFT two */
#define STEER_SAMPLES 10
#define DRIVE_STEER 1.4334209F
#define STRAIGHTEN_STEER 2.1501312F
#define DRIFT_STEER 2.8668418F
/* DRIVE sets steering_timer to DRIFT_WINDOW at each step without the
 * accelerator. it counts the timer down at each step with it. while the timer
 * is above 0, a kart above drift_min_speed starts a drift. it takes the
 * accelerator held and the stick past DRIFT_STICK. so a drift starts only
 * within DRIFT_WINDOW steps of a press of the accelerator */
#define DRIFT_WINDOW 10
#define DRIFT_STICK 20
/* in the air the kart pitches AIR_PITCH_STEP a step, up to EIGHTH_TURN in
 * DRIVE. on the road it levels by LEVEL_STEP a step */
#define AIR_PITCH_STEP ((Angle)(0.5F * ANGLE_PER_DEGREE))
#define LEVEL_STEP ((Angle)(2 * ANGLE_PER_DEGREE))
/* the body leans against its change of velocity in its own frame. the change is
 * the latest velocity less the mean of the last MEAN_SAMPLES. it rolls
 * ROLL_LEAN a unit of the change along x. it pitches PITCH_LEAN a unit of the
 * change along z. while the change along x passes LEAN_LIMIT, it pitches
 * LEAN_PITCH instead */
#define MEAN_SAMPLES 30
#define PITCH_LEAN 256.0F
#define ROLL_LEAN 512.0F
#define LEAN_LIMIT 2.5F
#define LEAN_PITCH 640
/* STRAIGHTEN halves the body's yaw each step, until it lies within STRAIGHT
 * of 0 */
#define STRAIGHT ((Angle)(3 * ANGLE_PER_DEGREE))
/* ENTER_DRIFT turns the body by drift_entry_turns, one a step for its
 * ENTRY_STEPS steps. a slide without the accelerator keeps SLIDE_DRAG of its
 * speed a step. a drift grips again after GRIP_STEPS steps in a row sliding
 * sideways slower than GRIP_SLIP. it grips at once below half its
 * drift_min_speed */
#define ENTRY_STEPS 5
#define SLIDE_DRAG 0.99F
#define GRIP_STEPS 20
#define GRIP_SLIP 1.0F
/* a kart whose speed along x and z falls by more than IMPACT_LOSS in a step
 * spins out. the spin starts at SPIN_START a step, and slows by SPIN_DECAY a
 * step. the spinning kart keeps SPIN_DRAG of its speed a step */
#define IMPACT_LOSS 2.5F
#define SPIN_START ((Angle)(10 * ANGLE_PER_DEGREE))
#define SPIN_DECAY 18
#define SPIN_DRAG 0.98F
#define SPIN_SOUND 15
#define TAILS_SPIN_VOICE 0x69d
#define ROUGE_SPIN_VOICE 0x529

/* a cleared story race's kart drives on CLEAR_SPEED a step along x. the
 * approach drives APPROACH_SPEED a step along -z, until its count of steps
 * passes APPROACH_STEPS */
#define CLEAR_SPEED 7.0F
#define APPROACH_SPEED 10.0F
#define APPROACH_STEPS 120
/* a CPU kart's top speed starts at CPU_SPEED. it adds SLOT_SPEED for each grid
 * slot it starts ahead of the last. it adds LAP_SPEED for each lap left after
 * the one it drives. it adds cart_speed_per_level times its place, which is 1
 * for the leader, and cart_speed_above_one behind the leader. cart_speed_scale
 * scales the sum */
#define CPU_SPEED 10.0F
#define SLOT_SPEED 0.5F
#define LAP_SPEED 0.2F
/* a CPU kart speeds up while its heading lies within ON_COURSE of the way to
 * its target. otherwise it slows, to no less than CPU_LEAST_SPEED. it turns by
 * CPU_TURN a step. once it crosses its target gate, or comes within GATE_REACH
 * of it, it takes the next. a draw of LANE_CHOICES values then picks its lane.
 * one value moves it up, another down, and the rest keep it */
#define ON_COURSE ((Angle)(2 * ANGLE_PER_DEGREE))
#define CPU_LEAST_SPEED 0.01F
#define CPU_TURN ((Angle)(6 * ANGLE_PER_DEGREE))
#define GATE_REACH 20.0F
#define LANE_CHOICES 5
/* a player's kart the CPU takes over heads for the gate TAKEOVER_GATES past its
 * own. its last gate is the one after its own. it keeps its speed, or takes
 * RESUME_SPEED where it stands still */
#define TAKEOVER_GATES 2
#define RESUME_SPEED 0.1F

/* light_positions holds the character karts' two head lamps from HEAD_LAMPS on.
 * their two tail lamps start at TAIL_LAMPS, and Rouge's story kart's two lamps
 * at ROUGE_LAMPS. a tail lamp shows red while the kart brakes. it shows at
 * TUNNEL_TAIL_ALPHA in a tunnel, and not at all otherwise */
#define HEAD_LAMPS 0
#define TAIL_LAMPS 2
#define ROUGE_LAMPS 4
#define LAMP_SCALE 0.1F
#define TUNNEL_TAIL_ALPHA 0.4F

/* a kart that falls below a race's course comes back RESPAWN_GATES gates
 * back, at once. it stands DROP_HEIGHT above where it last stood on the road,
 * or CPU_DROP_HEIGHT for a CPU kart. its last position stands DROP_FALL above
 * that, and its velocity keeps DROP_KEEP of its x and z. the fall sets
 * respawn_timer past RESPAWN_STEPS, the wait recover_fall tests. a player's
 * kart that falls in a story race restarts the stage instead. a story race's
 * respawn would set a gate that falls below 0 to STORY_FIRST_GATE */
#define RESPAWN_GATES 5
#define RESPAWN_STEPS 180
#define STORY_FIRST_GATE 1
#define DROP_HEIGHT 150.0F
#define CPU_DROP_HEIGHT 170.0F
#define DROP_FALL 10.0F
#define DROP_KEEP 0.01F
/* a player's kart readies a boost at BOOST_RINGS rings. a boost sets its
 * speed to BOOST_SPEED, and for BOOST_STEPS multiplies its acceleration and
 * cruise speed by BOOST_FACTOR. it throws POWDER_SPECKS of powder a step,
 * from POWDER_DROP below it */
#define BOOST_RINGS 20
#define BOOST_SPEED 20.0F
#define BOOST_STEPS 120
#define BOOST_FACTOR 1.5F
#define POWDER_SPECKS 5
#define POWDER_DROP 3.0F
#define INVINCIBLE_STEPS 1260
/* a kart that hits a car keeps CAR_HIT_KEEP of its velocity */
#define CAR_HIT_KEEP 0.8F
/* a player's pad vibrates at VIBRATION_POWER, or at CAR_HIT_POWER for a car.
 * the frequency is LAND_FREQ for a landing and BOOST_FREQ for a boost. a crash
 * into a wall or a car takes CRASH_FREQ */
#define VIBRATION_POWER 4
#define CAR_HIT_POWER 5
#define LAND_FREQ 20
#define CRASH_FREQ 30
#define BOOST_FREQ 50
/* a kart keys its engine's and its brakes' sounds by its task's address plus
 * the sound's id. each call keeps a sound SOUND_STEPS + 1 steps, so a call
 * each step holds it. the engine's pitch rises 1 << PITCH_SHIFT for each
 * whole unit of speed. the brakes sound while braking above BRAKE_SPEED */
#define ENGINE_SOUND 13
#define BRAKE_SOUND 16
#define SOUND_PRIORITY 1
#define SOUND_STEPS 1
#define KART_VOLUME (-20)
#define PITCH_SHIFT 8
#define BRAKE_SPEED 3.0F

static const char racer_name[16] = "cartExecuter";
static const NJS_VECTOR zero_velocity = {0};
static const char invincibility_music[] = "invncibl.adx";
Float cart_speed_scale = 0.8F;
Float cart_speed_per_level = 0.3F;
Float cart_speed_above_one = 0.1F;
struct cart_tuning cart_tunings[CART_MODELS] = {
    {0.07F, -0.07F, -0.05F, 10.0F, 0.4F, 1.0F, 1.05F, 7.0F, 1.8F, 15.0F},
    {0.07F, -0.08F, -0.06F, 12.0F, 0.2F, 1.0F, 1.05F, 7.0F, 1.8F, 15.0F},
    {0.06F, -0.05F, -0.02F, 14.0F, 0.25F, 1.5F, 1.05F, 7.0F, 1.0F, 16.0F},
    {0.07F, -0.07F, -0.03F, 13.0F, 0.3F, 1.0F, 1.05F, 7.0F, 1.8F, 16.0F},
    {0.12F, -0.08F, -0.04F, 12.0F, 0.2F, 1.0F, 1.05F, 7.0F, 1.5F, 15.5F},
    {0.055F, -0.08F, -0.05F, 13.5F, 0.4F, 1.0F, 1.05F, 7.0F, 1.8F, 16.0F},
    {0.06F, -0.05F, -0.02F, 14.0F, 0.25F, 1.5F, 1.05F, 7.0F, 1.0F, 16.0F},
    {0.12F, -0.08F, -0.04F, 12.0F, 0.2F, 1.0F, 1.05F, 7.0F, 1.5F, 15.5F},
    {0.07F, -0.07F, -0.03F, 12.0F, 0.3F, 1.0F, 1.05F, 7.0F, 1.5F, 15.0F}};
/* clang-format off */
static struct collision_shape racer_shape[] = {
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS,
     SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(1), SHAPE_HURTABLE, {0}, 10.0F, 0,
     0, 0, {0}}};
/* clang-format on */
static int driver_sounds[CART_MODEL_DOWNLOAD][CART_DRIVER_SOUNDS] = {
    {8, 9}, {6, 7}, {0, 1}, {4, 5}, {8, 9}, {10, 11}, {2, 3}, {6, 7}};
#ifdef NONMATCHING
/* this side keeps the datum in .data, where the image holds it. nothing writes
 * it, so GCC would fold it into the code or place it among the constants */
__attribute__((used, section(".data")))
#endif
static NJS_CNK_OBJECT *player_markers[CART_PLAYERS] = {
#include "cart/vehicle/player_markers.inc"
};
static struct angles drive_rotation = {0}, straighten_rotation = {0},
                     drift_rotation = {0};
static Angle drift_entry_turns[ENTRY_STEPS] = {364, 364, 364, 364, 364};
static struct angles spin_rotation = {0};
static Float shadow_height = -2.0F, shadow_scale = 1.0F;
static Sint32 approach_steps = 0;
#ifdef NONMATCHING
/* this side keeps the datum, in .data, as for player_markers */
__attribute__((used, section(".data")))
#endif
static NJS_POINT3 light_positions[6] = {
#include "cart/vehicle/light_positions.inc"
};
static Float unk_5f87a8[4] = {1.0F, 1.0F, 1.0F, 1.0F};
static NJS_TEXANIM light_cell[1] = {
#include "cart/vehicle/light_cell.inc"
};
static Float traffic_push = 0.1F;

Bool cart_boost_ready[CART_PLAYERS], cart_story_approach;
/* the stepping kart's tuning, which a boost raises for the step, and the
 * kart as the step began */
static struct cart_tuning tuning;
static struct cart_vehicle last_step;
static Uint8 unk_7e4dcc[8];
static NJS_VECTOR velocity_history[CART_PLAYERS][CART_HISTORY_STEPS];
static struct angles angle_history[CART_PLAYERS][CART_HISTORY_STEPS];
static Sint32 motion_frame[CART_PLAYERS];
static Sint32 stick_history[CART_PLAYERS][CART_HISTORY_STEPS];
static Sint32 stick_frame[CART_PLAYERS];

static void racer_step(struct task *task);
static void racer_draw(struct task *task);
static void racer_destroy(struct task *task);
static void init_racer(struct task *task);
static void start_cpu_drive(struct task *task);
static void probe_road(struct task *task);
static void resolve_road(struct task *task);
static void check_motion_flags(struct task *task);
static void read_pedals(struct task *task);
static void face_velocity(struct task *task);
static void drive(struct task *task);
static void straighten(struct task *task);
static void slide(struct task *task);
static void enter_drift(struct task *task);
static void drift(struct task *task);
static void spin_out(struct task *task);
static void rotate_vector(struct angles *angles, NJS_VECTOR *vector);
static void draw_shadow(struct task *task);
static void drive_clear(struct task *task);
static void drive_approach(struct task *task);
static void cpu_drive(struct task *task);
static void draw_lights(struct task *task);
static Bool use_distant_model(struct task *task);
static void check_impact(struct task *task);
static void recover_fall(struct task *task);
static void update_boost(struct task *task);
static void record_motion(int player, NJS_VECTOR *speed, struct angles *angles);
static void previous_velocity(int player, int steps_back, NJS_VECTOR *out);
static void record_stick(int player);
static int sum_stick(int player, int count);
static void inverse_rotate(struct angles *angles, NJS_VECTOR *src,
                           NJS_VECTOR *dest);
static void engine_sound(struct task *task);

/* @hack redundant: a call that does nothing, after each mode's motion and
 * before the engine's sounds */
static void do_nothing(struct task *task)
{
    struct cart_vehicle *kart = task->motion.p;
    (void)kart;
}

struct task *cart_create_racer(struct cart_start *start)
{
    struct task *task;
    struct cart_vehicle *kart;
    task = task_create(TASK_WORK, 2, racer_step, racer_name);
    task->draw = racer_draw;
    task->destructor = racer_destroy;
    kart = task->motion.p = syCalloc(1, sizeof(struct cart_vehicle));
    /* @bug memory: writes the kart's start through null where the heap can't
     * give one. racer_destroy tests task->motion at 0x8c501d9e, but the copy
     * at 0x8c501d56 writes through it unchecked */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the heap has no kart, it stops
     * the game on the crash screen, which shows HEAP and the task's name. a
     * racer without its kart can't race, and the race's steps read each
     * racer's kart */
    if (kart == NULL) {
        port_stop("HEAP", racer_name, 0);
    }
#endif
    kart->start = *start;
    init_racer(task);
    player_work[0] = task->work;
    collision_init(task, racer_shape, 1, COLLISION_ENEMY);
    return task;
}

static void racer_destroy(struct task *task)
{
    if (task->motion.p) {
        syFree(task->motion.p);
        task->motion.p = NULL;
    }
}

static void racer_draw(struct task *task)
{
    struct cart_vehicle *kart;
    struct task_work *work;
    NJS_CNK_OBJECT *wheel;
    NJS_VECTOR from_camera;
    kart = task->motion.p;
    work = &kart->previous;
    if (!kart->start.player_controlled) {
        work->pos = task->work->pos;
    }
    vector_diff(&work->pos, &camera_views[camera_current_index]->pos,
                &from_camera);
    if (njScalor2(&from_camera) > DRAW_RANGE * DRAW_RANGE) {
        return;
    }
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, work->pos.y + MODEL_Y, work->pos.z);
    if (kart->start.player_controlled && kart->driver != camera_current_index) {
        njPushMatrixEx();
        njSetTexture(&cart_race_textures);
        njTranslate(NULL, 0, MARKER_RISE, 0);
        njScale(NULL, MARKER_SCALE, MARKER_SCALE, MARKER_SCALE);
        njCnkEasyDrawObject(player_markers[kart->driver]);
        njPopMatrixEx();
    }
    if (kart->textures) {
        njSetTexture(kart->textures);
    }
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y + kart->yaw - HALF_TURN);
    njRotateZ(NULL, kart->body_tilt.z + kart->air_tilt.z);
    njRotateX(NULL, kart->body_tilt.x + kart->air_tilt.x);
    njRotateY(NULL, kart->body_tilt.y + kart->air_tilt.y);
    if (kart->start.model >= CART_MODEL_SONIC &&
        kart->start.model <= CART_MODEL_ROUGE) {
        wheel = kart->object->child;
        wheel->evalflags &= ~NJD_EVAL_UNIT_ANG;
        wheel->ang[0] = kart->wheel;
        wheel = kart->object->child->sibling;
        wheel->evalflags &= ~NJD_EVAL_UNIT_ANG;
        wheel->ang[0] = kart->wheel;
        wheel = kart->object->child->sibling->sibling;
        wheel->evalflags &= ~NJD_EVAL_UNIT_ANG;
        wheel->ang[0] = kart->wheel;
    }
    if (use_distant_model(task)) {
        njCnkSimpleDrawObject(kart->second);
    } else {
        njCnkSimpleDrawObject(kart->object);
    }
    njPopMatrixEx();
    switch (cart_setup.course_mode) {
    case CART_TAILS:
    case CART_ROUGE:
        break;
    default:
        draw_lights(task);
        break;
    }
    draw_shadow(task);
}

#define SURFACE_STEP(task)                                                     \
    do {                                                                       \
        probe_road(task);                                                      \
        resolve_road(task);                                                    \
    } while (0)
#define END_MOTION(task)                                                       \
    do {                                                                       \
        check_motion_flags(task);                                              \
        do_nothing(task);                                                      \
    } while (0)

static void racer_step(struct task *task)
{
    struct task_work *work;
    struct cart_vehicle *kart;
    NJS_VECTOR direction;
    Float speed;
    work = task->work;
    kart = task->motion.p;
    kart->previous = *work;
    last_step = *kart;
    kart->flags = 0;
    if (kart->driver >= 0) {
        record_stick(kart->driver);
    }
    if (cart_story_approach) {
        kart->mode = STORY_APPROACH;
    } else if (cart_clock_held) {
        SURFACE_STEP(task);
        recover_fall(task);
        END_MOTION(task);
        return;
    }
    tuning = cart_tunings[kart->start.model];
    if (kart->velocity.x != 0 || kart->velocity.y != 0 ||
        kart->velocity.z != 0) {
        direction = kart->velocity;
        speed = njUnitVector(&direction);
        if (speed > tuning.max_speed) {
            kart->velocity.x = direction.x * tuning.max_speed;
            kart->velocity.y = direction.y * tuning.max_speed;
            kart->velocity.z = direction.z * tuning.max_speed;
            last_step.velocity.x = direction.x * tuning.max_speed;
            last_step.velocity.y = direction.y * tuning.max_speed;
            last_step.velocity.z = direction.z * tuning.max_speed;
        }
    }
    switch (kart->mode) {
    case DRIVE:
        read_pedals(task);
        update_boost(task);
        drive(task);
        engine_sound(task);
        SURFACE_STEP(task);
        check_impact(task);
        recover_fall(task);
        END_MOTION(task);
        break;
    case STRAIGHTEN:
        read_pedals(task);
        update_boost(task);
        straighten(task);
        engine_sound(task);
        SURFACE_STEP(task);
        check_impact(task);
        recover_fall(task);
        END_MOTION(task);
        break;
    case ENTER_DRIFT:
        read_pedals(task);
        update_boost(task);
        enter_drift(task);
        engine_sound(task);
        SURFACE_STEP(task);
        check_impact(task);
        recover_fall(task);
        kart->flags |= DRIFTING;
        END_MOTION(task);
        break;
    case DRIFT:
        read_pedals(task);
        update_boost(task);
        drift(task);
        engine_sound(task);
        SURFACE_STEP(task);
        check_impact(task);
        recover_fall(task);
        kart->flags |= DRIFTING;
        END_MOTION(task);
        break;
    case SPIN_OUT:
        read_pedals(task);
        spin_out(task);
        engine_sound(task);
        SURFACE_STEP(task);
        recover_fall(task);
        kart->flags |= SPINNING;
        END_MOTION(task);
        break;
    case STOPPED:
        break;
    case CPU_DRIVE:
        cpu_drive(task);
        engine_sound(task);
        SURFACE_STEP(task);
        check_motion_flags(task);
        recover_fall(task);
        break;
    case STORY_CLEAR:
        drive_clear(task);
        engine_sound(task);
        SURFACE_STEP(task);
        check_motion_flags(task);
        break;
    case STORY_APPROACH:
        drive_approach(task);
        engine_sound(task);
        SURFACE_STEP(task);
        check_motion_flags(task);
        break;
    case FINISHED:
        break;
    case DEBUG_MOVE:
        point_move_by_pad(&work->pos, DEBUG_SPEED, kart->player, TRUE);
        do_nothing(task);
    }
    kart->wall_cooldown--;
    if (kart->wall_cooldown < 0) {
        kart->wall_cooldown = 0;
    }
    if (kart->invincible_steps) {
        kart->invincible_steps--;
        if (kart->invincible_steps == 0) {
            kart->invincible = 0;
        }
    }
    if (cart_setup.course_mode <= CART_ROUGE) {
        if (game_state.mission == MISSION_RINGS &&
            game_state.rings[0] >= RINGS_GOAL && kart->mode != FINISHED &&
            !cart_finished) {
            kart->mode = FINISHED;
            cart_finished = 1;
        }
        if (cart_race.countdown <= 0) {
            if (kart->mode != FINISHED) {
                kart->mode = FINISHED;
                cart_time_over();
            }
        } else if (kart->gate > cart_gate_count - LOOP_END_GATES) {
            if (game_state.mission == MISSION_RINGS) {
                work->pos.x = RESTART_X;
                work->pos.y = RESTART_Y;
                work->pos.z = RESTART_Z;
                kart->previous.pos = work->pos;
                kart->velocity.z = njScalor(&kart->velocity);
                kart->velocity.x = 0;
                kart->velocity.y = 0;
                work->ang.x = 0;
                work->ang.y = 0;
                work->ang.z = 0;
                kart->gate = 0;
            } else {
                kart->gate = cart_gate_count - LOOP_START_GATES;
                work->pos.x -= LOOP_LENGTH;
                if (!kart->chase_target) {
                    kart->chase_target = cart_create_chase_target(task);
                }
            }
        }
    }
    if (cart_finished) {
        if (kart->lap > CART_LAPS) {
            kart->lap = CART_LAPS;
            kart->furthest_lap = CART_LAPS;
        }
        if (kart->start.player_controlled) {
            start_cpu_drive(task);
        }
    } else if (cart_story_clear) {
        kart->mode = STORY_CLEAR;
    }
    collision_register(task);
}

static void init_racer(struct task *task)
{
    struct task_work *work;
    struct cart_vehicle *kart;
    struct cart_start *start;
    work = task->work;
    kart = task->motion.p;
    start = &kart->start;
    work->pos = start->pos;
    work->ang = start->ang;
    kart->object = cart_setup.models[start->model].object;
    kart->second = cart_setup.models[start->model].second;
    kart->textures = cart_setup.models[start->model].textures;
    kart->mode = DRIVE;
    kart->player = start->player;
    kart->previous.pos = work->pos;
    /* @hack redundant: a second store of DRIVE to mode */
    kart->mode = DRIVE;
    kart->yaw = 0;
    kart->velocity.x = 0;
    kart->velocity.y = 0;
    kart->velocity.z = 0;
    kart->body_tilt.x = 0;
    kart->body_tilt.y = 0;
    kart->body_tilt.z = 0;
    kart->invincible = 0;
    if (start->player_controlled) {
        kart->driver = start->player;
    } else {
        kart->driver = -1;
    }
    kart->gate = 0;
    kart->lap = 0;
    kart->furthest_lap = 0;
    kart->boost_rings = 0;
    kart->boost_steps = 0;
    kart->chase_target = NULL;
    kart->wall_cooldown = 0;
    kart->airborne_steps = 0;
    kart->unk_c8 = 0;
    kart->respawn_timer = 0;
    if (!kart->start.player_controlled) {
        kart->mode = CPU_DRIVE;
        kart->target_gate = kart->start.player + 1;
        kart->target_lane = START_LANE;
        kart->last_gate = kart->start.player;
        kart->last_lane = START_LANE;
        kart->cpu_speed = 0;
        kart->cpu_heading = start->ang.y - QUARTER_TURN;
    }
}

static void start_cpu_drive(struct task *task)
{
    struct cart_vehicle *kart;
    struct task_work *work;
    NJS_POINT3 position;
    NJS_POINT3 target;
    NJS_VECTOR toward;
    work = task->work;
    kart = task->motion.p;
    kart->mode = CPU_DRIVE;
    kart->target_gate = (kart->gate + TAKEOVER_GATES) % cart_gate_count;
    kart->target_lane = START_LANE;
    kart->last_gate = (kart->gate + 1) % cart_gate_count;
    kart->last_lane = START_LANE;
    if (kart->velocity.x == 0 && kart->velocity.y == 0 &&
        kart->velocity.z == 0) {
        kart->cpu_speed = RESUME_SPEED;
    } else {
        kart->cpu_speed = njScalor(&kart->velocity);
    }
    kart->yaw = 0;
    kart->start.player_controlled = 0;
    target.x = cart_gates[kart->target_gate][kart->target_lane].x;
    target.y = 0;
    target.z = cart_gates[kart->target_gate][kart->target_lane].z;
    position.x = work->pos.x;
    position.y = 0;
    position.z = work->pos.z;
    vector_diff(&target, &position, &toward);
    kart->cpu_heading = KART_ANGLE(atan2f(toward.z, toward.x));
}

static void probe_road(struct task *task)
{
    struct cart_vehicle *kart;
    struct task_work *work;
    struct task_work *previous;
    struct camera_probe *probe;
    kart = task->motion.p;
    work = task->work;
    previous = &kart->previous;
    probe = &kart->probe;
    probe->pos = previous->pos;
    probe->next = work->pos;
    probe->skip = 0;
    probe->radius = PROBE_RADIUS;
    kart->grounded = camera_probe_move(probe);
    if (!kart->grounded) {
        kart->airborne_steps++;
    } else {
        if (kart->airborne_steps > LANDING_STEPS) {
            dust_create(&work->pos, &kart->velocity, DUST_SCALE);
            if (kart->start.player_controlled) {
                sound_play(LAND_SOUND, NULL, 0, 0);
                vibration_start(kart->player, VIBRATION_POWER, LAND_FREQ,
                                VIBRATION_UNTIMED);
            }
        }
        kart->airborne_steps = 0;
    }
}

static void resolve_road(struct task *task)
{
    struct cart_vehicle *kart;
    struct task_work *work;
    struct camera_probe *probe;
    struct task_work *previous;
    struct angles tilt;
    kart = task->motion.p;
    work = task->work;
    probe = &kart->probe;
    previous = &kart->previous;
    work->pos = probe->hit;
    vector_diff(&work->pos, &previous->pos, &kart->velocity);
    /* @hack redundant: the test's empty else */
    if (kart->grounded) {
        /* @hack redundant: a second test of grounded, which the test around
         * it made */
        if (kart->driver >= 0 && kart->grounded &&
            _builtin_fabsf(probe->slides[0].normal.y) < WALL_SLOPE) {
            kart->push.x = probe->slides[0].normal.x * WALL_PUSH;
            kart->push.z = probe->slides[0].normal.z * WALL_PUSH;
            kart->velocity.y = 0;
            work->pos.x = probe->pos.x;
            work->pos.y = probe->pos.y;
            work->pos.z = probe->pos.z;
            sound_play(CRASH_SOUND, NULL, 0, 0);
            vibration_start(kart->player, VIBRATION_POWER, CRASH_FREQ,
                            VIBRATION_UNTIMED);
            if (kart->start.player_controlled && !kart->wall_cooldown) {
                switch (cart_setup.course_mode) {
                case CART_TAILS:
                    voice_play_priority_2(TAILS_WALL_VOICE);
                    break;
                case CART_ROUGE:
                    voice_play_priority_2(ROUGE_WALL_VOICE);
                    break;
                }
                if (game_state.mission == MISSION_TIMED) {
                    kart->mode = FINISHED;
                    cart_time_over();
                }
            }
            kart->wall_cooldown = WALL_COOLDOWN;
        }
        y_axis_angles(&probe->slides[0].normal, &tilt.x, &tilt.z);
        work->ang.x = angle_approach(work->ang.x, tilt.x, TILT_STEP);
        work->ang.z = angle_approach(work->ang.z, tilt.z, TILT_STEP);
    } else {
    }
    if (kart->driver >= 0) {
        vector_sum(&work->pos, &kart->push, &work->pos);
        kart->push.x *= PUSH_DECAY;
        kart->push.z *= PUSH_DECAY;
    }
    inverse_rotate(&work->ang, &kart->velocity, &kart->local_velocity);
    if (kart->driver >= 0) {
        record_motion(kart->driver, &kart->velocity, &work->ang);
    }
    kart->wheel += (Angle)(njScalor(&kart->velocity) * WHEEL_SPIN);
}

static void check_motion_flags(struct task *task)
{
    struct cart_vehicle *kart;
    NJS_VECTOR scaled;
    kart = task->motion.p;
    /* @hack redundant: a scaled copy of the velocity that nothing reads, and
     * tests of the flags and the steps with empty bodies */
    scaled = kart->velocity;
    scaled.x *= 0.95F;
    scaled.y *= 1.45F;
    scaled.z *= 0.95F;
    if (kart->flags & DRIFT_READY) {
    }
    if (kart->flags & SPINNING) {
    }
    if (kart->flags & DRIFTING) {
        if (game_state.steps & 3) {
        }
    }
}

static void read_pedals(struct task *task)
{
    struct cart_vehicle *kart = task->motion.p;
    if (KART_PAD(kart).on & PDD_DGT_TA) {
        kart->accelerating = 1;
    } else {
        kart->accelerating = 0;
    }
    if (KART_PAD(kart).on & (PDD_DGT_TX | PDD_DGT_TB)) {
        kart->braking = 1;
    } else {
        kart->braking = 0;
    }
    if (kart->airborne_steps > AIR_BRAKE_STEPS) {
        kart->braking = 0;
    }
}

static void face_velocity(struct task *task)
{
    struct cart_vehicle *kart;
    struct task_work *work;
    kart = task->motion.p;
    work = task->work;
    if (_builtin_fabsf(kart->velocity.z) < FACING_SPEED &&
        _builtin_fabsf(kart->velocity.x) < FACING_SPEED) {
        return;
    }
    work->ang.y =
        -KART_ANGLE(atan2f(kart->velocity.z, kart->velocity.x)) - QUARTER_TURN;
    work->ang.y &= ANGLE_MASK;
}

/* a kart that climbs the road falls by CLIMB_GRAVITY of its gravity */
#define CLIMB_GRAVITY 0.25F
#define APPLY_GRAVITY(kart, work, gravity)                                     \
    do {                                                                       \
        if ((kart)->velocity.y > 0 && (kart)->grounded) {                      \
            (work)->pos.y += (kart)->velocity.y - ((gravity) * CLIMB_GRAVITY); \
        } else {                                                               \
            (work)->pos.y += (kart)->velocity.y - (gravity);                   \
        }                                                                      \
    } while (0)

static void drive(struct task *task)
{
    struct task_work *work;
    struct cart_vehicle *kart;
    NJS_VECTOR forward;
    NJS_VECTOR sample;
    NJS_VECTOR mean;
    Float steer;
    Float speed;
    Float acceleration;
    Float ratio;
    Float gravity;
    int i;
    kart = task->motion.p;
    work = task->work;
    steer = sum_stick(kart->player, STEER_SAMPLES) * (1.0F / STEER_SAMPLES);
    kart->yaw = 0;
    forward.x = kart->velocity.x;
    forward.y = 0;
    forward.z = kart->velocity.z;
    if (forward.x == 0 && forward.z == 0) {
        speed = 0;
    } else {
        speed = njScalor(&forward);
    }
    forward.x = 0;
    forward.y = 0;
    forward.z = 0;
    ratio = (tuning.cruise_speed - speed) / tuning.cruise_speed;
    work->ang.y = work->ang.y - (Angle)(steer * DRIVE_STEER);
    if (kart->accelerating) {
        acceleration = tuning.acceleration;
        acceleration *= ratio;
        kart->steering_timer--;
    } else if (kart->braking) {
        acceleration = tuning.braking;
        kart->steering_timer = DRIFT_WINDOW;
    } else {
        acceleration = tuning.coasting;
        kart->steering_timer = DRIFT_WINDOW;
    }
    forward.z = speed + acceleration;
    forward.z *= -1.0F;
    drive_rotation.y = work->ang.y;
    rotate_vector(&drive_rotation, &forward);
    vector_sum(&work->pos, &forward, &work->pos);
    gravity = tuning.gravity;
    APPLY_GRAVITY(kart, work, gravity);
    if (kart->grounded) {
        kart->air_tilt.x = angle_approach(kart->air_tilt.x, 0, LEVEL_STEP);
    } else {
        kart->air_tilt.x += AIR_PITCH_STEP;
        if (kart->air_tilt.x > EIGHTH_TURN) {
            kart->air_tilt.x = EIGHTH_TURN;
        }
    }
    mean = zero_velocity;
    for (i = 1; i <= MEAN_SAMPLES; i++) {
        previous_velocity(kart->driver, i, &sample);
        vector_sum(&sample, &mean, &mean);
    }
    mean.x *= 1.0F / MEAN_SAMPLES;
    mean.y *= 1.0F / MEAN_SAMPLES;
    mean.z *= 1.0F / MEAN_SAMPLES;
    previous_velocity(kart->driver, 1, &sample);
    vector_diff(&sample, &mean, &mean);
    inverse_rotate(&work->ang, &mean, &mean);
    if (mean.x < -LEAN_LIMIT) {
        kart->body_tilt.x = LEAN_PITCH;
    } else if (mean.x > LEAN_LIMIT) {
        kart->body_tilt.x = -LEAN_PITCH;
    } else {
        kart->body_tilt.x = (Angle) - (mean.z * PITCH_LEAN);
    }
    kart->body_tilt.z = (Angle) - (mean.x * ROLL_LEAN);
    if (speed > tuning.drift_min_speed) {
        if (kart->steering_timer > 0) {
            kart->flags |= DRIFT_READY;
            if (kart->accelerating) {
                /* @hack type: the stick cast to int and then to Float, where
                 * Float alone would do */
                if (_builtin_fabsf((Float)(int)KART_PAD(kart).x1) >
                    DRIFT_STICK) {
                    kart->mode = ENTER_DRIFT;
                    kart->yaw = 0;
                    kart->drift_stick = KART_PAD(kart).x1;
                    kart->steering_timer = 0;
                }
            }
        }
    }
}

static void straighten(struct task *task)
{
    struct task_work *work;
    struct cart_vehicle *kart;
    struct cart_tuning *params;
    NJS_VECTOR forward;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_VECTOR unused;
    Float speed;
    Float acceleration;
    Float ratio;
    Float gravity;
    kart = task->motion.p;
    work = task->work;
    if (kart->velocity.z != 0 || kart->velocity.x != 0) {
        work->ang.y = -KART_ANGLE(atan2f(kart->velocity.z, kart->velocity.x)) -
                      QUARTER_TURN;
        if (angle_distance(kart->yaw, 0) < STRAIGHT) {
            kart->mode = DRIVE;
            kart->yaw = 0;
        } else {
            kart->yaw /= 2;
            work->ang.y += kart->yaw;
        }
    }
    /* @hack type: the stick cast to int, where its own Sint16 would do */
    work->ang.y -= (Angle)((int)KART_PAD(kart).x1 * STRAIGHTEN_STEER);
    forward.x = kart->velocity.x;
    forward.y = 0;
    forward.z = kart->velocity.z;
    if (forward.x == 0 && forward.z == 0) {
        speed = 0;
    } else {
        speed = njScalor(&forward);
    }
    forward.x = 0;
    forward.y = 0;
    forward.z = 0;
    /* @hack spelling: params, a pointer to tuning, where tuning itself would
     * do */
    params = &tuning;
    ratio = (params->cruise_speed - speed) / params->cruise_speed;
    if (kart->accelerating) {
        acceleration = params->acceleration * ratio;
    } else {
        acceleration = kart->braking ? params->braking : params->coasting;
    }
    acceleration += speed;
    forward.z = acceleration;
    forward.z *= -1.0F;
    straighten_rotation.y = work->ang.y;
    rotate_vector(&straighten_rotation, &forward);
    if (kart->grounded) {
        kart->air_tilt.x = angle_approach(kart->air_tilt.x, 0, LEVEL_STEP);
    } else {
        kart->air_tilt.x += AIR_PITCH_STEP;
    }
    vector_sum(&work->pos, &forward, &work->pos);
    gravity = params->gravity;
    APPLY_GRAVITY(kart, work, gravity);
}

static void slide(struct task *task)
{
    struct task_work *work;
    struct cart_vehicle *kart;
    NJS_VECTOR delta;
    Float speed;
    kart = task->motion.p;
    work = task->work;
    /* @hack redundant: a first copy of the velocity in delta, which nothing
     * reads */
    delta = kart->velocity;
    delta.y = 0;
    kart->drift_velocity = kart->velocity;
    kart->drift_velocity.y = 0;
    drift_rotation.y = -kart->yaw - work->ang.y;
    rotate_vector(&drift_rotation, &kart->drift_velocity);
    if (kart->drift_velocity.x == 0 && kart->drift_velocity.y == 0 &&
        kart->drift_velocity.z == 0) {
        speed = 0;
    } else {
        speed = njScalor(&kart->drift_velocity);
    }
    kart->drift_velocity.z *= tuning.drift_grip;
    if (!kart->accelerating) {
        speed *= SLIDE_DRAG;
    }
    njUnitVector(&kart->drift_velocity);
    kart->drift_velocity.x *= speed;
    kart->drift_velocity.z *= speed;
    drift_rotation.y = kart->yaw + work->ang.y;
    delta = kart->drift_velocity;
    rotate_vector(&drift_rotation, &delta);
    vector_sum(&work->pos, &delta, &work->pos);
}

static void enter_drift(struct task *task)
{
    struct task_work *work;
    struct cart_vehicle *kart;
    kart = task->motion.p;
    work = task->work;
    face_velocity(task);
    if (kart->drift_stick < 0) {
        kart->yaw += drift_entry_turns[kart->steering_timer++];
    } else {
        kart->yaw -= drift_entry_turns[kart->steering_timer++];
    }
    if (kart->steering_timer >= ENTRY_STEPS) {
        kart->mode = DRIFT;
        kart->steering_timer = 0;
    }
    slide(task);
    APPLY_GRAVITY(kart, work, tuning.gravity);
}

static void drift(struct task *task)
{
    struct task_work *work;
    struct cart_vehicle *kart;
    Angle previous_yaw;
    work = task->work;
    kart = task->motion.p;
    previous_yaw = work->ang.y;
    face_velocity(task);
    kart->yaw = previous_yaw + kart->yaw - work->ang.y;
    /* @hack redundant: a test of the stick against the drift's side, with an
     * empty body */
    if (KART_PAD(kart).x1 * kart->drift_stick < 0) {
    }
    /* @hack type: the stick cast to int, where its own Sint16 would do */
    kart->yaw -= (Angle)((int)KART_PAD(kart).x1 * DRIFT_STEER);
    slide(task);
    if (_builtin_fabsf(kart->drift_velocity.x) < GRIP_SLIP) {
        kart->steering_timer++;
        if (kart->steering_timer >= GRIP_STEPS) {
            kart->mode = DRIVE;
        }
    } else {
        kart->steering_timer = 0;
    }
    if (kart->velocity.x == 0 && kart->velocity.y == 0 &&
        kart->velocity.z == 0) {
        kart->mode = DRIVE;
    } else if (njScalor(&kart->velocity) < tuning.drift_min_speed * 0.5F) {
        kart->mode = DRIVE;
    }
    if (kart->grounded) {
        kart->air_tilt.x = angle_approach(kart->air_tilt.x, 0, LEVEL_STEP);
    } else {
        kart->air_tilt.x += AIR_PITCH_STEP;
    }
    APPLY_GRAVITY(kart, work, tuning.gravity);
}

static void spin_out(struct task *task)
{
    struct task_work *work;
    struct cart_vehicle *kart;
    NJS_VECTOR forward;
    Float speed;
    kart = task->motion.p;
    work = task->work;
    kart->yaw += kart->spin;
    if (kart->spin > 0) {
        kart->spin -= SPIN_DECAY;
        if (kart->spin < 0) {
            kart->mode = DRIVE;
            face_velocity(task);
            kart->yaw = 0;
        }
    } else {
        kart->spin += SPIN_DECAY;
        if (kart->spin > 0) {
            kart->mode = DRIVE;
            face_velocity(task);
            kart->yaw = 0;
        }
    }
    forward.x = kart->velocity.x;
    forward.y = 0;
    forward.z = kart->velocity.z;
    if (forward.x == 0 && forward.z == 0) {
        speed = 0;
    } else {
        speed = njScalor(&forward);
    }
    forward.x = 0;
    forward.y = 0;
    forward.z = -(speed * SPIN_DRAG);
    spin_rotation.y = work->ang.y;
    rotate_vector(&spin_rotation, &forward);
    vector_sum(&work->pos, &forward, &work->pos);
    APPLY_GRAVITY(kart, work, tuning.gravity);
}

static void rotate_vector(struct angles *angles, NJS_VECTOR *vector)
{
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njRotateZ(NULL, angles->z);
    njRotateX(NULL, angles->x);
    njRotateY(NULL, angles->y);
    njCalcVector(NULL, vector, vector);
    njPopMatrix(1);
}

static void draw_shadow(struct task *task)
{
    struct cart_vehicle *kart;
    struct task_work *work;
    /* @hack redundant: unused_a and unused_b, locals that only size the
     * frame */
    NJS_VECTOR unused_a;
    NJS_VECTOR unused_b;
    kart = task->motion.p;
    work = &kart->previous;
    draw_control_set(NJD_CONTROL_3D_SHADOW | NJD_CONTROL_3D_TRANS_MODIFIER);
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, work->pos.y + shadow_height + SHADOW_LIFT,
                work->pos.z);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y + kart->yaw - HALF_TURN);
    njRotateZ(NULL, kart->body_tilt.z + kart->air_tilt.z);
    njRotateX(NULL, kart->body_tilt.x + kart->air_tilt.x);
    njRotateY(NULL, kart->body_tilt.y + kart->air_tilt.y);
    switch (kart->start.model) {
    case CART_MODEL_TAILS_STORY:
        njRotateY(NULL, QUARTER_TURN);
        njCnkModDrawObject(&cart_tails_shadow);
        break;
    case CART_MODEL_ROUGE_STORY:
        njCnkModDrawObject(&cart_rouge_shadow);
        break;
    case CART_MODEL_SONIC:
    case CART_MODEL_KNUCKLES:
    case CART_MODEL_TAILS:
    case CART_MODEL_EGGMAN:
    case CART_MODEL_SHADOW:
    case CART_MODEL_ROUGE:
        njScale(NULL, shadow_scale, shadow_scale, shadow_scale);
        njCnkModDrawObject(&cart_racer_shadow);
        break;
    case CART_MODEL_DOWNLOAD:
        njScale(NULL, DOWNLOAD_SHADOW_SCALE, 1, DOWNLOAD_SHADOW_SCALE);
        njCnkModDrawObject(&shadow_object);
        break;
    }
    njPopMatrixEx();
    draw_control_clear(NJD_CONTROL_3D_SHADOW | NJD_CONTROL_3D_TRANS_MODIFIER);
}

static void drive_clear(struct task *task)
{
    struct task_work *work;
    struct cart_vehicle *kart;
    kart = task->motion.p;
    work = task->work;
    work->pos.x += CLEAR_SPEED;
    work->ang.x = 0;
    work->ang.y = -QUARTER_TURN;
    work->ang.z = 0;
    kart->yaw = 0;
    kart->boost_steps = 0;
}

static void drive_approach(struct task *task)
{
    struct task_work *work;
    struct cart_vehicle *kart;
    kart = task->motion.p;
    work = task->work;
    work->pos.z -= APPROACH_SPEED;
    kart->yaw = 0;
    kart->boost_steps = 0;
    if (approach_steps++ > APPROACH_STEPS) {
        cart_story_approach = 0;
        kart->mode = DRIVE;
    }
}

static void cpu_drive(struct task *task)
{
    struct task_work *work;
    struct cart_vehicle *kart;
    NJS_POINT3 position;
    NJS_POINT3 target;
    NJS_VECTOR toward;
    Angle heading;
    Float limit;
    kart = task->motion.p;
    work = task->work;
    target.x = cart_gates[kart->target_gate][kart->target_lane].x;
    target.y = 0;
    target.z = cart_gates[kart->target_gate][kart->target_lane].z;
    position.x = work->pos.x;
    position.y = 0;
    position.z = work->pos.z;
    vector_diff(&target, &position, &toward);
    heading = KART_ANGLE(atan2f(toward.z, toward.x));
    limit = (((CART_RACERS - 1 - kart->player) * SLOT_SPEED) +
             ((CART_LAPS - 1 - kart->lap) * LAP_SPEED) +
             (kart->place * cart_speed_per_level) +
             ((kart->place > 1) * cart_speed_above_one) + CPU_SPEED) *
            cart_speed_scale;
    if (angle_distance(heading, kart->cpu_heading) < ON_COURSE) {
        kart->cpu_speed += tuning.acceleration;
        if (kart->cpu_speed > limit) {
            kart->cpu_speed = limit;
        }
    } else {
        kart->cpu_speed += tuning.braking;
        if (kart->cpu_speed < CPU_LEAST_SPEED) {
            kart->cpu_speed = CPU_LEAST_SPEED;
        }
    }
    kart->cpu_heading = angle_approach(kart->cpu_heading, heading, CPU_TURN);
    work->pos.x += kart->cpu_speed * njCos(kart->cpu_heading);
    work->pos.z += kart->cpu_speed * njSin(kart->cpu_heading);
    work->pos.y += kart->velocity.y - tuning.gravity;
    if (kart->target_gate == kart->gate ||
        njScalor2(&toward) < GATE_REACH * GATE_REACH) {
        kart->last_gate = kart->target_gate;
        kart->last_lane = kart->target_lane;
        kart->target_gate++;
        if (kart->target_gate >= cart_gate_count) {
            kart->target_gate = 0;
        }
        switch ((int)(rand() * (LANE_CHOICES * RAND_SCALE))) {
        case 0:
            kart->target_lane++;
            break;
        case 1:
            kart->target_lane--;
            break;
        }
        if (kart->target_lane > CART_GATE_POINTS - 1) {
            kart->target_lane = CART_GATE_POINTS - 1;
        } else if (kart->target_lane < 0) {
            kart->target_lane = 0;
        }
    }
    if (kart->grounded) {
        kart->last_ground = work->pos;
    }
    work->ang.y = -kart->cpu_heading - QUARTER_TURN;
}

static void draw_lights(struct task *task)
{
    struct task_work *work;
    struct cart_vehicle *kart;
    NJS_SPRITE sprite;
    NJS_VECTOR offset;
    struct angles angles;
    int i;
    int piece;
    kart = task->motion.p;
    work = &kart->previous;
    sprite.tlist = &cart_race_textures;
    sprite.tanim = light_cell;
    sprite.ang = 0;
    sprite.sx = LAMP_SCALE;
    sprite.sy = LAMP_SCALE;
    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    constant_material_set(0, 0, 0, 0);
    angles.x = work->ang.x;
    angles.y = work->ang.y + kart->yaw - HALF_TURN;
    angles.z = work->ang.z;
    switch (kart->start.model) {
    case CART_MODEL_ROUGE_STORY:
        for (i = ROUGE_LAMPS; i <= ROUGE_LAMPS + 1; i++) {
            sprite.p.x = work->pos.x;
            sprite.p.y = work->pos.y + MODEL_Y;
            sprite.p.z = work->pos.z;
            offset = light_positions[i];
            rotate_vector(&angles, &offset);
            vector_sum(&offset, &sprite.p, &sprite.p);
            njDrawSprite3D(&sprite, 0, NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE);
        }
        break;
    case CART_MODEL_SONIC:
    case CART_MODEL_KNUCKLES:
    case CART_MODEL_TAILS:
    case CART_MODEL_EGGMAN:
    case CART_MODEL_SHADOW:
    case CART_MODEL_ROUGE:
        /* @hack spelling: braking tested against 1, where a truth test would
         * do */
        if (kart->braking == 1) {
            constant_material_set(1, 1, 0, 0);
        } else {
            piece = cart_setup.pieces[cart_gate_sections[kart->gate]];
            if (piece >= CART_FIRST_TUNNEL && piece <= CART_LAST_TUNNEL) {
                constant_material_set(TUNNEL_TAIL_ALPHA, 1, 0, 0);
            } else {
                constant_material_set(0, 1, 0, 0);
            }
        }
        for (i = HEAD_LAMPS; i <= TAIL_LAMPS + 1; i++) {
            sprite.p.x = work->pos.x;
            sprite.p.y = work->pos.y + MODEL_Y;
            sprite.p.z = work->pos.z;
            offset = light_positions[i];
            rotate_vector(&angles, &offset);
            vector_sum(&offset, &sprite.p, &sprite.p);
            if (i == TAIL_LAMPS || i == TAIL_LAMPS + 1) {
                njDrawSprite3D(&sprite, 0,
                               NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE |
                                   NJD_SPRITE_COLOR);
            } else {
                njDrawSprite3D(&sprite, 0, NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE);
            }
        }
        constant_material_set(0, 0, 0, 0);
        break;
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}

static Bool use_distant_model(struct task *task)
{
    struct cart_vehicle *kart;
    struct task_work *work;
    NJS_VECTOR delta;
    kart = task->motion.p;
    work = &kart->previous;
    if (kart->second) {
        vector_diff(&camera_views[camera_current_index]->pos, &work->pos,
                    &delta);
        if ((delta.x * delta.x) + (delta.y * delta.y) + (delta.z * delta.z) >
            NEAR_RANGE * NEAR_RANGE) {
            return TRUE;
        }
    }
    return FALSE;
}

/* @hack redundant: register on task and kart, where plain locals would do */
static void check_impact(register struct task *task)
{
    register struct cart_vehicle *kart;
    Float speed[2];
    kart = task->motion.p;
    speed[0] = njSqrt((kart->velocity.x * kart->velocity.x) +
                      (kart->velocity.z * kart->velocity.z));
    speed[1] = njSqrt((last_step.velocity.x * last_step.velocity.x) +
                      (last_step.velocity.z * last_step.velocity.z));
    if (speed[0] < speed[1] - IMPACT_LOSS) {
        kart->mode = SPIN_OUT;
        if (kart->yaw > 0) {
            kart->spin = SPIN_START;
        } else {
            kart->spin = -SPIN_START;
        }
        sound_play(SPIN_SOUND, NULL, 0, 0);
        switch (cart_setup.course_mode) {
        case CART_TAILS:
            voice_play_priority_2(TAILS_SPIN_VOICE);
            break;
        case CART_ROUGE:
            voice_play_priority_2(ROUGE_SPIN_VOICE);
            break;
        }
    }
}

static void recover_fall(struct task *task)
{
    struct cart_vehicle *kart;
    struct task_work *work;
    struct task_work *previous;
    kart = task->motion.p;
    work = task->work;
    previous = &kart->previous;
    if (kart->grounded) {
        kart->last_ground = work->pos;
    }
    if (work->pos.y < cart_course_lowest) {
        switch (cart_setup.course_mode) {
        case CART_TAILS:
        case CART_ROUGE:
            if (game_state.lives[0] < 1) {
                kart->mode = FINISHED;
                stage_end(STAGE_END_QUIT);
                return;
            } else {
                stage_end(STAGE_END_RESTART);
            }
            break;
        default:
            kart->respawn_timer = RESPAWN_STEPS + 1;
            break;
        }
        if (!kart->start.player_controlled) {
            /* @bug memory: reads cart_gates before its first gate for a CPU
             * kart that falls while its last_gate is below RESPAWN_GATES
             * (0x8c50426a, 0x8c50429e). the kart's target_gate then falls
             * below 0 (0x8c5042b0), and cpu_drive reads cart_gates by it */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: last_gate wraps past the
             * course's last gate first */
            if (kart->last_gate < RESPAWN_GATES) {
                kart->last_gate += cart_gate_count;
            }
#endif
            work->pos.x =
                cart_gates[kart->last_gate - RESPAWN_GATES][kart->last_lane].x;
            work->pos.y = kart->last_ground.y + CPU_DROP_HEIGHT;
            work->pos.z =
                cart_gates[kart->last_gate - RESPAWN_GATES][kart->last_lane].z;
            kart->target_gate = kart->last_gate - (RESPAWN_GATES - 1);
            kart->last_gate -= RESPAWN_GATES;
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: target_gate wraps too */
            kart->target_gate %= cart_gate_count;
#endif
            previous->pos = work->pos;
            previous->pos.y += DROP_FALL;
            kart->velocity.x *= DROP_KEEP;
            kart->velocity.y = 0;
            kart->velocity.z *= DROP_KEEP;
            kart->yaw = 0;
            work->mode = 0;
            face_velocity(task);
            /* @hack redundant: a second clear of yaw, which face_velocity
             * leaves as it was */
            kart->yaw = 0;
        } else if (kart->respawn_timer == 1) {
            return;
        } else {
            if (kart->respawn_timer++ > RESPAWN_STEPS) {
                kart->gate -= RESPAWN_GATES;
                if (kart->gate < 0) {
                    if (cart_setup.course_mode == CART_TAILS ||
                        cart_setup.course_mode == CART_ROUGE) {
                        kart->gate = STORY_FIRST_GATE;
                    } else {
                        kart->gate = kart->gate + cart_gate_count;
                        kart->lap--;
                        kart->unk_c8 = 2;
                    }
                }
                work->pos.x = cart_gates[kart->gate][START_LANE].x;
                work->pos.y = kart->last_ground.y + DROP_HEIGHT;
                work->pos.z = cart_gates[kart->gate][START_LANE].z;
                previous->pos = work->pos;
                previous->pos.y += DROP_FALL;
                kart->velocity.x *= DROP_KEEP;
                kart->velocity.y = 0;
                kart->velocity.z *= DROP_KEEP;
                kart->yaw = 0;
                kart->air_tilt.x = 0;
                kart->air_tilt.y = 0;
                kart->air_tilt.z = 0;
                kart->respawn_timer = 0;
            } else {
                work->pos = previous->pos;
                work->ang = previous->ang;
            }
        }
    }
}

#pragma inline(charge_boost)
static void charge_boost(struct cart_vehicle *kart)
{
    if (kart->boost_rings >= BOOST_RINGS) {
        cart_boost_ready[kart->driver] = 1;
        kart->boost_rings = 0;
    }
    /* @hack redundant: a return at the function's end */
    /* NOLINTNEXTLINE(readability-redundant-control-flow) */
    return;
}

static void update_boost(struct task *task)
{
    struct task_work *work;
    struct cart_vehicle *kart;
    NJS_POINT3 pos;
    NJS_VECTOR velocity;
    kart = task->motion.p;
    work = task->work;
    if (kart->driver < 0) {
        return;
    }
    charge_boost(kart);
    if (kart->boost_steps) {
        tuning.acceleration *= BOOST_FACTOR;
        tuning.cruise_speed *= BOOST_FACTOR;
        kart->boost_steps--;
        pos = work->pos;
        pos.y -= POWDER_DROP;
        velocity = kart->velocity;
        powder_create(kart->driver, POWDER_SPECKS, &pos, &velocity);
    } else if (cart_boost_ready[kart->driver]) {
        if (player_pads[kart->driver]->press & PDD_DGT_TY) {
            kart->boost_steps = BOOST_STEPS;
            cart_boost_ready[kart->driver] = 0;
            njUnitVector(&kart->velocity);
            kart->velocity.x *= BOOST_SPEED;
            kart->velocity.y *= BOOST_SPEED;
            kart->velocity.z *= BOOST_SPEED;
            cart_driver_sound(task, CART_BOOST_SOUND);
            vibration_start_fading(kart->driver, VIBRATION_POWER, BOOST_FREQ,
                                   BOOST_STEPS);
        }
    }
}

void cart_reset_history(void)
{
    char *bytes;
    int remaining;
    /* @hack spelling: each record cleared a byte at a time, under a count
     * that runs down to 0, where memset would do */
    bytes = (char *)velocity_history;
    for (remaining = sizeof(velocity_history); remaining != 0; remaining--) {
        *bytes++ = 0;
    }
    bytes = (char *)angle_history;
    for (remaining = sizeof(angle_history); remaining != 0; remaining--) {
        *bytes++ = 0;
    }
    motion_frame[0] = 0;
    motion_frame[1] = 0;
    bytes = (char *)stick_history;
    for (remaining = sizeof(stick_history); remaining != 0; remaining--) {
        *bytes++ = 0;
    }
    stick_frame[0] = 0;
    stick_frame[1] = 0;
}

static void record_motion(int player, NJS_VECTOR *speed, struct angles *angles)
{
    velocity_history[player][motion_frame[player]] = *speed;
    angle_history[player][motion_frame[player]] = *angles;
    motion_frame[player]++;
    if (motion_frame[player] >= CART_HISTORY_STEPS) {
        motion_frame[player] = 0;
    }
}

static void previous_velocity(int player, int steps_back, NJS_VECTOR *out)
{
    int frame = motion_frame[player] - steps_back;
    if (frame < 0) {
        frame += CART_HISTORY_STEPS;
    }
    *out = velocity_history[player][frame];
}

void cart_previous_angles(int player, int steps_back, struct angles *out)
{
    int frame = motion_frame[player] - steps_back;
    if (frame < 0) {
        frame += CART_HISTORY_STEPS;
    }
    *out = angle_history[player][frame];
}

static void record_stick(int player)
{
    stick_history[player][stick_frame[player]] =
        cart_pad_history[cart_pad_frame][player].x1;
    stick_frame[player]++;
    if (stick_frame[player] >= CART_HISTORY_STEPS) {
        stick_frame[player] = 0;
    }
}

static int previous_stick(int player, int steps_back)
{
    int frame = stick_frame[player] - steps_back;
    if (frame < 0) {
        frame += CART_HISTORY_STEPS;
    }
    return stick_history[player][frame];
}

static int sum_stick(int player, int count)
{
    int sum;
    int i;
    sum = 0;
    /* @bug logic: sums the records 0 to count - 1 back, where 1 to count is
     * meant (0x8c504760). 0 back is the ring's oldest record, from
     * CART_HISTORY_STEPS steps ago, which the sum takes in place of the
     * record count back */
    for (i = 0; i < count; i++) {
        sum += previous_stick(player, i);
    }
    return sum;
}

static void inverse_rotate(struct angles *angles, NJS_VECTOR *src,
                           NJS_VECTOR *dest)
{
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateZ(NULL, angles->z);
    njRotateX(NULL, angles->x);
    njRotateY(NULL, angles->y);
    njInvertMatrix(NULL);
    njCalcVector(NULL, src, dest);
    njPopMatrixEx();
}

void cart_launch(struct task *task, NJS_VECTOR *velocity)
{
    struct cart_vehicle *kart;
    Angle heading;
    kart = task->motion.p;
    if (!kart->start.player_controlled) {
        if (velocity->x == 0 && velocity->y == 0 && velocity->z == 0) {
            kart->cpu_speed = 0;
        } else {
            kart->cpu_speed = njScalor(velocity);
        }
        heading = KART_ANGLE(atan2f(velocity->z, velocity->x));
        kart->cpu_heading =
            angle_approach(kart->cpu_heading, heading, QUARTER_TURN);
    } else {
        kart->mode = DRIVE;
        kart->yaw = 0;
        kart->velocity = *velocity;
        face_velocity(task);
    }
}

void cart_grant_invincibility(struct task *task)
{
    struct cart_vehicle *kart = task->motion.p;
    kart->invincible = 1;
    kart->invincible_steps = INVINCIBLE_STEPS;
    jingle_play(invincibility_music);
    cart_create_invincibility(kart->player);
}

void cart_hit_traffic(struct task *task, NJS_POINT3 *pos)
{
    struct task_work *work;
    struct cart_vehicle *kart;
    NJS_VECTOR away;
    work = task->work;
    kart = task->motion.p;
    sound_play(CRASH_SOUND, NULL, 0, 0);
    vector_diff(&work->pos, pos, &away);
    kart->push.x = away.x * traffic_push;
    kart->push.z = away.z * traffic_push;
    kart->velocity.x *= CAR_HIT_KEEP;
    kart->velocity.y *= CAR_HIT_KEEP;
    kart->velocity.z *= CAR_HIT_KEEP;
    vibration_start(kart->player, CAR_HIT_POWER, CRASH_FREQ, VIBRATION_UNTIMED);
    if (game_state.mission == MISSION_CHAO &&
        kart->start.model < CART_MODEL_SONIC) {
        if (game_state.lives[0] < 1) {
            stage_end(STAGE_END_QUIT);
        } else {
            stage_end(STAGE_END_RESTART);
        }
    }
}

/* play the engine's and the brakes' sounds for a step. a step where
 * cart_racers[0]'s kart passes into or out of a tunnel stops both */
#define UPDATE_SOUNDS()                                                        \
    do {                                                                       \
        do_nothing(task);                                                      \
        if (kart->tunnel != cart_leader_tunnel_sound()) {                      \
            kart->tunnel = !kart->tunnel;                                      \
            sound_stop_key(engine);                                            \
            sound_stop_key(brakes);                                            \
        } else if (kart->accelerating) {                                       \
            sound_play_at_for(ENGINE_SOUND, engine, SOUND_PRIORITY,            \
                              KART_VOLUME, SOUND_STEPS, &task->work->pos);     \
            sound_stop_key(brakes);                                            \
        } else {                                                               \
            if (kart->braking) {                                               \
                if (speed > BRAKE_SPEED) {                                     \
                    sound_play_at_for(BRAKE_SOUND, brakes, SOUND_PRIORITY,     \
                                      KART_VOLUME, SOUND_STEPS,                \
                                      &task->work->pos);                       \
                } else {                                                       \
                    sound_stop_key(brakes);                                    \
                }                                                              \
                sound_stop_key(engine);                                        \
            } else {                                                           \
                sound_stop_key(brakes);                                        \
                sound_stop_key(engine);                                        \
            }                                                                  \
        }                                                                      \
        sound_set_pitch(engine, SOUND_ANY_ID, pitch);                          \
    } while (0)

static void engine_sound(struct task *task)
{
    struct cart_vehicle *kart;
    Sint16 pitch;
    NJS_VECTOR horizontal;
    Float speed;
    char *engine;
    char *brakes;
    kart = task->motion.p;
    if (!kart->start.player_controlled) {
        return;
    }
    horizontal.x = kart->velocity.x;
    horizontal.y = 0;
    horizontal.z = kart->velocity.z;
    if (horizontal.x == 0 && horizontal.z == 0) {
        speed = 0;
    } else {
        speed = njScalor(&horizontal);
    }
    /* @hack spelling: the switch's expression sets the keys and the pitch,
     * the brakes' key in two steps, where statements before the switch would
     * do */
    switch (engine = (char *)task + ENGINE_SOUND, brakes = (char *)task,
            brakes += BRAKE_SOUND, pitch = (int)speed << PITCH_SHIFT,
            kart->mode) {
    case DRIVE:
        UPDATE_SOUNDS();
        break;
    case STRAIGHTEN:
    case ENTER_DRIFT:
    case DRIFT:
        UPDATE_SOUNDS();
        sound_play_at_for(BRAKE_SOUND, brakes, SOUND_PRIORITY, KART_VOLUME,
                          SOUND_STEPS, &task->work->pos);
        break;
    case STORY_APPROACH:
    case STORY_CLEAR:
        sound_play_held_at(ENGINE_SOUND, engine, SOUND_PRIORITY, KART_VOLUME,
                           &task->work->pos);
        break;
    }
}

Bool cart_driver_sound(struct task *task, int sound)
{
    struct cart_vehicle *kart = task->motion.p;
    if (kart->start.player_controlled &&
        kart->start.model < CART_MODEL_DOWNLOAD) {
        cart_leader_tunnel_sound();
        sound_play(driver_sounds[kart->start.model][sound], NULL, 0, 0);
        return TRUE;
    }
    return FALSE;
}

/* the CPU karts' speed scales on the three courses of a race. on the expert
 * course, cart_speed_above_one rises too. a download course gives its own */
#define BEGINNER_SPEED_SCALE 0.98F
#define STANDARD_SPEED_SCALE 0.93F
#define EXPERT_SPEED_SCALE 0.9F
#define EXPERT_ABOVE_ONE 0.2F
void cart_set_tuning(void)
{
    struct cart_course *course;
    switch (cart_setup.course_mode) {
    case CART_BEGINNER:
        cart_speed_scale = BEGINNER_SPEED_SCALE;
        break;
    case CART_STANDARD:
        cart_speed_scale = STANDARD_SPEED_SCALE;
        break;
    case CART_EXPERT:
        cart_speed_scale = EXPERT_SPEED_SCALE;
        cart_speed_above_one = EXPERT_ABOVE_ONE;
        break;
    case CART_DOWNLOAD:
        course = cart_download_course();
        cart_tunings[CART_MODEL_DOWNLOAD] = course->tuning;
        if (game_state.kart_character_karts == 1) {
            cart_speed_scale = course->speed[1].scale;
            cart_speed_per_level = course->speed[1].per_level;
            cart_speed_above_one = course->speed[1].above_one;
        } else {
            cart_speed_scale = course->speed[0].scale;
            cart_speed_per_level = course->speed[0].per_level;
            cart_speed_above_one = course->speed[0].above_one;
        }
        break;
    }
}
