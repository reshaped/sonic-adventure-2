/* the missile, ROCKETMISSSILE in a stage's object list and MISSSILE in a
 * boss's, as the images spell them. a base raises the missile. a player on
 * the button beside the base sends it to its destination. a table holds the
 * targets that wait for a missile of their id */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "draw/lines.h"
#include "effect/burst.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/missile.h"
#include "shared/object_builds.h"
#include "shared/rocket_thrust.h"
#include "shared/switch.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "shared/stage_texture_groups.h"
#include "vmu/vibration.h"

#include "shared/missile/text.inc"

/* a missile's modes, in its work's mode */
enum missile_mode {
    MODE_WAIT = 2, /* in its base until a launch */
    MODE_RISE, /* rising, then risen until a player is on the button */
    MODE_AIM, /* turning toward its destination, then glowing */
    MODE_SPENT, /* in its base, never to launch again */
    MODE_FLY,
    MODE_LANDED /* hidden at its destination, until returned to its base */
};

/* the button lies at (BUTTON_X, BUTTON_Y, 0) in the base's frame. player 0
 * or 1 is on the button within BUTTON_REACH of that point. its work's flags
 * must also hold a bit of BUTTON_PLAYER_FLAGS. the button draws at a height
 * of BUTTON_PRESSED_Y while a player is on it */
#define BUTTON_X (-15.1F)
#define BUTTON_Y 2.6F
#define BUTTON_PRESSED_Y 1.6F
#define BUTTON_REACH 6.5F
#define BUTTON_PLAYER_FLAGS (PLAYER_WORK_ON_GROUND | PLAYER_WORK_ON_SHAPE)

/* the work's submode */
enum missile_press {
    UNPRESSED, /* set as the missile returns to its base */
    PRESSED /* set as a player arrives on the button */
};

/* MISSILE_LAUNCH_BITS of ang.x, modulo MISSILE_LAUNCHES, pick a
 * missile_launch. missile.h gives each launch's condition */
enum missile_launch {
    MISSILE_EVERY,
    MISSILE_ONCE,
    MISSILE_SWITCH,
    MISSILE_ENEMY,
    MISSILE_LAUNCHES
};
#define ENEMY_LAUNCH_RANGE 100.0F
/* in ang.z: a COLLISION_ENEMY record of missile_enemy_shape, and a launch
 * that needs no target. missile_init clears the bit */
#define MISSILE_ENEMY_COLLISION 0x80

/* a risen missile lies RISE above the base's pos, or below a base upside
 * down */
#define RISE 9.0F
#define RISE_STEP 0.05F
#define HATCH_STEP 0.05F
#define MISSILE_LAST_FRAME 7.0F
#define MISSILE_FRAME_STEP 0.3F
/* the most the aim turns yaw or pitch in a step, in Angle units. pitch
 * turns only on a step that leaves yaw as it is */
#define AIM_TURN 320
/* an aiming missile bobs by up to BOB_HEIGHT about its risen place. the
 * bob's phase turns BOB_STEP a step, in Angle units */
#define BOB_HEIGHT 0.3F
#define BOB_STEP 0x600
#define THRUST_CHANCE 0.6F /* the chance of a puff at each step of the aim */
/* an aiming missile rolls by up to ROLL_DEGREES either way. the roll's
 * phase turns ROLL_STEP a step, in Angle units */
#define ROLL_DEGREES 10.0F
#define ROLL_STEP 0x400
#define GLOW_STEP 0.07F
/* the glow model draws at GLOW_FLICKER of the missile's glow while
 * game_state.steps is odd */
#define GLOW_FLICKER 0.6F
/* how far the missile flies in a step. a missile nearer than that to its
 * destination lands */
#define FLIGHT_STEP 5.1F
/* a flying missile gives off a puff only while farther than THRUST_DISTANCE
 * from its destination. it gives off none at a step whose game_state.steps
 * has FLIGHT_PUFF_SKIP set */
#define THRUST_DISTANCE 10.2F
#define FLIGHT_PUFF_SKIP 2
/* a flying missile's puff moves at FLIGHT_PUFF_SPEED of the missile's move
 * in that step. each part of its speed then changes at random by up to half
 * of FLIGHT_JITTER either way */
#define FLIGHT_PUFF_SPEED 0.5F
#define FLIGHT_JITTER 0.3F
/* a flying missile's puff takes a random size from FLIGHT_SIZE_MIN up to
 * FLIGHT_SIZE_MIN + FLIGHT_SIZE_RANGE */
#define FLIGHT_SIZE_MIN 2.0F
#define FLIGHT_SIZE_RANGE 1.5F
/* the thrust leaves the missile at TAIL_X on its x axis, which its yaw and
 * pitch turn */
#define TAIL_X (-0.2F)
/* the size scale of the burst at a landing, and the share of tail its speed
 * takes */
#define BURST_SCALE 15.0F
#define BURST_SPEED_SCALE 0.2F

/* the missile's shape has no attack for a flight's first FLIGHT_SAFE_STEPS
 * + 1 steps. it then has FLIGHT_ATTACK, in the SHAPE_ATTACK bits of its
 * levels, until the flight ends */
