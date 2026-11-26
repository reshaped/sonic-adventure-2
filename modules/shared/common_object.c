/* the common objects, which the stages' objects share */
#include <machine.h>
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/chunk.h"
#include "draw/trail.h"
#include "effect/particle.h"
#include "enemy/enemy.h"
#include "object/animal.h"
#include "object/fall.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/flash.h"
#include "shared/switch.h"
#include "shared/vertex_colors.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* nothing calls these, and no header declares them. they are external only
 * because SHC leaves out a static function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void body_free(struct body *body);
/* the first stacked task within STACK_FIND_REACH of the point STACK_HEIGHT
 * above pos, or null */
struct task *stack_find_above(NJS_POINT3 *pos);
/* make five lock-on faces, children of parent, and return the fifth. the
 * result is null where task_create_child failed for the fifth. each is a
 * WORK_ID_STOP_LOCKON, which stops a mech's lock-on. the face is the x-y plane
 * through parent's pos, turned about y by parent's ang.y. each step, a lock-on
 * face follows the start of player's aim, unk_254, across the face. it keeps
 * the point's x within range of 0, and its y from 0 to range. player must name
 * a slot that holds a player while the lock-on faces step */
struct task *lockon_faces_create(struct task *parent, int player, Float range);
/* return blink_level(period, rest, fade) as an opaque gray: black at 0, white
 * at 1 */
