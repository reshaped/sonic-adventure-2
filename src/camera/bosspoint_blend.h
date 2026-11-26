/* the BOSSPOINT step's curves, which mode_bosspoint.c and the Biolizard's
 * camera both use. each macro works on one coordinate. each camera keeps its
 * own previous eye and target */
#ifndef CAMERA_BOSSPOINT_BLEND_H
#define CAMERA_BOSSPOINT_BLEND_H
#include "camera/bosspoint.h"

#define BOSSPOINT_EYE_STEPS 60
#define BOSSPOINT_EASE(value, goal) ((((goal) - (value)) * 0.1F) + (value))
#define BOSSPOINT_BLEND_EYE(eye, previous, steps)                              \
    ((((previous) - (eye)) * (steps) * (1.0F / BOSSPOINT_EYE_STEPS)) + (eye))
#define BOSSPOINT_TARGET(player, target, part, blend)                          \
    ((((player) - (target)) *                                                  \
      (1.0F - ((part) * (blend) * (1.0F / CAMERA_BOSSPOINT_BLEND)))) +         \
     (target))
#define BOSSPOINT_LIFT(lift, blend)                                            \
    ((lift) * (1.0F - ((blend) * (1.0F / CAMERA_BOSSPOINT_BLEND))))
#define BOSSPOINT_TURN(previous, target)                                       \
    ((previous) + (((target) - (previous)) * 0.2F))

#endif
