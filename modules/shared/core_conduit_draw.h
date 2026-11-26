/* the additive draw of the conduits on Eggman's and Tails's routes.
 * CONDUIT_ADD_BEGIN saves the draw state, and adds what draws after it onto the
 * screen. CONDUIT_ADD_END returns blending to alpha and restores the state */
#ifndef SHARED_CORE_CONDUIT_DRAW_H
#define SHARED_CORE_CONDUIT_DRAW_H
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/chunk.h"
#include "draw/lines.h"
#include "system/util.h"

#define CONDUIT_ADD_BEGIN()                                                    \
    draw_control_save();                                                       \
    draw_attr_save();                                                          \
    draw_control_clear(NJD_CONTROL_3D_OFFSET_MATERIAL |                        \
                       NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL);              \
    draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE |                           \
                     NJD_CONTROL_3D_CNK_CONSTANT_ATTR |                        \
                     NJD_CONTROL_3D_CONSTANT_MATERIAL);                        \
    njSetCnkBlendMode(NJD_FBS_ONE | NJD_FBD_ONE);                              \
    njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA | NJD_FST_IS | NJD_FST_IL);
#define CONDUIT_ADD_END()                                                      \
    njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ISA);                               \
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);        \
    njColorBlendingMode(NJD_DESTINATION_COLOR,                                 \
                        NJD_COLOR_BLENDING_INVSRCALPHA);                       \
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);                             \
    draw_attr_restore();                                                       \
    draw_control_restore();

#endif