Uint32 blink_gray(int period, int rest, int fade);
void fun_common_object_3dcc(int value);
int fun_common_object_3dd2(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* what a step takes from spd.y in the air, and what it takes under BODY_SLOW */
#define BODY_GRAVITY 0.08F
#define BODY_SLOW_GRAVITY 0.016F
/* the share of spd that a BODY_SLOW body moves by in a step */
#define BODY_SLOW_SHARE 0.5F
/* each step multiplies spd by BODY_DRAG, and holds spd.y within
 * BODY_MAX_SPD_Y of 0. on a floor, a spd.y of BODY_MIN_BOUNCE or less
 * becomes 0 */
#define BODY_DRAG 0.997F
#define BODY_MAX_SPD_Y 2.45F
#define BODY_MIN_BOUNCE 0.24F
/* a body is still while each of spd's parts lies within BODY_STILL of 0 */
#define BODY_STILL 0.001F
/* the spd.y that body_step gives a still BODY_RESTS body whose work_flags
 * hold WORK_HELD_BY_PLAYER. the body is then no longer still, so the step
 * collides it */
#define BODY_WAKE_SPD_Y (-0.264F)
/* the share of ang.x's and ang.z's low 16 bits that a held body keeps each
 * step */
#define HELD_TILT_KEPT 0.9F
/* the radius of the ball a BODY_ROLLS body rolls as. ROLL_PER_UNIT is the
 * ball's turn for each unit it rolls */
#define ROLL_RADIUS 10.0F
#define ROLL_PER_UNIT (FULL_TURN / (2.0F * 3.14F * ROLL_RADIUS))

/* the share of a ceiling's or a floor's overlap that one collision undoes */
#define OVERLAP_UNDONE 0.3F
/* the share of spd that a stop at a ledge moves pos back by */
#define LEDGE_STEP_BACK 0.4F
/* the most a landing turns ang.x and ang.z toward the floor's tilt in a step */
#define TILT_STEP 0x80
/* the probe looks for walls along -x, +x, -z and +z. a surface is a wall where
 * the dot product of its normal and that direction lies below WALL_FACING.
 * under BODY_ANY_WALL, the product's magnitude need only pass -WALL_FACING. a
 * wall pushes pos along its normal's x and z by WALL_UNDONE of the overlap */
#define WALL_FACING (-0.5F)
#define WALL_UNDONE 0.35F

#define STACK_FIND_REACH 9.0F
#define STACK_DROP_ABOVE 11.0F
#define STACK_DROP_REACH 8.0F

/* a bit the solid object's tests mask the work's mode with, beside
 * OBJECT_NEAR_ONLY. the mode is a Sint8, and OBJECT_NEAR_ONLY is its sign bit.
 * so the mode, widened to an int, holds this bit wherever it holds
 * OBJECT_NEAR_ONLY */
#define OBJECT_NEAR_ONLY_WIDENED 0x8000

/* how far past its model's radius a solid object reaches */
#define SOLID_MARGIN 15.0F

#define FACE_PLAYER(work) ((work)->ang.z)
#define FACE_RANGE(work) ((work)->scl.x)
#define FACE_OFFSET_X(work) ((work)->scl.y)
#define FACE_OFFSET_Y(work) ((work)->scl.z)
/* in the submode of the lock-on faces' parent's work. while it is set, the
 * lock-on faces neither move nor register their collision */
#define FACE_OFF 0x80
/* a lock-on face stands FACE_STANDOFF off the face, on the side away from the
 * point it follows. four of the five stand FACE_SPACING off that point along x
 * and along y */
#define FACE_STANDOFF 15.0F
#define FACE_SPACING 15.0F

#define LOCKON_BOX_BACK 30.0F

#define WEAK_POINT_INDEX(work) ((work)->tick)
#define WEAK_POINT_PAIRED(work) ((work)->mode)
/* a hurt weak point leaves a puff of dust and a flash */
#define HURT_DUST_SCALE 6.0F
#define HURT_FLASH_SCALE 20.0F
#define HURT_SPARKS 3
#define HURT_FLASH_DEPTH_BIAS 30.0F

#define WEAK_POINT_IGNORES                                                     \
    (SHAPE_IGNORE_PLAYERS | SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_OBJECTS |      \
     SHAPE_IGNORE_PICKUPS | SHAPE_IGNORE_UNK_9)

#define BUILD_EVAL                                                             \
    (NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL | NJD_EVAL_BREAK)

/* probe the stage at pos plus body->probe_offset turned by ang */
/* @hack redundant: spd, which nothing reads */
static void body_probe(NJS_POINT3 *pos, NJS_VECTOR *spd, struct angles *ang,
                       struct body *body)
{
    struct surface_probe *probe = &body->probe;
    /* @hack redundant: unused, a local that only sizes the frame */
    Uint32 unused[3];
    NJS_VECTOR offset;

    offset = body->probe_offset;
    njPushMatrix(&_nj_unit_matrix_);
    njRotateZ(NULL, ang->z);
    njRotateX(NULL, ang->x);
    njRotateY(NULL, ang->y);
    njCalcPoint(NULL, &offset, &body->probe_turned);
    njPopMatrixEx();
    probe->pos.x = pos->x + body->probe_turned.x;
    probe->pos.y = pos->y + body->probe_turned.y;
    probe->pos.z = pos->z + body->probe_turned.z;
    if (body->flags & BODY_PROBE_FLAT) {
        if (surface_probe_sides_flat(probe)) {
            body->flags |= BODY_ON_FLAT;
        } else {
            body->flags &= ~BODY_ON_FLAT;
        }
    } else {
        surface_probe_sides(probe);
    }
}

/* @hack redundant: ang, which nothing reads */
static void body_hit_ceiling(NJS_POINT3 *pos, NJS_VECTOR *spd,
                             struct angles *ang, struct body *body)
{
    Float top = body->probe.pos.y + body->top;
    struct surface_side *ceiling = &body->probe.sides[SURFACE_POS_Y];

    body->flags &= ~BODY_HIT_CEILING;
    if (top > ceiling->pos && ceiling->normal.y < 0.0F) {
        spd->y = 0.0F;
        pos->y -= (top - ceiling->pos) * OVERLAP_UNDONE;
        body->flags |= BODY_HIT_CEILING;
    }
}

/* land body on the floor below its probe, or under BODY_LEDGE_STOP stop it at
 * a ledge */
static void body_hit_floor(NJS_POINT3 *pos, NJS_VECTOR *spd, struct angles *ang,
                           struct body *body)
{
    Float bottom = body->probe.pos.y + body->bottom;
    struct surface_side *floor = &body->probe.sides[SURFACE_NEG_Y];

    if ((body->flags & BODY_LEDGE_STOP) && !(body->flags & BODY_LEDGE_WAIT) &&
        (bottom - body->step_down > floor->pos || !floor->found)) {
        *pos = body->last_pos;
        body->flags |= BODY_LEDGE_WAIT | BODY_ON_FLOOR;
        pos->x -= spd->x * LEDGE_STEP_BACK;
        pos->z -= spd->z * LEDGE_STEP_BACK;
        spd->x = 0.0F;
        spd->y = 0.0F;
        spd->z = 0.0F;
        body->floor_normal.x = 0.0F;
        body->floor_normal.y = 1.0F;
        body->floor_normal.z = 0.0F;
    } else {
        body->flags &= ~BODY_ON_FLOOR;
        if (floor->pos > bottom && floor->normal.y > 0.0F) {
            spd->y *= -body->floor_bounce;
            spd->x *= body->friction;
            spd->z *= body->friction;
            pos->y += (floor->pos - bottom) * OVERLAP_UNDONE;
            if (body->flags & BODY_TILTS) {
                ang->x = angle_approach(ang->x, floor->ang_x, TILT_STEP);
                ang->z = angle_approach(ang->z, floor->ang_z, TILT_STEP);
            }
            body->floor_normal = floor->normal;
            body->flags |= BODY_ON_FLOOR;
            body->flags &= ~BODY_LEDGE_WAIT;
        } else {
            body->floor_normal.x = 0.0F;
            body->floor_normal.y = 1.0F;
            body->floor_normal.z = 0.0F;
        }
    }
}

/* collide body with the walls its probe found along -x, +x, -z and +z, in
 * that order. dist is how far the probe lies in front of a wall, along the
 * wall's normal.
 *
 * the probe has crossed a wall where dist lies between -radius and 0. the
 * nearest crossed wall moves pos to the probe's nearest point on it. it then
 * turns spd and ang.y, as wall_bounce and wall_turn say, whichever way spd
 * heads.
 *
 * a wall the probe hasn't crossed pushes pos out along its normal where dist is
 * radius or less. once a side finds a crossed wall, no later side pushes. a
 * pushing wall turns spd and ang.y the same way, but only where spd heads into
 * it */
static void body_hit_walls(NJS_POINT3 *pos, NJS_VECTOR *spd, struct angles *ang,
                           struct body *body)
{
    NJS_POINT3 at;
    NJS_PLANE plane;
    NJS_POINT3 best_cross;
    NJS_VECTOR best_normal;
    /* @hack redundant: unused, a local that only sizes the frame */
    Uint32 unused[3];
    struct surface_probe *probe = &body->probe;
    struct surface_side *hit;
    Float radius = body->radius;
    Float best;
    Float dist;
    int best_side;
    int facing;
    int found = 0;

    at = probe->pos;
    body->flags &= ~BODY_HIT_WALL;
    if (probe->sides[SURFACE_NEG_X].found) {
        NJS_VECTOR dir = {-1.0F, 0.0F, 0.0F};
        NJS_POINT3 cross;
        NJS_VECTOR push;
        NJS_VECTOR axis;
        NJS_VECTOR reflect;
        Float depth;

        hit = &probe->sides[SURFACE_NEG_X];
        if (body->flags & BODY_ANY_WALL) {
            facing = njAbs(njInnerProduct(&dir, &hit->normal)) > -WALL_FACING;
        } else {
            facing = njInnerProduct(&dir, &hit->normal) < WALL_FACING;
        }
        if (facing) {
            plane.px = hit->pos;
            plane.py = at.y;
            plane.pz = at.z;
            plane.vx = hit->normal.x;
            plane.vy = hit->normal.y;
            plane.vz = hit->normal.z;
            dist = point_plane_distance(&at, &plane, &cross);
            if (dist < 0.0F && njAbs(dist) < radius) {
                found = 1;
                best_cross = cross;
                best = njAbs(dist);
                best_normal = hit->normal;
                best_side = SURFACE_NEG_X;
            } else {
                depth = radius - dist;
                if (depth >= 0.0F) {
                    push.x = hit->normal.x;
                    push.y = 0.0F;
                    push.z = hit->normal.z;
                    push.x *= depth * WALL_UNDONE;
                    push.y *= depth * WALL_UNDONE;
                    push.z *= depth * WALL_UNDONE;
                    njAddVector(pos, &push);
                    body->flags |= BODY_HIT_WALL;
                    if (njInnerProduct(spd, &hit->normal) < 0.0F) {
                        reflect = hit->normal;
                        if (njUnitVector(&reflect) == 0.0F) {
                            reflect.x = 1.0F;
                            reflect.y = 0.0F;
                            reflect.z = 0.0F;
                        }
                        dist = -(njInnerProduct(spd, &reflect) *
                                 body->wall_bounce * 2.0F);
                        reflect.x *= dist;
                        reflect.y *= dist;
                        reflect.z *= dist;
                        njAddVector(spd, &reflect);
                        njOuterProduct(&reflect, spd, &axis);
                        if (axis.y > 0.0F) {
                            ang->y -= body->wall_turn;
                        } else {
                            ang->y += body->wall_turn;
                        }
                    }
                }
            }
        }
    }
    if (probe->sides[SURFACE_POS_X].found) {
        NJS_VECTOR dir = {1.0F, 0.0F, 0.0F};
        NJS_POINT3 cross;
        NJS_VECTOR push;
        NJS_VECTOR axis;
        NJS_VECTOR reflect;
        Float depth;

        hit = &probe->sides[SURFACE_POS_X];
        if (body->flags & BODY_ANY_WALL) {
            facing = njAbs(njInnerProduct(&dir, &hit->normal)) > -WALL_FACING;
        } else {
            facing = njInnerProduct(&dir, &hit->normal) < WALL_FACING;
        }
        if (facing) {
            plane.px = hit->pos;
            plane.py = at.y;
            plane.pz = at.z;
            plane.vx = hit->normal.x;
            plane.vy = hit->normal.y;
            plane.vz = hit->normal.z;
            dist = point_plane_distance(&at, &plane, &cross);
            /* @hack spelling: depth assigned inside the else's test, where a
             * statement of its own would do */
            if (dist < 0.0F && njAbs(dist) < radius) {
                if (!found || njAbs(dist) <= best) {
                    found = 1;
                    best_cross = cross;
                    best = njAbs(dist);
                    best_normal = hit->normal;
                    best_side = SURFACE_POS_X;
                }
            } else if (!found && (depth = radius - dist) >= 0.0F) {
                push.x = hit->normal.x;
                push.y = 0.0F;
                push.z = hit->normal.z;
                push.x *= depth * WALL_UNDONE;
                push.y *= depth * WALL_UNDONE;
                push.z *= depth * WALL_UNDONE;
                njAddVector(pos, &push);
                body->flags |= BODY_HIT_WALL;
                if (njInnerProduct(spd, &hit->normal) < 0.0F) {
                    reflect = hit->normal;
                    if (njUnitVector(&reflect) == 0.0F) {
                        reflect.x = 1.0F;
                        reflect.y = 0.0F;
                        reflect.z = 0.0F;
                    }
                    dist = -(njInnerProduct(spd, &reflect) * body->wall_bounce *
                             2.0F);
                    reflect.x *= dist;
                    reflect.y *= dist;
                    reflect.z *= dist;
                    njAddVector(spd, &reflect);
                    njOuterProduct(&reflect, spd, &axis);
                    if (axis.y > 0.0F) {
                        ang->y -= body->wall_turn;
                    } else {
                        ang->y += body->wall_turn;
                    }
                }
            }
        }
    }
    if (probe->sides[SURFACE_NEG_Z].found) {
        NJS_VECTOR dir = {0.0F, 0.0F, -1.0F};
        NJS_POINT3 cross;
        NJS_VECTOR push;
        NJS_VECTOR axis;
        NJS_VECTOR reflect;
        Float depth;

        hit = &probe->sides[SURFACE_NEG_Z];
        if (body->flags & BODY_ANY_WALL) {
            facing = njAbs(njInnerProduct(&dir, &hit->normal)) > -WALL_FACING;
        } else {
            facing = njInnerProduct(&dir, &hit->normal) < WALL_FACING;
        }
        if (facing) {
            plane.px = at.x;
            plane.py = at.y;
            plane.pz = hit->pos;
            plane.vx = hit->normal.x;
            plane.vy = hit->normal.y;
            plane.vz = hit->normal.z;
            dist = point_plane_distance(&at, &plane, &cross);
            /* @hack spelling: depth assigned inside the else's test, where a
             * statement of its own would do */
            if (dist < 0.0F && njAbs(dist) < radius) {
                if (!found || njAbs(dist) <= best) {
                    found = 1;
                    best_cross = cross;
                    best = njAbs(dist);
                    best_normal = hit->normal;
                    best_side = SURFACE_NEG_Z;
                }
            } else if (!found && (depth = radius - dist) >= 0.0F) {
                push.x = hit->normal.x;
                push.y = 0.0F;
                push.z = hit->normal.z;
                push.x *= depth * WALL_UNDONE;
                push.y *= depth * WALL_UNDONE;
                push.z *= depth * WALL_UNDONE;
                njAddVector(pos, &push);
                body->flags |= BODY_HIT_WALL;
                if (njInnerProduct(spd, &hit->normal) < 0.0F) {
                    reflect = hit->normal;
                    if (njUnitVector(&reflect) == 0.0F) {
                        reflect.x = 1.0F;
                        reflect.y = 0.0F;
                        reflect.z = 0.0F;
                    }
                    dist = -(njInnerProduct(spd, &reflect) * body->wall_bounce *
                             2.0F);
                    reflect.x *= dist;
                    reflect.y *= dist;
                    reflect.z *= dist;
                    njAddVector(spd, &reflect);
                    njOuterProduct(&reflect, spd, &axis);
                    if (axis.y > 0.0F) {
                        ang->y -= body->wall_turn;
                    } else {
                        ang->y += body->wall_turn;
                    }
                }
            }
        }
    }
    if (probe->sides[SURFACE_POS_Z].found) {
        NJS_VECTOR dir = {0.0F, 0.0F, 1.0F};
        NJS_POINT3 cross;
        NJS_VECTOR push;
        NJS_VECTOR axis;
        NJS_VECTOR reflect;
        Float depth;

        hit = &probe->sides[SURFACE_POS_Z];
        if (body->flags & BODY_ANY_WALL) {
            facing = njAbs(njInnerProduct(&dir, &hit->normal)) > -WALL_FACING;
        } else {
            facing = njInnerProduct(&dir, &hit->normal) < WALL_FACING;
        }
        if (facing) {
            plane.px = at.x;
            plane.py = at.y;
            plane.pz = hit->pos;
            plane.vx = hit->normal.x;
            plane.vy = hit->normal.y;
            plane.vz = hit->normal.z;
            dist = point_plane_distance(&at, &plane, &cross);
            /* @hack spelling: depth assigned inside the else's test, where a
             * statement of its own would do */
            if (dist < 0.0F && njAbs(dist) < radius) {
                if (!found || njAbs(dist) <= best) {
                    found = 1;
                    best_cross = cross;
                    best = njAbs(dist);
                    best_normal = hit->normal;
                    best_side = SURFACE_POS_Z;
                }
            } else if (!found && (depth = radius - dist) >= 0.0F) {
                push.x = hit->normal.x;
                push.y = 0.0F;
                push.z = hit->normal.z;
                push.x *= depth * WALL_UNDONE;
                push.y *= depth * WALL_UNDONE;
                push.z *= depth * WALL_UNDONE;
                njAddVector(pos, &push);
                body->flags |= BODY_HIT_WALL;
                if (njInnerProduct(spd, &hit->normal) < 0.0F) {
                    reflect = hit->normal;
                    if (njUnitVector(&reflect) == 0.0F) {
                        reflect.x = 1.0F;
                        reflect.y = 0.0F;
                        reflect.z = 0.0F;
                    }
                    dist = -(njInnerProduct(spd, &reflect) * body->wall_bounce *
                             2.0F);
                    reflect.x *= dist;
                    reflect.y *= dist;
                    reflect.z *= dist;
                    njAddVector(spd, &reflect);
                    njOuterProduct(&reflect, spd, &axis);
                    if (axis.y > 0.0F) {
                        ang->y -= body->wall_turn;
                    } else {
                        ang->y += body->wall_turn;
                    }
                }
            }
        }
    }
    if (found) {
        NJS_VECTOR reflect;
        NJS_VECTOR axis;

        /* @bug logic: puts pos, not the probe, at the probe's nearest point on
         * the wall, so the probe ends probe_turned past that point. the floor,
         * the ceiling and the pushing walls move pos by a distance, which the
         * offset doesn't change. the store is at 0xc72 of the object's code */
        *pos = best_cross;
        body->flags |= BODY_HIT_WALL;
        reflect = best_normal;
        if (njUnitVector(&reflect) == 0.0F) {
            switch (best_side) {
            case SURFACE_NEG_X:
                reflect.x = 1.0F;
                reflect.z = 0.0F;
                break;
            case SURFACE_POS_X:
                reflect.x = -1.0F;
                reflect.z = 0.0F;
                break;
            case SURFACE_NEG_Z:
                reflect.x = 0.0F;
                reflect.z = 1.0F;
                break;
            case SURFACE_POS_Z:
                reflect.x = 0.0F;
                reflect.z = -1.0F;
                break;
            }
            reflect.y = 0.0F;
        }
        dist = -(njInnerProduct(spd, &reflect) * body->wall_bounce * 2.0F);
        reflect.x *= dist;
        reflect.y *= dist;
        reflect.z *= dist;
        njAddVector(spd, &reflect);
        njOuterProduct(&reflect, spd, &axis);
        if (axis.y > 0.0F) {
            ang->y -= body->wall_turn;
        } else {
            ang->y += body->wall_turn;
        }
    }
}

static void body_collide(NJS_POINT3 *pos, NJS_VECTOR *spd, struct angles *ang,
                         struct body *body)
{
    body_probe(pos, spd, ang, body);
    body_hit_ceiling(pos, spd, ang, body);
    body_hit_floor(pos, spd, ang, body);
    body_hit_walls(pos, spd, ang, body);
    body->last_pos = *pos;
}

void body_free(struct body *body)
{
    syFree(body);
}

/* @hack redundant: spd and ang, which nothing reads */
static struct body *body_alloc(NJS_POINT3 *pos, NJS_VECTOR *spd,
                               struct angles *ang)
{
    struct body *body = syCalloc(1, sizeof(struct body));

    if (body == NULL) {
        return NULL;
    }
    body->radius = 10.0F;
    body->probe_offset.y = 5.0F;
    body->top = 15.0F;
    body->bottom = -5.0F;
    body->step_down = 50.0F;
    body->floor_bounce = 0.7F;
    body->friction = 0.7F;
    body->unk_ac = 0.1F;
    body->wall_bounce = 0.8F;
    body->wall_turn = 0x400;
    body->last_pos = *pos;
    body->floor_normal.x = 0.0F;
    body->floor_normal.y = 1.0F;
    body->floor_normal.z = 0.0F;
    body->flags |= BODY_LEDGE_WAIT | BODY_UNK_400;
    body->flags &= ~BODY_PROBE_FLAT;
    njUnitMatrix(&body->rolled);
    njUnitMatrix(&body->roll);
    return body;
}

void body_init(struct task *task)
{
    struct task_motion *motion = task->motion.p;
    struct task_work *work = task->work;

    fall_set_kind(work, motion, FALL_KIND_NORMAL);
    task->force.p = body_alloc(&work->pos, &motion->spd, &work->ang);
}

void body_destroy(struct task *task)
{
    /* @hack spelling: a block with a local of its own, owner, set to task,
     * where syFree(task->force) and task->force = NULL would do */
    {
        struct task *owner = task;

        syFree(owner->force.p);
        owner->force.p = NULL;
    }
}

/* the y axis, a tenth long. on a floor, a BODY_ROLLS body's step turns it by
 * rolled, then by ang.y. the step then takes the x and z of the turned axis's
 * upper end from spd */
static NJS_VECTOR up_tenth = {0.0F, 0.1F, 0.0F};

void body_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task_motion *motion = task->motion.p;
    int pushed = 0;
    Angle heading;
    Angle roll;
    NJS_VECTOR lean;
    Float speed;
    Sint16 *low;
    Float value;

    if (!(work->flags & WORK_HELD_BY_PLAYER)) {
        if (work->collision == NULL || collision_pushed(task)) {
            pushed = 1;
        }
        if ((BODY(task)->flags & BODY_UNFROZEN) || !game_state.time_stopped) {
            if (!(BODY(task)->flags & BODY_NO_GRAVITY)) {
                if (BODY(task)->flags & BODY_ON_FLOOR) {
                    if (BODY(task)->flags & BODY_ROLLS) {
                        njPushMatrixEx();
                        njUnitMatrix(NULL);
                        njRotateY(NULL, work->ang.y);
                        njMultiMatrix(NULL, &BODY(task)->rolled);
                        njCalcVector(NULL, &up_tenth, &lean);
                        njPopMatrixEx();
                        if (lean.y > 0.0F) {
                            motion->spd.x -= lean.x;
                            motion->spd.z -= lean.z;
                        } else {
                            motion->spd.x += lean.x;
                            motion->spd.z += lean.z;
                        }
                        heading = (Angle)(atan2f(motion->spd.x, motion->spd.z) *
                                          ANGLE_PER_RADIAN) -
                                  work->ang.y;
                        roll = (Angle)(njSqrt((motion->spd.x * motion->spd.x) +
                                              (motion->spd.z * motion->spd.z)) *
                                       ROLL_PER_UNIT);
                        njPushMatrixEx();
                        njUnitMatrix(NULL);
                        njRotateY(NULL, heading);
                        njRotateX(NULL, roll);
                        njRotateY(NULL, -heading);
                        njGetMatrix(&BODY(task)->roll);
                        njPopMatrixEx();
                    }
                    motion->spd.y = njAbs(motion->spd.y);
                    if (motion->spd.y <= BODY_MIN_BOUNCE) {
                        motion->spd.y = 0.0F;
                    }
                } else if (!(BODY(task)->flags & BODY_RESTS) ||
                           njAbs(motion->spd.x) >= BODY_STILL ||
                           njAbs(motion->spd.y) >= BODY_STILL ||
                           njAbs(motion->spd.z) >= BODY_STILL || pushed) {
                    if (BODY(task)->flags & BODY_SLOW) {
                        motion->spd.y -= BODY_SLOW_GRAVITY;
                    } else {
                        motion->spd.y -= BODY_GRAVITY;
                    }
                }
            }
            motion->spd.x *= BODY_DRAG;
            motion->spd.y *= BODY_DRAG;
            motion->spd.z *= BODY_DRAG;
            if ((BODY(task)->flags & BODY_RESTS) &&
                (BODY(task)->work_flags & WORK_HELD_BY_PLAYER) &&
                njAbs(motion->spd.x) < BODY_STILL &&
                njAbs(motion->spd.y) < BODY_STILL &&
                njAbs(motion->spd.z) < BODY_STILL) {
                motion->spd.y = BODY_WAKE_SPD_Y;
            }
            speed = njAbs(motion->spd.y);
            if (speed > BODY_MAX_SPD_Y) {
                speed = BODY_MAX_SPD_Y / speed;
                motion->spd.y *= speed;
            }
            if (BODY(task)->flags & BODY_SLOW) {
                work->pos.x += motion->spd.x * BODY_SLOW_SHARE;
                work->pos.y += motion->spd.y * BODY_SLOW_SHARE;
                work->pos.z += motion->spd.z * BODY_SLOW_SHARE;
            } else {
                work->pos.x += motion->spd.x;
                work->pos.y += motion->spd.y;
                work->pos.z += motion->spd.z;
            }
            if (BODY(task)->flags & BODY_ROLLS) {
                njMultiMatrix(&BODY(task)->rolled, &BODY(task)->roll);
            }
        }
        if (!(BODY(task)->flags & BODY_RESTS) ||
            njAbs(motion->spd.x) >= BODY_STILL ||
            njAbs(motion->spd.y) >= BODY_STILL ||
            njAbs(motion->spd.z) >= BODY_STILL || pushed) {
            body_collide(&work->pos, &motion->spd, &work->ang, BODY(task));
        }
        collision_register(task);
    } else {
        if (game_state.time_stopped) {
            BODY(task)->flags |= BODY_UNFROZEN;
        }
        motion->spd.x = 0.0F;
        motion->spd.y = 0.0F;
        motion->spd.z = 0.0F;
        /* @hack type: low, a Sint16 pointer to an angle's low 16 bits, and
         * value, a Float that scales them, where masks on the whole angle
         * would do */
        low = (Sint16 *)&work->ang.x;
        value = *low;
        value *= HELD_TILT_KEPT;
        *low = value;
        low = (Sint16 *)&work->ang.z;
        value = *low;
        value *= HELD_TILT_KEPT;
        *low = value;
        BODY(task)->flags &= ~BODY_ON_FLOOR;
        njUnitMatrix(&BODY(task)->rolled);
        njUnitMatrix(&BODY(task)->roll);
    }
    if (!game_state.time_stopped) {
        BODY(task)->flags &= ~BODY_UNFROZEN;
    }
    BODY(task)->work_flags = work->flags;
    if (pushed) {
        BODY(task)->work_flags |= WORK_HELD_BY_PLAYER;
    }
}

