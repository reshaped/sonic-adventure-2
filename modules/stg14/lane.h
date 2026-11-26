/* the lanes, SLANE in Radical Highway's object list: stretches of road that
 * crack and fall, tilt or swing. the quakes, SCOL there, set them off */
#ifndef STG14_LANE_H
#define STG14_LANE_H
#include <ninja.h>

struct task;

/* the lanes' texture lists for texture_load_shared, whole and cracked, ended
 * by null */
extern NJS_TEXLIST *highway_lane_texlists[];

/* the object list's step for SCOL, a quake. unless placement_try_leave(task)
 * returns TRUE, it gives task the quake's own step, draw and destructor. it
 * also lists the quake under its id, unless one is listed there already. where
 * the heap can't give the list, it lists none.
 *
 * task needs a work. the low four bits of ang.x are the quake's id. a player
 * within scl.x plus 10 of pos stands in the quake. while a lane of its id
 * cracks, falls or swings, the quake shakes the first camera. so does a lane
 * tilting up to its limit. the shake lasts 60 steps past the last such step.
 * the quake rumbles the pad of the last player to stand in it. its step does
 * nothing while game_state.time_stopped isn't 0 */
void highway_lane_quake_init(struct task *task);
/* the object list's step for SLANE, a lane. unless placement_try_leave(task)
 * returns TRUE, it gives task a record in task->motion. it also gives task the
 * lane's own step, draw and destructor. it puts a copy of the lane's collision
 * object in the collision world too. where the heap can't give the record,
 * this stays task's step and tries again on the next.
 *
 * task needs a work and TASK_FORCE's records. the set file gives the lane
 * these settings in the work.
 * - pos places it, and ang.y turns it.
 * - the low four bits of ang.x: its quake's id. a lane whose id names no
 *   listed quake does nothing in its step.
 * - bit 8 of ang.x: its tilt or swing goes the other way.
 * - the low four bits of ang.z, modulo 6: its type. types 0, 1 and 4 crack
 *   and fall, 2 tilts, 3 swings and 5 stays.
 * - scl.y: the height it falls. a lane of a scl.y above 0 lands and stays.
 *   one of 0 or below breaks and fades out.
 *
 * a lane starts once a player stands in its quake. a lane whose placement
 * keeps a state other than 0 starts where its fall ends. it stays there, or
 * fades out. a riding player moves with the lane by TASK_FORCE's records. while
 * game_state.time_stopped isn't 0, the lane neither moves nor shakes. the world
 * collects its copy only while player_near finds a player within the copy's
 * radius plus 50 */
void highway_lane_init(struct task *task);

#endif
