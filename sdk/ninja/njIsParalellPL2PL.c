#include <ninja.h>
#include <sg_xpt.h>

/* the cosine magnitude, of pl1's vector to pl2's, above which the planes
 * count as parallel */
#define PARALLEL_COS_MIN 0.975F

Bool njIsParalellPL2PL(NJS_PLANE *pl1, NJS_PLANE *pl2)
{
    /* @hack spelling: cosine, a local for the quotient, where the quotient
     * in the comparison would do */
    Float cosine;

    cosine = ((pl1->vx * pl2->vx) + (pl1->vy * pl2->vy) + (pl1->vz * pl2->vz)) /
             (njSqrt((pl1->vx * pl1->vx) + (pl1->vy * pl1->vy) +
                     (pl1->vz * pl1->vz)) *
              njSqrt((pl2->vx * pl2->vx) + (pl2->vy * pl2->vy) +
                     (pl2->vz * pl2->vz)));
    return njAbs(cosine) > PARALLEL_COS_MIN;
}
