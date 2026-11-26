/* the collision meshes of the world's objects, and tests of balls, points
 * and rays against meshes and models */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/mesh.h"
#include "collision/polygon.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/vector.h"

/* polygon_find_sides starts each side REACH from pos. it takes a triangle
 * only where its first corner lies within REACH of pos. polygon_cast_ray takes
 * a triangle whose plane lies within length + REACH of the ray's origin. a
 * model's reach test reaches REACH in size class 0 */
#define REACH 220.0F

/* the longest and the shortest ray polygon_cast_ray follows. it makes a
 * length outside them RAY_LENGTH */
#define RAY_LENGTH 400.0F
#define RAY_LENGTH_MIN 0.01F

/* the most points a strip holds: strip_next cuts a longer one short */
#define STRIP_POINTS 1024
/* the records in the pool of meshes, and in the pool of polygons the
 * meshes share */
#define MESHES 192
#define MESH_POLYGONS 8192
/* the most polygons polygon_collect_near lists, and the count at which it
 * warns that the list is filling */
#define NEAR_POLYGONS 192
#define NEAR_POLYGONS_MANY 96

/* polygon_collect_near multiplies a triangle's limit, reach * reach plus its
 * radius2, by this. at 1 it changes nothing */
#define NEAR_SCALE 1.0F

/* how far from pos polygon_find_near_levels starts *above and *below */
#define FAR_LEVEL 100000.0F

/* the bits of a strip's count word that hold its count. NJD_TRIMESH_END
 * there marks a strip whose first triangle winds the other way */
#define STRIP_COUNT 0x7fff

/* a vector's part along an axis counts as none under 0.01. the tests
 * compare its square with this */
#define NO_PART2 (0.01F * 0.01F)
/* polygon_sweep turns a triangle whose normal's y is over this by nothing.
 * it turns one whose y is under its negative by half a turn, both without
 * acosf */
#define UPRIGHT_NORMAL 0.99999F
/* polygon_cast_ray meets a triangle only where the ray's direction runs at
 * least this far toward its plane */
#define GRAZING 0.01F
/* a sweep moves the ball this share of its radius between tests */
#define SWEEP_STEP 0.5F
/* a third, as a polygon's center and polygon_sweep's loose test round it */
#define CENTER_THIRD 0.3333F
#define MIDDLE_THIRD 0.33333F

/* the debug display prints a pool's use in COLOR_WARNING from this percent
 * on. it then goes back to COLOR_TEXT */
#define USED_WARNING 95
#define COLOR_WARNING 0xffff8080
#define COLOR_TEXT 0xffffffff
/* where the debug display prints, in njPrint's columns and rows. each
 * warning stands at WARNING_COLUMN on a row of its own. the meshes' and the
 * polygons' use each take a row: a label, a percent and a count. the meshes'
 * row then gives the most polygons any mesh holds. the title stands at
 * TITLE_COLUMN on the polygons' row */
#define WARNING_COLUMN 2
#define STRIP_FULL_ROW 17
#define MESHES_FULL_ROW 18
#define POLYGONS_FULL_ROW 19
#define NEAR_FULL_ROW 20
#define NEAR_MANY_ROW 21
#define TOUCHES_FULL_ROW 22
#define MESHES_ROW 26
#define POLYGONS_ROW 27
#define TITLE_COLUMN 2
#define LABEL_COLUMN 10
#define PERCENT_COLUMN 15
#define PERCENT_SIGN_COLUMN 18
#define COUNT_COLUMN 20
#define MOST_LABEL_COLUMN 27
#define MOST_COLUMN 35
/* the digits the debug display prints a percent, a count and the most
 * polygons in */
#define PERCENT_DIGITS 3
#define COUNT_DIGITS 5
#define MOST_DIGITS 4

/* the evalflags of a node that leaves the matrix alone, and of one that only
 * moves it */
#define EVAL_UNIT (NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL)
#define EVAL_MOVE_ONLY (NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL)

#define DEBUG_SHOWN (game_state.debug_display == 1)

#define DOT(a, b) (((a)->x * (b)->x) + ((a)->y * (b)->y) + ((a)->z * (b)->z))

/* the side of line ab that point lies on, in the plane of axes u and v. the
 * sign gives the side, and 0 is on the line. a point inside a triangle
 * gives its three edges the same sign */
#define EDGE_SIDE(a, b, point, u, v)                                           \
    ((((b)->u - (a)->u) * ((point)->v - (a)->v)) -                             \
     (((b)->v - (a)->v) * ((point)->u - (a)->u)))

/* the normal made a unit vector, or straight up when it has no length */
#define UNIT_NORMAL(normal)                                                    \
    if (njScalor(normal) == 0.0F) {                                            \
        (normal)->x = 0.0F;                                                    \
        (normal)->y = 1.0F;                                                    \
        (normal)->z = 0.0F;                                                    \
    } else {                                                                   \
        njUnitVector(normal);                                                  \
    }

/* the vector made a unit vector, or left with no length */
#define UNIT_VECTOR(vector)                                                    \
    if (njScalor(vector) == 0.0F) {                                            \
        (vector)->x = (vector)->y = (vector)->z = 0.0F;                        \
    } else {                                                                   \
        njUnitVector(vector);                                                  \
    }

/* the tilt about x and about z that turns y to a unit normal */
#define NORMAL_ANG_X(normal) ((Angle)(asinf((normal).z) * ANGLE_PER_RADIAN))
#define NORMAL_ANG_Z(normal)                                                   \
    (-(Angle)(atan2f((normal).x, (normal).y) * ANGLE_PER_RADIAN))

/* a cursor through a model's meshsets, and the points of the polygon or
 * strip it has reached */
struct strip {
    int kind; /* the meshset's type, NJD_MESHSET_3 to NJD_MESHSET_TRIMESH */
    int count; /* the points the polygon or strip holds */
    /* the meshset's next polygon's indices, or for a strip the count word
     * before them */
    Sint16 *indices;
    NJS_MODEL *model;
    NJS_POINT3 points[STRIP_POINTS];
};

/* a ball's sweep over the models of an object's tree */
struct sweep {
    Bool hit;
    int count; /* the contacts found so far */
    int tested; /* the triangles that passed the reach test */
    Float reach2; /* SIZE_REACH2 of the object's flags */
    Float length; /* the length of the ball's speed */
    Float spacing; /* how far the ball goes between tests */
    struct polygon_contact *contacts;
};

/* a triangle polygon_sweep touched: a copy of it, and the contact */
struct touch_polygon {
    Sint32 unk_00;
    struct polygon polygon;
    struct polygon_contact contact;
};

struct polygon_contact polygon_contacts[POLYGON_CONTACTS];
NJS_POINT3 polygon_contact_corners[POLYGON_CONTACTS][3];
/* the polygon polygon_pool_seek_free looks from, and its index in
 * polygon_pool */
static struct polygon *polygon_cursor;
static int polygon_cursor_index;
/* a word no code reads or writes */
static Uint32 dat_8c1c1dc8;
static struct collision_mesh mesh_pool[MESHES];
static struct polygon polygon_pool[MESH_POLYGONS];
/* the triangles polygon_collect_near listed, mesh by mesh */
static int near_polygon_count;
static struct polygon *near_polygons[NEAR_POLYGONS];
/* the triangles polygon_sweep has touched */
static int touch_polygon_count;
static struct touch_polygon touch_polygons[POLYGON_CONTACTS];
/* an object whose mesh collision_mesh_update makes again. no code sets it
 * but to null */
static NJS_OBJECT *mesh_stale_object;
/* the corners of the triangle polygon_sweep last touched, moved a unit
 * along its normal, for touch_triangle to draw. polygon_sweep sets them
 * only while the debug display shows */
static NJS_POINT3 touch_points[3];
/* what collision_mesh_update knows of the mesh it is making */
static Float mesh_center_z; /* the z its buckets sort centers from */
static Float mesh_span; /* its polygons' largest extent in z so far */
static Float dat_8c249f04; /* 0, which the mesh's unk_2c takes */
/* polygon_sweep's matrix, kept while it draws a touched triangle */
static NJS_MATRIX touch_matrix;

/* the touched triangle on the debug display: touch_points, with its corners
 * red, green and blue. SHC refuses braces around each color, with error
 * 2129 */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_COLOR touch_colors[3] = {0x00ff0000, 0x0000ff00, 0x000000ff};
/* NOLINTEND(clang-diagnostic-missing-braces) */
static NJS_POINT3COL touch_triangle = {touch_points, touch_colors, NULL, 1};

/* the squared reach of a model test, by the size class in the object's
 * world flags. class 0 reaches REACH, and each other class its largest size
 * times CLASS_REACH_SCALE */
#define CLASS_REACH_SCALE 1.01F
#define CLASS_REACH2(largest)                                                  \
    (((largest) * CLASS_REACH_SCALE) * ((largest) * CLASS_REACH_SCALE))
static const Float size_reach2[WORLD_SIZE_CLASSES] = {
    REACH * REACH,
    CLASS_REACH2(WORLD_SIZE_CLASS_1_MAX),
    CLASS_REACH2(WORLD_SIZE_CLASS_2_MAX),
    CLASS_REACH2(WORLD_SIZE_CLASS_3_MAX),
};

/* size_reach2's entry for the size class in an object's world flags */
#define SIZE_REACH2(flags) size_reach2[WORLD_SIZE_CLASS_OF(flags)]

static Bool find_model_heights_near(struct height_probe *probe,
                                    NJS_OBJECT *object, Uint32 flags);

/* polygon_normal's twin, which the tests here call */
static void triangle_normal(const Float *v0, const Float *v1, const Float *v2,
                            Float *normal)
{
    Float ux;
    Float uy;
    Float uz;
    Float vx;
    Float vy;
    Float vz;

    vx = *v0++;
    ux = *v1++ - vx;
    /* @hack spelling: vy set inside the difference, where a statement of its
     * own would do */
    uy = *v1++ - (vy = *v0++);
    vz = *v0;
    uz = *v1;
    vx -= *v2++;
    vy -= *v2++;
    uz -= vz;
    vz -= *v2;
    normal[0] = (uz * vy) - (uy * vz);
    normal[1] = (ux * vz) - (uz * vx);
    normal[2] = (uy * vx) - (ux * vy);
}

/* step to the meshset's next polygon or strip, and put its points through
 * the current matrix in strip->points. returns whether it is a strip whose
 * first triangle winds the other way */
static Bool strip_next(struct strip *strip)
{
    NJS_POINT3 *points;
    NJS_POINT3 *out;
    Sint16 *indices;
    int i;
    Bool reversed = FALSE;

    switch (strip->kind) {
    case NJD_MESHSET_3:
    case NJD_MESHSET_4:
        indices = strip->indices;
        points = strip->model->points;
        out = strip->points;
        for (i = strip->count; i > 0; i--) {
            njCalcPoint(NULL, &points[*indices], out);
            indices++;
            out++;
        }
        strip->indices = indices;
        break;
    case NJD_MESHSET_N:
    case NJD_MESHSET_TRIMESH:
        strip->count = *strip->indices & STRIP_COUNT;
        if (*strip->indices & NJD_TRIMESH_END) {
            reversed = TRUE;
        }
        if (strip->count > STRIP_POINTS) {
            strip->count = STRIP_POINTS;
            if (DEBUG_SHOWN) {
                njPrintC(NJM_LOCATION(WARNING_COLUMN, STRIP_FULL_ROW),
                         "WARNING: STRIP buf is FULL");
            }
        }
        strip->indices++;
        points = strip->model->points;
        out = strip->points;
        indices = strip->indices;
        for (i = strip->count; i > 0; i--) {
            njCalcPoint(NULL, &points[*indices], out);
            indices++;
            out++;
        }
        strip->indices = indices;
        break;
    }
    return reversed;
}

/* strip_next, with the points as the model has them */
static Bool strip_next_plain(struct strip *strip)
{
    NJS_POINT3 *points;
    NJS_POINT3 *out;
    Sint16 *indices;
    int i;
    Bool reversed = FALSE;

    switch (strip->kind) {
    case NJD_MESHSET_3:
    case NJD_MESHSET_4:
        indices = strip->indices;
        points = strip->model->points;
        out = strip->points;
        for (i = strip->count; i > 0; i--) {
            *out = points[*indices];
            indices++;
            out++;
        }
        strip->indices = indices;
        break;
    case NJD_MESHSET_N:
    case NJD_MESHSET_TRIMESH:
        strip->count = *strip->indices & STRIP_COUNT;
        if (*strip->indices & NJD_TRIMESH_END) {
            reversed = TRUE;
        }
        if (strip->count > STRIP_POINTS) {
            strip->count = STRIP_POINTS;
            if (DEBUG_SHOWN) {
                njPrintC(NJM_LOCATION(WARNING_COLUMN, STRIP_FULL_ROW),
                         "WARNING: STRIP buf is FULL");
            }
        }
        strip->indices++;
        points = strip->model->points;
        out = strip->points;
        indices = strip->indices;
        for (i = strip->count; i > 0; i--) {
            *out = points[*indices];
            indices++;
            out++;
        }
        strip->indices = indices;
        break;
    }
    return reversed;
}

/* strip_next, with the points moved by the object's position */
static Bool strip_next_moved(struct strip *strip, NJS_OBJECT *object)
{
    NJS_POINT3 *points;
    NJS_POINT3 *out;
    /* @hack spelling: point, a Float cursor each loop steps across a point's
     * members, where points[*indices].x, .y and .z would do */
    Float *point;
    Float x;
    Float y;
    Float z;
    Sint16 *indices;
    int i;
    Bool reversed = FALSE;

    switch (strip->kind) {
    case NJD_MESHSET_3:
    case NJD_MESHSET_4:
        x = object->pos[0];
        y = object->pos[1];
        z = object->pos[2];
        indices = strip->indices;
        points = strip->model->points;
        out = strip->points;
        for (i = strip->count; i > 0; i--) {
            point = &points[*indices].x;
            out->x = *point++ + x;
            out->y = *point++ + y;
            out->z = *point + z;
            indices++;
            out++;
        }
        strip->indices = indices;
        break;
    case NJD_MESHSET_N:
    case NJD_MESHSET_TRIMESH:
        strip->count = *strip->indices & STRIP_COUNT;
        if (*strip->indices & NJD_TRIMESH_END) {
            reversed = TRUE;
        }
        if (strip->count > STRIP_POINTS) {
            strip->count = STRIP_POINTS;
            if (DEBUG_SHOWN) {
                njPrintC(NJM_LOCATION(WARNING_COLUMN, STRIP_FULL_ROW),
                         "WARNING: STRIP buf is FULL");
            }
        }
        strip->indices++;
        /* @hack spelling: y, z and x read in that order, where x, y and z,
         * as the case above reads them, would do */
        y = object->pos[1];
        z = object->pos[2];
        x = object->pos[0];
        points = strip->model->points;
        indices = strip->indices;
        out = strip->points;
        for (i = strip->count; i > 0; i--) {
            point = &points[*indices].x;
            out->x = *point++ + x;
            out->y = *point++ + y;
            out->z = *point + z;
            indices++;
            out++;
        }
        strip->indices = indices;
        break;
    }
    return reversed;
}

