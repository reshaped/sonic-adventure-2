#include <ninja.h>
#include <sg_xpt.h>

void njGetBoxMinMax(NJS_BOX *hull, NJS_POINT3 *min, NJS_POINT3 *max)
{
    Int i;

    /* @hack spelling: v[0] copied to min and to max a member at a time, where
     * *min = hull->v[0] and *max = hull->v[0] would do */
    min->x = hull->v[0].x;
    min->y = hull->v[0].y;
    min->z = hull->v[0].z;
    max->x = hull->v[0].x;
    max->y = hull->v[0].y;
    max->z = hull->v[0].z;
    for (i = 1; i < (Int)(sizeof(hull->v) / sizeof(hull->v[0])); i++) {
        if (hull->v[i].x < min->x) {
            min->x = hull->v[i].x;
        }
        if (hull->v[i].y < min->y) {
            min->y = hull->v[i].y;
        }
        if (hull->v[i].z < min->z) {
            min->z = hull->v[i].z;
        }
        if (hull->v[i].x > max->x) {
            max->x = hull->v[i].x;
        }
        if (hull->v[i].y > max->y) {
            max->y = hull->v[i].y;
        }
        if (hull->v[i].z > max->z) {
            max->z = hull->v[i].z;
        }
    }
}
