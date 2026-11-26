/* a stage's camset zones: the marked zones objects add and free, and
 * camset_put_held */
#ifndef CAMERA_CAMSET_H
#define CAMERA_CAMSET_H
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/zones.h"

/* a marked zone is one of mode CAMERA_MODE_COLLISION whose entry is
 * CAMSET_MARK_ENTRY. camset_add_marked adds such zones to table 0,
 * camset_pick_up passes over them, and camset_free_marked frees them */
#define CAMSET_MARK_ENTRY 1

/* add a marked zone of shape to table 0, with CAMERA_ADJUST_NONE, and return
 * it. for a full table it returns null. it overwrites camera_editor's held zone
 * from mode to scl, whatever it returns. the new zone is a copy of the held
 * zone. its members after scl are those the held zone had */
struct camera_zone *camset_add_marked(const struct zone_shape *shape);
/* free zone, setting its mode to CAMERA_MODE_NONE. a null zone is
 * ignored */
void camset_free_zone(struct camera_zone *zone);

/* the zone camset_put_held filled last */
extern struct camera_zone *camset_last_put;

/* copy camera_editor's held zone over table's first free zone, set
 * camset_last_put to it, and return TRUE. for a full table it returns FALSE,
 * changing nothing */
Bool camset_put_held(int table);
/* free each marked zone of the three tables */
void camset_free_marked(void);

#endif
