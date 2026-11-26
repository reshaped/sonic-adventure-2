/* the Egg Golem, the boss of BOSS_GOLEM.PRS */
#ifndef BOSS_GOLEM_GOLEM_H
#define BOSS_GOLEM_GOLEM_H
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_golem/head_parts.h"
#include "shared/golem.h"

struct task;

/* create the golem's task BossGolemExec at *pos, turned to ang_y, and return
 * it. the task's mover takes *home as its home. the call allocates golem at
 * full power. it creates the platforms, the head's parts, the hands, the flash,
 * the weak spot and the sensors. it puts player 0 on the floor, 210 along z
 * from the origin. the golem waits for the fight to start, under its gauge and
 * title.
 *
 * the call returns null where the heap can't give the task or golem. where
 * the heap can't give a hand, the flash or the weak spot, it returns null. it
 * then leaves the task to step, unfinished. where any other allocation fails,
 * the call or a later step writes through null. the non-matching side instead
 * destroys the task and returns null wherever an allocation fails */
struct task *golem_create(NJS_POINT3 *pos, Angle ang_y, NJS_POINT3 *home);

/* place object, task's object in the collision world, where the current matrix
 * puts it in the world's axes. the call takes the view out by the inverse that
 * the golem's draw keeps. it must run inside that draw. *base takes what
 * *matrix held, and *matrix takes the new place.
 *
 * while neither player lies within range + 15 of the place, the call clears
 * WORLD_ADDED in task's work. the world then passes the object over. otherwise
 * it sets WORLD_ADDED, and task's force takes the move and the turn from *base
 * to *matrix. the move is the one that carries player 0's pos with the object.
 * task must have a force, and player 0 a work, unchecked */
void golem_sensor_place(struct task *task, NJS_OBJECT *object,
                        NJS_MATRIX *matrix, Float range, NJS_MATRIX *base);

/* the golem's state, from golem_create on, or null */
extern struct golem *golem;
/* the golem's texture lists, ending in null. the stage's groups of shared
 * texture lists hold them */
extern NJS_TEXLIST *golem_texlists[];

/* golem_sensor_matrices holds each sensor's place in the world's axes, as
 * golem_sensor_place last set it. golem_sensor_bases holds the place before
 * that, and golem_head_matrices the places of the head's parts */
extern NJS_MATRIX golem_sensor_matrices[GOLEM_SENSORS];
extern NJS_MATRIX golem_sensor_bases[GOLEM_SENSORS];
extern NJS_MATRIX golem_head_matrices[GOLEM_HEAD_PARTS];
/* the hands' and the flash's points in the world's axes, as the golem's draw
 * last found them */
extern NJS_POINT3 golem_hand_a_pos;
extern NJS_POINT3 golem_hand_b_pos;
extern NJS_POINT3 golem_flash_pos;

#endif