#define FLIGHT_ATTACK SHAPE_ATTACK_LEVEL(2)
#define FLIGHT_SAFE_STEPS 15
/* the steps a missile has flown, in its task's unk_3c. the count stops at
 * FLIGHT_SAFE_STEPS + 1, and is 0 outside a flight */
#define STEPS_FLOWN(task) ((task)->unk_3c.l)

#define SOUND_LAUNCH 0x1005
#define SOUND_THRUST 0x1019
/* the priority of SOUND_THRUST, and the steps it plays for after each call */
#define THRUST_PRIORITY 1
#define THRUST_SOUND_STEPS 30
#define VIBRATION_POWER 5
#define VIBRATION_FREQ 50

/* the size that picks the base's WORLD_SIZE_CLASS is the base model's radius
 * plus BASE_MARGIN */
#define BASE_MARGIN 5.0F
/* the missile's step clears WORLD_ADDED while no player is within the base
 * model's radius plus BASE_NEAR. world_collect_near_players then passes over
 * the base */
#define BASE_NEAR 32.0F

struct missile {
    /* the base's object in the collision world: a copy of
     * missile_base_collision */
    NJS_OBJECT base;
    Float frame; /* the missile's motion frame, 0 to MISSILE_LAST_FRAME */
    Float hatch; /* the base's motion frame: 0 shut, 1 open */
    Angle roll;
    Angle yaw;
    Angle pitch;
    /* the missile's position. a landed missile points its targets here */
    NJS_POINT3 pos;
    Float glow; /* the glow model's brightness, 0 to 1 */
    /* 0 while a player is on the button, and 1 otherwise. the button draws
     * this far above BUTTON_PRESSED_Y */
    Float button_rise;
    /* the slot of the player whose pad shakes as the missile takes off. the
     * slot is 0 while player 0 is on the button, and 1 otherwise */
    Sint32 presser;
};

#define MISSILE(task) ((struct missile *)(task)->motion.p)

/* The three fins and the two sliding hatch doors use position keys. */

static NJS_TEXNAME missile_textures[3] = {
#include "shared/missile/missile_textures.inc"
};

NJS_TEXLIST missile_texlist = {
#include "shared/missile/missile_texlist.inc"
};

static Sint16 missile_fin_negative_z_polygons[91] = {
#include "shared/missile/missile_fin_negative_z_polygons.inc"
};

static Sint32 missile_fin_negative_z_vertices[57] = {
#include "shared/missile/missile_fin_negative_z_vertices.inc"
};

static NJS_CNK_MODEL missile_fin_negative_z_model = {
#include "shared/missile/missile_fin_negative_z_model.inc"
};

static NJS_CNK_OBJECT missile_fin_negative_z = {
#include "shared/missile/missile_fin_negative_z.inc"
};

static Sint16 missile_fin_positive_z_polygons[91] = {
#include "shared/missile/missile_fin_positive_z_polygons.inc"
};

static Sint32 missile_fin_positive_z_vertices[57] = {
#include "shared/missile/missile_fin_positive_z_vertices.inc"
};

static NJS_CNK_MODEL missile_fin_positive_z_model = {
#include "shared/missile/missile_fin_positive_z_model.inc"
};

static NJS_CNK_OBJECT missile_fin_positive_z = {
#include "shared/missile/missile_fin_positive_z.inc"
};

static Sint16 missile_fin_lower_polygons[91] = {
#include "shared/missile/missile_fin_lower_polygons.inc"
};

static Sint32 missile_fin_lower_vertices[57] = {
#include "shared/missile/missile_fin_lower_vertices.inc"
};

static NJS_CNK_MODEL missile_fin_lower_model = {
#include "shared/missile/missile_fin_lower_model.inc"
};

static NJS_CNK_OBJECT missile_fin_lower = {
#include "shared/missile/missile_fin_lower.inc"
};

static Sint16 missile_polygons[759] = {
#include "shared/missile/missile_polygons.inc"
};

static Sint32 missile_vertices[459] = {
#include "shared/missile/missile_vertices.inc"
};

static NJS_CNK_MODEL missile_model = {
#include "shared/missile/missile_model.inc"
};

NJS_CNK_OBJECT missile_object = {
#include "shared/missile/missile_object.inc"
};

static NJS_MKEY_F missile_fin_lower_positions[8] = {
#include "shared/missile/missile_fin_lower_positions.inc"
};

static NJS_MKEY_F missile_fin_positive_z_positions[8] = {
#include "shared/missile/missile_fin_positive_z_positions.inc"
};

static NJS_MKEY_F missile_fin_negative_z_positions[8] = {
#include "shared/missile/missile_fin_negative_z_positions.inc"
};

static NJS_MDATA2 missile_motion_channels[4] = {
#include "shared/missile/missile_motion_channels.inc"
};

NJS_MOTION missile_motion = {
#include "shared/missile/missile_motion.inc"
};

static NJS_TEXNAME missile_glow_textures[1] = {
#include "shared/missile/missile_glow_textures.inc"
};

NJS_TEXLIST missile_glow_texlist = {
#include "shared/missile/missile_glow_texlist.inc"
};

static Sint16 missile_glow_polygons[93] = {
#include "shared/missile/missile_glow_polygons.inc"
};

static Sint32 missile_glow_vertices[57] = {
#include "shared/missile/missile_glow_vertices.inc"
};

