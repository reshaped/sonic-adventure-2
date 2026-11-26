/* the mover, as mover.h declares it */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/polygon.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "object/mover.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

/* mover_step brings a mover to rest where its speed across the ground falls
 * below REST_SPEED. it wakes the mover where its whole speed passes
 * REST_SPEED */
#define REST_SPEED 0.01F
/* under MOVER_SLOPE_BOUNCE, a floor whose normal has a y above LEVEL_FLOOR
 * reflects only spd.y */
#define LEVEL_FLOOR 0.78F
/* the radius about the probe's point that the world's objects are listed
 * within */
#define PROBE_RADIUS 1.0F
/* a surface on a side is a wall where its normal's part along the side lies
 * beyond WALL_FACING either way. below -WALL_FACING, the wall faces the
 * probe's point. above WALL_FACING, its back faces the point */
#define WALL_FACING 0.2F
/* the share of an overlap with a wall that a call of mover_meet_stage
 * undoes */
#define WALL_UNDONE 0.95F
/* a wall whose back faces the probe's point pushes the mover through only
 * where radius plus the wall's distance lies below BEHIND_WALL_REACH */
#define BEHIND_WALL_REACH 8.0F
/* the mover heads into a wall whose normal's part along the mover's heading
 * lies below HEADING_INTO_WALL */
#define HEADING_INTO_WALL (-0.07071136F)

static Float mover_player_distance(struct task *task, int player);

#ifdef NONMATCHING
/* the squared distance from pos to point, summed as SHC's code sums it. SHC
 * squares the y difference by FMUL, then adds the x and z squares by FMAC.
 * the sum thus rounds as the console's does */
static Float distance2(const NJS_POINT3 *point, const NJS_POINT3 *pos)
{
    Float dx = point->x - pos->x;
    Float dy = point->y - pos->y;
    Float dz = point->z - pos->z;

    return __builtin_fmaf(dz, dz, __builtin_fmaf(dx, dx, dy * dy));
}
#endif

struct mover *mover_alloc(struct task *task)
{
    struct mover *mover;

    mover = syCalloc(sizeof(struct mover), 1);
#ifdef NONMATCHING
    /* this side returns null where syCalloc fails, where the matching side
     * writes through the null */
    if (mover == NULL) {
        return NULL;
    }
#endif
    task->motion.p = mover;
    mover->turn_speed = 0x100;
    mover->unk_b4 = 3.0F;
    mover->radius = 2.2F;
    mover->bottom = -3.0F;
    mover->unk_c0 = 40.0F;
    mover->wall_bounce = 0.8F;
    mover->floor_bounce = 0.9F;
    mover->unk_cc = 0.2F;
    mover->friction = 0.8F;
    mover->probe_height = 3.0F;
    return mover;
}

void mover_step(struct task *task)
{
    struct mover *mover = task->motion.p;
    NJS_VECTOR *spd = &mover->spd;
    NJS_VECTOR *acc = &mover->acc;
    NJS_POINT3 *pos = &task->work->pos;
    NJS_VECTOR across;

    acc->y += mover->gravity;
    spd->x += acc->x;
    spd->y += acc->y;
    spd->z += acc->z;
    acc->x = 0.0F;
    acc->y = 0.0F;
    acc->z = 0.0F;
    if (mover->flags & MOVER_CAN_REST) {
        if (mover->flags & MOVER_RESTING) {
            if (!collision_pushed(task) && njScalor(spd) <= REST_SPEED) {
                return;
            }
            mover->flags &= ~MOVER_RESTING;
        } else if ((mover->flags & MOVER_ON_FLOOR) &&
                   !(mover->flags & MOVER_AT_WALL)) {
            across = *spd;
            across.y = 0.0F;
            if (njScalor(&across) < REST_SPEED) {
                spd->x = 0.0F;
                spd->y = 0.0F;
                spd->z = 0.0F;
                mover->flags |= MOVER_RESTING;
                return;
            }
        }
    }
    pos->x += spd->x;
    pos->y += spd->y;
    pos->z += spd->z;
}

