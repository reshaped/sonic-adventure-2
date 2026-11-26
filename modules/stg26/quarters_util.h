/* helpers Egg Quarters' objects share: hunters' attacks, turns about y, model
 * draws and the water's height */
#ifndef STG26_QUARTERS_UTIL_H
#define STG26_QUARTERS_UTIL_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/game_state.h"

struct task;

/* the blend Ninja's draws take, which a draw that changes it puts back */
#define ALPHA_BLEND (NJD_FBS_SA | NJD_FBD_ISA)
/* the blend that adds a model's colors to those drawn already */
#define ADDITIVE_BLEND (NJD_FBS_ONE | NJD_FBD_ONE)
/* the blend that lays colors already multiplied by their alpha over those
 * drawn already */
#define PREMULTIPLIED_BLEND (NJD_FBS_ONE | NJD_FBD_ISA)
/* the strip flags a draw under a constant attribute gives each strip */
#define STRIP_FLAGS (NJD_FST_UA | NJD_FST_IS | NJD_FST_IL)

/* whether the guards of stg26/guard.h wait and their eyes fade. they do
 * while game_state.unk_0b is 0 or the stage is cleared. they do too while
 * one player's tasks run alone or a mini event runs */
#define GUARDS_STAND_DOWN                                                      \
    (game_state.unk_0b == 0 || game_state.cleared == 1 ||                      \
     game_state.solo_player != 0 || game_state.mini_event_running == 1)

/* whether task's player is a Knuckles or a Rouge that attacks. a glide, a
 * spiral and a dive count as attacks. the player is the one player_slot_of
 * finds for task, which must find one. its work and record must not be null,
 * unchecked */
Bool quarters_hunter_attacking(struct task *task);

/* turn (0, 0, radius) by ang about y, as njRotateY turns a point. *x and *z
 * take the result's x and z */
void quarters_polar_xz(Angle ang, Float radius, Float *x, Float *z);

/* draw model scaled by scale, or for a _v call by scale's x, y and z.
 * quarters_model_draw_scaled and its _v draw with njCnkEasyDrawModel.
 * quarters_direct_draw_scaled and its _v draw a build with
 * njDirectDrawObject. quarters_model_draw_tinted_scaled_v draws with
 * model_draw_tinted.
 *
 * each call leaves the current matrix scaled, as njScale leaves it. a clip
 * test in the draw takes model->r times the largest factor. the call then
 * puts r back */
void quarters_model_draw_scaled(NJS_CNK_MODEL *model, Float scale);
void quarters_model_draw_scaled_v(NJS_CNK_MODEL *model, NJS_VECTOR *scale);
void quarters_direct_draw_scaled(NJS_DIRECT_HEAD *model, Float scale);
void quarters_direct_draw_scaled_v(NJS_DIRECT_HEAD *model, NJS_VECTOR *scale);
void quarters_model_draw_tinted_scaled_v(NJS_CNK_MODEL *model, Uint32 color,
                                         NJS_VECTOR *scale);

/* the shorter turn from from to to, from -HALF_TURN to HALF_TURN - 1 */
Angle quarters_angle_turn(Angle from, Angle to);
/* what quarters_water_level returns where no water lies under player 0 */
#define QUARTERS_NO_WATER (-20000.0F)
/* the height of the water's surface where player 0 stands. it is the level of
 * the first of two pools whose box holds player 0's x and z. it is
 * QUARTERS_NO_WATER outside them both. a box holds x and z above its low
 * bounds and at or below its high bounds. player_work[0] must not be null,
 * unchecked */
Float quarters_water_level(void);
/* whether the current camera's view dips below height. the point it tests is
 * the middle of the view's bottom edge, 3 in front of the camera. the view is
 * 70 degrees across a 640 by 480 screen. it turns by the camera's ang.x alone.
 * the call returns FALSE where the camera lies below height.
 *
 * the call leaves the current matrix as it found it. Ninja's matrix stack
 * must have room for one more matrix, unchecked */
Bool quarters_view_dips_below(Float height);

/* draw model with njCnkModDrawModel. where its vertex list starts with an
 * NJD_CV chunk, a copy of that chunk stands for the list. the copy is tilted
 * by -roll about z, then by -pitch about x. it lies at WORK_AREA of
 * system/areas.h, over what was there. Ninja's matrix stack must have room for
 * one more matrix, unchecked */
void quarters_modifier_draw_tilted(NJS_CNK_MODEL *model, Angle pitch,
                                   Angle roll);
/* return the WORLD_SIZE_CLASS flags of the class that radius falls in.
 * collision/world.h gives each class's bounds */
Uint32 quarters_world_size_class(Float radius);

#endif
