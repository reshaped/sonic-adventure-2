#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include <shinobi.h>
#include "boss_last1/combat_art.h"
#include "boss_last1/body.h"
#include "boss_last1/pose.h"
#include "system/angle.h"
#include "system/pad.h"
#include "boss_last1/arena.h"
#include "boss_last1/lift.h"
#include "boss_last1/sphere.h"
#include "camera/camera.h"
#include "collision/collision.h"
#include "draw/lines.h"
#include "effect/burst.h"
#include "player/player.h"
#include "shared/last_boss_arena.h"
#include "shared/last_boss_sphere.h"
#include "sound/sound.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"
#include "vmu/vibration.h"

/* a sphere's models at each of three levels of detail: a shell, then its
 * core. the draw takes a shell's core as the shell + 1 */
enum biolizard_sphere_mesh {
    SPHERE_NEAR_SHELL,
    SPHERE_NEAR_CORE,
    SPHERE_MIDDLE_SHELL,
    SPHERE_MIDDLE_CORE,
    SPHERE_FAR_SHELL,
    SPHERE_FAR_CORE,
    SPHERE_MESH_COUNT
};

/* clang-format off */
static const struct collision_shape sphere_shape = {
    0, SHAPE_SPHERE, PUSH_PLAYERS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
    SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(1),
    SHAPE_IN_WORLD | SHAPE_UNTURNED | SHAPE_IGNORE_ENEMIES |
        SHAPE_IGNORE_OBJECTS | SHAPE_IGNORE_PICKUPS,
    {0, 0, 0}, 5, 0, 0, 0, {0, 0, 0}};
/* clang-format on */
static const NJS_VECTOR burst_rise = {0, 0.2F, 0};
/* the latest wave's spheres, by slot. a slot is null once its sphere is
 * aimed, fades, lifts or dies. a new wave clears every slot */
#define SPHERE_SLOTS 128
static struct task *sphere_slots[SPHERE_SLOTS];
/* how many spheres a wave holds, and how many a lifted wave does. the debug
 * menu changes wave_spheres */
static Sint16 wave_spheres = 80;
static Sint16 lifted_wave_spheres = 55;
/* TRUE where the debug menu last showed the lifted wave's settings.
 * biolizard_preview_spheres makes that wave */
static Bool preview_lift = FALSE;
/* a hovering sphere that isn't lifted bobs by sphere_swing_height either way.
 * the bob's phase steps at a rate the sphere picks at random as it spawns.
 * the rate lies from half of hover_step up to hover_step */
static Angle hover_step = 0x200;
static Float sphere_swing_height = 10.0F;
/* the speed of an aimed sphere's flight. its first step covers
 * sphere_speed / d of its curve, where d is its distance from the target.
 * the debug menu changes it */
static Float sphere_speed = 4.5F;
/* each step of a lifted wave's volley adds orbit_ramp[RAMP_STEP] to the
 * wave's turn, up to orbit_ramp[RAMP_MOST] */
enum { RAMP_MOST, RAMP_STEP };
/* @hack type: orbit_ramp an array of two, where a record of the most and the
 * step would do */
static Angle orbit_ramp[] = {64, 4};
/* sphere_models compiled for njDirectDrawObject. a model that didn't fit
 * below DEMO_AREA is null */
static NJS_DIRECT_HEAD *compiled_meshes[SPHERE_MESH_COUNT] = {NULL};
static NJS_CNK_OBJECT *sphere_models[SPHERE_MESH_COUNT] = {
#include "boss_last1/spheres/sphere_models.inc"
};

#define SPHERE_BUILD_MODELS biolizard_build_models
#define SPHERE_TEXTURES biolizard_effect_texlist
#define SPHERE_MESHES compiled_meshes
#define SPHERE_RESET_CLIP
#include "shared/last_boss_sphere_motion.h"

/* a sphere rises from RISE_DEPTH to its height, and starts the faster the
 * higher it goes. each step it keeps RISE_DRAG of its speed, but no less
 * than RISE_MIN_SPEED */
#define RISE_DEPTH (-10.0F)
#define RISE_SPEED 7.0F
#define RISE_SPREAD 3.0F
#define RISE_GAIN 0.00015F
#define RISE_DRAG 0.8F
#define RISE_MIN_SPEED 1.5F

static void sphere_rise(struct task *task)
{
    struct task_work *work = task->work;
    struct last_boss_sphere *sphere = task->extra.p;
    switch (work->submode) {
    case 0:
        sphere->velocity.y =
            (RISE_SPEED + (rand() * (RISE_SPREAD * RAND_SCALE))) *
            sphere->target.y * sphere->target.y * RISE_GAIN;
        work->submode++;
        /* fall through */
    case 1:
        work->pos.y += sphere->velocity.y;
        sphere->velocity.y *= RISE_DRAG;
        if (sphere->velocity.y < RISE_MIN_SPEED) {
            sphere->velocity.y = RISE_MIN_SPEED;
        }
        if (work->pos.y >= sphere->target.y) {
            work->pos.y = sphere->target.y;
            sphere->velocity.y = 0.0F;
            last_boss_sphere_burst(task, &sphere->center, LAST_BOSS_BURST_LONG);
            work->mode = SPHERE_HOVER;
            work->submode = 0;
        }
        break;
    }
}