void mover_set_spd(struct task *task, NJS_VECTOR *spd)
{
    struct mover *mover = task->motion.p;

    mover->spd = *spd;
}

void mover_clear_spd(struct task *task)
{
    struct mover *mover = task->motion.p;

    mover->spd.x = 0.0F;
    mover->spd.y = 0.0F;
    mover->spd.z = 0.0F;
}

void mover_set_acc(struct task *task, NJS_VECTOR *acc)
{
    struct mover *mover = task->motion.p;

    mover->acc = *acc;
}

void mover_clear_acc(struct task *task)
{
    struct mover *mover = task->motion.p;
    NJS_VECTOR *acc = &mover->acc;

    acc->x = 0.0F;
    acc->y = 0.0F;
    acc->z = 0.0F;
}

void mover_set_gravity(struct task *task, Float gravity)
{
    struct mover *mover = task->motion.p;

    mover->gravity = gravity;
}

void mover_set_hold_turn(struct task *task, Sint32 value)
{
    struct mover *mover = task->motion.p;

    mover->hold_turn = value;
}

void mover_set_home(struct task *task)
{
    struct mover *mover = task->motion.p;
    NJS_POINT3 *pos = &task->work->pos;

    mover->home = *pos;
}

void mover_save_pos(struct task *task)
{
    struct mover *mover = task->motion.p;
    NJS_POINT3 *pos = &task->work->pos;

    mover->last_pos = *pos;
}

void mover_return_home(struct task *task)
{
    struct mover *mover = task->motion.p;
    NJS_POINT3 *pos = &task->work->pos;

    *pos = mover->home;
}

void mover_restore_pos(struct task *task)
{
    struct mover *mover = task->motion.p;
    NJS_POINT3 *pos = &task->work->pos;

    *pos = mover->last_pos;
}

void mover_set_turn_speed(struct task *task, Angle speed)
{
    struct mover *mover = task->motion.p;

    mover->turn_speed = speed;
}

void mover_set_target(struct task *task, NJS_POINT3 *target)
{
    struct mover *mover = task->motion.p;

    mover->target = *target;
}

Float mover_target_distance(struct task *task)
{
    /* @hack spelling: the mover taken through a cast of task->motion, where
     * a local of it would do */
    NJS_POINT3 *point = &((struct mover *)task->motion.p)->target;
    NJS_POINT3 *pos = &task->work->pos;
    /* @hack spelling: squared, a local for the sum, where passing the sum
     * to njSqrt would do */
    Float squared;

#ifdef NONMATCHING
    /* summed by distance2, as SHC's FMACs round the sum */
    squared = distance2(point, pos);
#else
    squared = ((point->x - pos->x) * (point->x - pos->x)) +
              ((point->y - pos->y) * (point->y - pos->y)) +
              ((point->z - pos->z) * (point->z - pos->z));
#endif
    return njSqrt(squared);
}

Float mover_target_distance2(struct task *task)
{
    struct mover *mover = task->motion.p;
    NJS_POINT3 *point = &mover->target;
    NJS_POINT3 *pos = &task->work->pos;

#ifdef NONMATCHING
    /* summed by distance2, as SHC's FMACs round the sum */
    return distance2(point, pos);
#else
    return ((point->x - pos->x) * (point->x - pos->x)) +
           ((point->y - pos->y) * (point->y - pos->y)) +
           ((point->z - pos->z) * (point->z - pos->z));
#endif
}

Angle mover_target_heading(struct task *task)
{
    struct task_work *work = task->work;
    struct mover *mover = task->motion.p;
    NJS_POINT3 *pos = &work->pos;
    NJS_POINT3 *point = &mover->target;

    return (Angle)(atan2f(point->x - pos->x, point->z - pos->z) *
                   ANGLE_PER_RADIAN);
}