struct task **stack_tasks = NULL;

int stack_add(struct task *task)
{
    int i;

    if (stack_tasks == NULL) {
        stack_tasks = syCalloc(STACK_TASKS, sizeof(struct task *));
    }
    if (stack_tasks != NULL) {
        for (i = 0; i < STACK_TASKS; i++) {
            if (stack_tasks[i] == NULL) {
                stack_tasks[i] = task;
                if (task->placement != NULL) {
                    /* @hack spelling: lifts, a local for the state, where the
                     * call inside the product would do */
                    Float lifts = placement_get_state(task);

                    task->work->pos.y -= lifts * STACK_HEIGHT;
                }
                return 1;
            }
        }
    }
    return 0;
}

void stack_remove(struct task *task)
{
    int i;
    int empty;

    if (stack_tasks != NULL) {
        empty = 0;
        for (i = 0; i < STACK_TASKS; i++) {
            if (stack_tasks[i] == task) {
                stack_tasks[i] = NULL;
            }
            if (stack_tasks[i] == NULL) {
                empty++;
            }
        }
        if (empty == STACK_TASKS) {
            syFree(stack_tasks);
            stack_tasks = NULL;
        }
    }
}

struct task *stack_find_above(NJS_POINT3 *pos)
{
    NJS_POINT3 above;
    int i;

    if (stack_tasks == NULL) {
        return NULL;
    }
    above.x = pos->x;
    above.y = pos->y + STACK_HEIGHT;
    above.z = pos->z;
    for (i = 0; i < STACK_TASKS; i++) {
        if (stack_tasks[i] != NULL &&
            njDistanceP2P(&above, &stack_tasks[i]->work->pos) <
                STACK_FIND_REACH) {
            return stack_tasks[i];
        }
    }
    return NULL;
}

int stack_drop(NJS_POINT3 *pos)
{
    NJS_POINT3 floor;
    struct task *task;
    Float dx;
    Float dz;
    Float dist2;
    int i;

    if (stack_tasks == NULL) {
        return 0;
    }
    floor.x = pos->x;
    floor.y = pos->y + STACK_DROP_ABOVE;
    floor.z = pos->z;
    for (i = 0; i < STACK_TASKS; i++) {
        if (stack_tasks[i] != NULL) {
            task = stack_tasks[i];
            if (task->work->pos.y > floor.y) {
                dx = task->work->pos.x - floor.x;
                dz = task->work->pos.z - floor.z;
                dist2 = (dx * dx) + (dz * dz);
                if (dist2 < STACK_DROP_REACH * STACK_DROP_REACH) {
                    task->work->scl.y += STACK_HEIGHT;
                    if (task->placement != NULL) {
                        placement_set_state(task,
                                            placement_get_state(task) + 1);
                    }
                }
            }
        }
    }
    return 0;
}

/* null the model and the texture list, which aren't the task's, so that
 * task_destroy doesn't free them */
static void model_object_destroy(struct task *task)
{
    task->force.p = NULL;
    task->motion.p = NULL;
}

static void model_object_step(struct task *task)
{
    struct task_work *work = task->work;

    if ((work->mode & OBJECT_MAY_LEAVE) && placement_try_leave(task)) {
        return;
    }
    if (work->timer == 0 && work->collision != NULL) {
        collision_register(task);
    }
}

static void model_object_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(task->force.p);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    if (work->mode & OBJECT_TURNED) {
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
    }
    njRotateY(NULL, work->ang.y);
    if (work->mode & OBJECT_AS_LAND) {
        draw_control_set(DRAW_CONTROL_SHADOW);
        njCnkEasyDrawModel(task->motion.p);
        draw_control_clear(DRAW_CONTROL_SHADOW);
    } else {
        njCnkEasyDrawModel(task->motion.p);
    }
    njPopMatrixEx();
}

