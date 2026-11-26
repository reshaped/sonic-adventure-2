/* the collision meshes of the world's objects, and tests of balls, points
 * and rays against meshes and models */
#ifndef COLLISION_POLYGON_H
#define COLLISION_POLYGON_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/surface.h"

/* a surface a sweep's ball touched, in the world */
struct polygon_hit {
    /* the tilt about x and about z that turns y to normal */
    Angle ang_x;
    Angle ang_z;
    NJS_VECTOR front; /* the unit normal of the surface's front */
    /* front, or -front for a ball behind a two-sided surface */
    NJS_VECTOR normal;
    /* the point of the surface's plane nearest the ball's pos */
    NJS_POINT3 point;
    NJS_POINT3 pos; /* point moved the ball's radius along normal */
    NJS_VECTOR speed; /* the part of the ball's speed along the surface */
    NJS_VECTOR normal_speed; /* the part of the ball's speed along front */
};

/* a ball a sweep moves from pos along speed, and the surface it hit */
struct polygon_ball {
    Float radius;
    NJS_POINT3 pos;
    NJS_VECTOR speed;
    NJS_VECTOR up; /* the up axis of the ball's owner, which no sweep reads */
    Uint16 count; /* how many surfaces polygon_contacts holds */
    Uint16 hit_index; /* hit's index in polygon_contacts */
    Uint8 unk_2c[8];
    struct polygon_hit hit;
};

/* a surface the last sweep touched */
struct polygon_contact {
    struct polygon_hit hit;
    /* the signed distance from the surface's plane to the ball's pos, along
     * front */
    Float height;
    Float height_after; /* height at pos + speed */
};

/* the most surfaces a sweep lists */
#define POLYGON_CONTACTS 128
/* the surfaces touched by the last sweep that touched one. that sweep's
 * ball counts them. polygon_sweep puts each one's corners in
 * polygon_contact_corners, at the same index */
extern struct polygon_contact polygon_contacts[POLYGON_CONTACTS];
extern NJS_POINT3 polygon_contact_corners[POLYGON_CONTACTS][3];

/* Running counts from polygon_sweep: candidate triangles, searches that
 * reach the end of the ball's motion, and contacts with triangles. */
extern Uint32 polygon_sweep_tests;
extern Uint32 polygon_sweep_ends;
extern Uint32 polygon_sweep_touches;

/* a triangle of a collision mesh, in the world. a mesh lists its triangles
 * by center z, lowest first, as far as mesh.h's buckets sort them */
struct polygon {
    struct polygon *next;
    struct polygon *prev;
    NJS_POINT3 center; /* its corners' mean, their sum times 0.3333 */
    /* the squared distance from center to its farthest corner.
     * collision_mesh_init sets it to 0, which polygon_pool_seek_free takes
     * for a triangle no mesh holds */
    Float radius2;
    NJS_POINT3 vertex[3];
    Uint32 attr; /* its material's attrflags, or 0 for a model with none */
};

/* the collision mesh of one of the world's objects: its models' triangles,
 * where the object puts them in the world */
struct collision_mesh {
    struct collision_mesh *next;
    /* the mesh before it in collision_mesh_list. the first mesh holds the
     * negated address of collision_mesh_list instead. collision_mesh_free
     * tells that from a mesh by its sign */
    struct collision_mesh *prev;
    NJS_OBJECT *object; /* null when the mesh is free or forgotten */
    Uint32 flags; /* the object's world flags */
    struct polygon *polygons; /* its first triangle */
    struct polygon *last; /* its last */
    Sint32 unk_18;
    /* where polygon_collect_near's list holds this mesh's triangles, or
     * null */
    struct polygon **near;
    Sint32 unk_20;
    int near_count; /* the number of this mesh's triangles at near */
    Float span; /* its triangles' largest extent in z */
    Float unk_2c;
    NJS_POINT3 center; /* its root model's center, in the world */
    Sint32 unk_3c;
};

/* a point, its reach, and the surface of one object nearest it straight
 * above or below */
struct level_probe {
    Float radius; /* with speed, widens the reach a triangle is tested in */
    NJS_POINT3 pos;
    NJS_VECTOR speed;
    Uint8 unk_1c[20];
    Float level; /* the surface's height */
    /* the tilt about x and about z that turns y to normal */
    Angle ang_x;
    Angle ang_z;
    NJS_VECTOR normal; /* a unit vector */
    Uint8 unk_48[60];
};

/* a ray, and the nearest surface it meets */
struct ray_query {
    /* polygon_cast_ray makes it 400 when it's over 400 or under 0.01 */
    Float length;
    NJS_POINT3 origin;
    NJS_VECTOR dir; /* a unit vector */
    Float distance; /* from origin to hit */
    NJS_POINT3 hit;
    struct surface_side surface;
};