Angle mover_turn_to_target(struct task *task)
{
    struct task_work *work = task->work;
    struct mover *mover = task->motion.p;

    mover->aim_y = mover_target_heading(task);
    work->ang.y = angle_approach(work->ang.y, mover->aim_y, mover->turn_speed);
    return angle_distance(mover->aim_y, work->ang.y);
}

Angle mover_turn_to_aim(struct task *task, Angle step)
{
    struct mover *mover = task->motion.p;
    struct task_work *work = task->work;

    work->ang.y = angle_approach(work->ang.y, mover->aim_y, step);
    return angle_distance(mover->aim_y, work->ang.y);
}

Angle mover_turn_to_target_by(struct task *task, Angle step)
{
    struct task_work *work = task->work;
    struct mover *mover = task->motion.p;

    mover->aim_y = mover_target_heading(task);
    work->ang.y = angle_approach(work->ang.y, mover->aim_y, step);
    return angle_distance(mover->aim_y, work->ang.y);
}

Float mover_home_distance(struct task *task)
{
    /* @hack spelling: the mover taken through a cast of task->motion, where
     * a local of it would do */
    NJS_POINT3 *point = &((struct mover *)task->motion.p)->home;
    NJS_POINT3 *pos = &task->work->pos;
    /* @hack spelling: squared, a local for the sum, where passing the sum
     * to njSqrt would do */
    Float squared;

#ifdef NONMATCHING
    /* summed by distance2, as SHC's FMACs round the sum */
    squared = distance2(point, pos);
#else
    squared = ((point->x - pos->x) * (point->x - pos->x)) +
              ((point->y - pos->y) * (point->y - pos->y)) +
              ((point->z - pos->z) * (point->z - pos->z));
#endif
    return njSqrt(squared);
}

Float mover_home_distance2(struct task *task)
{
    struct mover *mover = task->motion.p;
    NJS_POINT3 *point = &mover->home;
    NJS_POINT3 *pos = &task->work->pos;

#ifdef NONMATCHING
    /* summed by distance2, as SHC's FMACs round the sum */
    return distance2(point, pos);
#else
    return ((point->x - pos->x) * (point->x - pos->x)) +
           ((point->y - pos->y) * (point->y - pos->y)) +
           ((point->z - pos->z) * (point->z - pos->z));
#endif
}

static Angle mover_home_heading(struct task *task)
{
    struct task_work *work = task->work;
    struct mover *mover = task->motion.p;
    NJS_POINT3 *pos = &work->pos;
    NJS_POINT3 *point = &mover->home;

    return (Angle)(atan2f(point->x - pos->x, point->z - pos->z) *
                   ANGLE_PER_RADIAN);
}

Angle mover_turn_to_home(struct task *task)
{
    struct task_work *work = task->work;
    struct mover *mover = task->motion.p;

    mover->aim_y = mover_home_heading(task);
    work->ang.y = angle_approach(work->ang.y, mover->aim_y, mover->turn_speed);
    return angle_distance(mover->aim_y, work->ang.y);
}

Angle mover_turn_to_home_by(struct task *task, Angle step)
{
    struct task_work *work = task->work;
    struct mover *mover = task->motion.p;

    mover->aim_y = mover_home_heading(task);
    work->ang.y = angle_approach(work->ang.y, mover->aim_y, step);
    return angle_distance(mover->aim_y, work->ang.y);
}

int mover_find_player(struct task *task)
{
    struct task_work *work = task->work;
    struct mover *mover = task->motion.p;
    /* @hack spelling: other, a local for the slot's work, where the slot
     * tested in place would do */
    struct task_work *other;
    Float distance;
    int off;
    int player;

    for (player = 0; player < GAME_PLAYERS; player++) {
        other = player_work[player];
        if (other) {
            distance = mover_player_distance(task, player);
            if (distance < mover->sight_distance) {
                /* @hack type: the distance cast to Uint16, which holds each
                 * distance angle_distance returns */
                off = (Uint16)angle_distance(mover_player_heading(task, player),
                                             work->ang.y);
                if (off < mover->sight_angle) {
                    return player;
                }
            }
        }
    }
    return 0;
}

