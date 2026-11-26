/* helpers Pyramid Cave's objects share: Sonic's combo, turns about y, model
 * draws and distances across x and z */
#ifndef STG28_CAVE_UTIL_H
#define STG28_CAVE_UTIL_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the blend Ninja's draws take, which a draw that changes it puts back */
#define ALPHA_BLEND (NJD_FBS_SA | NJD_FBD_ISA)
/* the blend that adds a model's colors to those drawn already */
#define ADDITIVE_BLEND (NJD_FBS_ONE | NJD_FBD_ONE)
/* the strip flags a draw under a constant attribute gives each strip */
#define STRIP_FLAGS (NJD_FST_UA | NJD_FST_IS | NJD_FST_IL)

/* a fire shows FIRE_FRAME(task), one of pyramid_cave_fire_texlist's
 * FIRE_FRAMES textures. its init picks a frame at random. each step takes the
 * next, back to 0 after the last */
#define FIRE_FRAME(task) ((task)->unk_3c.l)
#define FIRE_FRAMES 16
/* the halfword of a fire model's polygon list that holds its texture's id. it
 * is the second of the list's first NJD_CT_TID chunk */
#define FIRE_TID 7
/* write frame into model's halfword at FIRE_TID, keeping the halfword's
 * NJD_FFM_MASK bits. texture is a Sint16 of the caller's */
/* @hack spelling: texture, a local of the caller's for the halfword, where the
 * halfword itself would do */
#define FIRE_SET_FRAME(model, frame, texture)                                  \
    do {                                                                       \
        (texture) = (model)->plist[FIRE_TID];                                  \
        (texture) = ((texture) & NJD_FFM_MASK) | (frame);                      \
        (model)->plist[FIRE_TID] = (texture);                                  \
    } while (0)

/* whether task's player is a Sonic in his combo. cave_sonic_in_combo counts
 * its moves, PLAYER_MODE_SONIC_COMBO_1 to PLAYER_MODE_SONIC_COMBO_RUNNING_3.
 * cave_sonic_combo_or_bounce counts PLAYER_MODE_SONIC_BOUNCE too. the player
 * is the one player_slot_of finds for task, which must find one. its work
 * and record must not be null, unchecked */
Bool cave_sonic_combo_or_bounce(struct task *task);
Bool cave_sonic_in_combo(struct task *task);

/* turn (0, 0, radius) by ang about y, as njRotateY turns a point. *x and *z
 * take the result's x and z */
void cave_polar_xz(Angle ang, Float radius, Float *x, Float *z);

/* draw model scaled. the plain calls scale by scale along each axis. the _v
 * calls scale by scale's x, y and z. cave_model_draw_scaled draws with
 * njCnkEasyDrawModel. cave_model_draw_tinted_scaled draws with
 * model_draw_tinted of shared/common_object.h. each _v call draws as its plain
 * call does.
 *
 * each call scales the current matrix, as njScale does, and leaves it so. for
 * the draw, model->r is scaled by the largest factor. any clip test in the draw
 * takes the scaled r. the call then puts r back */
void cave_model_draw_scaled(NJS_CNK_MODEL *model, Float scale);
void cave_model_draw_scaled_v(NJS_CNK_MODEL *model, NJS_VECTOR *scale);
void cave_model_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color,
                                   Float scale);
void cave_model_draw_tinted_scaled_v(NJS_CNK_MODEL *model, Uint32 color,
                                     NJS_VECTOR *scale);

/* whether point lands on camera 0's screen of 640 by 480. the call takes point
 * through the current matrix, and projects it by camera 0's angle of view. a
 * point whose z is then below 0 fails */
Bool cave_on_screen(NJS_POINT3 *point);
/* the short turn from from to to, -0x8000 to 0x7fff */
Angle cave_angle_turn(Angle from, Angle to);

/* draw model with njCnkSimpleDrawModel. each material's diffuse color is
 * (alpha, red, green, blue). the call puts back _nj_control_3d_flag_ and
 * njSetConstantAttr's masks. it saves them with draw_control_save and
 * draw_attr_save, so a caller's own save is lost. it leaves the constant
 * material at 0 */
void cave_model_draw_material(NJS_CNK_MODEL *model, Float alpha, Float red,
                              Float green, Float blue);

/* the square of point's distance from the y axis */
Float cave_xz_length_sq(NJS_POINT3 *point);
/* the distance from from to to, in x and z alone */
Float cave_xz_distance(NJS_POINT3 *from, NJS_POINT3 *to);

/* turn (x, 0, z) by ang about y, as njRotateY turns a point. point->x and
 * point->z take the result's x and z. point->y keeps its value */
void cave_point_turn_y(Float x, Float z, Angle ang, NJS_POINT3 *point);

#endif
