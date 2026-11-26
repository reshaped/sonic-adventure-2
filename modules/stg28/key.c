/* the key, KEY in Pyramid Cave's object list. a player carries it to the
 * keyhole of its color, where it locks. the keyhole then sinks, and the key
 * door of that color opens */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "object/fall.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "sound/sound.h"
#include "stg28/cave_util.h"
#include "stg28/key.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

#include "stg28/key/text.inc"

struct task *cave_keys[KEY_COLORS] = {NULL, NULL};

struct task *cave_keyholes[KEY_COLORS] = {NULL, NULL};

struct task *cave_key_doors[KEY_COLORS] = {NULL, NULL};

#include "stg28/models/key.h"

#define KEY_LOOSE_SHAPES 3
#define KEY_FLIGHT_SHAPES 1
#define KEY_ATTACK_SHAPES 1
#define FLOOR_BOUNCE 0.25F
#define FRICTION 0.5F
#define WALL_BOUNCE 0.5F

/* a key that lies loose starts its flight within KEYHOLE_REACH of its keyhole
 * across x and z. it must also lie within KEYHOLE_SLACK of
 * KEYHOLE_HEIGHT above the keyhole */
#define KEYHOLE_REACH 23.3556F
#define KEYHOLE_HEIGHT 10.0F
#define KEYHOLE_SLACK 2.0F
/* the flight ends KEYHOLE_HEIGHT above the keyhole, and the sink SUNK_HEIGHT
 * above it */
#define SUNK_HEIGHT 7.1281F
/* what each step of the flight and the sink adds to the key's speed down */
#define GRAVITY 0.08F
/* the flight and the sink end on the step their timer passes these, wherever
 * the key lies */
#define FLIGHT_STEPS 900
#define SINK_STEPS 180
/* player 0 sends the key back from within STAND_REACH of its stand, while the
 * stand shows it */
#define STAND_REACH 15.0F

/* the key's sounds, ids in pyramid_cave_sound_list. the hum plays while the key
 * lies loose or sinks, and lasts HUM_SOUND_STEPS + 1 updates. it plays for the
 * task, and the flight and the lock for SOUND_SECOND_KEY. sound_set_volume for
 * the task would set the hum's volume too */
#define HUM_SOUND 6
#define HUM_SOUND_STEPS 15
#define FLY_SOUND 8
#define LOCK_SOUND 9

/* the glow runs its cycle once each GLOW_STEPS of game_state.steps */
#define GLOW_STEPS 60
/* the fade's control bits. the blend mode lets FADE_BLEND take. the constant
 * attribute gives each strip STRIP_FLAGS. the constant material gives the alpha
 */
#define FADE_BLEND_CONTROL_SET                                                 \
    (DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE)
/* the fade's blend, for colors the draw multiplies by alpha itself */
#define FADE_BLEND (NJD_FBS_ONE | NJD_FBD_ISA)
/* the stand draws STAND_LIFT above the key's home */
#define STAND_LIFT 0.01F

static void key_destroy(struct task *task);
static void key_step(struct task *task);
static void key_draw(struct task *task);
static void key_child_step(struct task *task);
static struct key *key_alloc(struct task *task);
static void key_free(struct task *task);
static void key_take_mode(struct task *task, int mode);
static void key_run_mode(struct task *task);
static Bool key_on_keyhole(struct task *task);
static Bool key_shows_stand(struct task *task);
static void key_loose_start(struct task *task);
static void key_fly_start(struct task *task);
static void key_sink_start(struct task *task);
static void key_lock_start(struct task *task);
static void key_return_start(struct task *task);
static void key_loose_step(struct task *task);
static void key_fly_step(struct task *task);
static void key_sink_step(struct task *task);
static void key_lock_step(struct task *task);
static void key_return_step(struct task *task);