/* sweep the ball over the triangles of object's own model, through the
 * current matrix. each triangle it touches joins the sweep's contacts. it
 * starts no meshset or strip once the sweep holds POLYGON_CONTACTS */
static void sweep_model(struct polygon_ball *ball, NJS_OBJECT *object,
                        struct sweep *sweep)
{
    struct strip strip;
    NJS_POINT3 tri[3];
    NJS_POINT3 pos;
    NJS_VECTOR speed;
    NJS_VECTOR foot;
    NJS_VECTOR normal;
    NJS_POINT3 out;
    NJS_VECTOR part;
    NJS_VECTOR stride;
    NJS_POINT3 at;
    NJS_MESHSET *meshset;
    NJS_POINT3 *corner;
    struct polygon_contact *contact;
    int meshsets;
    int strips;
    int i;
    Bool reversed;
    Bool hit;
    Bool more;
    Uint32 two_sided;
    Angle ang_x;
    Angle ang_y;
    Float *v0;
    Float *v1;
    Float *v2;
    /* @hack spelling: edge1 and edge2, byte cursors over tri[1] and tri[2],
     * where stores to the two points' members would do */
    unsigned char *edge1;
    unsigned char *edge2;
    Float dx;
    Float dy;
    Float dz;
    Float reach2;
    Float edge;
    Float x;
    Float y;
    Float z;
    Float x0;
    Float y0;
    Float z0;
    Float x1;
    Float y1;
    Float z1;
    Float radius;
    Float travelled;
    Float e1;
    Float e2;
    Float e3;

    strip.model = object->model;
    meshsets = strip.model->nbMeshset;
    for (meshset = strip.model->meshsets;
         meshsets > 0 && sweep->count < POLYGON_CONTACTS;
         meshsets--, meshset++) {
        /* @hack spelling: the assignment inside the switch, where a statement
         * of its own would do */
        switch (strip.kind = meshset->type_matId & NJD_MESHSET_MASK) {
        case NJD_MESHSET_3:
            strip.count = 3;
            break;
        case NJD_MESHSET_4:
            strip.count = 4;
            break;
        case NJD_MESHSET_N:
        case NJD_MESHSET_TRIMESH:
            break;
        default:
            continue;
        }
        strips = meshset->nbMesh;
        strip.indices = meshset->meshes;
        for (; strips && sweep->count < POLYGON_CONTACTS; strips--) {
            reversed = strip_next(&strip);
            tri[0].x = tri[0].y = tri[0].z = 0.0F;
            for (i = 0, corner = strip.points; i < strip.count - 2;
                 i++, corner++) {
                /* @hack spelling: Float cursors stepped across the corners'
                 * members from &corner->x, + 3 and + 6, where corner[0] to
                 * corner[2]'s members would do */
                v0 = &corner->x;
                v1 = &corner->x + 3;
                v2 = &corner->x + 6;
                x1 = *v1++;
                y1 = *v1++;
                z1 = *v1;
                dx = x1 - *v0++;
                dy = y1 - *v0++;
                dz = z1 - *v0++;
                reach2 = (dx * dx) + (dy * dy) + (dz * dz);
                dx = *v2++ - x1;
                dy = *v2++ - y1;
                dz = *v2++ - z1;
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((edge = (dx * dx) + (dy * dy) + (dz * dz)) > reach2) {
                    reach2 = edge;
                }
                v0 -= 3;
                v2 -= 3;
                dx = *v0++ - *v2++;
                dy = *v0++ - *v2++;
                dz = *v0 - *v2;
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((edge = (dx * dx) + (dy * dy) + (dz * dz)) > reach2) {
                    reach2 = edge;
                }
                reach2 += (ball->speed.x * ball->speed.x) +
                          (ball->speed.y * ball->speed.y) +
                          (ball->speed.z * ball->speed.z) +
                          (ball->radius * ball->radius);
                /* @hack spelling: a Float cursor stepped across the members
                 * of the ball's pos, where ball->pos.x, .y and .z would do */
                v0 = &ball->pos.x;
                x = *v0++;
                y = *v0++;
                z = *v0;
                /* @hack spelling: Float cursors stepped across the corners'
                 * members again, in the reach tests and both arms below,
                 * where corner[0] to corner[2]'s members would do */
                v0 = &corner->x;
                dx = x - *v0++;
                dy = y - *v0++;
                dz = z - *v0;
                if ((dx * dx) + (dy * dy) + (dz * dz) > reach2) {
                    continue;
                }
                v0 = &corner->x + 3;
                dx = x - *v0++;
                dy = y - *v0++;
                dz = z - *v0;
                if ((dx * dx) + (dy * dy) + (dz * dz) > reach2) {
                    continue;
                }
                v0 = &corner->x + 6;
                dx = x - *v0++;
                dy = y - *v0++;
                dz = z - *v0;
                if ((dx * dx) + (dy * dy) + (dz * dz) > reach2) {
                    continue;
                }
                sweep->tested++;
                if (!((i + reversed) & 1)) {
                    v1 = &corner->x + 3;
                    v0 = &corner->x;
                    x0 = *v0++;
                    y0 = *v0++;
                    z0 = *v0;
                    edge1 = (unsigned char *)&tri[1];
                    *(Float *)edge1 = *v1++ - x0;
                    *(Float *)(edge1 + sizeof(Float)) = *v1++ - y0;
                    *(Float *)(edge1 + (2 * sizeof(Float))) = *v1 - z0;
                    edge2 = (unsigned char *)&tri[2];
                    v2 = &corner->x + 6;
                    *(Float *)edge2 = *v2++ - x0;
                    *(Float *)(edge2 + sizeof(Float)) = *v2++ - y0;
                    *(Float *)(edge2 + (2 * sizeof(Float))) = *v2 - z0;
                } else {
                    v2 = &corner->x + 6;
                    v0 = &corner->x;
                    x0 = *v0++;
                    y0 = *v0++;
                    z0 = *v0;
                    edge1 = (unsigned char *)&tri[1];
                    *(Float *)edge1 = *v2++ - x0;
                    *(Float *)(edge1 + sizeof(Float)) = *v2++ - y0;
                    *(Float *)(edge1 + (2 * sizeof(Float))) = *v2 - z0;
                    edge2 = (unsigned char *)&tri[2];
                    v1 = &corner->x + 3;
                    *(Float *)edge2 = *v1++ - x0;
                    *(Float *)(edge2 + sizeof(Float)) = *v1++ - y0;
                    *(Float *)(edge2 + (2 * sizeof(Float))) = *v1 - z0;
                }
                if ((tri[0].x == tri[1].x && tri[0].y == tri[1].y &&
                     tri[0].z == tri[1].z) ||
                    (tri[0].x == tri[2].x && tri[0].y == tri[2].y &&
                     tri[0].z == tri[2].z) ||
                    (tri[1].x == tri[2].x && tri[1].y == tri[2].y &&
                     tri[1].z == tri[2].z)) {
                    continue;
                }
                pos.x = ball->pos.x - corner->x;
                pos.y = ball->pos.y - corner->y;
                pos.z = ball->pos.z - corner->z;
                speed.x = ball->speed.x;
                speed.y = ball->speed.y;
                speed.z = ball->speed.z;
                triangle_normal(&tri[0].x, &tri[1].x, &tri[2].x, &normal.x);
                UNIT_NORMAL(&normal);
                ang_x = (Angle)(acosf(normal.y) * ANGLE_PER_RADIAN);
                ang_y = (Angle)(atan2f(normal.x, normal.z) * ANGLE_PER_RADIAN);
                njPushMatrix(&_nj_unit_matrix_);
                njRotateX(NULL, -ang_x);
                njRotateY(NULL, -ang_y);
                njCalcPoint(NULL, &tri[1], &out);
                tri[1] = out;
                njCalcPoint(NULL, &tri[2], &out);
                tri[2] = out;
                njCalcPoint(NULL, &pos, &out);
                pos = out;
                njCalcPoint(NULL, &speed, &out);
                speed = out;
                two_sided =
                    strip.model->mats[meshset->type_matId & ~NJD_MESHSET_MASK]
                        .attrflags &
                    NJD_FLAG_DOUBLE_SIDE;
                radius = ball->radius;
                if (two_sided) {
                    if (pos.y < 0.0F) {
                        radius *= -1.0F;
                    }
                } else {
                    /* @hack redundant: an empty else */
                }
                stride = speed;
                hit = FALSE;
                UNIT_VECTOR(&stride);
                stride.x *= sweep->spacing;
                stride.y *= sweep->spacing;
                stride.z *= sweep->spacing;
                at = pos;
                travelled = 0.0F;
                for (;;) {
                    more = TRUE;
                    if (travelled >= sweep->length) {
                        /* @hack spelling: Float cursors stepped across the
                         * members of pos and speed, where pos.x, speed.x
                         * and so on would do */
                        v0 = &pos.x;
                        v1 = &speed.x;
                        at.x = *v0++ + *v1++;
                        at.y = *v0++ + *v1++;
                        at.z = *v0 + *v1;
                        more = FALSE;
                    }
                    if (njAbs(at.y) <= ball->radius) {
                        e1 = EDGE_SIDE(&tri[0], &tri[1], &at, z, x);
                        e2 = EDGE_SIDE(&tri[1], &tri[2], &at, z, x);
                        e3 = EDGE_SIDE(&tri[2], &tri[0], &at, z, x);
                        if (e1 * e2 >= 0.0F && e2 * e3 >= 0.0F &&
                            e1 * e3 >= 0.0F) {
                            hit = TRUE;
                            break;
                        }
                    }
                    if (!more) {
                        break;
                    }
                    at.x = at.x + stride.x;
                    at.y = at.y + stride.y;
                    at.z = at.z + stride.z;
                    travelled += sweep->spacing;
                }
                if (!hit) {
                    njPopMatrixEx();
                    continue;
                }
                sweep->hit = TRUE;
                contact = &sweep->contacts[sweep->count];
                contact->height = pos.y;
                contact->height_after = pos.y + speed.y;
                contact->hit.front = normal;
                if (two_sided) {
                    if (pos.y >= 0.0F) {
                        contact->hit.normal = normal;
                    } else {
                        contact->hit.normal.x = -normal.x;
                        contact->hit.normal.y = -normal.y;
                        contact->hit.normal.z = -normal.z;
                    }
                } else {
                    contact->hit.normal = normal;
                }
                contact->hit.ang_z = NORMAL_ANG_Z(contact->hit.normal);
                contact->hit.ang_x = NORMAL_ANG_X(contact->hit.normal);
                njInvertMatrix(NULL);
                foot.x = pos.x;
                foot.y = 0.0F;
                foot.z = pos.z;
                njCalcPoint(NULL, &foot, &out);
                contact->hit.point.x = out.x + corner->x;
                contact->hit.point.y = out.y + corner->y;
                contact->hit.point.z = out.z + corner->z;
                foot.y = radius;
                njCalcPoint(NULL, &foot, &out);
                contact->hit.pos.x = out.x + corner->x;
                contact->hit.pos.y = out.y + corner->y;
                contact->hit.pos.z = out.z + corner->z;
                part = speed;
                /* @hack redundant: the negation of part.y, which the store of
                 * 0.0F below overwrites */
                if (two_sided) {
                    if (pos.y * speed.y < 0.0F) {
                        part.y = -speed.y;
                    }
                } else if (radius * speed.y < 0.0F) {
                    part.y = -speed.y;
                }
                part.x = speed.x;
                part.y = 0.0F;
                part.z = speed.z;
                njCalcPoint(NULL, &part, &out);
                contact->hit.speed = out;
                part.x = 0.0F;
                part.y = speed.y;
                part.z = 0.0F;
                njCalcPoint(NULL, &part, &out);
                contact->hit.normal_speed = out;
                njPopMatrixEx();
                sweep->count++;
                /* @bug memory: the triangles' loop doesn't test the count, so
                 * a strip that brings the contacts past POLYGON_CONTACTS
                 * writes the rest past polygon_contacts' end. the strips' and
                 * meshsets' loops test it, at 0x8c01360e and 0x8c01362e. the
                 * triangles' loop, CMP/GE R3,R2 at 0x8c0135f6, doesn't. the
                 * store indexes the contacts by the count (MUL.L R3,R2 at
                 * 0x8c0133bc) */
#ifdef NONMATCHING
                /* this side repairs the tag above: it ends the model's sweep
                 * at POLYGON_CONTACTS contacts */
                if (sweep->count == POLYGON_CONTACTS) {
                    return;
                }
#endif
            }
        }
    }
}

/* give the ball the hit and index of the sweep's nearest contact. the hit's
 * point and pos are averages over all the contacts */
static void sweep_take_nearest(struct polygon_ball *ball, struct sweep *sweep)
{
    struct polygon_ball sum;
    struct polygon_contact *contact;
    int i;
    int nearest;
    Float height;
    Float scale;

    nearest = 0;
    /* nearness is |height| for contact 0, and |height_after| for the rest */
    height = njAbs(sweep->contacts[0].height);
    for (i = 1; i < sweep->count; i++) {
        if (height > njAbs(sweep->contacts[i].height_after)) {
            nearest = i;
            height = njAbs(sweep->contacts[i].height_after);
        }
    }
    ball->hit_index = nearest;
    contact = &sweep->contacts[nearest];
    ball->hit.ang_x = contact->hit.ang_x;
    ball->hit.ang_z = contact->hit.ang_z;
    ball->hit.front = contact->hit.front;
    ball->hit.normal = contact->hit.normal;
    ball->hit.speed = contact->hit.speed;
    ball->hit.normal_speed = contact->hit.normal_speed;
    sum.hit.point.x = sum.hit.point.y = sum.hit.point.z = 0.0F;
    sum.hit.pos.x = sum.hit.pos.y = sum.hit.pos.z = 0.0F;
    scale = 1.0F / (Float)sweep->count;
    for (i = sweep->count, contact = sweep->contacts; i > 0; i--) {
        sum.hit.point.x += scale * contact->hit.point.x;
        sum.hit.point.y += scale * contact->hit.point.y;
        sum.hit.point.z += scale * contact->hit.point.z;
        sum.hit.pos.x += scale * contact->hit.pos.x;
        sum.hit.pos.y += scale * contact->hit.pos.y;
        sum.hit.pos.z += scale * contact->hit.pos.z;
        contact++;
    }
    ball->hit.point = sum.hit.point;
    ball->hit.pos = sum.hit.pos;
}

/* sweep the ball over the trees of object's children, then over its own
 * model, all through its transform. returns its sibling */
static NJS_OBJECT *sweep_tree(struct polygon_ball *ball, NJS_OBJECT *object,
                              struct sweep *sweep)
{
    NJS_OBJECT *child;