/* collision_mesh_init frees every mesh and polygon, as a stage starts.
 *
 * collision_mesh_update runs each step. it lists in world_objects the
 * objects near the players, by world_collect_near_players, and takes the
 * first 192. each of those with WORLD_MOVING, or without a mesh, wants a
 * new one. the update frees the meshes of moving objects and of objects no
 * longer listed. it frees none where all 192 want a mesh. it then makes the
 * meshes wanted. the pool lends at most 191 meshes, and the update writes
 * each mesh wanted past those through null.
 *
 * each returns TRUE */
Bool collision_mesh_init(void);
Bool collision_mesh_update(void);
/* free every polygon and mesh, as collision_mesh_init does. the meshes'
 * objects and the polygons' radius2 keep their values. no image calls it */
void collision_mesh_link_pools(void);
/* move the pool's cursor to the first polygon at or past it whose radius2
 * is 0. the cursor becomes null where none is left. nothing points the
 * cursor into the pool, so a call reads radius2 through null. no image
 * calls it */
void polygon_pool_seek_free(void);
/* set the object of object's mesh to null, as object leaves the world. a
 * later update frees the mesh. an object with no mesh changes nothing */
void collision_mesh_forget(NJS_OBJECT *object);

/* list the triangles polygon_sweep tests for the ball, mesh by mesh, and
 * return how many, at most 192. each mesh's near and near_count give its
 * part of the list. once the list is full, the meshes left keep the near
 * and near_count they had.
 *
 * only the meshes of the objects world_list_near lists within the ball's
 * radius of pos count. it reads through null for a mesh that holds no
 * triangle. let reach be the ball's radius plus speed's length. a triangle's
 * limit is reach * reach plus its radius2. the triangle is near where its
 * center's squared distance from pos is at most its limit.
 *
 * the list takes the near triangles a forward walk of each mesh meets. the
 * walk's window is pos's z, give or take reach plus the mesh's span. the walk
 * starts at the mesh's first triangle where that one's center lies above the
 * window's low end. where the first center lies below the low end, the walk
 * skips to the first center above it. it does so only where the last center
 * lies above the high end. a mesh that fits neither case gives none. the walk
 * stops at the first center at or above the high end */
int polygon_collect_near(struct polygon_ball *ball);
/* sweep the ball over the triangles polygon_collect_near listed from
 * object's mesh. the ball's center steps from pos toward pos + speed, half
 * its radius at a time. its last step ends at pos + speed. the ball touches
 * a triangle where one of those points lies within radius of its plane. the
 * point must also lie over the triangle along its normal. flags goes unread.
 *
 * where loose isn't FALSE, the point need not lie over the triangle. let r
 * be the distance from the corners' mean, their sum times 0.33333, to the
 * farthest corner. the point must instead lie within r of some corner.
 *
 * it returns TRUE when the ball touches any triangle. polygon_contacts then
 * lists the touches, at most POLYGON_CONTACTS, and polygon_contact_corners
 * their corners. the ball takes their count, and the index of the touch
 * with the lowest height_after. its hit is that touch's, with point and pos
 * averaged over all of them. a touch that fills polygon_contacts ends the
 * sweep, and leaves a matrix pushed on the matrix stack.
 *
 * it returns FALSE where radius isn't over 0, object has no mesh, or the
 * ball touches nothing. polygon_contacts, polygon_contact_corners and the
 * ball then stay as they were */
Bool polygon_sweep(struct polygon_ball *ball, NJS_OBJECT *object, Uint32 flags,
                   Bool loose);
/* sweep the ball as polygon_sweep does with loose FALSE, over the models of
 * object's tree. it returns TRUE where the ball touches any triangle. each
 * triangle lies where the tree puts it. a triangle's squared reach is the
 * sum of the squares of its longest edge, speed's length and radius. the
 * sweep tests a triangle only where each of its corners lies within that
 * reach of pos. flags changes nothing.
 *
 * the port keeps the first POLYGON_CONTACTS touches in traversal order.
 * the original checks the count only between strips. the sweep leaves
 * polygon_contact_corners as it was. the ball takes the touches' count and
 * the index of the touch nearest its plane. that compares the first
 * touch's |height| with each other's |height_after|. the ball's hit is
 * the nearest's, with point and pos averaged over all of them. with no
 * touch, the ball keeps its values.
 * no image calls it */
Bool polygon_sweep_model(struct polygon_ball *ball, NJS_OBJECT *object,
                         Uint32 flags);

/* the searches declared below look along an axis for a point's surfaces. a
 * triangle gives one where the point, seen along that axis, lies inside it
 * or on an edge. the triangle's projection across that axis must also have
 * an area of at least 0.005. polygon_find_nearest_height takes any area
 * over 0. each search but polygon_find_near_levels returns TRUE where it
 * finds a surface, and FALSE otherwise */

