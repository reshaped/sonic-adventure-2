#include <ninja.h>
#include <sg_xpt.h>

/* the slack of the tests of direction and of position on a segment, as
 * njCollisionCheckBC's comment gives each. its tests of a distance against r
 * or reach take none */
#define TOLERANCE 0.025F
/* a box's faces, and the corners and edges of each */
#define FACES 6
#define FACE_CORNERS 4

/* NORMALIZE(v, inv) scales v to unit length, and sets inv to 1 over v's old
 * length */
#ifdef NONMATCHING
/* this side keeps the console's bits. each __builtin_fmaf below is one of
 * SHC's FMACs. SUM3 is a * b + c * d + e * f in SHC's order. SHC's code rounds
 * c * d. FMACs then fuse a * b, then e * f, into that sum */
#define SUM3(a, b, c, d, e, f)                                                 \
    __builtin_fmaf(e, f, __builtin_fmaf(a, b, (c) * (d)))
#define NORMALIZE(v, inv)                                                      \
    {                                                                          \
        (inv) = 1.0F / njSqrt(SUM3((v).x, (v).x, (v).y, (v).y, (v).z, (v).z)); \
        (v).x *= (inv);                                                        \
        (v).y *= (inv);                                                        \
        (v).z *= (inv);                                                        \
    }
#define DISTANCE(a, b)                                                         \
    njSqrt(SUM3((a).x - (b).x, (a).x - (b).x, (a).y - (b).y, (a).y - (b).y,    \
                (a).z - (b).z, (a).z - (b).z))
#else
#define NORMALIZE(v, inv)                                                      \
    {                                                                          \
        (inv) = 1.0F /                                                         \
                njSqrt(((v).x * (v).x) + ((v).y * (v).y) + ((v).z * (v).z));   \
        (v).x *= (inv);                                                        \
        (v).y *= (inv);                                                        \
        (v).z *= (inv);                                                        \
    }
#define DISTANCE(a, b)                                                         \
    njSqrt((((a).x - (b).x) * ((a).x - (b).x)) +                               \
           (((a).y - (b).y) * ((a).y - (b).y)) +                               \
           (((a).z - (b).z) * ((a).z - (b).z)))
#endif

/* 0 where the box's and the capsule's bounding boxes, as njGetBoxMinMax and
 * njGetCapsuleMinMax give them, don't overlap. otherwise 1 where the face test
 * or the end test below passes, else 0. the capsule's ends are its centers c1
 * and c2, and its axis is the line through them. two unit vectors count as the
 * same where their x, y and z each differ by TOLERANCE or less.
 *
 * the face test passes where some face that the axis isn't parallel to
 * passes. the axis is parallel to a face where its cosine to the normal is
 * within TOLERANCE of 0. p is where the axis meets the face's plane. p lies
 * between the ends where its distances to them sum to within TOLERANCE of
 * the ends' distance. an edge holds p where two things hold. the direction
 * from p to the edge's line is the normal of the face across the edge. p lies
 * within r of an end, or between the ends. q is the point of the edge's line
 * nearest p. q lies on the edge where its distances to the corners sum to at
 * most the edge's length plus twice TOLERANCE. reach is the root of r squared
 * less the square of p's distance to the nearer end. the face passes in three
 * cases:
 * - every edge holds p
 * - each edge that doesn't hold p has q on it and within reach of p
 * - each edge that doesn't hold p has q off it and a corner within reach of p
 *
 * an end lies behind a face where the face's normal points from the end to
 * the face's plane. otherwise it lies in front, as an end on the plane does.
 * a front plane is near the end where it lies within r. an end's sum is the
 * sum of its distances to its front planes. the end test needs an end with
 * every front plane near. it then passes where either end has fewer than two
 * near planes. it also passes where either end has two, and a sum under r
 * times the root of 2 */
