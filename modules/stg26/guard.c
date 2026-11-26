/* the guards: two robots that patrol a closed path through Egg Quarters. a
 * guard whose sensors find player 0 locks on, and attacks once the lock has
 * held guard_lock_steps. a touch of its body attacks at once. an attack calls
 * down the guard eyes, and the other guard waits until it ends */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camset.h"
#include "camera/zones.h"
#include "collision/collision.h"
#include "draw/chunk.h"
#include "draw/color.h"
#include "draw/lines.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/fog.h"
#include "shared/object_builds.h"
#include "sound/sound.h"
#include "stage/path.h"

#define GUARDS 2
/* defined above the includes, guard_positions and guards lie in bss ahead
 * of what guard.h and guard_eye.h declare. the image has them there */
struct guard_state;
static NJS_POINT3 guard_positions[GUARDS];
static struct guard_state *guards[GUARDS];

#include "stg26/guard_art.h"
#include "stg26/guard.h"
#include "stg26/guard_eye.h"
#include "stg26/guard_patrol.h"
#include "stg26/hiding_spot.h"
#include "stg26/quarters_area.h"
#include "stg26/quarters_util.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/matrix.h"
#include "system/task.h"
#include "system/util.h"

#define GUARD_LEVEL 2
#define RADAR_LEVEL 1
#define SENSOR_SHAPES 2
#define GUARD_ZONE_RADIUS 22.5F
/* the guards start GUARD_SPACING of the path apart */
#define GUARD_SPACING 0.5F
/* after an attack, a guard can't lock on or attack until it has patrolled
 * for COOLDOWN_STEPS */
#define COOLDOWN_STEPS 600

/* the guards' sounds, ids in the stage's sound list. a play of ENGINE_SOUND
 * or LOCK_SOUND lasts SOUND_STEPS + 1 updates, as sound_play_at_for says */
#define ENGINE_SOUND 2
#define RADAR_SOUND 3
#define LOCK_SOUND 4
#define ATTACK_SOUND 5
#define SOUND_STEPS 15
#define ENGINE_PRIORITY 1
#define RADAR_PRIORITY 1
#define LOCK_PRIORITY 0
#define ATTACK_PRIORITY 0
/* a sound's key is its task's address plus ATTACK_KEY, ENGINE_KEY or
 * RADAR_KEY. LOCK_SOUND's key is the guard's task itself */
#define ATTACK_KEY 1
#define ENGINE_KEY 2
#define RADAR_KEY 3

/* the sensors glow green while the guard cools down, and red otherwise.
 * alpha and the glow's own color take GLOW_FULL, and the other colors
 * GLOW_DIM */
#define GLOW_FULL 1.0F
#define GLOW_DIM 0.3F
/* a guard banks into a turn by TURN_BANK times the turn's angle, through a
 * damped spring */
#define TURN_BANK 24.0F
#define BANK_SPRING 0.003F
#define BANK_DAMPING 0.04F
/* the guard aims its lens at AIM_HEIGHT above player 0's pos */
#define AIM_HEIGHT 5.0F
#define SHADOW_SCALE_Y 1.2F

/* (njCos(angle) - COS_EIGHTH_TURN) * FADE_SCALE falls from 1 at an angle of
 * 0 to 0 at system/angle.h's EIGHTH_TURN. a sprite fades by it */
#define COS_EIGHTH_TURN 0.7071F
#define FADE_SCALE (1.0F / (1.0F - COS_EIGHTH_TURN))
#define LENS_STEPS 8
/* a lens flash's alpha is LENS_ALPHA_SHARE of lens_opacity at its
 * brightest */
#define LENS_ALPHA_SHARE 0.75F
/* an exhaust puff is white. its alpha is EXHAUST_ALPHA, 3/4 of 255, at its
 * brightest. a puff's size takes a random scale of EXHAUST_SCALE_MIN, plus up
 * to EXHAUST_SCALE_SPREAD */
#define EXHAUST_COLORS 0xffffff
#define EXHAUST_ALPHA 191.25F
#define EXHAUST_SCALE_MIN 0.9F
#define EXHAUST_SCALE_SPREAD 0.2F

/* a distance past any guard's, where the radar's search starts */
#define FAR_AWAY 99999.0F
/* the radar beeps every guard_radar_period steps at a fog speed of
 * BEEP_SPEED. at a slower speed, it beeps less often */
#define BEEP_SPEED 2.0F
/* the radar's volume while player 0 hides, a share of guard_radar_volume */
#define HIDDEN_VOLUME_SHARE 0.6666667F

enum guard_mode {
    NO_NEXT_MODE = -1, /* next_mode while no mode waits to start */
    GUARD_WAIT,
    GUARD_PATROL,
    GUARD_LOCK,
    GUARD_ATTACK,
    GUARD_MODES
};

/* a lens flash or a puff of exhaust, in its guard's list */
struct guard_sprite {
    NJS_POINT3 pos;
    Float scale;
    Sint32 age;
    struct guard_sprite *prev;
    struct guard_sprite *next;
    struct task *guard; /* the task whose list holds it */
};

struct guard_state {
    Sint32 id; /* its index in guards */
    enum guard_mode mode;
    /* the mode guard_step_mode starts next, or NO_NEXT_MODE */
    enum guard_mode next_mode;
    Sint32 phase; /* 0 at each mode's start, which no mode reads */
    Sint32 steps; /* the steps since the mode started */
    /* the patrol steps left before the guard can lock on again, or 0 */
    Sint32 cooldown;
    /* the placement's pos and ang, and the pos before the latest move. no
     * code reads them */
    NJS_POINT3 home;
    struct angles home_ang;
    NJS_POINT3 previous_pos;
    /* the guard's place on guard_path, as patrol_path_move keeps it */
    Sint32 point;
    Float along;
    Angle path_pitch;
    Angle path_yaw;
    Angle roll_speed; /* what the step adds to ang.z */
    /* the guard's place and turn, which its draw takes */
    NJS_MATRIX *world_matrix;
    Uint8 unk_54[0x4c];
    Bool lens_on;
    /* the camera zone camset_add_marked gave the guard, or null where
     * camset's table was full */
    struct camera_zone *zone;
    /* whether the guard's record took part in the last round of collisions:
     * its last step registered it */
    Bool registered;
    Bool visible; /* quarters_guard_visible at its last step */
    /* the newest of the guard's lens flashes, and of its exhaust puffs */
    struct guard_sprite *lens;
    struct guard_sprite *exhaust;
};
#define GUARD(task) ((struct guard_state *)(task)->extra.p)

