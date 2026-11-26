/* the debug draws: a debug model in a translucent dark gray, and a mark of
 * three axis lines. SHC builds this source unoptimized, as
 * config/compilers.mk routes it */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "draw/lines.h"
#include "system/angle.h"
#include "system/util.h"

/* the debug model's constant material: its alpha, and the gray each of its
 * red, green and blue takes */
#define MODEL_ALPHA 0.5F
#define MODEL_GRAY 0.2F

void debug_draw_model(NJS_POINT3 *pos, const struct angles *ang,
                      NJS_VECTOR *scl, int model)
{
    Uint32 control;
    Uint32 and_attr;
    Uint32 or_attr;

    njPushMatrixEx();
    njTranslateEx(pos);
    if (ang) {
        njRotateZ(NULL, ang->z);
        njRotateX(NULL, ang->x);
        njRotateY(NULL, ang->y);
    }
    if (scl) {
        njScaleEx(scl);
    }
    and_attr = _nj_constant_attr_and_;
    or_attr = _nj_constant_attr_or_;
    control = _nj_control_3d_flag_;
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT);
    draw_attr_set(NJD_FST_UA, 0);
    constant_material_set(MODEL_ALPHA, MODEL_GRAY, MODEL_GRAY, MODEL_GRAY);
    njCnkSimpleDrawObject(debug_models[model]);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    _nj_control_3d_flag_ = control;
    _nj_constant_attr_and_ = and_attr;
    _nj_constant_attr_or_ = or_attr;
    njPopMatrixEx();
}

void debug_draw_mark(NJS_POINT3 *pos, Float size)
{
    NJS_COLOR to;
    NJS_COLOR from;

    from.argb.a = 0;
    from.argb.r = 0;
    from.argb.g = 0;
    from.argb.b = 0;
    to.argb.a = 255;
    to.argb.r = 255;
    to.argb.g = 0;
    to.argb.b = 0;
    line_draw_colored(pos->x - (size * 0.5F), pos->y, pos->z, &from,
                      pos->x + (size * 0.5F), pos->y, pos->z, &to);
    from.argb.a = 0;
    from.argb.r = 0;
    from.argb.g = 0;
    from.argb.b = 0;
    to.argb.a = 255;
    to.argb.r = 0;
    to.argb.g = 255;
    to.argb.b = 0;
    line_draw_colored(pos->x, pos->y - (size * 0.5F), pos->z, &from, pos->x,
                      pos->y + (size * 0.5F), pos->z, &to);
    from.argb.a = 0;
    from.argb.r = 0;
    from.argb.g = 0;
    from.argb.b = 0;
    to.argb.a = 255;
    to.argb.r = 0;
    to.argb.g = 0;
    to.argb.b = 255;
    line_draw_colored(pos->x, pos->y, pos->z - (size * 0.5F), &from, pos->x,
                      pos->y, pos->z + (size * 0.5F), &to);
}
