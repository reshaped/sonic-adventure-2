/* the lit near clippers' body, which njDirectClipLit.c compiles as
 * njDirectNearClipSub and as njDirectEnvNearClipSub. the includer defines
 * CLIP_TRIANGLE as the function's name. it defines FILL_CORNERS() to set
 * intensity, u and v in corners[0..2], the corners of source[0..2] */
#include <ninja.h>
#include <nj_direct_draw.h>
#include <sg_xpt.h>

void CLIP_TRIANGLE(NJS_DIRECT_VERTEX *source, Uint32 mask, Float first_x,
                   Float first_y, Float first_z, Float first_w, Float second_x,
                   Float second_y, Float second_z)
{
    struct direct_lit_corner triangle[DIRECT_TRIANGLE_CORNERS];
    struct direct_lit_screen_vertex vertices[DIRECT_MOST_VERTICES];
    /* @hack redundant: register on next, queue and count */
    register struct direct_lit_screen_vertex *next;
    struct direct_lit_screen_vertex *clipped;
    struct direct_lit_corner *corners;
    struct direct_lit_corner *current;
    struct direct_lit_corner *previous;
    register Uint32 *queue;
    const Float *screen;
    Int edges;
    register Int count;

    /* @hack spelling: queue, a copy of direct_queue held across the clip,
     * where direct_queue itself would do */
    queue = direct_queue;
    /* @hack spelling: corners, a pointer to triangle, where triangle itself
     * would do */
    corners = triangle;
    current = corners;
    /* @hack spelling: each corner's x, y, z and behind written through
     * current, where triangle[0] to triangle[2] would do */
    current->x = first_x;
    current->y = first_y;
    current->z = first_z;
    /* @hack spelling: each corner's bit of mask shifted down to bit 0, where
     * (mask & DIRECT_FIRST_BEHIND) != 0 and the same tests of the other two
     * bits would do */
    current->behind = (mask >> 2) & 1;
    current[1].x = second_x;
    current[1].y = second_y;
    current[1].z = second_z;
    current[1].behind = (mask >> 1) & 1;
    current[2].x = direct_point.x;
    current[2].y = direct_point.y;
    current[2].z = direct_point.z;
    current[2].behind = mask & 1;
    FILL_CORNERS();
    screen = &direct_light.scale_x;
    next = vertices;
    count = 0;
    previous = current + DIRECT_TRIANGLE_CORNERS - 1;
    /* @hack spelling: clipped, a copy of next, where vertices itself would
     * do */
    clipped = next;
    edges = DIRECT_TRIANGLE_CORNERS;
    /* each edge runs from previous to current, the first from the last corner.
     * where the edge crosses the near plane, njDirectClipAdjust writes the
     * projection of the crossing at next. where current is in front of the
     * plane, njDirectClipAdjust2 writes current's projection at next */
    do {
        /* @hack spelling: the two behind flags compared by ^, where != would
         * do */
        if (previous->behind ^ current->behind) {
            njDirectClipAdjust(previous, current, next, screen);
            ++count;
            ++next;
        }
        if (!current->behind) {
            njDirectClipAdjust2(current, next, screen);
            ++count;
            ++next;
        }
        previous = current;
        ++current;
    } while (--edges);
    queue = njDirectClipStore((Float *)clipped, queue, count);
    direct_queue = queue;
}
