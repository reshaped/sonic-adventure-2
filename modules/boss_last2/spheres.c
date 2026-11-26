#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include "boss_last2/attack_art.h"
#include "boss_last2/arena.h"
#include "boss_last2/body.h"
#include "boss_last2/sphere.h"
#include "camera/camera.h"
#include "collision/collision.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "effect/burst.h"
#include "player/player.h"
#include "shared/last_boss_arena.h"
#include "shared/last_boss_sphere.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/pad.h"
#include "system/game_state.h"
#include "system/matrix.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"
#ifdef NONMATCHING
/* zam_vec3_turn_z, by which this side turns a vector. for a finite vector it
 * gives the shipped bits, as turn.h says */
#include "engine/dc/turn.h"
#endif

enum sphere_mesh { MESH_NEAR, MESH_FAR, SPHERE_MESH_COUNT };
/* a wave's modes, in its work's mode. a wave waits in WAVE_READY for
 * finalhazard_sphere_volley, unless it repeats. it waits in WAVE_FINISHED for
 * finalhazard_sphere_finished to end it. nothing sets WAVE_WAIT,
 * WAVE_DISPERSE or WAVE_RELEASE. only finalhazard_sphere_clear, which no code
 * calls, sets WAVE_CLEAR */
enum wave_mode {
    WAVE_SPAWN,
    WAVE_READY,
    WAVE_VOLLEY,
    WAVE_WAIT,
    WAVE_FINISHED,
    WAVE_CLEAR,
    WAVE_DISPERSE,
    WAVE_RELEASE,
    WAVE_RESTART
};
/* the work's flag finalhazard_sphere_repeat sets. a repeating wave starts its
 * volley by itself, and spawns again once its spheres are gone */
#define WAVE_REPEATING 0x100
/* a wave's record, in its task's extra */
struct sphere_wave {
    Sint16 count; /* the spheres it spawns */
    /* the spheres not yet aimed. a sphere that bursts as it rises or hovers
     * counts too. nothing reads it but wave_lose_sphere, which keeps it from
     * falling below 0 */
    Sint16 remaining;
    /* the obstacles finalhazard_sphere_wave was given, and their count.
     * nothing reads either */
    Sint16 obstacle_count;
    const struct last_boss_sphere_obstacle *obstacles;
    Angle turn; /* nothing reads or writes it */
    Sint32 hold; /* 0, which each sphere copies into its own hold */
};
/* the latest wave's spheres, by slot. a slot is null once its sphere is
 * aimed or dies. it is null too once its sphere fades or lifts, which only
 * modes no code sets start. a new wave clears every slot */
#define SPHERE_SLOTS 128
#define NO_SLOT (-1) /* a slot index that names none */
static struct task *sphere_slots[SPHERE_SLOTS];
/* clang-format off */
static const struct collision_shape sphere_shape = {
    0, SHAPE_SPHERE, PUSH_PLAYERS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
    SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(3),
    SHAPE_IN_WORLD | SHAPE_UNTURNED | SHAPE_IGNORE_ENEMIES |
        SHAPE_IGNORE_OBJECTS | SHAPE_IGNORE_PICKUPS,
    {0, 0, 0}, 10, 0, 0, 0, {0, 0, 0}};
/* clang-format on */
static const NJS_VECTOR burst_rise = {0, 0.2F, 0};
static Sint16 default_wave_count = 80;
/* how many spheres a WAVE_FEW, a WAVE_NORMAL and a WAVE_MANY wave hold. the
 * debug menu changes them */
static Sint16 spheres_few = 9;
static Sint16 spheres_normal = 40;
static Sint16 spheres_many = 80;
/* a sphere draws its near model within near_distance of the active player's
 * camera. beyond, it draws its far one, turned to face camera 0 */
static Float near_distance = 400.0F;
/* a hovering sphere bobs by swing_height either way, which is 0, so no
 * sphere bobs. the bob's phase steps at a rate the sphere picks as it spawns.
 * the rate lies at random from half of hover_step up to hover_step */
static Angle hover_step = 0x200;
static Float swing_height = 0.0F;
/* an aimed sphere's flight steps along its curve by sphere_speed / d, plus a
 * growing acceleration. d is its distance from the target as it starts. the
 * debug menu changes sphere_speed */
static Float sphere_speed = 2.5F;
static Angle orbit_acceleration[] = {64, 4};
/* sphere_models compiled for njDirectDrawObject. a model that didn't fit
 * below DEMO_AREA is null */
static NJS_DIRECT_HEAD *compiled_meshes[SPHERE_MESH_COUNT] = {NULL};
static NJS_CNK_OBJECT *sphere_models[SPHERE_MESH_COUNT] = {
#include "boss_last2/spheres/sphere_models.inc"
};
#define SPHERE_BUILD_MODELS finalhazard_build_shots
#define SPHERE_TEXTURES finalhazard_effect_texlist
#define SPHERE_MESHES compiled_meshes
#include "shared/last_boss_sphere_motion.h"

