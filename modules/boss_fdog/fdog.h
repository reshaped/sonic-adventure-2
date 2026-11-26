/* the Flying Dog, the boss of BOSS_FDOG.PRS */
#ifndef BOSS_FDOG_FDOG_H
#define BOSS_FDOG_FDOG_H
#include <ninja.h>
#include <sg_xpt.h>

/* the arena's half sizes. the stage holds player 0 within FDOG_ARENA_HALF_X of
 * x = 0 and FDOG_ARENA_HALF_Z of z = 0. each charge, and the flight on after
 * a hit, heads out to FDOG_ARENA_HALF_X and ends once past it */
#define FDOG_ARENA_HALF_X 200.0F
#define FDOG_ARENA_HALF_Z 175.0F

/* create the Flying Dog's task 50 above *pos, turned to ang_y, and start its
 * opening. it takes player 0's pad until the opening ends.
 *
 * the heap must give the task and the record's mover, unchecked. it must also
 * give the turret's and the gun's aims and the capsule, unchecked. where the
 * heap can't give the boss's record, the call kills the task. where it can't
 * give the record's block, the record and the mover are then each freed twice.
 * where it can't give the parts, the call kills the task, and the record's
 * block is lost unfreed */
void fdog_create(NJS_POINT3 *pos, Angle ang_y);

#endif
