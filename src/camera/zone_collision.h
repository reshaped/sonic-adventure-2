/* the camera's collision with the zones of modes COLLISION and COLLI_LR */
#ifndef CAMERA_ZONE_COLLISION_H
#define CAMERA_ZONE_COLLISION_H
#include <sg_xpt.h>

/* move camera_state's pos out of the COLLISION and COLLI_LR zones of the shared
 * table and of player's own. it returns the number of zones that acted.
 * In the portable build, views 2 and 3 use only the shared table.
 *
 * a zone acts only where it holds the camera. a COLLISION sphere moves the
 * camera out to its surface along the line from its center. a COLLISION box
 * moves it onto the face it lies nearest.
 *
 * a COLLI_LR zone acts only while camera_current_index's pad holds L or R.
 * that camera's stack must also have a standing other than 0 and a
 * trigger_pushed of FALSE. the zone moves the camera to where a line from
 * camera_state's target crosses the zone's surface. a sphere's line runs
 * along camera_state's dir, and the camera takes the crossing nearer the
 * target. a box's line runs toward the camera. the camera takes a crossing
 * within one of the faces that look toward the target. where no crossing lies
 * within its face, the box acts all the same. the camera then stays where it
 * is.
 *
 * camera_in_colli_lr becomes TRUE where target lies in a COLLI_LR zone, and
 * FALSE where it lies in none. the portable build restores the current matrix
 * after every box test. The original leaves a matrix pushed whenever a
 * box rejects the camera. Ninja's matrix stack must have room for the test */
int camera_zones_collide(int player);

/* TRUE where camera_zones_collide last found camera_state's target in a
 * COLLI_LR zone. while it is TRUE, L and R turn no KNUKLES or KNUCKLES_L
 * camera */
extern Bool camera_in_colli_lr;

#endif