/* an aimed sphere's flight follows the active player for its first
 * track_steps steps. it doesn't follow in a step where the Finalhazard is
 * stunned */
static Sint16 track_steps = 10;
static Sint16 unused_tracking = 0;

/* a sphere rises from finalhazard_center straight out to its place. it
 * starts at RISE_SPEED, plus up to RISE_SPREAD. each step multiplies its
 * speed by RISE_GROWTH while that is below RISE_TOP_SPEED. it stops once as
 * far from the origin as its place */
#define RISE_SPEED 0.2F
#define RISE_SPREAD 0.3F
#define RISE_GROWTH 1.2F
#define RISE_TOP_SPEED 10.0F

static void sphere_rise(struct task *task)
{
    struct task_work *work = task->work;
    struct last_boss_sphere *sphere = task->extra.p;
    NJS_VECTOR radial;
    switch (work->submode) {
    case 0:
        vector_diff(&sphere->target, &work->pos, &radial);
        last_boss_vector_length(
            &radial, RISE_SPEED + (rand() * (RISE_SPREAD * RAND_SCALE)),
            &radial);
        sphere->velocity = radial;
        work->submode++;
        /* fall through */
    case 1:
        njAddVector(&work->pos, &sphere->velocity);
        if (njScalor(&sphere->velocity) < RISE_TOP_SPEED) {
            SCALE_COMPONENTS(sphere->velocity, RISE_GROWTH);
        }
        if (!(njScalor(&sphere->target) > njScalor(&work->pos))) {
            work->pos.x = sphere->target.x;
            work->pos.y = sphere->target.y;
            work->pos.z = sphere->target.z;
            sphere->velocity.x = 0.0F;
            sphere->velocity.y = 0.0F;
            sphere->velocity.z = 0.0F;
            work->mode = SPHERE_HOVER;
            work->submode = 0;
        }
        break;
    }
}

static void sphere_hover(struct task *task)
{
    struct last_boss_sphere *sphere = task->extra.p;
    PULSE_SPHERE(sphere);
    if (sphere->hold != 1) {
        sphere->swing += sphere->swing_step;
        if (sphere->swing > FULL_TURN) {
            sphere->swing -= FULL_TURN;
        }
    }
}

/* an aimed sphere's timer counts down from CHARGE_STEPS before it flies at
 * the active player. its curve's control points lie FIRST_CONTROL and
 * SECOND_CONTROL of the way to the target. both then turn about z, around the
 * sphere, by TURN_LEAST plus up to TURN_SPREAD either way */
#define CHARGE_STEPS 30
#define FIRST_CONTROL 0.25F
#define SECOND_CONTROL 0.75F
#define TURN_LEAST 1024.0F
#define TURN_SPREAD 1024.0F
/* a coin toss: whether a random value below 10, as a byte, is even */
#define HEADS() ((Sint8)(rand() * (10.0F * RAND_SCALE)) % 2 == 0)
/* the flight's first acceleration along its curve */
#define CURVE_ACCEL 0.0005F

static void sphere_charge(struct task *task)
{
    struct task_work *work = task->work;
    struct last_boss_sphere *sphere = task->extra.p;
    NJS_VECTOR offset;
    NJS_VECTOR direction;
    Angle turn;
    switch (work->submode) {
    case 0:
        work->timer = CHARGE_STEPS;
        task->work->collision->flags &= ~COLLISION_TARGET;
        work->pos.y += swing_height * njSin(sphere->swing);
        sphere->swing = 0;
        last_boss_sphere_burst(task, &work->pos, LAST_BOSS_BURST_SHORT);
        sound_play_at(SOUND_SPHERE_CHARGE, NULL, 0, 0, &work->pos);
        work->submode++;
        /* fall through */
    case 1:
        if (work->timer-- == 0) {
            Float distance;
            sphere->target = player_work[finalhazard_players->active]->pos;
            sphere->start = work->pos;
            vector_diff(&sphere->target, &work->pos, &direction);
            turn = TURN_LEAST + (rand() * (TURN_SPREAD * RAND_SCALE));
            if (HEADS()) {
                turn = -turn;
            }
#ifdef NONMATCHING
            /* for a finite vector, the kernel gives the bits of the matrix
             * calls below, as engine/dc/turn.h says */
            zam_vec3_turn_z(turn, &direction.x, &direction.x);
#else
            matrix_push_unit();
            njRotateZ(NULL, turn);
            njCalcVector(NULL, &direction, &direction);
            njPopMatrixEx();
#endif
            offset = direction;
            SCALE_COMPONENTS(offset, FIRST_CONTROL);
            vector_sum(&work->pos, &offset, &sphere->first_control);
            offset = direction;
            SCALE_COMPONENTS(offset, SECOND_CONTROL);
            vector_sum(&work->pos, &offset, &sphere->second_control);
            distance = vector_distance(&work->pos, &sphere->target);
            sphere->curve_speed =
                sphere_speed * njInvertSqrt(distance) * njInvertSqrt(distance);
            sphere->curve_time = 0.0F;
            sphere->curve_accel = CURVE_ACCEL;
            work->timer = track_steps;
            work->mode = SPHERE_FLY;
            work->submode = 0;
        }
        break;
    }
}

