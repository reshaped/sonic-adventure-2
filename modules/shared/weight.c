/* the weight, BUNCHIN in a stage's object list: a block that waits above the
 * ground. it drops, bounces where it lands and rises back */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "effect/particle.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/modifier_volume.h"
#include "shared/switch.h"
#include "shared/weight.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"
#include "shared/weight/text.inc"

enum weight_mode { MODE_WAIT, MODE_RISE, MODE_FALL, MODE_LANDED };

struct weight {
    Float height; /* how far the weight's bottom lies above pos */
    /* the weight's copy of weight_collision in the collision world */
    NJS_OBJECT object;
    /* where the weight dropped from MODE_WAIT on a player: that player's y
     * less pos.y, at most 0. any other way out of MODE_WAIT sets it to 0 */
    Float target_y;
};

#define WEIGHT(task) ((struct weight *)(task)->motion.p)
/* the weight's speed up y in MODE_FALL and MODE_LANDED */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define SPEED(task) (*(Float *)&(task)->unk_3c)

#define TYPE(work) ((work)->ang.x & WEIGHT_TYPE_BITS)
#define WEIGHT_SWITCH_IS_ON(work)                                              \
    (switch_is_on((work)->ang.z & WEIGHT_SWITCH_ID_BITS))

/* the scale a scl.x or scl.z gives the weight in x or z */
#define SCALE(scl) ((scl) + 1.0F)
/* half the weight's width in x, and half its depth in z, at a scl of 0 */
#define HALF_SIZE 20.0F
#define HALF_X(work) (SCALE((work)->scl.x) * HALF_SIZE)
#define HALF_Z(work) (SCALE((work)->scl.z) * HALF_SIZE)
#define WIDTH (2.0F * HALF_SIZE)
#define BLOCK_HEIGHT 20.0F
/* the weight's bottom waits |scl.y| + REST_BASE above pos */
#define REST_BASE 30.0F
#define REST_HEIGHT(work) (_builtin_fabsf((work)->scl.y) + REST_BASE)
#define MAX_SCL 300.0F

/* the weight's column reaches COLUMN_REACH past its sides in x and z, and
 * COLUMN_HALF_HEIGHT above and below pos. IN_COLUMN's player_work[slot] must
 * not be null */
#define COLUMN_REACH 20.0F
#define COLUMN_HALF_HEIGHT 1000.0F
#ifdef NONMATCHING
/* this side repairs the tag in weight_step's MODE_WAIT: an empty slot lies in
 * no column, so the wait drops the weight on no one there */
#define IN_COLUMN(work, slot)                                                  \
    (player_work[slot] != NULL &&                                              \
     weight_box_contains(&player_work[slot]->pos, &(work)->pos, (work)->ang.y, \
                         HALF_X(work) + COLUMN_REACH, COLUMN_HALF_HEIGHT,      \
                         HALF_Z(work) + COLUMN_REACH))
#else
#define IN_COLUMN(work, slot)                                                  \
    weight_box_contains(&player_work[slot]->pos, &(work)->pos, (work)->ang.y,  \
                        HALF_X(work) + COLUMN_REACH, COLUMN_HALF_HEIGHT,       \
                        HALF_Z(work) + COLUMN_REACH)
#endif
#define PLAYER_IN_COLUMN(work)                                                 \
    ((player_work[0] != NULL && IN_COLUMN(work, 0)) ||                         \
     (player_work[1] != NULL && IN_COLUMN(work, 1)))
/* a waiting weight of either DOWN_ON_SONIC type drops only on a player whose
 * flags hold one of these */
#define DROP_PLAYER_FLAGS (PLAYER_WORK_ON_GROUND | PLAYER_WORK_ON_SHAPE)

/* the copy's size, for its WORLD_SIZE_CLASS, is its model's radius times its
 * largest scale, plus CLASS_MARGIN */
#define CLASS_MARGIN 10.0F
/* the step sets WORLD_ADDED while player_near finds a player near the middle
 * of the weight's top. the range is the copy's model radius times its largest
 * scale, plus NEAR_MARGIN */
#define NEAR_MARGIN 32.0F

#define HANDLE_BUILD 0
#define WEIGHT_BUILDS 1

