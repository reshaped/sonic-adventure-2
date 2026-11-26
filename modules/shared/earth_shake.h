/* the earth shake: a quake that shakes each camera within its range by a damped
 * spring */
#ifndef SHARED_EARTH_SHAKE_H
#define SHARED_EARTH_SHAKE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* start a quake at pos, and return its task, EsShakeRad in task level 3. it
 * returns null where task_create fails.
 *
 * the quake's spring starts unstretched, moving at power. a stretch that would
 * fall below 0 stays at 0, and the spring rebounds. the quake ends once the
 * size of its speed falls below 0.1.
 *
 * each step shakes each camera nearer pos than range. the shake is the spring's
 * stretch along y, and 0.35 of its speed along x and z. it is turned about y by
 * an angle that grows each step. the shake falls off evenly with distance, to 0
 * at range.
 *
 * a second task, EarthShakeManExec, sums a step's shakes and gives cameras 0
 * and 1 their sums. the shakes of every camera past 0 add to camera 1's sum.
 * the task ends once a step finds both sums' squared lengths below 0.001. it
 * then stops each camera's shake */
struct task *shake_create(NJS_POINT3 *pos, Float range, Float power);

#endif