#define SPHERE_TRACK_PLAYER
#define SPHERE_FINAL_APPROACH 50.0F
#define SPHERE_ARENA_LIMIT 2000.0F
#define SPHERE_TRAIL_PARENT NULL
#include "shared/last_boss_sphere_flight.h"

#define BURST_SCALE 5.0F

static void wave_lose_sphere(struct task *task);
static void sphere_explode(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR velocity;
    velocity = burst_rise;
    if (work->mode <= SPHERE_HOVER) {
        wave_lose_sphere(task->parent);
    }
    burst_create_wobbling(&work->pos, &velocity, BURST_SCALE);
    task_kill(task);
}

/* a sphere enters the collision lists only within CONTACT_RANGE of the
 * active player across the ground. a sphere that touches the player bursts.
 * where its attack beats the player's guard, it also plays
 * SOUND_SPHERE_IMPACT at HURT_VOLUME. it then knocks the player back, and
 * stuns the Finalhazard */
#define CONTACT_RANGE 100.0F
#define HURT_VOLUME 30

static void sphere_contact(struct task *task)
{
    struct task_work *work = task->work;
    struct last_boss_sphere *sphere = task->extra.p;
    const NJS_POINT3 *position = &work->pos;
    const NJS_POINT3 *player = &player_work[finalhazard_players->active]->pos;
    struct collision_hit *hit;
    sphere->player_distance = njSqrt(SPHERE_SQ(player->x - position->x) +
                                     SPHERE_SQ(player->z - position->z));
    if (sphere->player_distance > CONTACT_RANGE) {
        return;
    }
    hit = collision_next_player_hit(task);
    if (hit) {
        if (last_boss_hit_penetrates(task, hit) == 1) {
            sound_play_at(SOUND_SPHERE_IMPACT, NULL, 0, HURT_VOLUME,
                          &work->pos);
            finalhazard_push_player(NULL, FINALHAZARD_KNOCKBACK);
            sphere_explode(task);
            finalhazard_stun();
        } else {
            sphere_explode(task);
        }
    }
    collision_set_shape_center(task, 0, &sphere->center);
    collision_register(task);
}

static void sphere_camera_distance(struct task *task)
{
    struct task_work *work = task->work;
    struct last_boss_sphere *sphere = task->extra.p;
    if (camera_views[finalhazard_players->active]) {
        const NJS_POINT3 *position = &work->pos;
        const NJS_POINT3 *eye = &camera_views[finalhazard_players->active]->pos;
        sphere->camera_distance = njSqrt(SPHERE_SQ(eye->x - position->x) +
                                         SPHERE_SQ(eye->y - position->y) +
                                         SPHERE_SQ(eye->z - position->z));
    }
}

static void sphere_center(struct task *task)
{
    struct task_work *work = task->work;
    struct last_boss_sphere *sphere = task->extra.p;
    /* @hack redundant: an if (0) around a return */
    if (0) {
        return;
    }
    sphere->center = work->pos;
}

static void sphere_step(struct task *task)
{
    struct task_work *work = task->work;
    if (game_state.time_stopped) {
        return;
    }
    switch (work->mode) {
    case SPHERE_RISE:
        sphere_rise(task);
        break;
    case SPHERE_HOVER:
        sphere_hover(task);
        break;
    case SPHERE_CHARGE:
        sphere_charge(task);
        break;
    case SPHERE_FLY:
        sphere_fly(task);
        break;
    case SPHERE_BURST:
        sphere_explode(task);
        break;
    case SPHERE_FADE:
        sphere_fade(task);
        break;
    }
    sphere_camera_distance(task);
    sphere_center(task);
    sphere_contact(task);
}