static void guard_create(int id);
static void guard_destroy(struct task *task);
static void guard_step(struct task *task);
static void guard_draw(struct task *task);
static struct guard_state *guard_alloc(struct task *task);
static void guard_free(struct task *task);
static void guard_start_mode(struct task *task, enum guard_mode mode);
static void guard_step_mode(struct task *task);
/* @hack type: guard_other_id external, where static would do. guard_yields
 * inlines its calls, and the image holds its body too */
int guard_other_id(struct task *task); /* NOLINT(misc-use-internal-linkage) */
static Bool guard_yields(struct task *task);
static void guard_wait_start(struct task *task);
static void guard_wait_step(struct task *task);
static void guard_patrol_start(struct task *task);
static void guard_patrol_step(struct task *task);
static void guard_lock_start(struct task *task);
static void guard_lock_step(struct task *task);
static void guard_attack_start(struct task *task);
static void guard_attack_step(struct task *task);
static Bool patrol_path_move(struct path *path, Sint32 *point, Float *along,
                             Float speed, NJS_POINT3 *pos, struct angles *ang);
static Bool player_ahead(struct task *task, struct path *path);
static void lens_add(struct task *task, NJS_POINT3 *pos);
static void lens_remove(struct guard_sprite *sprite);
static void lens_draw(struct guard_sprite *sprite);
static void exhaust_add(struct task *task, NJS_POINT3 *pos);
static void exhaust_remove(struct guard_sprite *sprite);
/* @hack type: guard_exhaust_step external, where static would do. guard_step
 * inlines its calls, and the image holds its body too */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void guard_exhaust_step(struct guard_sprite *sprite);
static void exhaust_draw(struct guard_sprite *sprite);
static void guard_body_step(struct task *task);
static void radar_step(struct task *task);
static Bool player_hidden(void);

static void (*mode_starts[GUARD_MODES])(struct task *) = {
    guard_wait_start,
    guard_patrol_start,
    guard_lock_start,
    guard_attack_start,
};
static void (*mode_steps[GUARD_MODES])(struct task *) = {
    guard_wait_step,
    guard_patrol_step,
    guard_lock_step,
    guard_attack_step,
};
/* lock on where the sensors touch player 0, or attack where the body does.
 * the guard does neither while it cools down or player 0 hides. nor does it
 * where its record took no part in the last round of collisions */
#define SENSE_PLAYER(task, sensor, body)                                       \
    if (!GUARD(task)->cooldown && GUARD(task)->registered &&                   \
        collision_next_player(task) && !player_hidden()) {                     \
        (sensor)->flags &= (Uint16)~COLLISION_TOUCHED;                         \
        guard_request_mode(task, GUARD_LOCK);                                  \
    } else if (!GUARD(task)->cooldown && GUARD(task)->registered &&            \
               collision_next_player((task)->child) && !player_hidden()) {     \
        (body)->flags &= (Uint16)~COLLISION_TOUCHED;                           \
        guard_request_mode(task, GUARD_ATTACK);                                \
    }
/* put the sensors back to the size they had before guard_lock_start scaled
 * them */
#define SENSORS_UNSCALE(task, sensor)                                          \
    (sensor)->shapes[0].radius /= guard_lock_scale;                            \
    (sensor)->shapes[0].size_y /= guard_lock_scale;                            \
    (sensor)->shapes[1].radius /= guard_lock_scale;                            \
    (sensor)->shapes[1].size_y /= guard_lock_scale;                            \
    collision_fit_radius(task)
/* turn the guard's lens toward AIM_HEIGHT above player 0's pos, and spring
 * its bank back toward level. it writes the caller's x, y and z, and its
 * pitch, yaw and bank */
/* @hack spelling: each difference a negation added, where a subtraction
 * would do. task->work read again for ang.z, where work would do */
#define AIM_AT_PLAYER(task, work)                                              \
    x = -player_work[0]->pos.x + (work)->pos.x;                                \
    y = -player_work[0]->pos.y + (work)->pos.y + lens_origin.y + -AIM_HEIGHT;  \
    z = -player_work[0]->pos.z + (work)->pos.z;                                \
    direction_angles_xy(x, y, z, &pitch, &yaw);                                \
    (work)->ang.x =                                                            \
        angle_interpolate((work)->ang.x, pitch, guard_aim_weight, 1.0F);       \
    (work)->ang.y =                                                            \
        angle_interpolate((work)->ang.y, yaw, guard_aim_weight, 1.0F);         \
    bank = -(task)->work->ang.z;                                               \
    GUARD(task)->roll_speed +=                                                 \
        (bank * BANK_SPRING) - (GUARD(task)->roll_speed * BANK_DAMPING);       \
    (task)->work->ang.z += GUARD(task)->roll_speed

static NJS_POINT3 exhaust_right = {
#include "stg26/guard/exhaust_right.inc"
};
static NJS_POINT3 exhaust_left = {
#include "stg26/guard/exhaust_left.inc"
};
static NJS_POINT3 lens_origin = {
#include "stg26/guard/lens_origin.inc"
};