/* weight_shapes' indexes */
#define TOP_SHAPE 0 /* a ball at the middle of the weight's top */
/* a box along the weight's bottom, which hurts what it touches. it has
 * SHAPE_OFF outside MODE_FALL, and while game_state.time_stopped isn't 0 */
#define BOTTOM_SHAPE 1
/* a box whose top lies BELOW_GAP under target_y. it has SHAPE_OFF outside
 * MODE_FALL */
#define BELOW_SHAPE 2
#define WEIGHT_SHAPES 3
#define SHAPE_HALF_HEIGHT 7.5F /* half the height of either box */
#define BELOW_GAP 1.0F

#define LIFT_REACH 10.0F
#define LIFT_HALF_HEIGHT 20.0F

/* the landing's dust lines a rectangle DUST_INSET inside the weight's sides */
#define DUST_INSET 5.0F
/* the landing vibrates the pad of each player within VIBRATION_RANGE times
 * DIAGONAL of pos. DIAGONAL is the diagonal of the weight's footprint */
#define VIBRATION_RANGE 1.5F
#define VIBRATION_POWER 5
#define VIBRATION_FREQ 20
#define DIAGONAL(work)                                                         \
    njSqrt((SCALE((work)->scl.x) * SCALE((work)->scl.x) * (WIDTH * WIDTH)) +   \
           (SCALE((work)->scl.z) * SCALE((work)->scl.z) * (WIDTH * WIDTH)))
#define FELT(work, slot)                                                       \
    (player_work[slot] != NULL &&                                              \
     njDistanceP2P(&player_work[slot]->pos, &(work)->pos) <                    \
         DIAGONAL(work) * VIBRATION_RANGE)
#define LAND_SOUND 0x1004
/* the part of its speed a landed weight keeps as it bounces off the ground */
#define BOUNCE 0.3F

/* the bottom's and the frame's tints at their darkest. the bottom glows and
 * the frame blinks between those and LIGHT */
#define BOTTOM_DARK 0xff101010
#define FRAME_DARK 0xff202020
#define LIGHT 0xffffffff
#define BLINK_PERIOD 30
#define BLINK_REST 15
#define BLINK_FADE 5
/* the shadow stretches modifier_volume_box_model over the weight's footprint.
 * it runs from a foot below pos up to a head near the weight's bottom */
#define SHADOW_HALF 15.0F /* half the side of modifier_volume_box_model */
#define SHADOW_FOOT 5.5F /* how far below pos the foot lies */
/* how much lower WEIGHT_LONG_SHADOW puts the foot */
#define LONG_SHADOW_DROP 200.0F
#define SHADOW_HEAD 0.5F /* how far above the weight's bottom the head lies */
/* the head lies no more than SHADOW_RISE + SHADOW_HEAD above pos */
#define SHADOW_RISE 200.0F

static void weight_destroy(struct task *task);
static void weight_step(struct task *task);
static void weight_draw(struct task *task);

/* the handle on the weight's top. weight_build builds its node */
static NJS_TEXNAME weight_handle_texnames[2] = {
#include "shared/weight/weight_handle_texnames.inc"
};
static NJS_TEXLIST weight_handle_texlist = {
#include "shared/weight/weight_handle_texlist.inc"
};
static Sint16 weight_handle_polygons[201] = {
#include "shared/weight/weight_handle_polygons.inc"
};
static Sint32 weight_handle_vertices[207] = {
#include "shared/weight/weight_handle_vertices.inc"
};
static NJS_CNK_MODEL weight_handle_model = {
#include "shared/weight/weight_handle_model.inc"
};
static NJS_CNK_OBJECT weight_handle_object = {
#include "shared/weight/weight_handle_object.inc"
};
/* the body. the draw scales it in x by scl.x + 1 and in z by scl.z + 1. it
 * scales the frame and the bottom alike */