    if ((object->evalflags & EVAL_UNIT) ^ EVAL_UNIT) {
        njPushMatrixEx();
        if (!(object->evalflags & NJD_EVAL_UNIT_POS)) {
            njTranslateEx((NJS_VECTOR *)object->pos);
        }
        if (!(object->evalflags & NJD_EVAL_UNIT_ANG)) {
            njRotateZ(NULL, object->ang[2]);
            njRotateY(NULL, object->ang[1]);
            njRotateX(NULL, object->ang[0]);
        }
        if (!(object->evalflags & NJD_EVAL_UNIT_SCL)) {
            njScaleEx((NJS_VECTOR *)object->scl);
        }
    }
    child = object->child;
    while (child != NULL) {
        child = sweep_tree(ball, child, sweep);
    }
    sweep_model(ball, object, sweep);
    if ((object->evalflags & EVAL_UNIT) ^ EVAL_UNIT) {
        njPopMatrixEx();
    }
    return object->sibling;
}

Bool polygon_sweep_model(struct polygon_ball *ball, NJS_OBJECT *object,
                         Uint32 flags)
{
    struct sweep sweep;
    NJS_OBJECT *child;

    sweep.hit = FALSE;
    sweep.count = 0;
    sweep.tested = 0;
    sweep.contacts = polygon_contacts;
    sweep.reach2 = SIZE_REACH2(flags);
    sweep.length = njScalor(&ball->speed);
    sweep.spacing = ball->radius * SWEEP_STEP;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    if (!(object->evalflags & NJD_EVAL_UNIT_POS)) {
        njTranslateEx((NJS_VECTOR *)object->pos);
    }
    if (!(object->evalflags & NJD_EVAL_UNIT_ANG)) {
        njRotateZ(NULL, object->ang[2]);
        njRotateY(NULL, object->ang[1]);
        njRotateX(NULL, object->ang[0]);
    }
    if (!(object->evalflags & NJD_EVAL_UNIT_SCL)) {
        njScaleEx((NJS_VECTOR *)object->scl);
    }
    child = object->child;
    while (child != NULL) {
        child = sweep_tree(ball, child, &sweep);
    }
    sweep_model(ball, object, &sweep);
    if (sweep.count > 0) {
        sweep_take_nearest(ball, &sweep);
        ball->count = sweep.count;
    }
    njPopMatrixEx();
    return sweep.hit;
}

void polygon_find_near_levels(NJS_POINT3 *pos, Float radius, Float *above,
                              Angle *above_x, Angle *above_z, Float *below,
                              Angle *below_x, Angle *below_z)
{
    struct level_probe probe;
    struct world_object *objects;
    Bool found_below;
    Bool found_above;
    int i;
    Float level;

    *above = pos->y + FAR_LEVEL;
    *below = pos->y - FAR_LEVEL;
    found_above = FALSE;
    found_below = FALSE;
    objects = world_objects;
    world_list_near(pos->x, pos->y, pos->z, radius);
    probe.pos.x = pos->x;
    probe.pos.y = pos->y;
    probe.pos.z = pos->z;
    for (i = 0; i < world_count; i++) {
        /* @bug uninitialized: polygon_find_nearest_height adds the probe's
         * radius and speed to its reach, and nothing here sets them (FMOV.S
         * at 0x8c0168a2, 0x8c0168a6, 0x8c0168b2 and 0x8c0168b4) */
        if (polygon_find_nearest_height(&probe, objects[i].object,
                                        objects[i].flags) == TRUE) {
            level = probe.level;
            if (level > pos->y) {
                if (*above > level) {
                    *above = level;
                    if (above_x != NULL) {
                        *above_x = probe.ang_x;
                    }
                    if (above_z != NULL) {
                        *above_z = probe.ang_z;
                    }
                    found_above = TRUE;
                }
            } else if (level > *below) {
                *below = level;
                if (below_x != NULL) {
                    *below_x = probe.ang_x;
                }
                if (below_z != NULL) {
                    *below_z = probe.ang_z;
                }
                found_below = TRUE;
            }
        }
    }
    if (!found_above) {
        *above = pos->y - FAR_LEVEL;
    }
    if (!found_below) {
        *below = pos->y + FAR_LEVEL;
    }
}

void collision_mesh_link_pools(void)
{
    polygon_pool_init(polygon_pool, MESH_POLYGONS);
    collision_mesh_pool_init(mesh_pool, MESHES);
}

void polygon_pool_seek_free(void)
{
    /* @bug memory: nothing points polygon_cursor into polygon_pool, so the
     * first call reads radius2 through null (FMOV.S @(R0,R1),FR2 at
     * 0x8c013b14) */
    while (polygon_cursor_index < MESH_POLYGONS &&
           polygon_cursor->radius2 != 0.0F) {
        polygon_cursor_index++;
        polygon_cursor++;
    }
    if (polygon_cursor_index >= MESH_POLYGONS) {
        polygon_cursor = NULL;
    }
}

/* as strip_next does, for a mesh being made */
static Bool mesh_strip_next(struct strip *strip)
{
    NJS_POINT3 *point;
    int i;
    Bool reversed;

    reversed = FALSE;
    switch (strip->kind) {
    case NJD_MESHSET_3:
    case NJD_MESHSET_4:
        for (i = strip->count, point = strip->points; i > 0; i--, point++) {
            njCalcPoint(NULL, &strip->model->points[*strip->indices], point);
            strip->indices++;
        }
        break;
    case NJD_MESHSET_N:
    case NJD_MESHSET_TRIMESH:
        strip->count = *strip->indices & STRIP_COUNT;
        if (*strip->indices & NJD_TRIMESH_END) {
            reversed = TRUE;
        }
        if (strip->count > STRIP_POINTS) {
            strip->count = STRIP_POINTS;
            if (DEBUG_SHOWN) {
                njPrintC(NJM_LOCATION(WARNING_COLUMN, STRIP_FULL_ROW),
                         "WARNING: STRIP buf is FULL");
            }
        }
        strip->indices++;
        for (i = strip->count, point = strip->points; i > 0; i--, point++) {
            njCalcPoint(NULL, &strip->model->points[*strip->indices], point);
            strip->indices++;
        }
        break;
    }
    return reversed;
}

/* put the triangles of object's own model, through the current matrix, in
 * the new mesh's buckets. returns FALSE when the polygons run out. flags and
 * mesh go unread */
static Bool mesh_add_model(NJS_OBJECT *object, Uint32 flags,
                           struct collision_mesh *mesh)
{
    struct strip strip;
    NJS_MESHSET *meshset;
    NJS_MATERIAL *mats;
    struct polygon *polygon;
    NJS_POINT3 *corner;
    int meshsets;
    int strips;
    Bool reversed;
    int i;
    Float x0;
    Float y0;
    Float z0;
    Float x1;
    Float y1;
    Float z1;
    Float x2;
    Float y2;
    Float z2;
    Float cx;
    Float cy;
    Float cz;
    Float span;
    Float span1;
    Float span2;
    Float radius2;
    Float distance2;
    Float third;

    strip.model = object->model;
    meshsets = strip.model->nbMeshset;
    for (meshset = strip.model->meshsets; meshsets > 0; meshsets--, meshset++) {
        /* @hack spelling: the assignment inside the switch, where a statement
         * of its own would do */
        switch (strip.kind = meshset->type_matId & NJD_MESHSET_MASK) {
        case NJD_MESHSET_3:
            strip.count = 3;
            break;
        case NJD_MESHSET_4:
            strip.count = 4;
            break;
        case NJD_MESHSET_N:
        case NJD_MESHSET_TRIMESH:
            break;
        default:
            continue;
        }
        strips = meshset->nbMesh;
        strip.indices = meshset->meshes;
        for (; strips > 0; strips--) {
            reversed = mesh_strip_next(&strip);
            for (i = 0, corner = strip.points; i < strip.count - 2;
                 i++, corner++) {
                x0 = corner[0].x;
                x1 = corner[1].x;
                x2 = corner[2].x;
                y0 = corner[0].y;
                y1 = corner[1].y;
                y2 = corner[2].y;
                z0 = corner[0].z;
                z1 = corner[1].z;
                z2 = corner[2].z;
                /* @hack spelling: third, a local for CENTER_THIRD, which its
                 * three uses could read in place */
                third = CENTER_THIRD;
                cx = (x0 + x1 + x2) * third;
                cy = (y0 + y1 + y2) * third;
                cz = (z0 + z1 + z2) * third;
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((polygon = polygon_alloc(cz - mesh_center_z)) != NULL) {
                    if (((reversed + i) & 1) == 0) {
                        polygon->vertex[0].x = x0;
                        polygon->vertex[0].y = y0;
                        polygon->vertex[0].z = z0;
                        polygon->vertex[1].x = x1;
                        polygon->vertex[1].y = y1;
                        polygon->vertex[1].z = z1;
                        polygon->vertex[2].x = x2;
                        polygon->vertex[2].y = y2;
                        polygon->vertex[2].z = z2;
                    } else {
                        polygon->vertex[0].x = x0;
                        polygon->vertex[0].y = y0;
                        polygon->vertex[0].z = z0;
                        polygon->vertex[1].x = x2;
                        polygon->vertex[1].y = y2;
                        polygon->vertex[1].z = z2;
                        polygon->vertex[2].x = x1;
                        polygon->vertex[2].y = y1;
                        polygon->vertex[2].z = z1;
                    }
                    span = njAbs(z1 - z0);
                    span1 = njAbs(z2 - z1);
                    span2 = njAbs(z0 - z2);
                    if (span1 > span) {
                        span = span1;
                    }
                    if (span2 > span) {
                        span = span2;
                    }
                    if (span > mesh_span) {
                        mesh_span = span;
                    }
                    x0 -= cx;
                    y0 -= cy;
                    z0 -= cz;
                    x1 -= cx;
                    y1 -= cy;
                    z1 -= cz;
                    x2 -= cx;
                    y2 -= cy;
                    z2 -= cz;
                    polygon->center.x = cx;
                    polygon->center.y = cy;
                    polygon->center.z = cz;
                    radius2 = vector_length2(x0, y0, z0);
                    distance2 = vector_length2(x1, y1, z1);
                    if (distance2 > radius2) {
                        radius2 = distance2;
                    }
                    distance2 = vector_length2(x2, y2, z2);
                    if (distance2 > radius2) {
                        radius2 = distance2;
                    }
                    polygon->radius2 = radius2;
                    mats = object->model->mats;
                    if (mats != NULL) {
                        polygon->attr =
                            mats[meshset->type_matId & ~NJD_MESHSET_MASK]
                                .attrflags;
                    } else {
                        polygon->attr = 0;
                    }
                } else {
                    return FALSE;
                }
            }
        }
    }
    return TRUE;
}

/* add the triangles of the models of object's tree, children first,
 * through its transform. returns its sibling, or null when the polygons run
 * out on its own model */
static NJS_OBJECT *mesh_add_tree(NJS_OBJECT *object, Uint32 flags,
                                 struct collision_mesh *mesh)
{
    NJS_OBJECT *child;

    njPushMatrixEx();
    njTranslateEx((NJS_VECTOR *)object->pos);
    njRotateZ(NULL, object->ang[2]);
    njRotateY(NULL, object->ang[1]);
    njRotateX(NULL, object->ang[0]);
    njScaleEx((NJS_VECTOR *)object->scl);
    child = object->child;
    while (child != NULL) {
        child = mesh_add_tree(child, flags, mesh);
    }
    if (!mesh_add_model(object, flags, mesh)) {
        njPopMatrixEx();
        return NULL;
    }
    njPopMatrixEx();
    return object->sibling;
}

/* clang-format off */
#ifdef DREAMCAST
/* @asm operand: fipr-registers, FIPR FV4,FV4 at 0x8c0140b4 sums the squares
 * of the three differences and FLDI0's zero in FR7 */
#pragma inline_asm(polygon_distance2_fipr)
Float polygon_distance2_fipr(Float *from, Float *to)
{
        FMOV.S      @R4+,FR4
        FMOV.S      @R5+,FR3
        FMOV.S      @R4+,FR5
        FMOV.S      @R5+,FR0
        FMOV.S      @R4,FR6
        FSUB        FR3,FR4
        FMOV.S      @R5,FR3
        FSUB        FR0,FR5
        FSUB        FR3,FR6
        FLDI0       FR7
        FIPR        FV4,FV4
        FMOV.S      FR7,FR0
}
#endif
/* clang-format on */

Float polygon_distance2(Float *from, Float *to)
{
    Float x;
    Float y;
    Float z;
    Float to_x;
    Float to_y;
    Float to_z;

    x = *from++;
    to_x = *to++;
    y = *from++;
    to_y = *to++;
    x = x - to_x;
    z = *from;
    to_z = *to;
    y = y - to_y;
    z = z - to_z;
    return (x * x) + (y * y) + (z * z);
}

