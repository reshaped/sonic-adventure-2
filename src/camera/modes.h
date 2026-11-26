/* the steps of the camera's modes and adjusts, named for their entries in
 * camera_modes and camera_adjusts. each runs for the current camera, with its
 * stack and the step's own slot. a mode's step leaves camera_state as the
 * camera's place for this step. the adjust's step then changes it.
 *
 * below, the player is the current camera's player, and its target is its
 * pos raised by its target_height */
#ifndef CAMERA_MODES_H
#define CAMERA_MODES_H

#include "camera/camera.h"

struct camera_slot;
struct camera_stack;

/* FOLLOW: look at player 0's target. player 1's pad turns the camera about
 * it, as camera/orbit.h says */
void camera_mode_follow(struct camera_stack *stack, struct camera_slot *slot);
/* KNUKLES: follow the player from behind, looking ahead of it as it moves.
 * outside a COLLI_LR zone, L and R turn the camera about the player. for
 * Knuckles or Rouge, KNUCKLES_L runs in KNUKLES's place while the player climbs
 * or digs into a wall. KNUCKLES_L also runs while player 1's slot holds a
 * player. where the player climbs a ledge, a CAMERA_MODE_USER takes the mode's
 * slot. it looks down on the player until the climb ends, then puts the
 * replaced mode back */
void camera_mode_knukles(struct camera_stack *stack, struct camera_slot *slot);
/* EDITOR: FOLLOW about the camera editor's cursor, by the current camera's
 * pad */
void camera_mode_editor(struct camera_stack *stack, struct camera_slot *slot);
/* EDITOR2: stand at the camera editor's camera_pos and look at its cursor,
 * with its roll */
void camera_mode_editor2(struct camera_stack *stack, struct camera_slot *slot);
/* SNAPSHOT: look at a point that starts at the player's pos. the camera's
 * pad moves it, as point_move_by_pad says. the pad also turns the camera
 * about it, as camera/orbit.h says */
void camera_mode_snapshot(struct camera_stack *stack, struct camera_slot *slot);
/* KLAMATH: stand param[ZONE_DIST] beyond the player in x and z, on the line
 * from the zone's point. the camera stands param[ZONE_HEIGHT] above the
 * player, or half of param[ZONE_DIST] where that is 0. the point it looks at
 * climbs toward that height as the player moves away from the zone's point.
 * slot must hold a zone */
void camera_mode_klamath(struct camera_stack *stack, struct camera_slot *slot);
/* POINT: look at a point param[ZONE_HEIGHT] above the player's target. the
 * camera stands param[ZONE_DIST] off it, along the line from the zone's point
 * to the player. slot must hold a zone */
void camera_mode_point(struct camera_stack *stack, struct camera_slot *slot);
/* ASHLAND: stand at the zone's camera_pos, with the roll in its camera_ang,
 * and look at the player's target. the camera's angle of view heads for the
 * zone's view[ZONE_VIEW_ANGLE], and camera_run_mode leaves it there. a step
 * sets a view[ZONE_VIEW_ANGLE] below 10 degrees to 70 in the zone. slot must
 * hold a zone */
void camera_mode_ashland(struct camera_stack *stack, struct camera_slot *slot);
/* FIX: stand at the camera_pos of the mode's zone and look at its point,
 * with the roll in its camera_ang. slot must hold a zone */
void camera_mode_fix(struct camera_stack *stack, struct camera_slot *slot);
/* LEAVE: look at the player's target. the camera stays where the mode's first
 * step found it, with the roll it had */
void camera_mode_leave(struct camera_stack *stack, struct camera_slot *slot);
/* SPACE: follow the player along the camlin nodes of the camera's table. the
 * camera stands through the mode's zone, whose camera_pos and point the step
 * writes. a step without a zone puts CAMERA_MODE_KNUKLES in the mode's slot.
 * the first step takes CAMERA_ADJUST_RELATIVE3. the current camera must be 0
 * or 1 */
void camera_mode_space(struct camera_stack *stack, struct camera_slot *slot);
/* CARMEL: KNUKLES, until the land hides Knuckles or Rouge from the camera.
 * the camera then sweeps in along the player's trail, or searches for a place
 * in sight. a step does nothing while camera_no_player(camera_current_index)
 * isn't 0 */