static Float mover_player_distance(struct task *task, int player)
{
    NJS_POINT3 *point = &player_work[player]->pos;
    NJS_POINT3 *pos = &task->work->pos;
    /* @hack spelling: squared, a local for the sum, where passing the sum
     * to njSqrt would do */
    Float squared;

#ifdef NONMATCHING
    /* summed by distance2, as SHC's FMACs round the sum */
    squared = distance2(point, pos);
#else
    squared = ((point->x - pos->x) * (point->x - pos->x)) +
              ((point->y - pos->y) * (point->y - pos->y)) +
              ((point->z - pos->z) * (point->z - pos->z));
#endif
    return njSqrt(squared);
}

Float mover_player_distance2(struct task *task, int player)
{
    NJS_POINT3 *point = &player_work[player]->pos;
    NJS_POINT3 *pos = &task->work->pos;

#ifdef NONMATCHING
    /* summed by distance2, as SHC's FMACs round the sum */
    return distance2(point, pos);
#else
    return ((point->x - pos->x) * (point->x - pos->x)) +
           ((point->y - pos->y) * (point->y - pos->y)) +
           ((point->z - pos->z) * (point->z - pos->z));
#endif
}

Angle mover_player_heading(struct task *task, int player)
{
    struct task_work *work = task->work;
    struct task_work *other = player_work[player];
    /* @hack redundant: mover, a local nothing reads */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-variable) */
    struct mover *mover = task->motion.p;
    NJS_POINT3 *pos = &work->pos;
    NJS_POINT3 *point = &other->pos;

    return (Angle)(atan2f(point->x - pos->x, point->z - pos->z) *
                   ANGLE_PER_RADIAN);
}

Angle mover_turn_to_player(struct task *task, int player)
{
    struct task_work *work = task->work;
    struct mover *mover = task->motion.p;

    mover->aim_y = mover_player_heading(task, player);
    work->ang.y = angle_approach(work->ang.y, mover->aim_y, mover->turn_speed);
    return angle_distance(mover->aim_y, work->ang.y);
}

Angle mover_turn_to_player_by(struct task *task, Angle step, int player)
{
    struct task_work *work = task->work;
    struct mover *mover = task->motion.p;

    mover->aim_y = mover_player_heading(task, player);
    work->ang.y = angle_approach(work->ang.y, mover->aim_y, step);
    return angle_distance(mover->aim_y, work->ang.y);
}

static void mover_meet_floor(struct task *task)
{
    struct task_work *work = task->work;
    struct mover *mover = task->motion.p;
    struct surface_side *floor = &mover->probe.sides[SURFACE_NEG_Y];
    NJS_VECTOR normal;
    Float floor_y;
    Float bottom_y;
    Float scale;

    bottom_y = work->pos.y + mover->probe_height + mover->bottom;
    floor_y = floor->pos;
    mover->flags &= ~MOVER_ON_FLOOR;
    if (bottom_y < floor_y) {
        work->pos.y += floor_y - bottom_y;
        if (!(mover->flags & MOVER_SLOPE_BOUNCE) ||
            floor->normal.y > LEVEL_FLOOR) {
            mover->spd.y -= mover->spd.y * mover->floor_bounce * 2.0F;
        } else {
            normal = floor->normal;
            scale = -(njInnerProduct(&mover->spd, &normal) *
                      mover->floor_bounce * 2.0F);
            normal.x *= scale;
            normal.y *= scale;
            normal.z *= scale;
            njAddVector(&mover->spd, &normal);
        }
        if (!(mover->flags & MOVER_SLOPE_BOUNCE)) {
            mover->spd.x *= mover->friction;
            mover->spd.z *= mover->friction;
        }
        mover->flags &= ~MOVER_SLOPE_BOUNCE;
        work->flags |= MOVER_WORK_ON_FLOOR;
        mover->flags |= MOVER_ON_FLOOR;
    }
}

