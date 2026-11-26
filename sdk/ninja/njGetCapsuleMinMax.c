#include <ninja.h>
#include <sg_xpt.h>

void njGetCapsuleMinMax(NJS_CAPSULE *hull, NJS_POINT3 *min, NJS_POINT3 *max)
{
    Float r;
    Float min1x;
    Float min1y;
    Float min1z;
    Float max1x;
    Float max1y;
    Float max1z;
    Float min2x;
    Float min2y;
    Float min2z;
    Float max2x;
    Float max2y;
    Float max2z;

    r = hull->r;
    min1x = hull->c1.x - r;
    min1y = hull->c1.y - r;
    min1z = hull->c1.z - r;
    max1x = hull->c1.x + r;
    max1y = hull->c1.y + r;
    max1z = hull->c1.z + r;
    min2x = hull->c2.x - r;
    min2y = hull->c2.y - r;
    min2z = hull->c2.z - r;
    max2x = hull->c2.x + r;
    max2y = hull->c2.y + r;
    max2z = hull->c2.z + r;
    min->x = min1x < min2x ? min1x : min2x;
    min->y = min1y < min2y ? min1y : min2y;
    min->z = min1z < min2z ? min1z : min2z;
    max->x = max1x > max2x ? max1x : max2x;
    max->y = max1y > max2y ? max1y : max2y;
    max->z = max1z > max2z ? max1z : max2z;
}
