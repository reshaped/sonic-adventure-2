/* the gravity cylinders, FCG_CYL0 and FCG_CYL1 in Final Chase's object list,
 * and FCG_CYL_PARTS and FCG_CYL_SPARK */
#ifndef STG40_GRAVITY_CYLINDER_H
#define STG40_GRAVITY_CYLINDER_H
#include <ninja.h>

struct task;

/* the objects' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *gravity_cylinder_texlists[];

/* the object list's steps for FCG_CYL0 and FCG_CYL1. task needs a work,
 * whose pos places the cylinder and whose ang.y and ang.z turn it. the low
 * byte of ang.y is the cylinder's number. ang.x is the field's turn about its
 * axis, in Angle units a step. scl.z is the field's outer radius.
 *
 * unless placement_try_leave(task) returns TRUE, each makes the cylinder's
 * field as task's child. FCG_CYL0 makes it again at any step that finds none.
 * FCG_CYL1's field takes a lid in the collision world as its own child.
 *
 * the field of the lowest number that player 0 lies in holds player 0 at the
 * next step. it points world_gravity at its axis, and carries player 0 round
 * the axis where player 0 touches it. a field reads player 0's work and its
 * collision record unchecked.
 *
 * the first field starts the task ManGCylExecutor, which outlives the
 * cylinders. it points world_gravity down again after each step in which
 * player 0 lay in no field */
void gravity_cylinder_0_step(struct task *task);
void gravity_cylinder_1_init(struct task *task);
/* the object list's step for FCG_CYL_PARTS. task needs a work, whose pos
 * places the parts and whose ang turns them. a capsule of the parts pushes
 * players, enemies and objects. the parts' step never calls
 * placement_try_leave, so they stay once set up */
void gravity_cylinder_parts_init(struct task *task);
/* the object list's step for FCG_CYL_SPARK. task needs a work, whose pos
 * places the spark and whose ang turns it. while a player is within 150 of
 * pos, ten children of task stand at the spark's points. each is a sphere of
 * radius 12, which hurts players, enemies and objects. each step plays sound 1
 * of the stage's sound list at pos. the spark's step never calls
 * placement_try_leave either */
void gravity_cylinder_spark_init(struct task *task);

#endif