static void sphere_draw(struct task *task)
{
    struct last_boss_sphere *sphere = task->extra.p;
    struct task_work *work = task->work;
    Sint8 mesh;
    struct angles facing;
    njSetTexture(&finalhazard_effect_texlist);
    if (sphere->fade < 0.0F) {
        draw_attr_save();
        draw_control_save();
        draw_control_set(NJD_CONTROL_3D_OFFSET_MATERIAL |
                         NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        draw_attr_set(0, NJD_FST_UA);
        constant_material_set(sphere->fade, sphere->fade, sphere->fade,
                              sphere->fade);
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njTranslate(NULL, 0.0F, swing_height * njSin(sphere->swing), 0.0F);
    if (sphere->camera_distance < near_distance) {
        njRotateY(NULL, QUARTER_TURN);
        mesh = MESH_NEAR;
    } else {
        if (camera_views[0]) {
            facing.y = last_boss_target_yaw(task, &camera_views[0]->pos);
            facing.x = last_boss_target_pitch(task, &camera_views[0]->pos,
                                              -QUARTER_TURN);
        }
#ifdef NONMATCHING
        /* this side repairs the bug tagged below: without camera 0's view, the
         * far model doesn't turn */
        else {
            facing.y = 0;
            facing.x = 0;
        }
#endif
        /* @bug uninitialized: turns the far model by two unset angles, at
         * 0x8c508cc6 and 0x8c508cce, where camera 0 has no view. the test at
         * 0x8c508ca8 then skips both bearings */
        njRotateY(NULL, facing.y);
        njRotateX(NULL, facing.x);
        mesh = MESH_FAR;
    }
    njScaleEx(&sphere->scale);
    /* @bug memory: draws a null model, at 0x8c508ce6, where the model didn't
     * fit in the buffer. njDirectDrawObject reads through it at 0x8c12785e */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it draws no model that didn't
     * fit */
    if (compiled_meshes[mesh])
#endif
    {
        njDirectDrawObject(compiled_meshes[mesh]);
    }
    njPopMatrixEx();
    light_apply(light_after_draw);
    if (sphere->fade < 0.0F) {
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
    }
}

static void sphere_destroy(struct task *task)
{
    struct last_boss_sphere *sphere = task->extra.p;
    sphere_slots[sphere->slot] = NULL;
}

/* a sprite of the whole of texture 5, which nothing draws */
static NJS_TEXANIM sphere_cell = {
#include "boss_last2/spheres/sphere_cell.inc"
};
static NJS_SPRITE sphere_sprite = {
#include "boss_last2/spheres/sphere_sprite.inc"
};
/* bands a wave could lay its spheres in. a band has an inner and an outer
 * radius, a height and a share of the wave. nothing reads bands, sector_ends
 * or unused_shell */
struct sphere_band {
    Float inner, outer, height, share;
};
/* clang-format off */
static struct sphere_band bands[2][4] = {
    {{150, 220, 30, 0.2F}, {150, 220, 80, 0.3F},
     {95, 180, 130, 0.3F}, {45, 130, 170, 0.1F}},
    {{215, 220, 30, 0.2F}, {165, 200, 80, 0.3F},
     {115, 150, 130, 0.3F}, {85, 100, 170, 0.1F}}};
/* clang-format on */
static Angle sector_ends[] = {0x2c00, 0x4000, 0x5400, 0x7fff,
                              0xac00, 0xc000, 0xd400, 0xffff};
static Float unused_shell[2] = {0, 0};
/* the shell a wave's spheres rest on, about the origin */
/* @hack type: shell a record of one Float, where the Float would do */
static struct sphere_shell {
    Float radius;
} shell = {250.0F};

#pragma inline(wave_hold)
static Sint32 wave_hold(const struct task *parent)
{
    const struct sphere_wave *wave = parent->extra.p;
    return wave->hold;
}

static struct task *sphere_spawn(struct task *parent, const NJS_POINT3 *target,
                                 Sint8 kind, Uint8 slot)
{
    struct task *task;
    struct task_work *work;
    struct last_boss_sphere *sphere;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, sphere_step, parent);
    if (!task) {
        return NULL;
    }
    task->draw = sphere_draw;
    task->destructor = sphere_destroy;
    work = task->work;
    sphere = syCalloc(1, sizeof(*sphere));
    if (!sphere) {
        /* @bug memory: reads the slot through a null extra, at 0x8c508d30,
         * where the sphere's record can't be allocated. task_destroy, at
         * 0x8c508d84, runs the destructor installed at 0x8c508d74 */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: task_destroy runs no
         * destructor */
        task->destructor = NULL;
#endif
        task_destroy(task);
        return NULL;
    }
    task->extra.p = sphere;
    sphere->target = *target;
    work->pos = finalhazard_center;
    sphere->scale.x = sphere->scale.y = sphere->scale.z = 1.0F;
    sphere->scale_phase.x = sphere->scale_phase.y = sphere->scale_phase.z =
        1.0F;
    sphere->kind = kind;
    sphere->slot = slot;
    sphere->hold = wave_hold(parent);
    work->mode = SPHERE_RISE;
    sphere->swing_step = ((Float)hover_step * rand() * (0.5F * RAND_SCALE)) +
                         (hover_step * 0.5F);
    if (!collision_init(task, &sphere_shape, 1, COLLISION_ENEMY)) {
        task_destroy(task);
        return NULL;
    }
    return task;
}

static void sphere_aim(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: an if (0) around a second read of the work */
    if (0) {
        work = task->work;
    }
    work->mode = SPHERE_CHARGE;
    work->submode = 0;
}
static void sphere_break(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: an if (0) around a return */
    if (0) {
        return;
    }
    work->mode = SPHERE_BURST;
    work->submode = 0;
}
static void sphere_begin_fade(struct task *task)
{
    struct task_work *work = task->work;
    struct last_boss_sphere *sphere = task->extra.p;
    work->mode = SPHERE_FADE;
    work->submode = 0;
    sphere_slots[sphere->slot] = NULL;
}
static void sphere_begin_lift(struct task *task)
{
    struct task_work *work = task->work;
    struct last_boss_sphere *sphere = task->extra.p;
    work->mode = SPHERE_LIFT;
    work->submode = 0;
    sphere_slots[sphere->slot] = NULL;
}
/* one of the wave's spheres, from a random slot on, or null where none is
 * left */
static struct task *sphere_choose(struct task *task)
{
    struct sphere_wave *wave = task->extra.p;
    Sint16 unscanned = wave->count;
    Sint16 index;
    struct task **slot;
    finalhazard_seed_random();
    index = (Float)rand() * wave->count * RAND_SCALE;
    slot = &sphere_slots[index];
    for (; unscanned > 0; unscanned--) {
        if (*slot) {
            return *slot;
        }
        if (++index == wave->count) {
            index = 0;
            slot = &sphere_slots[index];
        } else {
            /* @hack spelling: slot = slot + 1, where slot++ would do */
            slot = slot + 1;
        }
    }
    return NULL;
}

/* spawn the wave's spheres in rows on a cap of the shell. the cap spans
 * CAP_SPAN of pitch and CAP_SPAN of yaw, and its rows and columns split it
 * evenly. it lies on the active player's side: +z for player 0, and -z for
 * player 1. a repeating wave starts its volley REPEAT_WAIT steps after it
 * spawns */
#define CAP_SPAN 0x6000
#define REPEAT_WAIT 60

static void sphere_distribute(struct task *task)
{
    struct sphere_wave *wave = task->extra.p;
    struct task_work *work = task->work;
    Sint16 rows;
    Sint16 remainder;
    Sint8 extra;
    Sint16 columns;
    Angle row_step;
    Angle column_step;
    Angle pitch;
    Angle yaw;
    Sint8 row;
    Sint8 column;
    /* @bug memory: writes past sphere_slots, at 0x8c5090fe, where the debug
     * menu sets a count of 128. placed, a signed byte, wraps at 128, so the
     * test at 0x8c509100 never sees it reach the count */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above. placed counts past 127, so the
     * test sees it reach the count */
    Sint16 placed;
#else
    Sint8 placed;
#endif
    struct task **slot;
    /* @hack redundant: unused, a local that only sizes the frame */
    Uint8 unused[4];
    NJS_POINT3 ring;
    NJS_POINT3 position;
    Float radius;
    struct task *sphere;

    /* @hack redundant: an empty test of hold */
    if (wave->hold == 1) {
    }
    rows = njRoundOff(njSqrt(wave->count));
    /* @bug arithmetic: divides by 0 rows at 0x8c508fd6, where the wave's
     * count is 0. the debug menu can lower a count to 0, and so does a spawn
     * whose first sphere can't be made */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: a wave of no spheres finishes */
    if (!rows) {
        work->mode = WAVE_FINISHED;
        work->submode = 0;
        return;
    }
#endif
    remainder = wave->count - (rows * rows);
    extra = remainder % rows;
    if (extra == 0) {
        extra = remainder / rows;
    } else {
        extra = (remainder / rows) + 1;
    }
    columns = extra + rows;
    row_step = CAP_SPAN / (rows + 1);
    /* @hack spelling: a do while (0) around the three cursors' first values,
     * where three statements would do */
    do {
        pitch = -(CAP_SPAN / 2);
        placed = 0;
        slot = sphere_slots;
    } while (0);
    /* row and column are Sint8. a count of at most SPHERE_SLOTS gives at
     * most 12 rows and 12 columns */
    /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
    for (row = 0; row < rows; row++) {
        pitch += row_step;
        column_step = CAP_SPAN / (columns + 1);
        ring.x = 0.0F;
        ring.y = -shell.radius * njSin(pitch);
        ring.z = shell.radius * njCos(pitch);
        radius = ring.z;
        if (finalhazard_players->active == 0) {
            yaw = -QUARTER_TURN - (CAP_SPAN / 2);
        } else {
            yaw = QUARTER_TURN - (CAP_SPAN / 2);
        }
        for (column = 0; column < columns;) {
            yaw += column_step;
            position.x = radius * njCos(yaw);
            position.y = ring.y;
            position.z = -(radius * njSin(yaw));
            sphere = sphere_spawn(task, &position, 0, placed);
            if (!sphere) {
                wave->count = placed;
                /* @bug logic: returns in WAVE_SPAWN where a sphere can't be
                 * made, at 0x8c5090f2, with the spheres made so far alive.
                 * the count falls to theirs, and the next step spawns that
                 * many again over their slots */
#ifdef NONMATCHING
                /* this side repairs the bug tagged above. the wave goes on with
                 * the spheres made so far, or finishes where it made none */
                wave->remaining = placed;
                if (placed) {
                    goto ready;
                }
                work->mode = WAVE_FINISHED;
                work->submode = 0;
#endif
                return;
            }
            *slot = sphere;
            column++;
            placed++;
            if (placed == wave->count) {
                if (work->flags & WAVE_REPEATING) {
                    work->timer = REPEAT_WAIT;
                }
                work->mode = WAVE_READY;
                work->submode = 0;
                return;
            }
            slot++;
        }
    }
#ifdef NONMATCHING
/* where a wave cut short by a failed sphere goes on */
ready:
#endif
    if (work->flags & WAVE_REPEATING) {
        work->timer = REPEAT_WAIT;
    }
    work->mode = WAVE_READY;
    work->submode = 0;
}

/* finalhazard_sphere_volley, or a repeating wave, starts a volley's timer at
 * VOLLEY_DELAY. each time the timer runs out, the volley aims SHOTS spheres
 * from a random slot on. it then restarts the timer at VOLLEY_INTERVAL. a wave
 * whose count is spheres_few aims FEW_SHOTS instead. once no sphere is left,
 * the wave finishes. a repeating wave spawns again RESTART_WAIT steps
 * later */
#define VOLLEY_DELAY 30
#define VOLLEY_INTERVAL 30
#define SHOTS 2
#define FEW_SHOTS 1
#define RESTART_WAIT 120

static void sphere_wait_volley(struct task *task)
{
    struct task_work *work = task->work;
    if (work->flags & WAVE_REPEATING) {
        if (--work->timer == 0) {
            work->timer = VOLLEY_DELAY;
            work->mode = WAVE_VOLLEY;
            work->submode = 0;
        }
    }
}

static void sphere_volley(struct task *task)
{
    struct sphere_wave *wave = task->extra.p;
    struct task_work *work = task->work;
    Sint16 unscanned = wave->count;
    Sint16 fallback;
    /* @hack redundant: unused, a local that only sizes the frame */
    Sint32 unused[1];
    /* @hack type: index a Uint32, where a Sint16 like the counts would do */
    Uint32 index;
    Sint8 shots;
    struct task **slot;
    if (!task->child) {
        if (!(work->flags & WAVE_REPEATING)) {
            work->mode = WAVE_FINISHED;
        } else {
            work->timer = RESTART_WAIT;
            work->mode = WAVE_RESTART;
        }
        work->submode = 0;
    }
    if (work->timer == 0) {
        fallback = NO_SLOT;
        index = (Float)rand() * wave->count * RAND_SCALE;
        slot = &sphere_slots[index];
        if (wave->count == spheres_few) {
            shots = FEW_SHOTS;
        } else {
            shots = SHOTS;
        }
        while (shots > 0 && unscanned > 0) {
            if (*slot) {
                fallback = index;
                sphere_aim(*slot);
                *slot = NULL;
                index = (Float)rand() * wave->count * RAND_SCALE;
                wave->remaining--;
                shots--;
                if (shots > 0) {
                    unscanned = wave->count;
                    index = (Float)rand() * wave->count * RAND_SCALE;
                    slot = &sphere_slots[index];
                    continue;
                }
            }
            if (++index == (Uint32)wave->count) {
                index = 0;
                slot = &sphere_slots[index];
            } else {
                slot++;
            }
            unscanned--;
        }
        /* @hack redundant: an aim at fallback, the slot of the last sphere
         * aimed, which that aim emptied. it never aims */
        if (shots > 0 && unscanned == 0 && fallback != NO_SLOT) {
            slot = &sphere_slots[fallback];
            /* @hack redundant: an if (0) around an empty test of the slot */
            if (0) {
                if (*slot) {
                }
            }
            if (*slot) {
                sphere_aim(*slot);
                *slot = NULL;
                wave->remaining--;
            }
        }
        work->timer = VOLLEY_INTERVAL;
    } else {
        work->timer--;
    }
    /* @hack redundant: an empty test of hold */
    if (wave->hold == 1) {
    }
}

/* a wave clears STEP_SPHERES spheres a step, and disperses as many each
 * DISPERSE_INTERVAL steps */
#define STEP_SPHERES 2
#define DISPERSE_INTERVAL 5

static void sphere_disperse(struct task *task)
{
    struct task_work *work = task->work;
    Sint8 i;
    struct task *sphere;
    /* @hack redundant: wave, a read of the record that nothing uses */
    struct sphere_wave *wave = task->extra.p;
    if (game_state.steps % DISPERSE_INTERVAL == 0) {
        for (i = 0; i < STEP_SPHERES; i++) {
            sphere = sphere_choose(task);
            if (!sphere) {
                work->mode = WAVE_FINISHED;
                work->submode = 0;
                break;
            }
            sphere_begin_fade(sphere);
        }
    }
}

static void sphere_release(struct task *task)
{
    struct task_work *work = task->work;
    struct sphere_wave *wave = task->extra.p;
    Sint16 i;
    struct task **slot;
    switch (work->submode) {
    case 0:
        for (i = 0, slot = sphere_slots; i < wave->count; i++, slot++) {
            if (*slot) {
                sphere_begin_lift(*slot);
            }
        }
        work->submode++;
        /* fall through */
    case 1:
        if (!task->child) {
            work->mode = WAVE_FINISHED;
            work->submode = 0;
        }
        break;
    }
}

static void sphere_clear(struct task *task)
{
    struct task_work *work = task->work;
    Sint8 i;
    struct task *sphere;
    /* @hack redundant: wave, a read of the record that nothing uses */
    struct sphere_wave *wave = task->extra.p;
    for (i = 0; i < STEP_SPHERES; i++) {
        sphere = sphere_choose(task);
        if (!sphere) {
            work->mode = WAVE_FINISHED;
            work->submode = 0;
            break;
        }
        sphere_break(sphere);
    }
}

static void sphere_restart(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: an if (0) around a second read of the work */
    if (0) {
        work = task->work;
    }
    if (--work->timer == 0) {
        work->mode = WAVE_SPAWN;
        work->submode = 0;
    }
}

static void wave_step(struct task *task)
{
    struct task_work *work = task->work;
    if (game_state.time_stopped) {
        return;
    }
    if (game_state.unk_09) {
        return;
    }
    if (FINALHAZARD_STUNNED() == 1) {
        return;
    }
    if (finalhazard_players->transition != SWITCH_PLAY) {
        task_kill(task);
    } else {
        switch (work->mode) {
        case WAVE_SPAWN:
            sphere_distribute(task);
            break;
        case WAVE_READY:
            sphere_wait_volley(task);
            break;
        case WAVE_VOLLEY:
            sphere_volley(task);
            break;
        case WAVE_WAIT:
        case WAVE_FINISHED:
            break;
        case WAVE_CLEAR:
            sphere_clear(task);
            break;
        case WAVE_DISPERSE:
            sphere_disperse(task);
            break;
        case WAVE_RELEASE:
            sphere_release(task);
            break;
        case WAVE_RESTART:
            sphere_restart(task);
            break;
        }
    }
}

static void sphere_wave_destroy(struct task *task)
{
}
static void sphere_clear_slots(struct sphere_wave *wave);

struct task *
finalhazard_sphere_wave(struct task *parent,
                        const struct last_boss_sphere_obstacle *obstacles,
                        Sint16 count, Sint8 density)
{
    struct task *task;
    struct sphere_wave *wave;
    if (!parent) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged in the matching side: it returns
         * null */
        return NULL;
#else
        /* @bug uninitialized: returns no value for a null parent, at
         * 0x8c5095c8. the result then holds density */
        /* NOLINTNEXTLINE(clang-diagnostic-return-mismatch) */
        return;
#endif
    }
    task = task_create_child(TASK_WORK, wave_step, parent);
    if (!task) {
        return NULL;
    }
    task->destructor = sphere_wave_destroy;
    wave = syCalloc(1, sizeof(*wave));
    if (!wave) {
        task_destroy(task);
        return NULL;
    }
    task->extra.p = wave;
    switch (density) {
    case WAVE_FEW:
        wave->count = spheres_few;
        break;
    case WAVE_NORMAL:
        wave->count = spheres_normal;
        break;
    case WAVE_MANY:
        wave->count = spheres_many;
        break;
    }
    wave->remaining = wave->count;
    wave->hold = 0;
    if (count != 0) {
        wave->obstacles = obstacles;
        wave->obstacle_count = count;
    } else {
        wave->obstacles = NULL;
        wave->obstacle_count = 0;
    }
    sphere_clear_slots(wave);
    return task;
}

