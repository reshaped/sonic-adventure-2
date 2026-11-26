/* the records Ninja's direct drawer shares with its SH-4 transform and clipping
 * kernels. the kernels read each field at a fixed offset */
#ifndef NJ_DIRECT_DRAW_H
#define NJ_DIRECT_DRAW_H
#include <ninja.h>
#include <nj_direct.h>
#include <sg_xpt.h>

struct direct_lighting {
    NJS_VECTOR direction;
    Float negative_intensity;
    Float ambient_bias;
    Float scale_x;
    Float scale_y;
    Float center_x;
    Float center_y;
    Float environment_scale;
    Float red;
    Float green;
    Float blue;
    Float reserved[3];
};

struct direct_frustum {
    Float far_z;
    Float near_z;
    Float scale_x;
    Float scale_y;
    Float right;
    Float left;
    Float bottom;
    Float top;
};
/* a triangle's corner as the D8 near clipper holds it. a D8 vertex packs its
 * texture coordinates into one word, as the high halves of two floats. the
 * clipper unpacks the word's high half into coordinate[0] and its low half
 * into coordinate[1] */
struct direct_d8_corner {
    Float z;
    Float x;
    Float y;
    Uint32 color;
    union {
        Float coordinate[2];
        Uint16 half[4];
    } uv;
    Bool behind;
    Uint32 padding;
};
struct direct_d8_screen_vertex {
    Float x;
    Float y;
    Float inverse_z;
    Float u;
    Float v;
    Uint32 color;
};

/* a triangle's corner as the lit near clippers hold it. behind is 1 where
 * the corner lies behind the near plane */
struct direct_lit_corner {
    Float z;
    Float x;
    Float y;
    Float intensity;
    Float u;
    Float v;
    Bool behind;
    Uint32 padding;
};
struct direct_lit_screen_vertex {
    Float x;
    Float y;
    Float inverse_z;
    Float intensity;
    Float u;
    Float v;
};
/* the lit near clippers' projections. screen holds scale_x, scale_y, center_x
 * and center_y, as struct direct_lighting lays them out. njDirectClipAdjust
 * writes at out the vertex where the edge from first to second crosses
 * z = 1. the vertex holds the crossing's screen x and y and an inverse z of 1.
 * its intensity, u and v are interpolated. njDirectClipAdjust2 writes at out
 * corner's own vertex, projected, with 1/z as its inverse z */
void njDirectClipAdjust(const struct direct_lit_corner *first,
                        const struct direct_lit_corner *second,
                        struct direct_lit_screen_vertex *out,
                        const Float *screen);
void njDirectClipAdjust2(const struct direct_lit_corner *corner,
                         struct direct_lit_screen_vertex *out,
                         const Float *screen);
void njDirectCalcLight(const Float *light, const NJS_DIRECT_VERTEX *source,
                       struct direct_lit_corner *corners);
void njDirectCalcEnvLight(const Float *light, const NJS_DIRECT_VERTEX *source,
                          struct direct_lit_corner *corners, Float half);
/* queue count struct direct_lit_screen_vertex records from vertices at queue,
 * as one strip that the last ends. count is DIRECT_TRIANGLE_CORNERS or
 * DIRECT_MOST_VERTICES. where it is DIRECT_MOST_VERTICES, the fourth goes
 * ahead of the third, so that the four make one strip. return queue past the
 * strip */
Uint32 *njDirectClipStore(const Float *vertices, Uint32 *queue, Int count);

extern struct direct_lighting direct_light;
extern struct direct_frustum direct_clip;
/* the light the lit near clippers take: its direction in the model's space,
 * and its ambient less 1. the near clips set it from the light the clip
 * drawers pass in FV4 */
extern Float direct_clip_light[4];
extern NJS_POINT3 direct_point;
extern Uint32 *direct_queue;

/* the near clippers. each cuts a triangle that crosses the near plane, z = 1,
 * at the plane. it queues what is left at direct_queue. source points at the
 * triangle's three vertices. the first two corners come as arguments, and the
 * third in direct_point, where the transform kernels leave it. mask holds the
 * DIRECT_*_BEHIND bits of the corners behind the plane. first_w is unread.
 * under SHC's convention it takes FR7, so that second_x arrives in FR8.
 * njDirectCalcPointLoop and njDirectCalcPointLoopD8 pass the second corner
 * there */
void njDirectNearClipSubD8(NJS_DIRECT_VERTEX_D8 *source, Uint32 mask,
                           Float first_x, Float first_y, Float first_z,
                           Float first_w, Float second_x, Float second_y,
                           Float second_z);
void njDirectNearClipSub(NJS_DIRECT_VERTEX *source, Uint32 mask, Float first_x,
                         Float first_y, Float first_z, Float first_w,
                         Float second_x, Float second_y, Float second_z);
void njDirectEnvNearClipSub(NJS_DIRECT_VERTEX *source, Uint32 mask,
                            Float first_x, Float first_y, Float first_z,
                            Float first_w, Float second_x, Float second_y,
                            Float second_z);
/* a near clipper's mask bits, each set where its corner is behind the near
 * plane */
#define DIRECT_FIRST_BEHIND 4
#define DIRECT_SECOND_BEHIND 2
#define DIRECT_THIRD_BEHIND 1
/* the corners of a near clipper's triangle, and the most vertices the near
 * plane leaves of it */
#define DIRECT_TRIANGLE_CORNERS 3
#define DIRECT_MOST_VERTICES 4
void njDirectCalcPointLoopD8(NJS_DIRECT_VERTEX_D8 *vertices, Int count,
                             NJS_POINT3 *third);
void njDirectCalcPointLoop(NJS_DIRECT_VERTEX *vertices, Int count,
                           NJS_POINT3 *third, Int environment);

Sint32 njDirectClip(Float *model, Float *frustum);
Uint32 *njDirectDraw(const struct direct_lighting *light, void **cursor,
                     Uint32 *queue, Uint32 count);
Uint32 *njDirectDrawD8(const struct direct_lighting *light, void **cursor,
                       Uint32 *queue, Uint32 count);
Uint32 *njDirectDrawClip(const struct direct_lighting *light, void **cursor,
                         Uint32 *queue, Uint32 count);
Uint32 *njDirectDrawD8Clip(const struct direct_lighting *light, void **cursor,
                           Uint32 *queue, Uint32 count);
Uint32 *njDirectEnvNearClip(NJS_DIRECT_GLOBAL *strip, Sint32 count,
                            Uint32 *queue, Float light_x, Float light_y,
                            Float light_z, Float ambient);
Uint32 *njDirectNearClip(NJS_DIRECT_GLOBAL *strip, Sint32 span, Uint32 *output,
                         NJS_DIRECT_VERTEX *start, Float light_x, Float light_y,
                         Float light_z, Float ambient);
Uint32 *njDirectD8NearClip(NJS_DIRECT_GLOBAL_D8 *strip, Sint32 span,
                           Uint32 *output, NJS_DIRECT_VERTEX_D8 *start);

#endif
