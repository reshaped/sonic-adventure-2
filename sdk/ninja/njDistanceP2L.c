#include <ninja.h>
#include <sg_xpt.h>

/* the distance squared below which p counts as on the line */
#define ON_LINE 0.025F

Float njDistanceP2L(NJS_POINT3 *p, NJS_LINE *l, NJS_POINT3 *cp)
{
    /* @hack redundant: register on t and d2 */
    register Float t;
    register Float d2;

    /* the point of l nearest p is l's point plus t times l's vector */
    t = ((l->vx * (p->x - l->px)) + (l->vy * (p->y - l->py)) +
         (l->vz * (p->z - l->pz))) /
        ((l->vx * l->vx) + (l->vy * l->vy) + (l->vz * l->vz));
    if (cp != NULL) {
        cp->x = l->px + (t * l->vx);
        cp->y = l->py + (t * l->vy);
        cp->z = l->pz + (t * l->vz);
        d2 = ((p->x - cp->x) * (p->x - cp->x)) +
             ((p->y - cp->y) * (p->y - cp->y)) +
             ((p->z - cp->z) * (p->z - cp->z));
    } else {
        d2 = ((p->x - (l->vx * t) - l->px) * (p->x - (l->vx * t) - l->px)) +
             ((p->y - (l->vy * t) - l->py) * (p->y - (l->vy * t) - l->py)) +
             ((p->z - (l->vz * t) - l->pz) * (p->z - (l->vz * t) - l->pz));
    }
    /* @hack spelling: an else after the arm that returns, where the call
     * after the if would do */
    if (d2 < ON_LINE) {
        return 0.0F;
    } else {
        return njSqrt(d2);
    }
}