/* the sensors: two cylinders that reach ahead of the lens, held by the
 * guard's own task. each turns a quarter turn about x, which lays its axis
 * along z */
static struct collision_shape sensors[SENSOR_SHAPES] = {
#include "stg26/guard/sensors.inc"
};
/* the body, held by the guard's child task */
static struct collision_shape body = {
#include "stg26/guard/body.inc"
};
/* the point of guard_path where guard 0 starts */
static Sint32 first_point = 120;
static Float lens_z = -2.0F;
static Sint32 lens_opacity = 192;
static Float lens_size = 5.0F;
static Float lens_depth_bias = -2.5F;
static Sint32 exhaust_fade_steps = 30;
static Sint32 exhaust_life = 24;
static Float exhaust_size = 5.5F;
static Float radar_near = 150.0F;
static Float radar_far = 300.0F;
/* the radar fog's speeds, slow and fast, as fog_setting.speed takes them.
 * below 0, each scrolls the fog's table the other way */
static Float radar_slow = -1.0F;
static Float radar_fast = -2.0F;
static Uint32 radar_green = 0x0040f060;
static Uint32 radar_red = 0x00ff5020;

struct fog_setting guard_radar_fog;
Bool guard_radar_on;
NJS_POINT3 guard_eye_positions[GUARD_EYES];
Sint32 guard_eye_count;
Sint32 guard_eye_free_count;
static Bool attack_in_progress;

/* @hack spelling: next_mode set through a call SHC expands, where an
 * assignment at each call would do */
static void guard_request_mode(struct task *task, enum guard_mode mode)
{
    GUARD(task)->next_mode = mode;
}

static void lens_step(struct guard_sprite *sprite)
{
    if (++sprite->age >= LENS_STEPS) {
        lens_remove(sprite);
    }
}

Uint8 *guard_build(Uint8 *buffer)
{
    draw_control_set(DRAW_CONTROL_SHADOW);
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buffer, guard_model_builds, 1) == 1) {
        guard_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    return buffer;
}

void guards_start(void)
{
    int id;

    for (id = 0; id < GUARDS; id++) {
        guard_create(id);
    }
    guard_eye_free_count = 0;
    attack_in_progress = FALSE;
}

static void guard_create(int id)
{
    struct task *task;
    struct task *child;
    struct zone_shape zone;
    Sint16 points;

#ifdef NONMATCHING
    /* this side makes no guard on a path of fewer than 3 points. a patrol
     * runs over every point but the last, and needs two */
    if (guard_path.count < 3) {
        return;
    }
#endif
    if ((task = task_create(TASK_WORK | TASK_FORCE, GUARD_LEVEL, guard_step,
                            "ObjectRobotExec")) == NULL) {
        return;
    }
    if ((child = task_create_child(TASK_WORK, guard_body_step, task)) == NULL) {
        task_destroy(task);
        return;
    }
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side: it destroys the
     * guard where guard_alloc fails */
    if (guard_alloc(task) == NULL) {
        task_destroy(task);
        return;
    }
#else
    guard_alloc(task);
    /* @bug memory: writes the id through null at 0x8c5054c8 where guard_alloc
     * fails. it returns null where either allocation fails, at 0x8c505b68 or
     * 0x8c505b7a */
#endif
    GUARD(task)->id = id;
    guards[GUARD(task)->id] = GUARD(task);
    task->draw = guard_draw;
    task->destructor = guard_destroy;
#ifdef NONMATCHING
    /* this side destroys the guard where collision_init fails. guard_destroy
     * reads zone and mode, so they are set first */
    GUARD(task)->zone = NULL;
    GUARD(task)->mode = GUARD_WAIT;
    if (!collision_init(task, sensors, SENSOR_SHAPES, COLLISION_OBJECT) ||
        !collision_init(child, &body, 1, COLLISION_OBJECT)) {
        task_destroy(task);
        return;
    }
#else
    collision_init(task, sensors, SENSOR_SHAPES, COLLISION_OBJECT);
    collision_init(child, &body, 1, COLLISION_OBJECT);
#endif
    zone.shape = ZONE_SPHERE;
    zone.pos = task->work->pos;
    zone.ang.x = 0;
    zone.ang.y = 0;
    zone.ang.z = 0;
    zone.scl.x = GUARD_ZONE_RADIUS;
    zone.scl.z = GUARD_ZONE_RADIUS * GUARD_ZONE_RADIUS;
    GUARD(task)->zone = camset_add_marked(&zone);
    guard_start_mode(task, GUARD_PATROL);
    /* @hack redundant: a jump to the statement after it */
    goto initial_point;
initial_point:
    points = guard_path.count;
    GUARD(task)->point =
        (int)(first_point + ((Float)points * id * GUARD_SPACING)) % points;
}

static void guard_destroy(struct task *task)
{
#ifdef NONMATCHING
    /* this side forgets the guard, and the attack it leaves unfinished */
    if (guards[GUARD(task)->id] == GUARD(task)) {
        guards[GUARD(task)->id] = NULL;
        if (attack_in_progress && GUARD(task)->mode == GUARD_ATTACK) {
            attack_in_progress = FALSE;
        }
    }
#endif
    if (GUARD(task)->zone) {
        camset_free_zone(GUARD(task)->zone);
    }
    while (GUARD(task)->lens) {
        lens_remove(GUARD(task)->lens);
    }
    while (GUARD(task)->exhaust) {
        exhaust_remove(GUARD(task)->exhaust);
    }
    guard_free(task);
}