KEPT static NJS_CNK_MODEL *key_models[KEY_COLORS] = {
#include "stg28/key/key_models.inc"
};
KEPT static NJS_CNK_MODEL *key_unlit_models[KEY_COLORS] = {
#include "stg28/key/key_unlit_models.inc"
};
NJS_TEXLIST *cave_key_texlists[5] = {
#include "stg28/key/cave_key_texlists.inc"
};
KEPT static NJS_CNK_MODEL *stand_models[KEY_COLORS] = {
#include "stg28/key/stand_models.inc"
};
KEPT static NJS_TEXLIST *stand_texlists[KEY_COLORS + 1] = {
#include "stg28/key/stand_texlists.inc"
};
static char *color_names[KEY_COLORS] = {
#include "stg28/key/color_names.inc"
};
/* clang-format off */
static struct collision_shape key_loose_shapes[KEY_LOOSE_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_UNK_2000 | SHAPE_HURTABLE, {0.0F, 2.0F, 0.0F}, 15.0F, 4.0F, 0.0F, 0,
     {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ENEMIES), 0,
     SHAPE_UNK_2000, {0.0F, 2.0F, 0.0F}, 0.2F, 4.5F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 0.0F, 0.0F}, 3.8F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* the loose key's thin cylinder */
static struct collision_shape key_flight_shapes[KEY_FLIGHT_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ENEMIES), 0,
     SHAPE_UNK_2000, {0.0F, 2.0F, 0.0F}, 0.2F, 4.5F, 0.0F, 0, {0, 0, 0}},
};
/* the child's attack */
static struct collision_shape key_attack_shapes[KEY_ATTACK_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(SHAPE_TOP_LEVEL) | SHAPE_HURT | SHAPE_HURTS_ENEMIES |
         SHAPE_HURTS_OBJECTS,
     SHAPE_HURTABLE | SHAPE_IGNORE_PLAYERS | SHAPE_IGNORE_UNK_9,
     {0.0F, 4.5F, 0.0F}, 5.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* each step of the flight pulls the key's speed across x and z by key_pull
 * toward the keyhole. it then keeps key_drag of that speed */
static Float key_pull = 0.02F;
static Float key_drag = 0.9F;
/* a float no code reads */
static Float dat_8c5cfe74 = 0.02F;
/* the hum's volume, and how near the key player 0 must lie for a step to play
 * it */
static Sint32 key_hum_volume = -15;
static Float key_hum_range = 100.0F;
/* the volume of the key's sounds as its flight starts, and as it locks */
static Sint32 key_fly_volume = 125;
static Sint32 key_lock_volume = 125;
typedef void (*key_mode_handler)(struct task *task);

KEPT static key_mode_handler key_mode_starts[KEY_MODES] = {
#include "stg28/key/key_mode_starts.inc"
};
KEPT static key_mode_handler key_mode_steps[KEY_MODES] = {
#include "stg28/key/key_mode_steps.inc"
};
/* the glow's brightness swings by key_glow_swing above key_glow_base. it rises
 * over the first key_glow_rise steps of each cycle, and falls over the next
 * key_glow_fall */
static Float key_glow_swing = 0.6F;
static Float key_glow_base = 0.4F;
static Sint32 key_glow_rise = 15;
static Sint32 key_glow_fall = 45;
/* what the key's ang.y gains each step of the flight */
static Angle key_spin = 384;
static Float stand_view_range = 300.0F;
static Float stand_near_range = 150.0F;

static void key_set_mode(struct task *task, int mode)
{
    KEY(task)->next_mode = mode;
}

void cave_key_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *child;

    /* @hack spelling: the init's work nested in a test of the child, where a
     * return would do */
    if ((child = task_create_child(TASK_WORK, key_child_step, task)) != NULL) {
        child->work->pos = work->pos;
        collision_init(child, key_attack_shapes, KEY_ATTACK_SHAPES,
                       COLLISION_UNK_9);
        child->work->id = WORK_ID_ATTACK;
        if (key_alloc(task) == NULL) {
            task_kill(child);
            return;
        }
        /* @hack spelling: task->work read again, where work would do */
        cave_keys[KEY_COLOR(task->work)] = task;
        task->draw = key_draw;
        task->step = key_step;
        task->destructor = key_destroy;
    }
}

static void key_destroy(struct task *task)
{
    if (task->force.p != NULL) {
        body_destroy(task);
    }
    placement_set_state(task, KEY(task)->mode);
    key_free(task);
    cave_keys[KEY_COLOR(task->work)] = NULL;
}

static void key_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack spelling: task->work read again, where work would do */
    cave_keys[KEY_COLOR(task->work)] = task;
    key_run_mode(task);
    KEY(task)->last_flags = work->flags;
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- KEY(     )";
    }
}