static NJS_TEXNAME weight_body_texnames[1] = {
#include "shared/weight/weight_body_texnames.inc"
};
static NJS_TEXLIST weight_body_texlist = {
#include "shared/weight/weight_body_texlist.inc"
};
static Sint16 weight_body_polygons[87] = {
#include "shared/weight/weight_body_polygons.inc"
};
static Sint32 weight_body_vertices[147] = {
#include "shared/weight/weight_body_vertices.inc"
};
static NJS_CNK_MODEL weight_body_model = {
#include "shared/weight/weight_body_model.inc"
};
static NJS_CNK_OBJECT weight_body_object = {
#include "shared/weight/weight_body_object.inc"
};
/* SHC refuses braces around the material's two colors, with error 2129 */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL weight_collision_material[1] = {
#include "shared/weight/weight_collision_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */
struct weight_collision_mesh_list {
    Uint16 mesh[4];
    Uint16 mesh_2[4];
    Uint16 mesh_3[13];
};
static struct weight_collision_mesh_list weight_collision_meshes = {
#include "shared/weight/weight_collision_meshes.inc"
};
static NJS_MESHSET weight_collision_meshsets[1] = {
#include "shared/weight/weight_collision_meshsets.inc"
};
static NJS_POINT3 weight_collision_points[8] = {
#include "shared/weight/weight_collision_points.inc"
};
static NJS_VECTOR weight_collision_normals[8] = {
#include "shared/weight/weight_collision_normals.inc"
};
static NJS_MODEL weight_collision_model = {
#include "shared/weight/weight_collision_model.inc"
};
/* the box the players stand on, 40 on a side and 20 high. its origin is the
 * middle of its top */
static NJS_OBJECT weight_collision = {
#include "shared/weight/weight_collision.inc"
};
/* the frame along the body's edges, and the bottom face. the draw tints
 * both */
static NJS_TEXNAME weight_frame_texnames[1] = {
#include "shared/weight/weight_frame_texnames.inc"
};
static NJS_TEXLIST weight_frame_texlist = {
#include "shared/weight/weight_frame_texlist.inc"
};
static Sint16 weight_frame_polygons[301] = {
#include "shared/weight/weight_frame_polygons.inc"
};
static Sint32 weight_frame_vertices[147] = {
#include "shared/weight/weight_frame_vertices.inc"
};
static NJS_CNK_MODEL weight_frame_model = {
#include "shared/weight/weight_frame_model.inc"
};
static NJS_CNK_OBJECT weight_frame_object = {
#include "shared/weight/weight_frame_object.inc"
};
static Sint16 weight_bottom_polygons[15] = {
#include "shared/weight/weight_bottom_polygons.inc"
};
static Sint32 weight_bottom_vertices[19] = {
#include "shared/weight/weight_bottom_vertices.inc"
};
static NJS_CNK_MODEL weight_bottom_model = {
#include "shared/weight/weight_bottom_model.inc"
};
static NJS_CNK_OBJECT weight_bottom_object = {
#include "shared/weight/weight_bottom_object.inc"
};

/* TRUE once weight_build has built the handle */
static Bool weight_built = FALSE;
static struct model_build weight_builds[WEIGHT_BUILDS] = {
#include "shared/weight/weight_builds.inc"
};
NJS_TEXLIST *weight_texlists[5] = {
#include "shared/weight/weight_texlists.inc"
};
static Float weight_rise_speed = 0.5F;
static Float weight_gravity = 0.2F;
static Float weight_fall_drag = 0.992F;
static Float weight_bounce_drag = 0.985F;
static Float weight_bounce_spring = 0.3F;
/* the names of the weight_types, in order, then of a weight without
 * WEIGHT_NORMAL and of one with it */
static const char *weight_names[6] = {
#include "shared/weight/weight_names.inc"
};
/* weight_init sizes BOTTOM_SHAPE and BELOW_SHAPE to the weight. collision_init
 * copies the shapes, so each record has its own */
/* clang-format off */
static struct collision_shape weight_shapes[WEIGHT_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 0.0F, 0.0F}, 6.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(2) | SHAPE_HURTS_ALL, SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, HALF_SIZE, SHAPE_HALF_HEIGHT, HALF_SIZE, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0,
     {0.0F, 0.0F, 0.0F}, HALF_SIZE, SHAPE_HALF_HEIGHT, HALF_SIZE, 0, {0, 0, 0}},
};
/* clang-format on */
static Float weight_dust_density = 1.4F;
static Float weight_dust_speed = 0.4F;
static Float weight_dust_scale = 3.0F;
/* a landed weight counts its steps in its work's timer. it may rise once the
 * count passes weight_landed_steps */
static Uint32 weight_landed_steps = 50;
static Sint32 weight_land_volume = 50;
/* the bottom's glow follows the sine of game_state.steps times
 * weight_glow_step. a sine of -1 gives BOTTOM_DARK, and 1 gives LIGHT */
