#include <ninja.h>
#include <sg_xpt.h>

Float njDistancePL2PL(NJS_PLANE *pl1, NJS_PLANE *pl2, NJS_LINE *l)
{
    NJS_POINT3 p;
    Float d1;
    Float d2;
    Float d3;
    Float inv_det;
    /* @hack spelling: det_x, det_y and det_z, locals for the three
     * numerators, where each sum in its product with inv_det would do */
    Float det_x;
    Float det_y;
    Float det_z;

    if (njIsParalellPL2PL(pl1, pl2)) {
        p.x = pl1->px;
        p.y = pl1->py;
        p.z = pl1->pz;
        if (l != NULL) {
            l->px = 0.0F;
            l->py = 0.0F;
            l->pz = 0.0F;
            l->vx = 0.0F;
            l->vy = 0.0F;
            l->vz = 0.0F;
        }
        return njDistanceP2PL(&p, pl2, NULL);
    }
    if (l != NULL) {
        l->vx = (pl1->vy * pl2->vz) - (pl2->vy * pl1->vz);
        l->vy = (pl1->vz * pl2->vx) - (pl2->vz * pl1->vx);
        l->vz = (pl1->vx * pl2->vy) - (pl2->vx * pl1->vy);
        /* l's point is where pl1, pl2 and a third plane meet. the third
         * passes through pl2's point, and its vector is l's. plane k holds
         * the points q where vk . q = dk, for vk its vector and dk the
         * product vk . pk with its point pk. Cramer's rule solves the three.
         * inv_det is 1 over the determinant of the three vectors. det_x is
         * that determinant with d1, d2 and d3 in place of their x parts.
         * det_y and det_z replace the y and z parts likewise */
        d1 = (pl1->vx * pl1->px) + (pl1->vy * pl1->py) + (pl1->vz * pl1->pz);
        d2 = (pl2->vx * pl2->px) + (pl2->vy * pl2->py) + (pl2->vz * pl2->pz);
        d3 = (l->vx * pl2->px) + (l->vy * pl2->py) + (l->vz * pl2->pz);
        inv_det =
            1.0F / ((pl1->vx * pl2->vy * l->vz) + (pl1->vy * pl2->vz * l->vx) +
                    (pl1->vz * l->vy * pl2->vx) - (pl1->vz * pl2->vy * l->vx) -
                    (pl1->vy * pl2->vx * l->vz) - (pl1->vx * l->vy * pl2->vz));
        det_x = (d1 * pl2->vy * l->vz) + (pl1->vy * pl2->vz * d3) +
                (pl1->vz * l->vy * d2) - (pl1->vz * pl2->vy * d3) -
                (pl1->vy * d2 * l->vz) - (d1 * l->vy * pl2->vz);
        det_y = (pl1->vx * d2 * l->vz) + (d1 * pl2->vz * l->vx) +
                (pl1->vz * d3 * pl2->vx) - (pl1->vz * d2 * l->vx) -
                (d1 * pl2->vx * l->vz) - (pl1->vx * d3 * pl2->vz);
        det_z = (pl1->vx * pl2->vy * d3) + (pl1->vy * d2 * l->vx) +
                (d1 * l->vy * pl2->vx) - (d1 * pl2->vy * l->vx) -
                (pl1->vy * pl2->vx * d3) - (pl1->vx * l->vy * d2);
        l->px = inv_det * det_x;
        l->py = inv_det * det_y;
        l->pz = inv_det * det_z;
    }
    return 0.0F;
}
