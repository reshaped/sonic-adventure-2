/* a node's aim: one node of a chunk model, turned toward a player or a point
 * within limits. Big Foot, Hot Shot and Flying Dog aim their parts with it */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "player/player.h"
#include "shared/node_aim.h"
#include "system/angle.h"
#include "system/task.h"

/* ang is the aim's own turn. node_aim_point and node_aim_return write it to
 * node->ang, and node_aim_player doesn't */
struct node_aim {
    struct angles ang;
    struct angles base; /* node's angles as node_aim_create found them */
    struct node_aim_limits limits;
    NJS_POINT3 *pivot;
    Uint32 unk_40;
    NJS_CNK_OBJECT *node;
};

static void node_aim_apply(struct node_aim *aim);

struct node_aim *node_aim_create(NJS_CNK_OBJECT *node, NJS_POINT3 *pivot)
{
    struct node_aim *aim;

    if (node == NULL) {
        return NULL;
    }
    aim = syCalloc(1, sizeof(struct node_aim));
    if (aim == NULL) {
        return NULL;
    }
    aim->node = node;
    aim->pivot = pivot;
    aim->base.x = aim->node->ang[0];
    aim->base.y = aim->node->ang[1];
    aim->base.z = aim->node->ang[2];
    return aim;
}

void node_aim_free(struct node_aim *aim)
{
    syFree(aim);
}

void node_aim_set_limits(struct node_aim *aim, struct node_aim_limits *limits)
{
    /* @hack spelling: the limits copied a record at a time, where one copy
     * of the whole would do */
    aim->limits.speed = limits->speed;
    aim->limits.upper = limits->upper;
    aim->limits.lower = limits->lower;
}

void node_aim_player(struct task *task, struct node_aim *aim, Sint8 player)
{
    struct task_work *work;
    struct task_work *target;
    NJS_POINT3 pivot;
    /* @hack redundant: world, the aim's angles in the world, which nothing
     * reads */
    struct angles world;
    Angle relative;
    Angle goal;
    Angle ang;

    work = task->work;
    target = player_work[player];
    /* @hack spelling: the point copied member by member, where pivot =
     * *aim->pivot would do */
    pivot.x = aim->pivot->x;
    pivot.y = aim->pivot->y;
    pivot.z = aim->pivot->z;
    world.x = work->ang.x + aim->ang.x;
    world.y = work->ang.y + aim->ang.y;
    world.z = work->ang.z + aim->ang.z;
    /* @hack spelling: the goal built in three statements, where one
     * expression would do */
    relative =
        (Angle)(atan2f(pivot.x - target->pos.x, pivot.z - target->pos.z) *
                ANGLE_PER_RADIAN) -
        QUARTER_TURN;
    relative -= work->ang.y;
    goal = relative - QUARTER_TURN;
    ang = angle_approach(aim->ang.y, goal, aim->limits.speed.y);
    if (ang > HALF_TURN) {
        ang -= FULL_TURN;
    }
    if (ang < aim->limits.lower.y) {
        aim->ang.y = aim->limits.lower.y;
    } else if (ang > aim->limits.upper.y) {
        aim->ang.y = aim->limits.upper.y;
    } else {
        aim->ang.y = ang;
    }
    /* @hack spelling: the goal built in three statements, where one
     * expression would do */
    relative =
        (Angle)(atan2f(pivot.x - target->pos.x, pivot.y - target->pos.y) *
                ANGLE_PER_RADIAN) -
        QUARTER_TURN;
    relative -= work->ang.z;
    goal = -relative;
    ang = angle_approach(aim->ang.z, goal, aim->limits.speed.z);
    if (ang > HALF_TURN) {
        ang -= FULL_TURN;
    }
    if (ang < aim->limits.lower.z) {
        aim->ang.z = aim->limits.lower.z;
    } else if (ang > aim->limits.upper.z) {
        aim->ang.z = aim->limits.upper.z;
    } else {
        aim->ang.z = ang;
    }
}

void node_aim_point(struct task *task, struct node_aim *aim, Float range,
                    NJS_POINT3 *target)
{
    /* @hack redundant: copy declared register, where a plain local would do */
    /* @hack spelling: copy, a copy of aim, where aim itself would do */
    register struct node_aim *copy;
    struct task_work *work;
    NJS_VECTOR to_target;
    Angle yaw;
    Angle turn;
    Angle pitch;
    Float distance;

    work = task->work;
    copy = aim;
    to_target.x = target->x - copy->pivot->x;
    to_target.y = target->y - copy->pivot->y;
    to_target.z = target->z - copy->pivot->z;
    yaw = copy->node->ang[1];
    turn = angle_difference(
        work->ang.y + yaw,
        (Angle)(atan2f(to_target.x, to_target.z) * ANGLE_PER_RADIAN));
    if (turn > copy->limits.speed.y) {
        turn = copy->limits.speed.y;
    } else if (turn < -copy->limits.speed.y) {
        turn = -copy->limits.speed.y;
    }
    yaw += turn;
    if (yaw > copy->limits.upper.y) {
        yaw = copy->limits.upper.y;
    } else if (yaw < copy->limits.lower.y) {
        yaw = copy->limits.lower.y;
    }
    copy->ang.y = yaw;
    distance = njScalor(&to_target);
    if (distance > range) {
        pitch = 0;
    } else {
        turn = (Angle)(atan2f(copy->pivot->y, distance) * ANGLE_PER_RADIAN);
        if (turn > copy->limits.upper.z) {
            pitch = copy->limits.upper.z;
        } else {
            pitch = turn;
        }
    }
    pitch = angle_approach(copy->node->ang[2], pitch, copy->limits.speed.z);
    copy->ang.z = pitch;
    copy->ang.x = 0;
    node_aim_apply(aim);
}

Bool node_aim_return(struct node_aim *aim)
{
    /* @hack spelling: copy, a copy of aim, where aim itself would do */
    struct node_aim *copy;

    copy = aim;
    copy->ang.x =
        angle_approach(copy->ang.x, copy->base.x, copy->limits.speed.x);
    copy->ang.y =
        angle_approach(copy->ang.y, copy->base.y, copy->limits.speed.y);
    copy->ang.z =
        angle_approach(copy->ang.z, copy->base.z, copy->limits.speed.z);
    node_aim_apply(aim);
    return copy->ang.x == 0 && copy->ang.y == 0 && copy->ang.z == 0;
}

static void node_aim_apply(struct node_aim *aim)
{
    aim->node->ang[0] = aim->ang.x;
    aim->node->ang[1] = aim->ang.y;
    aim->node->ang[2] = aim->ang.z;
}

void node_aim_set(struct node_aim *aim, struct angles *ang)
{
    aim->node->ang[0] = ang->x;
    aim->node->ang[1] = ang->y;
    if (ang->z > aim->limits.upper.z) {
        aim->node->ang[2] = aim->limits.upper.z;
    } else if (ang->z < aim->limits.lower.z) {
        aim->node->ang[2] = aim->limits.lower.z;
    } else {
        aim->node->ang[2] = ang->z;
    }
}

void node_aim_reset(struct node_aim *aim)
{
    aim->node->ang[0] = aim->base.x;
    aim->node->ang[1] = aim->base.y;
    aim->node->ang[2] = aim->base.z;
}
