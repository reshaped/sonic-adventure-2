/* camera zones: putting their modes on the cameras' stacks, and testing a
 * point against their shapes */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "camera/zones.h"
#include "player/player.h"
#include "system/task.h"
#include "system/vector.h"

/* the plane z = 0 of a zone's turned axes, where a ZONE_PLANE zone lies */
static NJS_PLANE zone_plane = {0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F};

Bool point_in_sphere(struct shape_test *test)
{
    NJS_VECTOR offset;

    vector_subtract(test->point, test->pos, &offset);
    /* @hack spelling: the comparison's result returned through an if, where
     * returning the comparison would do */
    if (njScalor2(&offset) < test->scl->z) {
        return TRUE;
    }
    return FALSE;
}

/* whether the step from prev to point crosses the plane within its edges. a
 * step that ends farther from pos than the plane's corners doesn't count,
 * even one that crosses it. it reads all five members of test */
static Bool step_crosses_plane(struct shape_test *test)
{
    /* @hack spelling: the locals declared as crossing, line, prev, point and
     * offset, not as offset, prev, point, line and crossing, the order the
     * code first uses them */
    NJS_POINT3 crossing;
    NJS_LINE line;
    NJS_POINT3 prev;
    NJS_POINT3 point;
    NJS_VECTOR offset;
    /* @hack spelling: distance2 and corner2, locals for the two sums the test
     * compares, where the sums in the test would do */
    Float distance2;
    Float corner2;

    offset.x = test->pos->x - test->point->x;
    offset.y = test->pos->y - test->point->y;
    offset.z = test->pos->z - test->point->z;
    distance2 =
        (offset.x * offset.x) + (offset.y * offset.y) + (offset.z * offset.z);
    corner2 = (test->scl->x * test->scl->x) + (test->scl->y * test->scl->y);
    if (distance2 > corner2) {
        return FALSE;
    }
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njTranslateV(NULL, test->pos);
    njRotateZ(NULL, test->ang->z);
    njRotateX(NULL, test->ang->x);
    njRotateY(NULL, test->ang->y);
    njInvertMatrix(NULL);
    njCalcPoint(NULL, test->prev, &prev);
    njCalcPoint(NULL, test->point, &point);
    njPopMatrix(1);
    if (prev.z < 0.0F && point.z < 0.0F) {
        return FALSE;
    }
    if (prev.z > 0.0F && point.z > 0.0F) {
        return FALSE;
    }
    line.px = prev.x;
    line.py = prev.y;
    line.pz = prev.z;
    line.vx = point.x - prev.x;
    line.vy = point.y - prev.y;
    line.vz = point.z - prev.z;
    njDistanceL2PL(&line, &zone_plane, &crossing);
    /* @hack redundant: the elses after the arms that return */
    if (crossing.x > test->scl->x) {
        return FALSE;
    } else if (crossing.x < -test->scl->x) {
        return FALSE;
    } else if (crossing.y > test->scl->y) {
        return FALSE;
    } else if (crossing.y < -test->scl->y) {
        return FALSE;
    }
    return TRUE;
}

Bool point_in_box(struct shape_test *test)
{
    /* @hack spelling: point declared ahead of offset, not after it */
    NJS_POINT3 point;
    NJS_VECTOR offset;
    /* @hack spelling: distance2 and corner2, locals for the two sums the test
     * compares, where the sums in the test would do */
    Float distance2;
    Float corner2;

    offset.x = test->pos->x - test->point->x;
    offset.y = test->pos->y - test->point->y;
    offset.z = test->pos->z - test->point->z;
    distance2 =
        (offset.x * offset.x) + (offset.y * offset.y) + (offset.z * offset.z);
    corner2 = (test->scl->x * test->scl->x) + (test->scl->y * test->scl->y) +
              (test->scl->z * test->scl->z);
    if (distance2 > corner2) {
        return FALSE;
    }
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njTranslateV(NULL, test->pos);
    njRotateZ(NULL, test->ang->z);
    njRotateX(NULL, test->ang->x);
    njRotateY(NULL, test->ang->y);
    njInvertMatrix(NULL);
    njCalcPoint(NULL, test->point, &point);
    njPopMatrix(1);
    /* @hack redundant: the elses after the arms that return */
    if (point.x > test->scl->x) {
        return FALSE;
    } else if (point.x < -test->scl->x) {
        return FALSE;
    } else if (point.y > test->scl->y) {
        return FALSE;
    } else if (point.y < -test->scl->y) {
        return FALSE;
    } else if (point.z > test->scl->z) {
        return FALSE;
    } else if (point.z < -test->scl->z) {
        return FALSE;
    }
    return TRUE;
}