static void key_draw(struct task *task)
{
    struct task_work *work = task->work;
    int step = game_state.steps % GLOW_STEPS;
    Float glow;

    if (step < key_glow_rise) {
        glow = (key_glow_swing * ((Float)step / (key_glow_rise - 1))) +
               key_glow_base;
    } else {
        glow = (key_glow_swing *
                ((Float)(key_glow_rise + key_glow_fall - step - 1) /
                 (key_glow_fall - 1))) +
               key_glow_base;
    }
    /* @hack spelling: task->work read again, where work would do */
    njSetTexture(cave_key_texlists[KEY_COLOR(task->work)]);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    if (KEY(task)->alpha == 1.0F) {
        if (KEY(task)->mode == KEY_MODE_LOOSE &&
            !(work->flags & WORK_HELD_BY_PLAYER)) {
            draw_control_save();
            draw_attr_save();
            draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
            draw_control_set(NJD_CONTROL_3D_CONSTANT_MATERIAL);
            constant_material_set(1.0F, glow, glow, glow);
            njCnkSimpleDrawModel(key_models[KEY_COLOR(task->work)]);
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
            draw_attr_restore();
            draw_control_restore();
        } else {
            njCnkSimpleDrawModel(key_models[KEY_COLOR(task->work)]);
        }
        njCnkSimpleDrawModel(key_unlit_models[KEY_COLOR(task->work)]);
    } else {
        Float alpha = KEY(task)->alpha;

        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(FADE_BLEND_CONTROL_SET);
        njSetCnkBlendMode(FADE_BLEND);
        njSetConstantAttr(CHUNK_TYPE_BITS, STRIP_FLAGS);
        constant_material_set(alpha, glow * alpha, glow * alpha, glow * alpha);
        njCnkSimpleDrawModel(key_models[KEY_COLOR(task->work)]);
        constant_material_set(alpha, alpha, alpha, alpha);
        njCnkSimpleDrawModel(key_unlit_models[KEY_COLOR(task->work)]);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
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
    njTranslateEx(&KEY(task)->home);
    /* @bug logic: tests the screen with the current matrix moved to the
     * key's home, by the JSR at 0x8c5038de before the BSR at 0x8c5038e2.
     * cave_on_screen takes each point through that matrix, so the test sees
     * the key and its home each moved by the home */
    if (key_shows_stand(task)) {
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(NJD_CONTROL_3D_CONSTANT_MATERIAL);
        constant_material_set(1.0F, glow, glow, glow);
        njCnkSimpleDrawModel(key_models[KEY_COLOR(task->work)]);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
        njCnkSimpleDrawModel(key_unlit_models[KEY_COLOR(task->work)]);
    }
    njTranslate(NULL, 0.0F, STAND_LIFT, 0.0F);
    njSetTexture(stand_texlists[KEY_COLOR(task->work)]);
    njCnkSimpleDrawModel(stand_models[KEY_COLOR(task->work)]);
    njPopMatrixEx();
}

static struct key *key_alloc(struct task *task)
{
    struct task_work *work = task->work;

    task->extra.p = syMalloc(sizeof(struct key));
    if (task->extra.p == NULL) {
        return NULL;
    }
    KEY(task)->spd.x = KEY(task)->spd.y = KEY(task)->spd.z = 0.0F;
    KEY(task)->home = work->pos;
    KEY(task)->alpha = 1.0F;
    key_take_mode(task, KEY_MODE_LOOSE);
    KEY(task)->last_flags = work->flags;
    return task->extra.p;
}

static void key_free(struct task *task)
{
    if (task->extra.p != NULL) {
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
}

static void key_take_mode(struct task *task, int mode)
{
    KEY(task)->mode = mode;
    KEY(task)->next_mode = KEY_NO_MODE;
    KEY(task)->timer = 0;
    key_mode_starts[KEY(task)->mode](task);
}

static void key_run_mode(struct task *task)
{
    if (KEY(task)->next_mode != KEY_NO_MODE) {
        KEY(task)->mode = KEY(task)->next_mode;
        KEY(task)->next_mode = KEY_NO_MODE;
        KEY(task)->timer = 0;
        key_mode_starts[KEY(task)->mode](task);
    }
    key_mode_steps[KEY(task)->mode](task);
    KEY(task)->timer++;
}

static void key_loose_start(struct task *task)
{
    if (task->force.p == NULL) {
        collision_init(task, key_loose_shapes, KEY_LOOSE_SHAPES,
                       COLLISION_UNK_9);
        body_init(task);
    }
    if (task->force.p != NULL) {
        BODY(task)->floor_bounce = FLOOR_BOUNCE;
        BODY(task)->friction = FRICTION;
        BODY(task)->wall_bounce = WALL_BOUNCE;
        BODY(task)->flags |= BODY_ANY_WALL | BODY_RESTS;
    }
}

static void key_loose_step(struct task *task)
{
    struct task_work *work = task->work;
    int player;

    if (work->flags & WORK_HELD_BY_PLAYER) {
        player = player_holding(task);
        if (player >= 0) {
            work->ang.x = player_work[player]->ang.x;
            work->ang.y = -player_work[player]->ang.y + QUARTER_TURN;
            work->ang.z = player_work[player]->ang.z;
        }
    } else {
        work->ang.x = work->ang.z = 0;
        if (key_on_keyhole(task)) {
            key_set_mode(task, KEY_MODE_FLY);
            collision_free(task);
            if (task->force.p != NULL) {
                body_destroy(task);
            }
            /* body_init gave these by fall_set_kind. no player is offered
             * the key once it flies */
            work->flags &= ~(FALL_OFFERABLE | FALL_UNK_200);
            return;
        }
        if (key_shows_stand(task) &&
            njDistanceP2P(&KEY(task)->home, &player_work[0]->pos) <
                STAND_REACH) {
            key_set_mode(task, KEY_MODE_RETURN);
            return;
        }
        if (njDistanceP2P(&player_work[0]->pos, &work->pos) < key_hum_range) {
            sound_play_task_at_for(HUM_SOUND, task, 0, key_hum_volume,
                                   HUM_SOUND_STEPS);
        }
    }
    /* @bug memory: body_step reads and writes the body through null where
     * body_init found no room. key_loose_start tests for that null at
     * 0x8c503b8a. the call at 0x8c503c8e takes it untested */
#ifdef NONMATCHING
    /* this side repairs the tag above: a key without a body skips body_step.
     * where no player holds it, it still registers its collision, as
     * body_step would, so a player can take it. nothing moves it while it
     * lies loose */
    if (task->force.p == NULL) {
        if (!(work->flags & WORK_HELD_BY_PLAYER)) {
            collision_register(task);
        }
        return;
    }
#endif
    body_step(task);
}

/* @hack redundant: task a register variable, where a plain parameter would
 * do */
static void key_fly_start(register struct task *task)
{
    collision_init(task, key_flight_shapes, KEY_FLIGHT_SHAPES,
                   COLLISION_OBJECT);
    sound_play(FLY_SOUND, SOUND_SECOND_KEY(task), 0, 0);
    sound_set_volume(SOUND_SECOND_KEY(task), SOUND_ANY_ID, key_fly_volume);
}

static void key_fly_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR to_keyhole;
    NJS_VECTOR pull;

    /* @hack spelling: task->work read again, where work would do */
    if (cave_keyholes[KEY_COLOR(task->work)] != NULL) {
        work->ang.y += key_spin;
        to_keyhole.x =
            cave_keyholes[KEY_COLOR(task->work)]->work->pos.x - work->pos.x;
        to_keyhole.y = 0.0F;
        to_keyhole.z =
            cave_keyholes[KEY_COLOR(task->work)]->work->pos.z - work->pos.z;
        pull = to_keyhole;
        njUnitVector(&pull);
        KEY(task)->spd.x += pull.x * key_pull;
        KEY(task)->spd.z += pull.z * key_pull;
        KEY(task)->spd.y += -GRAVITY;
        KEY(task)->spd.x *= key_drag;
        KEY(task)->spd.z *= key_drag;
        if ((work->pos.y == cave_keyholes[KEY_COLOR(task->work)]->work->pos.y +
                                KEYHOLE_HEIGHT &&
             cave_xz_length_sq(&KEY(task)->spd) >
                 cave_xz_length_sq(&to_keyhole)) ||
            KEY(task)->timer > FLIGHT_STEPS) {
            work->pos.x = cave_keyholes[KEY_COLOR(task->work)]->work->pos.x;
            work->pos.y = cave_keyholes[KEY_COLOR(task->work)]->work->pos.y +
                          KEYHOLE_HEIGHT;
            work->pos.z = cave_keyholes[KEY_COLOR(task->work)]->work->pos.z;
            key_set_mode(task, KEY_MODE_SINK);
        } else {
            work->pos.x += KEY(task)->spd.x;
            work->pos.z += KEY(task)->spd.z;
            work->pos.y += KEY(task)->spd.y;
            if (work->pos.y <=
                cave_keyholes[KEY_COLOR(task->work)]->work->pos.y +
                    KEYHOLE_HEIGHT) {
                work->pos.y =
                    cave_keyholes[KEY_COLOR(task->work)]->work->pos.y +
                    KEYHOLE_HEIGHT;
            }
        }
    }
    collision_register(task);
}

/* the sink reads the keyhole of the key's color untested. the flight found
 * it, and keyhole_step never lets a keyhole leave */
static void key_sink_start(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack spelling: task->work read again, where work would do */
    work->ang.y = cave_keyholes[KEY_COLOR(task->work)]->work->ang.y;
    KEY(task)->spd.x = KEY(task)->spd.y = KEY(task)->spd.z = 0.0F;
}

static void key_sink_step(struct task *task)
{
    struct task_work *work = task->work;

    KEY(task)->spd.y -= GRAVITY;
    work->pos.y += KEY(task)->spd.y;
    /* @hack spelling: task->work read again, where work would do */
    if (work->pos.y <=
            cave_keyholes[KEY_COLOR(task->work)]->work->pos.y + SUNK_HEIGHT ||
        KEY(task)->timer > SINK_STEPS) {
        work->pos.y =
            cave_keyholes[KEY_COLOR(task->work)]->work->pos.y + SUNK_HEIGHT;
        key_set_mode(task, KEY_MODE_LOCKED);
    }
    if (njDistanceP2P(&player_work[0]->pos, &work->pos) < key_hum_range) {
        sound_play_task_at_for(HUM_SOUND, task, 0, key_hum_volume,
                               HUM_SOUND_STEPS);
    }
    collision_register(task);
}

static void key_lock_start(struct task *task)
{
    sound_play(LOCK_SOUND, SOUND_SECOND_KEY(task), 0, 0);
    sound_set_volume(SOUND_SECOND_KEY(task), SOUND_ANY_ID, key_lock_volume);
}

static void key_lock_step(struct task *task)
{
    collision_register(task);
}

static void key_return_start(struct task *task)
{
    struct task_work *work = task->work;

    work->pos = KEY(task)->home;
}

static void key_return_step(struct task *task)
{
    key_set_mode(task, KEY_MODE_LOOSE);
}

static Bool key_on_keyhole(struct task *task)
{
    if (cave_keyholes[KEY_COLOR(task->work)] == NULL) {
        return FALSE;
    }
    return KEY(task)->mode == KEY_MODE_LOOSE &&
           cave_xz_distance(&task->work->pos,
                            &cave_keyholes[KEY_COLOR(task->work)]->work->pos) <
               KEYHOLE_REACH &&
           njAbs(task->work->pos.y -
                 cave_keyholes[KEY_COLOR(task->work)]->work->pos.y -
                 KEYHOLE_HEIGHT) < KEYHOLE_SLACK;
}

static void key_child_step(struct task *task)
{
    task->work->pos = task->parent->work->pos;
    collision_register(task);
}

/* whether the stand shows a copy of the key. it shows one while the key is in
 * KEY_MODE_LOOSE, stand_near_range or more from its home. it shows none where
 * the key and its home lie on screen, each within stand_view_range of the
 * camera. the screen test holds only under the camera's view: cave_on_screen
 * takes each point through the current matrix */
static Bool key_shows_stand(struct task *task)
{
    struct task_work *work = task->work;
    Float distance;
    Float camera_to_key;
    Float camera_to_stand;

    if (KEY(task)->mode == KEY_MODE_LOOSE) {
        distance = njDistanceP2P(&KEY(task)->home, &work->pos);
        camera_to_key =
            njDistanceP2P(&camera_views[camera_current_index]->pos, &work->pos);
        camera_to_stand = njDistanceP2P(
            &camera_views[camera_current_index]->pos, &KEY(task)->home);
        if (distance < stand_near_range) {
            return FALSE;
        }
        if (cave_on_screen(&work->pos) && cave_on_screen(&KEY(task)->home) &&
            camera_to_key < stand_view_range &&
            camera_to_stand < stand_view_range) {
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}