int polygon_collect_near(struct polygon_ball *ball)
{
    struct polygon **near;
    struct collision_mesh *mesh;
    struct polygon *polygon;
    struct polygon *last;
    Bool found;
    struct world_object *object;
    int i;
    Float reach;
    Float reach2;
    Float low;
    Float high;
    Float z;
    Float gap;
    Float distance2;
    Float limit;
    Float *center;
    Float *point;

    near_polygon_count = 0;
    near = near_polygons;
    reach = njScalor(&ball->speed) + ball->radius;
    reach2 = reach * reach;
    world_list_near(ball->pos.x, ball->pos.y, ball->pos.z, ball->radius);
    for (mesh = collision_mesh_list; mesh != NULL; mesh = mesh->next) {
        mesh->near = NULL;
        mesh->near_count = 0;
        found = FALSE;
        for (i = world_count, object = world_objects; i > 0; i--, object++) {
            if (object->object == mesh->object) {
                found = TRUE;
                break;
            }
        }
        if (!found) {
            continue;
        }
        polygon = mesh->polygons;
        last = mesh->last;
        z = ball->pos.z;
        low = z - mesh->span - reach;
        high = z + mesh->span + reach;
        /* @bug memory: a mesh that holds no polygon has null polygons and
         * last, and this function reads center.z through both without a test
         * (FMOV.S at 0x8c01417c, 0x8c014186 and 0x8c014334). mesh.src's
         * polygon_find_floor and polygon_find_heights read it so too.
         * polygon_find_sides (TST R3,R3 at 0x8c014eb8), polygon_cast_ray (TST
         * R2,R2 at 0x8c01614a) and the debug display's count (TST R4,R4 at
         * 0x8c01467e) test polygons for null first */
        if (polygon->center.z < low && last->center.z > high) {
            while (polygon->center.z <= low) {
                polygon = polygon->next;
                if (polygon == NULL) {
                    break;
                }
            }
        }
        if (polygon != NULL && polygon->center.z > low) {
            for (; polygon != NULL && polygon->center.z < high;
                 polygon = polygon->next) {
                limit = (reach2 + polygon->radius2) * NEAR_SCALE;
                center = &polygon->center.x;
                point = &ball->pos.x;
                gap = *center++ - *point++;
                /* @hack spelling: the assignments inside this test and the
                 * next two, where statements of their own would do */
                if ((distance2 = gap * gap) > limit) {
                    continue;
                }
                gap = *center++ - *point++;
                if ((distance2 += gap * gap) > limit) {
                    continue;
                }
                gap = *center - *point;
                if ((distance2 += gap * gap) > limit) {
                    continue;
                }
                if (mesh->near == NULL) {
                    mesh->near = near;
                }
                mesh->near_count++;
                *near = polygon;
                near++;
                /* @hack spelling: the increment inside the test, where a
                 * statement of its own would do */
                if (++near_polygon_count == NEAR_POLYGONS) {
                    if (DEBUG_SHOWN) {
                        njPrintC(NJM_LOCATION(WARNING_COLUMN, NEAR_FULL_ROW),
                                 "WARNING: NEAR-poly is FULL");
                    }
                    /* @hack spelling: the goto to the return, where a return
                     * would do */
                    goto end;
                }
                if (near_polygon_count == NEAR_POLYGONS_MANY) {
                    if (DEBUG_SHOWN) {
                        njPrintC(NJM_LOCATION(WARNING_COLUMN, NEAR_MANY_ROW),
                                 "WARNING: NEAR-poly is MANY");
                    }
                }
            }
        } else {
            for (polygon = last; polygon->center.z > low && polygon != NULL;
                 polygon = polygon->prev) {
                limit = (reach2 + polygon->radius2) * NEAR_SCALE;
                center = &polygon->center.x;
                point = &ball->pos.x;
                gap = *center++ - *point++;
                /* @hack spelling: the assignments inside this test and the
                 * next two, where statements of their own would do */
                if ((distance2 = gap * gap) > limit) {
                    continue;
                }
                gap = *center++ - *point++;
                if ((distance2 += gap * gap) > limit) {
                    continue;
                }
                gap = *center - *point;
                if ((distance2 += gap * gap) > limit) {
                    continue;
                }
                if (mesh->near == NULL) {
                    mesh->near = near;
                }
                mesh->near_count++;
                *near = polygon;
                near++;
                /* @hack spelling: the increment inside the test, where a
                 * statement of its own would do */
                if (++near_polygon_count == NEAR_POLYGONS) {
                    if (DEBUG_SHOWN) {
                        njPrintC(NJM_LOCATION(WARNING_COLUMN, NEAR_FULL_ROW),
                                 "WARNING: NEAR-poly is FULL");
                    }
                    /* @hack spelling: the goto to the return, where a return
                     * would do */
                    goto end;
                }
                if (near_polygon_count == NEAR_POLYGONS_MANY) {
                    if (DEBUG_SHOWN) {
                        njPrintC(NJM_LOCATION(WARNING_COLUMN, NEAR_MANY_ROW),
                                 "WARNING: NEAR-poly is MANY");
                    }
                }
            }
        }
    }
end:
    return near_polygon_count;
}

Bool collision_mesh_init(void)
{
    struct collision_mesh *mesh;
    struct polygon *polygon;
    int i;

    near_polygon_count = 0;
    touch_polygon_count = 0;
    collision_mesh_list = NULL;
    mesh_stale_object = NULL;
    for (i = MESHES, mesh = mesh_pool; i > 0; i--, mesh++) {
        mesh->object = NULL;
    }
    for (i = MESH_POLYGONS, polygon = polygon_pool; i > 0; i--, polygon++) {
        polygon->radius2 = 0.0F;
    }
    polygon_pool_init(polygon_pool, MESH_POLYGONS);
    collision_mesh_pool_init(mesh_pool, MESHES);
    return TRUE;
}

Bool collision_mesh_update(void)
{
    NJS_OBJECT *objects[MESHES];
    Uint32 flags[MESHES];
    NJS_POINT3 center;
    NJS_OBJECT **object;
    Uint32 *flag;
    struct world_object *entry;
    struct collision_mesh *mesh;
    struct collision_mesh *next;
    struct polygon *polygon;
    NJS_OBJECT *root;
    NJS_MATRIX *matrix;
    int count;
    Bool found;
    int wanted;
    int i;
    int free;
    int most;
    int total;
    int used;

    world_collect_near_players();
    /* @hack spelling: the block as a loop of one pass, which a break leaves
     * once the list is full, where an if around the loop that frees meshes
     * would do */
    do {
        wanted = 0;
        object = objects;
        flag = flags;
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((count = world_count) > MESHES) {
            count = MESHES;
        }
        for (entry = world_objects; count > 0; count--, entry++) {
            if (entry->flags & WORLD_MOVING) {
                *object = entry->object;
                object++;
                wanted++;
                *flag = entry->flags;
                flag++;
            } else if (mesh_stale_object == entry->object) {
                wanted++;
                mesh_stale_object = NULL;
                *object = entry->object;
                object++;
                *flag = entry->flags;
                flag++;
            } else {
                found = FALSE;
                for (mesh = collision_mesh_list; mesh != NULL;
                     mesh = mesh->next) {
                    if (mesh->object == entry->object) {
                        found = TRUE;
                        break;
                    }
                }
                if (found) {
                    continue;
                }
                wanted++;
                *object++ = entry->object;
                *flag = entry->flags;
                flag++;
                if (wanted == MESHES) {
                    break;
                }
            }
        }
        if (wanted == MESHES) {
            break;
        }
        for (mesh = collision_mesh_list; mesh != NULL; mesh = next) {
            found = FALSE;
            for (i = world_count, entry = world_objects; i > 0; i--, entry++) {
                if (mesh->object == entry->object) {
                    if ((entry->flags & WORLD_MOVING) ||
                        mesh_stale_object == entry->object) {
                        found = FALSE;
                    } else {
                        found = TRUE;
                    }
                    break;
                }
            }
            next = mesh->next;
            if (!found) {
                collision_mesh_free(mesh);
            }
        }
    } while (0);
    njPushMatrix(&_nj_unit_matrix_);
    flag = flags;
    /* @hack spelling: the loop as a do inside an if, which counts i down
     * from wanted, where a for loop would do */
    i = wanted;
    object = objects;
    /* @hack spelling: the unit matrix's address in matrix, where
     * &_nj_unit_matrix_ at the call would do */
    matrix = &_nj_unit_matrix_;
    if (i > 0) {
        do {
            mesh = collision_mesh_alloc();
            /* @bug memory: the stores below write through null once the pool
             * is out of meshes (FMOV.S FR3,@(R0,R13) at 0x8c0145f6 on), as
             * nothing tests what collision_mesh_alloc returns. the pool lends
             * 191 of its MESHES, since collision_mesh_alloc returns null rather
             * than take the free list's last record (TST R3,R3 at
             * 0x8c017cfa) */
#ifdef NONMATCHING
            /* this side repairs the tag above: it makes no more meshes once
             * the pool is out */
            if (mesh == NULL) {
                break;
            }
#endif
            collision_mesh_clear_buckets();
            njPushMatrix(matrix);
            root = *object;
            njTranslateEx((NJS_VECTOR *)root->pos);
            njRotateZ(NULL, root->ang[2]);
            njRotateY(NULL, root->ang[1]);
            njRotateX(NULL, root->ang[0]);
            njScaleEx((NJS_VECTOR *)root->scl);
            center.x = root->model->center.x;
            center.y = root->model->center.y;
            center.z = root->model->center.z;
            njCalcPoint(NULL, &center, &center);
            mesh_center_z = center.z;
            mesh->center.x = center.x;
            mesh->center.y = center.y;
            mesh->center.z = center.z;
            njPopMatrixEx();
            mesh_span = 0.0F;
            dat_8c249f04 = 0.0F;
            mesh->object = *object;
            mesh->flags = *flag;
            mesh->polygons = NULL;
            mesh->unk_18 = 0;
            mesh->near = NULL;
            mesh->near_count = 0;
            mesh_add_tree(*object++, *flag, mesh);
            collision_mesh_take_buckets(mesh);
            mesh->span = mesh_span;
            mesh->unk_2c = dat_8c249f04;
            flag++;
        } while (--i > 0);
    }
    njPopMatrixEx();
    if (DEBUG_SHOWN) {
        njPrintC(NJM_LOCATION(TITLE_COLUMN, POLYGONS_ROW), "colbuf:");
        most = 0;
        total = 0;
        /* @hack spelling: free set from most, where 0 would do */
        for (i = MESHES, mesh = mesh_pool, free = most; i > 0; i--, mesh++) {
            if (mesh->object == NULL) {
                free++;
            } else {
                count = 0;
                for (polygon = mesh->polygons; polygon;
                     polygon = polygon->next) {
                    count++;
                }
                if (most < count) {
                    most = count;
                }
                total += count;
            }
        }
        used = 100 - (free * 100 / MESHES);
        njPrintC(NJM_LOCATION(LABEL_COLUMN, MESHES_ROW), "obj:");
        if (used >= USED_WARNING) {
            njPrintColor(COLOR_WARNING);
            njPrintD(NJM_LOCATION(PERCENT_COLUMN, MESHES_ROW), used,
                     PERCENT_DIGITS);
            njPrintC(NJM_LOCATION(PERCENT_SIGN_COLUMN, MESHES_ROW), "%");
            njPrintColor(COLOR_TEXT);
        } else {
            njPrintD(NJM_LOCATION(PERCENT_COLUMN, MESHES_ROW), used,
                     PERCENT_DIGITS);
            njPrintC(NJM_LOCATION(PERCENT_SIGN_COLUMN, MESHES_ROW), "%");
        }
        njPrintD(NJM_LOCATION(COUNT_COLUMN, MESHES_ROW), MESHES - free,
                 COUNT_DIGITS);
        /* @bug logic: the meshes' warning never shows, as free is never 0
         * (TST R11,R11 at 0x8c014798). collision_mesh_alloc never lends the
         * free list's last record (TST R3,R3 at 0x8c017cfa). that record's
         * object stays null, so free counts it */
        if (free == 0 && DEBUG_SHOWN) {
            njPrintC(NJM_LOCATION(WARNING_COLUMN, MESHES_FULL_ROW),
                     "WARNING: OBJECT buf is FULL");
        }
        njPrintC(NJM_LOCATION(MOST_LABEL_COLUMN, MESHES_ROW), "polMAX:");
        njPrintD(NJM_LOCATION(MOST_COLUMN, MESHES_ROW), most, MOST_DIGITS);
        /* @hack spelling: the percent in two statements, where one would
         * do */
        used = total * 100;
        used /= MESH_POLYGONS;
        njPrintC(NJM_LOCATION(LABEL_COLUMN, POLYGONS_ROW), "r-p:");
        if (used >= USED_WARNING) {
            njPrintColor(COLOR_WARNING);
            njPrintD(NJM_LOCATION(PERCENT_COLUMN, POLYGONS_ROW), used,
                     PERCENT_DIGITS);
            njPrintC(NJM_LOCATION(PERCENT_SIGN_COLUMN, POLYGONS_ROW), "%");
            njPrintColor(COLOR_TEXT);
        } else {
            njPrintD(NJM_LOCATION(PERCENT_COLUMN, POLYGONS_ROW), used,
                     PERCENT_DIGITS);
            njPrintC(NJM_LOCATION(PERCENT_SIGN_COLUMN, POLYGONS_ROW), "%");
        }
        njPrintD(NJM_LOCATION(COUNT_COLUMN, POLYGONS_ROW), total, COUNT_DIGITS);
        /* @bug logic: the polygons' warning tests free, the count of free
         * meshes, as the meshes' warning does (TST R11,R11 at 0x8c014816).
         * free is never 0, so this warning never shows either */
        if (free == 0 && DEBUG_SHOWN) {
            njPrintC(NJM_LOCATION(WARNING_COLUMN, POLYGONS_FULL_ROW),
                     "WARNING: RANGE-poly buf is FULL");
        }
    }
    return TRUE;
}

