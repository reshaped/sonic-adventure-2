/* the adjusts that blend a camera's place from where it was toward where its
 * mode puts it */
#ifndef CAMERA_BLEND_H
#define CAMERA_BLEND_H

struct camera_slot;
struct camera_stack;

/* the step of the adjusts CAMERA_ADJUST_THREE1 to CAMERA_ADJUST_RELATIVE6.
 * it blends the members of camera_state that camera_state.type names. each goes
 * from its value in stack's place toward its value in camera_state. each adjust
 * number has a length, from 60 to 30000 steps. a member goes adjust->steps /
 * length of the way. a share outside 0 to 1 counts as the whole way. an angle
 * turns the short way round.
 *
 * under a RELATIVE adjust, pos and target blend as offsets from the current
 * camera's player. the place's are offsets from the player's pos as
 * camera_player_place gives it. camera_state's are offsets from the player's
 * pos now. camera_blend then adds the player's pos now to both camera_state's
 * pos and its target. one the type doesn't name is left off by the player's
 * pos, for camera_state_complete to work out again.
 *
 * stack must be the current camera's. adjust must be paired with the mode on
 * top of it, whose adjust number gives the length. on adjust's first step,
 * camera_blend writes two Angles at the start of adjust's work. they hold the
 * turns from the place's x and y angles to those of the aim. the aim looks from
 * camera_state's pos at the current camera's player */
void camera_blend(struct camera_stack *stack, struct camera_slot *adjust);

#endif
