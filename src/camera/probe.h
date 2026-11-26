/* the camera's probes of the land. each casts a line, or moves a sphere,
 * among the faces of the world objects near its start */
#ifndef CAMERA_PROBE_H
#define CAMERA_PROBE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

/* the farthest next may lie from pos. a probe past it tests nothing,
 * returns 0 and leaves hit at next */
#define CAMERA_PROBE_REACH 300.0F
/* the most faces a move slides along */
#define CAMERA_PROBE_SLIDES 3

/* a face a move slid along */
struct camera_probe_slide {
    Uint8 unk_00[0x24];
    NJS_VECTOR normal; /* the unit normal of the face's front */
    struct angles ang; /* the face's tilt about x and about z. y is 0 */
};

/* a probe of the land. its caller sets pos, next and skip, and radius for a
 * move. the probes write the rest */
struct camera_probe {
    NJS_POINT3 pos;
    NJS_POINT3 next;
    /* world flags: a probe passes over each world object with any of them */
    Uint32 skip;
    /* the radius of a move's sphere, which camera_probe_cast doesn't read */
    Float radius;
    /* where a cast's line crossed a face, or where a move's sphere came to
     * rest. it is next where the line crossed no face, or no face stopped
     * the sphere */
    NJS_POINT3 hit;
    /* the tilt about x and about z of the face a cast crossed. y is 0 */
    struct angles ang;
    NJS_VECTOR normal; /* the unit normal of the crossed face's front */
    NJS_VECTOR moved; /* a cast's hit less pos */
    Sint32 flags; /* the world flags of the crossed face's object */
    /* the faces a move slid along, in turn, as many as it returned */
    struct camera_probe_slide slides[CAMERA_PROBE_SLIDES];
    NJS_VECTOR dir; /* next less pos */
    Uint8 unk_114[0x24];
    /* the list's index of the face the last slide stopped at, or -1 for
     * none. the next slide passes over that face */
    Sint32 slid_face;
};

/* while it isn't 0, the probes draw each face they take, and a cast prints
 * its counts. the camera task's draw sets it to 0 */
extern Sint32 camera_probe_debug;

/* camera_probe_alloc takes the probes' list of faces from the Shinobi heap,
 * unless it holds one already. where the heap can't give it, the list stays
 * null. a probe that then takes a face writes through null. camera_probe_free
 * frees the list, where there is one */
void camera_probe_alloc(void);
void camera_probe_free(void);

/* each probe, camera_probe_cast, camera_probe_move or
 * camera_probe_move_two_sided, first empties the list of faces. it sets
 * probe's dir, its hit to next and its slid_face to -1. where next lies past
 * CAMERA_PROBE_REACH from pos, it returns 0 at once. otherwise
 * polygon_collect_near lists in world_objects the world objects near pos. it
 * does so for a ball of radius 10 moving from pos to next. the ball then sweeps
 * over each of them without a bit of skip. the probe takes its faces from those
 * the ball touches. the list of faces must be allocated */

/* cast a line from probe's pos to its next. where it crosses a face, it
 * returns TRUE, with hit at the crossing nearest pos. ang and normal are then
 * that face's, and flags its object's. where it crosses none, it returns
 * FALSE with hit at next. ang and normal then hold no face's values. where the
 * ball touched an object, flags holds the first one's. moved is hit less pos
 * either way.
 *
 * a face counts only where pos isn't behind it. the cast copies each such face
 * the ball touches into the list of faces, which holds 384. nothing checks that
 * they fit. njDistanceL2PL can count the line parallel to a face's plane. its
 * crossing is then the plane's point nearest pos, off the line */
Bool camera_probe_cast(struct camera_probe *probe);

/* move a sphere of probe's radius from pos toward next, sliding along each
 * face that stops it. it returns how many faces it slid along, up to
 * CAMERA_PROBE_SLIDES, and slides holds them in turn. hit is where the
 * sphere comes to rest. camera_probe_move takes the faces the ball touches
 * that pos isn't behind, and camera_probe_move_two_sided every one. each
 * takes 128 at most. their sweeps are loose, as polygon_sweep says.
 *
 * each slide heads from pos for next. a face stops it where the line from
 * pos to next, made radius longer, crosses the face. a face also stops it where
 * next lies over the face, within radius of its plane. the slide stops at the
 * face whose crossing, or next's foot on it, lies nearest pos. it passes over
 * the face the slide before stopped at. where no face stops it, the sphere
 * rests at next. otherwise next moves to its foot on the plane radius in front
 * of the face. the next slide heads there. after the last slide the sphere
 * rests at that next, unchecked. njDistanceL2PL can count a slide's line
 * parallel to a face's plane. the line then crosses it at the plane's point
 * nearest pos */
int camera_probe_move(struct camera_probe *probe);
int camera_probe_move_two_sided(struct camera_probe *probe);

/* whether point lies on the triangle of corners, and within the sphere
 * whose diameter is line's segment. the segment runs from line's point to
 * that point plus line's vector. point must lie in the triangle's plane.
 *
 * the test sees the triangle along one world axis, which the corners choose. it
 * works in that view's own x and y. seen along y they are the world's x and z.
 * seen along z they are x and y, and seen along x, z and y. a point on an edge
 * counts where the triangle lies at a higher view y than the edge. on an edge
 * upright in the view, it counts where the triangle lies at a higher view x. a
 * point on any other edge doesn't count */
Bool camera_probe_on_triangle(NJS_POINT3 *corners, NJS_POINT3 *point,
                              NJS_LINE *line);

#endif