Int njCollisionCheckBC(NJS_BOX *box, NJS_CAPSULE *capsule)
{
    NJS_POINT3 box_min;
    NJS_POINT3 box_max;
    NJS_POINT3 capsule_min;
    NJS_POINT3 capsule_max;

    njGetBoxMinMax(box, &box_min, &box_max);
    njGetCapsuleMinMax(capsule, &capsule_min, &capsule_max);
    if (!njCheckBBox(&box_min, &box_max, &capsule_min, &capsule_max)) {
        return 0;
    }
    {
        /* the corners of each face, in turn around it */
        Int corner[FACES][FACE_CORNERS] = {
            {0, 1, 2, 3}, {4, 7, 6, 5}, {0, 3, 7, 4},
            {2, 1, 5, 6}, {0, 4, 5, 1}, {3, 2, 6, 7},
        };
        NJS_VECTOR normal[FACES]; /* each face's unit normal, out of the box */
        /* across[i][k] is the face across face i's edge from its corner k to
         * the next */
        Int across[FACES][FACE_CORNERS] = {
            {4, 3, 5, 2}, {2, 5, 3, 4}, {0, 5, 1, 4},
            {0, 4, 1, 5}, {2, 1, 3, 0}, {0, 3, 1, 2},
        };
        NJS_VECTOR axis;
        Float length;
        Float inv_length;
        Float offset;
        Float t;
        Float cosine;
        Int i;
        Int k;
        Int a;
        Int b;
        Int c1_inside;
        Int c2_inside;
        Int c1_near;
        Int c2_near;
        Int c1_behind;
        Int c2_behind;
        Float c1_sum;
        Float c2_sum;
        Int within;
        Int near_edges;
        Int near_corners;
        Int holding_edges;

        c1_inside = 1;
        c2_inside = 1;
        c1_near = 0;
        c2_near = 0;
        c1_behind = 0;
        c2_behind = 0;
        c1_sum = 0.0F;
        c2_sum = 0.0F;
        length = njDistanceP2P(&capsule->c1, &capsule->c2);
        axis.x = capsule->c2.x - capsule->c1.x;
        axis.y = capsule->c2.y - capsule->c1.y;
        axis.z = capsule->c2.z - capsule->c1.z;
        NORMALIZE(axis, inv_length);
        /* v0, v1 and v2 are face i's first three corners. normal[i] is the
         * unit vector along (v2 - v0) x (v1 - v0) */
        for (i = 0; i < FACES; i++) {
            /* @hack spelling: each part of the normal as minus one product
             * plus another, where the second less the first would do */
#ifdef NONMATCHING
            /* this side keeps the console's bits. SHC's code rounds the negated
             * product, then fuses the other into it by FMAC */
            normal[i].x = __builtin_fmaf(
                box->v[corner[i][1]].z - box->v[corner[i][0]].z,
                box->v[corner[i][2]].y - box->v[corner[i][0]].y,
                -((box->v[corner[i][1]].y - box->v[corner[i][0]].y) *
                  (box->v[corner[i][2]].z - box->v[corner[i][0]].z)));
            normal[i].y = __builtin_fmaf(
                box->v[corner[i][1]].x - box->v[corner[i][0]].x,
                box->v[corner[i][2]].z - box->v[corner[i][0]].z,
                -((box->v[corner[i][1]].z - box->v[corner[i][0]].z) *
                  (box->v[corner[i][2]].x - box->v[corner[i][0]].x)));
            normal[i].z = __builtin_fmaf(
                box->v[corner[i][1]].y - box->v[corner[i][0]].y,
                box->v[corner[i][2]].x - box->v[corner[i][0]].x,
                -((box->v[corner[i][1]].x - box->v[corner[i][0]].x) *
                  (box->v[corner[i][2]].y - box->v[corner[i][0]].y)));
#else
            normal[i].x = -((box->v[corner[i][1]].y - box->v[corner[i][0]].y) *
                            (box->v[corner[i][2]].z - box->v[corner[i][0]].z)) +
                          ((box->v[corner[i][1]].z - box->v[corner[i][0]].z) *
                           (box->v[corner[i][2]].y - box->v[corner[i][0]].y));
            normal[i].y = -((box->v[corner[i][1]].z - box->v[corner[i][0]].z) *
                            (box->v[corner[i][2]].x - box->v[corner[i][0]].x)) +
                          ((box->v[corner[i][1]].x - box->v[corner[i][0]].x) *
                           (box->v[corner[i][2]].z - box->v[corner[i][0]].z));
            normal[i].z = -((box->v[corner[i][1]].x - box->v[corner[i][0]].x) *
                            (box->v[corner[i][2]].y - box->v[corner[i][0]].y)) +
                          ((box->v[corner[i][1]].y - box->v[corner[i][0]].y) *
                           (box->v[corner[i][2]].x - box->v[corner[i][0]].x));
#endif
            NORMALIZE(normal[i], inv_length);
        }
        for (i = 0; i < FACES; i++) {
            within = 1;
            near_edges = 0;
            near_corners = 0;
            holding_edges = 0;
#ifdef NONMATCHING
            /* this side keeps the console's bits: SUM3 fuses the products as
             * SHC's FMACs do */
            cosine = SUM3(axis.x, normal[i].x, axis.y, normal[i].y, axis.z,
                          normal[i].z);
#else
            cosine = (axis.x * normal[i].x) + (axis.y * normal[i].y) +
                     (axis.z * normal[i].z);
#endif
            if (_builtin_fabsf(cosine) > TOLERANCE) {
                NJS_POINT3 p;
                NJS_POINT3 q;
                NJS_VECTOR d;
                Float to_c1;
                Float to_c2;
                Float nd;

                offset = (-normal[i].x * box->v[corner[i][0]].x) -
                         (normal[i].y * box->v[corner[i][0]].y) -
                         (normal[i].z * box->v[corner[i][0]].z);
#ifdef NONMATCHING
                /* this side keeps the console's bits: SUM3 fuses the products
                 * as SHC's FMACs do, and offset is added after */
                t = SUM3(normal[i].x, capsule->c1.x, normal[i].y, capsule->c1.y,
                         normal[i].z, capsule->c1.z) +
                    offset;
#else
                t = (normal[i].x * capsule->c1.x) +
                    (normal[i].y * capsule->c1.y) +
                    (normal[i].z * capsule->c1.z) + offset;
#endif
                /* @hack spelling: nd, the cosine worked out again, where
                 * cosine would do */
#ifdef NONMATCHING
                /* this side keeps the console's bits: SUM3 fuses the products
                 * as SHC's FMACs do */
                nd = SUM3(normal[i].x, axis.x, normal[i].y, axis.y, normal[i].z,
                          axis.z);
#else
                nd = (normal[i].x * axis.x) + (normal[i].y * axis.y) +
                     (normal[i].z * axis.z);
#endif
                t = -t / nd;
#ifdef NONMATCHING
                /* this side keeps the console's bits: each part is one of SHC's
                 * FMACs */
                p.x = __builtin_fmaf(axis.x, t, capsule->c1.x);
                p.y = __builtin_fmaf(axis.y, t, capsule->c1.y);
                p.z = __builtin_fmaf(axis.z, t, capsule->c1.z);
#else
                p.x = (axis.x * t) + capsule->c1.x;
                p.y = (axis.y * t) + capsule->c1.y;
                p.z = (axis.z * t) + capsule->c1.z;
#endif
                to_c1 = DISTANCE(p, capsule->c1);
                to_c2 = DISTANCE(p, capsule->c2);
                for (k = 0; k < FACE_CORNERS; k++) {
                    NJS_VECTOR edge;
                    /* the face's corners in turn, and its first again */
                    Int ring[FACE_CORNERS + 1] = {0, 1, 2, 3, 0};
                    Float off_x;
                    Float off_y;
                    Float off_z;
                    Float off_segment;

                    a = ring[k];
                    b = ring[k + 1];
                    edge.x = box->v[corner[i][a]].x - box->v[corner[i][b]].x;
                    edge.y = box->v[corner[i][a]].y - box->v[corner[i][b]].y;
                    edge.z = box->v[corner[i][a]].z - box->v[corner[i][b]].z;
#ifdef NONMATCHING
                    /* this side keeps the console's bits: SUM3 and each part's
                     * __builtin_fmaf fuse the products as SHC's FMACs do */
                    t = SUM3(edge.x, p.x - box->v[corner[i][a]].x, edge.y,
                             p.y - box->v[corner[i][a]].y, edge.z,
                             p.z - box->v[corner[i][a]].z) /
                        SUM3(edge.x, edge.x, edge.y, edge.y, edge.z, edge.z);
                    q.x = __builtin_fmaf(edge.x, t, box->v[corner[i][a]].x);
                    q.y = __builtin_fmaf(edge.y, t, box->v[corner[i][a]].y);
                    q.z = __builtin_fmaf(edge.z, t, box->v[corner[i][a]].z);
#else
                    t = ((edge.x * (p.x - box->v[corner[i][a]].x)) +
                         (edge.y * (p.y - box->v[corner[i][a]].y)) +
                         (edge.z * (p.z - box->v[corner[i][a]].z))) /
                        ((edge.x * edge.x) + (edge.y * edge.y) +
                         (edge.z * edge.z));
                    q.x = (edge.x * t) + box->v[corner[i][a]].x;
                    q.y = (edge.y * t) + box->v[corner[i][a]].y;
                    q.z = (edge.z * t) + box->v[corner[i][a]].z;
#endif
                    d.x = q.x - p.x;
                    d.y = q.y - p.y;
                    d.z = q.z - p.z;
                    NORMALIZE(d, inv_length);
                    off_x = _builtin_fabsf(normal[across[i][a]].x - d.x);
                    off_y = _builtin_fabsf(normal[across[i][a]].y - d.y);
                    off_z = _builtin_fabsf(normal[across[i][a]].z - d.z);
                    off_segment = _builtin_fabsf(to_c1 - length + to_c2);
                    if (off_x <= TOLERANCE && off_y <= TOLERANCE &&
                        off_z <= TOLERANCE &&
                        (to_c1 <= capsule->r || to_c2 <= capsule->r ||
                         off_segment <= TOLERANCE)) {
                        /* @hack redundant: within &= 1, which changes
                         * nothing */
                        within &= 1;
                        holding_edges++;
                    } else {
                        Float to_a;
                        Float to_b;
                        Float edge_length;
                        Float nearer;
                        Float reach;
                        Float to_q;

                        within = 0;
                        to_a = DISTANCE(box->v[corner[i][a]], q);
                        to_b = DISTANCE(box->v[corner[i][b]], q);
                        edge_length = DISTANCE(box->v[corner[i][a]],
                                               box->v[corner[i][b]]);
                        nearer = to_c2 > to_c1 ? to_c1 : to_c2;
                        reach = njSqrt((capsule->r * capsule->r) -
                                       (nearer * nearer));
                        if (edge_length + TOLERANCE >=
                            to_a + to_b - TOLERANCE) {
                            /* @hack spelling: to_q, a local for p's distance
                             * to q, where the call in the test would do */
                            to_q = DISTANCE(p, q);
                            if (to_q < reach) {
                                near_edges++;
                            }
                        } else {
                            to_a = DISTANCE(box->v[corner[i][a]], p);
                            to_b = DISTANCE(box->v[corner[i][b]], p);
                            if (reach >= to_a || reach >= to_b) {
                                near_corners++;
                            }
                        }
                    }
                }
                if (within || near_edges + holding_edges == FACE_CORNERS ||
                    near_corners + holding_edges == FACE_CORNERS) {
                    return 1;
                }
            }
            {
                NJS_POINT3 foot1;
                NJS_POINT3 foot2;
                NJS_VECTOR down1;
                NJS_VECTOR down2;
                Float t2;
                Float nn;
                Float to_foot;

                /* @hack spelling: offset worked out again, where working it
                 * out once ahead of the axis test would do */
                offset = (-normal[i].x * box->v[corner[i][0]].x) -
                         (normal[i].y * box->v[corner[i][0]].y) -
                         (normal[i].z * box->v[corner[i][0]].z);
#ifdef NONMATCHING
                /* this side keeps the console's bits: SUM3 and each part's
                 * __builtin_fmaf fuse the products as SHC's FMACs do */
                t = SUM3(normal[i].x, capsule->c1.x, normal[i].y, capsule->c1.y,
                         normal[i].z, capsule->c1.z) +
                    offset;
                nn = SUM3(normal[i].x, normal[i].x, normal[i].y, normal[i].y,
                          normal[i].z, normal[i].z);
                t = -t / nn;
                t2 = SUM3(normal[i].x, capsule->c2.x, normal[i].y,
                          capsule->c2.y, normal[i].z, capsule->c2.z) +
                     offset;
                t2 = -t2 / nn;
                foot1.x = __builtin_fmaf(normal[i].x, t, capsule->c1.x);
                foot1.y = __builtin_fmaf(normal[i].y, t, capsule->c1.y);
                foot1.z = __builtin_fmaf(normal[i].z, t, capsule->c1.z);
                foot2.x = __builtin_fmaf(normal[i].x, t2, capsule->c2.x);
                foot2.y = __builtin_fmaf(normal[i].y, t2, capsule->c2.y);
                foot2.z = __builtin_fmaf(normal[i].z, t2, capsule->c2.z);
#else
                t = (normal[i].x * capsule->c1.x) +
                    (normal[i].y * capsule->c1.y) +
                    (normal[i].z * capsule->c1.z) + offset;
                nn = (normal[i].x * normal[i].x) + (normal[i].y * normal[i].y) +
                     (normal[i].z * normal[i].z);
                t = -t / nn;
                t2 = (normal[i].x * capsule->c2.x) +
                     (normal[i].y * capsule->c2.y) +
                     (normal[i].z * capsule->c2.z) + offset;
                t2 = -t2 / nn;
                foot1.x = (normal[i].x * t) + capsule->c1.x;
                foot1.y = (normal[i].y * t) + capsule->c1.y;
                foot1.z = (normal[i].z * t) + capsule->c1.z;
                foot2.x = (normal[i].x * t2) + capsule->c2.x;
                foot2.y = (normal[i].y * t2) + capsule->c2.y;
                foot2.z = (normal[i].z * t2) + capsule->c2.z;
#endif
                down1.x = foot1.x - capsule->c1.x;
                down1.y = foot1.y - capsule->c1.y;
                down1.z = foot1.z - capsule->c1.z;
                NORMALIZE(down1, inv_length);
                down2.x = foot2.x - capsule->c2.x;
                down2.y = foot2.y - capsule->c2.y;
                down2.z = foot2.z - capsule->c2.z;
                NORMALIZE(down2, inv_length);
                if (_builtin_fabsf(normal[i].x - down1.x) <= TOLERANCE &&
                    _builtin_fabsf(normal[i].y - down1.y) <= TOLERANCE &&
                    _builtin_fabsf(normal[i].z - down1.z) <= TOLERANCE) {
                    /* @hack redundant: c1_inside &= 1, which changes
                     * nothing */
                    c1_inside &= 1;
                    c1_behind++;
                } else {
                    c1_inside = 0;
                    to_foot = DISTANCE(capsule->c1, foot1);
                    if (to_foot <= capsule->r) {
                        c1_near++;
                    }
                    c1_sum += to_foot;
                }
                if (_builtin_fabsf(normal[i].x - down2.x) <= TOLERANCE &&
                    _builtin_fabsf(normal[i].y - down2.y) <= TOLERANCE &&
                    _builtin_fabsf(normal[i].z - down2.z) <= TOLERANCE) {
                    /* @hack redundant: c2_inside &= 1, which changes
                     * nothing */
                    c2_inside &= 1;
                    c2_behind++;
                } else {
                    c2_inside = 0;
                    to_foot = DISTANCE(capsule->c2, foot2);
                    if (to_foot <= capsule->r) {
                        c2_near++;
                    }
                    c2_sum += to_foot;
                }
            }
        }
        if (c1_inside || c2_inside || c1_behind + c1_near == FACES ||
            c2_behind + c2_near == FACES) {
            /* @hack spelling: three tests that each return 1, where one
             * test of the three would do */
            if (c1_near == 2 &&
                c1_sum * c1_sum < capsule->r * capsule->r * 2.0F) {
                return 1;
            } else if (c2_near == 2 &&
                       c2_sum * c2_sum < capsule->r * capsule->r * 2.0F) {
                return 1;
            } else if (c1_near < 2 || c2_near < 2) {
                return 1;
            }
        }
    }
    return 0;
}
