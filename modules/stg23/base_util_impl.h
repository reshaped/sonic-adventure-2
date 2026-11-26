/* the bodies of base_cylinder_draw and base_angle_delta, which
 * stg23/base_util.h declares. stg23/base_util.c builds shared/ocean_util.c
 * under HIDDEN_BASE_UTIL. ocean_util.c then includes this file between
 * ocean_turn_xz and vlist_copy_tinted, where the two stand in the image */
#ifndef STG23_BASE_UTIL_IMPL_H
#define STG23_BASE_UTIL_IMPL_H
#include "camera/camera.h"
#include "draw/lines.h"
#include "stg23/base_util.h"

#define CYLINDER_SHADE 0.3F

void base_cylinder_draw(NJS_VECTOR *scale)
{
    /* @hack spelling: size, a copy of *scale, where scale read in place would
     * do */
    NJS_VECTOR size;
    Float largest;
    /* @hack spelling: radius, a pointer to the cylinder model's r, where r
     * read and written in place would do */
    Float *radius;
    Float saved;

    size = *scale;
    largest = size.x;
    largest = largest > size.y ? largest : size.y;
    largest = size.z > largest ? size.z : largest;
    radius = &debug_models[DEBUG_MODEL_CYLINDER]->model->r;
    saved = *radius;
    *radius *= largest;
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT);
    njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ISA);
    njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA | NJD_FST_IS | NJD_FST_IL);
    constant_material_set(CYLINDER_SHADE, CYLINDER_SHADE, CYLINDER_SHADE,
                          CYLINDER_SHADE);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, size.y * 0.5F, 0.0F);
    njScaleEx(&size);
    njCnkSimpleDrawObject(debug_models[DEBUG_MODEL_CYLINDER]);
    njPopMatrixEx();
    draw_attr_restore();
    draw_control_restore();
    *radius = saved;
}

Angle base_angle_delta(Angle from, Angle to)
{
    return (Sint16)(to - from);
}

#endif
