/* the dark story's Egg Golem, the boss of BOSS_GOLEM_E.PRS */
#ifndef BOSS_GOLEM_E_GOLEM_H
#define BOSS_GOLEM_E_GOLEM_H
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/golem.h"

struct task;

/* the push of the hands', the head's, the sensors' and the weak points'
 * shapes. it resists every group but the pickups, and pushes none */
#define GOLEM_E_PUSH_RESISTS PUSH_FIRM(PUSH_ALL_BUT_PICKUPS)
/* the guard level of the sensors' and the weak points' shapes */
#define GOLEM_E_GUARD_LEVEL 3

/* create the golem's task BossGolemExec at *pos, turned to ang_y, and return
 * it. the task's mover takes *home as its home. golem_e takes the golem's
 * state, at full power. the call creates the hands, the head and the sensors
 * as the task's children. golem_e_weak_points_create makes the weak points.
 * the call puts player 0 at (0, golem_e_floor_y, 210). the golem then waits for
 * the fight to start, under its gauge and title.
 *
 * where the heap can't give the task or golem_e, the call returns null. where
 * it can't give a hand or the head, the call returns null too. it then leaves
 * the task to step, unfinished. where any other allocation fails, the call or a
 * later step reads or writes through null.
 *
 * the non-matching side instead destroys the task and the weak points, and
 * returns null, wherever an allocation fails */
struct task *golem_e_create(NJS_POINT3 *pos, Angle ang_y, NJS_POINT3 *home);

/* the golem's state, from golem_e_create until the golem's task ends, or
 * null */
extern struct golem *golem_e;
/* TRUE once one call of golem_e_parts_build has built all its models */
extern Bool golem_e_parts_built;
/* the golem's texture lists, ending in null. golem_texture_groups, the
 * stage's groups of shared texture lists, holds them */
extern NJS_TEXLIST *golem_e_texlists[];
/* the steps a hit's flash lasts. a hit on a sensor sets golem_e's flash_steps
 * to it. a hit on a weak point sets the weak point's */
#define GOLEM_E_HIT_FLASH_STEPS 6
/* the color a hit on a sensor gives the golem and each closed weak point. it
 * fades to white as golem_e's flash_steps run out */
extern Float golem_e_flash_red;
extern Float golem_e_flash_green;
extern Float golem_e_flash_blue;

/* golem_e_sensor_matrices holds each sensor's place in the world's axes, as
 * the golem's draw last set it. golem_e_sensor_bases holds the place before
 * that */
extern NJS_MATRIX golem_e_sensor_matrices[GOLEM_SENSORS];
extern NJS_MATRIX golem_e_sensor_bases[GOLEM_SENSORS];
/* the hands' and the head's points in the world's axes, as the golem's draw
 * last found them */
extern NJS_POINT3 golem_e_hand_a_pos;
extern NJS_POINT3 golem_e_hand_b_pos;
extern NJS_POINT3 golem_e_head_pos;

#endif
