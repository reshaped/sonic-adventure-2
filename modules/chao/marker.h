/* glows drawn at a point of the garden, facing the camera, and the height
 * marker's task. the marker is a line that grows up from its pos until it
 * is scl.y tall */
#ifndef CHAO_MARKER_H
#define CHAO_MARKER_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;
void garden_draw_glow(NJS_POINT3 *position, Uint32 color, Float size);
void garden_draw_inverted_glow(NJS_POINT3 *position, Uint32 color, Float size);
void garden_height_marker_step(struct task *marker);
void garden_height_marker_draw(struct task *marker);
void garden_height_marker_destroy(struct task *marker);
#endif
