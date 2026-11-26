/* the steps and creators of the cart course's objects, and the texture lists
 * they draw with. the traffic's and the chase target's calls are here too. no
 * code calls a creator that takes pos and ang. placements.c's object list
 * names steps, not creators */
#ifndef CART_OBJECTS_H
#define CART_OBJECTS_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"
struct task;
struct cart_sign {
    NJS_POINT3 pos;
    Angle yaw;
};

extern const char cart_traffic_texture_filenames[];
extern const char cart_course_texture_filenames[];
extern NJS_TEXLIST cart_object_textures, cart_story_textures;
/* the lamps' texture in the start gate's and the checkpoint's models, start_l
 * in cart_object_textures */
#define CART_LAMP_TEXTURE 73
/* start in cart_object_textures, which both models' draws show in place of
 * CART_LAMP_TEXTURE */
#define CART_START_TEXTURE 69
extern Sint32 cart_traffic_paused;
/* TRUE where pos is 2000 units or more from the camera of the view being
 * drawn */
Bool cart_out_of_view(NJS_POINT3 *pos);
/* TRUE where pos is 2000 units or more from every active camera */
Bool cart_out_of_range(NJS_POINT3 *pos);
struct task *cart_start_gate_create(NJS_POINT3 *pos, struct angles *ang);
struct task *cart_checkpoint_create(NJS_POINT3 *pos, struct angles *ang);
void cart_reset_checkpoints(void);
void cart_show_time_extension(void);
struct task *cart_jump_create(NJS_POINT3 *pos, struct angles *ang);
struct task *cart_dash_create(NJS_POINT3 *pos, struct angles *ang);
struct task *cart_sign_create(NJS_POINT3 *pos, struct angles *ang);
/* the kinds of sign cart_draw_sign draws, from 0 */
#define CART_SIGN_KINDS 4
void cart_draw_sign(struct cart_sign *sign, int kind);
struct task *cart_animated_sign_create(NJS_POINT3 *pos, struct angles *ang);
struct task *cart_create_chase_target(struct task *racer);
struct task *cart_create_invincibility(int player);
void cart_load_chase_textures(void);
void cart_release_chase_textures(void);
void cart_start_traffic(void);
/* end each car at its next step. no code of the game calls it */
void cart_stop_traffic(void);
void cart_airship_step(struct task *task);
void cart_kakusi_step(struct task *task);
void cart_ring_group_step(struct task *task);
void cart_ring_group_draw(struct task *task);
void cart_jump_step(struct task *task);
void cart_dash_step(struct task *task);
void cart_start_gate_step(struct task *task);
void cart_checkpoint_step(struct task *task);
void cart_sign_step(struct task *task);
void cart_restart_step(struct task *task);
void cart_ring_step(struct task *task);
void cart_balloon_step(struct task *task);
void cart_ring_create(NJS_POINT3 *pos);
#endif