NJS_CNK_MODEL missile_glow_model = {
#include "shared/missile/missile_glow_model.inc"
};

static NJS_CNK_OBJECT missile_glow_object = {
#include "shared/missile/missile_glow_object.inc"
};

static NJS_TEXNAME missile_button_textures[2] = {
#include "shared/missile/missile_button_textures.inc"
};

NJS_TEXLIST missile_button_texlist = {
#include "shared/missile/missile_button_texlist.inc"
};

static Sint16 missile_button_polygons[169] = {
#include "shared/missile/missile_button_polygons.inc"
};

static Sint32 missile_button_vertices[105] = {
#include "shared/missile/missile_button_vertices.inc"
};

static NJS_CNK_MODEL missile_button_model = {
#include "shared/missile/missile_button_model.inc"
};

NJS_CNK_OBJECT missile_button_object[1] = {
#include "shared/missile/missile_button_object.inc"
};

static NJS_MKEY_F missile_hatch_negative_z_positions[2] = {
#include "shared/missile/missile_hatch_negative_z_positions.inc"
};

static NJS_MKEY_F missile_hatch_positive_z_positions[2] = {
#include "shared/missile/missile_hatch_positive_z_positions.inc"
};

static NJS_MDATA2 missile_base_motion_channels[4] = {
#include "shared/missile/missile_base_motion_channels.inc"
};

NJS_MOTION missile_base_motion = {
#include "shared/missile/missile_base_motion.inc"
};

static NJS_TEXNAME missile_base_textures[2] = {
#include "shared/missile/missile_base_textures.inc"
};

NJS_TEXLIST missile_base_texlist = {
#include "shared/missile/missile_base_texlist.inc"
};

static Sint16 missile_hatch_positive_z_polygons[41] = {
#include "shared/missile/missile_hatch_positive_z_polygons.inc"
};

static Sint32 missile_hatch_positive_z_vertices[39] = {
#include "shared/missile/missile_hatch_positive_z_vertices.inc"
};

static NJS_CNK_MODEL missile_hatch_positive_z_model = {
#include "shared/missile/missile_hatch_positive_z_model.inc"
};

static NJS_CNK_OBJECT missile_hatch_positive_z = {
#include "shared/missile/missile_hatch_positive_z.inc"
};

static Sint16 missile_hatch_negative_z_polygons[41] = {
#include "shared/missile/missile_hatch_negative_z_polygons.inc"
};

static Sint32 missile_hatch_negative_z_vertices[39] = {
#include "shared/missile/missile_hatch_negative_z_vertices.inc"
};

static NJS_CNK_MODEL missile_hatch_negative_z_model = {
#include "shared/missile/missile_hatch_negative_z_model.inc"
};

static NJS_CNK_OBJECT missile_hatch_negative_z = {
#include "shared/missile/missile_hatch_negative_z.inc"
};

static Sint16 missile_base_polygons[551] = {
#include "shared/missile/missile_base_polygons.inc"
};

static Sint32 missile_base_vertices[363] = {
#include "shared/missile/missile_base_vertices.inc"
};

static NJS_CNK_MODEL missile_base_model = {
#include "shared/missile/missile_base_model.inc"
};

NJS_CNK_OBJECT missile_base_object = {
#include "shared/missile/missile_base_object.inc"
};

/* SHC accepts the two packed material colors as scalars. */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL missile_base_material = {
#include "shared/missile/missile_base_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 missile_base_indices[64] = {
#include "shared/missile/missile_base_indices.inc"
};

static NJS_MESHSET missile_base_meshset = {
#include "shared/missile/missile_base_meshset.inc"
};

static NJS_POINT3 missile_base_points[18] = {
#include "shared/missile/missile_base_points.inc"
};

static NJS_VECTOR missile_base_normals[18] = {
#include "shared/missile/missile_base_normals.inc"
};

static NJS_MODEL missile_base_bounds = {
#include "shared/missile/missile_base_bounds.inc"
};

NJS_OBJECT missile_base_collision = {
#include "shared/missile/missile_base_collision.inc"
};

NJS_TEXLIST *missile_texlists[7] = {
#include "shared/missile/missile_texlists.inc"
};

struct missile_target missile_targets[MISSILE_TARGETS] = {
#include "shared/missile/missile_targets.inc"
};
/* the puffs of thrust an aiming missile gives off. thrust_spd is a puff's
 * speed before the missile's yaw and pitch turn it */
static NJS_VECTOR thrust_spd = {-2.7F, 0.0F, 0.0F};
/* each part of a puff's speed then changes at random by up to half of
 * thrust_jitter either way */
static Float thrust_jitter = 0.8F;
/* a puff's floor lies thrust_rise plus the puff's size above the base's pos */
static Float thrust_rise = 1.5F;
/* a puff takes a random size from thrust_size_min up to thrust_size_min +
 * thrust_size_range */
static Float thrust_size_min = 2.0F;
static Float thrust_size_range = 1.0F;
static char *missile_launch_labels[2 * MISSILE_LAUNCHES] = {
#include "shared/missile/missile_launch_labels.inc"
};
/* the missile's collision shape, as a COLLISION_OBJECT record and as a
 * COLLISION_ENEMY one. the two differ only in their ids */