/* push the work along wall's normal, across the ground, by WALL_UNDONE of
 * overlap. where the mover heads into the wall, keep its plane in
 * facing_wall and set MOVER_FACING_WALL. where spd moves into the wall,
 * reflect spd off it by wall_bounce. the macro uses the locals of MEET_WALL
 * and mover_meet_walls */
#define PUSH_FROM_WALL()                                                       \
    do {                                                                       \
        push.x = wall->normal.x;                                               \
        push.y = 0.0F;                                                         \
        push.z = wall->normal.z;                                               \
        push.x *= overlap * WALL_UNDONE;                                       \
        push.y *= overlap * WALL_UNDONE;                                       \
        push.z *= overlap * WALL_UNDONE;                                       \
        njAddVector(&work->pos, &push);                                        \
        mover->flags |= MOVER_AT_WALL;                                         \
        if (njInnerProduct(&wall->normal, &heading) < HEADING_INTO_WALL) {     \
            mover->facing_wall = plane;                                        \
            mover->flags |= MOVER_FACING_WALL;                                 \
        }                                                                      \
        if (njInnerProduct(&mover->spd, &wall->normal) < 0.0F) {               \
            reflect = wall->normal;                                            \
            if (njUnitVector(&reflect) == 0.0F) {                              \
                reflect.x = 1.0F;                                              \
                reflect.y = 0.0F;                                              \
                reflect.z = 0.0F;                                              \
            }                                                                  \
            scale = -(njInnerProduct(&mover->spd, &reflect) *                  \
                      mover->wall_bounce * 2.0F);                              \
            reflect.x *= scale;                                                \
            reflect.y *= scale;                                                \
            reflect.z *= scale;                                                \
            njAddVector(&mover->spd, &reflect);                                \
        }                                                                      \
    } while (0)

/* meet the wall the probe found on side, if any. (dir_x, 0, dir_z) points
 * toward the side. the wall's plane passes through (plane_x, point.y,
 * plane_z), and plane_x and plane_z may read wall. a wall that faces the
 * point pushes the work out of its overlap with radius. a wall whose back
 * faces the point pushes the work through to its front. the macro uses
 * mover_meet_walls' locals */
#define MEET_WALL(side, dir_x, dir_z, plane_x, plane_z)                        \
    do {                                                                       \
        if (probe->sides[side].found) {                                        \
            NJS_VECTOR dir = {dir_x, 0.0F, dir_z};                             \
            NJS_POINT3 nearest;                                                \
            NJS_VECTOR push;                                                   \
            /* @hack redundant: spare, a local that only sizes the frame */    \
            NJS_VECTOR spare; /* NOLINT(clang-diagnostic-unused-variable) */   \
            NJS_VECTOR reflect;                                                \
                                                                               \
            wall = &probe->sides[side];                                        \
            along = njInnerProduct(&dir, &wall->normal);                       \
            if (along < -WALL_FACING) {                                        \
                plane.px = plane_x;                                            \
                plane.py = point.y;                                            \
                plane.pz = plane_z;                                            \
                plane.vx = wall->normal.x;                                     \
                plane.vy = wall->normal.y;                                     \
                plane.vz = wall->normal.z;                                     \
                overlap = radius - njDistanceP2PL(&point, &plane, &nearest);   \
                if (overlap > 0.0F) {                                          \
                    PUSH_FROM_WALL();                                          \
                }                                                              \
            } else if (along > WALL_FACING) {                                  \
                plane.px = plane_x;                                            \
                plane.py = point.y;                                            \
                plane.pz = plane_z;                                            \
                plane.vx = wall->normal.x;                                     \
                plane.vy = wall->normal.y;                                     \
                plane.vz = wall->normal.z;                                     \
                overlap = radius + njDistanceP2PL(&point, &plane, &nearest);   \
                if (overlap < BEHIND_WALL_REACH) {                             \
                    PUSH_FROM_WALL();                                          \
                }                                                              \
            }                                                                  \
        }                                                                      \
    } while (0)

