#include <ninja.h>
#include <sg_xpt.h>

Int njCheckBBox(NJS_POINT3 *min_h1, NJS_POINT3 *max_h1, NJS_POINT3 *min_h2,
                NJS_POINT3 *max_h2)
{
    /* @hack spelling: lo and hi, locals for the ends of the span that holds
     * both boxes on an axis, where each conditional in the axis's comparison
     * would do */
    Float lo;
    Float hi;
    Int x;
    Int y;
    Int z;

    /* lo to hi is the span that holds both boxes on an axis. the boxes
     * overlap on it where their sizes sum to more than the span */
    lo = min_h1->x < min_h2->x ? min_h1->x : min_h2->x;
    hi = max_h1->x > max_h2->x ? max_h1->x : max_h2->x;
    /* @hack redundant: ? 1 : 0 on this comparison and on y's */
    x = max_h1->x - min_h1->x + max_h2->x - min_h2->x > hi - lo ? 1 : 0;
    lo = min_h1->y < min_h2->y ? min_h1->y : min_h2->y;
    hi = max_h1->y > max_h2->y ? max_h1->y : max_h2->y;
    y = max_h1->y - min_h1->y + max_h2->y - min_h2->y > hi - lo ? 1 : 0;
    lo = min_h1->z < min_h2->z ? min_h1->z : min_h2->z;
    hi = max_h1->z > max_h2->z ? max_h1->z : max_h2->z;
    z = max_h1->z - min_h1->z + max_h2->z - min_h2->z > hi - lo;
    /* @hack spelling: the three results joined by &, where && would do */
    return x & y & z;
}
