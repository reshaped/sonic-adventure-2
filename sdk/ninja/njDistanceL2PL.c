#include <ninja.h>
#include <sg_xpt.h>

Float njDistanceL2PL(NJS_LINE *l, NJS_PLANE *pl, NJS_POINT3 *cp)
{
    NJS_POINT3 p;
    /* @hack redundant: register on d, height, slope and t */
    /* @hack spelling: d, height and slope, locals for the three sums that give
     * t, where the sums in t's expression would do */
    register Float d;
    register Float height;
    register Float slope;
    register Float t;

    if (njIsParalellL2PL(l, pl)) {
        p.x = l->px;
        p.y = l->py;
        p.z = l->pz;
        return njDistanceP2PL(&p, pl, cp);
    }
    if (cp != NULL) {
        /* pl holds the points q where v . q + d = 0, for v its vector.
         * height is v . p + d at l's point p. slope is what one step of l's
         * vector adds to it. so l crosses pl t steps from p */
        d = -((pl->vx * pl->px) + (pl->vy * pl->py) + (pl->vz * pl->pz));
        height = (pl->vx * l->px) + (pl->vy * l->py) + (pl->vz * l->pz) + d;
        slope = (pl->vx * l->vx) + (pl->vy * l->vy) + (pl->vz * l->vz);
        t = -height / slope;
        cp->x = l->px + (t * l->vx);
        cp->y = l->py + (t * l->vy);
        cp->z = l->pz + (t * l->vz);
    }
    return 0.0F;
}
