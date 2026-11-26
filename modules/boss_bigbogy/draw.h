/* drawing, dust and projection for King Boom Boo's fight. draw.c also defines
 * the five calls shared/sandglass.h asks of the binary */
#ifndef BOSS_BIGBOGY_DRAW_H
#define BOSS_BIGBOGY_DRAW_H
#include <ninja.h>
#include <sg_xpt.h>

/* make a puff with dust_create, of speed spd and size scale. it lies at pos
 * plus an offset of (0, 0, radius). the offset first turns about x by a random
 * 0 to 60 degrees. it then turns about y by a random 120 to 240 degrees. it
 * last takes the current matrix's turn, as current_matrix_angles_xyz reads
 * it.
 *
 * Ninja's matrix stack must have room for one more matrix, unchecked. the
 * call leaves the current matrix as it found it */
void bigbogy_dust_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                         Float scale);

/* scale the current matrix by scale, and draw model by njCnkEasyDrawModel.
 * model->r takes the same scale for the draw, and comes back after it. the
 * matrix keeps the scale. a _v call scales each axis by its own component of
 * scale, and model->r by the largest. a _tinted call draws by
 * model_draw_tinted in color */
void bigbogy_model_draw_scaled(NJS_CNK_MODEL *model, Float scale);
void bigbogy_model_draw_scaled_v(NJS_CNK_MODEL *model, NJS_VECTOR *scale);
void bigbogy_model_draw_tinted_scaled(NJS_CNK_MODEL *model, Uint32 color,
                                      Float scale);
void bigbogy_model_draw_tinted_scaled_v(NJS_CNK_MODEL *model, Uint32 color,
                                        NJS_VECTOR *scale);

/* set *sx and *sy to point's place on camera 0's screen, in pixels from its
 * center. the current matrix must take point into camera 0's view. camera
 * 0's angle of view spans the screen's width. bigbogy_on_screen returns TRUE
 * where that place lies inside the screen's edges. it doesn't test the
 * point's depth, so a point behind the camera can pass */
void bigbogy_point_to_screen(NJS_POINT3 *point, Float *sx, Float *sy);
Bool bigbogy_on_screen(NJS_POINT3 *point);
#endif