/* clang-format off */
static struct collision_shape missile_shape = {
    0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_HURTS_ALL | SHAPE_HURT | SHAPE_GUARD_LEVEL(1) | SHAPE_ATTACK_LEVEL(2),
    SHAPE_TURN_YZX | SHAPE_HURTABLE, {4.7F, 0.4F, 0.0F}, 2.4F, 6.0F, 5.0F, 0,
    {0, 0, QUARTER_TURN},
};
static struct collision_shape missile_enemy_shape = {
    0x11, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_HURTS_ALL | SHAPE_HURT | SHAPE_GUARD_LEVEL(1) | SHAPE_ATTACK_LEVEL(2),
    SHAPE_TURN_YZX | SHAPE_HURTABLE, {4.7F, 0.4F, 0.0F}, 2.4F, 6.0F, 5.0F, 0,
    {0, 0, QUARTER_TURN},
};
/* clang-format on */

static void missile_destroy(struct task *task);
static void missile_step(struct task *task);
static void missile_draw(struct task *task);
static void missile_collision_init(struct task *task);
static void missile_collision_step(struct task *task);
static void missile_collision_destroy(struct task *task);

struct missile_target *missile_target_add(Sint32 id, NJS_POINT3 **hit)
{
    struct missile_target *target;
    int i;

    for (i = 0, target = missile_targets; i < MISSILE_TARGETS; i++, target++) {
        if (target->hit == NULL) {
            target->hit = hit;
            target->id = id;
            *hit = NULL;
            return target;
        }
    }
    return NULL;
}

struct missile_target *missile_target_remove(NJS_POINT3 **hit)
{
    struct missile_target *target;
    int i;

    for (i = 0, target = missile_targets; i < MISSILE_TARGETS; i++, target++) {
        if (target->hit == hit) {
            *hit = NULL;
            target->hit = NULL;
            target->id = -1;
            return target;
        }
    }
    return NULL;
}

/* the first taken entry whose id's low 16 bits are id's, or null where none
 * is. no code calls it, and no header declares it. it is external so that
 * SHC keeps it, as the image does */
/* NOLINTBEGIN(misc-use-internal-linkage) */
struct missile_target *missile_target_find(Sint32 id)
{
    struct missile_target *target;
    int i;

    /* @hack spelling: id narrowed in place before the loop, where the test
     * narrowing it would do */
    id = (Uint16)id;
    for (i = 0, target = missile_targets; i < MISSILE_TARGETS; i++, target++) {
        if (TARGET_MISSILE_ID(target) == id && target->hit != NULL) {
            return target;
        }
    }
    return NULL;
}
/* NOLINTEND(misc-use-internal-linkage) */

void missile_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *base;
    Uint32 flags;
    Float radius;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct missile));
    if (task->motion.p == NULL) {
        return;
    }
    task->draw = missile_draw;
    task->step = missile_step;
    task->destructor = missile_destroy;
    work->mode = MODE_WAIT;
    MISSILE(task)->pos = work->pos;
    MISSILE(task)->pitch = 0;
    MISSILE(task)->roll = 0;
    MISSILE(task)->yaw = work->ang.y;
    MISSILE(task)->frame = 0.0F;
    MISSILE(task)->hatch = 0.0F;
    MISSILE(task)->glow = 0.0F;
    MISSILE(task)->button_rise = 0.0F;
    base = &MISSILE(task)->base;
    *base = missile_base_collision;
    base->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    base->ang[1] = work->ang.y;
    /* @hack type: the half turn and 0 as unsigned, where signed would do */
    base->ang[2] = (work->ang.x & MISSILE_UPSIDE_DOWN) ? (Uint32)HALF_TURN : 0U;
    base->ang[0] = 0;
    base->pos[0] = work->pos.x;
    base->pos[1] = work->pos.y;
    base->pos[2] = work->pos.z;
    radius = base->model->r + BASE_MARGIN;
    flags = WORLD_SOLID;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, base);
    work->ang.x &= MISSILE_LAUNCH_BITS | MISSILE_UPSIDE_DOWN;
    /* @bug logic: the tests of MISSILE_ENEMY_COLLISION never hold, since the
     * mask keeps only the id of ang.z (AND at 0x1c4 of the object's code, TST
     * at 0x536 and 0x11ec) */
    work->ang.z &= MISSILE_ID;
    work->ang.z += (work->ang.x & MISSILE_UPSIDE_DOWN) ? HALF_TURN : 0;
    task_create_child(TASK_WORK, missile_collision_init, task);
    if (task->placement != NULL && placement_get_state(task)) {
        work->mode = MODE_SPENT;
    }
}

