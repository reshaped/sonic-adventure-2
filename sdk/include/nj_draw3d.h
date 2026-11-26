/* what Ninja's lines and triangles in the world share with njDrawPolygon3DEx
 * and njDrawTexture3DEx. their point projectors write the vertices below, and
 * their senders read them */
#ifndef NJ_DRAW3D_H
#define NJ_DRAW3D_H
#include <ninja.h>
#include <sg_xpt.h>

/* a vertex as njDCalcPoint3DP2 and njCalcPoint3DP project it: its packed
 * color, ooz and place on the screen. (x, y, z, w) is the point through the
 * current matrix, and ooz is w / z. so ooz is 1 / z for an affine current
 * matrix */
struct draw3d_vertex {
    Uint32 col;
    Float ooz;
    Float sy;
    Float sx;
};
/* a vertex as njDCalcPoint3DT2 and njCalcPointT3DP project it: a struct
 * draw3d_vertex with a texture coordinate. u and v count 1 to the texture's
 * side */
struct draw3d_tex_vertex {
    Uint32 col;
    Float v;
    Float u;
    Float ooz;
    Float sy;
    Float sx;
};
/* the shifts of a packed ARGB color's channels, and a channel's bits */
#define ALPHA_SHIFT 24
#define RED_SHIFT 16
#define GREEN_SHIFT 8
#define CHANNEL_MASK 0xff
/* the words of u and v in a sent vertex's 32-byte block, which an untextured
 * vertex leaves unwritten */
#define UV_WORDS 2

/* project count points of p, 1 or more, from point index on, into buf as
 * struct draw3d_vertex records. the current matrix takes a point to
 * (x, y, z, w). its place on the screen is x * sx * ooz + cx across. it is
 * y * sy * ooz + cy down. point k's color is p->col[k]. p->col must not be
 * null. the bug tag in njDCalcPoint3D2.c says what the call reads for a null
 * one.
 *
 * the call returns -1 at the first point beyond _nj_far_clip_. it writes no
 * vertex for that point or those after it. otherwise it returns 1 where a point
 * lies at or nearer than _nj_near_clip_, else 0 */
Sint32 njDCalcPoint3DP2(NJS_POINT3COL *p, Int index, Int count, void *buf,
                        Float sx, Float sy, Float cx, Float cy);
/* njDCalcPoint3DP2 into struct draw3d_tex_vertex records. point k's u and v are
 * p->tex[k]'s two halfwords over 256, so p->tex must not be null. p->col must
 * not be null either. the bug tag in njDCalcPoint3D2.c says what the call reads
 * for a null one */
Sint32 njDCalcPoint3DT2(NJS_POINT3COL *p, Int index, Int count, void *buf,
                        Float sx, Float sy, Float cx, Float cy);
/* project count points from the start of p, into the same records and with
 * the same clipping result as the indexed projectors. njDCalcPoint3DP uses
 * white when p->col is null. njDCalcPoint3DT requires p->col and p->tex. */
Sint32 njDCalcPoint3DP(NJS_POINT3COL *p, Int count, void *buf, Float sx,
                       Float sy, Float cx, Float cy);
Sint32 njDCalcPoint3DT(NJS_POINT3COL *p, Int count, void *buf, Float sx,
                       Float sy, Float cx, Float cy);
/* what a point projector returns at a point beyond _nj_far_clip_ */
#define BEYOND_FAR_CLIP (-1)
/* a point projector, such as njDCalcPoint3DP2, and a vertex sender, such as
 * njTri_Filled. the line and triangle draws pick one of each */
typedef Sint32 (*point_projector)(NJS_POINT3COL *p, Int index, Int count,
                                  void *buf, Float sx, Float sy, Float cx,
                                  Float cy);
typedef Uint32 *(*vertex_sender)(void *buf, Int count, Uint32 *sq, Sint32 clip);
/* send buf's count vertices as the outline of a polygon, from sq on. buf
 * holds struct draw3d_vertex records for njTri_Wired, and struct
 * draw3d_tex_vertex records for njTri_Wired_With_Tex. the call returns the
 * address after the last vertex sent. clip is never read.
 *
 * each edge runs to a vertex from the one before, and the first from the last
 * vertex. a vertex is in front where its ooz lies between 0 and 1 over
 * _nj_near_clip_. an edge with both ends in front goes as a quad, a strip of
 * four vertices. one with one end in front goes as a quad cut at the near
 * clip. one with neither is dropped. each vertex of a textured quad takes its
 * end's u and v */
Uint32 *njTri_Wired(void *buf, Int count, Uint32 *sq, Sint32 clip);
Uint32 *njTri_Wired_With_Tex(void *buf, Int count, Uint32 *sq, Sint32 clip);
/* send buf's count vertices as one filled polygon, from sq on. the call
 * returns the address after the last vertex sent. buf holds struct
 * draw3d_vertex records for njTri_Filled, and struct draw3d_tex_vertex
 * records for njTri_Filled_With_Tex. count must be 3 or more.
 *
 * the vertices go as one strip, taken from both ends of buf inward: 0,
 * count - 1, 1, count - 2 and so on. so a convex polygon whose vertices run
 * around it in order fills. with clip at 0 or below, the strip goes as it
 * stands. its last vertex goes twice, the second time to end the strip.
 *
 * with clip above 0, the strip's triangles go one at a time. a vertex is in
 * front where its ooz lies between 0 and 1 over _nj_near_clip_. a triangle
 * with its three vertices in front joins a strip of such triangles. one with
 * one or two in front goes cut at the near clip, as a strip of its own. one
 * with none is dropped */
Uint32 *njTri_Filled(void *buf, Int count, Uint32 *sq, Sint32 clip);
Uint32 *njTri_Filled_With_Tex(void *buf, Int count, Uint32 *sq, Sint32 clip);
#endif
