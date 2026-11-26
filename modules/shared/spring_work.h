/* what the springs of spring.c and sg_spring.c share: their modes, the Floats
 * they keep in their task's words, their work's fields, and the numbers of
 * their bob and their launch */
#ifndef SHARED_SPRING_WORK_H
#define SHARED_SPRING_WORK_H
#include <sg_xpt.h>

/* a spring's modes, in its work's mode. neither source sets MODE_KILL */
enum spring_mode { MODE_INIT, MODE_WAIT, MODE_LAUNCH, MODE_KILL };

/* a spring keeps three Floats in its task's motion, force and extra. the
 * destructors clear the three, which task_destroy would otherwise free */
/* the coil's scale along y, less 1. a touch and a launch set it to
 * LAUNCH_STRETCH. each waiting step then sets it between -BOB and 0 */
#define STRETCH(task) ((task)->motion.f)
#define PHASE_B(task) ((task)->force.f) /* spring b's bob phase */
/* the bob's size, which shrinks as the phase runs on */
#define BOB(task) ((task)->extra.f)

/* spring a's bob phase. spring b keeps its own in PHASE_B, since its scl.x
 * is STICK_LOCK */
#define PHASE_A(work) ((work)->scl.x)
/* the stick_lock spring b gives the player it launches */
#define STICK_LOCK(work) ((work)->scl.x)
/* what a launch adds to LAUNCH_SPEED */
#define ADD_SPEED(work) ((work)->scl.y)
#define PHASE_SPEED(work) ((work)->scl.z)
/* the slot of the player the next launch sends */
#define LAUNCH_SLOT(work) ((work)->submode)

/* a bob's phase is in degrees, and its speed in degrees a step. at rest the
 * phase is PHASE_REST, whose sine is 0, and the speed is 0 */
#define PHASE_REST 180.0F
/* the bob's size is a share of the coil's length. it starts at BOB_START.
 * each waiting step sets it to BOB_SIZE * cos((phase - PHASE_REST) /
 * BOB_SLOWDOWN). the size falls below 0 once the phase runs 90 *
 * BOB_SLOWDOWN degrees past PHASE_REST. the spring then comes to rest */
#define BOB_START 0.5F
#define BOB_SIZE 0.6F
#define BOB_SLOWDOWN 10
/* a touch sets the phase to TOUCH_PHASE and the speed to TOUCH_PHASE_SPEED.
 * as the phase passes SLOW_PHASE, the speed drops to A_PHASE_SPEED, or to
 * B_PHASE_SPEED for spring b */
#define TOUCH_PHASE 270.0F
#define TOUCH_PHASE_SPEED 20.0F
#define SLOW_PHASE 290.0F
#define A_PHASE_SPEED 18.0F
#define B_PHASE_SPEED 15.0F

/* the work's tick counts waiting steps, up to TICK_LIMIT, which keeps the
 * Uint8 from wrapping */
#define TICK_LIMIT 50

#define LAUNCH_STRETCH 0.1F
/* a launch sends the player along the spring's y axis at LAUNCH_SPEED plus
 * ADD_SPEED. the launch's top is the point that far along the axis from pos */
#define LAUNCH_SPEED 5.0F
/* a launch first puts a player at RUN_SPEED or more on the top. it pulls a
 * slower player PULL toward the top in x and z. it pulls only from PULL or
 * more away in x and z, so no pull passes the top. it pulls nothing from
 * PULL_RANGE or more away */
#define RUN_SPEED 1.0F
#define PULL 2.0F
#define PULL_RANGE 14.0F
/* spring b also puts a slow player at the top's x and z. its y goes B_RISE of
 * the way up from pos.y to the top's */
#define B_RISE 0.8F
#define SPRING_SOUND 0x1000
#define VIBRATION_POWER 4
#define VIBRATION_FREQ 15

/* the top moves TOP_TRAVEL times the stretch along y */
#define TOP_TRAVEL 4.0F

#endif