/* the draw of a model object whose mode has no flag but OBJECT_MAY_LEAVE */
static void model_object_draw_upright(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(task->force.p);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(task->motion.p);
    njPopMatrixEx();
}

int model_object_init(struct task *task, NJS_TEXLIST *texlist,
                      NJS_CNK_MODEL *model, struct collision_shape *shapes,
                      int count, Sint16 mode)
{
    struct task_work *work = task->work;

    if ((mode & OBJECT_MAY_LEAVE) && placement_try_leave(task)) {
        return 0;
    }
    work->mode = mode;
    if (texlist != NULL && model != NULL) {
        if (mode & ~OBJECT_MAY_LEAVE) {
            task->draw = model_object_draw;
        } else {
            task->draw = model_object_draw_upright;
        }
    }
    task->motion.p = model;
    task->force.p = texlist;
    task->step = model_object_step;
    task->destructor = model_object_destroy;
    if (shapes != NULL) {
        collision_init(task, shapes, count, COLLISION_OBJECT);
    }
    return 1;
}

struct solid_object {
    /* the copy of solid_object_init's object, placed as the work is, that
     * world_add puts in the collision world */
    NJS_OBJECT object;
    NJS_POINT3 center; /* the center of object's model, in world space */
    NJS_TEXLIST *texlist;
    NJS_CNK_MODEL *model;
    Uint32 world_flags; /* the flags world_add takes object under */
};

#define SOLID(task) ((struct solid_object *)(task)->extra.p)

/* the draw of a solid object whose mode is 0 */
static void solid_object_draw_upright(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(SOLID(task)->texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(SOLID(task)->model);
    njPopMatrixEx();
}

static void solid_object_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(SOLID(task)->texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    if (work->mode & OBJECT_TURNED) {
        njRotateZ(NULL, work->ang.z);
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, work->ang.x);
    } else {
        njRotateY(NULL, work->ang.y);
    }
    if (work->mode & OBJECT_AS_LAND) {
        draw_control_set(DRAW_CONTROL_SHADOW);
        njCnkEasyDrawModel(SOLID(task)->model);
        draw_control_clear(DRAW_CONTROL_SHADOW);
    } else {
        njCnkEasyDrawModel(SOLID(task)->model);
    }
    njPopMatrixEx();
}

static void solid_object_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (SOLID(task) != NULL) {
        /* @bug logic: takes the copy out of the world even where a step took
         * it out already. world_remove then finds nothing, and does nothing.
         * the test of the mode always holds, since the mode has
         * OBJECT_NEAR_ONLY_WIDENED wherever it has OBJECT_NEAR_ONLY. the test
         * is at 0x1aa6 of the object's code */
        if (SOLID(task)->object.model != NULL &&
            (work->mode & (OBJECT_NEAR_ONLY | OBJECT_NEAR_ONLY_WIDENED)) !=
                OBJECT_NEAR_ONLY) {
            world_remove(task, &SOLID(task)->object);
        }
        syFree(SOLID(task));
        task->extra.p = NULL;
    }
}

static void solid_object_step(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: a read of the work's flags that nothing uses */
    Sint16 flags = work->flags;

    if ((work->mode & OBJECT_MAY_LEAVE) && placement_try_leave(task)) {
        return;
    }
    if (work->timer == 0 && work->collision != NULL) {
        collision_register(task);
    }
    if (SOLID(task) == NULL || SOLID(task)->object.model == NULL) {
        return;
    }
    if (work->timer == 0 &&
        player_near(&SOLID(task)->center,
                    SOLID(task)->object.model->r + SOLID_MARGIN)) {
        /* @bug logic: never puts the copy back in the world once a step took
         * it out. the test of the mode never holds, since the mode has
         * OBJECT_NEAR_ONLY_WIDENED wherever it has OBJECT_NEAR_ONLY. the test
         * is at 0x1b86 of the object's code */
        if ((work->mode & (OBJECT_NEAR_ONLY | OBJECT_NEAR_ONLY_WIDENED)) ==
            OBJECT_NEAR_ONLY) {
            world_add(SOLID(task)->world_flags, task, &SOLID(task)->object);
        }
        work->flags |= WORLD_ADDED;
    } else {
        if ((work->mode & (OBJECT_NEAR_ONLY | OBJECT_NEAR_ONLY_WIDENED)) ==
            (OBJECT_NEAR_ONLY | OBJECT_NEAR_ONLY_WIDENED)) {
            world_remove(task, &SOLID(task)->object);
        }
        /* @hack spelling: the mask written out, where &= would do */
        work->flags = work->flags & ~WORLD_ADDED;
    }
}

int solid_object_init(struct task *task, NJS_TEXLIST *texlist,
                      NJS_CNK_MODEL *model, struct collision_shape *shapes,
                      int count, NJS_OBJECT *object, Sint16 mode)
{
    struct task_work *work = task->work;
    struct solid_object *solid;
    Float radius;
    Uint32 flags;

    if ((mode & OBJECT_MAY_LEAVE) && placement_try_leave(task)) {
        return 0;
    }
    task->extra.p = syCalloc(1, sizeof(struct solid_object));
    if (task->extra.p == NULL) {
        return 0;
    }
    solid = task->extra.p;
    solid->object = *object;
    solid->object.evalflags =
        (solid->object.evalflags & ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG)) |
        NJD_EVAL_BREAK;
    solid->object.child = NULL;
    solid->object.sibling = NULL;
    solid->object.ang[1] = work->ang.y;
    if (mode & OBJECT_TURNED) {
        solid->object.ang[2] = work->ang.z;
        solid->object.ang[0] = work->ang.x;
    } else {
        solid->object.ang[2] = 0;
        solid->object.ang[0] = 0;
    }
    solid->object.pos[0] = work->pos.x;
    solid->object.pos[1] = work->pos.y;
    solid->object.pos[2] = work->pos.z;
    /* @bug memory: reads the radius of object's model through null where
     * object has no model. solid_object_step and solid_object_destroy test the
     * copy's model for null, at 0x1b58 and 0x1a98 of the object's code. the
     * radius is read by solid_object_init at 0x1c92 and by
     * solid_object_init_flags at 0x1f06 */
    radius = solid->object.model->r + SOLID_MARGIN;
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
    /* @hack spelling: task->extra read again from here on, where solid would
     * do */
    SOLID(task)->world_flags = flags;
    if (!(work->mode & OBJECT_NEAR_ONLY)) {
        /* @hack type: solid cast to its first member's type, where
         * &solid->object would do */
        world_add(flags, task, (NJS_OBJECT *)solid);
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx((NJS_VECTOR *)SOLID(task)->object.pos);
    if (mode & OBJECT_TURNED) {
        njRotateZ(NULL, SOLID(task)->object.ang[2]);
        njRotateY(NULL, SOLID(task)->object.ang[1]);
        njRotateX(NULL, SOLID(task)->object.ang[0]);
    } else {
        njRotateY(NULL, SOLID(task)->object.ang[1]);
    }
    njCalcPoint(NULL, &SOLID(task)->object.model->center, &SOLID(task)->center);
    njPopMatrixEx();
    work->mode = mode;
    if (texlist != NULL && model != NULL) {
        if (mode) {
            task->draw = solid_object_draw;
        } else {
            task->draw = solid_object_draw_upright;
        }
    }
    SOLID(task)->model = model;
    SOLID(task)->texlist = texlist;
    task->step = solid_object_step;
    task->destructor = solid_object_destroy;
    if (shapes != NULL) {
        collision_init(task, shapes, count, COLLISION_OBJECT);
    }
    return 1;
}

int solid_object_init_flags(struct task *task, NJS_TEXLIST *texlist,
                            NJS_CNK_MODEL *model,
                            struct collision_shape *shapes, int count,
                            NJS_OBJECT *object, Sint16 mode, Uint32 world_flags)
{
    struct task_work *work = task->work;
    struct solid_object *solid;
    Float radius;
    Uint32 flags;

    if ((mode & OBJECT_MAY_LEAVE) && placement_try_leave(task)) {
        return 0;
    }
    task->extra.p = syCalloc(1, sizeof(struct solid_object));
    if (task->extra.p == NULL) {
        return 0;
    }
    solid = task->extra.p;
    solid->object = *object;
    solid->object.evalflags =
        (solid->object.evalflags & ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG)) |
        NJD_EVAL_BREAK;
    solid->object.child = NULL;
    solid->object.sibling = NULL;
    solid->object.ang[1] = work->ang.y;
    if (mode & OBJECT_TURNED) {
        solid->object.ang[2] = work->ang.z;
        solid->object.ang[0] = work->ang.x;
    } else {
        solid->object.ang[2] = 0;
        solid->object.ang[0] = 0;
    }
    solid->object.pos[0] = work->pos.x;
    solid->object.pos[1] = work->pos.y;
    solid->object.pos[2] = work->pos.z;
    radius = solid->object.model->r + SOLID_MARGIN;
    flags = world_flags | WORLD_SOLID;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    /* @hack spelling: task->extra read again from here on, where solid would
     * do */
    SOLID(task)->world_flags = flags;
    if (!(work->mode & OBJECT_NEAR_ONLY)) {
        /* @hack type: solid cast to its first member's type, where
         * &solid->object would do */
        world_add(flags, task, (NJS_OBJECT *)solid);
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx((NJS_VECTOR *)SOLID(task)->object.pos);
    if (mode & OBJECT_TURNED) {
        njRotateZ(NULL, SOLID(task)->object.ang[2]);
        njRotateY(NULL, SOLID(task)->object.ang[1]);
        njRotateX(NULL, SOLID(task)->object.ang[0]);
    } else {
        njRotateY(NULL, SOLID(task)->object.ang[1]);
    }
    njCalcPoint(NULL, &SOLID(task)->object.model->center, &SOLID(task)->center);
    njPopMatrixEx();
    work->mode = mode;
    if (texlist != NULL && model != NULL) {
        if (mode) {
            task->draw = solid_object_draw;
        } else {
            task->draw = solid_object_draw_upright;
        }
    }
    SOLID(task)->model = model;
    SOLID(task)->texlist = texlist;
    task->step = solid_object_step;
    task->destructor = solid_object_destroy;
    if (shapes != NULL) {
        collision_init(task, shapes, count, COLLISION_OBJECT);
    }
    return 1;
}

struct task *solid_object_create(struct task *parent, NJS_TEXLIST *texlist,
                                 NJS_CNK_MODEL *model,
                                 struct collision_shape *shapes, int count,
                                 NJS_OBJECT *object, Sint16 mode)
{
    struct task *task;

    task = task_create_child(TASK_WORK | TASK_FORCE, solid_object_step, parent);
    mode &= ~OBJECT_MAY_LEAVE;
    if (task != NULL) {
        task->work->pos = parent->work->pos;
        task->work->ang = parent->work->ang;
        task->work->scl = parent->work->scl;
        solid_object_init(task, texlist, model, shapes, count, object, mode);
        return task;
    }
    return NULL;
}