static void wave_orbit(struct task *task, NJS_POINT3 *position);
static void wave_lose_sphere(struct task *task);
static void sphere_hover(struct task *task)
{
    struct task_work *work = task->work;
    struct last_boss_sphere *sphere = task->extra.p;
    PULSE_SPHERE(sphere);
    if (sphere->hold == 1) {
        biolizard_lift_sphere(&work->pos, &sphere->velocity.y,
                              &sphere->lift_timer);
        wave_orbit(task->parent, &work->pos);
    } else {
        sphere->swing += sphere->swing_step;
        if (sphere->swing > FULL_TURN) {
            sphere->swing -= FULL_TURN;
        }
    }
}

/* an aimed sphere's timer counts down from CHARGE_STEPS before it flies at
 * player 0. in flight its attack and its guard are SHAPE_TOP_LEVEL */
#define CHARGE_STEPS 30
/* the flight's control points start FIRST_CONTROL and SECOND_CONTROL of the
 * way to the target */
#define FIRST_CONTROL 0.5F
#define SECOND_CONTROL 0.75F
/* the flight's first acceleration along its curve */
#define CURVE_ACCEL 0.0005F

static void sphere_charge(struct task *task)
{
    struct task_work *work = task->work;
    struct last_boss_sphere *sphere = task->extra.p;
    NJS_VECTOR direction;
    NJS_POINT3 point;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_VECTOR unused;
    Float radius;
    Float target_radius;
    Float start_radius;
    Float distance;

    switch (work->submode) {
    case 0:
        work->timer = CHARGE_STEPS;
        task->work->collision->flags &= ~COLLISION_TARGET;
        sphere->swing = 0;
        last_boss_sphere_burst(task, &work->pos, LAST_BOSS_BURST_SHORT);
        sound_play_at(SOUND_SPHERE_CHARGE, NULL, 0, 0, &work->pos);
        work->submode++;
        /* fall through */
    case 1:
        if (work->timer-- == 0) {
            sphere->target = player_work[0]->pos;
            sphere->start = work->pos;
            /* each control point then moves out along its line from the
             * origin. it ends as far out as the farther of the sphere and the
             * target. the flight then bows out around the arena */
            vector_diff(&sphere->target, &work->pos, &direction);
            SCALE_COMPONENTS(direction, FIRST_CONTROL);
            vector_sum(&work->pos, &direction, &point);
            target_radius = njScalor(&sphere->target);
            start_radius = njScalor(&work->pos);
            if (target_radius > start_radius) {
                radius = target_radius;
            } else {
                radius = start_radius;
            }
            direction = point;
            njUnitVector(&direction);
            SCALE_COMPONENTS(direction, radius);
            sphere->first_control = direction;
            vector_diff(&sphere->target, &work->pos, &direction);
            SCALE_COMPONENTS(direction, SECOND_CONTROL);
            vector_sum(&work->pos, &direction, &point);
            direction = point;
            njUnitVector(&direction);
            SCALE_COMPONENTS(direction, radius);
            sphere->second_control = direction;
            distance = vector_distance(&work->pos, &sphere->target);
            sphere->curve_speed =
                sphere_speed * njInvertSqrt(distance) * njInvertSqrt(distance);
            sphere->curve_time = 0.0F;
            sphere->curve_accel = CURVE_ACCEL;
            collision_set_attack(task, 0, SHAPE_TOP_LEVEL);
            collision_set_guard(task, 0, SHAPE_TOP_LEVEL);
            work->mode = SPHERE_FLY;
            work->submode = 0;
        }
        break;
    }
}

#define SPHERE_FINAL_APPROACH 10.0F
#define SPHERE_ARENA_LIMIT 500.0F
#define SPHERE_TRAIL_PARENT task
#include "shared/last_boss_sphere_flight.h"

/* a released sphere dies once above LIFT_TOP */
#define LIFT_TOP 400.0F

static void sphere_lift(struct task *task)
{
    struct task_work *work = task->work;
    struct last_boss_sphere *sphere = task->extra.p;
    biolizard_lift_sphere(&work->pos, &sphere->velocity.y, &sphere->lift_timer);
    wave_orbit(task->parent, &work->pos);
    if (work->pos.y > LIFT_TOP) {
        task_kill(task);
    }
}

#define BURST_SCALE 5.0F

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

/* a sphere enters the collision lists only within CONTACT_RANGE of player 0
 * across the ground */
#define CONTACT_RANGE 100.0F
/* a sphere that touches player 0 bursts. one that hurts player 0 gives it a
 * speed of KNOCKBACK through biolizard_push_player. one that can't throws it
 * up at BOUNCE_SPEED */
#define KNOCKBACK 5.0F
#define BOUNCE_SPEED 3.5F
/* the volume of a sphere's burst where it hurts player 0, and where it
 * doesn't. either rumbles player 0's pad by vibration_start's TOUCH_POWER and
 * TOUCH_FREQ */
#define HURT_VOLUME 30
#define BREAK_VOLUME (-10)
#define TOUCH_POWER 1
#define TOUCH_FREQ 5

