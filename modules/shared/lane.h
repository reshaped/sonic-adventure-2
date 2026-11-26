/* the lanes, SLANE and SLANE2 in Mission Street's object list. a lane is a
 * stretch of road that cracks and falls, tilts or swings. the quakes, SCOL
 * there, set the lanes off */
#ifndef SHARED_LANE_H
#define SHARED_LANE_H
#include <ninja.h>

struct task;

/* the lanes' texture lists for texture_load_shared, whole and cracked, ended
 * by null */
extern NJS_TEXLIST *lane_texlists[];

/* the object list's step for SCOL, a quake. task needs a work. its pos
 * places the quake, and the low byte of its ang.x, modulo 17, is its id. the
 * step does nothing where placement_try_leave(task) returns TRUE. otherwise
 * it gives task the quake's step, draw and destructor. it also lists the
 * quake under its id, unless one is listed there already. where the heap
 * can't give the list, it lists none.
 *
 * a player within scl.x plus 10 of pos stands in the quake. while a lane of its
 * id cracks, falls or swings, the quake shakes the first camera. so does a
 * lane tilting up to its limit. the shake lasts 60 steps past the last such
 * step. once a player has stood in the quake, it and its lanes rumble player
 * 0's pad. its step does nothing while game_state.time_stopped isn't 0 */
void lane_quake_init(struct task *task);
/* the object list's steps for SLANE and SLANE2, the lanes. task needs a work
 * and TASK_FORCE's records. each step does nothing where
 * placement_try_leave(task) returns TRUE. otherwise it takes a record in
 * task->motion, and gives task the lane's step, draw and destructor. it puts
 * a copy of the lane's collision object in the collision world too. where
 * the heap can't give the record, task keeps this step to try again.
 *
 * the placement gives the lane these settings in the work.
 * - pos places it, and ang.y turns it. an SLANE2 lane's ang.z tilts it about
 *   x too.
 * - the low byte of ang.x, modulo 17: its quake's id. a lane whose id names no
 *   listed quake does nothing in its step.
 * - bit 8 of ang.x: its tilt or swing goes the other way.
 * - the low four bits of ang.z, modulo 6: an SLANE lane's type. an SLANE2
 *   lane takes bits 12 to 15 of ang.x instead. types 0, 1 and 4 crack and
 *   fall, 2 tilts, 3 swings and 5 stays.
 * - scl.y: the height it falls. a lane of a scl.y above 0 lands and stays.
 *   one of 0 or below breaks and fades out.
 *
 * a lane starts once a player stands in its quake. a lane whose placement
 * keeps a state other than 0 starts where its fall ends. it stays there, or
 * fades out. a riding player moves with the lane by TASK_FORCE's records */
void lane_init(struct task *task);
void lane_tilted_init(struct task *task);

#endif