void camera_mode_carmel(struct camera_stack *stack, struct camera_slot *slot);
/* MOTION: play the motion camera_motion_start gave, as camera/motion.h
 * says */
void camera_mode_motion(struct camera_stack *stack, struct camera_slot *slot);
/* BOSSINIT: a boss's opening shot, from what camera_bossinit_start gave, as
 * camera/bossinit.h says */
void camera_mode_bossinit(struct camera_stack *stack, struct camera_slot *slot);
/* BOSSPOINT: stand behind a player, on the far side from a point, from what
 * camera_bosspoint_start gave, as camera/bosspoint.h says */
void camera_mode_bosspoint(struct camera_stack *stack,
                           struct camera_slot *slot);
/* PSTONE: keep players 0 and 1 in sight. look at the point midway between
 * them, raised by the player's target_height. the camera stands off to the
 * side of the line between them. the zone's camera_pos, seen from its point,
 * picks the side. a step leaves camera_state as it is while
 * player_tasks[1] is null. slot must hold a zone */
void camera_mode_pstone(struct camera_stack *stack, struct camera_slot *slot);
/* the index of a camera's stack where camera_start and camera_reset_modes
 * put INIT */
#define CAMERA_INIT_SLOT 14
/* INIT: look at the player's target from a place back along the player's x
 * axis, and higher. the mode must sit at CAMERA_INIT_SLOT, and keeps
 * CAMERA_ADJUST_NONE there. its seventh step fills the camera's trail and
 * takes the mode off. the new top then takes CAMERA_ADJUST_NONE. a step does
 * nothing while the player has no work */
void camera_mode_init(struct camera_stack *stack, struct camera_slot *slot);
/* EASYSET: set camera_state to the current camera's entry of
 * camera_easyset_states, whole. the code that pushes the mode fills that
 * entry */
extern struct camera_state camera_easyset_states[CAMERAS];
void camera_mode_easyset(struct camera_stack *stack, struct camera_slot *slot);
/* BOSSKLAMATH: KLAMATH from what camera_bossklamath_start gave, as
 * camera/bossklamath.h says */
void camera_mode_bossklamath(struct camera_stack *stack,
                             struct camera_slot *slot);
/* GAKUGAKU: hold the camera still, leaving camera_state as it is.
 * camera_run_mode pushes it, as camera/stack.h says. each step takes the mode
 * on top off once for each test that holds: camera_settle_step returns 0;
 * the camera's pad holds L or R; a camera motion plays. the first step can
 * pull stack's place in toward the player. it does so where a probe between
 * them meets the land */
void camera_mode_gakugaku(struct camera_stack *stack, struct camera_slot *slot);
/* KNUCKLES_L: KNUKLES from further off. KNUKLES runs it in its place */
void camera_mode_knuckles_l(struct camera_stack *stack,
                            struct camera_slot *slot);
/* FIX2: leave camera_state as it is */
void camera_mode_fix2(struct camera_stack *stack, struct camera_slot *slot);
/* PSTONE2: look at the point midway between players 0 and 1. the mode's
 * first step keeps the dir of the current camera's place. the camera stands
 * off the point by that dir, times the players' distance apart. once that
 * puts it more than 150 from the point, the step takes the mode off. the new
 * top then takes CAMERA_ADJUST_RELATIVE1. both players must have work */
void camera_mode_pstone2(struct camera_stack *stack, struct camera_slot *slot);
/* SS: look at a point between two, from what camera_ss_start gave, as
 * camera/ss.h says */
void camera_mode_ss(struct camera_stack *stack, struct camera_slot *slot);

/* HALF: move camera_state's pos halfway back to stack's place. its type
 * becomes CAMERA_POS_ANG_DIST, so a target the mode set no longer counts.
 * ang.x and ang.y don't turn back with pos. each comes out as the mode left
 * it, cut to its low 16 bits */
void camera_adjust_half(struct camera_stack *stack, struct camera_slot *adjust);

#endif
