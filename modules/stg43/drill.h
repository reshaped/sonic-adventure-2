/* Cosmic Wall's two drill rides. a ride's platform carries player 0 along its
 * path. a drill follows the platform once a DRILLCOL placement releases it */
#ifndef STG43_DRILL_H
#define STG43_DRILL_H
#include <sg_xpt.h>

#include "stage/path.h"
struct task;
/* the rides' numbers, which a platform's work's submode holds */
#define COSMIC_DRILL_FIRST_RIDE 0
#define COSMIC_DRILL_SECOND_RIDE 1
#define COSMIC_DRILL_RIDES 2
/* the paths of the first and the second ride */
extern struct path cosmic_wall_first_path;
extern struct path cosmic_wall_second_path;
/* a ride's track word, which its platform's task holds in unk_3c, is the
 * address of the ride's path */
#ifdef NONMATCHING
/* this side's track word is the ride's number instead. a 64-bit host's
 * pointer is wider than the word */
#define COSMIC_DRILL_FIRST_TRACK COSMIC_DRILL_FIRST_RIDE
#define COSMIC_DRILL_SECOND_TRACK COSMIC_DRILL_SECOND_RIDE
#else
#define COSMIC_DRILL_FIRST_TRACK ((Sint32) & cosmic_wall_first_path)
#define COSMIC_DRILL_SECOND_TRACK ((Sint32) & cosmic_wall_second_path)
#endif

/* each ride's entry is TRUE while its platform carries player 0 or runs its
 * path, and FALSE otherwise. once game_state.cleared is set, it keeps its last
 * value. a GATE lights while its ride's entry is TRUE */
extern Bool cosmic_drill_active[COSMIC_DRILL_RIDES];

/* the step that starts a ride's platform. task needs a work, whose submode is
 * the ride's number, and the ride's track word in unk_3c. where the heap has
 * no room for the platform's record, the step runs again next step.
 *
 * the platform waits at its path's start until player 0 stands on it. it then
 * draws player 0 into its seat and runs its path. the first ride's platform
 * lets player 0 go at its path's end, and stays there. the second's throws
 * player 0 by egg_jump_create of stg43/egg_jump.h */
void cosmic_drill_platform_init(struct task *task);
/* the object list's step for DRILLCOL, a sphere of radius scl.x about pos.
 * the platforms and the drills test the spheres each step. a sphere whose
 * ang.x has bit 0 set is a speed trigger. a speed trigger eases the speed of a
 * platform or a drill inside it toward scl.z. any other sphere releases a
 * drill from a running platform inside it that has none yet. the drill starts
 * at the platform's place. it shows and moves once scl.y steps have passed,
 * scl.y cut toward 0. a sphere stays once started, since its step never tests
 * its range */
void cosmic_drill_trigger_init(struct task *task);
#endif