struct task *solid_object_create_flags(struct task *parent,
                                       NJS_TEXLIST *texlist,
                                       NJS_CNK_MODEL *model,
                                       struct collision_shape *shapes,
                                       int count, NJS_OBJECT *object,
                                       Sint16 mode, Uint32 world_flags)
{
    struct task *task;

    task = task_create_child(TASK_WORK | TASK_FORCE, solid_object_step, parent);
    mode &= ~OBJECT_MAY_LEAVE;
    if (task != NULL) {
        task->work->pos = parent->work->pos;
        task->work->ang = parent->work->ang;
        task->work->scl = parent->work->scl;
        solid_object_init_flags(task, texlist, model, shapes, count, object,
                                mode, world_flags);
        return task;
    }
    return NULL;
}

struct task *solid_object_create_at(struct task *parent, NJS_POINT3 *pos,
                                    struct angles *ang, NJS_TEXLIST *texlist,
                                    NJS_CNK_MODEL *model,
                                    struct collision_shape *shapes, int count,
                                    NJS_OBJECT *object, Sint16 mode,
                                    Uint32 world_flags)
{
    struct task *task;

    task = task_create_child(TASK_WORK | TASK_FORCE, solid_object_step, parent);
    mode &= ~OBJECT_MAY_LEAVE;
    if (task != NULL) {
        task->work->pos = *pos;
        task->work->ang = *ang;
        task->work->scl = parent->work->scl;
        solid_object_init_flags(task, texlist, model, shapes, count, object,
                                mode, world_flags);
        return task;
    }
    return NULL;
}

struct task *model_object_create(struct task *parent, NJS_TEXLIST *texlist,
                                 NJS_CNK_MODEL *model,
                                 struct collision_shape *shapes, int count,
                                 Sint16 mode)
{
    struct task *task;

    task = task_create_child(TASK_WORK, model_object_step, parent);
    mode &= ~OBJECT_MAY_LEAVE;
    if (task != NULL) {
        model_object_init(task, texlist, model, shapes, count, mode);
        return task;
    }
    return NULL;
}

static void collider_destroy(struct task *task)
{
    enemy_destroy(task);
}

static void lockon_face_step(struct task *task)
{
    struct task_work *parent_work = task->parent->work;
    struct task_work *work = task->work;
    NJS_POINT3 point;

    if (parent_work->submode & FACE_OFF) {
        return;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, -parent_work->ang.y);
    njTranslate(NULL, -parent_work->pos.x, -parent_work->pos.y,
                -parent_work->pos.z);
    njCalcPoint(NULL, &player_records[FACE_PLAYER(work)]->unk_254, &point);
    if (point.z >= 0.0F) {
        point.z = -FACE_STANDOFF;
    } else {
        point.z = FACE_STANDOFF;
    }
    if (point.x > FACE_RANGE(work)) {
        point.x = FACE_RANGE(work);
    }
    if (-FACE_RANGE(work) > point.x) {
        point.x = -FACE_RANGE(work);
    }
    if (point.y < 0.0F) {
        point.y = 0.0F;
    }
    if (point.y > FACE_RANGE(work)) {
        point.y = FACE_RANGE(work);
    }
    point.x += FACE_OFFSET_X(work);
    point.y += FACE_OFFSET_Y(work);
    njUnitMatrix(NULL);
    njTranslateEx(&parent_work->pos);
    njRotateY(NULL, parent_work->ang.y);
    njCalcPoint(NULL, &point, &work->pos);
    njPopMatrixEx();
    if (!work->mode) {
        collision_register(task);
    }
}

static void lockon_box_step(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_POINT3 unused;

    if (work->mode == 0) {
        collision_register(task);
    }
}