static void mover_meet_walls(struct task *task)
{
    struct task_work *work = task->work;
    struct mover *mover = task->motion.p;
    struct mover_probe *probe = &mover->probe;
    Float radius = mover->radius;
    NJS_POINT3 point;
    NJS_PLANE plane;
    NJS_VECTOR heading;
    struct surface_side *wall;
    Float along;
    Float overlap;
    Float scale;

    heading.x = njSin(work->ang.y);
    heading.y = 0.0F;
    heading.z = njCos(work->ang.y);
    point = probe->pos;
    /* @hack spelling: the two flags cleared one at a time, where one clear
     * of both would do */
    mover->flags &= ~MOVER_AT_WALL;
    mover->flags &= ~MOVER_FACING_WALL;
    /* @hack spelling: the meeting expanded for each of the four sides, where
     * a function called for each would do */
    MEET_WALL(SURFACE_NEG_X, -1.0F, 0.0F, wall->pos, point.z);
    MEET_WALL(SURFACE_POS_X, 1.0F, 0.0F, wall->pos, point.z);
    MEET_WALL(SURFACE_NEG_Z, 0.0F, -1.0F, point.x, wall->pos);
    MEET_WALL(SURFACE_POS_Z, 0.0F, 1.0F, point.x, wall->pos);
}

