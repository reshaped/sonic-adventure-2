/* camera zones: shapes in a stage, each with a mode for a player's camera */
#ifndef CAMERA_ZONES_H
#define CAMERA_ZONES_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

/* the numbers of a zone's shapes */
#define ZONE_SPHERE 1
#define ZONE_PLANE 2
#define ZONE_BOX 3

/* a zone: a shape with one of the camera's modes. camera_zones_collide acts
 * on the zones of modes COLLISION and COLLI_LR, and camera_zones_update on
 * those of every other mode.
 *
 * the shape turns by ang about y, then x, then z, and moves to pos. a
 * sphere's radius is scl.x, and the tests read its square from scl.z. a
 * plane is the turned z = 0, reaching scl.x each way along x and scl.y along
 * y. a box reaches scl.x, scl.y and scl.z each way along the turned axes */
struct camera_zone {
    /* a CAMERA_MODE_ number. CAMERA_MODE_NONE marks a free zone, which acts
     * on no camera */
    Sint32 mode;
    Sint32 adjust; /* a CAMERA_ADJUST_ number, to pair with mode */
    /* the index of the camera's stack where camera_zones_update puts mode */
    Sint32 entry;
    Sint32 shape; /* a ZONE_ shape */
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR scl;
    /* a place for the camera, and in camera_ang.z a roll. the modes FIX and
     * ASHLAND stand the camera there with that roll. PSTONE starts from the
     * direction from point toward it. SPACE sets camera_pos, camera_ang.z
     * and point. nothing reads camera_ang's x and y */
    NJS_POINT3 camera_pos;
    struct angles camera_ang;
    /* the point FIX looks at. KLAMATH and POINT keep the player between it
     * and the camera */
    NJS_POINT3 point;
    /* ASHLAND's view, by ZONE_VIEW_ANGLE and ZONE_VIEW_TURN. SPACE reads the
     * two numbers as flags of its own */
    /* @hack type: the two numbers as an array, where two members would
     * do */
    Sint32 view[2];
    Uint8 unk_60[0x18];
    Float param[2]; /* by ZONE_DIST and ZONE_HEIGHT */
    Uint8 unk_80[0x18];
};

/* a zone's shape and where it lies: its members from shape to scl */
struct zone_shape {
    Sint32 shape;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR scl;
};

/* a zone's param: how far behind the player KLAMATH and POINT stand the
 * camera, and a height. modes.h says how each mode reads the height */
#define ZONE_DIST 0
#define ZONE_HEIGHT 1

/* a zone's view, in degrees: the angle of view ASHLAND gives the camera, and
 * the most the camera turns toward it in a step */
#define ZONE_VIEW_ANGLE 0
#define ZONE_VIEW_TURN 1

/* the bounds of the zone tables, which loader_read_camera_files fills from a
 * stage's camset files. table t runs from camera_zone_tables[t] up to
 * camera_zone_tables[t + 1]. the shared table, in CAMSET_AREA, holds every
 * player's zones. player 0's own table is in CAMSET_AREA_0, and player 1's in
 * CAMSET_AREA_1. The portable build gives views 2 and 3 empty ranges. */
extern struct camera_zone *camera_zone_tables[];
#define ZONE_TABLE_SHARED 0
#define ZONE_TABLE_OWN(player) ((player) + 1)

/* a point to test against a zone's shape. pos, scl and ang point to the
 * zone's. prev is where point lay a step before. each test reads only the
 * members its comment names */
struct shape_test {
    NJS_POINT3 *pos;
    NJS_VECTOR *scl;
    struct angles *ang;
    NJS_POINT3 *point;
    NJS_POINT3 *prev;
};

/* whether point lies inside the sphere: whether its distance squared from pos
 * is under scl->z. it reads pos, scl and point */
Bool point_in_sphere(struct shape_test *test);
/* whether point lies inside the box or on its surface. it reads pos, scl, ang
 * and point */
Bool point_in_box(struct shape_test *test);

/* first take modes off player's camera's stack from the top, down to index
 * 1 at most. each comes off where a sphere or box put it there and no longer
 * holds the player. it stops at the first mode that stays.
 *
 * then each camera p in use takes modes from the shared table and from
 * player p's own. a sphere or box acts while it holds player p. a plane acts
 * as player p steps through it. an acting zone puts its mode at its entry of
 * camera p's stack. the slot keeps the zone and its shape, and pairs the
 * zone's adjust with the mode. each table's pass skips the zone kept by the
 * slot that was on top as the pass began. */
void camera_zones_update(int player);

#endif