static void zones_enter(int table, int player)
{
    int top;
    struct camera_zone *zone;
    struct camera_zone **bounds;
    struct shape_test test;
    NJS_POINT3 prev;

    top = camera_top(player);
    /* @hack spelling: bounds assigned inside the for's first clause, in the
     * expression that sets zone, where a statement of its own would do */
    for (zone = *(bounds = &camera_zone_tables[table]); zone < bounds[1];
         zone++) {
        /* @hack spelling: the test of the top mode's zone in an if of its
         * own, where && with the tests of the zone's mode would do */
        if (zone->mode != CAMERA_MODE_NONE &&
            zone->mode != CAMERA_MODE_COLLISION &&
            zone->mode != CAMERA_MODE_COLLI_LR) {
            if (camera_mode_zone(player, top) != zone) {
                /* @hack spelling: the three calls written out in each case,
                 * where one set after the switch, under a flag each case sets,
                 * would do */
                switch (zone->shape) {
                case ZONE_SPHERE:
                    test.pos = &zone->pos;
                    test.scl = &zone->scl;
                    test.point = &player_work[player]->pos;
                    if (point_in_sphere(&test)) {
                        camera_set_mode_with_shape(player, zone->entry,
                                                   zone->mode, zone->shape);
                        camera_set_mode_zone(player, zone->entry, zone);
                        camera_set_adjust(player, zone->entry, zone->adjust);
                    }
                    break;
                case ZONE_PLANE:
                    test.pos = &zone->pos;
                    test.scl = &zone->scl;
                    test.ang = &zone->ang;
                    test.point = &player_work[player]->pos;
                    player_get_past(player, 1, &prev, NULL);
                    test.prev = &prev;
                    if (step_crosses_plane(&test)) {
                        camera_set_mode_with_shape(player, zone->entry,
                                                   zone->mode, zone->shape);
                        camera_set_mode_zone(player, zone->entry, zone);
                        camera_set_adjust(player, zone->entry, zone->adjust);
                    }
                    break;
                case ZONE_BOX:
                    test.pos = &zone->pos;
                    test.scl = &zone->scl;
                    test.ang = &zone->ang;
                    test.point = &player_work[player]->pos;
                    if (point_in_box(&test)) {
                        camera_set_mode_with_shape(player, zone->entry,
                                                   zone->mode, zone->shape);
                        camera_set_mode_zone(player, zone->entry, zone);
                        camera_set_adjust(player, zone->entry, zone->adjust);
                    }
                    break;
                }
            }
        }
    }
}

static void zones_leave(int player)
{
    int top;
    Bool again;
    struct camera_zone *zone;
    struct shape_test test;

    do {
        top = camera_top(player);
        if (top == 0) {
            return;
        }
        again = FALSE;
        switch (camera_mode_shape(player, top)) {
        case ZONE_SPHERE:
            zone = camera_mode_zone(player, top);
            test.pos = &zone->pos;
            test.scl = &zone->scl;
            test.point = &player_work[player]->pos;
            if (!point_in_sphere(&test)) {
                camera_remove_mode(player, top);
                again = TRUE;
            }
            break;
        case ZONE_BOX:
            zone = camera_mode_zone(player, top);
            test.pos = &zone->pos;
            test.scl = &zone->scl;
            test.ang = &zone->ang;
            test.point = &player_work[player]->pos;
            if (!point_in_box(&test)) {
                camera_remove_mode(player, top);
                again = TRUE;
            }
            break;
        }
    } while (again);
}

static void zones_enter_all(void)
{
    int player;

    for (player = 0; player < camera_active_count; player++) {
        zones_enter(ZONE_TABLE_SHARED, player);
        zones_enter(ZONE_TABLE_OWN(player), player);
    }
}

void camera_zones_update(int player)
{
    zones_leave(player);
    zones_enter_all();
}
