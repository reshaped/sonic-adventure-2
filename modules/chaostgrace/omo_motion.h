/* the Omochao's motions in the race. each has a channel for each of the
 * Omochao model's nine nodes */
#ifndef CHAOSTGRACE_OMO_MOTION_H
#define CHAOSTGRACE_OMO_MOTION_H
#include <ninja.h>

enum { OMO_MOTION_NODES = 9 };
extern NJS_MOTION omo_idle;
extern NJS_MOTION omo_right;
extern NJS_MOTION omo_left;
extern NJS_MOTION omo_starter_idle;
extern NJS_MOTION omo_start;
extern NJS_MOTION omo_cheer;

#endif
