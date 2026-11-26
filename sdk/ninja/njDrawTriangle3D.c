#include <ninja.h>
#include <nj_draw3d.h>
#include <nj_km.h>
#include <sg_xpt.h>

/* a triangle's points, and those of a fan's triangle beside point 0 */
#define TRIANGLE_POINTS 3
#define FAN_POINTS 2

void njDrawTriangle3D(NJS_POINT3COL *p, Int n, Uint32 attr)
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
    Int cur;
    Int stride;
    Uint8 buf[TRIANGLE_POINTS * sizeof(struct draw3d_tex_vertex)];
    Sint32 clip[TRIANGLE_POINTS];
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
        if (attr & NJD_FILL) {
            send = njTri_Filled_With_Tex;
        } else {
            send = njTri_Wired_With_Tex;
        }
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
        if (attr & NJD_FILL) {
            send = njTri_Filled;
        } else {
            send = njTri_Wired;
        }
        calc = njDCalcPoint3DP2;
        stride = sizeof(struct draw3d_vertex);
    }
    if (attr & NJD_DRAW_CONNECTED) {
        /* the triangle's points take turns in buf's vertices */
        clip[0] = calc(p, 0, 1, vtx, sx, sy, cx, cy);
        /* @hack type: the second vertex's address summed as an integer,
         * where vtx + stride would do */
        clip[1] =
            calc(p, 1, 1, (Uint8 *)((Uint32)vtx + stride), sx, sy, cx, cy);
        cur = 2;
        for (i = 0; i < n; i++) {
            clip[cur] = calc(p, i + 2, 1, vtx + (cur * stride), sx, sy, cx, cy);
            if (clip[0] != BEYOND_FAR_CLIP && clip[1] != BEYOND_FAR_CLIP &&
                clip[2] != BEYOND_FAR_CLIP) {
                code = clip[0] + clip[1] + clip[2];
                sq = send(buf, TRIANGLE_POINTS, sq, code);
            }
            cur++;
            if (cur > TRIANGLE_POINTS - 1) {
                cur = 0;
            }
        }
    } else if (attr & NJD_DRAW_FAN) {
        /* @hack redundant: clip[0] set from point 0's projection, which no
         * test of the fan reads */
        if ((clip[0] = calc(p, 0, 1, vtx, sx, sy, cx, cy)) != BEYOND_FAR_CLIP) {
            for (i = 0; i < n; i++) {
                /* @hack spelling: j, a local for the first point's index,
                 * where (i * FAN_POINTS) + 1 in the call would do */
                j = (i * FAN_POINTS) + 1;
                /* @hack type: the second vertex's address summed as an
                 * integer, where vtx + stride would do */
                code = calc(p, j, FAN_POINTS, (Uint8 *)((Uint32)vtx + stride),
                            sx, sy, cx, cy);
                if (code != BEYOND_FAR_CLIP) {
                    /* @bug logic: a fan's filled triangle goes uncut where
                     * point 0 alone lies at or nearer than the near clip. the
                     * send takes only the code of the call that projects
                     * points j and j + 1 (MOV R0,R6 at 0x8c11b220, MOV R6,R7
                     * at 0x8c11b222, the call at 0x8c11b228). point 0's code
                     * is stored at 0x8c11b1f6 and read nowhere. the connected
                     * path adds every point's code (ADD R3,R6 at 0x8c11b1a6
                     * and 0x8c11b1aa) */
                    sq = send(buf, TRIANGLE_POINTS, sq, code);
                }
            }
        }
    } else {
        for (i = 0; i < n; i++) {
            code = calc(p, i * TRIANGLE_POINTS, TRIANGLE_POINTS, vtx, sx, sy,
                        cx, cy);
            if (code != BEYOND_FAR_CLIP) {
                sq = send(buf, TRIANGLE_POINTS, sq, code);
            }
        }
    }
    njEndVertex(sq);
    njSetkmPointer();
}