static void missile_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (work->mode == MODE_SPENT && task->placement != NULL) {
        placement_set_state(task, MISSILE_SPENT_STATE);
    }
    if (MISSILE(task)->base.model != NULL) {
        world_remove(task, &MISSILE(task)->base);
    }
    task_destroy_children(task);
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void missile_step(struct task *task)
{
    struct task_work *work = task->work;
    struct missile *missile;
    struct missile_target *target;
    /* @hack spelling: vec reused for the button's place, where a local of
     * its own would do */
    NJS_VECTOR vec;
    Float distance;
    int i;
    /* @hack spelling: the players' works and the first of them through
     * locals, where player_work[0] read in place would do */
    struct task_work **table;
    struct task_work **slot;

    if (work->mode < MODE_FLY && placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: an else no path reaches, which keeps only the id of
     * ang.z again */
    if (1) {
    } else {
        work->ang.z &= MISSILE_ID;
    }
    vec.x = (work->ang.x & MISSILE_UPSIDE_DOWN) ? -BUTTON_X : BUTTON_X;
    vec.y = (work->ang.x & MISSILE_UPSIDE_DOWN) ? -BUTTON_Y : BUTTON_Y;
    vec.z = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, &vec, &vec);
    njPopMatrix(1);
    /* @hack spelling: the store and the assignment joined by a comma, where
     * two statements would do */
    MISSILE(task)->presser = 0, table = &player_work[0];
    if ((*(slot = table) != NULL &&
         njDistanceP2P(&(*slot)->pos, &vec) < BUTTON_REACH &&
         ((*slot)->flags & BUTTON_PLAYER_FLAGS)) ||
        (MISSILE(task)->presser = 1,
         player_work[1] != NULL &&
             njDistanceP2P(&player_work[1]->pos, &vec) < BUTTON_REACH &&
             (player_work[1]->flags & BUTTON_PLAYER_FLAGS))) {
        if (work->submode == UNPRESSED && MISSILE(task)->button_rise > 0.0F) {
            work->submode = PRESSED;
        }
        MISSILE(task)->button_rise = 0.0F;
    } else {
        MISSILE(task)->button_rise = 1.0F;
    }
    if (!game_state.time_stopped || work->submode != UNPRESSED) {
        switch (work->mode) {
        case MODE_WAIT:
            MISSILE(task)->pos = work->pos;
            MISSILE(task)->pitch = 0;
            MISSILE(task)->yaw = work->ang.y;
            MISSILE(task)->roll = 0;
            MISSILE(task)->hatch = 0.0F;
            MISSILE(task)->frame = 0.0F;
            work->mode = MODE_RISE;
            switch ((work->ang.x & MISSILE_LAUNCH_BITS) % MISSILE_LAUNCHES) {
            /* @hack spelling: the default ahead of the cases, where after
             * them would do */
            default:
                break;
            case MISSILE_SWITCH:
                if (!switch_is_on(work->ang.z & MISSILE_ID)) {
                    work->mode = MODE_WAIT;
                }
                break;
            case MISSILE_ENEMY:
                if (!player_near(&work->pos, ENEMY_LAUNCH_RANGE) ||
                    !enemy_group_empty(work->ang.z & MISSILE_ID)) {
                    work->mode = MODE_WAIT;
                }
                break;
            }
            if (!(work->ang.z & MISSILE_ENEMY_COLLISION)) {
                /* @hack redundant: two counts of the targets of the
                 * missile's id, split by TARGET_UNSHUT, where one count
                 * would do */
                int clear = 0;
                int set = 0;

                for (i = 0, target = missile_targets; i < MISSILE_TARGETS;
                     i++, target++) {
                    if (TARGET_MISSILE_ID(target) ==
                        (work->ang.z & MISSILE_ID)) {
                        if (target->id & TARGET_UNSHUT) {
                            set++;
                        } else {
                            clear++;
                        }
                    }
                }
                if (clear == 0 && set == 0) {
                    work->mode = MODE_WAIT;
                }
            }
            if (work->mode == MODE_RISE) {
                sound_play_at(SOUND_LAUNCH, NULL, 0, 0, &work->pos);
            }
            break;
        case MODE_RISE:
            MISSILE(task)->hatch += HATCH_STEP;
            if (MISSILE(task)->hatch >= 1.0F) {
                MISSILE(task)->hatch = 1.0F;
                if (work->ang.x & MISSILE_UPSIDE_DOWN) {
                    MISSILE(task)->pos.y -= RISE_STEP;
                    if (MISSILE(task)->pos.y <= work->pos.y - RISE) {
                        MISSILE(task)->pos.y = work->pos.y - RISE;
                        MISSILE(task)->frame += MISSILE_FRAME_STEP;
                        if (MISSILE(task)->frame >= MISSILE_LAST_FRAME) {
                            MISSILE(task)->frame = MISSILE_LAST_FRAME;
                            if (MISSILE(task)->button_rise == 0.0F) {
                                MISSILE(task)->glow = 0.0F;
                                work->mode = MODE_AIM;
                                work->timer = 0;
                            }
                        }
                    }
                } else {
                    MISSILE(task)->pos.y += RISE_STEP;
                    if (MISSILE(task)->pos.y >= work->pos.y + RISE) {
                        MISSILE(task)->pos.y = work->pos.y + RISE;
                        MISSILE(task)->frame += MISSILE_FRAME_STEP;
                        if (MISSILE(task)->frame >= MISSILE_LAST_FRAME) {
                            MISSILE(task)->frame = MISSILE_LAST_FRAME;
                            if (MISSILE(task)->button_rise == 0.0F) {
                                MISSILE(task)->glow = 0.0F;
                                work->mode = MODE_AIM;
                                work->timer = 0;
                            }
                        }
                    }
                }
            }
            break;
        case MODE_AIM: {
            struct angles aim;
            NJS_VECTOR tail;
            Sint16 turn_yaw;
            Sint16 turn_pitch;
            Float dx;
            Float dz;

            /* @hack spelling: the way to the destination as -pos + scl,
             * where scl - pos would do */
            vec.x = -MISSILE(task)->pos.x + work->scl.x;
            vec.y = -MISSILE(task)->pos.y + work->scl.y;
            vec.z = -MISSILE(task)->pos.z + work->scl.z;
            /* @hack spelling: dz and dx copied from vec in the test, where
             * vec.z and vec.x read in place would do */
            if (dz = vec.z, dx = vec.x, work->ang.x & MISSILE_UPSIDE_DOWN) {
                aim.z = -(Angle)(atan2f(vec.y, njSqrt((dz * dz) + (dx * dx))) *
                                 ANGLE_PER_RADIAN);
                aim.y = -(Angle)(atan2f(vec.z, vec.x) * ANGLE_PER_RADIAN) +
                        HALF_TURN;
            } else {
                aim.z = (Angle)(atan2f(vec.y, njSqrt((dz * dz) + (dx * dx))) *
                                ANGLE_PER_RADIAN);
                aim.y = -(Angle)(atan2f(vec.z, vec.x) * ANGLE_PER_RADIAN);
            }
            /* @hack spelling: each turn as -angle + aim, where aim - angle
             * would do */
            turn_pitch = -(Sint16)MISSILE(task)->pitch + aim.z;
            turn_yaw = -(Sint16)MISSILE(task)->yaw + aim.y;
            if (turn_yaw > 0) {
                if (turn_yaw > AIM_TURN) {
                    turn_yaw = AIM_TURN;
                }
            } else if (turn_yaw < -AIM_TURN) {
                turn_yaw = -AIM_TURN;
            }
            /* @hack redundant: a second clamp of turn_yaw to -AIM_TURN that no
             * path reaches */
            if (0) {
                turn_yaw = -AIM_TURN;
            }
            if (turn_yaw == 0) {
                if (turn_pitch > 0) {
                    if (turn_pitch > AIM_TURN) {
                        turn_pitch = AIM_TURN;
                    }
                } else if (turn_pitch < -AIM_TURN) {
                    turn_pitch = -AIM_TURN;
                }
            } else {
                turn_pitch = 0;
            }
            MISSILE(task)->yaw += turn_yaw;
            MISSILE(task)->pitch += turn_pitch;
            MISSILE(task)->roll = njSin(work->timer * ROLL_STEP) *
                                  (ROLL_DEGREES * ANGLE_PER_DEGREE);
            if (work->ang.x & MISSILE_UPSIDE_DOWN) {
                MISSILE(task)->pos.y =
                    work->pos.y +
                    (njSin(work->timer++ * BOB_STEP) * BOB_HEIGHT) - RISE;
            } else {
                MISSILE(task)->pos.y =
                    work->pos.y +
                    (njSin(work->timer++ * BOB_STEP) * BOB_HEIGHT) + RISE;
            }
            tail.x = TAIL_X;
            tail.y = 0.0F;
            tail.z = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateV(NULL, &MISSILE(task)->pos);
            njRotateY(NULL, MISSILE(task)->yaw);
            njRotateZ(NULL, MISSILE(task)->pitch);
            njCalcPoint(NULL, &tail, &tail);
            /* @hack spelling: vec reused for the puff's speed, where a local of
             * its own would do */
            njCalcVector(NULL, &thrust_spd, &vec);
            njPopMatrix(1);
            sound_play_at_for(SOUND_THRUST, work, THRUST_PRIORITY, 0,
                              THRUST_SOUND_STEPS, &tail);
            if (rand() * RAND_SCALE < THRUST_CHANCE) {
                Float size;

                vec.x += ((rand() * RAND_SCALE) - 0.5F) * thrust_jitter;
                vec.y += ((rand() * RAND_SCALE) - 0.5F) * thrust_jitter;
                vec.z += ((rand() * RAND_SCALE) - 0.5F) * thrust_jitter;
                size =
                    (rand() * thrust_size_range * RAND_SCALE) + thrust_size_min;
                rocket_thrust_settle_create(&tail, &vec, size,
                                            work->pos.y + thrust_rise + size);
            }
            if ((Uint16)MISSILE(task)->yaw == (Uint16)aim.y &&
                (Uint16)MISSILE(task)->pitch == (Uint16)aim.z) {
                MISSILE(task)->hatch -= HATCH_STEP;
                if (MISSILE(task)->hatch <= 0.0F) {
                    MISSILE(task)->glow += GLOW_STEP;
                    MISSILE(task)->hatch = 0.0F;
                    if (MISSILE(task)->glow >= 1.0F) {
                        MISSILE(task)->glow = 1.0F;
                        work->timer = 0;
                        work->mode = MODE_FLY;
                        vibration_start(MISSILE(task)->presser, VIBRATION_POWER,
                                        VIBRATION_FREQ, 0);
                    }
                }
            }
            break;
        }
        case MODE_FLY: {
            NJS_VECTOR tail;
            Float scale;

            /* @hack spelling: the way to the destination as -pos + scl,
             * where scl - pos would do */
            vec.x = -MISSILE(task)->pos.x + work->scl.x;
            vec.y = -MISSILE(task)->pos.y + work->scl.y;
            vec.z = -MISSILE(task)->pos.z + work->scl.z;
            distance = njScalor(&vec);
            /* @hack spelling: scale, a copy of distance that the test reads,
             * where distance would do */
            scale = distance;
            /* @bug logic: a touch never ends a flight. the test reads
             * STEPS_FLOWN of the child that carries the collision, which stays
             * 0, where missile_collision_step counts the missile task's (MOV.L
             * at 0xb9a of the object's code, the count at 0x1348 and 0x1368) */
            if (scale < FLIGHT_STEP ||
                (task->child != NULL &&
                 STEPS_FLOWN(task->child) > FLIGHT_SAFE_STEPS &&
                 (task->child->work->collision->flags & COLLISION_TOUCHED))) {
                if (task->child == NULL ||
                    !(task->child->work->collision->flags &
                      COLLISION_TOUCHED)) {
                    MISSILE(task)->pos.x += vec.x;
                    MISSILE(task)->pos.y += vec.y;
                    MISSILE(task)->pos.z += vec.z;
                    for (i = 0, target = missile_targets; i < MISSILE_TARGETS;
                         i++, target++) {
                        if (TARGET_MISSILE_ID(target) ==
                                (work->ang.z & MISSILE_ID) &&
                            target->hit != NULL) {
                            *target->hit = &MISSILE(task)->pos;
                        }
                    }
                }
                work->mode = MODE_LANDED;
            } else {
                scale = FLIGHT_STEP / scale;
                vec.x *= scale;
                vec.y *= scale;
                vec.z *= scale;
                MISSILE(task)->pos.x += vec.x;
                MISSILE(task)->pos.y += vec.y;
                MISSILE(task)->pos.z += vec.z;
            }
            if (work->mode == MODE_LANDED) {
                /* @bug uninitialized: reads tail before missile_step sets
                 * it, and passes the scaled value as the burst's speed
                 * (FMOV.S at 0xd22, 0xd30 and 0xd3a of the object's code) */
#ifdef NONMATCHING
                /* this side repairs the tag above: the burst starts still.
                 * the image's tail is what the frame held, which C can't
                 * give */
                tail.x = 0.0F;
                tail.y = 0.0F;
                tail.z = 0.0F;
#endif
                tail.x *= BURST_SPEED_SCALE;
                tail.y *= BURST_SPEED_SCALE;
                tail.z *= BURST_SPEED_SCALE;
                burst_create_wobbling(&MISSILE(task)->pos, &tail, BURST_SCALE);
            }
            tail.x = TAIL_X;
            tail.y = 0.0F;
            tail.z = 0.0F;
            vec.x *= FLIGHT_PUFF_SPEED;
            vec.y *= FLIGHT_PUFF_SPEED;
            vec.z *= FLIGHT_PUFF_SPEED;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateV(NULL, &MISSILE(task)->pos);
            njRotateY(NULL, MISSILE(task)->yaw);
            njRotateZ(NULL, MISSILE(task)->pitch);
            njCalcPoint(NULL, &tail, &tail);
            njPopMatrix(1);
            sound_play_at_for(SOUND_THRUST, work, THRUST_PRIORITY, 0,
                              THRUST_SOUND_STEPS, &tail);
            if (!(game_state.steps & FLIGHT_PUFF_SKIP) &&
                distance > THRUST_DISTANCE) {
                vec.x += ((rand() * RAND_SCALE) - 0.5F) * FLIGHT_JITTER;
                vec.y += ((rand() * RAND_SCALE) - 0.5F) * FLIGHT_JITTER;
                vec.z += ((rand() * RAND_SCALE) - 0.5F) * FLIGHT_JITTER;
                rocket_thrust_create(
                    &tail, &vec,
                    (rand() * (FLIGHT_SIZE_RANGE * RAND_SCALE)) +
                        FLIGHT_SIZE_MIN);
                /* @bug logic: pops a matrix the step didn't push (JSR at
                 * 0xe58 of the object's code, after the pop at 0xdb8) */
                njPopMatrix(1);
            }
            break;
        }
        case MODE_LANDED:
            /* @hack spelling: MISSILE_SWITCH's case ahead of MISSILE_ONCE's,
             * where the enum's order would do */
            switch ((work->ang.x & MISSILE_LAUNCH_BITS) % MISSILE_LAUNCHES) {
            case MISSILE_EVERY:
                work->mode = MODE_WAIT;
                break;
            case MISSILE_SWITCH:
                work->mode = MODE_WAIT;
                switch_set(work->ang.z & MISSILE_ID, 0);
                break;
            case MISSILE_ONCE:
                work->mode = MODE_SPENT;
            }
            MISSILE(task)->pos = work->pos;
            MISSILE(task)->pitch = 0;
            MISSILE(task)->yaw = work->ang.y;
            MISSILE(task)->roll = 0;
            MISSILE(task)->frame = 0.0F;
            MISSILE(task)->hatch = 0.0F;
            MISSILE(task)->glow = 0.0F;
            work->submode = UNPRESSED;
            for (i = 0, target = missile_targets; i < MISSILE_TARGETS;
                 i++, target++) {
                if (target->hit != NULL &&
                    *target->hit == &MISSILE(task)->pos) {
                    *target->hit = NULL;
                }
            }
            break;
        case MODE_SPENT:
        default:
            break;
        }
    }
    /* @hack spelling: missile, a local for the record, where MISSILE(task)
     * read in place would do */
    missile = task->motion.p;
    if (player_near((NJS_POINT3 *)missile->base.pos,
                    missile->base.model->r + BASE_NEAR)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void missile_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct missile *missile;
    Float glow;

    /* @hack redundant: a block no path reaches, which keeps the set editor's
     * strings */
    if (0) {
        (void)Y_ANGLE_LABEL;
        (void)TARGET_ID_LABEL;
        (void)COLLISION_LABEL;
        (void)AIM_X_LABEL;
        (void)AIM_Y_LABEL;
        (void)AIM_Z_LABEL;
    }

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    if (work->ang.x & MISSILE_UPSIDE_DOWN) {
        njRotateY(NULL, work->ang.y);
        njRotateZ(NULL, HALF_TURN);
    } else {
        njRotateY(NULL, work->ang.y);
    }
    njSetTexture(&missile_base_texlist);
    njCnkEasyDrawMotion(&missile_base_object, &missile_base_motion,
                        MISSILE(task)->hatch);
    if (objects_fog_off) {
        njFogDisable();
    }
    njSetTexture(&missile_button_texlist);
    njTranslate(NULL, missile_button_object->pos[0],
                MISSILE(task)->button_rise + BUTTON_PRESSED_Y,
                missile_button_object->pos[2]);
    njCnkSimpleDrawModel(missile_button_object->model);
    if (objects_fog_off) {
        njFogEnable();
    }
    njPopMatrix(1);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPushMatrix(NULL);
    njTranslateV(NULL, &MISSILE(task)->pos);
    njRotateY(NULL, MISSILE(task)->yaw);
    njRotateZ(NULL, MISSILE(task)->pitch);
    njRotateX(NULL, MISSILE(task)->roll);
    if (work->ang.x & MISSILE_UPSIDE_DOWN) {
        njRotateZ(NULL, HALF_TURN);
    }
    if (work->mode != MODE_WAIT && work->mode != MODE_SPENT &&
        work->mode != MODE_LANDED) {
        njSetTexture(&missile_texlist);
        njCnkEasyDrawMotion(&missile_object, &missile_motion,
                            MISSILE(task)->frame);
        draw_control_save();
        draw_control_clear(NJD_CONTROL_3D_OFFSET_MATERIAL |
                           NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL);
        draw_control_set(NJD_CONTROL_3D_CONSTANT_MATERIAL);
        /* @hack redundant: glow read in the statement that sets missile,
         * though the next statement sets it again */
        glow = (missile = task->motion.p)->glow;
        glow = (game_state.steps & 1) ? missile->glow * GLOW_FLICKER
                                      : missile->glow;
        constant_material_set(1.0F, glow, glow, glow);
        njSetTexture(&missile_glow_texlist);
        njCnkSimpleDrawModel(&missile_glow_model);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_control_restore();
    }
    njPopMatrix(1);
}

/* the first step of the child that carries the missile's collision */
static void missile_collision_init(struct task *task)
{
    struct task *parent = task->parent;
    struct task_work *work = task->work;
    struct task_work *parent_work = parent->work;

    if (parent->motion.p != NULL) {
        if (parent_work->ang.z & MISSILE_ENEMY_COLLISION) {
            collision_init(task, &missile_enemy_shape, 1, COLLISION_ENEMY);
        } else {
            collision_init(task, &missile_shape, 1, COLLISION_OBJECT);
        }
        task->step = missile_collision_step;
        task->destructor = missile_collision_destroy;
        work->pos = MISSILE(parent)->pos;
        work->ang.x = 0;
        work->ang.y =
            MISSILE(parent)->yaw +
            ((parent_work->ang.x & MISSILE_UPSIDE_DOWN) ? HALF_TURN : 0);
        work->ang.z = MISSILE(parent)->pitch;
    }
}

static void missile_collision_step(struct task *task)
{
    struct task *parent = task->parent;
    struct task_work *work = task->work;
    struct task_work *parent_work = parent->work;

    if (parent->motion.p == NULL) {
        return;
    }
    work->pos = MISSILE(parent)->pos;
    work->ang.x = 0;
    work->ang.y = MISSILE(parent)->yaw +
                  ((parent_work->ang.x & MISSILE_UPSIDE_DOWN) ? HALF_TURN : 0);
    work->ang.z = MISSILE(parent)->pitch;
    if (parent_work->mode == MODE_FLY) {
        if (STEPS_FLOWN(parent) > FLIGHT_SAFE_STEPS) {
            work->collision->shapes->levels =
                (work->collision->shapes->levels & ~SHAPE_ATTACK) |
                FLIGHT_ATTACK;
        } else {
            STEPS_FLOWN(parent)++;
        }
    } else {
        work->collision->shapes->levels =
            work->collision->shapes->levels & ~SHAPE_ATTACK;
        STEPS_FLOWN(parent) = 0;
    }
    collision_register(task);
}

static void missile_collision_destroy(struct task *task)
{
    /* @hack redundant: the reads of the missile and its work, which nothing
     * uses */
    struct task *parent = task->parent;
    struct task_work *parent_work = parent->work;
}