void finalhazard_sphere_volley(struct task *task)
{
    if (task) {
        struct task_work *work = task->work;
        work->timer = VOLLEY_DELAY;
        work->mode = WAVE_VOLLEY;
        work->submode = 0;
    }
}
void finalhazard_sphere_clear(struct task *task)
{
    if (task) {
        struct task_work *work = task->work;
        work->mode = WAVE_CLEAR;
        work->submode = 0;
    }
}
int finalhazard_sphere_finished(struct task *task)
{
    struct task_work *work;
    if (!task) {
        return 1;
    }
    work = task->work;
    if (work->mode == WAVE_FINISHED) {
        task_kill(task);
        return 1;
    }
    return 0;
}
int finalhazard_sphere_repeat(struct task *task)
{
    struct task_work *work;
    if (!task) {
        return -1;
    }
    work = task->work;
    work->flags |= WAVE_REPEATING;
    return 1;
}
static void wave_lose_sphere(struct task *task)
{
    struct sphere_wave *wave = task->extra.p;
    if (wave->remaining > 0) {
        wave->remaining--;
    }
}
/* @hack redundant: wave, a parameter nothing reads */
static void sphere_clear_slots(struct sphere_wave *wave)
{
    struct task **slot = sphere_slots;
    Uint8 i;
    for (i = 0; i < SPHERE_SLOTS; i++) {
        *slot = NULL;
        slot++;
    }
}

