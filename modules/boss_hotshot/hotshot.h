/* Hot Shot, the boss of BOSS_HOTSHOT.PRS */
#ifndef BOSS_HOTSHOT_HOTSHOT_H
#define BOSS_HOTSHOT_HOTSHOT_H
#include <ninja.h>
#include <sg_xpt.h>

/* the arena's half sizes. the stage holds player 0 within HOTSHOT_ARENA_HALF_X
 * of x = 0 and HOTSHOT_ARENA_HALF_Z of z = 0. each charge ends once past
 * HOTSHOT_ARENA_HALF_X */
#define HOTSHOT_ARENA_HALF_X 200.0F
#define HOTSHOT_ARENA_HALF_Z 175.0F

/* create Hot Shot's task at *pos, turned to ang_y, and start its opening. it
 * takes player 0's pad until the opening ends.
 *
 * the heap must give the task and the boss's record, with its mover and its
 * block, unchecked. it must also give the turret's and the gun's aims and the
 * capsule, unchecked. where it can't give the parts, the call kills the task,
 * and the record's block is lost unfreed */
void hotshot_create(NJS_POINT3 *pos, Angle ang_y);

#endif