Bool polygon_find_sides(struct surface_probe *probe, NJS_OBJECT *object,
                        Uint32 flags)
{
    NJS_POINT3 *p0;
    NJS_POINT3 *p1;
    NJS_POINT3 *p2;
    struct surface_side *side;
    struct collision_mesh *mesh;
    struct polygon *polygon;
    NJS_VECTOR normal;
    Float e1;
    Float e2;
    Float e3;
    Float plane;
    Float x;
    Float y;
    Float z;
    Float nx;
    Sint32 mesh_flags;
    Bool found;
    Bool any;
    int i;

    any = FALSE;
    for (i = SURFACE_SIDES, side = probe->sides; i > 0; i--, side++) {
        side->found = FALSE;
    }
    probe->sides[SURFACE_NEG_X].pos = probe->pos.x + REACH;
    probe->sides[SURFACE_POS_X].pos = probe->pos.x - REACH;
    probe->sides[SURFACE_NEG_Y].pos = probe->pos.y + REACH;
    probe->sides[SURFACE_POS_Y].pos = probe->pos.y - REACH;
    probe->sides[SURFACE_NEG_Z].pos = probe->pos.z + REACH;
    probe->sides[SURFACE_POS_Z].pos = probe->pos.z - REACH;
    found = FALSE;
    for (mesh = collision_mesh_list; mesh != NULL; mesh = mesh->next) {
        if (mesh->object == object) {
            found = TRUE;
            break;
        }
    }
    if (!found) {
        return FALSE;
    }
    /* @hack redundant: a test of mesh that never runs */
    if (0) {
        if (mesh) {
        }
    }
    polygon = mesh->polygons;
    mesh_flags = mesh->flags;
    for (; polygon != NULL; polygon = polygon->next) {
        if (((polygon->vertex[0].x - probe->pos.x) *
             (polygon->vertex[0].x - probe->pos.x)) +
                ((polygon->vertex[0].y - probe->pos.y) *
                 (polygon->vertex[0].y - probe->pos.y)) +
                ((polygon->vertex[0].z - probe->pos.z) *
                 (polygon->vertex[0].z - probe->pos.z)) >
            REACH * REACH) {
            continue;
        }
        p0 = &polygon->vertex[0];
        p1 = &polygon->vertex[1];
        p2 = &polygon->vertex[2];

        do {
            /* @hack redundant: a call that never runs */
            if (0) {
                njPopMatrixEx();
            }
            e1 = EDGE_SIDE(p0, p1, &probe->pos, x, y);
            e2 = EDGE_SIDE(p1, p2, &probe->pos, x, y);
            e3 = EDGE_SIDE(p2, p0, &probe->pos, x, y);
            if (e1 * e2 < 0.0F || e2 * e3 < 0.0F || e1 * e3 < 0.0F) {
                break;
            }
            /* @hack redundant: a test of any that never runs */
            if (0) {
                if (any) {
                }
            }
            triangle_normal(&p0->x, &p1->x, &p2->x, &normal.x);
            /* @hack spelling: nx set inside the product, where a statement of
             * its own would do */
            plane = (-(nx = normal.x) * p0->x) - (normal.y * p0->y) -
                    (normal.z * p0->z);
            if (normal.z * normal.z < NO_PART2) {
                break;
            }
            z = (-(nx * probe->pos.x) - (normal.y * probe->pos.y) - plane) /
                normal.z;
            side = NULL;
            if (z < probe->pos.z) {
                if (!probe->sides[SURFACE_NEG_Z].found ||
                    z > probe->sides[SURFACE_NEG_Z].pos) {
                    side = &probe->sides[SURFACE_NEG_Z];
                }
            } else {
                if (!probe->sides[SURFACE_POS_Z].found ||
                    z < probe->sides[SURFACE_POS_Z].pos) {
                    side = &probe->sides[SURFACE_POS_Z];
                }
            }
            if (side == NULL) {
                break;
            }
            UNIT_NORMAL(&normal);
            any = TRUE;
            side->found = TRUE;
            side->flags = mesh_flags;
            side->pos = z;
            side->normal = normal;
            side->ang_z = NORMAL_ANG_Z(normal);
            side->ang_x = NORMAL_ANG_X(normal);
        } while (0);

        do {
            /* @hack redundant: a call that never runs */
            if (0) {
                njPopMatrixEx();
            }
            e1 = EDGE_SIDE(p0, p1, &probe->pos, y, z);
            e2 = EDGE_SIDE(p1, p2, &probe->pos, y, z);
            e3 = EDGE_SIDE(p2, p0, &probe->pos, y, z);
            if (e1 * e2 < 0.0F || e2 * e3 < 0.0F || e1 * e3 < 0.0F) {
                break;
            }
            triangle_normal(&p0->x, &p1->x, &p2->x, &normal.x);
            /* @hack spelling: nx set inside the product, where a statement of
             * its own would do */
            plane = (-(nx = normal.x) * p0->x) - (normal.y * p0->y) -
                    (normal.z * p0->z);
            if (nx * normal.x < NO_PART2) {
                break;
            }
            x = (-(normal.y * probe->pos.y) - (normal.z * probe->pos.z) -
                 plane) /
                nx;
            side = NULL;
            if (x < probe->pos.x) {
                if (!probe->sides[SURFACE_NEG_X].found ||
                    x > probe->sides[SURFACE_NEG_X].pos) {
                    side = &probe->sides[SURFACE_NEG_X];
                }
            } else {
                if (!probe->sides[SURFACE_POS_X].found ||
                    x < probe->sides[SURFACE_POS_X].pos) {
                    side = &probe->sides[SURFACE_POS_X];
                }
            }
            if (side == NULL) {
                break;
            }
            UNIT_NORMAL(&normal);
            any = TRUE;
            side->found = TRUE;
            side->flags = mesh_flags;
            side->pos = x;
            side->normal = normal;
            side->ang_z = NORMAL_ANG_Z(normal);
            side->ang_x = NORMAL_ANG_X(normal);
        } while (0);

        do {
            /* @hack redundant: a call that never runs */
            if (0) {
                njPopMatrixEx();
            }
            e1 = EDGE_SIDE(p0, p1, &probe->pos, z, x);
            e2 = EDGE_SIDE(p1, p2, &probe->pos, z, x);
            e3 = EDGE_SIDE(p2, p0, &probe->pos, z, x);
            if (e1 * e2 < 0.0F || e2 * e3 < 0.0F || e1 * e3 < 0.0F) {
                break;
            }
            triangle_normal(&p0->x, &p1->x, &p2->x, &normal.x);
            /* @hack spelling: nx set inside the product, where a statement of
             * its own would do */
            plane = (-(nx = normal.x) * p0->x) - (normal.y * p0->y) -
                    (normal.z * p0->z);
            if (normal.y * normal.y < NO_PART2) {
                break;
            }
            y = (-(normal.z * probe->pos.z) - (nx * probe->pos.x) - plane) /
                normal.y;
            side = NULL;
            if (y < probe->pos.y) {
                if (!probe->sides[SURFACE_NEG_Y].found ||
                    y > probe->sides[SURFACE_NEG_Y].pos) {
                    side = &probe->sides[SURFACE_NEG_Y];
                }
            } else {
                if (!probe->sides[SURFACE_POS_Y].found ||
                    y < probe->sides[SURFACE_POS_Y].pos) {
                    side = &probe->sides[SURFACE_POS_Y];
                }
            }
            if (side == NULL) {
                break;
            }
            UNIT_NORMAL(&normal);
            any = TRUE;
            side->found = TRUE;
            side->flags = mesh_flags;
            side->pos = y;
            side->normal = normal;
            side->ang_z = NORMAL_ANG_Z(normal);
            side->ang_x = NORMAL_ANG_X(normal);
        } while (0);
    }
    /* @hack redundant: a store of 1.0F that never runs */
    if (0) {
        normal.y = 1.0F;
    }
    /* @hack redundant: a test of p0 that never runs */
    if (0) {
        if (p0) {
        }
    }
    /* @hack redundant: a test of p1 that never runs */
    if (0) {
        if (p1) {
        }
    }
    return any;
}

Bool polygon_sweep(struct polygon_ball *ball, NJS_OBJECT *object, Uint32 flags,
                   Bool loose)
{
    struct collision_mesh *mesh;
    NJS_OBJECT *sought;
    struct polygon **near;
    struct polygon *poly;
    struct touch_polygon *touch;
    struct touch_polygon *touched;
    struct polygon_contact *hit_contact;
    struct polygon_contact *listed;
    NJS_POINT3 *corners;
    Bool found;
    int count;
    Bool hit;
    Bool more;
    Bool inside;
    Uint32 two_sided;
    int nearest;
    int i;
    Angle ang_x;
    Angle ang_y;
    Float length;
    Float spacing;
    Float travelled;
    Float radius;
    Float height;
    Float scale;
    Float reach2;
    Float far;
    Float e1;
    Float e2;
    Float e3;
    NJS_POINT3 tri[3];
    NJS_POINT3 pos;
    NJS_VECTOR speed;
    NJS_VECTOR normal;
    NJS_VECTOR stride;
    NJS_POINT3 at;

    if (!(ball->radius > 0.0F)) {
        return FALSE;
    }
    touch_polygon_count = 0;
    length = njScalor(&ball->speed);
    spacing = ball->radius * SWEEP_STEP;
    touch = touch_polygons;
    found = FALSE;
    /* @hack spelling: sought, a copy of object for the search to compare,
     * where object itself would do */
    sought = object;
    for (mesh = collision_mesh_list; mesh != NULL; mesh = mesh->next) {
        if (mesh->object == sought) {
            found = TRUE;
            break;
        }
    }
    if (!found) {
        return FALSE;
    }
    /* @hack redundant: an unreachable test of the mesh */
    if (0) {
        if (mesh) {
        }
    }
    /* @hack redundant: a store to touch that never runs */
    if (0) {
        touch = (struct touch_polygon *)mesh->near;
    }
    for (count = mesh->near_count, near = mesh->near,
        corners = &polygon_contact_corners[0][0];
         count > 0; count--, near++) {
        Float *v0;
        Float *v1;
        Float *v2;
        Float *edge;
        NJS_POINT3 *vertex;
        Float x0;
        Float y0;
        Float z0;
        Float x;
        Float y;
        Float z;

        polygon_sweep_tests++;
        tri[0].x = tri[0].y = tri[0].z = 0.0F;
        /* @hack spelling: poly assigned inside vertex's assignment, where a
         * statement of its own would do */
        vertex = &(poly = *near)->vertex[0];
        /* @hack redundant: the address of the current polygon pointer */
        (void)&poly;
        /* @hack spelling: Float cursors stepped across the three vertices'
         * members, where vertex[0] to vertex[2]'s members would do */
        v0 = &vertex->x;
        v1 = &vertex[1].x;
        v2 = &vertex[2].x;
        x0 = *v0++;
        y0 = *v0++;
        x = *v1++ - x0;
        y = *v1++ - y0;
        z0 = *v0;
        /* @hack spelling: tri[1] written through a Float cursor stepped down
         * from past its z, where its members would do */
        edge = &tri[1].x + 3;
        z = *v1 - z0;
        *--edge = z;
        *--edge = y;
        *--edge = x;
        /* @hack spelling: x's subtraction after z's, where x = *v2++ - x0
         * would do */
        x = *v2++;
        y = *v2++ - y0;
        z = *v2 - z0;
        x -= x0;
        /* @hack spelling: tri[2] written the same way */
        edge = &tri[2].x + 3;
        *--edge = z;
        *--edge = y;
        *--edge = x;
        pos.x = ball->pos.x - x0;
        pos.y = ball->pos.y - y0;
        pos.z = ball->pos.z - z0;
        speed = ball->speed;
        njPushMatrix(&_nj_unit_matrix_);
        /* @hack type: the points cast to Float *, where &tri[0].x and the
         * rest would do */
        triangle_normal((Float *)&tri[0], (Float *)&tri[1], (Float *)&tri[2],
                        (Float *)&normal);
        if (njScalor(&normal) == 0.0F) {
            /* @hack redundant: the stores in a loop of one pass */
            do {
                (&normal)->x = 0.0F;
                (&normal)->y = 1.0F;
                (&normal)->z = 0.0F;
            } while (0);
        } else {
            njUnitVector(&normal);
        }
        if (normal.y > UPRIGHT_NORMAL) {
            ang_x = 0;
            ang_y = 0;
        } else if (normal.y < -UPRIGHT_NORMAL) {
            ang_x = -HALF_TURN;
            ang_y = -HALF_TURN;
        } else {
            ang_x = -(Angle)(acosf(normal.y) * ANGLE_PER_RADIAN);
            ang_y = -(Angle)(atan2f(normal.x, normal.z) * ANGLE_PER_RADIAN);
        }
        njRotateX(NULL, ang_x);
        njRotateY(NULL, ang_y);
        njCalcPoint(NULL, &tri[1], &tri[1]);
        njCalcPoint(NULL, &tri[2], &tri[2]);
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(NULL, &speed, &speed);
        radius = ball->radius;
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((two_sided = (*near)->attr & NJD_FLAG_DOUBLE_SIDE) != 0) {
            if (pos.y < 0.0F) {
                radius *= -1.0F;
            }
        }
        hit = FALSE;
        stride = speed;
        UNIT_VECTOR(&stride);
        stride.x *= spacing;
        stride.y *= spacing;
        stride.z *= spacing;
        at = pos;
        travelled = 0.0F;
        for (;;) {
            more = TRUE;
            if (!(travelled < length)) {
                /* @hack redundant: a copy of pos into at, which the sums
                 * below overwrite */
                at = pos;
                at.x = pos.x + speed.x;
                at.y = pos.y + speed.y;
                at.z = pos.z + speed.z;
                more = FALSE;
                polygon_sweep_ends++;
            }
            if (!(njAbs(at.y) > ball->radius)) {
                inside = FALSE;
                if (loose) {
                    NJS_POINT3 middle;
                    NJS_VECTOR away;
                    NJS_VECTOR to[3];

                    vector_sum(&tri[0], &tri[1], &middle);
                    vector_sum(&tri[2], &middle, &middle);
                    middle.x *= MIDDLE_THIRD;
                    middle.y *= MIDDLE_THIRD;
                    middle.z *= MIDDLE_THIRD;
                    vector_diff(&tri[0], &middle, &away);
                    reach2 = njScalor2(&away);
                    vector_diff(&tri[1], &middle, &away);
                    /* @hack spelling: far set apart from its test, where
                     * the call in the test would do */
                    far = njScalor2(&away);
                    if (far > reach2) {
                        reach2 = far;
                    }
                    vector_diff(&tri[2], &middle, &away);
                    far = njScalor2(&away);
                    if (far > reach2) {
                        reach2 = far;
                    }
                    vector_diff(&tri[0], &at, &to[0]);
                    vector_diff(&tri[1], &at, &to[1]);
                    vector_diff(&tri[2], &at, &to[2]);
                    if (reach2 > njScalor2(&to[0]) ||
                        reach2 > njScalor2(&to[1]) ||
                        reach2 > njScalor2(&to[2])) {
                        inside = TRUE;
                    }
                } else {
                    e1 = EDGE_SIDE(&tri[0], &tri[1], &at, z, x);
                    e2 = EDGE_SIDE(&tri[1], &tri[2], &at, z, x);
                    e3 = EDGE_SIDE(&tri[2], &tri[0], &at, z, x);
                    if (!(e1 * e2 < 0.0F) && !(e2 * e3 < 0.0F) &&
                        !(e1 * e3 < 0.0F)) {
                        inside = TRUE;
                    }
                }
                if (inside) {
                    hit = TRUE;
                    polygon_sweep_touches++;
                    break;
                }
            }
            if (!more) {
                break;
            }
            at.x = at.x + stride.x;
            at.y = at.y + stride.y;
            at.z = at.z + stride.z;
            travelled += spacing;
        }
        if (hit) {
            if (DEBUG_SHOWN) {
                touch_points[0] = (*near)->vertex[0];
                touch_points[1] = (*near)->vertex[1];
                touch_points[2] = (*near)->vertex[2];
                touch_points[0].x += normal.x;
                touch_points[1].x += normal.x;
                touch_points[2].x += normal.x;
                touch_points[0].y += normal.y;
                touch_points[1].y += normal.y;
                touch_points[2].y += normal.y;
                touch_points[0].z += normal.z;
                touch_points[1].z += normal.z;
                touch_points[2].z += normal.z;
                njGetMatrix(&touch_matrix);
                njPopMatrixEx();
                /* @bug mismatched-call: the count is 3 where touch_points
                 * holds one triangle. njDrawTriangle3D takes a count of
                 * triangles, three points each where attr's bits 0 and 1
                 * are clear, and so reads six points past touch_points' end
                 * (JSR at 0x8c015640, MOV #3,R5 at 0x8c015642) */
                njDrawTriangle3D(&touch_triangle, 3, NJD_FILL);
                njPushMatrix(&touch_matrix);
            }
            /* @hack spelling: the corners copied through a cursor of their
             * own in a bare block, where corners itself would do */
            {
                NJS_POINT3 *copy = corners;

                copy[0] = (*near)->vertex[0];
                copy[1] = (*near)->vertex[1];
                copy[2] = (*near)->vertex[2];
            }
            corners += 3;
            {
                struct polygon_contact contact;
                NJS_VECTOR part;

                contact.height = pos.y;
                contact.height_after = pos.y + speed.y;
                contact.hit.front = normal;
                /* @hack spelling: the normal chosen in a loop of one pass,
                 * which a break leaves after the turned copy, where an if
                 * and an else would do */
                do {
                    if (two_sided) {
                        if (pos.y < 0.0F) {
                            contact.hit.normal.x = -normal.x;
                            contact.hit.normal.y = -normal.y;
                            contact.hit.normal.z = -normal.z;
                            break;
                        }
                    }
                    contact.hit.normal = normal;
                } while (0);
                contact.hit.ang_z = NORMAL_ANG_Z(contact.hit.normal);
                contact.hit.ang_x = NORMAL_ANG_X(contact.hit.normal);
                njInvertMatrix(NULL);
                stride.x = pos.x;
                stride.y = 0.0F;
                stride.z = pos.z;
                njCalcPoint(NULL, &stride, &at);
                contact.hit.point.x = at.x + (*near)->vertex[0].x;
                contact.hit.point.y = at.y + (*near)->vertex[0].y;
                contact.hit.point.z = at.z + (*near)->vertex[0].z;
                stride.y = radius;
                njCalcPoint(NULL, &stride, &at);
                contact.hit.pos.x = at.x + (*near)->vertex[0].x;
                contact.hit.pos.y = at.y + (*near)->vertex[0].y;
                contact.hit.pos.z = at.z + (*near)->vertex[0].z;
                part = speed;
                /* @hack redundant: the negation of part.y, which the store of
                 * 0.0F below overwrites */
                if (two_sided) {
                    if (pos.y * speed.y < 0.0F) {
                        part.y = -speed.y;
                    }
                } else if (radius * speed.y < 0.0F) {
                    part.y = -speed.y;
                }
                part.x = speed.x;
                part.y = 0.0F;
                part.z = speed.z;
                njCalcPoint(NULL, &part, &at);
                contact.hit.speed = at;
                part.x = 0.0F;
                part.y = speed.y;
                part.z = 0.0F;
                njCalcPoint(NULL, &part, &at);
                contact.hit.normal_speed = at;
                touch->polygon = **near;
                touch->contact = contact;
            }
            touch++;
            /* @hack spelling: the increment inside the test, where a
             * statement of its own would do */
            if (++touch_polygon_count == POLYGON_CONTACTS) {
                if (DEBUG_SHOWN) {
                    njPrintC(NJM_LOCATION(WARNING_COLUMN, TOUCHES_FULL_ROW),
                             "WARNING: TOUCH-poly buf is FULL");
                }
                /* @bug logic: the break leaves this triangle's matrix
                 * pushed. at the touch that fills the list (CMP/EQ R1,R3 at
                 * 0x8c01593e), BF at 0x8c015948 and BRA at 0x8c015954 leave
                 * the loop past njPopMatrixEx's JSR at 0x8c01595a */
                break;
            }
        }
        njPopMatrixEx();
    }
    count = touch_polygon_count;
    if (count < 1) {
        return FALSE;
    }
    /* @hack redundant: an unreachable test of the contact count */
    if (0) {
        if (count) {
        }
    }
    nearest = 0;
    height = touch_polygons[0].contact.height_after;
    /* @hack spelling: touched set in the loop's header after i, where
     * a separate initialization would do. i advances by assignment */
    for (i = 1, touched = &touch_polygons[1]; i < count; i = i + 1, touched++) {
        /* @hack redundant: unreachable tests of the count, index and contact
         * list */
        if (0) {
            if (count) {
            }
            if (i) {
            }
            if (&touch_polygons[0]) {
            }
        }
        if (height > touched->contact.height_after) {
            height = touched->contact.height_after;
            nearest = i;
        }
    }
    /* @hack redundant: unreachable tests of the index and contact cursor */
    if (0) {
        if (i) {
        }
        if (touched) {
        }
    }
    /* @hack spelling: subtract from i and add to listed by assignment,
     * where a decrement and a pointer increment would do */
    for (i = count, touched = touch_polygons, listed = polygon_contacts; i > 0;
         i = i - 1, touched++, listed = listed + 1) {
        *listed = touched->contact;
    }
    /* @hack redundant: an unreachable test of the nearest contact index */
    if (0) {
        if (nearest) {
        }
    }
    ball->count = touch_polygon_count;
    ball->hit_index = nearest;
    hit_contact = &touch_polygons[nearest].contact;
    ball->hit.ang_x = hit_contact->hit.ang_x;
    ball->hit.ang_z = hit_contact->hit.ang_z;
    ball->hit.front = hit_contact->hit.front;
    ball->hit.normal = hit_contact->hit.normal;
    ball->hit.speed = hit_contact->hit.speed;
    ball->hit.normal_speed = hit_contact->hit.normal_speed;
    if (touch_polygon_count == 1) {
        ball->hit.point = hit_contact->hit.point;
        ball->hit.pos = hit_contact->hit.pos;
    } else {
        touched = touch_polygons;
        ball->hit.point = touched->contact.hit.point;
        ball->hit.pos = touched->contact.hit.pos;
        touched++;
        for (i = touch_polygon_count - 1; i > 0; i--, touched++) {
            /* @hack redundant: an unreachable test of the contact cursor */
            if (0) {
                if (touched) {
                }
            }
            ball->hit.point.x += touched->contact.hit.point.x;
            ball->hit.point.y += touched->contact.hit.point.y;
            ball->hit.point.z += touched->contact.hit.point.z;
            ball->hit.pos.x += touched->contact.hit.pos.x;
            ball->hit.pos.y += touched->contact.hit.pos.y;
            ball->hit.pos.z += touched->contact.hit.pos.z;
        }
        scale = 1.0F / touch_polygon_count;
        ball->hit.point.x *= scale;
        ball->hit.point.y *= scale;
        ball->hit.point.z *= scale;
        ball->hit.pos.x *= scale;
        ball->hit.pos.y *= scale;
        ball->hit.pos.z *= scale;
    }
    return TRUE;
}

