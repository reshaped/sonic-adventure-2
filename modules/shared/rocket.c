/* the rocket, ROCKET in a stage's object list: a launcher that raises a
 * rocket from its base. a player who takes hold of the rocket rides it to a
 * place. the launcher is the object's task, and each rocket, a body, is a
 * child task of it. Final Rush's STROCKET shares the launcher and body steps */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "effect/burst.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/rocket.h"
#include "shared/rocket_thrust.h"
#include "shared/switch.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "shared/rocket_work.h"

#include "shared/rocket/text.inc"

/* The rideable rocket and its two sliding hatch doors, in model order. */

static NJS_TEXNAME rocket_textures[3] = {
#include "shared/rocket/rocket_textures.inc"
};

NJS_TEXLIST rocket_texlist = {
#include "shared/rocket/rocket_texlist.inc"
};

static Sint16 rocket_fin_lower_negative_z_polygons[325] = {
#include "shared/rocket/rocket_fin_lower_negative_z_polygons.inc"
};

static Sint32 rocket_fin_lower_negative_z_vertices[189] = {
#include "shared/rocket/rocket_fin_lower_negative_z_vertices.inc"
};

static NJS_CNK_MODEL rocket_fin_lower_negative_z_model = {
#include "shared/rocket/rocket_fin_lower_negative_z_model.inc"
};

static NJS_CNK_OBJECT rocket_fin_lower_negative_z = {
#include "shared/rocket/rocket_fin_lower_negative_z.inc"
};

static Sint16 rocket_fin_lower_positive_z_polygons[317] = {
#include "shared/rocket/rocket_fin_lower_positive_z_polygons.inc"
};

static Sint32 rocket_fin_lower_positive_z_vertices[189] = {
#include "shared/rocket/rocket_fin_lower_positive_z_vertices.inc"
};

static NJS_CNK_MODEL rocket_fin_lower_positive_z_model = {
#include "shared/rocket/rocket_fin_lower_positive_z_model.inc"
};

static NJS_CNK_OBJECT rocket_fin_lower_positive_z = {
#include "shared/rocket/rocket_fin_lower_positive_z.inc"
};

static Sint16 rocket_fin_upper_negative_z_polygons[311] = {
#include "shared/rocket/rocket_fin_upper_negative_z_polygons.inc"
};

static Sint32 rocket_fin_upper_negative_z_vertices[189] = {
#include "shared/rocket/rocket_fin_upper_negative_z_vertices.inc"
};

static NJS_CNK_MODEL rocket_fin_upper_negative_z_model = {
#include "shared/rocket/rocket_fin_upper_negative_z_model.inc"
};

static NJS_CNK_OBJECT rocket_fin_upper_negative_z = {
#include "shared/rocket/rocket_fin_upper_negative_z.inc"
};

static Sint16 rocket_fin_upper_positive_z_polygons[313] = {
#include "shared/rocket/rocket_fin_upper_positive_z_polygons.inc"
};

static Sint32 rocket_fin_upper_positive_z_vertices[189] = {
#include "shared/rocket/rocket_fin_upper_positive_z_vertices.inc"
};

static NJS_CNK_MODEL rocket_fin_upper_positive_z_model = {
#include "shared/rocket/rocket_fin_upper_positive_z_model.inc"
};

static NJS_CNK_OBJECT rocket_fin_upper_positive_z = {
#include "shared/rocket/rocket_fin_upper_positive_z.inc"
};

static NJS_CNK_OBJECT rocket_handle_tip = {
#include "shared/rocket/rocket_handle_tip.inc"
};

static Sint16 rocket_handle_polygons[215] = {
#include "shared/rocket/rocket_handle_polygons.inc"
};

static Sint32 rocket_handle_vertices[159] = {
#include "shared/rocket/rocket_handle_vertices.inc"
};

static NJS_CNK_MODEL rocket_handle_model = {
#include "shared/rocket/rocket_handle_model.inc"
};

static NJS_CNK_OBJECT rocket_handle = {
#include "shared/rocket/rocket_handle.inc"
};

