/* the collision objects of a stage's object list: SPHERE, CCYL, CCUBE, CWALL
 * and CCIRCLE. each is a shape that no model draws, sized by its placement.
 * the Chao stages' lists name the first four C_SPHERE, C_CYL, C_CUBE and
 * WALL_PLANE */
#ifndef SHARED_COLLISION_OBJECT_H
#define SHARED_COLLISION_OBJECT_H

struct task;

/* the object lists' steps. task needs a work, and the heap must have room
 * for the work's collision record, unchecked. the first step gives the work
 * the record, and each later step registers it. its one shape's radius,
 * size_y and size_z are 10 more than the work's scl.x, scl.y and scl.z.
 * CCIRCLE's step takes placement_try_leave_by at a range of 3000 in place of
 * placement_try_leave.
 *
 * the records of SPHERE, CCYL and CCUBE are of kind COLLISION_OBJECT. the
 * records of CWALL and CCIRCLE are of kind COLLISION_WALL */

/* SPHERE's step: a sphere */
void collision_sphere_step(struct task *task);
/* CCYL's step: an upright cylinder */
void collision_cylinder_step(struct task *task);
/* CCUBE's step: a box */
void collision_cube_step(struct task *task);
/* CWALL's step: a wall */
void collision_wall_step(struct task *task);
/* CCIRCLE's step: a SHAPE_UNK_10 shape */
void collision_circle_step(struct task *task);

#endif