/* clang-format off */
static struct collision_shape collider_shapes[] = {
    {10, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_IGNORE_PLAYERS | SHAPE_IGNORE_UNK_9 | SHAPE_UNTURNED | SHAPE_UNK_2000,
     {0.0F, 0.0F, 0.0F}, 1.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static void lockon_face_init(struct task *task, int player, Float range)
{
    struct task_work *work = task->work;

    FACE_PLAYER(work) = player;
    FACE_RANGE(work) = range;
    work->pos = task->parent->work->pos;
    work->id = WORK_ID_STOP_LOCKON;
    enemy_work_alloc(task);
    task->step = lockon_face_step;
    task->destructor = collider_destroy;
    collision_init(task, collider_shapes, 1, COLLISION_ITEM);
}

static void lockon_box_init(struct task *task, Float x, Float y, Float z)
{
    struct task_work *work = task->work;

    work->scl.x = x;
    work->scl.y = y;
    work->scl.z = z;
    work->id = WORK_ID_STOP_LOCKON_BOX;
    enemy_work_alloc(task);
    task->step = lockon_box_step;
    task->destructor = collider_destroy;
    collision_init(task, collider_shapes, 1, COLLISION_ITEM);
}

struct task *lockon_faces_create(struct task *parent, int player, Float range)
{
    struct task *task;

    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if ((task = task_create_child(TASK_WORK, lockon_face_step, parent)) !=
        NULL) {
        lockon_face_init(task, player, range);
        FACE_OFFSET_X(task->work) = FACE_SPACING;
        FACE_OFFSET_Y(task->work) = FACE_SPACING;
    }
    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if ((task = task_create_child(TASK_WORK, lockon_face_step, parent)) !=
        NULL) {
        lockon_face_init(task, player, range);
        FACE_OFFSET_X(task->work) = 0.0F;
        FACE_OFFSET_Y(task->work) = 0.0F;
    }
    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if ((task = task_create_child(TASK_WORK, lockon_face_step, parent)) !=
        NULL) {
        lockon_face_init(task, player, range);
        FACE_OFFSET_X(task->work) = -FACE_SPACING;
        FACE_OFFSET_Y(task->work) = -FACE_SPACING;
    }
    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if ((task = task_create_child(TASK_WORK, lockon_face_step, parent)) !=
        NULL) {
        lockon_face_init(task, player, range);
        FACE_OFFSET_X(task->work) = FACE_SPACING;
        FACE_OFFSET_Y(task->work) = -FACE_SPACING;
    }
    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if ((task = task_create_child(TASK_WORK, lockon_face_step, parent)) !=
        NULL) {
        lockon_face_init(task, player, range);
        FACE_OFFSET_X(task->work) = -FACE_SPACING;
        FACE_OFFSET_Y(task->work) = FACE_SPACING;
    }
    return task;
}

struct task *lockon_box_create(struct task *parent, Angle *ang,
                               NJS_VECTOR *size)
{
    struct task *task;
    NJS_VECTOR turned;
    NJS_POINT3 pos;

    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if ((task = task_create_child(TASK_WORK, lockon_box_step, parent)) !=
        NULL) {
        pos.x = 0.0F;
        pos.y = 0.0F;
        pos.z = -LOCKON_BOX_BACK;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&parent->work->pos);
        njRotateEx(ang, 0);
        njCalcVector(NULL, size, &turned);
        njCalcPoint(NULL, &pos, &pos);
        njPopMatrixEx();
        task->work->pos = pos;
        task->work->ang.x = 0;
        task->work->ang.z = 0;
        task->work->ang.y = 0;
        lockon_box_init(task, njAbs(turned.x), njAbs(turned.y),
                        njAbs(turned.z));
    }
    return task;
}

/* the words of a vertex's normal, after its point */
#define NORMAL_WORDS (CHUNK_VN_WORDS - CHUNK_V_WORDS)

/* copy src's first chunk to dst as an NJD_CV_D8 chunk, with color in place of
 * each vertex's normal. then copy the word after the chunk. each vertex of the
 * chunk must be a point, then a normal. the call never tests the chunk's
 * kind */
/* @hack redundant: mask, which nothing reads */
static Sint32 vertex_chunk_copy(Sint32 *dst, Sint32 *src, Uint32 color,
                                Uint32 mask)
{
    Sint32 word;
    Uint32 count;

    src++;
    word = *src++;
    count = (Uint16)(word >> CHUNK_HIGH_SHIFT);
    *dst++ = CHUNK_D8_HEAD(count);
    *dst++ = word;
    while (count--) {
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        src += NORMAL_WORDS;
        prefetch(src);
        *dst++ = color;
    }
    /* @hack spelling: the two assignments chained, where two statements would
     * do */
    *dst = word = *src;
    /* @bug logic: never returns unless NJD_CE follows the vertices, since the
     * loop tests word and never reads another. its branch back to the test is
     * at 0x2672 of the object's code */
    while (word != NJD_CE) { /* NOLINT(bugprone-infinite-loop) */
    }
    return 0;
}

void object_tree_tint(NJS_CNK_OBJECT *object, Uint32 color)
{
    /* @hack spelling: node, a copy of object for the null test and the read
     * of the child, where object would do */
    NJS_CNK_OBJECT *node = object;
    NJS_CNK_OBJECT *child;
    Sint32 *vlist;

    if (node != NULL) {
        vlist = object->model->vlist;
        if (color != COLOR_NONE && (Uint16)*vlist == NJD_CV_D8) {
            vlist_shade(vlist, color);
        }
    }
    /* @bug memory: reads node->child through null where object is null, since
     * the test above guards only the tint. the read is at 0x26a2 of the
     * object's code, and the test at 0x2684 */
    /* @bug logic: steps the loop to object->sibling, where child->sibling is
     * meant, at 0x26ac of the object's code. so the call visits no sibling of
     * object's child. where object has a sibling, the loop visits that sibling
     * without end */
    for (child = node->child; child != NULL; child = object->sibling) {
        object_tree_tint(child, color);
    }
}

void model_draw_tinted(NJS_CNK_MODEL *model, Uint32 color)
{
    Sint32 *vlist = model->vlist;
    Sint32 *copy;

    if (color != COLOR_NONE && (Uint16)*vlist == NJD_CV_D8) {
        copy = (Sint32 *)WORK_AREA;
        vlist_copy_shaded(copy, vlist, color, VLIST_UNREAD_MASK);
        model->vlist = copy;
        njCnkEasyDrawModel(model);
        model->vlist = vlist;
    } else {
        njCnkEasyDrawModel(model);
    }
}

void model_draw_colored(NJS_CNK_MODEL *model, Uint32 color)
{
    Sint32 *vlist = model->vlist;
    Sint32 *copy = (Sint32 *)WORK_AREA;

    vertex_chunk_copy(copy, vlist, color, 0xffff);
    model->vlist = copy;
    njCnkEasyDrawModel(model);
    model->vlist = vlist;
}

/* @hack type: color a Sint32, where the Uint32 that vertex_chunk_copy takes
 * would do */
NJS_DIRECT_HEAD *model_build_colored(Uint8 **buffer, NJS_CNK_MODEL *model,
                                     Sint32 color)
{
    Sint32 *vlist = model->vlist;
    Sint32 *copy = (Sint32 *)WORK_AREA;
    NJS_DIRECT_HEAD *built;

    vertex_chunk_copy(copy, vlist, color, 0xffff);
    model->vlist = copy;
    built = model_build(buffer, model);
    model->vlist = vlist;
    return built;
}

/* the players player_holding looks at, from slot 0 */
#define HOLDING_PLAYERS 2

int player_holding(struct task *task)
{
    int i;

    for (i = 0; i < HOLDING_PLAYERS; i++) {
        if (player_records[i] != NULL && player_records[i]->held == task) {
            return i;
        }
    }
    return -1;
}

Bool camera_near(NJS_POINT3 *pos, Float range2)
{
    NJS_POINT3 *camera = &camera_views[camera_current_index]->pos;
    NJS_VECTOR offset;
    /* @hack spelling: the two points walked as Floats, where member reads
     * would do */
    Float *out = &offset.x;
    Float *from = &camera->x;
    Float *point = &pos->x;

    *out++ = *from++ - *point++;
    *out++ = *from++ - *point++;
    *out = *from - *point;
    return njScalor2(&offset) <= range2;
}

Float pulse_level(int hold, int rest, int rise, int fall)
{
    int step;
    int period;
    Float level;

    period = hold + rest + rise + fall;
    step = game_state.steps % period;
    if (step < rise) {
        level = (Float)step / (Float)rise;
    } else {
        step -= rise;
        if (step < hold) {
            level = 1.0F;
        } else {
            step -= hold;
            if (step < fall) {
                /* @hack spelling: level set in two statements, where one that
                 * takes the quotient from 1.0F would do */
                level = (Float)step / (Float)fall;
                level = 1.0F - level;
            } else {
                level = 0.0F;
            }
        }
    }
    if (level < 0.0F) {
        level = 0.0F;
    }
    if (level > 1.0F) {
        level = 1.0F;
    }
    return level;
}

Float blink_level(int period, int rest, int fade)
{
    int step;
    Float level;

    step = game_state.steps % period;
    if (step < fade) {
        /* @hack spelling: level set in two statements, where one that takes
         * the quotient from 1.0F would do */
        level = (Float)step / (Float)fade;
        level = 1.0F - level;
    } else if (step > rest) {
        step -= rest;
        if (step < fade) {
            level = (Float)step / (Float)fade;
        } else {
            level = 1.0F;
        }
    } else {
        level = 0.0F;
    }
    if (level < 0.0F) {
        level = 0.0F;
    }
    if (level > 1.0F) {
        level = 1.0F;
    }
    return level;
}

/* each channel of a gray runs to CHANNEL_MAX in CHANNEL_BITS bits, under an
 * OPAQUE alpha. blink_light_gray's gray halves the level and adds
 * LIGHT_GRAY_LOW */
#define CHANNEL_MAX 255
#define CHANNEL_BITS 8
#define OPAQUE 0xff000000
#define LIGHT_GRAY_LOW 128

Uint32 blink_gray(int period, int rest, int fade)
{
    Float level;
    int step;
    int gray;

    step = game_state.steps % period;
    if (step < fade) {
        /* @hack spelling: level set and scaled in two statements, where one
         * expression in gray's assignment would do */
        level = (Float)step / (Float)fade;
        level *= CHANNEL_MAX;
        gray = CHANNEL_MAX - (int)level;
    } else if (step > rest) {
        step -= rest;
        if (step < fade) {
            /* @hack spelling: level set and scaled in two statements, where
             * one expression in gray's assignment would do */
            level = (Float)step / (Float)fade;
            level *= CHANNEL_MAX;
            gray = level;
        } else {
            gray = CHANNEL_MAX;
        }
    } else {
        gray = 0;
    }
    if (gray < 0) {
        gray = 0;
    }
    if (gray > CHANNEL_MAX) {
        gray = CHANNEL_MAX;
    }
    gray =
        (gray << (2 * CHANNEL_BITS)) + (gray << CHANNEL_BITS) + gray + OPAQUE;
    return gray;
}

Uint32 blink_light_gray(int period, int rest, int fade)
{
    int step;
    int gray;
    Float level;

    step = game_state.steps % period;
    if (step < fade) {
        /* @hack spelling: level set and scaled in two statements, where one
         * expression in gray's assignment would do */
        level = (Float)step / (Float)fade;
        level *= CHANNEL_MAX;
        gray = CHANNEL_MAX - (int)level;
    } else if (step > rest) {
        step -= rest;
        if (step < fade) {
            /* @hack spelling: level set and scaled in two statements, where
             * one expression in gray's assignment would do */
            level = (Float)step / (Float)fade;
            level *= CHANNEL_MAX;
            gray = level;
        } else {
            gray = CHANNEL_MAX;
        }
    } else {
        gray = 0;
    }
    gray /= 2;
    gray += LIGHT_GRAY_LOW;
    if (gray < LIGHT_GRAY_LOW) {
        gray = LIGHT_GRAY_LOW;
    }
    if (gray > CHANNEL_MAX) {
        gray = CHANNEL_MAX;
    }
    gray =
        (gray << (2 * CHANNEL_BITS)) + (gray << CHANNEL_BITS) + gray + OPAQUE;
    return gray;
}

static void weak_point_step(struct task *task);
static void weak_point_destroy(struct task *task);

/* clang-format off */
static struct collision_shape weak_point_shapes[] = {
    {10, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     WEAK_POINT_IGNORES | SHAPE_UNK_800000, {0.0F, 0.0F, 0.0F}, 1.0F, 0.0F, 0.0F,
     0, {0, 0, 0}},
};
/* clang-format on */

static void weak_point_init(struct task *task)
{
    task->step = weak_point_step;
    task->destructor = weak_point_destroy;
    collision_init(task, weak_point_shapes, 1, COLLISION_ITEM);
    task->work->collision->flags &= ~COLLISION_TARGET;
    enemy_work_alloc(task);
}

struct task *weak_point_create(struct task *parent, int index, NJS_POINT3 *pos)
{
    struct task *task;

    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    /* @hack spelling: task returned while it is null, where NULL would do */
    if ((task = task_create_child(TASK_WORK, weak_point_init, parent)) ==
        NULL) {
        return task;
    }
    task->work->pos = *pos;
    WEAK_POINT_INDEX(task->work) = index;
    return task;
}

struct task *weak_point_create_shaped(struct task *parent, int index,
                                      NJS_POINT3 *pos,
                                      struct collision_shape *shapes, int count)
{
    struct task *task;
    struct task_work *work;

    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    /* @hack spelling: task returned while it is null, where NULL would do */
    if ((task = task_create_child(TASK_WORK, weak_point_step, parent)) ==
        NULL) {
        return task;
    }
    work = task->work;
    work->pos = *pos;
    /* @hack spelling: task->work read again, where work would do */
    WEAK_POINT_INDEX(task->work) = index;
    task->step = weak_point_step;
    task->destructor = weak_point_destroy;
    if (shapes != NULL && count != 0) {
        collision_init(task, shapes, count, COLLISION_ITEM);
    }
    enemy_work_alloc(task);
    return task;
}

struct task *weak_point_create_paired(struct task *parent, int index,
                                      NJS_POINT3 *pos)
{
    struct task *task;

    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    /* @hack spelling: task returned while it is null, where NULL would do */
    if ((task = task_create_child(TASK_WORK, weak_point_init, parent)) ==
        NULL) {
        return task;
    }
    task->work->pos = *pos;
    WEAK_POINT_INDEX(task->work) = index;
    WEAK_POINT_PAIRED(task->work) = TRUE;
    return task;
}

static void weak_point_destroy(struct task *task)
{
    enemy_destroy(task);
}

static void weak_point_step(struct task *task)
{
    struct task *parent = task->parent;
    struct task_work *parent_work = parent->work;
    struct task_work *work = task->work;
    struct enemy_work *enemy;
    NJS_VECTOR spd;
    NJS_POINT3 pos;

    if (work->flags & WORK_HURT) {
        /* @hack spelling: task->work read again, where work would do */
        if (WEAK_POINT_PAIRED(task->work)) {
            parent_work->submode |= 1 << (WEAK_POINT_INDEX(work) * 2);
            enemy = task->motion.p;
            if (enemy != NULL) {
                if (enemy->shot_by) {
                    parent_work->submode |=
                        1 << ((WEAK_POINT_INDEX(work) * 2) + 1);
                } else {
                    parent_work->submode &=
                        ~(1 << ((WEAK_POINT_INDEX(work) * 2) + 1));
                }
            }
        } else {
            parent_work->submode |= 1 << WEAK_POINT_INDEX(work);
        }
        spd.x = 0.0F;
        spd.y = 0.0F;
        spd.z = 0.0F;
        pos = work->pos;
        dust_create(&pos, &spd, HURT_DUST_SCALE);
        flash_manager_create_sparks_depth(pos.x, pos.y, pos.z, HURT_FLASH_SCALE,
                                          HURT_SPARKS, HURT_FLASH_DEPTH_BIAS);
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
    } else if (!work->submode) {
        collision_register(task);
    }
}

void matrix_get_angles(NJS_MATRIX *matrix, struct angles *ang)
{
    NJS_VECTOR axis;

    njPushMatrix(matrix);
    axis.x = 0.0F;
    axis.y = 1.0F;
    axis.z = 0.0F;
    njCalcVector(NULL, &axis, &axis);
    ang->z = atan2f(-axis.x, axis.y) * ANGLE_PER_RADIAN;
    ang->x =
        atan2f(axis.z, njSqrt(1.0F - (axis.z * axis.z))) * ANGLE_PER_RADIAN;
    njInvertMatrix(NULL);
    njRotateZ(NULL, ang->z);
    njRotateX(NULL, ang->x);
    axis.x = 0.0F;
    axis.y = 0.0F;
    axis.z = 1.0F;
    njCalcVector(NULL, &axis, &axis);
    ang->y = -(Angle)(atan2f(axis.x, axis.z) * ANGLE_PER_RADIAN);
    njPopMatrixEx();
}

struct trail *trail_alloc(struct trail_style *style, int count)
{
    struct trail *trail;
    NJS_POINT3 *points;