static void mover_probe_stage(struct task *task)
{
    struct mover *mover = task->motion.p;
    struct mover_probe *probe;
    Uint16 i;
    struct world_object *objects;
    NJS_VECTOR to_object;
    /* @hack type: object_probe declared a mover_probe, where the
     * surface_probe that polygon_find_sides fills would do */
    struct mover_probe object_probe;
    NJS_OBJECT *object;
    Float distance;
    Float scale;
    /* @hack spelling: reach, a local for the object's reach, where the
     * product compared in place would do */
    Float reach;
    Float current;
    Float candidate;
    int side;
    NJS_POINT3 *pos;

    mover->flags &= ~MOVER_FLAT_FOUND;
    probe = &mover->probe;
    pos = &probe->pos;
    probe->pos = task->work->pos;
    probe->pos.y += mover->probe_height;
    object_probe.pos = probe->pos;
    for (side = 0; side < SURFACE_SIDES; side++) {
        probe->sides[side] = probe->initial[side];
        probe->sides[side].found = 0;
    }
    probe->sides[SURFACE_NEG_X].pos = NO_FLOOR;
    probe->sides[SURFACE_POS_X].pos = -NO_FLOOR;
    probe->sides[SURFACE_NEG_Y].pos = NO_FLOOR;
    probe->sides[SURFACE_POS_Y].pos = -NO_FLOOR;
    probe->sides[SURFACE_NEG_Z].pos = NO_FLOOR;
    probe->sides[SURFACE_POS_Z].pos = -NO_FLOOR;
    mover->flat_y = NO_FLAT_FLOOR;
    objects = world_objects;
    world_list_near(pos->x, pos->y, pos->z, PROBE_RADIUS);
    for (i = 0; i < world_count; i++) {
        object = objects[i].object;
        if (objects[i].owner) {
            to_object.x = object->pos[0] - pos->x;
            to_object.y = object->pos[1] - pos->y;
            to_object.z = object->pos[2] - pos->z;
            distance = njScalor(&to_object);
            scale = object->scl[0];
            if (object->scl[1] > scale) {
                scale = object->scl[1];
            }
            if (object->scl[2] > scale) {
                scale = object->scl[2];
            }
            reach = scale * object->model->r;
            if (distance > reach) {
                continue;
            }
        }
        /* @hack spelling: the object read from the list again, where object
         * would do */
        if (polygon_find_sides((struct surface_probe *)&object_probe,
                               objects[i].object, objects[i].flags)) {
            if (object_probe.sides[SURFACE_NEG_X].found) {
                current = njAbs(pos->x - probe->sides[SURFACE_NEG_X].pos);
                candidate =
                    njAbs(pos->x - object_probe.sides[SURFACE_NEG_X].pos);
                if (current > candidate) {
                    probe->sides[SURFACE_NEG_X] =
                        object_probe.sides[SURFACE_NEG_X];
                }
            }
            if (object_probe.sides[SURFACE_POS_X].found) {
                if (!(objects[i].flags & WORLD_FLAT)) {
                    current = njAbs(probe->sides[SURFACE_POS_X].pos - pos->x);
                    candidate =
                        njAbs(object_probe.sides[SURFACE_POS_X].pos - pos->x);
                    if (current > candidate) {
                        probe->sides[SURFACE_POS_X] =
                            object_probe.sides[SURFACE_POS_X];
                    }
                }
            }
            if (object_probe.sides[SURFACE_NEG_Y].found) {
                if (objects[i].flags & WORLD_FLAT) {
                    mover->flat_y = object_probe.sides[SURFACE_NEG_Y].pos;
                    mover->flags |= MOVER_FLAT_FOUND;
                } else {
                    current = njAbs(pos->y - probe->sides[SURFACE_NEG_Y].pos);
                    candidate =
                        njAbs(pos->y - object_probe.sides[SURFACE_NEG_Y].pos);
                    if (current > candidate) {
                        probe->sides[SURFACE_NEG_Y] =
                            object_probe.sides[SURFACE_NEG_Y];
                    }
                }
            }
            if (object_probe.sides[SURFACE_POS_Y].found) {
                if (objects[i].flags & WORLD_FLAT) {
                    /* @bug logic: a flat object's surface above sets flat_y
                     * to the object's surface below, which polygon_find_sides
                     * need not have found. flat_y is then 220 above the
                     * probe's point. the FMOV.S at 0x8c0352b8 reads the side
                     * below, as the one at 0x8c035246 does */
                    mover->flat_y = object_probe.sides[SURFACE_NEG_Y].pos;
                    mover->flags |= MOVER_FLAT_FOUND;
                } else {
                    current = njAbs(probe->sides[SURFACE_POS_Y].pos - pos->y);
                    candidate =
                        njAbs(object_probe.sides[SURFACE_POS_Y].pos - pos->y);
                    if (current > candidate) {
                        probe->sides[SURFACE_POS_Y] =
                            object_probe.sides[SURFACE_POS_Y];
                    }
                }
            }
            if (object_probe.sides[SURFACE_NEG_Z].found) {
                current = njAbs(pos->z - probe->sides[SURFACE_NEG_Z].pos);
                candidate =
                    njAbs(pos->z - object_probe.sides[SURFACE_NEG_Z].pos);
                if (current > candidate) {
                    probe->sides[SURFACE_NEG_Z] =
                        object_probe.sides[SURFACE_NEG_Z];
                }
            }
            if (object_probe.sides[SURFACE_POS_Z].found) {
                current = njAbs(probe->sides[SURFACE_POS_Z].pos - pos->z);
                candidate =
                    njAbs(object_probe.sides[SURFACE_POS_Z].pos - pos->z);
                if (current > candidate) {
                    probe->sides[SURFACE_POS_Z] =
                        object_probe.sides[SURFACE_POS_Z];
                }
            }
        }
    }
}

void mover_meet_stage(struct task *task)
{
    struct mover *mover = task->motion.p;
    struct task_work *work = task->work;

    if (!(mover->flags & MOVER_RESTING)) {
        work->flags &= ~MOVER_WORK_ON_FLOOR;
        mover->floor_calls = 0;
        mover_probe_stage(task);
        mover_meet_floor(task);
        mover_meet_walls(task);
    }
}