Bool polygon_cast_ray(struct ray_query *query)
{
    int count;
    Bool met;
    Float distance;
    struct world_object *object;
    struct collision_mesh *mesh;
    struct polygon *polygon;
    NJS_POINT3 *v0;
    NJS_POINT3 *v1;
    NJS_POINT3 *v2;
    Bool found;
    Float reach2;
    Float gap;
    Float e1;
    Float e2;
    Float e3;

    met = FALSE;
    query->surface.found = FALSE;
    if (query->length < RAY_LENGTH_MIN || query->length > RAY_LENGTH) {
        query->length = RAY_LENGTH;
    }
    reach2 = query->length + REACH;
    reach2 *= reach2;
    world_list_near(query->origin.x, query->origin.y, query->origin.z,
                    query->length);
    for (count = world_count, object = world_objects; count > 0;
         count--, object++) {
        found = FALSE;
        for (mesh = collision_mesh_list; mesh != NULL; mesh = mesh->next) {
            if (mesh->object == object->object) {
                found = TRUE;
                break;
            }
        }
        if (!found) {
            continue;
        }
        for (polygon = mesh->polygons; polygon != NULL;
             polygon = polygon->next) {
            /* @hack spelling: v0 and v2 both set from the vertices, and v2
             * stepped to the last, where &polygon->vertex[n] for each would
             * do */
            v0 = v2 = polygon->vertex;
            v1 = v0 + 1;
            v2 += 2;
            /* @hack spelling: to0 to to2 in a bare block, and tri and the
             * rest in the next, where the function's own locals would do */
            {
                NJS_VECTOR to0;
                NJS_VECTOR to1;
                NJS_VECTOR to2;

                to0.x = v0->x - query->origin.x;
                to0.y = v0->y - query->origin.y;
                to0.z = v0->z - query->origin.z;
                to1.x = v1->x - query->origin.x;
                to1.y = v1->y - query->origin.y;
                to1.z = v1->z - query->origin.z;
                to2.x = v2->x - query->origin.x;
                to2.y = v2->y - query->origin.y;
                to2.z = v2->z - query->origin.z;
                if (DOT(&to0, &query->dir) < 0.0F &&
                    DOT(&to1, &query->dir) < 0.0F &&
                    DOT(&to2, &query->dir) < 0.0F) {
                    continue;
                }
            }
            {
                NJS_POINT3 tri[3];
                NJS_POINT3 *edge;
                NJS_POINT3 origin;
                NJS_VECTOR dir;
                NJS_VECTOR normal;
                NJS_POINT3 at;
                NJS_POINT3 out;

                /* @hack spelling: edge, a second pointer to tri[1], for
                 * the normal's call, where &tri[1] would do */
                edge = &tri[1];
                tri[0].x = tri[0].y = tri[0].z = 0.0F;
                tri[1] = *v1;
                tri[2] = *v2;
                tri[1].x -= v0->x;
                tri[2].x -= v0->x;
                tri[1].y -= v0->y;
                tri[2].y -= v0->y;
                tri[1].z -= v0->z;
                tri[2].z -= v0->z;
                origin.x = query->origin.x - v0->x;
                origin.y = query->origin.y - v0->y;
                origin.z = query->origin.z - v0->z;
                dir = query->dir;
                /* @hack type: the points cast to Float *, where &tri[0].x
                 * and the rest would do */
                triangle_normal((Float *)&tri[0], (Float *)edge,
                                (Float *)&tri[2], (Float *)&normal);
                if (!(polygon->attr & NJD_FLAG_DOUBLE_SIDE) &&
                    DOT(&normal, &dir) > -GRAZING) {
                    continue;
                }
                /* @hack redundant: an else after the continue */
                else if (njScalor(&normal) == 0.0F) {
                    normal.x = 0.0F;
                    normal.y = 1.0F;
                    normal.z = 0.0F;
                } else {
                    njUnitVector(&normal);
                }
                gap = (normal.x * origin.x) + (normal.y * origin.y) +
                      (normal.z * origin.z);
                gap *= gap;
                if (gap > reach2) {
                    continue;
                }
                njPushMatrix(&_nj_unit_matrix_);
                njRotateX(NULL, -(Angle)(acosf(normal.y) * ANGLE_PER_RADIAN));
                njRotateY(NULL, -(Angle)(atan2f(normal.x, normal.z) *
                                         ANGLE_PER_RADIAN));
                njCalcPoint(NULL, &tri[1], &at);
                /* @hack spelling: the block as a loop of one pass, which its
                 * breaks leave, where a goto to the njPopMatrixEx would do */
                do {
                    tri[1] = at;
                    njCalcPoint(NULL, &tri[2], &at);
                    tri[2] = at;
                    njCalcPoint(NULL, &origin, &at);
                    origin = at;
                    njCalcPoint(NULL, &dir, &at);
                    dir = at;
                    if (origin.y >= 0.0F) {
                        if (dir.y > -GRAZING) {
                            break;
                        }
                    } else {
                        if (dir.y < GRAZING) {
                            break;
                        }
                    }
                    at.x = -(dir.x * origin.y / dir.y) + origin.x;
                    at.z = -(dir.z * origin.y / dir.y) + origin.z;
                    at.y = 0.0F;
                    e1 = EDGE_SIDE(&tri[0], &tri[1], &at, z, x);
                    e2 = EDGE_SIDE(&tri[1], &tri[2], &at, z, x);
                    e3 = EDGE_SIDE(&tri[2], &tri[0], &at, z, x);
                    if (e1 * e2 < 0.0F || e2 * e3 < 0.0F || e1 * e3 < 0.0F) {
                        break;
                    }
                    /* @hack redundant: a test of e3 that never runs */
                    if (0) {
                        if (e3) {
                            break;
                        }
                    }
                    /* @hack spelling: the assignment inside the test, where a
                     * statement of its own would do */
                    if ((distance = njDistanceP2P(&origin, &at)) >
                        query->length) {
                        break;
                    }
                    njInvertMatrix(NULL);
                    njCalcPoint(NULL, &at, &out);
                    at.x = out.x + v0->x;
                    at.y = out.y + v0->y;
                    at.z = out.z + v0->z;
                    if (!met || query->distance > distance) {
                        met = TRUE;
                        query->distance = distance;
                        query->hit = at;
                        query->surface.found = TRUE;
                        query->surface.flags = object->flags;
                        query->surface.ang_x = NORMAL_ANG_X(normal);
                        query->surface.ang_z = NORMAL_ANG_Z(normal);
                        query->surface.normal = normal;
                        reach2 = query->length + REACH;
                        reach2 *= reach2;
                    }
                } while (0);
                njPopMatrixEx();
            }
        }
    }
    return met;
}

Bool polygon_find_near_heights(struct height_probe *probe, Float radius)
{
    struct height_probe farthest;
    Bool any;
    int count;
    struct world_object *object;

    any = FALSE;
    farthest.below.found = FALSE;
    farthest.above.found = FALSE;
    world_list_near(probe->pos.x, probe->pos.y, probe->pos.z, radius);
    count = world_count;
    object = world_objects;
    for (; count > 0; count--, object++) {
        if (find_model_heights_near(probe, object->object, object->flags) ==
            TRUE) {
            if (probe->below.found == TRUE) {
                if (!farthest.below.found) {
                    any = TRUE;
                    farthest.below.found = TRUE;
                    farthest.below.pos = probe->below.pos;
                } else if (farthest.below.pos > probe->below.pos) {
                    farthest.below.pos = probe->below.pos;
                }
            }
            if (probe->above.found == TRUE) {
                if (!farthest.above.found) {
                    any = TRUE;
                    farthest.above.found = TRUE;
                    farthest.above.pos = probe->above.pos;
                } else if (farthest.above.pos < probe->above.pos) {
                    farthest.above.pos = probe->above.pos;
                }
            }
        }
    }
    /* @bug uninitialized: nothing writes farthest's flags, angles and
     * normal, nor a pos whose found stays FALSE, and the probe takes them
     * (MOV.L @(16,R14),R3 at 0x8c01627c on, and MOV.L @R1,R3 at 0x8c0162a4
     * on) */
    probe->below = farthest.below;
    probe->above = farthest.above;
    return any;
}

void polygon_normal(const Float *v0, const Float *v1, const Float *v2,
                    Float *normal)
{
    Float ux;
    Float uy;
    Float uz;
    Float vx;
    Float vy;
    Float vz;

    vx = *v0++;
    ux = *v1++ - vx;
    /* @hack spelling: vy set inside the difference, where a statement of its
     * own would do */
    uy = *v1++ - (vy = *v0++);
    vz = *v0;
    uz = *v1;
    vx -= *v2++;
    vy -= *v2++;
    uz -= vz;
    vz -= *v2;
    normal[0] = (uz * vy) - (uy * vz);
    normal[1] = (ux * vz) - (uz * vx);
    normal[2] = (uy * vx) - (ux * vy);
}