    trail = syCalloc(1, sizeof(struct trail));
    if (trail != NULL) {
        points = syCalloc(count, sizeof(NJS_POINT3));
        if (points != NULL) {
            trail->points = points;
            trail->count = count;
        } else {
            syFree(trail);
            trail = NULL;
        }
    }
    return trail;
}

/* the curves TRAIL_CURVE picks from. each takes a point's place along the
 * trail, 0 at the oldest point and 1 at the newest. it returns the share of
 * the way from the style's old width and color to its new ones */
static Float trail_curve_square(Float part)
{
    return part * part;
}

static Float trail_curve_sine(Float part)
{
    return njSin(part * QUARTER_TURN);
}

static Float trail_curve_line(Float part)
{
    return part;
}

/* @hack spelling: three inline functions that walk a vector as Floats, where
 * its members written out at each use would do */
#pragma inline(vector_scale_by, point_minus, point_plus)

static void vector_scale_by(Float *vec, Float scale)
{
    *vec++ *= scale;
    *vec++ *= scale;
    *vec *= scale;
}

static void point_minus(Float *vec, Float *from, Float *out)
{
    *out++ = *from++ - *vec++;
    *out++ = *from++ - *vec++;
    *out = *from - *vec;
}

static void point_plus(Float *vec, Float *from, Float *out)
{
    *out++ = *from++ + *vec++;
    *out++ = *from++ + *vec++;
    *out = *from + *vec;
}

/* a trail's strip takes a vertex on each of its SIDES for each kept point. it
 * draws only once it holds a TRIANGLE. a draw keeps pos for at most MOST_KEPT
 * of the steps since the trail's last draw */
#define SIDES 2
#define TRIANGLE 3
#define MOST_KEPT 4

static void trail_draw_plain(struct trail_style *style, struct trail *trail,
                             NJS_POINT3 *pos)
{
    Float (*curve)(Float part);
    Float (*picked)(Float part);
    NJS_POLYGON_VTX *vtx;
    NJS_POLYGON_VTX *out;
    NJS_POINT3 tip;
    NJS_VECTOR across;
    NJS_POINT3 mid;
    NJS_POINT3 *p0;
    NJS_POINT3 *p1;
    Float scale;
    Float part;
    Float rest;
    Float len;
    Uint32 color;
    int i;
    int k;
    int cur;
    int prev;
    int nv;
    int num;
    int used;
    int count;
    Uint32 step;
    Float (*spare)(Float part);
    Sint32 *area;

    /* @hack spelling: the flags read into num for the switch, where
     * style->flags there would do */
    num = style->flags;
    switch (num & TRAIL_CURVE) {
    case TRAIL_SQUARE:
        picked = trail_curve_square;
        break;
    case TRAIL_SINE:
        picked = trail_curve_sine;
        break;
    default:
        picked = trail_curve_line;
        break;
    }
    /* @hack redundant: spare, a copy of picked that curve then takes, where
     * curve = picked would do */
    spare = picked;
    curve = spare;
    /* @hack redundant: area, a pointer to WORK_AREA that vtx then takes,
     * where vtx = (NJS_POLYGON_VTX *)WORK_AREA would do */
    area = (Sint32 *)WORK_AREA;
    vtx = (NJS_POLYGON_VTX *)area;
    /* @hack redundant: a read of pos->x that nothing uses */
    (void)pos->x;
    used = trail->used;
    count = trail->count;
    /* @hack spelling: 0 for the null matrix, where NULL would do */
    njCalcPoint(0, pos, &tip);
    out = vtx;
    nv = 0;
    scale = used - 1;
    for (i = used - 1; i >= 1; i--) {
        part = (Float)i / scale;
        if (part < 0.0F) {
            part = 0.0F;
        }
        if (part > 1.0F) {
            part = 1.0F;
        }
        part = curve(part);
        rest = 1.0F - part;
        color = color_blend(style->new_color, style->old_color, rest);
        /* @hack spelling: k, a local for trail->head + i, where the sum in
         * each index would do */
        k = trail->head + i;
        cur = k % count;
        prev = (k - 1) % count;
        p1 = &trail->points[prev];
        p0 = &trail->points[cur];
        across.x = p1->y - p0->y;
        across.y = -p1->x + p0->x;
        across.z = 0.0F;
        len = njScalor(&across);
        if (len > 1.0F) {
            vector_scale_by(&across.x,
                            ((part * (style->new_width - style->old_width)) +
                             style->old_width) /
                                len);
            if (nv == 0) {
                point_minus(&across.x, &tip.x, &out->x);
                out->col = style->new_color;
                out++;
                point_plus(&across.x, &tip.x, &out->x);
                nv += SIDES;
                out->col = style->new_color;
                out++;
            }
            point_midpoint(&trail->points[prev], &trail->points[cur], &mid);
            point_minus(&across.x, &mid.x, &out->x);
            out->col = color;
            out++;
            point_plus(&across.x, &mid.x, &out->x);
            nv += SIDES;
            out->col = color;
            out++;
        }
    }
    if (nv >= TRIANGLE) {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njDrawPolygon3DEx(vtx, nv, 1);
        njPopMatrixEx();
    }
    if (stage_get_mode() == STAGE_PAUSE) {
        return;
    }
    step = trail->step;
    num = game_state.steps - step;
    if (num < 0) {
        num = -num;
    }
    /* @hack spelling: the cap tested as num >= MOST_KEPT + 1, where
     * num > MOST_KEPT would do */
    if (num >= MOST_KEPT + 1) {
        num = MOST_KEPT;
    }
    if (num == 0 && game_state.time_stopped) {
        trail->used = 0;
        /* @hack spelling: head set from used, where 0 would do */
        trail->head = trail->used;
    }
    for (; num--; step++) {
        if (!(style->flags & TRAIL_ODD_STEPS) || (step & 1)) {
            trail->points[(trail->head + trail->used) % count] = tip;
            if (trail->used < count) {
                trail->used++;
            } else {
                trail->head = (trail->head + 1) % count;
            }
        }
    }
    trail->step = game_state.steps;
}

static void trail_draw_textured(struct trail_style *style, struct trail *trail,
                                NJS_POINT3 *pos)
{
    Float (*curve)(Float part);
    Float (*picked)(Float part);
    NJS_TEXTURE_VTX *vtx;
    NJS_TEXTURE_VTX *out;
    NJS_POINT3 tip;
    NJS_VECTOR across;
    NJS_POINT3 mid;
    NJS_POINT3 *p0;
    NJS_POINT3 *p1;
    Float scale;
    Float part;
    Float rest;
    Float tex_u;
    Float len;
    Uint32 color;
    int i;
    int cur;
    int prev;
    int nv;
    int num;
    int used;
    int count;
    Uint32 step;

    /* @hack spelling: the flags read into num for the switch, where
     * style->flags there would do */
    num = style->flags;
    switch (num & TRAIL_CURVE) {
    case TRAIL_SQUARE:
        picked = trail_curve_square;
        break;
    case TRAIL_SINE:
        picked = trail_curve_sine;
        break;
    default:
        picked = trail_curve_line;
        break;
    }
    curve = picked;
    vtx = (NJS_TEXTURE_VTX *)WORK_AREA;
    used = trail->used;
    count = trail->count;
    /* @hack spelling: 0 for the null matrix, where NULL would do */
    njCalcPoint(0, pos, &tip);
    out = vtx;
    nv = 0;
    scale = used - 1;
    for (i = used - 1; i >= 1; i--) {
        part = (Float)i / scale;
        if (part < 0.0F) {
            part = 0.0F;
        }
        if (part > 1.0F) {
            part = 1.0F;
        }
        part = curve(part);
        /* @hack spelling: rest assigned inside the call's argument, where a
         * statement of its own would do */
        color =
            color_blend(style->new_color, style->old_color, rest = 1.0F - part);
        tex_u = 0.5F + (0.5F * rest);
        cur = (trail->head + i) % count;
        prev = (trail->head + i - 1) % count;
        /* @hack spelling: p1 and p0 stepped from the points' start, where
         * &trail->points[prev] and &trail->points[cur] would do */
        p0 = trail->points;
        p1 = p0 + prev;
        p0 += cur;
        across.x = p1->y - p0->y;
        across.y = -p1->x + p0->x;
        across.z = 0.0F;
        len = njScalor(&across);
        if (len > 1.0F) {
            vector_scale_by(&across.x,
                            ((part * (style->new_width - style->old_width)) +
                             style->old_width) /
                                len);
            if (nv == 0) {
                point_minus(&across.x, &tip.x, &out->x);
                out->col = style->new_color;
                out->u = tex_u;
                /* @hack spelling: out stepped inside the store, where a
                 * statement of its own would do */
                (out++)->v = 0.0F;
                point_plus(&across.x, &tip.x, &out->x);
                nv += SIDES;
                out->col = style->new_color;
                out->u = tex_u;
                /* @hack spelling: out stepped inside the store, where a
                 * statement of its own would do */
                (out++)->v = 1.0F;
            }
            point_midpoint(&trail->points[prev], &trail->points[cur], &mid);
            point_minus(&across.x, &mid.x, &out->x);
            out->col = color;
            out->u = tex_u;
            /* @hack spelling: out stepped inside the store, where a statement
             * of its own would do */
            (out++)->v = 0.0F;
            point_plus(&across.x, &mid.x, &out->x);
            nv += SIDES;
            out->col = color;
            out->u = tex_u;
            out->v = 1.0F;
            out++;
        }
    }
    if (nv >= TRIANGLE) {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njSetTextureNum(style->texture);
        njDrawTexture3DEx(vtx, nv, 1);
        njPopMatrixEx();
    }
    if (stage_get_mode() == STAGE_PAUSE) {
        return;
    }
    step = trail->step;
    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if ((num = game_state.steps - step) < 0) {
        num = -num;
    }
    /* @hack spelling: the cap tested as num >= MOST_KEPT + 1, where
     * num > MOST_KEPT would do */
    if (num >= MOST_KEPT + 1) {
        num = MOST_KEPT;
    }
    if (num == 0 && game_state.time_stopped) {
        trail->used = 0;
        /* @hack spelling: head set from used, where 0 would do */
        trail->head = trail->used;
    }
    for (; num--; step++) {
        if (!(style->flags & TRAIL_ODD_STEPS) || (step & 1)) {
            trail->points[(trail->head + trail->used) % count] = tip;
            if (trail->used < count) {
                trail->used++;
            } else {
                trail->head = (trail->head + 1) % count;
            }
        }
    }
    trail->step = game_state.steps;
}

void trail_draw(struct trail_style *style, struct trail *trail, NJS_POINT3 *pos)
{
    if (style == NULL) {
        if (trail != NULL) {
            trail->used = 0;
        }
        return;
    }
    /* @bug memory: reads trail through null where a caller passes a null
     * trail to trail_drop, or to trail_draw with a style. trail_alloc returns
     * null where the heap has no room. trail_draw tests trail for null at
     * 0x355c of the object's code, and trail_free at 0x357e. trail is read by
     * trail_draw_plain at 0x2ecc, by trail_draw_textured at 0x322c and by
     * trail_drop at 0x359a */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: a null trail draws nothing */
    if (trail == NULL) {
        return;
    }
#endif
    if (style->flags & TRAIL_TEXTURED) {
        trail_draw_textured(style, trail, pos);
    } else {
        trail_draw_plain(style, trail, pos);
    }
}

void trail_free(struct trail_style *style, struct trail *trail)
{
    if (trail != NULL) {
        syFree(trail->points);
        syFree(trail);
    }
}

int trail_drop(struct trail *trail)
{
#ifdef NONMATCHING
    /* this side repairs the bug tagged in trail_draw: a null trail drops
     * nothing */
    if (trail == NULL) {
        return 0;
    }
#endif
    if (trail->used >= 1) {
        trail->head = (trail->head + 1) % trail->count;
        trail->used--;
        return 1;
    }
    return 0;
}

/* a node with no place, turn or scale, which model_build copies and gives
 * its model to compile */
/* clang-format off */
static NJS_CNK_OBJECT build_object = {
    BUILD_EVAL, NULL,
    {0.0F, 0.0F, 0.0F}, {0, 0, 0}, {1.0F, 1.0F, 1.0F},
    NULL, NULL,
};
/* clang-format on */

NJS_DIRECT_HEAD *model_build(Uint8 **buffer, NJS_CNK_MODEL *model)
{
    NJS_CNK_OBJECT object;
    Uint8 *end;
    NJS_DIRECT_HEAD *built;

    built = NULL;
    object = build_object;
    object.model = model;
    end = *buffer + njCnkDirectObjectCompileSize(&object);
    if (end < DEMO_AREA) {
        njCnkDirectObjectCompile(&object, *buffer);
        built = (NJS_DIRECT_HEAD *)*buffer;
        *buffer = end;
    }
    return built;
}

NJS_DIRECT_HEAD *object_build(Uint8 **buffer, NJS_CNK_OBJECT *object)
{
    NJS_CNK_OBJECT copy;
    Uint8 *end;
    NJS_DIRECT_HEAD *built;

    copy = *object;
    copy.evalflags = BUILD_EVAL;
    copy.child = NULL;
    copy.sibling = NULL;
    built = NULL;
    end = *buffer + njCnkDirectObjectCompileSize(&copy);
    if (end < DEMO_AREA) {
        njCnkDirectObjectCompile(&copy, *buffer);
        built = (NJS_DIRECT_HEAD *)*buffer;
        *buffer = end;
    }
    return built;
}

NJS_DIRECT_HEAD *object_tree_build(Uint8 **buffer, NJS_CNK_OBJECT *object)
{
    NJS_CNK_OBJECT copy;
    Uint8 *end;
    NJS_DIRECT_HEAD *built;

    copy = *object;
    built = NULL;
    end = *buffer + njCnkDirectObjectCompileSize(&copy);
    if (end < DEMO_AREA) {
        njCnkDirectObjectCompile(&copy, *buffer);
        built = (NJS_DIRECT_HEAD *)*buffer;
        *buffer = end;
    }
    return built;
}

int model_builds_compile(Uint8 **buffer, struct model_build *builds, int count)
{
    int i;
    int built;

    built = 0;
    for (i = 0; i < count; i++, builds++) {
        if (builds->built == NULL) {
            if (builds->texlist != NULL) {
                if (texture_list_loaded(builds->texlist)) {
                    njSetTexture(builds->texlist);
                } else {
                    continue;
                }
            }
            switch (builds->kind) {
            default:
            case MODEL_BUILD_ALONE:
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((builds->built = object_build(buffer, builds->object)) !=
                    NULL) {
                    built++;
                }
                break;
            case MODEL_BUILD_ALONE_AS_LAND:
                draw_control_set(DRAW_CONTROL_SHADOW);
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((builds->built = object_build(buffer, builds->object)) !=
                    NULL) {
                    built++;
                }
                draw_control_clear(DRAW_CONTROL_SHADOW);
                break;
            case MODEL_BUILD_TREE:
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((builds->built =
                         object_tree_build(buffer, builds->object)) != NULL) {
                    built++;
                }
                break;
            case MODEL_BUILD_TREE_AS_LAND:
                draw_control_set(DRAW_CONTROL_SHADOW);
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((builds->built =
                         object_tree_build(buffer, builds->object)) != NULL) {
                    built++;
                }
                draw_control_clear(DRAW_CONTROL_SHADOW);
                break;
            }
        }
    }
    return built;
}

Bool sphere_in_front_no_depth(NJS_POINT3 *center, Float radius)
{
    NJS_POINT3 view;

    njCalcPoint(NULL, center, &view);
    if (-radius > view.z) {
        return FALSE;
    }
    return TRUE;
}

Bool sphere_in_front(NJS_POINT3 *center, Float radius, Float *depth)
{
    NJS_POINT3 view;

    njCalcPoint(NULL, center, &view);
    *depth = view.z;
    if (-radius > view.z) {
        return FALSE;
    }
    return TRUE;
}

Bool sphere_in_front_within(NJS_POINT3 *center, Float radius, Float reach)
{
    NJS_POINT3 view;

    njCalcPoint(NULL, center, &view);
    if (view.z < -radius || view.z > reach) {
        return FALSE;
    }
    return TRUE;
}

/* The corners of the screen, in pixels, which screen_fill colors and draws. */
#define SCREEN_CORNERS 4
static NJS_POLYGON_VTX screen_corners[SCREEN_CORNERS] = {
#include "shared/common_object/screen_corners.inc"
};

void screen_fill(NJS_COLOR *color)
{
    int trans;

    if (color->argb.a) {
        if (color->argb.a == CHANNEL_MAX) {
            trans = 0;
        } else {
            trans = 1;
        }
        screen_corners[0].col = screen_corners[1].col = screen_corners[2].col =
            screen_corners[3].col = color->color;
        njFogDisable();
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njDrawPolygon(screen_corners, SCREEN_CORNERS, trans);
        njFogEnable();
    }
}

#define HOLDABLE_RELEASES_ANIMALS 1
#include "shared/holdable_motion_impl.h"

#define HOLDABLE_MODEL(task) ((NJS_CNK_MODEL *)(task)->extra.p)
/* how far above pos a holdable object draws its shadow, and the shadow's
 * scale across x and z */
#define SHADOW_LIFT 0.1F
#define SHADOW_SCALE 3.0F

static void holdable_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture((NJS_TEXLIST *)task->unk_3c.p);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    if (work->flags & WORK_HELD_BY_PLAYER) {
        njRotateZ(NULL, player_work[HOLDER(work)]->ang.z);
        njRotateX(NULL, player_work[HOLDER(work)]->ang.x);
    } else {
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, SHADOW_LIFT, 0.0F);
        njScale(NULL, SHADOW_SCALE, 1.0F, SHADOW_SCALE);
        njCnkModDrawObject(&shadow_object);
        njPopMatrixEx();
    }
    njRotateY(NULL, work->ang.y);
    njTranslate(NULL, HOLDABLE_MODEL(task)->center.x,
                HOLDABLE_MODEL(task)->center.y, HOLDABLE_MODEL(task)->center.z);
    njMultiMatrix(NULL, &BODY(task)->rolled);
    njTranslate(NULL, -HOLDABLE_MODEL(task)->center.x,
                -HOLDABLE_MODEL(task)->center.y,
                -HOLDABLE_MODEL(task)->center.z);
    njCnkEasyDrawModel(HOLDABLE_MODEL(task));
    njPopMatrixEx();
}

