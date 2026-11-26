#include <ninja.h>
#include <nj_draw3d.h>
#include <nj_km.h>
#include <sg_xpt.h>

/* a line's ends */
#define LINE_ENDS 2

void njDrawLine3D(NJS_POINT3COL *p, Int n, Uint32 attr)
{
    Float sx;
    Float sy;
    Float cx;
    Float cy;
    Uint32 *sq;
    point_projector calc;
    vertex_sender send;
    Int i;
    Int j;
    /* @hack spelling: cur declared before stride, where either order would
     * do */
    Int cur;
    Int stride;
    Uint8 buf[LINE_ENDS * sizeof(struct draw3d_tex_vertex)];
    Sint32 clip[LINE_ENDS];
    Sint32 code;
    /* @hack spelling: vtx, a second name for buf, where buf would do */
    Uint8 *vtx;

    sx = _nj_screen_.dist * _nj_scr_aspect_x_;
    sy = _nj_screen_.dist * _nj_scr_aspect_y_;
    cx = _nj_screen_.cx;
    cy = _nj_screen_.cy;
    njGetkmPointer();
    vtx = buf;
    if (attr & NJD_USE_TEXTURE) {
        if (attr & NJD_TRANSPARENT) {
            njSetkmColorMode(NJD_COLOR_MODE_PACKED_TEXTURE_TRANS);
            sq = njStartVertexTexTrans();
        } else {
            njSetkmColorMode(NJD_COLOR_MODE_PACKED_TEXTURE);
            sq = njStartVertexTexOpaque();
        }
        send = njTri_Wired_With_Tex;
        calc = njDCalcPoint3DT2;
        stride = sizeof(struct draw3d_tex_vertex);
    } else {
        if (attr & NJD_TRANSPARENT) {
            njSetkmColorMode(NJD_COLOR_MODE_PACKED_NORMAL_TRANS);
            sq = njStartVertexNonTexTrans();
        } else {
            njSetkmColorMode(NJD_COLOR_MODE_PACKED_NORMAL);
            sq = njStartVertexNonTexOpaque();
        }
        send = njTri_Wired;
        calc = njDCalcPoint3DP2;
        stride = sizeof(struct draw3d_vertex);
    }
    if (attr & NJD_DRAW_CONNECTED) {
        /* the line's ends take turns in buf's vertices */
        clip[0] = calc(p, 0, 1, vtx, sx, sy, cx, cy);
        cur = 1;
        for (i = 0, j = 1; i < n; i++, j++) {
            clip[cur] = calc(p, j, 1, vtx + (cur * stride), sx, sy, cx, cy);
            if (clip[0] != BEYOND_FAR_CLIP && clip[1] != BEYOND_FAR_CLIP) {
                code = clip[0] + clip[1];
                sq = send(buf, LINE_ENDS, sq, code);
            }
            cur ^= 1;
        }
    } else if (attr & NJD_DRAW_FAN) {
        if (calc(p, 0, 1, vtx, sx, sy, cx, cy) != BEYOND_FAR_CLIP) {
            for (i = 0, j = 1; i < n; i++, j++) {
                code = calc(p, j, 1, vtx + stride, sx, sy, cx, cy);
                if (code != BEYOND_FAR_CLIP) {
                    sq = send(buf, LINE_ENDS, sq, code);
                }
            }
        }
    } else {
        for (i = 0, j = 0; i < n; i++, j += LINE_ENDS) {
            code = calc(p, j, LINE_ENDS, vtx, sx, sy, cx, cy);
            if (code != BEYOND_FAR_CLIP) {
                sq = send(buf, LINE_ENDS, sq, code);
            }
        }
    }
    njEndVertex(sq);
    njSetkmPointer();
}