Bool collision_segment_nearest(NJS_POINT3 *start, NJS_POINT3 *end,
                               NJS_POINT3 *point, NJS_POINT3 *nearest,
                               Float *distance2)
{
    NJS_VECTOR along;
    NJS_VECTOR past_start;
    NJS_VECTOR short_of_end;
    NJS_VECTOR away;
    Float part;

    along.x = end->x - start->x;
    along.y = end->y - start->y;
    along.z = end->z - start->z;
    past_start.x = point->x - start->x;
    past_start.y = point->y - start->y;
    past_start.z = point->z - start->z;
    short_of_end.x = end->x - point->x;
    short_of_end.y = end->y - point->y;
    short_of_end.z = end->z - point->z;
    if ((along.x * past_start.x) + (along.y * past_start.y) +
                (along.z * past_start.z) <
            0.0F ||
        (along.x * short_of_end.x) + (along.y * short_of_end.y) +
                (along.z * short_of_end.z) <
            0.0F) {
        return FALSE;
    }
    if (along.x * along.x < NO_PART2 && along.y * along.y < NO_PART2 &&
        along.z * along.z < NO_PART2) {
        *distance2 = (past_start.x * past_start.x) +
                     (past_start.y * past_start.y) +
                     (past_start.z * past_start.z);
        return TRUE;
    }
    part =
        ((along.x * (point->x - start->x)) + (along.y * (point->y - start->y)) +
         (along.z * (point->z - start->z))) /
        ((along.x * along.x) + (along.y * along.y) + (along.z * along.z));
    nearest->x = start->x + (along.x * part);
    nearest->y = start->y + (along.y * part);
    nearest->z = start->z + (along.z * part);
    away.x = nearest->x - point->x;
    away.y = nearest->y - point->y;
    away.z = nearest->z - point->z;
    *distance2 = (away.x * away.x) + (away.y * away.y) + (away.z * away.z);
    return TRUE;
}

Float polygon_point_distance2_across(const NJS_POINT3 *from,
                                     const NJS_POINT3 *to)
{
    Float dx = to->x - from->x;
    Float dz = to->z - from->z;

    return (dx * dx) + (dz * dz);
}

Float polygon_point_distance2(const NJS_POINT3 *from, const NJS_POINT3 *to)
{
    Float dx = to->x - from->x;
    Float dy = to->y - from->y;
    Float dz = to->z - from->z;

    return (dx * dx) + (dy * dy) + (dz * dz);
}

Float collision_segment_distance(NJS_POINT3 *point, NJS_POINT3 *start,
                                 NJS_POINT3 *end, NJS_POINT3 *nearest)
{
    NJS_LINE line;
    Float x;
    Float y;
    Float z;

    if (vector_dot_from(start, end, point) < 0.0F) {
        if (nearest != NULL) {
            *nearest = *start;
        }
        return njDistanceP2P(point, start);
    }
    if (vector_dot_from(end, start, point) < 0.0F) {
        if (nearest != NULL) {
            *nearest = *end;
        }
        return njDistanceP2P(point, end);
    }
    x = start->x;
    y = start->y;
    z = start->z;
    line.px = x;
    line.py = y;
    line.pz = z;
    line.vx = end->x - x;
    line.vy = end->y - y;
    line.vz = end->z - z;
    return njDistanceP2L(point, &line, nearest);
}

/* the squared length of *vector */
/* @hack spelling: the members read through the vector's address, where
 * edge.x and the rest, read from the local itself, would do */
#define SQUARED(vector)                                                        \
    (((vector)->x * (vector)->x) + ((vector)->y * (vector)->y) +               \
     ((vector)->z * (vector)->z))

Bool polygon_find_nearest_height(struct level_probe *probe, NJS_OBJECT *object,
                                 Uint32 flags)
{
    NJS_MODEL model;
    NJS_MODEL *source;
    struct strip strip;
    NJS_VECTOR normal;
    NJS_VECTOR edge;
    int strips;
    NJS_POINT3 *v2;
    NJS_POINT3 *v1;
    NJS_POINT3 *v0;
    NJS_POINT3 *p0;
    NJS_POINT3 *p1;
    NJS_POINT3 *p2;
    Bool found;
    int i;
    Bool reversed;
    Float *pos;
    Float *slot;
    Float reach2;
    Float length2;
    Float px;
    Float py;
    Float pz;
    Float dx;
    Float dy;
    Float dz;
    Float e1;
    Float e2;
    Float e3;
    Float plane;
    Float y;

    found = FALSE;
    source = object->model;
    model = *source;
    strip.model = &model;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    if (!(object->evalflags & NJD_EVAL_UNIT_POS)) {
        njTranslateEx((NJS_VECTOR *)object->pos);
    }
    if (!(object->evalflags & NJD_EVAL_UNIT_ANG)) {
        njRotateZ(NULL, object->ang[2]);
        njRotateY(NULL, object->ang[1]);
        njRotateX(NULL, object->ang[0]);
    }
    if (!(object->evalflags & NJD_EVAL_UNIT_SCL)) {
        njScaleEx((NJS_VECTOR *)object->scl);
    }
    for (; model.nbMeshset != 0; model.meshsets++, model.nbMeshset--) {
        /* @hack spelling: the assignment inside the switch, where a statement
         * of its own would do */
        switch (strip.kind = model.meshsets->type_matId & NJD_MESHSET_MASK) {
        case NJD_MESHSET_3:
            strip.count = 3;
            break;
        case NJD_MESHSET_4:
            strip.count = 4;
            break;
        case NJD_MESHSET_N:
        case NJD_MESHSET_TRIMESH:
            break;
        default:
            return FALSE;
        }
        strips = model.meshsets->nbMesh;
        strip.indices = model.meshsets->meshes;
        for (; strips != 0; strips--) {
            if ((object->evalflags & EVAL_UNIT) == EVAL_UNIT) {
                reversed = strip_next_plain(&strip);
            } else if ((object->evalflags & EVAL_MOVE_ONLY) == EVAL_MOVE_ONLY) {
                reversed = strip_next_moved(&strip, object);
            } else {
                reversed = strip_next(&strip);
            }
            for (i = 0, v0 = &strip.points[0], v1 = &strip.points[1],
                v2 = &strip.points[2];
                 i < strip.count - 2; i++, v0 = v1, v1 = v2, v2++) {
                edge.x = v1->x - v0->x;
                edge.y = v1->y - v0->y;
                edge.z = v1->z - v0->z;
                reach2 = SQUARED(&edge);
                edge.x = v2->x - v1->x;
                edge.y = v2->y - v1->y;
                edge.z = v2->z - v1->z;
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((length2 = SQUARED(&edge)) > reach2) {
                    reach2 = length2;
                }
                edge.x = v0->x - v2->x;
                edge.y = v0->y - v2->y;
                edge.z = v0->z - v2->z;
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((length2 = SQUARED(&edge)) > reach2) {
                    reach2 = length2;
                }
                reach2 += (probe->speed.x * probe->speed.x) +
                          (probe->speed.y * probe->speed.y) +
                          (probe->speed.z * probe->speed.z) +
                          (probe->radius * probe->radius);
                /* @hack spelling: a Float cursor stepped across the members
                 * of the probe's pos, where probe->pos.x, .y and .z would
                 * do */
                pos = &probe->pos.x;
                px = *pos++;
                py = *pos++;
                pz = *pos;
                /* @bug memory: the first reach test reads three Floats of
                 * the stack, from v0's own slot on, not the corner v0
                 * points at (0x8c0168d2, 0x8c0168d6, 0x8c0168da) */
#ifdef NONMATCHING
                /* this side keeps the reads the tag above describes. SHC's
                 * frame holds v0, strips and edge.x from v0's slot on. this
                 * side reads those three by name, each as a Float. an address
                 * or a count read as a Float lies within 1e-30 of 0. so the
                 * test decides alike on both sides */
                dx = px - *(Float *)&v0;
                dy = py - *(Float *)&strips;
                dz = pz - edge.x;
#else
                slot = (Float *)&v0;
                dx = px - *slot++;
                dy = py - *slot++;
                dz = pz - *slot;
#endif
                if ((dx * dx) + (dy * dy) + (dz * dz) > reach2) {
                    continue;
                }
                /* @bug memory: the second reads them from v1's own slot on
                 * (0x8c0168f6, 0x8c0168fc, 0x8c016902) */
#ifdef NONMATCHING
                /* this side keeps the reads, as for the first test: SHC's
                 * frame holds v1, v0 and strips from v1's slot on */
                dx = px - *(Float *)&v1;
                dy = py - *(Float *)&v0;
                dz = pz - *(Float *)&strips;
#else
                slot = (Float *)&v1;
                dx = px - *slot++;
                dy = py - *slot++;
                dz = pz - *slot;
#endif
                if ((dx * dx) + (dy * dy) + (dz * dz) > reach2) {
                    continue;
                }
                /* @bug memory: the third reads them from v2's own slot on
                 * (0x8c01691e, 0x8c016922, 0x8c016926) */
#ifdef NONMATCHING
                /* this side keeps the reads, as for the first test: SHC's
                 * frame holds v2, v1 and v0 from v2's slot on */
                dx = px - *(Float *)&v2;
                dy = py - *(Float *)&v1;
                dz = pz - *(Float *)&v0;
#else
                slot = (Float *)&v2;
                dx = px - *slot++;
                dy = py - *slot++;
                dz = pz - *slot;
#endif
                if ((dx * dx) + (dy * dy) + (dz * dz) > reach2) {
                    continue;
                }
                if (((i + reversed) & 1) == 0) {
                    p0 = v0;
                    p1 = v1;
                    p2 = v2;
                } else {
                    p0 = v0;
                    p1 = v2;
                    p2 = v1;
                }
                e1 = EDGE_SIDE(p0, p1, &probe->pos, z, x);
                e2 = EDGE_SIDE(p1, p2, &probe->pos, z, x);
                e3 = EDGE_SIDE(p2, p0, &probe->pos, z, x);
                if (e1 * e2 < 0.0F || e2 * e3 < 0.0F || e1 * e3 < 0.0F) {
                } else {
                    /* @hack type: the points cast to Float *, where &p0->x
                     * and the rest would do */
                    triangle_normal((Float *)p0, (Float *)p1, (Float *)p2,
                                    (Float *)&normal);
                    plane = -(normal.x * p0->x) - (normal.y * p0->y) -
                            (normal.z * p0->z);
                    if (normal.y != 0.0F) {
                        y = (-(normal.x * probe->pos.x) -
                             (normal.z * probe->pos.z) - plane) /
                            normal.y;
                        UNIT_NORMAL(&normal);
                        /* @hack redundant: found tested again, against
                         * TRUE, which it holds once it isn't FALSE */
                        if (!found ||
                            (found == TRUE &&
                             njAbs(y - probe->pos.y) <
                                 njAbs(probe->level - probe->pos.y))) {
                            probe->level = y;
                            probe->normal = normal;
                            found = TRUE;
                            probe->ang_z = NORMAL_ANG_Z(normal);
                            probe->ang_x = NORMAL_ANG_X(normal);
                        }
                    }
                }
            }
        }
    }
    njPopMatrixEx();
    return found;
}

/* as polygon_find_model_heights, for the triangles that pass a reach test
 * of SIZE_REACH2(flags). a shipped bug has the test measure to stack words,
 * not to a corner */
static Bool find_model_heights_near(struct height_probe *probe,
                                    NJS_OBJECT *object, Uint32 flags)
{
    NJS_POINT3 *v0;
    NJS_POINT3 *v1;
    NJS_POINT3 *v2;
    NJS_POINT3 *points;
    NJS_POINT3 *p0;
    NJS_POINT3 *p1;
    NJS_POINT3 *p2;
    struct surface_side *side;
    NJS_MODEL *source;
    NJS_MODEL model;
    struct strip strip;
    NJS_VECTOR normal;
    Float *pos;
    Float *slot;
    Float reach2;
    Float dx;
    Float dy;
    Float dz;
    Float e1;
    Float e2;
    Float e3;
    Float plane;
    Float y;
    Float nx;
    Bool any;
    int strips;
    int i;
    Bool reversed;

    any = FALSE;
    probe->below.found = FALSE;
    probe->above.found = FALSE;
    source = object->model;
    model = *source;
    strip.model = &model;
    reach2 = SIZE_REACH2(flags);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    if (!(object->evalflags & NJD_EVAL_UNIT_POS)) {
        njTranslateEx((NJS_VECTOR *)object->pos);
    }
    if (!(object->evalflags & NJD_EVAL_UNIT_ANG)) {
        njRotateZ(NULL, object->ang[2]);
        njRotateY(NULL, object->ang[1]);
        njRotateX(NULL, object->ang[0]);
    }
    if (!(object->evalflags & NJD_EVAL_UNIT_SCL)) {
        njScaleEx((NJS_VECTOR *)object->scl);
    }
    for (; model.nbMeshset != 0; model.meshsets++, model.nbMeshset--) {
        /* @hack spelling: the assignment inside the switch, where a statement
         * of its own would do */
        switch (strip.kind = model.meshsets->type_matId & NJD_MESHSET_MASK) {
        case NJD_MESHSET_3:
            strip.count = 3;
            break;
        case NJD_MESHSET_4:
            strip.count = 4;
            break;
        case NJD_MESHSET_N:
        case NJD_MESHSET_TRIMESH:
            break;
        default:
            return FALSE;
        }
        strips = model.meshsets->nbMesh;
        strip.indices = model.meshsets->meshes;
        for (; strips != 0; strips--) {
            if ((object->evalflags & EVAL_UNIT) == EVAL_UNIT) {
                reversed = strip_next_plain(&strip);
            } else if ((object->evalflags & EVAL_MOVE_ONLY) == EVAL_MOVE_ONLY) {
                reversed = strip_next_moved(&strip, object);
            } else {
                reversed = strip_next(&strip);
            }
            for (i = 0, v0 = &strip.points[0], points = strip.points,
                v1 = &points[1], v2 = &points[2];
                 i < strip.count - 2; i++, v0 = v1, v1 = v2, v2++) {
                /* @hack spelling: a Float cursor stepped across the members
                 * of the probe's pos, where probe->pos.x, .y and .z would
                 * do */
                pos = &probe->pos.x;
                /* @bug memory: the reach test reads three Floats of the
                 * stack, from v0's own slot on, not the corner v0 points at
                 * (0x8c016d76, 0x8c016d7c, 0x8c016d82) */
#ifdef NONMATCHING
                /* this side keeps the reads the tag above describes, as
                 * polygon_find_nearest_height's does. SHC's frame holds v0, v2
                 * and i from v0's slot on */
                dx = *pos++ - *(Float *)&v0;
                dy = *pos++ - *(Float *)&v2;
                dz = *pos - *(Float *)&i;
#else
                slot = (Float *)&v0;
                dx = *pos++ - *slot++;
                dy = *pos++ - *slot++;
                dz = *pos - *slot;
#endif
                if ((dx * dx) + (dy * dy) + (dz * dz) > reach2) {
                    continue;
                }
                if (((i + reversed) & 1) == 0) {
                    p0 = v0;
                    p1 = v1;
                    p2 = v2;
                } else {
                    p0 = v0;
                    p1 = v2;
                    p2 = v1;
                }
                do {
                    e1 = EDGE_SIDE(p0, p1, &probe->pos, z, x);
                    e2 = EDGE_SIDE(p1, p2, &probe->pos, z, x);
                    e3 = EDGE_SIDE(p2, p0, &probe->pos, z, x);
                    if (e1 * e2 < 0.0F || e2 * e3 < 0.0F || e1 * e3 < 0.0F) {
                        break;
                    }
                    triangle_normal(&p0->x, &p1->x, &p2->x, &normal.x);
                    /* @hack spelling: nx set inside the product, where a
                     * statement of its own would do */
                    plane = (-(nx = normal.x) * p0->x) - (normal.y * p0->y) -
                            (normal.z * p0->z);
                    if (normal.y * normal.y >= NO_PART2) {
                        y = (-(normal.z * probe->pos.z) - (nx * probe->pos.x) -
                             plane) /
                            normal.y;
                        side = NULL;
                        if (probe->pos.y > y) {
                            if (!probe->below.found || y > probe->below.pos) {
                                side = &probe->below;
                            }
                        } else {
                            if (!probe->above.found || probe->above.pos > y) {
                                side = &probe->above;
                            }
                        }
                        if (side == NULL) {
                            break;
                        }
                        UNIT_NORMAL(&normal);
                        any = TRUE;
                        side->found = TRUE;
                        side->pos = y;
                        side->normal = normal;
                        side->ang_z = NORMAL_ANG_Z(normal);
                        side->ang_x = NORMAL_ANG_X(normal);
                    }
                } while (0);
            }
        }
    }
    njPopMatrixEx();
    /* @hack redundant: a test of strips that never runs */
    if (0) {
        if (strips) {
        }
    }
    return any;
}

