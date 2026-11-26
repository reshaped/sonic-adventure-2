/* helpers Death Chamber's objects share: hunters' attacks, turns about y,
 * model draws and the water's height */
#ifndef STG25_CHAMBER_UTIL_H
#define STG25_CHAMBER_UTIL_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* what chamber_water_level returns where no water lies under the camera */
#define CHAMBER_NO_WATER (-20000.0F)

/* whether task's player is a Knuckles or a Rouge that attacks.
 * chamber_hunter_attacking counts a glide, a spiral and a dive as attacks.
 * chamber_hunter_in_combo counts only the attack chains,
 * PLAYER_MODE_HUNTER_ATTACK_1 to PLAYER_MODE_HUNTER_RUN_ATTACK_3. the player
 * is the one player_slot_of finds for task, which must find one. its work
 * and record must not be null, unchecked */
Bool chamber_hunter_attacking(struct task *task);
Bool chamber_hunter_in_combo(struct task *task);

/* turn (0, 0, radius) by ang about y, as njRotateY turns a point. *x and *z
 * take the result's x and z */
void chamber_polar_xz(Angle ang, Float radius, Float *x, Float *z);

/* draw model scaled. the plain calls scale by scale along each axis. the _v
 * calls scale by scale's x, y and z. chamber_model_draw_scaled draws with
 * njCnkEasyDrawModel. chamber_direct_draw_scaled draws a build with
 * njDirectDrawObject. chamber_model_draw_tinted_scaled draws with
 * model_draw_tinted. each _v call draws as its plain call does.
 *
 * each call scales the current matrix, as njScale does, and leaves it so. for
 * the draw, model->r is scaled by the largest factor. any clip test in the draw
 * takes the scaled r. the call then puts r back */
void chamber_model_draw_scaled(NJS_CNK_MODEL *model, Float scale);
void chamber_model_draw_scaled_v(NJS_CNK_MODEL *model, NJS_VECTOR *scale);
void chamber_direct_draw_scaled(NJS_DIRECT_HEAD *model, Float scale);
void chamber_direct_draw_scaled_v(NJS_DIRECT_HEAD *model, NJS_VECTOR *scale);
void chamber_model_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color,
                                      Float scale);

/* the short turn from from to to, -0x8000 to 0x7fff */
Angle chamber_angle_turn(Angle from, Angle to);
/* turn (x, 0, z) by ang about y, as njRotateY turns a point. point->x and
 * point->z take the result's x and z. point->y keeps its value */
void chamber_point_turn_y(Float x, Float z, Angle ang, NJS_POINT3 *point);

/* draw model with njCnkSimpleDrawModel. each material's diffuse color is
 * (alpha, red, green, blue). the call puts back _nj_control_3d_flag_ and
 * njSetConstantAttr's masks. it saves them with draw_control_save and
 * draw_attr_save, so a caller's own save is lost. it leaves the constant
 * material at 0 */
void chamber_model_draw_material(NJS_CNK_MODEL *model, Float alpha, Float red,
                                 Float green, Float blue);
/* draw model with model_draw_tinted, adding its colors to those drawn
 * already. each strip's flags become NJD_FST_UA, NJD_FST_IS and NJD_FST_IL in
 * place of its own. the call puts back _nj_control_3d_flag_ and
 * njSetConstantAttr's masks, as chamber_model_draw_material does. it leaves
 * njSetCnkBlendMode's blend at NJD_FBS_SA | NJD_FBD_ISA. it leaves
 * njColorBlendingMode's factors at NJD_COLOR_BLENDING_SRCALPHA for the source
 * and NJD_COLOR_BLENDING_INVSRCALPHA for the destination. unlike
 * model_draw_tinted_additive of shared/stage_util.h, it leaves the constant
 * material as it was */
void chamber_model_draw_tinted_additive(NJS_CNK_MODEL *model, Uint32 color);

/* the height of the water's surface where the current camera stands. it is
 * the level of the first of five pools whose box holds the camera's x and z.
 * it is CHAMBER_NO_WATER outside them all. a box holds x and z above its low
 * bounds and at or below its high bounds */
Float chamber_water_level(void);

#endif
