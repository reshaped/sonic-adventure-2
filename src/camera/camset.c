/* a stage's camset zones: the camera editor's edits, their draw, and the
 * freeing of marked zones */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "camera/camset.h"
#include "camera/editor.h"
#include "camera/stack.h"
#include "camera/zones.h"
#include "system/areas.h"
#include "system/vector.h"

/* the bounds of the three tables zones.h describes. table 2 ends where the
 * camlin areas start */
struct camera_zone *camera_zone_tables[] = {
    (struct camera_zone *)CAMSET_AREA,
    (struct camera_zone *)CAMSET_AREA_0,
    (struct camera_zone *)CAMSET_AREA_1,
    (struct camera_zone *)CAMLIN_AREA_0,
#ifdef NONMATCHING
    /* Chao starts four cameras. Views 2 and 3 have no private camset. */
    (struct camera_zone *)CAMLIN_AREA_0,
    (struct camera_zone *)CAMLIN_AREA_0,
#endif
};

struct camera_zone *camset_last_put;

/* nothing calls these. none is static, since SHC leaves out a static
 * function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* pick up the zone of the shared table or of table whose pos lies nearest
 * camera_editor's cursor, closer than CAMERA_NEAREST_REACH. free and marked
 * zones are passed over. of zones at one distance, the first is taken, the
 * shared table's before table's. the zone is copied to the held zone and
 * freed, and it returns TRUE. camera_editor.shared gets 1 for a zone of the
 * shared table, else 0. where no zone lies that close, it returns FALSE,
 * changing nothing */
Bool camset_pick_up(int table);
/* draw each zone of table that isn't free and lies within CAMERA_MODEL_RANGE of
 * the current camera. each draws as the debug model of its shape, turned by its
 * ang. a sphere scales by scl.x along each axis, a plane by scl, and a box by
 * twice scl */
void camset_draw(int table);
/* NOLINTEND(misc-use-internal-linkage) */

/* the first free zone of table, or null for a full table */
static struct camera_zone *camset_find_free(int table)
{
    struct camera_zone *zones;
    int count;
    int i;

    zones = camera_zone_tables[table];
    /* @hack spelling: the zones counted by a loop of their own before the
     * search, where one loop that tests the bound as it searches would do */
    for (count = 0; &zones[count] < camera_zone_tables[table + 1]; count++) {
    }
    for (i = 0; i < count; i++) {
        if (zones[i].mode == CAMERA_MODE_NONE) {
            return &zones[i];
        }
    }
    return NULL;
}

Bool camset_put_held(int table)
{
    struct camera_zone *zone;

    zone = camset_find_free(table);
    /* @hack spelling: the null return last, after the copy, where an early
     * return would do */
    if (zone != NULL) {
        *zone = camera_editor.held_zone;
        camset_last_put = zone;
        return TRUE;
    }
    return FALSE;
}

Bool camset_pick_up(int table)
{
    struct camera_zone *zones;
    struct camera_zone *nearest;
    Float least;
    Float distance;
    int shared;
    int i;

    least = CAMERA_NEAREST_REACH;
    nearest = NULL;
    zones = camera_zone_tables[ZONE_TABLE_SHARED];
    for (i = 0; &zones[i] < camera_zone_tables[ZONE_TABLE_SHARED + 1]; i++) {
        /* @hack spelling: two ifs, where one test joined by && would do */
        if (zones[i].mode != CAMERA_MODE_NONE) {
            if (zones[i].mode != CAMERA_MODE_COLLISION ||
                zones[i].entry != CAMSET_MARK_ENTRY) {
                distance = njDistanceP2P(&camera_editor.cursor, &zones[i].pos);
                if (distance < least) {
                    least = distance;
                    nearest = &zones[i];
                    shared = 1;
                }
            }
        }
    }
    zones = camera_zone_tables[table];
    for (i = 0; &zones[i] < camera_zone_tables[table + 1]; i++) {
        /* @hack spelling: two ifs, where one test joined by && would do */
        if (zones[i].mode != CAMERA_MODE_NONE) {
            if (zones[i].mode != CAMERA_MODE_COLLISION ||
                zones[i].entry != CAMSET_MARK_ENTRY) {
                distance = njDistanceP2P(&camera_editor.cursor, &zones[i].pos);
                if (distance < least) {
                    least = distance;
                    nearest = &zones[i];
                    shared = 0;
                }
            }
        }
    }
    if (nearest == NULL) {
        return FALSE;
    }
    camera_editor.held_zone = *nearest;
    camera_editor.shared = shared;
    nearest->mode = CAMERA_MODE_NONE;
    return TRUE;
}

void camset_draw(int table)
{
    struct camera_zone *zones;
    NJS_VECTOR from_camera;
    NJS_VECTOR scl;
    int i;

    zones = camera_zone_tables[table];
    for (i = 0; &zones[i] < camera_zone_tables[table + 1]; i++) {
        if (zones[i].mode != CAMERA_MODE_NONE) {
            vector_subtract(&zones[i].pos,
                            &camera_views[camera_current_index]->pos,
                            &from_camera);
            if (njScalor(&from_camera) < CAMERA_MODEL_RANGE) {
                switch (zones[i].shape) {
                case ZONE_SPHERE:
                    scl.x = zones[i].scl.x;
                    scl.y = zones[i].scl.x;
                    scl.z = zones[i].scl.x;
                    debug_draw_model(&zones[i].pos, &zones[i].ang, &scl,
                                     DEBUG_MODEL_SPHERE);
                    break;
                case ZONE_PLANE:
                    debug_draw_model(&zones[i].pos, &zones[i].ang,
                                     &zones[i].scl, DEBUG_MODEL_PLANE);
                    break;
                case ZONE_BOX:
                    scl.x = zones[i].scl.x * 2.0F;
                    scl.y = zones[i].scl.y * 2.0F;
                    scl.z = zones[i].scl.z * 2.0F;
                    debug_draw_model(&zones[i].pos, &zones[i].ang, &scl,
                                     DEBUG_MODEL_BOX);
                    break;
                }
            }
        }
    }
}

void camset_free_marked(void)
{
    struct camera_zone *zone;

    /* @hack spelling: a loop of its own for each table, where one loop over
     * the tables would do */
    for (zone = camera_zone_tables[ZONE_TABLE_SHARED];
         zone < camera_zone_tables[ZONE_TABLE_SHARED + 1]; zone++) {
        if (zone->mode == CAMERA_MODE_COLLISION &&
            zone->entry == CAMSET_MARK_ENTRY) {
            zone->mode = CAMERA_MODE_NONE;
        }
    }
    for (zone = camera_zone_tables[ZONE_TABLE_OWN(0)];
         zone < camera_zone_tables[ZONE_TABLE_OWN(0) + 1]; zone++) {
        if (zone->mode == CAMERA_MODE_COLLISION &&
            zone->entry == CAMSET_MARK_ENTRY) {
            zone->mode = CAMERA_MODE_NONE;
        }
    }
    for (zone = camera_zone_tables[ZONE_TABLE_OWN(1)];
         zone < camera_zone_tables[ZONE_TABLE_OWN(1) + 1]; zone++) {
        if (zone->mode == CAMERA_MODE_COLLISION &&
            zone->entry == CAMSET_MARK_ENTRY) {
            zone->mode = CAMERA_MODE_NONE;
        }
    }
}