/* the settings the test menu's spheres page shows. L lowers a count only
 * while it is above 0. R raises it only while it is below SPHERE_SLOTS */
enum sphere_setting { SPHERES_FEW, SPHERES_NORMAL, SPHERES_MANY, SPHERE_SPEED };

static Sint8 selection = SPHERES_FEW;
/* @hack spelling: settings, a copy of the settings' count that the page
 * reads, where a constant would do */
static Sint8 settings = 4;
static Sint8 repeat_delay;

void finalhazard_debug_spheres(void)
{
    Sint8 i;
    for (i = 0; i < settings; i++) {
        if (i == selection) {
            njPrintColor(FINALHAZARD_MENU_SELECTED);
        }
        switch (i) {
        case SPHERES_FEW:
            njPrint(FINALHAZARD_SETTING_LINE(i), "IKURA NUM FEW %d",
                    spheres_few);
            break;
        case SPHERES_NORMAL:
            njPrint(FINALHAZARD_SETTING_LINE(i), "IKURA NUM NORMAL %d",
                    spheres_normal);
            break;
        case SPHERES_MANY:
            njPrint(FINALHAZARD_SETTING_LINE(i), "IKURA NUM MANY %d",
                    spheres_many);
            break;
        case SPHERE_SPEED:
            njPrint(FINALHAZARD_SETTING_LINE(i), "IKURA Speed %.3F",
                    sphere_speed);
            break;
        }
        if (i == selection) {
            njPrintColor(FINALHAZARD_MENU_TEXT);
        }
    }
    if (player_pad_records[finalhazard_players->active].press & PDD_DGT_TY) {
        selection++;
        if (selection == settings) {
            selection = SPHERES_FEW;
        }
    }
    if (player_pad_records[finalhazard_players->active].on & PDD_DGT_TL) {
        if (repeat_delay-- > 0) {
            return;
        }
        /* @hack spelling: a test of each setting, then one of its bound,
         * where one test of both would do */
        if (selection == SPHERES_FEW) {
            if (spheres_few > 0) {
                spheres_few--;
            }
        }
        if (selection == SPHERES_NORMAL) {
            if (spheres_normal > 0) {
                spheres_normal--;
            }
        }
        if (selection == SPHERES_MANY) {
            if (spheres_many > 0) {
                spheres_many--;
            }
        }
        if (selection == SPHERE_SPEED) {
            if (sphere_speed > 0.0F) {
                sphere_speed -= FINALHAZARD_MENU_SPEED_STEP;
            }
        }
        repeat_delay = FINALHAZARD_MENU_REPEAT;
    } else if (player_pad_records[finalhazard_players->active].on &
               PDD_DGT_TR) {
        if (repeat_delay-- > 0) {
            return;
        }
        if (selection == SPHERES_FEW) {
            if (spheres_few < SPHERE_SLOTS) {
                spheres_few++;
            }
        }
        if (selection == SPHERES_NORMAL) {
            if (spheres_normal < SPHERE_SLOTS) {
                spheres_normal++;
            }
        }
        if (selection == SPHERES_MANY) {
            if (spheres_many < SPHERE_SLOTS) {
                spheres_many++;
            }
        }
        if (selection == SPHERE_SPEED) {
            if (sphere_speed < FINALHAZARD_MENU_TOP_SPEED) {
                sphere_speed += FINALHAZARD_MENU_SPEED_STEP;
            }
        }
        repeat_delay = FINALHAZARD_MENU_REPEAT;
    } else {
        repeat_delay = FINALHAZARD_MENU_REPEAT;
    }
}

#define TRAIL_NAME "BL2IkuraSmokeExec"
#define TRAIL_COLOR_COUNT 16
#define TRAIL_LIFE 12
#define TRAIL_LEVEL 2
#define BURST_TEXLIST finalhazard_effect_texlist
#define BURST_MODEL finalhazard_sphere_flash
#include "shared/last_boss_burst_body.h"