static void sphere_contact(struct task *task)
{
    struct task_work *work = task->work;
    struct last_boss_sphere *sphere = task->extra.p;
    NJS_POINT3 *position = &work->pos;
    const NJS_POINT3 *player = &player_work[0]->pos;
    struct collision_hit *hit;

    sphere->player_distance = njSqrt(SPHERE_SQ(player->x - position->x) +
                                     SPHERE_SQ(player->z - position->z));
    if (sphere->player_distance > CONTACT_RANGE) {
        return;
    }
    hit = collision_next_player_hit(task);
    if (hit) {
        if (last_boss_hit_penetrates(task, hit) == 1) {
            biolizard_push_player(NULL, KNOCKBACK);
            sphere_explode(task);
            sound_play_at(SOUND_SPHERE_IMPACT, NULL, 0, HURT_VOLUME,
                          &work->pos);
            biolizard_mark_player_hurt();
        } else {
            player_set_speed(0, 0.0F, BOUNCE_SPEED, 0.0F);
            sound_play_at(SOUND_SPHERE_IMPACT, NULL, 0, BREAK_VOLUME,
                          &work->pos);
            sphere_explode(task);
        }
        vibration_start(0, TOUCH_POWER, TOUCH_FREQ, VIBRATION_UNTIMED);
    }
    collision_set_shape_center(task, 0, &sphere->center);
    collision_register(task);
}

static void sphere_camera_distance(struct task *task)
{
    struct task_work *work = task->work;
    struct last_boss_sphere *sphere = task->extra.p;
    const NJS_POINT3 *position = &work->pos;
    const NJS_POINT3 *eye = &camera_views[0]->pos;
    sphere->camera_distance = njSqrt(SPHERE_SQ(eye->x - position->x) +
                                     SPHERE_SQ(eye->y - position->y) +
                                     SPHERE_SQ(eye->z - position->z));
}

static void sphere_center(struct task *task)
{
    struct task_work *work = task->work;
    struct last_boss_sphere *sphere = task->extra.p;
    sphere->center = work->pos;
    sphere->center.y += sphere_swing_height * njSin(sphere->swing);
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
        sound_play_at(SOUND_SPHERE_BREAK, NULL, 0, BREAK_VOLUME, &work->pos);
        sphere_explode(task);
        break;
    case SPHERE_FADE:
        sphere_fade(task);
        break;
    case SPHERE_LIFT:
        sphere_lift(task);
        break;
    }
    sphere_camera_distance(task);
    sphere_center(task);
    sphere_contact(task);
}

/* a sphere draws its near models within NEAR_RANGE of camera 0, and its
 * middle ones within MIDDLE_RANGE. farther out it draws its far ones */
#define NEAR_RANGE 150.0F
#define MIDDLE_RANGE 200.0F

static void sphere_draw(struct task *task)
{
    struct last_boss_sphere *sphere = task->extra.p;
    struct task_work *work = task->work;
    struct angles facing;
    Sint8 mesh;

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
    njTranslate(NULL, 0.0F, sphere_swing_height * njSin(sphere->swing), 0.0F);
    if (sphere->camera_distance < NEAR_RANGE) {
        mesh = SPHERE_NEAR_SHELL;
    } else if (sphere->camera_distance < MIDDLE_RANGE) {
        mesh = SPHERE_MIDDLE_SHELL;
    } else {
        mesh = SPHERE_FAR_SHELL;
    }
    /* the far models are half spheres, which turn their domes to camera 0 */
    if (mesh == SPHERE_FAR_SHELL) {
        facing.y = last_boss_target_yaw(task, &camera_views[0]->pos);
        facing.x =
            last_boss_target_pitch(task, &camera_views[0]->pos, -QUARTER_TURN);
        njRotateY(NULL, facing.y);
        njRotateX(NULL, facing.x);
    } else {
        njRotateY(NULL, QUARTER_TURN);
    }
    njPushMatrixEx();
    njScaleEx(&sphere->scale);
    /* @bug memory: draws a null model, at 0x8c507b5a or 0x8c507b70, where
     * the model didn't fit in the buffer. njDirectDrawObject reads through
     * it at 0x8c12785e */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it draws no model that didn't
     * fit */
    if (compiled_meshes[mesh])
#endif
    {
        njDirectDrawObject(compiled_meshes[mesh]);
    }
    matrix_load_parent();
    njScaleEx(&sphere->scale_phase);
#ifdef NONMATCHING
    /* this side repairs the bug tagged above the first draw: it draws no model
     * that didn't fit */
    if (compiled_meshes[mesh + 1])
#endif
    {
        njDirectDrawObject(compiled_meshes[mesh + 1]);
    }
    njPopMatrixEx();
    njPopMatrixEx();
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

/* a wave's modes, in its work's mode. a wave waits in WAVE_READY for
 * biolizard_sphere_volley, and in WAVE_FINISHED for biolizard_sphere_finished
 * to end it. nothing sets WAVE_WAIT */
enum wave_mode {
    WAVE_SPAWN,
    WAVE_READY,
    WAVE_VOLLEY,
    WAVE_WAIT,
    WAVE_FINISHED,
    WAVE_CLEAR,
    WAVE_DISPERSE,
    WAVE_RELEASE
};
/* a wave's record, in its task's extra */
struct sphere_wave {
    Sint16 count; /* the spheres it spawns */
    /* its spheres neither aimed nor burst while rising or hovering */
    Sint16 remaining;
    Sint16 obstacle_count;
    /* the shapes its spheres spawn clear of, or null for none */
    const struct last_boss_sphere_obstacle *obstacles;
    Angle turn; /* the lifted wave's turn about the y axis each step */
    Bool lift; /* TRUE for the lifted wave */
};
/* a band of a wave's spheres */
struct sphere_band {
    /* the least and greatest distance from the origin across the ground */
    Float inner, outer;
    Float height;
    Float share; /* the band's share of the wave's spheres */
};

/* a sprite of the whole of texture 6, which nothing draws */
static NJS_TEXANIM sphere_cell = {
#include "boss_last1/spheres/sphere_cell.inc"
};
static NJS_SPRITE sphere_sprite = {
#include "boss_last1/spheres/sphere_sprite.inc"
};
/* a wave's bands from the lowest, by its lift: a wave's, then a lifted
 * wave's. a wave takes the shares of LOW_BAND and HIGH_BAND, and splits the
 * rest evenly between the middle two. the debug menu changes those two
 * shares */
#define BANDS 4
#define LOW_BAND 0
#define LOWER_MIDDLE_BAND 1
#define UPPER_MIDDLE_BAND 2
#define HIGH_BAND (BANDS - 1)
/* clang-format off */
static struct sphere_band bands[2][BANDS] = {
    {{150, 220, 30, 0.2F}, {150, 220, 80, 0.3F},
     {95, 180, 130, 0.3F}, {45, 130, 175, 0.1F}},
    {{215, 220, 30, 0.2F}, {165, 200, 80, 0.3F},
     {115, 150, 130, 0.3F}, {85, 100, 170, 0.1F}}};
/* clang-format on */
/* the sectors around the origin, by the angle sphere_distribute places a
 * sphere at. sector i ends before sector_ends[i], so 0xffff lies in none */
#define SECTORS 8
static Angle sector_ends[SECTORS] = {0x2c00, 0x4000, 0x5400, 0x7fff,
                                     0xac00, 0xc000, 0xd400, 0xffff};
/* the rows of biolizard_debug_spheres's menu, and the row it selects. Y steps
 * through the rows, and L and R held lower and raise the selected row's
 * setting */
enum setting_row { ROW_COUNT, ROW_LOW_SHARE, ROW_HIGH_SHARE, ROW_SPEED, ROWS };
static Sint8 debug_row = ROW_COUNT;
/* @hack spelling: debug_rows, a copy of ROWS that the menu reads, where ROWS
 * would do */
static Sint8 debug_rows = ROWS;

#pragma inline(wave_lift)
static Bool wave_lift(const struct task *controller)
{
    const struct sphere_wave *wave = controller->extra.p;
    return wave->lift;
}

static struct task *sphere_spawn(struct task *parent,
                                 const NJS_POINT3 *position, Sint8 sector,
                                 Sint8 slot)
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
    work = task->work;
    sphere = syCalloc(1, sizeof(*sphere));
    if (!sphere) {
        task_kill(task);
        return NULL;
    }
    task->extra.p = sphere;
    work->pos = *position;
    sphere->target.y = work->pos.y;
    work->pos.y = RISE_DEPTH;
    task->draw = sphere_draw;
    task->destructor = sphere_destroy;
    sphere->scale.x = sphere->scale.y = sphere->scale.z = 1.0F;
    sphere->scale_phase.x = sphere->scale_phase.y = sphere->scale_phase.z =
        1.0F;
    sphere->kind = sector;
    sphere->slot = slot;
    sphere->hold = wave_lift(parent);
    sphere->swing_step = ((Float)hover_step * rand() * (0.5F * RAND_SCALE)) +
                         (hover_step * 0.5F);
    if (!collision_init(task, &sphere_shape, 1, COLLISION_ENEMY)) {
        task_kill(task);
        return NULL;
    }
    return task;
}

