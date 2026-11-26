/* Big Foot, the boss of BOSS_BIGFOOT.PRS */
#ifndef BOSS_BIGFOOT_BIGFOOT_H
#define BOSS_BIGFOOT_BIGFOOT_H
#include <ninja.h>
#include <sg_xpt.h>

/* the arena's radius. the stage holds player 0 within it of the origin, along
 * x and z. each charge runs out past it */
#define BIGFOOT_ARENA_RADIUS 120.0F
/* the light Big Foot's body draws in, but for the first half of a hurt blink */
#define BIGFOOT_LIGHT 8

/* create Big Foot's task at *pos, turned to ang_y, and start its opening. it
 * takes player 0's pad until the opening ends.
 *
 * the heap must give the task and the boss's record, with its mover and its
 * block, unchecked. it must also give the turret's aim and the capsule,
 * unchecked. where it can't give the parts, the call kills the task, and the
 * record's block is lost unfreed */
void bigfoot_create(NJS_POINT3 *pos, Angle ang_y);

#endif