static Sint16 rocket_handle_joint_polygons[79] = {
#include "shared/rocket/rocket_handle_joint_polygons.inc"
};

static Sint32 rocket_handle_joint_vertices[51] = {
#include "shared/rocket/rocket_handle_joint_vertices.inc"
};

static NJS_CNK_MODEL rocket_handle_joint_model = {
#include "shared/rocket/rocket_handle_joint_model.inc"
};

static NJS_CNK_OBJECT rocket_handle_joint = {
#include "shared/rocket/rocket_handle_joint.inc"
};

static Sint16 rocket_nose_polygons[203] = {
#include "shared/rocket/rocket_nose_polygons.inc"
};

static Sint32 rocket_nose_vertices[105] = {
#include "shared/rocket/rocket_nose_vertices.inc"
};

static NJS_CNK_MODEL rocket_nose_model = {
#include "shared/rocket/rocket_nose_model.inc"
};

static NJS_CNK_OBJECT rocket_nose = {
#include "shared/rocket/rocket_nose.inc"
};

static Sint16 rocket_polygons[363] = {
#include "shared/rocket/rocket_polygons.inc"
};

static Sint32 rocket_vertices[201] = {
#include "shared/rocket/rocket_vertices.inc"
};

static NJS_CNK_MODEL rocket_model = {
#include "shared/rocket/rocket_model.inc"
};

NJS_CNK_OBJECT rocket_object = {
#include "shared/rocket/rocket_object.inc"
};

static NJS_TEXNAME rocket_base_textures[2] = {
#include "shared/rocket/rocket_base_textures.inc"
};

NJS_TEXLIST rocket_base_texlist = {
#include "shared/rocket/rocket_base_texlist.inc"
};

static Sint16 rocket_hatch_positive_z_polygons[41] = {
#include "shared/rocket/rocket_hatch_positive_z_polygons.inc"
};

static Sint32 rocket_hatch_positive_z_vertices[39] = {
#include "shared/rocket/rocket_hatch_positive_z_vertices.inc"
};

static NJS_CNK_MODEL rocket_hatch_positive_z_model = {
#include "shared/rocket/rocket_hatch_positive_z_model.inc"
};

static NJS_CNK_OBJECT rocket_hatch_positive_z = {
#include "shared/rocket/rocket_hatch_positive_z.inc"
};

static Sint16 rocket_hatch_negative_z_polygons[41] = {
#include "shared/rocket/rocket_hatch_negative_z_polygons.inc"
};

static Sint32 rocket_hatch_negative_z_vertices[39] = {
#include "shared/rocket/rocket_hatch_negative_z_vertices.inc"
};

static NJS_CNK_MODEL rocket_hatch_negative_z_model = {
#include "shared/rocket/rocket_hatch_negative_z_model.inc"
};

static NJS_CNK_OBJECT rocket_hatch_negative_z = {
#include "shared/rocket/rocket_hatch_negative_z.inc"
};

static Sint16 rocket_base_polygons[387] = {
#include "shared/rocket/rocket_base_polygons.inc"
};

static Sint32 rocket_base_vertices[219] = {
#include "shared/rocket/rocket_base_vertices.inc"
};

static NJS_CNK_MODEL rocket_base_model = {
#include "shared/rocket/rocket_base_model.inc"
};

NJS_CNK_OBJECT rocket_base_object = {
#include "shared/rocket/rocket_base_object.inc"
};

/* SHC accepts the two packed material colors as scalars. */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL rocket_floor_material = {
#include "shared/rocket/rocket_floor_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 rocket_floor_indices[42] = {
#include "shared/rocket/rocket_floor_indices.inc"
};

static NJS_MESHSET rocket_floor_meshset = {
#include "shared/rocket/rocket_floor_meshset.inc"
};

static NJS_POINT3 rocket_floor_points[17] = {
#include "shared/rocket/rocket_floor_points.inc"
};

static NJS_VECTOR rocket_floor_normals[17] = {
#include "shared/rocket/rocket_floor_normals.inc"
};

static NJS_MODEL rocket_floor_mesh = {
#include "shared/rocket/rocket_floor_mesh.inc"
};

