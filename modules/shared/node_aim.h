/* a node's aim: one node of a chunk model, turned toward a player or a point
 * within limits. Big Foot, Hot Shot and Flying Dog aim their parts with it */
#ifndef SHARED_NODE_AIM_H
#define SHARED_NODE_AIM_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

struct task;

/* how far an aim turns. speed is the most each of its angles turns in a call.
 * lower and upper hold the bounds that the calls below name, and no call reads
 * lower.x or upper.x */
struct node_aim_limits {
    struct angles speed;
    struct angles upper;
    struct angles lower;
};

struct node_aim;

/* make an aim of node, which turns about the point *pivot, with every limit 0.
 * the aim keeps node's angles as its base. it returns the aim, or null for a
 * null node or where the heap can't give one. *pivot must outlive the aim.
 * node_aim_free frees it */
struct node_aim *node_aim_create(NJS_CNK_OBJECT *node, NJS_POINT3 *pivot);
void node_aim_free(struct node_aim *aim);
void node_aim_set_limits(struct node_aim *aim, struct node_aim_limits *limits);
/* turn aim's ang.y and ang.z toward player_work[player]'s pos as the pivot sees
 * it, relative to task's work's angles. each turns by at most its speed and
 * stays within its lower and upper bound. the node's angles stay as they are.
 * player_work[player] must not be null. no code in the game's images calls
 * node_aim_player */
void node_aim_player(struct task *task, struct node_aim *aim, Sint8 player);
/* turn aim toward target, then write its angles to the node. each turn starts
 * from the node's angle, not aim's. ang.y turns toward target as the pivot sees
 * it, by at most speed.y. it stays within lower.y and upper.y. the node's y
 * angle counts from task's work's ang.y. ang.z turns by at most speed.z toward
 * atan(y / d). y is the pivot's own y, and d is target's distance from the
 * pivot. upper.z caps that angle, and no lower bound applies. where d passes
 * range, ang.z turns toward 0 instead. ang.x becomes 0 */
void node_aim_point(struct task *task, struct node_aim *aim, Float range,
                    NJS_POINT3 *target);
/* turn each of aim's angles toward its base by at most its speed, with no
 * bound. then write them to the node. it returns whether all three angles are
 * 0, not whether they reached the base */
Bool node_aim_return(struct node_aim *aim);
/* write ang to the node, its z kept within lower.z and upper.z. aim's own
 * angles stay as they are */
void node_aim_set(struct node_aim *aim, struct angles *ang);
/* write the base angles to the node. aim's own angles stay as they are */
void node_aim_reset(struct node_aim *aim);

#endif