static Angle weight_glow_step = 0x900;

Uint8 *weight_build(Uint8 *buf)
{
    if (model_builds_compile(&buf, weight_builds, WEIGHT_BUILDS) ==
        WEIGHT_BUILDS) {
        weight_built = TRUE;
    }
    return buf;
}

/* whether pos lies inside a box about center, turned by ang about y. the box
 * reaches half_x, half_y and half_z from center along its own axes. a point
 * on a face lies outside it */
static Bool weight_box_contains(NJS_POINT3 *pos, NJS_POINT3 *center, Angle ang,
                                Float half_x, Float half_y, Float half_z)
{
    NJS_VECTOR offset;

    offset.x = center->x - pos->x;
    offset.y = center->y - pos->y;
    offset.z = center->z - pos->z;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, -ang);
    njCalcVector(NULL, &offset, &offset);
    njPopMatrixEx();
    return offset.x < half_x && offset.x > -half_x && offset.y < half_y &&
           offset.y > -half_y && offset.z < half_z && offset.z > -half_z;
}

void weight_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float radius;
    Uint32 flags;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct weight));
    if (task->motion.p == NULL) {
        return;
    }
    task->draw = weight_draw;
    task->step = weight_step;
    task->destructor = weight_destroy;
    work->submode = MODE_WAIT;
    WEIGHT(task)->height = REST_HEIGHT(work);
    if (work->scl.x > MAX_SCL) {
        work->scl.x = MAX_SCL;
    }
    if (work->scl.x < 0.0F) {
        work->scl.x = 0.0F;
    }
    if (work->scl.z > MAX_SCL) {
        work->scl.z = MAX_SCL;
    }
    if (work->scl.z < 0.0F) {
        work->scl.z = 0.0F;
    }
    object = &WEIGHT(task)->object;
    *object = weight_collision;
    object->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    object->scl[0] = SCALE(work->scl.x);
    object->scl[1] = 1.0F;
    object->scl[2] = SCALE(work->scl.z);
    object->ang[1] = work->ang.y;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y + WEIGHT(task)->height + BLOCK_HEIGHT;
    object->pos[2] = work->pos.z;
    radius = (SCALE(work->scl.x > work->scl.z ? work->scl.x : work->scl.z) *
              object->model->r) +
             CLASS_MARGIN;
    flags = WORLD_SOLID | WORLD_MOVING;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    collision_init(task, weight_shapes, WEIGHT_SHAPES, COLLISION_OBJECT);
    /* @bug memory: reads the collision record's shapes through null where
     * collision_init found no room. weight_step's fall tests the record for
     * null at 0x7c6 of the object's code. weight_init reads through the null
     * at 0x28a, and weight_step's end at 0xa90 */
#ifdef NONMATCHING
    /* this side repairs the tag above: where collision_init fails, the init
     * destroys the weight, and set.c makes it again on a later pass */
    if (work->collision == NULL) {
        task_destroy(task);
        return;
    }
#endif
    work->collision->shapes[BOTTOM_SHAPE].radius = HALF_X(work);
    work->collision->shapes[BOTTOM_SHAPE].size_z = HALF_Z(work);
    work->collision->shapes[BELOW_SHAPE].radius = HALF_X(work);
    work->collision->shapes[BELOW_SHAPE].size_z = HALF_Z(work);
    work->collision->shapes[BOTTOM_SHAPE].flags |= SHAPE_OFF;
    work->collision->shapes[BELOW_SHAPE].flags |= SHAPE_OFF;
    work->ang.x &= WEIGHT_TYPE_BITS | WEIGHT_NORMAL;
    work->ang.z &= WEIGHT_SWITCH_ID_BITS | WEIGHT_LONG_SHADOW;
}

