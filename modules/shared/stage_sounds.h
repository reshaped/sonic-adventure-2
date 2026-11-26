/* the stages' sound objects: a siren, SE_KAZE's wind and SE_TOPPU's gust. Big
 * Foot, Mission Street and Mission Street's two-player stage hold them */
#ifndef SHARED_STAGE_SOUNDS_H
#define SHARED_STAGE_SOUNDS_H

struct task;

/* the first step of a siren. it plays sound 7 of the stage's list at its work's
 * pos, keyed by the work. its pitch rises as the first camera nears it, and
 * falls as the camera draws away. a siren whose work's submode is 0 ends once
 * placement_try_leave(task) finds it out of range */
void siren_init(struct task *task);
/* the object list's step for SE_KAZE, a wind in a box. the box is centered on
 * the work's pos and turned by its ang.y. it reaches the work's scl from its
 * center along each axis. the wind is sound 15 of the stage's list. while
 * player 0 lies inside the box, each step plays it by a chance of 3 in 100.
 * it plays at a random point within 265 of the player along each axis */
void wind_sound_init(struct task *task);
/* the object list's step for SE_TOPPU, a gust. it plays sound 16 of the stage's
 * list at the work's pos, keyed by the task */
void gust_sound_init(struct task *task);

#endif