NJS_OBJECT rocket_base_collision = {
#include "shared/rocket/rocket_base_collision.inc"
};

static NJS_MKEY_F rocket_nose_positions[6] = {
#include "shared/rocket/rocket_nose_positions.inc"
};

static NJS_MKEY_A rocket_handle_angles[4] = {
#include "shared/rocket/rocket_handle_angles.inc"
};

static NJS_MKEY_F rocket_fin_upper_positive_z_positions[7] = {
#include "shared/rocket/rocket_fin_upper_positive_z_positions.inc"
};

static NJS_MKEY_A rocket_fin_upper_positive_z_angles[7] = {
#include "shared/rocket/rocket_fin_upper_positive_z_angles.inc"
};

static NJS_MKEY_F rocket_fin_upper_negative_z_positions[7] = {
#include "shared/rocket/rocket_fin_upper_negative_z_positions.inc"
};

static NJS_MKEY_A rocket_fin_upper_negative_z_angles[7] = {
#include "shared/rocket/rocket_fin_upper_negative_z_angles.inc"
};

static NJS_MKEY_F rocket_fin_lower_positive_z_positions[7] = {
#include "shared/rocket/rocket_fin_lower_positive_z_positions.inc"
};

static NJS_MKEY_A rocket_fin_lower_positive_z_angles[7] = {
#include "shared/rocket/rocket_fin_lower_positive_z_angles.inc"
};

static NJS_MKEY_F rocket_fin_lower_negative_z_positions[7] = {
#include "shared/rocket/rocket_fin_lower_negative_z_positions.inc"
};

static NJS_MKEY_A rocket_fin_lower_negative_z_angles[7] = {
#include "shared/rocket/rocket_fin_lower_negative_z_angles.inc"
};

static NJS_MDATA2 rocket_motion_channels[9] = {
#include "shared/rocket/rocket_motion_channels.inc"
};

NJS_MOTION rocket_motion = {
#include "shared/rocket/rocket_motion.inc"
};

static NJS_MKEY_F rocket_hatch_negative_z_positions[2] = {
#include "shared/rocket/rocket_hatch_negative_z_positions.inc"
};

static NJS_MKEY_F rocket_hatch_positive_z_positions[2] = {
#include "shared/rocket/rocket_hatch_positive_z_positions.inc"
};

static NJS_MDATA2 rocket_base_motion_channels[3] = {
#include "shared/rocket/rocket_base_motion_channels.inc"
};

NJS_MOTION rocket_base_motion = {
#include "shared/rocket/rocket_base_motion.inc"
};

/* how far the rocket rises above the launcher's pos */
static Float body_rise = 8.0F;
/* the point of the rocket's model that lies at the rocket's pos */
static NJS_POINT3 body_model_offset = {8.1F, -5.0F, 0.0F};
/* where a held rider stands, from the rocket's pos turned by its ang.y
 * alone */
static NJS_POINT3 rider_hold_offset = {-3.0F, -6.0F, 0.0F};
/* Sonic's and Shadow's offset in flight, as rocket_body_step's switch gives
 * it */
static NJS_POINT3 dat_rocket_0 = {-4.3F, -4.9F, 0.0F};
/* an aiming rocket's puffs of thrust. thrust_spd is a puff's speed in the
 * frame of the rocket's model. thrust_jitter is the span of a random change
 * to each part of that speed. a puff's size runs from thrust_size_min up to
 * thrust_size_min + thrust_size_range. its floor lies thrust_rise plus its
 * size above the launcher's pos */
static NJS_VECTOR thrust_spd = {0.0F, -2.7F, 0.0F};
static Float thrust_jitter = 0.8F;
static Float thrust_rise = 1.0F;
static Float thrust_size_min = 2.0F;
static Float thrust_size_range = 1.0F;
static Float dat_rocket_1 = 20.0F;
static Float dat_rocket_2 = 20.0F;

NJS_TEXLIST *rocket_texlists[5] = {
#include "shared/rocket/rocket_texlists.inc"
};

