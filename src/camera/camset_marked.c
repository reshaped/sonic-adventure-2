/* the marked zones objects add to a stage's camset tables, and free */
#include "camera/camset.h"
#include "camera/editor.h"
#include "camera/stack.h"
#include "camera/zones.h"

struct camera_zone *camset_add_marked(const struct zone_shape *shape)
{
    camera_editor.held_zone.mode = CAMERA_MODE_COLLISION;
    camera_editor.held_zone.adjust = CAMERA_ADJUST_NONE;
    camera_editor.held_zone.entry = CAMSET_MARK_ENTRY;
    camera_editor.held_zone.shape = shape->shape;
    camera_editor.held_zone.pos = shape->pos;
    camera_editor.held_zone.ang = shape->ang;
    camera_editor.held_zone.scl = shape->scl;
    if (camset_put_held(0)) {
        return camset_last_put;
    }
    return NULL;
}

void camset_free_zone(struct camera_zone *zone)
{
    if (zone != NULL) {
        zone->mode = CAMERA_MODE_NONE;
    }
}