static void guard_step(struct task *task)
{
    struct task_work *work = task->work;
    struct guard_sprite *sprite = GUARD(task)->lens;
#ifdef NONMATCHING
    /* this side's loops below store each sprite's next here, before the
     * sprite's step can free it */
    struct guard_sprite *next;
#endif
    NJS_POINT3 nozzle;
    NJS_POINT3 lens;
    NJS_POINT3 pos;

    /* @hack redundant: a label no jump targets */
visibility: /* NOLINT(clang-diagnostic-unused-label) */
    GUARD(task)->visible = quarters_guard_visible(task);
    guard_step_mode(task);
    GUARD(task)->lens_on = GUARD(task)->cooldown == 0;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njRotateZ(NULL, work->ang.z);
    matrix_get(GUARD(task)->world_matrix);
    if ((game_state.steps & 1) && GUARD(task)->visible) {
        if (GUARD(task)->lens_on) {
            lens = lens_origin;
            lens.z += lens_z;
            njCalcPoint(NULL, &lens, &pos);
            lens_add(task, &pos);
        }
        njCalcPoint(NULL, &exhaust_right, &nozzle);
        exhaust_add(task, &nozzle);
        njCalcPoint(NULL, &exhaust_left, &nozzle);
        exhaust_add(task, &nozzle);
    }
    njPopMatrixEx();
    if (GUARDS_STAND_DOWN) {
        guard_request_mode(task, GUARD_WAIT);
    } else {
        if (GUARD(task)->visible) {
            collision_register(task);
            GUARD(task)->registered = TRUE;
        } else {
            GUARD(task)->registered = FALSE;
        }
#ifdef NONMATCHING
        /* this side repairs the bug tagged in the matching side: it writes no
         * pos without a zone */
        if (GUARD(task)->zone) {
            GUARD(task)->zone->pos = work->pos;
        }
#else
        /* @bug memory: writes the zone's pos through null at 0x8c505834 where
         * the camset table was full. guard_destroy tests the zone for null at
         * 0x8c50560e */
        GUARD(task)->zone->pos = work->pos;
#endif
    }
#ifdef NONMATCHING
    /* this side repairs the two bugs tagged in the matching side: it takes each
     * sprite's next before its step can free it */
    for (; sprite; sprite = next) {
        next = sprite->next;
        lens_step(sprite);
    }
    for (sprite = GUARD(task)->exhaust; sprite; sprite = next) {
        next = sprite->next;
        guard_exhaust_step(sprite);
    }
#else
    /* @bug memory: reads an expired lens sprite's next at 0x8c505854, after
     * lens_step has freed it at 0x8c505850 */
    for (; sprite; sprite = sprite->next) {
        lens_step(sprite);
    }
    /* @bug memory: reads an expired exhaust sprite's next at 0x8c50587c,
     * after guard_exhaust_step has freed it at 0x8c505878 */
    for (sprite = GUARD(task)->exhaust; sprite; sprite = sprite->next) {
        guard_exhaust_step(sprite);
    }
#endif
    guard_positions[GUARD(task)->id] = work->pos;
}