/* an upright rocket's record negates the hull's center.y */
/* clang-format off */
static struct collision_shape body_shapes[BODY_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_ALL | SHAPE_GUARD_LEVEL(1) | HULL_ATTACK,
     SHAPE_UNK_800000 | SHAPE_HURTABLE | SHAPE_TURN_YZX, {4.5F, 9.4F, 0.0F},
     2.4F, 6.0F, 0.0F, 0, {0, 0, QUARTER_TURN}},
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_UNK_800000 | SHAPE_TURN_YZX, {0.0F, 0.0F, 0.0F}, 2.0F, 0.0F, 0.0F,
     0, {0, 0, QUARTER_TURN}},
};
/* clang-format on */

#include "shared/rocket_launcher.h"

/* in a time stop, the step does its mode's work only in ROCKET_RISE. after a
 * catch in the stop, it does the work in any mode. the stop holds a rising
 * rocket and its base still. a player can still take hold of the rocket */
static void rocket_step(struct task *task)
{
    struct task_work *work = task->work;
    struct launcher *launcher;
    struct task *body;
    int slot;
    /* @hack spelling: vec reused for the upright rocket's shift, the way to
     * the place and the thrust's speed, where a local for each would do */
    NJS_VECTOR vec;
    struct angles aim;
    NJS_POINT3 tail;
    Sint16 turn_y;
    Sint16 turn_z;
    Float size;

    if (LAUNCHER(task)->riders == 0 && placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, which leaves its strings in
     * the object */
    if (0) {
        /* @hack redundant: the first two strings picked by ang.x, where each
         * on its own would do */
        (void)((work->ang.x & ROCKET_ON_SWITCH) ? APPEAR_ON_SWITCH_LABEL
                                                : APPEAR_ALWAYS_LABEL);
        (void)Y_ANGLE_LABEL;
        (void)SWITCH_ID_LABEL;
        (void)AIM_X_LABEL;
        (void)AIM_Y_LABEL;
        (void)AIM_Z_LABEL;
    }
    /* @hack spelling: the test of ROCKET_RISE as a loop that jumps into the
     * if's body, where a first operand of the if's || would do */
    while (work->mode == ROCKET_RISE) {
        goto run;
    }
    if (CAUGHT_IN_STOP(work) || !game_state.time_stopped) {
    run:
        if (!game_state.time_stopped) {
            CAUGHT_IN_STOP(work) = 0;
        }
        switch (work->mode) {
        case ROCKET_WAIT:
            if (LAUNCHER(task)->body == NULL &&
                (!(work->ang.x & ROCKET_ON_SWITCH) ||
                 switch_is_on(work->ang.z & ROCKET_SWITCH_ID))) {
                LAUNCHER(task)->body =
                    task_create_child(TASK_WORK, rocket_body_init, task);
                /* @bug memory: the rise reads the body's work through null
                 * where task_create_child had no body to give (MOV.L at
                 * 0x470 and 0x48c of the object's code). this step makes
                 * ROCKET_RISE the mode either way, though it tests the body
                 * for null next (TST at 0x312) */
#ifdef NONMATCHING
                /* this side repairs the tag above: without a body the launcher
                 * stays in ROCKET_WAIT, to try again on its next step */
                if (LAUNCHER(task)->body == NULL) {
                    break;
                }
#endif
                work->mode = ROCKET_RISE;
                sound_play(SOUND_RISE, NULL, 0, 0);
            }
            if (LAUNCHER(task)->body != NULL) {
                BODY_WORK(task)->submode = NO_RIDER;
                BODY_WORK(task)->pos = work->pos;
                if (work->submode == ROCKET_UPRIGHT) {
                    vec.x = WAIT_UPRIGHT_X;
                    vec.y = 0.0F;
                    vec.z = 0.0F;
                    njPushMatrixEx();
                    njUnitMatrix(NULL);
                    njRotateY(NULL, work->ang.y);
                    njCalcVector(NULL, &vec, &vec);
                    ROCKET_POP();
                    BODY_WORK(task)->pos.x += vec.x;
                    BODY_WORK(task)->pos.z += vec.z;
                }
                BODY_WORK(task)->pos.y -= WAIT_DROP;
                BODY_WORK(task)->ang.z =
                    (work->submode == ROCKET_UPRIGHT) ? QUARTER_TURN : 0;
                BODY_WORK(task)->ang.y = work->ang.y;
                BODY_WORK(task)->ang.x = 0;
                LAUNCHER(task)->base_frame = 0.0F;
                LAUNCHER(task)->body_frame = 0.0F;
            }
            break;
        case ROCKET_RISE:
            if (!game_state.time_stopped) {
                LAUNCHER(task)->base_frame += BASE_FRAME_STEP;
            }
            if (LAUNCHER(task)->base_frame >= BASE_FRAMES) {
                LAUNCHER(task)->base_frame = BASE_FRAMES;
                if (!game_state.time_stopped) {
                    BODY_WORK(task)->pos.y += RISE_STEP;
                }
                if (BODY_WORK(task)->pos.y >= work->pos.y + body_rise) {
                    BODY_WORK(task)->pos.y = work->pos.y + body_rise;
                    if (!game_state.time_stopped) {
                        LAUNCHER(task)->body_frame += BODY_FRAME_STEP;
                    }
                    if (LAUNCHER(task)->body_frame >= BODY_FRAMES) {
                        LAUNCHER(task)->body_frame = BODY_FRAMES;
                        /* @bug logic: tests the launcher's first child for a
                         * catch, not its body. while an earlier rocket still
                         * flies, the first child is that rocket, whose grip
                         * is off. no player can then take hold of the new
                         * rocket until the earlier one is destroyed (MOV.L
                         * at 0x4d8 of the object's code) */
                        body = task->child;
                        /* @hack spelling: the assignment inside the test,
                         * where a statement of its own would do */
                        if (body != NULL &&
                            (body->work->collision->flags &
                             COLLISION_TOUCHED) &&
                            body->work->collision->own_shape == BODY_GRIP &&
                            (slot = player_slot_of(
                                 body->work->collision->hit->owner)) >= 0) {
                            BODY_WORK(task)->scl = work->scl;
                            BODY_WORK(task)->submode = slot;
                            BODY_WORK(task)->mode = BODY_HELD;
                            BODY_WORK(task)->tick = game_state.time_stopped;
                            player_request(slot, PLAYER_REQUEST_ANIM,
                                           RIDE_HOLD);
                            LAUNCHER(task)->riders++;
                            work->mode = ROCKET_AIM;
                            work->timer = 0;
                            player_work[slot]->ang.x = 0;
                            player_work[slot]->ang.y = BODY_WORK(task)->ang.y;
                            player_work[slot]->ang.z = 0;
                            CAUGHT_IN_STOP(work) = game_state.time_stopped;
                        }
                    }
                }
            }
            break;
        case ROCKET_AIM:
            BODY_WORK(task)->tick = game_state.time_stopped;
#include "shared/rocket_aim.inc"
            work->timer++;
            if ((Uint16)BODY_WORK(task)->ang.y == (Uint16)aim.y &&
                (Uint16)BODY_WORK(task)->ang.z == (Uint16)aim.z &&
                work->timer > AIM_STEPS) {
                LAUNCHER(task)->base_frame -= BASE_FRAME_STEP;
                if (LAUNCHER(task)->base_frame <= 0.0F) {
                    LAUNCHER(task)->base_frame = 0.0F;
                    player_request(BODY_WORK(task)->submode,
                                   PLAYER_REQUEST_ANIM, RIDE_FLY);
                    work->timer = 0;
                    work->mode = ROCKET_WAIT;
                    BODY_WORK(task)->mode = BODY_FLYING;
                    LAUNCHER(task)->body = NULL;
                }
            }
            break;
        }
    }
    /* @hack spelling: launcher, a local for the record, where LAUNCHER(task)
     * read in place would do */
    launcher = task->motion.p;
    if (player_near((NJS_POINT3 *)launcher->base.pos,
                    launcher->base.model->r + BASE_NEAR)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

#include "shared/rocket_body.h"
