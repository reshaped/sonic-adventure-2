/* the work of the camera's KNUKLES mode, and what KNUKLES and KNUCKLES_L
 * share */
#ifndef CAMERA_KNUKLES_H
#define CAMERA_KNUKLES_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

/* the player modes of a hunter's wall dig, from its start to the dig the wall
 * refuses */
#define KNUKLES_WALL_DIG_FIRST PLAYER_MODE_HUNTER_WALL_DIG
#define KNUKLES_WALL_DIG_LAST PLAYER_MODE_HUNTER_WALL_DIG_FAIL

/* the slot's first step puts work->pos at KNUKLES_NOWHERE in x, y and z, out
 * of any player's reach */
#define KNUKLES_NOWHERE 100000.0F
/* the player is found still once more than KNUKLES_STILL_STEPS steps find
 * it within KNUKLES_STILL_REACH of work->pos. it stays still until a step
 * finds it further off */
#define KNUKLES_STILL_REACH 2.0F
#define KNUKLES_STILL_STEPS 60
/* the player's lead is its mean move a step over its last KNUKLES_LEAD_BACK
 * steps. the camera looks ahead of the player by KNUKLES_LEAD times its
 * lead. below KNUKLES_LEAD_SLOW, the lead scales by KNUKLES_LEAD_RAMP times its
 * length less KNUKLES_LEAD_NONE. that factor falls from 1 at KNUKLES_LEAD_SLOW
 * to 0 at KNUKLES_LEAD_NONE */
#define KNUKLES_LEAD_BACK 5
#define KNUKLES_LEAD 12.0F
#define KNUKLES_LEAD_SLOW 1.0F
#define KNUKLES_LEAD_NONE 0.5F
#define KNUKLES_LEAD_RAMP (1.0F / (KNUKLES_LEAD_SLOW - KNUKLES_LEAD_NONE))
/* for the camera task's first KNUKLES_FIRST_STEPS steps, the camera stands
 * off the player by the mode's offset. offset turns about y by the player's
 * ang.y negated, then about x by KNUKLES_PITCH. the camera takes offset's y
 * negated */
#define KNUKLES_FIRST_STEPS 5
#define KNUKLES_PITCH ((Angle)(-20 * ANGLE_PER_DEGREE))
/* the point the camera looks at moves KNUKLES_EASE of the way to its goal in
 * a step. it moves all the way in the first KNUKLES_FIRST_STEPS */
#define KNUKLES_EASE 0.1F
/* while pitch_held is 0, a step turns the camera's pitch toward
 * KNUKLES_PITCH by the mode's PITCH_TURN. it leaves a pitch within
 * KNUKLES_PITCH_NEAR of KNUKLES_PITCH */
#define KNUKLES_PITCH_NEAR ((Angle)(10 * ANGLE_PER_DEGREE))
/* the turn L or R gives the camera about the player in a step */
#define KNUKLES_TRIGGER_TURN ((Angle)(3 * ANGLE_PER_DEGREE))
/* past the mode's FAR, a step keeps KNUKLES_FAR_KEPT of the camera's
 * distance beyond FAR */
#define KNUKLES_FAR_KEPT 0.9F

/* the work of KNUKLES and of KNUCKLES_L, which KNUKLES runs in its place.
 * CARMEL and SPACE hold one first in their own work */
struct knukles_work {
    Angle ang; /* the heading about the player L or R last turned to */
    /* 1 where L or R turned the camera in the last step outside a COLLI_LR
     * zone, else 0 */
    Sint8 turned;
    /* 1 while the mode finds the player still about pos. still_steps counts
     * the steps toward it */
    Sint8 still;
    /* a step that finds the player out of reach of pos moves pos to the player.
     * it leaves still_steps as it is. so the count takes in steps that weren't
     * in a row. a player that keeps moving, but less than KNUKLES_STILL_REACH a
     * step, is found still all the same. the point the camera looks at then
     * stops until the player moves out of reach */
    Uint8 still_steps;
    /* while it is 0, each step tilts the camera toward KNUKLES_PITCH. a step
     * that finds the camera beyond the mode's FAR sets it to 0. CARMEL sets
     * it to 1 once the land hides the player */
    Sint8 pitch_held;
    /* the point the player is found still about. it is the player's pos at
     * the last step that found the player beyond KNUKLES_STILL_REACH of it */
    NJS_POINT3 pos;
    /* while it isn't 0, the mode's FARTHEST and NEAREST don't hold. a step
     * sets a negative one to 0. CARMEL sets it as a sweep starts, and only
     * the slot's first step sets it back to 0 */
    Sint32 unlimited;
};

#endif