static void sphere_aim(struct task *task)
{
    struct task_work *work = task->work;
    work->mode = SPHERE_CHARGE;
    work->submode = 0;
    /* @hack redundant: an if (0) around a test of task */
    if (0) {
        if (task) {
        }
    }
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

/* a wave that isn't lifted aims from AIM_SECTORS sectors. they are the
 * sector of player 0's bearing less the Biolizard's front's, and the one
 * either side */
#define AIM_SECTORS 3

/* 1 where task's sphere lies in one of the AIM_SECTORS in sectors, else 0.
 * -1 for a null task, or a task without a sphere */
static Sint8 sphere_in_sectors(const struct task *task, const Sint8 *sectors)
{
    const struct last_boss_sphere *sphere;
    const Sint8 *sector;
    Sint8 i;
    if (!task) {
        return -1;
    }
    sphere = task->extra.p;
    if (!sphere) {
        return -1;
    }
    sector = sectors;
    for (i = 0; i < AIM_SECTORS; i++, sector++) {
        if (*sector == sphere->kind) {
            return 1;
        }
    }
    return 0;
}

static Sint8 sphere_sector(Angle angle)
{
    const Angle *end = sector_ends;
    Sint8 sector;
    angle = (Uint16)angle;
    for (sector = 0; sector < SECTORS; sector++, end++) {
        /* @hack redundant: angle >= 0, which the cast to Uint16 makes true */
        if (angle >= 0 && angle < *end) {
            return sector;
        }
    }
    /* @bug uninitialized: returns no value for an angle of 0xffff, which
     * lies in no sector, at 0x8c507e42 */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: the angle 0xffff takes the last
     * sector */
    return SECTORS - 1;
#endif
    /* NOLINTNEXTLINE(clang-diagnostic-return-type) */
}

static void sphere_neighbors(Sint8 sector, Sint8 *sectors)
{
    sectors[0] = sector - 1;
    sectors[1] = sector;
    sectors[2] = sector + 1;
    if (sector == SECTORS - 1) {
        sectors[2] = 0;
    }
    if (sector == 0) {
        sectors[0] = SECTORS - 1;
    }
}

/* one of the wave's spheres, from a random slot on, or null where none is
 * left */
static struct task *sphere_choose(struct task *task)
{
    struct sphere_wave *wave = task->extra.p;
    Sint16 unscanned = wave->count;
    Sint16 index;
    struct task **first;
    struct task **slot;
    biolizard_seed_random();
    index = rand() * (Float)wave->count * RAND_SCALE;
    first = sphere_slots;
    slot = &sphere_slots[index];
    while (unscanned > 0) {
        if (*slot) {
            return *slot;
        }
        index++;
        if (index == wave->count) {
            index = 0;
            slot = first;
        } else {
            slot++;
        }
        unscanned--;
    }
    return NULL;
}

static const NJS_POINT3 radial_axis = {1, 0, 0};

/* spawn the wave's spheres, the lowest band first. a band's spheres split
 * between ARCS arcs as evenly as they go, the first arcs taking any extra.
 * two arcs spread from biolizard_body_angle + HALF_TURN and two from
 * biolizard_body_angle, one each way. a sphere at angle a lies at
 * (r cos a, height, -r sin a). r lies at random between its band's radii. a
 * sphere inside an obstacle moves out along a by its depth inside */
#define ARCS 4
/* the angle an arc spans, in a wave and in a lifted wave */
#define ARC_WIDTH 0x2800
#define LIFTED_ARC_WIDTH 0x3c00

static void sphere_distribute(struct task *task)
{
    struct task_work *work = task->work;
    struct sphere_wave *wave = task->extra.p;
    Sint8 lift;
    struct sphere_band *band_table;
    Sint8 counts[BANDS];
    /* @bug memory: writes past sphere_slots, at 0x8c50816e, where the debug
     * menu sets a count of 128 and band shares that sum past 1. placed, a
     * signed byte, wraps at 0x8c50817a, so the test at 0x8c508182 never sees
     * it reach the count */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above. placed counts past 127, so the
     * test sees it reach the count */
    Sint16 placed;
#else
    Sint8 placed;
#endif
    struct task **slot;
    Sint8 band;
    Sint16 per_arc;
    Sint16 extra;
    Sint16 arc;
    Sint16 count;
    Angle step;
    Angle angle;
    Angle start;
    Sint8 i;
    Float inner;
    Float outer;
    Float thickness;
    Float height;
    Float radius;
    /* @hack redundant: axis, a copy of radial_axis for each sphere, which
     * nothing reads */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    NJS_POINT3 axis;
    NJS_POINT3 position;
    NJS_VECTOR offset;
    const struct last_boss_sphere_obstacle *obstacle;
    Uint16 obstacle_index;
    Float distance;
    struct task *sphere;
    Sint8 sector;

    /* @hack spelling: lift set to 1 or 0 by a test of wave->lift, where
     * lift = wave->lift == 1 would do */
    if (wave->lift == 1) {
        lift = 1;
    } else {
        lift = 0;
    }
    band_table = bands[lift];
    counts[LOW_BAND] = wave->count * band_table[LOW_BAND].share;
    counts[HIGH_BAND] = wave->count * band_table[HIGH_BAND].share;
    counts[LOWER_MIDDLE_BAND] =
        (wave->count - counts[LOW_BAND] - counts[HIGH_BAND]) * 0.5F;
    counts[UPPER_MIDDLE_BAND] =
        (wave->count - counts[LOW_BAND] - counts[HIGH_BAND]) * 0.5F;
    placed = 0;
    slot = sphere_slots;
    band = 0;
    /* @hack redundant: a second band = 0, in a block no path reaches */
    if (0) {
        band = 0;
    }
    do {
        /* @hack redundant: a test of band, in a block no path reaches */
        if (0) {
            if (band) {
            }
        }
        per_arc = counts[band] / ARCS;
        extra = counts[band] - (per_arc * ARCS);
        for (arc = 0; arc < ARCS; arc++) {
            count = per_arc;
            if (extra > 0) {
                count++;
                extra--;
            }
            /* @bug arithmetic: divides by an arc's count of 0 at
             * 0x8c508020. the debug menu's count and shares can leave an arc
             * no spheres */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: it skips an arc of no
             * spheres */
            if (count <= 0) {
                continue;
            }
#endif
            if (lift) {
                step = LIFTED_ARC_WIDTH / count;
            } else {
                step = ARC_WIDTH / count;
            }
            if (arc < ARCS / 2) {
                start = biolizard_body_angle() + HALF_TURN;
            } else {
                start = biolizard_body_angle();
            }
            if (arc == 1 || arc == 3) {
                step = -step;
            }
            i = 0;
            angle = (Uint16)((step / 2) + start);
            inner = band_table[band].inner;
            outer = band_table[band].outer;
            height = band_table[band].height;
            thickness = outer - inner;
            while (i < count) {
                axis = radial_axis;
                biolizard_seed_random();
                radius = inner + (rand() * thickness * RAND_SCALE);
                position.x = radius * njCos(angle);
                position.y = height;
                position.z = -(radius * njSin(angle));
                if (wave->obstacles) {
                    obstacle = wave->obstacles;
                    for (obstacle_index = 0;
                         obstacle_index < wave->obstacle_count;
                         obstacle_index++, obstacle++) {
                        vector_diff(&position, &obstacle->position, &offset);
                        distance = njScalor(&offset);
                        if (obstacle->radius > distance) {
                            radius = obstacle->radius - distance;
                            position.x += radius * njCos(angle);
                            position.z += -(radius * njSin(angle));
                        }
                    }
                }
                sector = sphere_sector(angle);
                sphere = sphere_spawn(task, &position, sector, placed);
                if (!sphere) {
                    /* @bug logic: returns in WAVE_SPAWN where a sphere
                     * can't be made, at 0x8c508168, with the spheres made
                     * so far alive. the next step spawns the whole wave
                     * again beside them */
#ifdef NONMATCHING
                    /* this side repairs the bug tagged above. it destroys the
                     * spheres made so far, so that the next step's wave doesn't
                     * stand beside them */
                    while (slot != sphere_slots) {
                        slot--;
                        task_destroy(*slot);
                    }
#endif
                    return;
                }
                *slot = sphere;
                i++;
                placed++;
                angle += step;
                if (placed == wave->count) {
                    work->mode = WAVE_READY;
                    work->submode = 0;
                    return;
                }
                slot++;
            }
        }
    } while (++band < BANDS);
    work->mode = WAVE_READY;
    work->submode = 0;
}

static void sphere_player_sectors(Sint8 *sectors)
{
    sphere_neighbors(sphere_sector(last_boss_angle_difference(
                         &biolizard_joint_positions[BIOLIZARD_JOINT_FRONT],
                         &player_work[0]->pos)),
                     sectors);
}

/* biolizard_sphere_volley starts a volley's timer at VOLLEY_DELAY. when the
 * timer runs out, the volley aims SHOTS spheres, or LIFTED_SHOTS in a lifted
 * wave. the timer then restarts at VOLLEY_INTERVAL. a wave that isn't lifted
 * aims a sphere in AIM_SECTORS where it finds one. else it aims the last
 * sphere its scan passed. a lifted wave holds while player 0 is within
 * SUPPORT_RANGE of the life support across the ground. the volley ends once
 * no more than VOLLEY_SHARE of the wave remains */
#define VOLLEY_DELAY 180
#define VOLLEY_INTERVAL 60
#define SHOTS 1
#define LIFTED_SHOTS 2
#define SUPPORT_RANGE 80.0F
#define VOLLEY_SHARE 0.5F

static void sphere_volley(struct task *task)
{
    struct sphere_wave *wave = task->extra.p;
    struct task_work *work = task->work;
    Sint16 unscanned = wave->count;
    struct task *life_support;
    Sint16 fallback;
    Sint8 sectors[AIM_SECTORS];
    Uint32 index;
    struct task **first;
    struct task **slot;
    struct task **candidate;
    Sint8 shots;
    /* @hack type: eligible a Float that holds 0 or 1, where an int would do */
    Float eligible;
    Float limit;
    Float step;
    if (!(wave->remaining > wave->count * VOLLEY_SHARE)) {
        if (wave->lift) {
            work->mode = WAVE_RELEASE;
            work->submode = 0;
            biolizard_lift_release();
        } else {
            work->mode = WAVE_DISPERSE;
            work->submode = 0;
        }
        return;
    }
    if (wave->lift) {
        life_support = biolizard_life_support_task();
        /* @bug memory: reads through null at 0x8c5082c8 where
         * biolizard_life_support_task, at 0x8c5082c2, finds no life support */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: without the life support, it
         * returns */
        if (!life_support) {
            return;
        }
#endif
        if (life_support->work) {
            if (njSqrt(SPHERE_SQ(life_support->work->pos.x -
                                 player_work[0]->pos.x) +
                       SPHERE_SQ(life_support->work->pos.z -
                                 player_work[0]->pos.z)) < SUPPORT_RANGE) {
                return;
            }
        }
    }
    if (work->timer == 0) {
        fallback = -1;
        sphere_player_sectors(sectors);
        biolizard_seed_random();
        index = rand() * (Float)wave->count * RAND_SCALE;
        candidate = &sphere_slots[index];
        /* @hack spelling: first set in each arm of the test, where one
         * statement before it would do */
        if (wave->lift == 1) {
            shots = LIFTED_SHOTS;
            first = sphere_slots;
        } else {
            shots = SHOTS;
            first = sphere_slots;
        }
        /* @hack redundant: a store of 0 to unscanned, in a block no path
         * reaches */
        if (0) {
            unscanned = 0;
        }
        /* @hack redundant: a test of first, in a block no path reaches */
        if (0) {
            if (first) {
            }
        }
        /* @hack spelling: a do while inside an if of its own test, where a
         * while would do */
        if (shots > 0 && unscanned > 0) {
            do {
                if (*candidate) {
                    fallback = index;
                    eligible = 0.0F;
                    if (wave->lift == 1) {
                        eligible = 1.0F;
                    } else if (sphere_in_sectors(*candidate, sectors) == 1) {
                        eligible = 1.0F;
                    }
                    if (eligible == 1.0F) {
                        sphere_aim(*candidate);
                        *candidate = NULL;
                        index = rand() * (Float)wave->count * RAND_SCALE;
                        /* @hack spelling: wave->remaining assigned itself
                         * less 1, here and for the fallback, where -- would
                         * do */
                        wave->remaining = wave->remaining - 1;
                        shots--;
                        if (shots > 0) {
                            unscanned = wave->count;
                            index = rand() * (Float)wave->count * RAND_SCALE;
                            candidate = &sphere_slots[index];
                            continue;
                        }
                    }
                }
                index++;
                unscanned--;
                if (index == (Uint32)wave->count) {
                    index = 0;
                    candidate = first;
                } else {
                    candidate++;
                }
            } while (shots > 0 && unscanned > 0);
        }
        if (shots > 0 && unscanned == 0 && fallback != -1) {
            slot = &sphere_slots[fallback];
            if (*slot) {
                sphere_aim(*slot);
                *slot = NULL;
                wave->remaining = wave->remaining - 1;
            }
        }
        work->timer = VOLLEY_INTERVAL;
    } else {
        work->timer--;
    }
    if (wave->lift == 1) {
        limit = orbit_ramp[RAMP_MOST];
        if (limit > wave->turn) {
            step = orbit_ramp[RAMP_STEP];
            wave->turn += step;
            if (wave->turn > limit) {
                wave->turn = limit;
            }
        }
    }
}

/* a wave clears STEP_SPHERES spheres a step, and disperses as many each
 * DISPERSE_INTERVAL steps */
#define STEP_SPHERES 2
#define DISPERSE_INTERVAL 5

static void sphere_disperse(struct task *task)
{
    struct task_work *work = task->work;
    struct task *sphere;
    Sint8 i;
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
    /* @hack redundant: an if (0) around a test of task */
    if (0) {
        if (task) {
        }
    }
}

static void sphere_release_lift(struct task *task)
{
    struct task_work *work = task->work;
    struct sphere_wave *wave = task->extra.p;
    Sint16 i;
    struct task **slot;
    switch (work->submode) {
    case 0:
        slot = sphere_slots;
        for (i = 0; i < wave->count; i++, slot++) {
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
    struct task *sphere;
    Sint8 i;
    for (i = 0; i < STEP_SPHERES; i++) {
        sphere = sphere_choose(task);
        if (!sphere) {
            work->mode = WAVE_FINISHED;
            work->submode = 0;
            break;
        }
        sphere_break(sphere);
    }
    /* @hack redundant: an if (0) around a test of task */
    if (0) {
        if (task) {
        }
    }
}

static void wave_step(struct task *task)
{
    struct task_work *work = task->work;
    if (game_state.time_stopped || game_state.unk_09) {
        return;
    }
    if (work->mode == WAVE_VOLLEY && biolizard_player_hurt() == 1) {
        return;
    }
    switch (work->mode) {
    case WAVE_SPAWN:
        sphere_distribute(task);
        break;
    case WAVE_READY:
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
        sphere_release_lift(task);
        break;
    }
}

static void sphere_wave_destroy(struct task *task)
{
    /* @hack redundant: an if (0) around a biolizard_seed_random, and the
     * return at the end */
    if (0) {
        biolizard_seed_random();
    }
    sound_stop_key(task);
    /* NOLINTNEXTLINE(readability-redundant-control-flow) */
    return;
}

static void sphere_clear_slots(void);

struct task *
biolizard_sphere_wave(struct task *parent, Sint32 lift,
                      const struct last_boss_sphere_obstacle *obstacles,
                      Sint16 count)
{
    struct task *task;
    struct sphere_wave *wave;
    struct sphere_wave *extra;
    if (parent) {
        task = task_create_child(TASK_WORK, wave_step, parent);
        if (!task) {
            return NULL;
        }
        /* @hack spelling: the record kept through extra and read back from
         * task->extra as wave, where wave alone would do */
        extra = syCalloc(1, sizeof(*extra));
        if (!extra) {
            task_kill(task);
            return NULL;
        }
        task->extra.p = extra;
        wave = task->extra.p;
        wave->lift = lift;
        if (wave->lift) {
            wave->count = lifted_wave_spheres;
            wave->remaining = lifted_wave_spheres;
        } else {
            wave->count = wave_spheres;
            wave->remaining = wave_spheres;
        }
        if (count != 0) {
            wave->obstacles = obstacles;
            wave->obstacle_count = count;
        } else {
            wave->obstacles = NULL;
            wave->obstacle_count = 0;
        }
        task->destructor = sphere_wave_destroy;
        sphere_clear_slots();
        return task;
    }
    /* @bug uninitialized: returns no value for a null parent, at
     * 0x8c50879e */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it returns null */
    return NULL;
#endif
    /* NOLINTNEXTLINE(clang-diagnostic-return-type) */
}

void biolizard_sphere_volley(struct task *task)
{
    struct task_work *work;
    if (task) {
        work = task->work;
        work->timer = VOLLEY_DELAY;
        work->mode = WAVE_VOLLEY;
        work->submode = 0;
    }
}

void biolizard_sphere_clear(struct task *task)
{
    struct task_work *work;
    if (task) {
        work = task->work;
        work->mode = WAVE_CLEAR;
        work->submode = 0;
    }
}

int biolizard_sphere_finished(struct task *task)
{
    struct sphere_wave *wave;
    struct task_work *work;
    /* @bug memory: reads task->extra through a null task at 0x8c5087d4,
     * before the test that returns 1 for it */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it returns 1 for a null task
     * before the read */
    if (!task) {
        return 1;
    }
#endif
    wave = task->extra.p;
    if (!task) {
        return 1;
    }
    work = task->work;
    if (work->mode == WAVE_FINISHED) {
        task_kill(task);
        if (wave->lift) {
            biolizard_lift_stop();
        }
        return 1;
    }
    return 0;
}

static void wave_lose_sphere(struct task *task)
{
    struct sphere_wave *wave = task->extra.p;
    if (wave->remaining > 0) {
        wave->remaining--;
    }
}

static void wave_orbit(struct task *task, NJS_POINT3 *position)
{
    struct sphere_wave *wave = task->extra.p;
    Angle turn = wave->turn;
    NJS_POINT3 next;
    next = *position;
    next.x = (next.z * njSin(turn)) + (next.x * njCos(turn));
    /* @bug logic: turns z by the x already turned, not the old x, so each
     * step scales the orbit's area by cos(turn) squared. the new x is stored
     * at 0x8c50887e and read back at 0x8c508892 */
    next.z = (next.z * njCos(turn)) - (next.x * njSin(turn));
    *position = next;
}

static void sphere_clear_slots(void)
{
    Uint8 i;
    struct task **slot = sphere_slots;
    for (i = 0; i < SPHERE_SLOTS; i++, slot++) {
        *slot = NULL;
    }
}

/* where the menu prints a row */
#define SETTING_LINE(row)                                                      \
    NJM_LOCATION(BIOLIZARD_MENU_COLUMN, (row) + BIOLIZARD_MENU_SETTING_ROW)
/* the menu changes a share or the speed by SETTING_STEP. a share goes up to
 * MOST_SHARE, and the speed to MOST_SPEED */
#define SETTING_STEP 0.01F
#define MOST_SHARE 1.0F
#define MOST_SPEED 10.0F

void biolizard_debug_spheres(int lift)
{
    Sint8 i;
    struct sphere_band *band_table;
    if (lift == 1) {
        band_table = bands[TRUE];
        preview_lift = TRUE;
    } else {
        band_table = bands[FALSE];
        preview_lift = FALSE;
    }
    for (i = 0; i < debug_rows; i++) {
        if (i == debug_row) {
            njPrintColor(BIOLIZARD_MENU_SELECTED);
        }
        switch (i) {
        case ROW_COUNT:
            njPrint(SETTING_LINE(i), "IKURA NUM %d", wave_spheres);
            break;
        case ROW_LOW_SHARE:
            njPrint(SETTING_LINE(i), "3M   %.2f", band_table[LOW_BAND].share);
            break;
        case ROW_HIGH_SHARE:
            njPrint(SETTING_LINE(i), "12M %.2f", band_table[HIGH_BAND].share);
            break;
        case ROW_SPEED:
            njPrint(SETTING_LINE(i), "Speed %.2f", sphere_speed);
            break;
        }
        if (i == debug_row) {
            njPrintColor(BIOLIZARD_MENU_TEXT);
        }
    }
    if (player_pad_records[0].press & PDD_DGT_TY) {
        debug_row++;
        if (debug_row == debug_rows) {
            debug_row = 0;
        }
    }
    if (player_pad_records[0].on & PDD_DGT_TL) {
        /* @hack spelling: a test of the row, then one of its bound, where one
         * test of both would do */
        if (debug_row == ROW_COUNT) {
            if (wave_spheres != 0) {
                wave_spheres--;
            }
        }
        if (debug_row == ROW_LOW_SHARE) {
            /* @bug logic: the tests at 0x8c508a3a, 0x8c508a52 and 0x8c508a6a
             * never stop a share or the speed at 0. each starts at a value
             * that steps past 0 by SETTING_STEP without landing on it, so it
             * falls below 0 for as long as L is held */
            if (band_table[LOW_BAND].share != 0.0F) {
                band_table[LOW_BAND].share -= SETTING_STEP;
            }
        }
        if (debug_row == ROW_HIGH_SHARE) {
            if (band_table[HIGH_BAND].share != 0.0F) {
                band_table[HIGH_BAND].share -= SETTING_STEP;
            }
        }
        if (debug_row == ROW_SPEED) {
            if (sphere_speed != 0.0F) {
                sphere_speed -= SETTING_STEP;
            }
        }
    } else if (player_pad_records[0].on & PDD_DGT_TR) {
        if (debug_row == ROW_COUNT) {
            if (wave_spheres < SPHERE_SLOTS) {
                wave_spheres++;
            }
        }
        if (debug_row == ROW_LOW_SHARE) {
            if (band_table[LOW_BAND].share < MOST_SHARE) {
                band_table[LOW_BAND].share += SETTING_STEP;
            }
        }
        if (debug_row == ROW_HIGH_SHARE) {
            if (band_table[HIGH_BAND].share < MOST_SHARE) {
                band_table[HIGH_BAND].share += SETTING_STEP;
            }
        }
        if (debug_row == ROW_SPEED) {
            if (sphere_speed < MOST_SPEED) {
                sphere_speed += SETTING_STEP;
            }
        }
    }
}

struct task *
biolizard_preview_spheres(struct task *parent,
                          const struct last_boss_sphere_obstacle *obstacles,
                          Sint16 count)
{
    Bool lift = preview_lift;
    struct task *wave;
    if (lift == 1) {
        wave = biolizard_sphere_wave(parent, lift, NULL, 0);
        biolizard_select_attack(BIOLIZARD_ATTACK_LIFTED_WAVE);
    } else {
        wave = biolizard_sphere_wave(parent, lift, obstacles, count);
        biolizard_select_attack(BIOLIZARD_ATTACK_WAVE);
    }
    return wave;
}

#define TRAIL_CROSSED
#define RINGS_FOLLOW_ORIGIN
#define TRAIL_NAME "BL1IkuraSmokeExec"
#define TRAIL_COLOR_COUNT 21
#define TRAIL_LIFE 20
#define TRAIL_LEVEL 3
#define BURST_TEXLIST biolizard_effect_texlist
#define BURST_MODEL biolizard_sphere_flash
#include "shared/last_boss_burst_body.h"