Bool polygon_find_model_sides(struct surface_probe *probe, NJS_OBJECT *object,
                              Uint32 flags)
{
    NJS_POINT3 *v0;
    NJS_POINT3 *v1;
    NJS_POINT3 *v2;
    NJS_POINT3 *p0;
    NJS_POINT3 *p1;
    NJS_POINT3 *p2;
    struct surface_side *side;
    NJS_MODEL *source;
    NJS_MODEL model;
    struct strip strip;
    NJS_VECTOR normal;
    Float *pos;
    Float *slot;
    Float reach2;
    Float dx;
    Float dy;
    Float dz;
    Float e1;
    Float e2;
    Float e3;
    Float plane;
    Float x;
    Float y;
    Float z;
    Float nx;
    Bool any;
    int strips;
    int i;
    Bool reversed;

    any = FALSE;
    for (i = SURFACE_SIDES, side = probe->sides; i > 0; i--, side++) {
        side->found = FALSE;
    }
    source = object->model;
    model = *source;
    strip.model = &model;
    reach2 = SIZE_REACH2(flags);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    if (!(object->evalflags & NJD_EVAL_UNIT_POS)) {
        njTranslateEx((NJS_VECTOR *)object->pos);
    }
    if (!(object->evalflags & NJD_EVAL_UNIT_ANG)) {
        njRotateZ(NULL, object->ang[2]);
        njRotateY(NULL, object->ang[1]);
        njRotateX(NULL, object->ang[0]);
    }
    if (!(object->evalflags & NJD_EVAL_UNIT_SCL)) {
        njScaleEx((NJS_VECTOR *)object->scl);
    }
    for (; model.nbMeshset != 0; model.meshsets++, model.nbMeshset--) {
        /* @hack spelling: the assignment inside the switch, where a statement
         * of its own would do */
        switch (strip.kind = model.meshsets->type_matId & NJD_MESHSET_MASK) {
        case NJD_MESHSET_3:
            strip.count = 3;
            break;
        case NJD_MESHSET_4:
            strip.count = 4;
            break;
        case NJD_MESHSET_N:
        case NJD_MESHSET_TRIMESH:
            break;
        default:
            return FALSE;
        }
        strips = model.meshsets->nbMesh;
        strip.indices = model.meshsets->meshes;
        for (; strips != 0; strips--) {
            if ((object->evalflags & EVAL_UNIT) == EVAL_UNIT) {
                reversed = strip_next_plain(&strip);
            } else if ((object->evalflags & EVAL_MOVE_ONLY) == EVAL_MOVE_ONLY) {
                reversed = strip_next_moved(&strip, object);
            } else {
                reversed = strip_next(&strip);
            }
            for (i = 0, v0 = &strip.points[0], v1 = &strip.points[1],
                v2 = &strip.points[2];
                 i < strip.count - 2; i++, v0 = v1, v1 = v2, v2++) {
                /* @hack spelling: a Float cursor stepped across the members
                 * of the probe's pos, where probe->pos.x, .y and .z would
                 * do */
                pos = &probe->pos.x;
                /* @bug memory: the reach test reads three Floats of the
                 * stack, from v0's own slot on, not the corner v0 points at
                 * (0x8c017222, 0x8c01722a, 0x8c017230) */
#ifdef NONMATCHING
                /* this side keeps the reads the tag above describes, as
                 * polygon_find_nearest_height's does. SHC's frame holds v0,
                 * object and v2 from v0's slot on */
                dx = *pos++ - *(Float *)&v0;
                dy = *pos++ - *(Float *)&object;
                dz = *pos - *(Float *)&v2;
#else
                slot = (Float *)&v0;
                dx = *pos++ - *slot++;
                dy = *pos++ - *slot++;
                dz = *pos - *slot;
#endif
                if ((dx * dx) + (dy * dy) + (dz * dz) > reach2) {
                    continue;
                }
                if (((i + reversed) & 1) == 0) {
                    p0 = v0;
                    p1 = v1;
                    p2 = v2;
                } else {
                    p0 = v0;
                    p1 = v2;
                    p2 = v1;
                }
                do {
                    e1 = EDGE_SIDE(p0, p1, &probe->pos, x, y);
                    e2 = EDGE_SIDE(p1, p2, &probe->pos, x, y);
                    e3 = EDGE_SIDE(p2, p0, &probe->pos, x, y);
                    if (e1 * e2 < 0.0F || e2 * e3 < 0.0F || e1 * e3 < 0.0F) {
                        break;
                    }
                    triangle_normal(&p0->x, &p1->x, &p2->x, &normal.x);
                    /* @hack spelling: nx set inside the product, where a
                     * statement of its own would do */
                    plane = (-(nx = normal.x) * p0->x) - (normal.y * p0->y) -
                            (normal.z * p0->z);
                    if (normal.z * normal.z < NO_PART2) {
                        break;
                    }
                    z = (-(nx * probe->pos.x) - (normal.y * probe->pos.y) -
                         plane) /
                        normal.z;
                    side = NULL;
                    if (z < probe->pos.z) {
                        if (!probe->sides[SURFACE_NEG_Z].found ||
                            z > probe->sides[SURFACE_NEG_Z].pos) {
                            side = &probe->sides[SURFACE_NEG_Z];
                        }
                    } else {
                        if (!probe->sides[SURFACE_POS_Z].found ||
                            z < probe->sides[SURFACE_POS_Z].pos) {
                            side = &probe->sides[SURFACE_POS_Z];
                        }
                    }
                    if (side == NULL) {
                        break;
                    }
                    UNIT_NORMAL(&normal);
                    any = TRUE;
                    side->found = TRUE;
                    side->pos = z;
                    side->normal = normal;
                    side->ang_z = NORMAL_ANG_Z(normal);
                    side->ang_x = NORMAL_ANG_X(normal);
                } while (0);
                do {
                    e1 = EDGE_SIDE(p0, p1, &probe->pos, y, z);
                    e2 = EDGE_SIDE(p1, p2, &probe->pos, y, z);
                    e3 = EDGE_SIDE(p2, p0, &probe->pos, y, z);
                    if (e1 * e2 < 0.0F || e2 * e3 < 0.0F || e1 * e3 < 0.0F) {
                        break;
                    }
                    triangle_normal(&p0->x, &p1->x, &p2->x, &normal.x);
                    /* @hack spelling: nx set inside the product, where a
                     * statement of its own would do */
                    plane = (-(nx = normal.x) * p0->x) - (normal.y * p0->y) -
                            (normal.z * p0->z);
                    if (nx * normal.x < NO_PART2) {
                        break;
                    }
                    x = (-(normal.y * probe->pos.y) -
                         (normal.z * probe->pos.z) - plane) /
                        nx;
                    side = NULL;
                    if (x < probe->pos.x) {
                        if (!probe->sides[SURFACE_NEG_X].found ||
                            x > probe->sides[SURFACE_NEG_X].pos) {
                            side = &probe->sides[SURFACE_NEG_X];
                        }
                    } else {
                        if (!probe->sides[SURFACE_POS_X].found ||
                            x < probe->sides[SURFACE_POS_X].pos) {
                            side = &probe->sides[SURFACE_POS_X];
                        }
                    }
                    if (side == NULL) {
                        break;
                    }
                    UNIT_NORMAL(&normal);
                    any = TRUE;
                    side->found = TRUE;
                    side->pos = x;
                    side->normal = normal;
                    side->ang_z = NORMAL_ANG_Z(normal);
                    side->ang_x = NORMAL_ANG_X(normal);
                } while (0);
                do {
                    e1 = EDGE_SIDE(p0, p1, &probe->pos, z, x);
                    e2 = EDGE_SIDE(p1, p2, &probe->pos, z, x);
                    e3 = EDGE_SIDE(p2, p0, &probe->pos, z, x);
                    if (e1 * e2 < 0.0F || e2 * e3 < 0.0F || e1 * e3 < 0.0F) {
                        break;
                    }
                    triangle_normal(&p0->x, &p1->x, &p2->x, &normal.x);
                    /* @hack spelling: nx set inside the product, where a
                     * statement of its own would do */
                    plane = (-(nx = normal.x) * p0->x) - (normal.y * p0->y) -
                            (normal.z * p0->z);
                    if (normal.y * normal.y < NO_PART2) {
                        break;
                    }
                    y = (-(normal.z * probe->pos.z) - (nx * probe->pos.x) -
                         plane) /
                        normal.y;
                    side = NULL;
                    if (y < probe->pos.y) {
                        if (!probe->sides[SURFACE_NEG_Y].found ||
                            y > probe->sides[SURFACE_NEG_Y].pos) {
                            side = &probe->sides[SURFACE_NEG_Y];
                        }
                    } else {
                        if (!probe->sides[SURFACE_POS_Y].found ||
                            y < probe->sides[SURFACE_POS_Y].pos) {
                            side = &probe->sides[SURFACE_POS_Y];
                        }
                    }
                    if (side == NULL) {
                        break;
                    }
                    UNIT_NORMAL(&normal);
                    any = TRUE;
                    side->found = TRUE;
                    side->pos = y;
                    side->normal = normal;
                    side->ang_z = NORMAL_ANG_Z(normal);
                    side->ang_x = NORMAL_ANG_X(normal);
                } while (0);
            }
        }
    }
    njPopMatrixEx();
    return any;
}

Bool polygon_find_model_heights(struct height_probe *probe, NJS_OBJECT *object)
{
    NJS_MODEL model;
    struct strip strip;
    NJS_VECTOR normal;
    NJS_MODEL *source;
    Bool any;
    struct surface_side *side;
    NJS_POINT3 *v2;
    NJS_POINT3 *v1;
    NJS_POINT3 *v0;
    NJS_POINT3 *p0;
    NJS_POINT3 *p1;
    NJS_POINT3 *p2;
    int i;
    int strips;
    Bool reversed;
    Float nx;
    Float e1;
    Float e2;
    Float e3;
    Float plane;
    Float y;

    any = FALSE;
    probe->below.found = FALSE;
    probe->above.found = FALSE;
    source = object->model;
    model = *source;
    strip.model = &model;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    if (!(object->evalflags & NJD_EVAL_UNIT_POS)) {
        njTranslateEx((NJS_VECTOR *)object->pos);
    }
    if (!(object->evalflags & NJD_EVAL_UNIT_ANG)) {
        njRotateZ(NULL, object->ang[2]);
        njRotateY(NULL, object->ang[1]);
        njRotateX(NULL, object->ang[0]);
    }
    if (!(object->evalflags & NJD_EVAL_UNIT_SCL)) {
        njScaleEx((NJS_VECTOR *)object->scl);
    }
    for (; model.nbMeshset != 0; model.meshsets++, model.nbMeshset--) {
        /* @hack spelling: the assignment inside the switch, where a statement
         * of its own would do */
        switch (strip.kind = model.meshsets->type_matId & NJD_MESHSET_MASK) {
        case NJD_MESHSET_3:
            strip.count = 3;
            break;
        case NJD_MESHSET_4:
            strip.count = 4;
            break;
        case NJD_MESHSET_N:
        case NJD_MESHSET_TRIMESH:
            break;
        default:
            return FALSE;
        }
        strips = model.meshsets->nbMesh;
        strip.indices = model.meshsets->meshes;
        for (; strips != 0; strips--) {
            if ((object->evalflags & EVAL_UNIT) == EVAL_UNIT) {
                reversed = strip_next_plain(&strip);
            } else if ((object->evalflags & EVAL_MOVE_ONLY) == EVAL_MOVE_ONLY) {
                reversed = strip_next_moved(&strip, object);
            } else {
                reversed = strip_next(&strip);
            }
            for (i = 0, v0 = &strip.points[0], v1 = &strip.points[1],
                v2 = &strip.points[2];
                 i < strip.count - 2; i++, v0 = v1, v1 = v2, v2++) {
                if (((i + reversed) & 1) == 0) {
                    p0 = v0;
                    p1 = v1;
                    p2 = v2;
                } else {
                    p0 = v0;
                    p1 = v2;
                    p2 = v1;
                }
                do {
                    e1 = EDGE_SIDE(p0, p1, &probe->pos, z, x);
                    e2 = EDGE_SIDE(p1, p2, &probe->pos, z, x);
                    e3 = EDGE_SIDE(p2, p0, &probe->pos, z, x);
                    if (e1 * e2 < 0.0F || e2 * e3 < 0.0F || e1 * e3 < 0.0F) {
                        break;
                    }
                    triangle_normal(&p0->x, &p1->x, &p2->x, &normal.x);
                    /* @hack spelling: nx set inside the product, where a
                     * statement of its own would do */
                    plane = (-(nx = normal.x) * p0->x) - (normal.y * p0->y) -
                            (normal.z * p0->z);
                    if (normal.y * normal.y >= NO_PART2) {
                        y = (-(normal.z * probe->pos.z) - (nx * probe->pos.x) -
                             plane) /
                            normal.y;
                        side = NULL;
                        if (probe->pos.y > y) {
                            if (!probe->below.found || y > probe->below.pos) {
                                side = &probe->below;
                            }
                        } else {
                            if (!probe->above.found || probe->above.pos > y) {
                                side = &probe->above;
                            }
                        }
                        if (side == NULL) {
                            break;
                        }
                        UNIT_NORMAL(&normal);
                        any = TRUE;
                        side->found = TRUE;
                        side->pos = y;
                        side->normal = normal;
                        side->ang_z = NORMAL_ANG_Z(normal);
                        side->ang_x = NORMAL_ANG_X(normal);
                    }
                } while (0);
            }
        }
    }
    njPopMatrixEx();
    return any;
}

void collision_mesh_forget(NJS_OBJECT *object)
{
    struct collision_mesh *mesh;

    for (mesh = collision_mesh_list; mesh != NULL; mesh = mesh->next) {
        if (mesh->object == object) {
            mesh->object = NULL;
            break;
        }
    }
}
