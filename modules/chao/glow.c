#include <ninja.h>
#include <sg_xpt.h>
#include "chao/icon.h"
#include "chao/marker.h"

#define QUAD_VERTICES 4
static NJS_TEXTURE_VTX glow_quad[QUAD_VERTICES] = {
#include "chao/glow/glow_quad.inc"
};

/* draw cell at position, size across, in color, blended as source over
 * destination */
#define DRAW_GLOW_CELL(cell, source, destination, position, color, size)       \
    NJS_POINT3 view;                                                           \
    glow_quad[0].col = (color);                                                \
    glow_quad[1].col = (color);                                                \
    glow_quad[2].col = (color);                                                \
    glow_quad[3].col = (color);                                                \
    njCalcPoint(NULL, (position), &view);                                      \
    njPushMatrix(&_nj_unit_matrix_);                                           \
    njSetTexture(&chao_icon_texlist);                                          \
    njSetTextureNum(cell);                                                     \
    njTranslateV(NULL, &view);                                                 \
    njScale(NULL, (size), (size), 1.0F);                                       \
    njColorBlendingMode(NJD_SOURCE_COLOR, source);                             \
    njColorBlendingMode(NJD_DESTINATION_COLOR, destination);                   \
    njDrawTexture3DEx(glow_quad, QUAD_VERTICES, TRUE);                         \
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);        \
    njColorBlendingMode(NJD_DESTINATION_COLOR,                                 \
                        NJD_COLOR_BLENDING_INVSRCALPHA);                       \
    njPopMatrixEx();

void garden_draw_glow(NJS_POINT3 *position, Uint32 color, Float size)
{
    DRAW_GLOW_CELL(CHAO_ICON_GLOW, NJD_COLOR_BLENDING_SRCALPHA,
                   NJD_COLOR_BLENDING_ONE, position, color, size);
}
void garden_draw_inverted_glow(NJS_POINT3 *position, Uint32 color, Float size)
{
    DRAW_GLOW_CELL(CHAO_ICON_INVERTED_GLOW, NJD_COLOR_BLENDING_INVDESTCOLOR,
                   NJD_COLOR_BLENDING_INVSRCCOLOR, position, color, size);
}
