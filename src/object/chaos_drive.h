/* chaos drives: the drives some enemies leave, which a player takes into
 * the team's item stock */
#ifndef OBJECT_CHAOS_DRIVE_H
#define OBJECT_CHAOS_DRIVE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/trail.h"

/* a drive's kinds, by the color of its glow. a drive's item in the stock is
 * ITEM_CHAOS_DRIVE of hud/item_stock.h plus its kind */
#define CHAOS_DRIVE_YELLOW 0
#define CHAOS_DRIVE_GREEN 1
#define CHAOS_DRIVE_RED 2
#define CHAOS_DRIVE_PURPLE 3
#define CHAOS_DRIVE_KINDS 4

/* how the drive moves and looks */
struct drive_params {
    Float speed; /* its speed as it starts */
    /* what each step takes from the speed while the drive rises, or lies
     * more than hover_height above the ground */
    Float gravity;
    Float drag; /* what each step multiplies the speed by */
    Angle spin_y; /* what each step adds to ang.y */
    Angle tilt; /* the model's turn about x, under its turn about y */
    Angle spin_z; /* what each step adds to ang.z */
    /* as ang.z turns, the glow's size runs from glow_base down to glow_base
     * less twice glow_amplitude. the material's alpha runs the same way by
     * alpha_base and alpha_amplitude */
    Float glow_amplitude;
    Float glow_base;
    Float alpha_amplitude;
    Float alpha_base;
};

/* the drive's trail, its glow and each kind's model */
extern struct trail_style chaos_drive_trail_style;
extern NJS_SPRITE chaos_drive_glow;
extern NJS_CNK_OBJECT chaos_drive_yellow_object;
extern NJS_CNK_OBJECT chaos_drive_green_object;
extern NJS_CNK_OBJECT chaos_drive_red_object;
extern NJS_CNK_OBJECT chaos_drive_purple_object;
extern NJS_CNK_OBJECT *chaos_drive_objects[CHAOS_DRIVE_KINDS];

/* make a drive of a random kind at pos. the stage's program sets the trail
 * calls of stage_hooks. the portable game skips a drive when the task cannot
 * be allocated, retries its collider on later steps, and keeps the drive
 * playable if storage for either camera's trail is unavailable.
 *
 * the drive leaps along the up axis of the collision checks. it then falls
 * back toward the ground beneath where it starts. within 7.5 of the ground it
 * falls no faster, and its drag stops it. a player who touches the drive once
 * it falls takes it. the drive goes into the stock of the player's team. it
 * adds 0.1 to the gauge of a player in a mech, and scores 10 points by
 * score_add. it then shrinks away. it ends once placement_try_leave finds it
 * out of range */
void chaos_drive_create(NJS_POINT3 *pos);

#endif
