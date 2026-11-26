/* the camera's collision with the zones of modes COLLISION and COLLI_LR */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "camera/zone_collision.h"
#include "camera/zones.h"
#include "system/math.h"
#include "system/pad.h"

Bool camera_in_colli_lr;

static int zone_table_collide(int table, int player);
static void line_sphere_crossing(NJS_SPHERE *sphere, NJS_LINE *line, Float r2,
                                 NJS_POINT3 *out);

int camera_zones_collide(int player)
{
    int count;

    camera_in_colli_lr = FALSE;
    count = zone_table_collide(ZONE_TABLE_SHARED, player);
    /* @hack spelling: the second count added into count ahead of the return,
     * where count plus that call returned in place would do */
    count += zone_table_collide(ZONE_TABLE_OWN(player), player);
    return count;
}

/* camera_zones_collide over the zones of table alone */
/* @hack redundant: player, which nothing reads */
static int zone_table_collide(int table, int player)
{
    struct shape_test test;
    struct camera_zone *zone;
    struct camera_zone **bounds;
    int count;

    count = 0;
    /* @hack spelling: bounds assigned inside the expression that sets zone,
     * where a statement of its own would do */
    zone = *(bounds = &camera_zone_tables[table]);
    /* @hack spelling: a while that steps zone at its end and before its one
     * continue, where a for that steps it would do */
    while (zone < bounds[1]) {
        if (zone->mode == CAMERA_MODE_COLLISION) {
            switch (zone->shape) {
            case ZONE_SPHERE: {
                NJS_VECTOR away;

                test.pos = &zone->pos;
                test.scl = &zone->scl;
                test.point = &camera_state.pos;
                if (point_in_sphere(&test)) {
                    /* @hack spelling: the zone's center and radius read
                     * through test, not from zone */
                    away.x = camera_state.pos.x - test.pos->x;
                    away.y = camera_state.pos.y - test.pos->y;
                    away.z = camera_state.pos.z - test.pos->z;
                    njUnitVector(&away);
                    camera_state.pos.x = test.pos->x + (away.x * test.scl->x);
                    camera_state.pos.y = test.pos->y + (away.y * test.scl->x);
                    camera_state.pos.z = test.pos->z + (away.z * test.scl->x);
                    count++;
                }
                break;
            }
            case ZONE_BOX: {
                NJS_POINT3 place;
                /* place's distance from the center along each axis, and
                 * then from the nearer face */
                /* @hack type: an array of three, where an NJS_VECTOR would
                 * do */
                Float depth[3];
                int axis;

                njPushMatrix(NULL);
                njUnitMatrix(NULL);
                njTranslateV(NULL, &zone->pos);
                njRotateZ(NULL, zone->ang.z);
                njRotateX(NULL, zone->ang.x);
                njRotateY(NULL, zone->ang.y);
                njInvertMatrix(NULL);
                njCalcPoint(NULL, &camera_state.pos, &place);
                /* @bug logic: leaves the matrix it pushed on Ninja's stack
                 * when the camera lies outside the box. the branches at
                 * 0x8c0b35d6, 0x8c0b35ee and 0x8c0b3606 skip njPopMatrix */
                /* @hack spelling: the assignments inside the test, where
                 * statements ahead of it would do */
                if ((depth[0] = njAbs(place.x)) > zone->scl.x ||
                    (depth[1] = njAbs(place.y)) > zone->scl.y ||
                    (depth[2] = njAbs(place.z)) > zone->scl.z) {
#ifdef NONMATCHING
                    /* this side repairs the tag above: it pops the matrix
                     * before the break */
                    njPopMatrix(1);
#endif
                    break;
                }
                depth[0] = zone->scl.x - depth[0];
                depth[1] = zone->scl.y - depth[1];
                depth[2] = zone->scl.z - depth[2];
                axis = 0;
                if (depth[1] < depth[0]) {
                    axis = 1;
                    if (depth[2] < depth[1]) {
                        axis = 2;
                    }
                } else if (depth[2] < depth[0]) {
                    axis = 2;
                }
                switch (axis) {
                case 0:
                    if (place.x > 0.0F) {
                        place.x = zone->scl.x;
                    } else {
                        place.x = -zone->scl.x;
                    }
                    break;
                case 1:
                    if (place.y > 0.0F) {
                        place.y = zone->scl.y;
                    } else {
                        place.y = -zone->scl.y;
                    }
                    break;
                case 2:
                    if (place.z > 0.0F) {
                        place.z = zone->scl.z;
                    } else {
                        place.z = -zone->scl.z;
                    }
                    break;
                }
                njInvertMatrix(NULL);
                njCalcPoint(NULL, &place, &camera_state.pos);
                njPopMatrix(1);
                count++;
                break;
            }
            }
        } else if (zone->mode == CAMERA_MODE_COLLI_LR) {
            switch (zone->shape) {
            case ZONE_SPHERE:
                test.pos = &zone->pos;
                test.scl = &zone->scl;
                test.point = &camera_state.target;
                if (point_in_sphere(&test)) {
                    camera_in_colli_lr = TRUE;
                }
                break;
            case ZONE_BOX:
                /* @hack spelling: ang set ahead of scl, where the order of
                 * test's members would do */
                test.pos = &zone->pos;
                test.ang = &zone->ang;
                test.scl = &zone->scl;
                test.point = &camera_state.target;
                if (point_in_box(&test)) {
                    camera_in_colli_lr = TRUE;
                }
                break;
            }
            /* @hack spelling: the pad's and standing's tests in an if of
             * their own that steps zone and continues, where && with the test
             * of trigger_pushed would do */
            if (!camera_stacks[camera_current_index].trigger_pushed) {
                if (!(player_pad_records[camera_current_index].on &
                      (PDD_DGT_TL | PDD_DGT_TR)) ||
                    camera_stacks[camera_current_index].standing == 0) {
                    zone++;
                    continue;
                }
                switch (zone->shape) {
                case ZONE_SPHERE: {
                    NJS_SPHERE sphere;
                    NJS_LINE line;

                    test.pos = &zone->pos;
                    test.scl = &zone->scl;
                    test.point = &camera_state.pos;
                    if (point_in_sphere(&test)) {
                        sphere.c = zone->pos;
                        /* @hack redundant: the radius, which
                         * line_sphere_crossing doesn't read */
                        sphere.r = zone->scl.x;
                        line.px = camera_state.target.x;
                        line.py = camera_state.target.y;
                        line.pz = camera_state.target.z;
                        line.vx = camera_state.dir.x;
                        line.vy = camera_state.dir.y;
                        line.vz = camera_state.dir.z;
                        line_sphere_crossing(&sphere, &line, zone->scl.z,
                                             &camera_state.pos);
                        count++;
                    }
                    break;
                }
                case ZONE_BOX: {
                    NJS_POINT3 place;
                    NJS_POINT3 start;
                    Float depth[3];
                    /* the box's faces on start's side, one perpendicular to
                     * each axis */
                    NJS_PLANE faces[3];
                    /* where line crosses each face's plane */
                    NJS_POINT3 crossings[3];
                    NJS_LINE line;
                    /* @hack spelling: found counted up, though it is only
                     * tested against 0, where found = 1 would do */
                    int found;

                    njPushMatrix(NULL);
                    njUnitMatrix(NULL);
                    njTranslateV(NULL, &zone->pos);
                    njRotateZ(NULL, zone->ang.z);
                    njRotateX(NULL, zone->ang.x);
                    njRotateY(NULL, zone->ang.y);
                    njInvertMatrix(NULL);
                    njCalcPoint(NULL, &camera_state.pos, &place);
                    /* @bug logic: leaves the matrix it pushed on Ninja's
                     * stack when the camera lies outside the box. the
                     * branches at 0x8c0b395c, 0x8c0b3974 and 0x8c0b398c skip
                     * njPopMatrix */
                    /* @hack redundant: depth, which nothing reads after
                     * these stores */
                    if ((depth[0] = njAbs(place.x)) > zone->scl.x ||
                        (depth[1] = njAbs(place.y)) > zone->scl.y ||
                        (depth[2] = njAbs(place.z)) > zone->scl.z) {
#ifdef NONMATCHING
                        /* this side repairs the tag above: it pops the matrix
                         * before the break */
                        njPopMatrix(1);
#endif
                        break;
                    }
                    njCalcPoint(NULL, &camera_state.target, &start);
                    faces[0].py = 0.0F;
                    faces[0].pz = 0.0F;
                    faces[0].vx = 1.0F;
                    faces[0].vy = 0.0F;
                    faces[0].vz = 0.0F;
                    if (start.x >= 0.0F) {
                        faces[0].px = zone->scl.x;
                    } else {
                        faces[0].px = -zone->scl.x;
                    }
                    /* @bug uninitialized: njDistanceL2PL reads faces[1].px
                     * and faces[2].px, which nothing sets (FMOV.S @R14,FR3
                     * at 0x8c11d468, for the calls at 0x8c0b3ab2 and
                     * 0x8c0b3abe). it multiplies each by its face's vx of 0,
                     * so a stale value shows only where it is a NaN or an
                     * infinity */
#ifdef NONMATCHING
                    /* this side repairs the tag above: both start at 0 */
                    faces[1].px = 0.0F;
                    faces[2].px = 0.0F;
#endif
                    /* @hack redundant: faces[1].py stored 0 ahead of its
                     * value */
                    faces[1].py = 0.0F;
                    faces[1].pz = 0.0F;
                    faces[1].vx = 0.0F;
                    faces[1].vy = 1.0F;
                    faces[1].vz = 0.0F;
                    if (start.y >= 0.0F) {
                        faces[1].py = zone->scl.y;
                    } else {
                        faces[1].py = -zone->scl.y;
                    }
                    faces[2].py = 0.0F;
                    /* @hack redundant: faces[2].pz stored 0 ahead of its
                     * value */
                    faces[2].pz = 0.0F;
                    faces[2].vx = 0.0F;
                    faces[2].vy = 0.0F;
                    faces[2].vz = 1.0F;
                    if (start.z >= 0.0F) {
                        faces[2].pz = zone->scl.z;
                    } else {
                        faces[2].pz = -zone->scl.z;
                    }
                    line.px = start.x;
                    line.py = start.y;
                    line.pz = start.z;
                    line.vx = place.x - start.x;
                    line.vy = place.y - start.y;
                    line.vz = place.z - start.z;
                    /* @bug logic: takes a point off line for a crossing.
                     * where njDistanceL2PL counts line parallel to a face's
                     * plane, it gives the point of the plane nearest start
                     * (the calls at 0x8c0b3aa6, 0x8c0b3ab2 and 0x8c0b3abe).
                     * on faces[1] and faces[2] that point has start's x. so
                     * where it lies within its face, it beats every true
                     * crossing, and the camera leaves line (FCMP/GT at
                     * 0x8c0b3b3a and 0x8c0b3ba6) */
                    njDistanceL2PL(&line, &faces[0], &crossings[0]);
                    njDistanceL2PL(&line, &faces[1], &crossings[1]);
                    njDistanceL2PL(&line, &faces[2], &crossings[2]);
                    /* place becomes the crossing within its face that lies
                     * nearest start. where none lies within its face, place
                     * stays the camera's place. the tests measure nearness
                     * by x alone. x orders the points of line by their
                     * distance from start, unless line.vx is 0. of crossings
                     * that tie, the first found stays */
                    found = 0;
                    if (njAbs(crossings[0].y) < zone->scl.y &&
                        njAbs(crossings[0].z) < zone->scl.z) {
                        place = crossings[0];
                        found++;
                    }
                    if (njAbs(crossings[1].x) < zone->scl.x &&
                        njAbs(crossings[1].z) < zone->scl.z) {
                        if (found) {
                            if (njAbs(start.x - crossings[1].x) <
                                njAbs(start.x - place.x)) {
                                place = crossings[1];
                            }
                        } else {
                            found++;
                            place = crossings[1];
                        }
                    }
                    /* @bug logic: tests crossings[2].z against scl.y where
                     * crossings[2].y is meant. that crossing lies on the
                     * face at scl.z or -scl.z, so give or take rounding, the
                     * test asks whether scl.z is below scl.y (FMOV.S
                     * @(R0,R8),FR3 at 0x8c0b3b7a) */
                    if (njAbs(crossings[2].x) < zone->scl.x &&
                        njAbs(crossings[2].z) < zone->scl.y) {
                        if (found) {
                            if (njAbs(start.x - crossings[2].x) <
                                njAbs(start.x - place.x)) {
                                place = crossings[2];
                            }
                        } else {
                            place = crossings[2];
                        }
                    }
                    njInvertMatrix(NULL);
                    njCalcPoint(NULL, &place, &camera_state.pos);
                    njPopMatrix(1);
                    count++;
                    break;
                }
                }
            }
        }
        zone++;
    }
    return count;
}

