/* lines and points, drawn at once or queued in batches that tasks draw, and
 * the constant material */
#ifndef DRAW_LINES_H
#define DRAW_LINES_H
#include <ninja.h>
#include <sg_xpt.h>

/* what line_queue draws: a line of two points, or from POINT_OPAQUE on a
 * point. each is opaque, translucent, or added to what lies behind it */
enum line_kind {
    LINE_OPAQUE,
    LINE_TRANSLUCENT,
    LINE_ADDITIVE,
    POINT_OPAQUE,
    POINT_TRANSLUCENT,
    POINT_ADDITIVE
};

void constant_material_set(Float alpha, Float red, Float green, Float blue);
/* draw a line from white at (x1, y1, z1) to pale blue at (x2, y2, z2) */
void line_draw(Float x1, Float y1, Float z1, Float x2, Float y2, Float z2);
/* draw a line from color1 at (x1, y1, z1) to color2 at (x2, y2, z2) */
void line_draw_colored(Float x1, Float y1, Float z1, NJS_COLOR *color1,
                       Float x2, Float y2, Float z2, NJS_COLOR *color2);
/* draw a line from white at from to pale blue at to */
void line_draw_points(NJS_POINT3 *from, NJS_POINT3 *to);
/* allocate the batches with syMalloc, unless they are allocated already.
 * where syMalloc fails they stay unallocated, and a later call tries again.
 * line_batches_free frees them */
void line_batches_alloc(void);
void line_batches_free(void);
/* draw a line of kind now, from points[0] in colors[0] to points[1] in
 * colors[1] */
void line_draw_now(NJS_POINT3 *points, NJS_COLOR *colors, enum line_kind kind);
/* queue a line of kind from points[0] to points[1], or a point at
 * points[0]. a batch holds lines or points of one kind in one color,
 * colors[0]. its task draws it for each camera, and empties it after the
 * last. where no batch of the kind and color has room, line_queue gives a
 * free batch a new task. it draws now where none is free, or where
 * task_create fails. a line drawn now ends in colors[1]. the batches must be
 * allocated, unchecked */
void line_queue(NJS_POINT3 *points, NJS_COLOR *colors, enum line_kind kind);

#endif