#define HOLDABLE_SETUP void holdable_init
#include "shared/holdable_setup.h" /* IWYU pragma: keep -- the code it compiles here */

int screen_project(NJS_POINT3 *point, NJS_POINT3 *out)
{
    NJS_POINT3 view;
    /* @hack redundant: sx, sy, cx and cy declared register, where plain locals
     * would do */
    register Float sx;
    register Float sy;
    register Float cx;
    register Float cy;
    Float inv;

    njCalcPoint(NULL, point, &view);
    if (view.z < 0.0F) {
        return 0;
    }
#ifdef NONMATCHING
    /* this side leaves the x aspect out, as the port's njProjectScreen
     * does (engine/dc/port.h) */
    sx = _nj_screen_.dist;
#else
    sx = _nj_screen_.dist * _nj_scr_aspect_x_;
#endif
    sy = _nj_screen_.dist * _nj_scr_aspect_y_;
    cx = _nj_screen_.cx;
    cy = _nj_screen_.cy;
    inv = 1.0F / view.z;
    out->x = (view.x * inv * sx) + cx;
    out->y = (view.y * inv * sy) + cy;
    /* @hack spelling: the quotient written out again, where inv would do */
    out->z = 1.0F / view.z;
    return 1;
}

static int dat_common_object_1 = 0;

void fun_common_object_3dcc(int value)
{
    dat_common_object_1 = value;
}

int fun_common_object_3dd2(void)
{
    return dat_common_object_1;
}

/* object_sound_play's kind 5 makes a task at DUMMY_LEVEL. the task waits out
 * its timer, then sets it again while its count lasts. the first timer is
 * DUMMY_FIRST_WAIT and each later one DUMMY_WAIT, each plus less than
 * DUMMY_WAIT_SPREAD at random. the count is DUMMY_WAITS plus less than
 * DUMMY_WAITS_SPREAD at random */
#define DUMMY_LEVEL 3
#define DUMMY_FIRST_WAIT 4.0F
#define DUMMY_WAIT 6.0F
#define DUMMY_WAIT_SPREAD 8.0F
#define DUMMY_WAITS 1.0F
#define DUMMY_WAITS_SPREAD 2.0F

/* the step of object_sound_play's task for kind 5, which plays no sound */
static void dummy_sound_step(struct task *task)
{
    struct task_work *work = task->work;

    if (work->timer-- > 0) {
        return;
    }
    /* @hack spelling: task->work read again, where work would do */
    task->work->timer =
        DUMMY_WAIT + (rand() * (DUMMY_WAIT_SPREAD / (RAND_MAX + 1)));
    if (work->submode-- > 0) {
        return;
    }
    task_kill(task);
}

/* the sound ids that object_sound_play picks from where kind is 10. it scales
 * rand to a whole below PICK_RANGE, and takes that modulo the ids' count.
 * PICK_RANGE is a multiple of the count, so each id is as likely */
#define PICK_RANGE 100.0F
static int object_sounds[] = {0x1010, 0x1011, 0x1004, 0x100c, 0x8015};

void object_sound_play(int kind, NJS_POINT3 *pos)
{
    struct task *task;
    Uint32 index;

    switch (kind) {
    case 1:
    case 2:
    case 4:
        sound_play_at(0x8015, NULL, 0, SOUND_LOUDEST, pos);
        break;
    case 3:
        sound_play_at(0x1022, NULL, 0, SOUND_LOUDEST, pos);
        break;
    case 5:
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((task = task_create(TASK_WORK, DUMMY_LEVEL, dummy_sound_step,
                                "ComObjDmySe")) != NULL) {
            sound_play_at(0x8015, NULL, 0, SOUND_LOUDEST, pos);
            task->work->timer = DUMMY_FIRST_WAIT +
                                (rand() * (DUMMY_WAIT_SPREAD / (RAND_MAX + 1)));
            task->work->submode =
                DUMMY_WAITS + (rand() * (DUMMY_WAITS_SPREAD / (RAND_MAX + 1)));
        }
        break;
    case 6:
        sound_play_at(0x1010, NULL, 0, SOUND_LOUDEST, pos);
        break;
    case 0:
    case 7:
        sound_play_at(0x1011, NULL, 0, SOUND_LOUDEST, pos);
        break;
    case 8:
        sound_play_at(0x1004, NULL, 0, SOUND_LOUDEST, pos);
        break;
    case 9:
        sound_play_at(0x100c, NULL, 0, SOUND_LOUDEST, pos);
        break;
    case 10:
        index = (int)(rand() * (PICK_RANGE / (RAND_MAX + 1))) %
                (sizeof(object_sounds) / sizeof(object_sounds[0]));
        sound_play_at(object_sounds[index], NULL, 0, SOUND_LOUDEST, pos);
        break;
    }
}