/* set *out to the crossing of line and the sphere nearer line's start. that
 * crossing may lie behind the start. the sphere's center is sphere->c, and r2
 * is its radius squared: sphere->r isn't read. line must meet the sphere */
static void line_sphere_crossing(NJS_SPHERE *sphere, NJS_LINE *line, Float r2,
                                 NJS_POINT3 *out)
{
    Angle ax;
    Angle ay;
    /* each crossing's distance along line from its start, signed */
    /* @hack type: an array of two, where two Floats would do */
    Float z[2];
    NJS_POINT3 center;

    /* this matrix moves a point into axes where line runs along z from the
     * origin. the crossings lie on that axis */
    direction_angles_xy(line->vx, line->vy, line->vz, &ax, &ay);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateX(NULL, -ax);
    njRotateY(NULL, -ay);
    njTranslate(NULL, -line->px, -line->py, -line->pz);
    njCalcPoint(NULL, &sphere->c, &center);
    njInvertMatrix(NULL);
    /* @hack spelling: z[0] holds the square root before it holds its
     * crossing, where a local of its own for the root would do */
    z[0] = njSqrt(r2 - (center.x * center.x) - (center.y * center.y));
    z[1] = center.z - z[0];
    z[0] = center.z + z[0];
    if (njAbs(z[0]) >= njAbs(z[1])) {
        out->z = z[1];
    } else {
        out->z = z[0];
    }
    out->x = 0.0F;
    out->y = 0.0F;
    njCalcPoint(NULL, out, out);
    njPopMatrixEx();
}