/* for each of the probe's sides, take the nearest surface of object's mesh
 * on that side of pos. a surface counts where it crosses the line through
 * pos along the side's axis. a crossing at pos counts for the side toward
 * +x, +y or +z. a surface takes the mesh's world flags. a triangle counts
 * only where its first corner lies within 220 of pos. a side with no
 * surface has found FALSE, and its pos 220 from pos toward the opposite
 * side. that holds for an object with no mesh too. flags goes unread */
Bool polygon_find_sides(struct surface_probe *probe, NJS_OBJECT *object,
                        Uint32 flags);
/* take the surface of object's own model nearest the probe's pos, straight
 * above or below it. a triangle must pass a reach test like
 * polygon_sweep_model's, with the probe's radius and speed. a shipped bug
 * has that test measure to stack words, not to the triangle's corners.
 * with no surface, the probe keeps its values. flags goes unread. only
 * polygon_find_near_levels calls it, and no image calls that */
Bool polygon_find_nearest_height(struct level_probe *probe, NJS_OBJECT *object,
                                 Uint32 flags);
/* take the highest surface of object's own model below pos, and the lowest
 * at or over pos. a side with no surface has found FALSE, and the rest of
 * it keeps its values */
Bool polygon_find_model_heights(struct height_probe *probe, NJS_OBJECT *object);
/* take the sides' surfaces as polygon_find_sides does, from object's own
 * model. a reach test of the size class in flags takes the place of
 * polygon_find_sides' limit of 220. a shipped bug has that test measure to
 * stack words, not to a corner. a side with no surface has found FALSE, and
 * the rest of it keeps its values. no side's flags change. no image calls
 * it */
Bool polygon_find_model_sides(struct surface_probe *probe, NJS_OBJECT *object,
                              Uint32 flags);
/* take the surfaces of each object world_list_near lists within radius of
 * the probe's pos, as polygon_find_model_heights does. a triangle must also
 * pass polygon_find_model_sides' reach test. the probe's below then takes the
 * lowest of the objects' surfaces below pos. its above takes the highest of
 * those above. each side's found says whether it took one. their other
 * fields, and a pos whose found is FALSE, take stack words no code wrote. no
 * image calls it */
Bool polygon_find_near_heights(struct height_probe *probe, Float radius);
/* take for each object world_list_near lists within radius of pos the one
 * surface polygon_find_nearest_height finds. *above gets the lowest of those
 * over pos, and *below the highest of those at or under it. each takes its
 * surface's angles too, through the angle pointers that aren't null. a level
 * with no surface ends on the wrong side of pos. *above then ends at
 * pos->y - 100000, and *below at pos->y + 100000. the reach test reads a
 * radius and speed that no code sets. no image calls it */
void polygon_find_near_levels(NJS_POINT3 *pos, Float radius, Float *above,
                              Angle *above_x, Angle *above_z, Float *below,
                              Angle *below_x, Angle *below_z);

/* cast the query's ray at the meshes of the objects world_list_near lists
 * within length of origin. the ray meets a one-sided triangle only from the
 * front, and meets no triangle it runs along. it returns TRUE where it meets a
 * surface within length. the query then takes the nearest one's distance, hit
 * and surface, but for surface.pos. surface.normal is the front's, from either
 * side. it returns FALSE where it meets none, with surface.found FALSE. the
 * rest of the query then stays as it was, but for length */
Bool polygon_cast_ray(struct ray_query *query);

/* the triangle's normal (v1 - v0) x (v2 - v0), as long as twice its area.
 * each point is the address of its x, which its y and z follow. no image
 * calls it */
void polygon_normal(const Float *v0, const Float *v1, const Float *v2,
                    Float *normal);
/* the squared distance between the points, and across x and z alone. no
 * image calls polygon_point_distance2_across */
Float polygon_point_distance2(const NJS_POINT3 *from, const NJS_POINT3 *to);
Float polygon_point_distance2_across(const NJS_POINT3 *from,
                                     const NJS_POINT3 *to);
/* the squared distance between the three Floats at each pointer.
 * polygon_distance2_fipr gives it by FIPR. no image calls either */
Float polygon_distance2(Float *from, Float *to);
Float polygon_distance2_fipr(Float *from, Float *to);

/* how far point is from the segment from start to end. the segment's
 * nearest point goes to nearest, if it isn't null */
Float collision_segment_distance(NJS_POINT3 *point, NJS_POINT3 *start,
                                 NJS_POINT3 *end, NJS_POINT3 *nearest);
/* TRUE when point lies beside the segment from start to end, not past
 * either end. otherwise FALSE, and it changes nothing. for TRUE, distance2
 * gets the squared distance from point to the segment, and nearest the
 * segment's nearest point. a segment under 0.01 long along each axis counts
 * as start, and leaves nearest as it was. no image calls it */
Bool collision_segment_nearest(NJS_POINT3 *start, NJS_POINT3 *end,
                               NJS_POINT3 *point, NJS_POINT3 *nearest,
                               Float *distance2);

#endif
