/* collision meshes: their pools, the buckets that sort their polygons by
 * z, and the searches for their surfaces */
#ifndef COLLISION_MESH_H
#define COLLISION_MESH_H
#include <ninja.h>
#include <sg_xpt.h>

struct collision_mesh;
struct floor_probe;
struct height_probe;
struct polygon;

/* the meshes in use, newest first */
extern struct collision_mesh *collision_mesh_list;

/* the free polygons and the free meshes each form a list linked through
 * next. an allocation returns null rather than take a list's last record.
 * so a pool of count records lends count - 1 */

/* make polygons[0..count) the free polygons, writing only their next.
 * count must be at least 2 */
void polygon_pool_init(struct polygon *polygons, int count);
/* make meshes[0..count) the free meshes, writing only their next, and
 * empty collision_mesh_list. count must be at least 2 */
void collision_mesh_pool_init(struct collision_mesh *meshes, int count);
/* take a free mesh and put it first in collision_mesh_list, writing only
 * its next and prev. returns it, or null */
struct collision_mesh *collision_mesh_alloc(void);
/* take the mesh out of collision_mesh_list, which must hold it. return it
 * and its polygons to their free lists. its object becomes null, and its
 * polygons and last keep their values */
void collision_mesh_free(struct collision_mesh *mesh);
/* take a free polygon and put it first in the mesh's polygons, writing
 * only its next and prev. returns it, or null. no image calls it */
struct polygon *collision_mesh_add_polygon(struct collision_mesh *mesh);

/* polygon_alloc sorts a new mesh's polygons into 128 buckets by z, cut
 * toward 0 to a whole number. each bucket takes 16 whole numbers, from
 * -1024 up. the first and the last also take every z beyond them.
 * collision_mesh_take_buckets doesn't empty the buckets, so a caller must
 * empty them by collision_mesh_clear_buckets before each new mesh */

void collision_mesh_clear_buckets(void);
/* take a free polygon and put it first in the bucket for z, writing only
 * its next and prev. returns it, or null */
struct polygon *polygon_alloc(Float z);
/* join the buckets' lists, lowest z first, as the mesh's polygons, from
 * mesh->polygons to mesh->last. within a bucket, the newest polygon comes
 * first. with every bucket empty, it sets only mesh->last, to null */
void collision_mesh_take_buckets(struct collision_mesh *mesh);

/* polygon_find_floor and polygon_find_heights search the first mesh in
 * collision_mesh_list whose object is object. each returns TRUE where it takes
 * a surface. neither reads flags.
 *
 * a search walks the polygons whose centers lie within the mesh's span of
 * probe->pos in z. it stops at the first center beyond them. within a bucket
 * the polygons lie newest first, not in z order. so a search can miss a polygon
 * its bucket holds past the center it stops at. a shipped bug has
 * polygon_find_floor pass one polygon untested where it skips ahead to the
 * window. a search tests only the polygons near pos. a polygon is near where
 * pos's squared distance from its center is radius2 + 900 or less.
 *
 * a polygon gives a surface where pos, seen along y, lies inside it or on an
 * edge. its projection on x and z must have an area over 0.005. a, b and c
 * are the polygon's vertex[0] to vertex[2]. n is (b - a) x (c - a). the
 * surface's pos is the polygon's y at pos's x and z, and its flags are the
 * mesh's. its normal is n as a unit vector. its ang_x and ang_z turn y to that
 * normal, at 10430 to a radian, cut toward 0 */

/* probe->floor takes the first surface the search finds. a later polygon's
 * surface, at height h, replaces it where floor.pos - n.y is over |h - n.y|.
 * a shipped bug has that test read n.y in place of pos.y. with no surface,
 * floor.found is FALSE and the rest of probe keeps its values */
Bool polygon_find_floor(struct floor_probe *probe, NJS_OBJECT *object,
                        Uint32 flags);
/* probe->below takes the highest surface at or under pos.y, and
 * probe->above the lowest over it. a side with no surface has found FALSE.
 * its pos is then pos.y - 220 for below, or pos.y + 220 for above. with no
 * mesh for object, both keep their pos */
Bool polygon_find_heights(struct height_probe *probe, NJS_OBJECT *object,
                          Uint32 flags);

/* the square of the length of the vector (x, y, z), as the SH-4's FIPR
 * instruction sums it */
Float vector_length2(Float x, Float y, Float z);
/* the squared distance between the points, and across x and z alone. no
 * image calls mesh_point_distance2_across */
Float mesh_point_distance2(const NJS_POINT3 *from, const NJS_POINT3 *to);
Float mesh_point_distance2_across(const NJS_POINT3 *from, const NJS_POINT3 *to);
/* transform point in place by XMTRX, the matrix in the FPU's back bank,
 * translation included. no image calls it */
void matrix_calc_point(NJS_POINT3 *point);

#endif
