#include <ninja.h>
#include <sg_xpt.h>

Float njDistanceP2PL(NJS_POINT3 *p, NJS_PLANE *pl, NJS_POINT3 *cp)
{
    /* @hack redundant: register on d, height, length2 and t */
    /* @hack spelling: d, a local for the inner product of pl's point and
     * vector, negated, where the product subtracted in height's sum would do */
    register Float d;
    register Float height;
    register Float length2;
    register Float t;

    /* pl holds the points q where v . q + d = 0, for v its vector. height is
     * v . p + d, p's signed distance from pl times the length of v */
    d = -((pl->vx * pl->px) + (pl->vy * pl->py) + (pl->vz * pl->pz));
    height = (pl->vx * p->x) + (pl->vy * p->y) + (pl->vz * p->z) + d;
    length2 = (pl->vx * pl->vx) + (pl->vy * pl->vy) + (pl->vz * pl->vz);
    if (cp != NULL) {
        t = -height / length2;
        cp->x = p->x + (t * pl->vx);
        cp->y = p->y + (t * pl->vy);
        cp->z = p->z + (t * pl->vz);
    }
    return njAbs(height) * njInvertSqrt(length2);
}
