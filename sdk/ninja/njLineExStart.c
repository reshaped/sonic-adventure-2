#include <ninja.h>
#include <nj_global.h>
#include <nj_km.h>
#include <nj_line.h>
#include <sg_xpt.h>

Float line_half_width;
Uint32 *line_cursor;
struct line_projection line_projection;

void njPtclPolygonStartTrans(Uint32 color)
{
    njGetkmPointer();
    _nj_ptcl_sq_ptr_ = njStartVertexNonTexSpriteTrans(color);
}

#define SET_WIDTH_AND_PROJECTION(half_width)                                   \
    {                                                                          \
        line_half_width = (half_width);                                        \
        line_projection.scale_x = _nj_screen_.dist * _nj_scr_aspect_x_;        \
        line_projection.scale_y = _nj_screen_.dist * _nj_scr_aspect_y_;        \
        line_projection.center_x = _nj_screen_.cx;                             \
        line_projection.center_y = _nj_screen_.cy;                             \
    }

void njDrawLineExStartOpaque(Float half_width, Uint32 color)
{
    njGetkmPointer();
    njSetkmColorMode(NJD_COLOR_MODE_FLAT_PACKED_NORMAL);
    line_cursor = njStartVertexNonTexSpriteOpaque(color);
    SET_WIDTH_AND_PROJECTION(half_width);
}

void njDrawLineExStartTrans(Float half_width, Uint32 color)
{
    njGetkmPointer();
    njSetkmColorMode(NJD_COLOR_MODE_FLAT_PACKED_NORMAL_TRANS);
    line_cursor = njStartVertexNonTexSpriteTrans(color);
    SET_WIDTH_AND_PROJECTION(half_width);
}

void njDrawLineExEnd(void)
{
    njEndVertex(line_cursor);
    njSetkmPointer();
}
