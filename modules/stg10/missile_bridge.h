/* the missile's bridges, MSBRILONG, MSBRISHORT, MSBRIMINI and MSBRISLOPE in
 * Metal Harbor's object list. the players walk on them. while the missile
 * lifts off, they draw nothing and bear no player */
#ifndef STG10_MISSILE_BRIDGE_H
#define STG10_MISSILE_BRIDGE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the bridges' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *missile_bridge_texlists[];

/* build each bridge's node at buf, as object_build of shared/common_object.h
 * does, with the bridge's texture list current. returns buf past the builds.
 * the bridges draw their models, not these builds */
Uint8 *missile_bridges_build(Uint8 *buf);

/* the object list's steps for MSBRILONG, MSBRISHORT, MSBRIMINI and MSBRISLOPE.
 * unless placement_try_leave(task) returns TRUE, each gives task a record in
 * task->motion. it also gives task the bridge's own step, draw and destructor.
 * where the heap can't give the record, the init stays task's step and tries
 * again on the next. the init puts a copy of the bridge's collision node in the
 * collision world. it gives task a collision of the bridge's rails: two boxes,
 * or six on the sloping bridge.
 *
 * task needs a work, whose pos places the bridge and whose ang.y turns it. the
 * world collects the copy only while a player is near. a player is near within
 * the node's radius plus 50 of its center. a bridge hides while
 * harbor_missile_task of stg10/harbor_missile.h has a timer above 44 in
 * HARBOR_MISSILE_MODE_LIFT_OFF or HARBOR_MISSILE_MODE_EMPTY. the world then
 * doesn't collect the copy, and the draw draws nothing. the rails' boxes still
 * collide */
void missile_bridge_long_init(struct task *task);
void missile_bridge_short_init(struct task *task);
void missile_bridge_mini_init(struct task *task);
void missile_bridge_slope_init(struct task *task);

#endif