static void weight_destroy(struct task *task)
{
    if (WEIGHT(task)->object.model != NULL) {
        world_remove(task, &WEIGHT(task)->object);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void weight_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    NJS_POINT3 pos;
    struct task *hit;
    int player;
    Float top;
    /* @hack spelling: bit, a local for 0x100, the value of both WORLD_ADDED
     * and WEIGHT_NORMAL, where each named at its use would do */
    Sint32 bit;
    /* @hack spelling: scl_x and scl_z, locals for the work's scl.x and scl.z,
     * where each read in place would do */
    Float scl_x;
    Float scl_z;
    /* @hack spelling: zero, a local set to 0.0F ahead of the time stop's
     * test, where 0.0F at each store would do */
    Float zero;

    if (placement_try_leave(task)) {
        return;
    }
    zero = 0.0F;
    if (!game_state.time_stopped) {
        switch (work->submode) {
        case MODE_WAIT:
            switch (TYPE(work)) {
            case WEIGHT_DOWN_ON_SONIC_SW:
                if (WEIGHT_SWITCH_IS_ON(work)) {
                    WEIGHT(task)->target_y = zero;
                    work->submode = MODE_RISE;
                    break;
                }
                /* fall through */
            case WEIGHT_DOWN_ON_SONIC:
                /* @bug memory: reads player 0's pos with no test for null. a
                 * null player_work[0] has weight_box_contains read through
                 * null. MOV.L at 0x47c of the object's code takes the pointer
                 * untested. MODE_RISE tests it, with TST at 0x5b6 */
                if (IN_COLUMN(work, 0) &&
                    (player_work[0]->flags & DROP_PLAYER_FLAGS)) {
                    WEIGHT(task)->target_y =
                        player_work[0]->pos.y - work->pos.y;
                    if (WEIGHT(task)->target_y > 0.0F) {
                        WEIGHT(task)->target_y = zero;
                    }
                    work->submode = MODE_FALL;
                } else if (player_work[1] != NULL && IN_COLUMN(work, 1) &&
                           (player_work[1]->flags & DROP_PLAYER_FLAGS)) {
                    WEIGHT(task)->target_y =
                        player_work[1]->pos.y - work->pos.y;
                    /* @hack spelling: the mode set in each arm, where one
                     * store after the if would do */
                    if (WEIGHT(task)->target_y > 0.0F) {
                        WEIGHT(task)->target_y = zero;
                        work->submode = MODE_FALL;
                    } else {
                        work->submode = MODE_FALL;
                    }
                }
                break;
            case WEIGHT_UPDOWN:
                WEIGHT(task)->target_y = zero;
                work->submode = MODE_FALL;
                break;
            case WEIGHT_UPDOWN_SW:
                if (!WEIGHT_SWITCH_IS_ON(work)) {
                    WEIGHT(task)->target_y = zero;
                    work->submode = MODE_FALL;
                }
                break;
            }
            /* @hack redundant: an empty if and else on the mode */
            if (work->submode == MODE_FALL) {
            } else {
            }
            break;
        case MODE_RISE:
            WEIGHT(task)->height += weight_rise_speed;
            if (!(TYPE(work) == WEIGHT_DOWN_ON_SONIC_SW &&
                  WEIGHT_SWITCH_IS_ON(work))) {
                if (!((TYPE(work) == WEIGHT_DOWN_ON_SONIC_SW ||
                       TYPE(work) == WEIGHT_DOWN_ON_SONIC) &&
                      PLAYER_IN_COLUMN(work))) {
                    if (WEIGHT(task)->height > REST_HEIGHT(work)) {
                        WEIGHT(task)->height = REST_HEIGHT(work);
                        work->submode = MODE_WAIT;
                    }
                } else {
                    work->submode = MODE_FALL;
                }
            } else if (WEIGHT(task)->height > REST_HEIGHT(work)) {
                WEIGHT(task)->height = REST_HEIGHT(work);
            }
            break;
        case MODE_FALL:
            WEIGHT(task)->height += SPEED(task);
            /* @hack spelling: the gravity negated and put first, where the
             * drag's product less the gravity would do */
            SPEED(task) = -weight_gravity + (SPEED(task) * weight_fall_drag);
            if (WEIGHT(task)->height < 0.0F) {
                WEIGHT(task)->height = zero;
                work->submode = MODE_LANDED;
                dust_short_rect(&work->pos, HALF_X(work) - DUST_INSET,
                                HALF_Z(work) - DUST_INSET, &work->ang,
                                weight_dust_scale, weight_dust_density,
                                weight_dust_speed);
                if (FELT(work, 0)) {
                    vibration_start(0, VIBRATION_POWER, VIBRATION_FREQ,
                                    VIBRATION_UNTIMED);
                }
                if (FELT(work, 1)) {
                    vibration_start(1, VIBRATION_POWER, VIBRATION_FREQ,
                                    VIBRATION_UNTIMED);
                }
                work->timer = 0;
                if (work->collision != NULL) {
                    work->collision->shapes[BOTTOM_SHAPE].flags |= SHAPE_OFF;
                    work->collision->shapes[BELOW_SHAPE].flags |= SHAPE_OFF;
                }
                sound_play_at(LAND_SOUND, NULL, 0, weight_land_volume,
                              &work->pos);
            } else if (work->collision != NULL) {
                work->collision->shapes[BOTTOM_SHAPE].flags &= ~SHAPE_OFF;
                work->collision->shapes[BELOW_SHAPE].flags &= ~SHAPE_OFF;
                work->collision->shapes[BOTTOM_SHAPE].center.y =
                    WEIGHT(task)->height + SHAPE_HALF_HEIGHT;
                work->collision->shapes[BELOW_SHAPE].center.y =
                    WEIGHT(task)->target_y - (SHAPE_HALF_HEIGHT + BELOW_GAP);
            }
            break;
        case MODE_LANDED:
            /* @hack spelling: the gravity negated and put first, where the
             * drag's product less the gravity and the spring's pull would do */
            SPEED(task) = -weight_gravity + (SPEED(task) * weight_bounce_drag) -
                          (WEIGHT(task)->height * weight_bounce_spring);
            WEIGHT(task)->height += SPEED(task);
            if (WEIGHT(task)->height < 0.0F) {
                WEIGHT(task)->height = zero;
                /* @hack redundant: the mode set to MODE_LANDED, which it
                 * holds already */
                work->submode = MODE_LANDED;
                SPEED(task) = _builtin_fabsf(SPEED(task)) * BOUNCE;
            }
            if (work->timer > weight_landed_steps) {
                if ((TYPE(work) == WEIGHT_DOWN_ON_SONIC_SW &&
                     WEIGHT_SWITCH_IS_ON(work)) ||
                    (TYPE(work) != WEIGHT_DOWN_ON_SONIC_SW &&
                     (TYPE(work) != WEIGHT_DOWN_ON_SONIC ||
                      !PLAYER_IN_COLUMN(work)))) {
                    SPEED(task) = zero;
                    WEIGHT(task)->height = zero;
                    work->submode = MODE_RISE;
                }
            } else {
                work->timer++;
            }
            break;
        /* @hack redundant: a default that does nothing */
        default:
            break;
        }
    } else if (work->collision != NULL) {
        work->collision->shapes[BOTTOM_SHAPE].flags |= SHAPE_OFF;
    }
    object = &WEIGHT(task)->object;
    top = work->pos.y + WEIGHT(task)->height + BLOCK_HEIGHT;
    TASK_FORCES_OF(task)->speed.y = top - object->pos[1];
    object->pos[1] = top;
    bit = 0x100;
    scl_x = work->scl.x;
    scl_z = work->scl.z;
    if (player_near((NJS_POINT3 *)object->pos,
                    (object->model->r * SCALE(scl_x > scl_z ? scl_x : scl_z)) +
                        NEAR_MARGIN)) {
        work->flags |= bit;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
    work->collision->shapes[TOP_SHAPE].center.y =
        WEIGHT(task)->height + BLOCK_HEIGHT;
    /* @hack spelling: the masked bit shifted down to bit 0 before the test,
     * where the masked bit tested in place would do */
    if (!((work->ang.x & bit) >> 8)) {
        while ((hit = collision_next_player_by_shape(task, BOTTOM_SHAPE)) !=
               NULL) {
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((player = player_slot_of(hit)) == -1) {
                continue;
            }
            if (weight_box_contains(&player_work[player]->pos, &work->pos,
                                    work->ang.y, HALF_X(work) + LIFT_REACH,
                                    LIFT_HALF_HEIGHT,
                                    HALF_Z(work) + LIFT_REACH)) {
                player_get_past(player, 1, &pos, NULL);
                pos.y = work->pos.y + WEIGHT(task)->height + BLOCK_HEIGHT;
                player_set_past(player, 0, &pos, NULL);
                player_work[player]->pos = pos;
            }
        }
    }
    collision_fit_record_radius(work->collision);
    collision_register(task);
}

static void weight_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack type: color an int, where the Uint32 that color_blend returns
     * would do */
    int color;
    Float half;
    /* @hack spelling: control, a local for DRAW_CONTROL_SHADOW that the
     * first call's argument sets, where the constant at each call would do */
    Uint32 control;

    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x,
                work->pos.y + WEIGHT(task)->height + BLOCK_HEIGHT, work->pos.z);
    njRotateY(NULL, work->ang.y);
    draw_control_set(control = DRAW_CONTROL_SHADOW);
    njSetTexture(&weight_handle_texlist);
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        /* @hack redundant: unused, a point nothing reads, which only sizes
         * the frame */
        NJS_POINT3 unused;

        /* @hack redundant: a call that leaves no code, but whose use SHC
         * counts as it assigns the draw's registers */
        njSetTexture(&weight_handle_texlist);
        (void)"<- TYPE";
        (void)"<- ANGLE Y";
        (void)"<- SWITCH ID";
        (void)"SHADOW:LONG";
        (void)"SHADOW:NORM";
        (void)"<- SCLX";
        (void)"<- UP HEIGHT";
        (void)"<- SCLZ";
    }
    if (weight_built) {
        njDirectDrawObject(weight_builds[HANDLE_BUILD].built);
    } else {
        njCnkSimpleDrawModel(&weight_handle_model);
    }
    draw_control_clear(control);
    njScale(NULL, SCALE(work->scl.x), 1.0F, SCALE(work->scl.z));
    if (!((TYPE(work) == WEIGHT_UPDOWN_SW ||
           TYPE(work) == WEIGHT_DOWN_ON_SONIC_SW) &&
          WEIGHT_SWITCH_IS_ON(work))) {
        color = color_blend(
            BOTTOM_DARK, LIGHT,
            (njSin(game_state.steps * weight_glow_step) * 0.5F) + 0.5F);
        njSetTexture(&weight_frame_texlist);
        model_draw_tinted(&weight_bottom_model, color);
        color = color_blend(FRAME_DARK, LIGHT,
                            blink_level(BLINK_PERIOD, BLINK_REST, BLINK_FADE));
        model_draw_tinted(&weight_frame_model, color);
    } else {
        njSetTexture(&weight_frame_texlist);
        model_draw_tinted(&weight_bottom_model, BOTTOM_DARK);
        model_draw_tinted(&weight_frame_model, FRAME_DARK);
    }
    draw_control_set(control);
    njSetTexture(&weight_body_texlist);
    njCnkSimpleDrawModel(&weight_body_model);
    njPopMatrix(1);
    draw_control_clear(control);
    half = WEIGHT(task)->height * 0.5F;
    if (half > SHADOW_RISE * 0.5F) {
        half = SHADOW_RISE * 0.5F;
    }
    njPushMatrix(NULL);
    if (work->ang.z & WEIGHT_LONG_SHADOW) {
        njTranslate(NULL, work->pos.x,
                    work->pos.y + half -
                        ((SHADOW_FOOT + LONG_SHADOW_DROP - SHADOW_HEAD) * 0.5F),
                    work->pos.z);
        njRotateY(NULL, work->ang.y);
        njScale(
            NULL, SCALE(work->scl.x) * (HALF_SIZE / SHADOW_HALF),
            (half + ((SHADOW_FOOT + LONG_SHADOW_DROP + SHADOW_HEAD) * 0.5F)) *
                (1.0F / SHADOW_HALF),
            SCALE(work->scl.z) * (HALF_SIZE / SHADOW_HALF));
    } else {
        njTranslate(NULL, work->pos.x,
                    work->pos.y + half - ((SHADOW_FOOT - SHADOW_HEAD) * 0.5F),
                    work->pos.z);
        njRotateY(NULL, work->ang.y);
        njScale(NULL, SCALE(work->scl.x) * (HALF_SIZE / SHADOW_HALF),
                (half + ((SHADOW_FOOT + SHADOW_HEAD) * 0.5F)) *
                    (1.0F / SHADOW_HALF),
                SCALE(work->scl.z) * (HALF_SIZE / SHADOW_HALF));
    }
    njCnkModDrawModel(&modifier_volume_box_model);
    njPopMatrix(1);
}
