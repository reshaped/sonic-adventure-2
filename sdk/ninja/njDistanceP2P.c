#include <ninja.h>
#include <sg_xpt.h>

/* the distance squared below which the two points count as one */
#define SAME_POINT 0.025F

Float njDistanceP2P(NJS_POINT3 *p1, NJS_POINT3 *p2)
{
    Float d2;

    d2 = ((p1->x - p2->x) * (p1->x - p2->x)) +
         ((p1->y - p2->y) * (p1->y - p2->y)) +
         ((p1->z - p2->z) * (p1->z - p2->z));
    /* @hack spelling: an else after the arm that returns, where the call
     * after the if would do */
    if (d2 < SAME_POINT) {
        return 0.0F;
    } else {
        return njSqrt(d2);
    }
}
