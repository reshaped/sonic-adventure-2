/* what the two fights of Sonic and Shadow, STAGE_VERSUS_SONIC_1 and
 * STAGE_VERSUS_SONIC_2, share. in each, a task named ControlShadow steers
 * player 1, the rival */
#ifndef SHARED_SHADOW_FIGHT_H
#define SHARED_SHADOW_FIGHT_H
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

/* ControlShadow's record, at the start of the task's extra. timer counts
 * steps, and goes back to 0 at most changes of mode. a timed mode ends once
 * timer passes wait. the intro's camera looks at look */
struct shadow_control {
    Sint8 mode;
    Sint8 intro_done; /* 1 from the intro's end, and 0 before it */
    Sint16 timer;
    Sint16 wait;
    NJS_POINT3 look;
};

/* the ang.y with which a work at from's pos faces to's. player_read_stick
 * takes a pad_stick's ang as a heading of the same kind. ControlShadow writes
 * this one to player 1's stick */
#define SHADOW_FIGHT_HEADING(from, to)                                         \
    (QUARTER_TURN - (Angle)(atan2f((to)->pos.x - (from)->pos.x,                \
                                   (to)->pos.z - (from)->pos.z) *              \
                            ANGLE_PER_RADIAN))
/* a player faces another while its ang.y lies less than this from the
 * heading toward the other */
#define SHADOW_FIGHT_FACING 0x5000

/* after a retry, the intro's end summons a messenger
 * SHADOW_FIGHT_MESSENGER_RISE above the arena's center. the stage's messages
 * hold a set of SHADOW_FIGHT_LINES messages for each character player 0 can
 * be. one retry picks the first of player 0's set, two the second, and three
 * or more the third */
#define SHADOW_FIGHT_MESSENGER_RISE 300.0F
#define SHADOW_FIGHT_LINES 3

/* the intro's wait, and its title's life, in steps */
#define SHADOW_FIGHT_INTRO_STEPS 360

#endif
