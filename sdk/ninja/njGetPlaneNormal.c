#include <ninja.h>

void njGetPlaneNormal(NJS_POINT3 *p, NJS_VECTOR *v)
{
    v->x = ((p[1].y - p[0].y) * (p[2].z - p[0].z)) -
           ((p[1].z - p[0].z) * (p[2].y - p[0].y));
    v->y = ((p[1].z - p[0].z) * (p[2].x - p[0].x)) -
           ((p[1].x - p[0].x) * (p[2].z - p[0].z));
    v->z = ((p[1].x - p[0].x) * (p[2].y - p[0].y)) -
           ((p[1].y - p[0].y) * (p[2].x - p[0].x));
}