static void guard_draw(struct task *task)
{
    struct guard_sprite *sprite = GUARD(task)->lens;
    struct task_work *work = task->work;
    Float *radius;
    Float saved_radius;

    if (!GUARD(task)->visible) {
        return;
    }
    njPushMatrixEx();
    matrix_multiply_before(GUARD(task)->world_matrix);
    njRotateY(NULL, HALF_TURN);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetTexture(&guard_texlist);
    if (guard_built) {
        njDirectDrawObject(guard_model_builds[0].built);
    } else {
        njCnkSimpleDrawObject(&quarters_guard_object);
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    if (GUARD(task)->lens_on || GUARD(task)->cooldown) {
        njSetTexture(&guard_sensor_texlist);
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE);
        njSetCnkBlendMode(ADDITIVE_BLEND);
        njSetConstantAttr(CHUNK_TYPE_BITS, STRIP_FLAGS);
        if (GUARD(task)->cooldown) {
            constant_material_set(GLOW_FULL, GLOW_DIM, GLOW_FULL, GLOW_DIM);
        } else {
            constant_material_set(GLOW_FULL, GLOW_FULL, GLOW_DIM, GLOW_DIM);
        }
        njCnkSimpleDrawModel(guard_sensor_objects[0].model);
        njSetCnkBlendMode(ALPHA_BLEND);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
    }
    njPopMatrixEx();
    njPushMatrixEx();
    matrix_multiply_before(GUARD(task)->world_matrix);
    njRotateZ(NULL, -work->ang.z);
    njRotateX(NULL, -work->ang.x);
    njRotateY(NULL, HALF_TURN);
    radius = &guard_shadow_objects->model->r;
    saved_radius = *radius;
    *radius *= SHADOW_SCALE_Y;
    njTranslate(NULL, 0.0F, guard_shadow_y, 0.0F);
    njScale(NULL, 1.0F, SHADOW_SCALE_Y, 1.0F);
    quarters_modifier_draw_tilted(guard_shadow_objects->model, work->ang.x,
                                  work->ang.z);
    *radius = saved_radius;
    njPopMatrixEx();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njFogDisable();
    njSetTexture(&guard_lens_texlist);
    for (; sprite; sprite = sprite->next) {
        lens_draw(sprite);
    }
    njSetTexture(&guard_exhaust_texlist);
    for (sprite = GUARD(task)->exhaust; sprite; sprite = sprite->next) {
        exhaust_draw(sprite);
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}

static struct guard_state *guard_alloc(struct task *task)
{
    struct task_work *work = task->work;
    NJS_MATRIX *matrix;

    if ((matrix = syMalloc(sizeof(NJS_MATRIX))) == NULL) {
        return NULL;
    }
    task->extra.p = syMalloc(sizeof(struct guard_state));
    if (task->extra.p == NULL) {
        syFree(matrix);
        return NULL;
    }
    GUARD(task)->cooldown = 0;
    /* @hack spelling: task->work read again for pos and ang, where work would
     * do */
    GUARD(task)->home = task->work->pos;
    GUARD(task)->home_ang = task->work->ang;
    GUARD(task)->previous_pos = task->work->pos;
    GUARD(task)->point = 0;
    GUARD(task)->along = 0.0F;
    GUARD(task)->roll_speed = 0;
    GUARD(task)->registered = FALSE;
    GUARD(task)->visible = quarters_guard_visible(task);
    GUARD(task)->lens = NULL;
    GUARD(task)->exhaust = NULL;
    GUARD(task)->world_matrix = matrix;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    matrix_get(GUARD(task)->world_matrix);
    njPopMatrixEx();
    return task->extra.p;
}

static void guard_free(struct task *task)
{
    if (task->extra.p) {
        if (GUARD(task)->world_matrix) {
            syFree(GUARD(task)->world_matrix);
        }
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
}

static void guard_start_mode(struct task *task, enum guard_mode mode)
{
    GUARD(task)->mode = mode;
    GUARD(task)->next_mode = NO_NEXT_MODE;
    GUARD(task)->phase = 0;
    GUARD(task)->steps = 0;
    mode_starts[GUARD(task)->mode](task);
}

static void guard_step_mode(struct task *task)
{
    if (GUARD(task)->next_mode != NO_NEXT_MODE) {
        /* @hack spelling: guard_start_mode's work repeated here, where a call
         * of it would do */
        GUARD(task)->mode = GUARD(task)->next_mode;
        GUARD(task)->next_mode = NO_NEXT_MODE;
        GUARD(task)->phase = 0;
        GUARD(task)->steps = 0;
        mode_starts[GUARD(task)->mode](task);
    }
    mode_steps[GUARD(task)->mode](task);
    GUARD(task)->steps++;
}

int guard_other_id(struct task *task)
{
    return (GUARD(task)->id + 1) % GUARDS;
}

/* whether the guard waits for the other. it waits while its point lies fewer
 * than guard_path.count / 2 - 1 points past the other's. the count runs up
 * the path's indexes, and round past the last */
static Bool guard_yields(struct task *task)
{
    int ahead;
    int count;

#ifdef NONMATCHING
    /* this side repairs the bug tagged below: with no other guard, it waits
     * only while the eyes are out */
    if (guards[guard_other_id(task)] == NULL) {
        return guard_eye_count != 0;
    }
#endif
    /* @bug memory: reads the other guard's point through null at 0x8c505e18
     * where guard_create gave that guard up. it gives up where a task_create
     * fails, at 0x8c50549a or 0x8c5054aa. radar_step tests each slot for null
     * at 0x8c506e9c */
    ahead = GUARD(task)->point - guards[guard_other_id(task)]->point;
    count = guard_path.count;
    ahead = (ahead + count) % count;
    return ahead < (count / 2) - 1;
}

static void guard_wait_start(struct task *task)
{
}

static void guard_wait_step(struct task *task)
{
    struct task_work *work = task->work;
    struct collision_work *sensor = work->collision;
    struct collision_work *child = task->child->work->collision;

    if (GUARDS_STAND_DOWN) {
        return;
    }
    if (guard_yields(task)) {
        return;
    }
    if (!game_state.cleared) {
        SENSE_PLAYER(task, sensor, child);
    }
    if (!game_state.winner_posed) {
        sound_play_at_for(ENGINE_SOUND, (char *)task + ENGINE_KEY,
                          ENGINE_PRIORITY, guard_engine_volume, SOUND_STEPS,
                          &work->pos);
    }
    if (!attack_in_progress && !game_state.unk_09) {
        guard_request_mode(task, GUARD_PATROL);
    }
}

static void guard_patrol_start(struct task *task)
{
    /* @hack redundant: the set editor's labels, which no code reads, in a
     * block no path reaches */
    if (0) {
        (void)"<- ROBOT SPEED";
        (void)"<- BIAS Y";
    }
}

static void guard_patrol_step(struct task *task)
{
    struct task_work *work;
    struct collision_work *sensor;
    Angle previous_yaw;
    struct collision_work *child;
    NJS_POINT3 pos;
    struct angles ang;
    Float speed;
    int bank;
    /* @hack spelling: guard_lock_speed_scale declared in the block, where a
     * declaration in guard_patrol.h would do */
    extern Float guard_lock_speed_scale;

    work = task->work;
    previous_yaw = work->ang.y;
    sensor = work->collision;
    child = task->child->work->collision;
    speed = guard_patrol_speed;
    if (GUARD(task)->mode == GUARD_LOCK) {
        patrol_path_move(&guard_path, &GUARD(task)->point, &GUARD(task)->along,
                         speed * guard_lock_speed_scale, &pos, &ang);
    } else {
        patrol_path_move(&guard_path, &GUARD(task)->point, &GUARD(task)->along,
                         speed, &pos, &ang);
    }
    GUARD(task)->previous_pos = work->pos;
    GUARD(task)->path_pitch = ang.x;
    GUARD(task)->path_yaw = ang.y;
    /* @hack redundant: a test of speed in a block no path reaches */
    if (0) {
        if (speed) {
        }
    }
    speed = 1.0F;
    work->ang.x = angle_interpolate(work->ang.x, GUARD(task)->path_pitch,
                                    guard_pitch_weight, speed);
    work->ang.y = angle_interpolate(work->ang.y, GUARD(task)->path_yaw,
                                    guard_yaw_weight, speed);
    bank = (int)((quarters_angle_turn(previous_yaw, work->ang.y) * TURN_BANK) -
                 work->ang.z);
    GUARD(task)->roll_speed +=
        (bank * BANK_SPRING) - (GUARD(task)->roll_speed * BANK_DAMPING);
    work->ang.z += GUARD(task)->roll_speed;
    work->pos.x = pos.x;
    /* @hack redundant: 0.0F added to pos.y */
    work->pos.y = pos.y + 0.0F;
    work->pos.z = pos.z;
    if (GUARD(task)->cooldown > 0) {
        GUARD(task)->cooldown--;
    }
    /* @hack type: game_state's cleared read as game_state_cleared here and
     * below, where game_state.cleared would do */
    if (attack_in_progress || game_state_cleared || game_state.unk_09 ||
        guard_yields(task)) {
        guard_request_mode(task, GUARD_WAIT);
        return;
    }
    if (!game_state_cleared) {
        SENSE_PLAYER(task, sensor, child);
    }
    if (!game_state.winner_posed) {
        sound_play_at_for(ENGINE_SOUND, (char *)task + ENGINE_KEY,
                          ENGINE_PRIORITY, guard_engine_volume, SOUND_STEPS,
                          &work->pos);
    }
}

static void guard_lock_start(struct task *task)
{
    struct task_work *work = task->work;
    struct collision_work *sensor = work->collision;

    GUARD(task)->previous_pos = work->pos;
    sensor->shapes[0].radius *= guard_lock_scale;
    sensor->shapes[0].size_y *= guard_lock_scale;
    sensor->shapes[1].radius *= guard_lock_scale;
    sensor->shapes[1].size_y *= guard_lock_scale;
    collision_fit_radius(task);
}

static void guard_lock_step(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: task->work read again for collision, where
     * work would do */
    struct collision_work *sensor = task->work->collision;
    NJS_POINT3 pos;
    struct angles ang;
    Angle pitch;
    Angle yaw;
    Float x;
    Float y;
    Float z;
    int bank;
    Float speed;

    if (njDistanceP2P(&player_work[0]->pos, &work->pos) > guard_stop_distance) {
        speed = player_ahead(task, &guard_path) ? -guard_patrol_speed
                                                : guard_patrol_speed;
        patrol_path_move(&guard_path, &GUARD(task)->point, &GUARD(task)->along,
                         speed, &pos, &ang);
        GUARD(task)->previous_pos = work->pos;
        work->pos.x = pos.x;
        /* @hack redundant: 0.0F added to pos.y */
        work->pos.y = pos.y + 0.0F;
        work->pos.z = pos.z;
    }
    AIM_AT_PLAYER(task, work);
    if (game_state.winner_posed) {
        guard_request_mode(task, GUARD_WAIT);
        return;
    }
    if ((GUARD(task)->registered && !collision_next_player(task)) ||
        player_hidden()) {
        sensor->flags &= ~COLLISION_TOUCHED;
        SENSORS_UNSCALE(task, sensor);
        guard_request_mode(task, GUARD_PATROL);
    }
    if (GUARD(task)->steps == guard_lock_steps) {
        SENSORS_UNSCALE(task, sensor);
        guard_request_mode(task, GUARD_ATTACK);
    }
    if (!game_state.winner_posed) {
        sound_play_task_at_for(LOCK_SOUND, task, LOCK_PRIORITY,
                               guard_lock_volume, SOUND_STEPS);
    }
}

static void guard_attack_start(struct task *task)
{
    int count;
    int slot;

    guard_eye_find_places(task);
    count = GUARD_EYES;
    if (count > guard_eye_free_count) {
        count = guard_eye_free_count;
    }
    attack_in_progress = TRUE;
    guard_eye_count = 0;
    sound_play(ATTACK_SOUND, (char *)task + ATTACK_KEY, ATTACK_PRIORITY, 0);
    sound_set_volume((char *)task + ATTACK_KEY, SOUND_ANY_ID,
                     guard_attack_volume);
    for (slot = 0; slot < count; slot++) {
        guard_eye_create(slot);
    }
}

static void guard_attack_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    struct angles ang;
    Angle pitch;
    Angle yaw;
    Float x;
    Float y;
    Float z;
    int bank;

    if (njDistanceP2P(&player_work[0]->pos, &work->pos) > guard_stop_distance) {
        patrol_path_move(&guard_path, &GUARD(task)->point, &GUARD(task)->along,
                         guard_patrol_speed, &pos, &ang);
        GUARD(task)->previous_pos = work->pos;
        work->pos.x = pos.x;
        /* @hack redundant: 0.0F added to pos.y */
        work->pos.y = pos.y + 0.0F;
        work->pos.z = pos.z;
    }
    AIM_AT_PLAYER(task, work);
    if (!guard_eye_count) {
        attack_in_progress = FALSE;
        guard_request_mode(task, GUARD_PATROL);
        GUARD(task)->cooldown = COOLDOWN_STEPS;
    }
}

#include "shared/patrol_path.h"

static Bool player_ahead(struct task *task, struct path *path)
{
    struct guard_state *state;
    NJS_POINT3 *points;
    int count;
    int here;
    int next;
    NJS_VECTOR to_player;
    NJS_VECTOR segment;
    Float dot;
    Float length_squared;
    Float guard_along;
    Float player_along;

    if (path->flags == PATH_LAYOUT_POS) {
        /* @hack redundant: (void)to_player, a statement that does nothing */
        (void)to_player;
        count = path->count - 1;
        points = path->points;
        here = GUARD(task)->point;
        state = GUARD(task);
        guard_along = state->along;
        here %= count;
        next = (here + 1) % count;
        segment.x = points[next].x - points[here].x;
        segment.z = points[next].z - points[here].z;
        to_player.x = player_work[0]->pos.x - points[here].x;
        to_player.z = player_work[0]->pos.z - points[here].z;
        dot = (to_player.x * segment.x) + (to_player.z * segment.z);
        if (dot > 0.0F) {
            length_squared = (segment.x * segment.x) + (segment.z * segment.z);
            player_along = dot;
            player_along /= length_squared;
            return player_along > guard_along;
        }
        return FALSE;
    }
    /* @bug uninitialized: returns R0 as the caller left it, for a path whose
     * flags aren't PATH_LAYOUT_POS. the branch at 0x8c506a46 goes straight to
     * the epilogue at 0x8c506af4. guard_path, the only path passed, is
     * PATH_LAYOUT_POS */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it returns FALSE */
    return FALSE;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static void lens_add(struct task *task, NJS_POINT3 *pos)
{
    struct guard_sprite *sprite;

    if ((sprite = syMalloc(sizeof(struct guard_sprite))) == NULL) {
        return;
    }
    sprite->prev = NULL;
    sprite->next = GUARD(task)->lens;
    if (sprite->next) {
        sprite->next->prev = sprite;
    }
    sprite->guard = task;
    GUARD(task)->lens = sprite;
    sprite->pos = *pos;
    sprite->scale = 1.0F;
    sprite->age = 0;
}

static void lens_remove(struct guard_sprite *sprite)
{
    if (!sprite) {
        return;
    }
    if (sprite->next) {
        sprite->next->prev = sprite->prev;
    }
    if (sprite->prev) {
        sprite->prev->next = sprite->next;
    } else if (sprite->guard) {
        GUARD(sprite->guard)->lens = sprite->next;
    }
    syFree(sprite);
}

static void lens_draw(struct guard_sprite *sprite)
{
    Angle angle;
    Uint8 alpha;
    Uint32 color;
    NJS_POINT3 pos;
    Float size;
    Float brightness;

    angle = (int)(sprite->age * ((Float)EIGHTH_TURN / LENS_STEPS));
    brightness = (njCos(angle) - COS_EIGHTH_TURN) * FADE_SCALE;
    alpha = lens_opacity * brightness * LENS_ALPHA_SHARE;
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    color = ((Uint32)alpha << ALPHA_SHIFT) | (alpha << RED_SHIFT) |
            (alpha << GREEN_SHIFT) | alpha;
#else
    /* @hack type: the alpha byte promoted to int before the high shift,
     * where casting it to Uint32 first would pack the same bits */
    color = (alpha << ALPHA_SHIFT) | (alpha << RED_SHIFT) |
            (alpha << GREEN_SHIFT) | alpha;
#endif
    size = njCos(angle) * lens_size;
    njCalcPoint(NULL, &sprite->pos, &pos);
    pos.z += lens_depth_bias;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njPtclSpriteStart(0, color, 1);
    njPtclDrawSprite(&pos, 1, size, size);
    njPtclSpriteEnd();
    njPopMatrixEx();
}

static void exhaust_add(struct task *task, NJS_POINT3 *pos)
{
    struct guard_sprite *sprite;

    if ((sprite = syMalloc(sizeof(struct guard_sprite))) == NULL) {
        return;
    }
    sprite->prev = NULL;
    sprite->next = GUARD(task)->exhaust;
    if (sprite->next) {
        sprite->next->prev = sprite;
    }
    sprite->guard = task;
    GUARD(task)->exhaust = sprite;
    sprite->pos = *pos;
    sprite->scale =
        (rand() * (EXHAUST_SCALE_SPREAD * RAND_SCALE)) + EXHAUST_SCALE_MIN;
    sprite->age = 0;
}

static void exhaust_remove(struct guard_sprite *sprite)
{
    if (!sprite) {
        return;
    }
    if (sprite->next) {
        sprite->next->prev = sprite->prev;
    }
    if (sprite->prev) {
        sprite->prev->next = sprite->next;
    } else if (sprite->guard) {
        GUARD(sprite->guard)->exhaust = sprite->next;
    }
    syFree(sprite);
}

void guard_exhaust_step(struct guard_sprite *sprite)
{
    if (++sprite->age >= exhaust_life) {
        exhaust_remove(sprite);
    }
}

static void exhaust_draw(struct guard_sprite *sprite)
{
    Float brightness;
    Uint32 color;

    brightness =
        (njCos((int)((Float)sprite->age / exhaust_fade_steps * EIGHTH_TURN)) -
         COS_EIGHTH_TURN) *
        FADE_SCALE;
    /* @hack type: the alpha converted through Uint8, where a conversion to
     * Uint32 would do */
    color = ((Uint32)(Uint8)(brightness * EXHAUST_ALPHA) << ALPHA_SHIFT) |
            EXHAUST_COLORS;
    brightness *= sprite->scale * exhaust_size;
    njPtclSpriteStart(0, color, 1);
    njPtclDrawSprite(&sprite->pos, 1, brightness, brightness);
    njPtclSpriteEnd();
}

static void guard_body_step(struct task *task)
{
    struct task_work *parent = task->parent->work;
    struct task_work *work = task->work;

    work->ang = parent->ang;
    work->pos = parent->pos;
    collision_register(task);
}

struct task *guard_radar_create(void)
{
    struct task *task;
    int i;

    task = task_create(TASK_WORK, RADAR_LEVEL, radar_step, "FogRaderExec");
    guard_radar_on = FALSE;
    for (i = 0; i < GUARDS; i++) {
        guards[i] = NULL;
    }
    return task;
}

/* the radar's fog is off while the winner poses. it runs red and fast while
 * a guard locks on or attacks. outside MISSION_HD, it otherwise runs green.
 * it runs fast within radar_near of the nearest guard in view, and slow within
 * radar_far. each guard in view that cools down raises the nearest distance
 * to radar_near. the distance it raises is the nearest of it and the guards
 * before it */
static void radar_step(struct task *task)
{
    int i;
    Bool locking;
    Bool attacking;
    Float distance;
    Float nearest;
    NJS_POINT3 pos;

    nearest = FAR_AWAY;
    attacking = FALSE;
    locking = FALSE;
    for (i = 0; i < GUARDS; i++) {
        if (guards[i] && guards[i]->mode == GUARD_LOCK) {
            locking = TRUE;
        }
        if (guards[i] && guards[i]->mode == GUARD_ATTACK) {
            attacking = TRUE;
        }
    }
    for (i = 0; i < GUARDS; i++) {
#ifdef NONMATCHING
        /* this side passes over a guard that never started */
        if (guards[i] == NULL) {
            continue;
        }
#endif
        distance = njDistanceP2P(&player_work[0]->pos, &guard_positions[i]);
        if (guards[i] && !guards[i]->visible) {
            continue;
        }
        if (nearest > distance) {
            nearest = distance;
        }
        if (guards[i] && guards[i]->cooldown) {
            if (radar_near > nearest) {
                nearest = radar_near;
            }
        }
    }
    /* @hack spelling: the lock's case apart from the attack's, where one test
     * of either would do */
    if (game_state.winner_posed) {
        guard_radar_fog.color = 0;
        guard_radar_on = FALSE;
    } else if (attacking) {
        guard_radar_fog.speed = radar_fast;
        guard_radar_fog.color = radar_red;
        guard_radar_on = TRUE;
    } else if (locking) {
        guard_radar_fog.speed = radar_fast;
        guard_radar_fog.color = radar_red;
        guard_radar_on = TRUE;
    } else if (game_state.mission != MISSION_HD && radar_near > nearest) {
        guard_radar_fog.speed = radar_fast;
        guard_radar_fog.color = radar_green;
        guard_radar_on = TRUE;
    } else if (game_state.mission != MISSION_HD && radar_far > nearest) {
        guard_radar_fog.speed = radar_slow;
        guard_radar_fog.color = radar_green;
        guard_radar_on = TRUE;
    } else {
        guard_radar_fog.color = 0;
        guard_radar_on = FALSE;
    }
#ifdef NONMATCHING
    /* this side tests the radar before it divides by the fog's speed */
    if (!guard_radar_on) {
        return;
    }
#endif
    if (game_state.steps % (int)(BEEP_SPEED / njAbs(guard_radar_fog.speed) *
                                 guard_radar_period) ==
            0 &&
        guard_radar_on) {
        if (njDistanceP2P(&guard_positions[0], &player_work[0]->pos) <
            njDistanceP2P(&guard_positions[1], &player_work[0]->pos)) {
            pos = guard_positions[0];
        } else {
            pos = guard_positions[1];
        }
#ifdef NONMATCHING
        /* this side sounds from a guard that started */
        if (guards[0] == NULL) {
            pos = guard_positions[1];
        } else if (guards[1] == NULL) {
            pos = guard_positions[0];
        }
#endif
        if (!game_state.winner_posed) {
            if (!player_hidden()) {
                sound_play_at_for(RADAR_SOUND, (char *)task + RADAR_KEY,
                                  RADAR_PRIORITY, guard_radar_volume,
                                  guard_radar_period, &pos);
            } else {
                sound_play_at_for(
                    RADAR_SOUND, (char *)task + RADAR_KEY, RADAR_PRIORITY,
                    (int)(guard_radar_volume * HIDDEN_VOLUME_SHARE),
                    guard_radar_period, &pos);
            }
        }
    }
}

static Bool player_hidden(void)
{
    return hiding_light_previous[0] || hiding_collision_previous[0];
}

Bool guard_player_in_shadow(void)
{
    return hiding_light_previous[0];
}

Bool quarters_guard_visible(struct task *guard)
{
    struct task_work *work = guard->work;
    enum quarters_area area;

    if (njDistanceP2P(&player_work[0]->pos, &work->pos) > guard_view_distance) {
        return FALSE;
    }
    area = quarters_area_at(&work->pos);
    /* @hack type: game_state's land_block_mask[0] read as
     * game_state_land_block_mask_0, where game_state.land_block_mask[0] would
     * do */
    /* @hack redundant: a break after each return that another case follows */
    switch (game_state_land_block_mask_0) {
    case QUARTERS_CENTER:
        return area == QUARTERS_SOUTH;
        break;
    case QUARTERS_SOUTH:
        return area == QUARTERS_SOUTH || area == QUARTERS_INNER_SOUTHWEST ||
               area == QUARTERS_INNER_SOUTHEAST;
        break;
    case QUARTERS_INNER_SOUTHEAST:
        return area == QUARTERS_INNER_SOUTHEAST || area == QUARTERS_SOUTH ||
               area == QUARTERS_EAST;
        break;
    case QUARTERS_EAST:
        return area == QUARTERS_EAST || area == QUARTERS_INNER_SOUTHEAST ||
               area == QUARTERS_INNER_NORTHEAST;
        break;
    case QUARTERS_INNER_NORTHEAST:
        return area == QUARTERS_INNER_NORTHEAST || area == QUARTERS_EAST ||
               area == QUARTERS_NORTHEAST;
        break;
    case QUARTERS_NORTHEAST:
        return area == QUARTERS_NORTHEAST || area == QUARTERS_INNER_NORTHEAST ||
               area == QUARTERS_NORTH;
        break;
    case QUARTERS_NORTH:
        return area == QUARTERS_NORTH || area == QUARTERS_NORTHEAST ||
               area == QUARTERS_NORTHWEST;
        break;
    case QUARTERS_NORTHWEST:
        return area == QUARTERS_NORTHWEST || area == QUARTERS_NORTH ||
               area == QUARTERS_INNER_NORTHWEST;
        break;
    case QUARTERS_INNER_NORTHWEST:
        return area == QUARTERS_INNER_NORTHWEST || area == QUARTERS_NORTHWEST ||
               area == QUARTERS_WEST;
        break;
    case QUARTERS_WEST:
        return area == QUARTERS_WEST || area == QUARTERS_INNER_NORTHWEST ||
               area == QUARTERS_INNER_SOUTHWEST;
        break;
    case QUARTERS_INNER_SOUTHWEST:
        return area == QUARTERS_INNER_SOUTHWEST || area == QUARTERS_WEST ||
               area == QUARTERS_SOUTH;
    }
    return FALSE;
}
