#include <ninja.h>
#include <sg_xpt.h>

/* the cosine magnitude, of l's vector to pl's, below which l counts as
 * parallel to pl. a line along a plane is perpendicular to the plane's
 * vector */
#define PARALLEL_COS_MAX 0.025F

Bool njIsParalellL2PL(NJS_LINE *l, NJS_PLANE *pl)
{
    /* @hack spelling: cosine, a local for the quotient, where the quotient
     * in the comparison would do */
    Float cosine;

    cosine =
        ((l->vx * pl->vx) + (l->vy * pl->vy) + (l->vz * pl->vz)) /
        (njSqrt((l->vx * l->vx) + (l->vy * l->vy) + (l->vz * l->vz)) *
         njSqrt((pl->vx * pl->vx) + (pl->vy * pl->vy) + (pl->vz * pl->vz)));
    return njAbs(cosine) < PARALLEL_COS_MAX;
}
