/* the headings ControlTWalker, ControlRouge and ControlShadow write to
 * player 1's stick to steer the rival. player_read_stick takes a pad_stick's
 * ang as a heading of the same kind */
#ifndef SHARED_RIVAL_HEADING_H
#define SHARED_RIVAL_HEADING_H
#include <mathf.h>
#include <ninja.h>

#include "system/angle.h"

/* the ang.y of a work facing along dx and dz */
#define RIVAL_HEADING(dx, dz)                                                  \
    (QUARTER_TURN - (Angle)(atan2f((dx), (dz)) * ANGLE_PER_RADIAN))
/* the ang.y with which a work at from's pos faces to's */
#define RIVAL_HEADING_TOWARD(from, to)                                         \
    RIVAL_HEADING((to)->pos.x - (from)->pos.x, (to)->pos.z - (from)->pos.z)

#endif
