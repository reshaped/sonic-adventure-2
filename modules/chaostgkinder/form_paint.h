/* the edges of a quad of four NJS_POLYGON_VTX, a screen-space triangle strip.
 * vertices 0 and 1 make its left edge, and 2 and 3 its right. vertices 0 and
 * 2 make its top, and 1 and 3 its bottom. FRAME_EDGES draws a black frame 2
 * pixels wide just inside left, top, right and bottom. it starts from the
 * strip v drew last, whose left, right and top must already be the frame's.
 * each side after is the side before with two edges moved */
#ifndef CHAOSTGKINDER_FORM_PAINT_H
#define CHAOSTGKINDER_FORM_PAINT_H

#define STRIP_LEFT(v) (v)[0].x = (v)[1].x
#define STRIP_RIGHT(v) (v)[2].x = (v)[3].x
#define STRIP_TOP(v) (v)[0].y = (v)[2].y
#define STRIP_BOTTOM(v) (v)[1].y = (v)[3].y
#define STRIP_DEPTH(v) (v)[0].z = (v)[1].z = (v)[2].z = (v)[3].z
#define STRIP_COLOR(v) (v)[0].col = (v)[1].col = (v)[2].col = (v)[3].col

#define FRAME_EDGES(v, left, top, right, bottom, depth)                        \
    STRIP_DEPTH(v) = (depth) + 0.01F;                                          \
    STRIP_COLOR(v) = 0xff000000;                                               \
    STRIP_BOTTOM(v) = (top) + 2.0F;                                            \
    njDrawPolygon(v, 4, FALSE);                                                \
    STRIP_TOP(v) = (bottom) - 2.0F;                                            \
    STRIP_BOTTOM(v) = (bottom);                                                \
    njDrawPolygon(v, 4, FALSE);                                                \
    STRIP_TOP(v) = (top);                                                      \
    STRIP_RIGHT(v) = (left) + 2.0F;                                            \
    njDrawPolygon(v, 4, FALSE);                                                \
    STRIP_LEFT(v) = (right) - 2.0F;                                            \
    STRIP_RIGHT(v) = (right);                                                  \
    njDrawPolygon(v, 4, FALSE)

#endif
