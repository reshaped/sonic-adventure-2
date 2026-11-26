/* the camera's probes of the land. each casts a line, or moves a sphere,
 * among the faces of the world objects near its start */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "camera/probe.h"
#include "collision/polygon.h"
#include "collision/world.h"
#include "draw/lines.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"

#define FACES 384 /* the faces the list holds */
#define MOVE_FACES 128 /* the most of them a move takes */
/* the radius of the ball the probes sweep for their faces */
#define BALL_RADIUS 10.0F
/* camera_probe_sweep tests its move in pieces no longer than PIECE_LENGTH,
 * with a ball of PIECE_RADIUS */
#define PIECE_LENGTH 100.0F
#define PIECE_RADIUS 1.0F
/* the most two corners may differ in a coordinate and still share it */
#define FLAT_SLACK 0.2F
/* the most edges' slopes may differ and still count as parallel */
#define SLOPE_SLACK 0.01F

/* the length of the normal camera_probe_slide_hit draws at hit. the moves'
 * debug draws scale the offset they give face_draw by it too */
#define NORMAL_LENGTH 10.0F
/* face_draw lifts layer n (n + 1) times LAYER_LIFT of its offset off the
 * face. a face drawn again in a higher layer then shows over its first draw */
#define LAYER_LIFT 0.001F
/* face_draw's colors for the corners of layer 0, then of any higher layer */
#define RED 0xffff0000
#define GREEN 0xff00ff00
#define BLUE 0xff0000ff
#define MAGENTA 0xffff00ff
#define YELLOW 0xffffff00
#define CYAN 0xff00ffff
/* the size of the mark camera_probe_slide_hit draws at hit */
#define MARK_SIZE 5.0F
/* a cast prints each count of its search at PRINT_COLUMN, on the row
 * CAST_ROW plus the count's index. the lengthened cast prints them from
 * LONG_ROW */
#define PRINT_COLUMN 2
#define CAST_ROW 12
#define LONG_ROW 17
#define PRINT_DIGITS 2
/* where camera_probe_slide_hit prints SLIDE! for a slide that crossed a face */
#define PRINT_SLIDE NJM_LOCATION(5, 25)
/* where the searches for the nearest crossing start: farther than a crossing
 * on a probe's line can lie. a cast starts its distance at FARTHEST. the search
 * of an object's contacts starts its squared distance at FARTHEST2, and
 * camera_probe_slide_hit its own at SLIDE_FARTHEST2 */
#define FARTHEST 10000.0F
#define FARTHEST2 1000000.0F
#define SLIDE_FARTHEST2 100000.0F

/* a face the probes took: a surface the ball touched, and its triangle's
 * corners */
struct probe_face {
    struct polygon_contact contact;
    NJS_POINT3 corners[3];
};

/* where a cast's line crosses a face, and the face's tilt, unit normal and
 * corners */
struct crossing {
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR normal;
    NJS_POINT3 corners[3];
};

/* an edge of a triangle seen along an axis. slope and intercept give the
 * line y = slope * x + intercept through its ends. from is the end with the
 * lower x. the edge spans each x from from.x up to, but not including,
 * to.x. an edge whose ends share an x spans none, and its slope isn't
 * finite */
struct edge {
    NJS_POINT2 from, to;
    Float slope;
    Float intercept;
};

/* the slots of search, for the debug print. each tells of the last search
 * of an object's contacts. SEARCH_TESTED counts the crossings the search
 * tested, and SEARCH_HELD those that lay on their faces. SEARCH_NEARER
 * counts those nearer than every one before them. SEARCH_NEAREST holds no
 * count: it is the index in polygon_contacts of the nearest crossing's
 * contact, or -1 */
enum {
    SEARCH_TESTED,
    SEARCH_HELD,
    SEARCH_NEARER,
    SEARCH_NEAREST,
    SEARCH_SLOTS
};

/* how a face stops a slide. SLIDE_CROSSED is where the slide's lengthened
 * line crosses it. SLIDE_NEAR is where next lies within radius of it, over
 * it along its normal. no slide takes SLIDE_UNK_2 */
enum slide_kind { SLIDE_NONE, SLIDE_CROSSED, SLIDE_UNK_2, SLIDE_NEAR };

static void probe_start(struct camera_probe *probe);
static void probe_sweep_start(struct camera_probe *probe);
static Bool probe_cross_contacts(struct camera_probe *probe,
                                 struct crossing *crossing);
static Bool probe_cross_contacts_long(struct camera_probe *probe,
                                      struct crossing *crossing);
static int faces_take_fronts(void);
static int faces_take_all(void);
static Bool probe_slide(NJS_POINT3 *pos, NJS_POINT3 *next,
                        struct camera_probe *probe, int index);

/* nothing calls these. none is static, since SHC leaves out a static
 * function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* sweep a ball of PIECE_RADIUS from probe's pos to its next by
 * world_sweep_ball_hit, a piece at a time. it returns TRUE at the first piece
 * to meet an object without a bit of skip, else FALSE. each piece lists the
 * world objects anew in world_objects */
Bool camera_probe_sweep(struct camera_probe *probe);
/* move a sphere of probe's radius from its pos toward its next by casts. it
 * returns TRUE where a face stops it, else FALSE. hit is where the sphere
 * comes to rest, and moved is hit less pos. pos and next end as they started,
 * but next ends at pos where the sphere can't move */
Bool camera_probe_move_by_casts(struct camera_probe *probe);
/* slide a cast's hit along the plane of the face it crossed, toward next's
 * foot on that plane. it returns TRUE where the slide crosses another face of
 * the list. hit is then that crossing, and ang and normal that face's.
 * otherwise it returns FALSE, with hit at the foot. probe must hold the
 * crossing a cast just found */
Bool camera_probe_slide_hit(struct camera_probe *probe);
/* camera_probe_cast, but for three things. its sweeps are loose, as
 * polygon_sweep says. its line is made probe's radius longer, and each
 * crossing moves back along it by the radius. a next past CAMERA_PROBE_REACH
 * leaves moved as it was */
Bool camera_probe_cast_long(struct camera_probe *probe);
/* NOLINTEND(misc-use-internal-linkage) */

/* the list of faces, or null while there is none. faces and
 * camera_probe_debug are initialized so that they lie in the object's data,
 * as the image has them */
static struct probe_face *faces = NULL;
Sint32 camera_probe_debug = 0;

static struct polygon_ball ball;
static Sint32 face_count;
/* the index in faces of the last face a search took for its nearest
 * crossing */
static Sint32 crossed_face;
/* TRUE where the last triangle_holds_ test saw its triangle edge-on. its
 * three edges' slopes then agree to within SLOPE_SLACK */
static Bool edges_parallel;
/* @hack type: search as an array of four Sint32, where a struct of them
 * would do */
static Sint32 search[SEARCH_SLOTS];

void camera_probe_alloc(void)
{
    if (faces == NULL) {
        faces = syCalloc(FACES, sizeof(struct probe_face));
    }
}

void camera_probe_free(void)
{
    if (faces != NULL) {
        syFree(faces);
        faces = NULL;
    }
}

Bool camera_probe_sweep(struct camera_probe *probe)
{
    NJS_POINT3 pos;
    NJS_VECTOR piece;
    NJS_VECTOR move;
    int pieces;
    int i;
    Bool met;

    vector_subtract(&probe->next, &probe->pos, &piece);
    pieces = (int)(njScalor(&piece) * (1.0F / PIECE_LENGTH)) + 1;
    piece.x /= (Float)pieces;
    piece.y /= (Float)pieces;
    piece.z /= (Float)pieces;
    for (i = 0; i < pieces; i++) {
        move = piece;
        pos.x = probe->pos.x + (piece.x * (Float)i);
        pos.y = probe->pos.y + (piece.y * (Float)i);
        pos.z = probe->pos.z + (piece.z * (Float)i);
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((met = world_sweep_ball_hit(&pos, &move, NULL, probe->skip,
                                        PIECE_RADIUS))) {
            return met;
        }
    }
    return FALSE;
}

/* camera_probe_cast, but leaving moved as it was */
static Bool probe_cast(struct camera_probe *probe)
{
    NJS_VECTOR speed;
    NJS_VECTOR to_place;
    struct crossing crossing;
    Bool result;
    Bool crossed;
    Float best;
    Float length;
    int i;

    best = FARTHEST;
    face_count = 0;
    probe_start(probe);
    probe->hit = probe->next;
    result = FALSE;
    if (njScalor2(&probe->dir) > (CAMERA_PROBE_REACH * CAMERA_PROBE_REACH)) {
        return FALSE;
    }
    probe_sweep_start(probe);
    for (i = 0; i < world_count; i++) {
        if (world_objects[i].flags & probe->skip) {
            continue;
        }
        if (polygon_sweep(&ball, world_objects[i].object,
                          world_objects[i].flags, FALSE)) {
            speed.x = 0.0F;
            speed.y = 0.0F;
            speed.z = 0.0F;
            if (world_objects[i].owner != NULL) {
                struct task_force *force;

                force = world_objects[i].owner->force.p;
                if (force != NULL) {
                    speed = force->speed;
                }
            }
            /* probe_cross_contacts below replaces crossing.pos before anything
             * reads the sum */
            vector_add(&speed, &crossing.pos, &crossing.pos);
#ifdef NONMATCHING
            /* GCC can't see that crossed is set wherever it is read. a
             * polygon_sweep that returns TRUE leaves ball.count above 0 */
            crossed = FALSE;
#endif
            if (ball.count > 0) {
                crossed = probe_cross_contacts(probe, &crossing);
            }
            vector_subtract(&crossing.pos, &probe->pos, &to_place);
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((length = njScalor(&to_place)) < best) {
                best = length;
                probe->hit = crossing.pos;
                probe->ang = crossing.ang;
                probe->normal = crossing.normal;
                probe->flags = world_objects[i].flags;
                if (crossed) {
                    result = TRUE;
                }
            }
        }
    }
    return result;
}

Bool camera_probe_cast(struct camera_probe *probe)
{
    Bool result = probe_cast(probe);

    vector_subtract(&probe->hit, &probe->pos, &probe->moved);
    return result;
}

Bool camera_probe_move_by_casts(struct camera_probe *probe)
{
    NJS_POINT3 start;
    NJS_POINT3 end;
    NJS_VECTOR ahead;
    Bool result;

    start = probe->pos;
    end = probe->next;
    vector_subtract(&end, &start, &ahead);
    if (ahead.x != 0.0F || ahead.y != 0.0F || ahead.z != 0.0F) {
        njUnitVector(&ahead);
    }
    ahead.x *= probe->radius;
    ahead.y *= probe->radius;
    ahead.z *= probe->radius;
    vector_add(&end, &ahead, &probe->next);
    result = probe_cast(probe);
    if (!result) {
        NJS_POINT3 foot;
        NJS_PLANE plane;
        int i;
        Bool near;

        near = FALSE;
        for (i = 0; i < face_count; i++) {
            plane.px = faces[i].corners[0].x;
            plane.py = faces[i].corners[0].y;
            plane.pz = faces[i].corners[0].z;
            plane.vx = faces[i].contact.hit.front.x;
            plane.vy = faces[i].contact.hit.front.y;
            plane.vz = faces[i].contact.hit.front.z;
            if (probe->radius > njDistanceP2PL(&end, &plane, &foot)) {
                near = TRUE;
                ahead.x = faces[i].contact.hit.front.x * probe->radius;
                ahead.y = faces[i].contact.hit.front.y * probe->radius;
                ahead.z = faces[i].contact.hit.front.z * probe->radius;
                vector_add(&foot, &ahead, &probe->hit);
            }
        }
        if (!near) {
            probe->hit = end;
            result = FALSE;
        } else {
            result = TRUE;
        }
        probe->pos = start;
        probe->next = end;
        vector_subtract(&probe->hit, &probe->pos, &probe->moved);
        return result;
    } else {
        NJS_PLANE plane;
        /* @hack redundant: line, which nothing uses */
        NJS_LINE line;
        NJS_POINT3 foot;
        NJS_POINT3 slid;
        NJS_VECTOR along;

        plane.px = probe->hit.x;
        plane.py = probe->hit.y;
        plane.pz = probe->hit.z;
        plane.vx = probe->normal.x;
        plane.vy = probe->normal.y;
        plane.vz = probe->normal.z;
        njDistanceP2PL(&end, &plane, &foot);
        along.x = probe->normal.x * probe->radius;
        along.y = probe->normal.y * probe->radius;
        along.z = probe->normal.z * probe->radius;
        vector_add(&foot, &along, &probe->next);
        slid = probe->next;
        vector_subtract(&probe->next, &probe->pos, &along);
        if (along.x == 0.0F && along.y == 0.0F && along.z == 0.0F) {
            probe->pos = start;
            probe->next = start;
            vector_subtract(&probe->hit, &probe->pos, &probe->moved);
            return TRUE;
        }
        njUnitVector(&along);
        along.x *= probe->radius;
        along.y *= probe->radius;
        along.z *= probe->radius;
        vector_add(&probe->next, &along, &probe->next);
        if (camera_probe_cast(probe)) {
            vector_subtract(&probe->hit, &probe->pos, &ahead);
            if (ahead.x != 0.0F || ahead.y != 0.0F || ahead.z != 0.0F) {
                njUnitVector(&ahead);
            }
            ahead.x *= probe->radius;
            ahead.y *= probe->radius;
            ahead.z *= probe->radius;
            vector_subtract(&probe->hit, &ahead, &probe->hit);
            probe->pos = start;
            probe->next = end;
            vector_subtract(&probe->hit, &probe->pos, &probe->moved);
        } else {
            probe->hit = slid;
            probe->pos = start;
            probe->next = end;
            vector_subtract(&probe->hit, &probe->pos, &probe->moved);
        }
        return TRUE;
    }
}

static void probe_start(struct camera_probe *probe)
{
    vector_subtract(&probe->next, &probe->pos, &probe->dir);
    probe->slid_face = -1;
}

static void probe_sweep_start(struct camera_probe *probe)
{
    ball.up.x = 0.0F;
    ball.up.y = 1.0F;
    ball.up.z = 0.0F;
    ball.pos = probe->pos;
    ball.radius = BALL_RADIUS;
    vector_subtract(&probe->next, &probe->pos, &ball.speed);
    polygon_collect_near(&ball);
}

static void face_draw(NJS_POINT3 *corners, NJS_VECTOR *offset, int layer)
{
    NJS_POLYGON_VTX vertices[3];
    int i;

    for (i = 0; i < 3; i++) {
        vertices[i].x =
            corners[i].x + (offset->x * (Float)(layer + 1) * LAYER_LIFT);
        vertices[i].y =
            corners[i].y + (offset->y * (Float)(layer + 1) * LAYER_LIFT);
        vertices[i].z =
            corners[i].z + (offset->z * (Float)(layer + 1) * LAYER_LIFT);
    }
    switch (layer) {
    case 0:
        vertices[0].col = RED;
        vertices[1].col = GREEN;
        vertices[2].col = BLUE;
        break;
    default:
        vertices[0].col = MAGENTA;
        vertices[1].col = YELLOW;
        vertices[2].col = CYAN;
        break;
    }
    njDrawPolygon3DEx(vertices, 3, FALSE);
}

/* set edge to the edge from p0 to p1 as seen along y. p0 and p1 each point to a
 * point's x, which its y and z follow. the triangle tests hold the points so */
static void edge_set_xz(struct edge *edge, const Float *p0, const Float *p1)
{
    NJS_POINT2 end;

    edge->from.x = p0[0];
    edge->from.y = p0[2];
    edge->to.x = p1[0];
    edge->to.y = p1[2];
    if (p0[0] > p1[0]) {
        end = edge->from;
        edge->from = edge->to;
        edge->to = end;
    }
    edge->slope = (edge->from.y - edge->to.y) / (edge->from.x - edge->to.x);
    edge->intercept = edge->from.y - (edge->slope * edge->from.x);
}

/* the same as seen along z */
static void edge_set_xy(struct edge *edge, const Float *p0, const Float *p1)
{
    NJS_POINT2 end;

    edge->from.x = p0[0];
    edge->from.y = p0[1];
    edge->to.x = p1[0];
    edge->to.y = p1[1];
    if (p0[0] > p1[0]) {
        end = edge->from;
        edge->from = edge->to;
        edge->to = end;
    }
    edge->slope = (edge->from.y - edge->to.y) / (edge->from.x - edge->to.x);
    edge->intercept = edge->from.y - (edge->slope * edge->from.x);
}

/* the same as seen along x */
/* @hack type: p0 and p1 as Float pointers, where NJS_POINT3 pointers would
 * do */
static void edge_set_zy(struct edge *edge, const Float *p0, const Float *p1)
{
    NJS_POINT2 end;

    edge->from.x = p0[2];
    edge->from.y = p0[1];
    edge->to.x = p1[2];
    edge->to.y = p1[1];
    if (p0[2] > p1[2]) {
        end = edge->from;
        edge->from = edge->to;
        edge->to = end;
    }
    edge->slope = (edge->from.y - edge->to.y) / (edge->from.x - edge->to.x);
    edge->intercept = edge->from.y - (edge->slope * edge->from.x);
}

static void edges_test_parallel(struct edge *edges)
{
    if (njAbs(edges[0].slope - edges[1].slope) < SLOPE_SLACK &&
        njAbs(edges[1].slope - edges[2].slope) < SLOPE_SLACK &&
        njAbs(edges[0].slope - edges[2].slope) < SLOPE_SLACK) {
        edges_parallel = TRUE;
    } else {
        edges_parallel = FALSE;
    }
}

/* whether the triangle of p0, p1 and p2 holds point as seen along y. it
 * sets edges_parallel too */
/* @hack type: p0, p1, p2 and point as Float pointers, where NJS_POINT3
 * pointers would do */
static Bool triangle_holds_xz(Float *p0, Float *p1, Float *p2,
                              const Float *point)
{
    struct edge edges[3];
    Float y[2];
    /* @hack type: i, first and second as Sint8, where int would do */
    Sint8 i;
    Sint8 first;
    Sint8 second;

    first = -1;
    second = -1;
    edge_set_xz(&edges[0], p0, p1);
    edge_set_xz(&edges[1], p1, p2);
    edge_set_xz(&edges[2], p2, p0);
    edges_test_parallel(edges);
    for (i = 0; i < 3; i++) {
        if (edges[i].from.x <= point[0] && edges[i].to.x > point[0]) {
            if (first < 0) {
                first = i;
            } else {
                second = i;
                break;
            }
        }
    }
    if (second < 0) {
        return FALSE;
    }
    y[0] = (edges[first].slope * point[0]) + edges[first].intercept;
    y[1] = (edges[second].slope * point[0]) + edges[second].intercept;
    if (y[0] < y[1]) {
        if (point[2] >= y[0] && point[2] < y[1]) {
            return TRUE;
        }
    } else {
        if (point[2] < y[0] && point[2] >= y[1]) {
            return TRUE;
        }
    }
    return FALSE;
}

/* the same as seen along z */
/* @hack type: as in triangle_holds_xz */
static Bool triangle_holds_xy(Float *p0, Float *p1, Float *p2,
                              const Float *point)
{
    struct edge edges[3];
    Float y[2];
    /* @hack type: i, first and second as Sint8, where int would do */
    Sint8 i;
    Sint8 first;
    Sint8 second;

    first = -1;
    second = -1;
    edge_set_xy(&edges[0], p0, p1);
    edge_set_xy(&edges[1], p1, p2);
    edge_set_xy(&edges[2], p2, p0);
    edges_test_parallel(edges);
    for (i = 0; i < 3; i++) {
        if (edges[i].from.x <= point[0] && edges[i].to.x > point[0]) {
            if (first < 0) {
                first = i;
            } else {
                second = i;
                break;
            }
        }
    }
    if (second < 0) {
        return FALSE;
    }
    y[0] = (edges[first].slope * point[0]) + edges[first].intercept;
    y[1] = (edges[second].slope * point[0]) + edges[second].intercept;
    if (y[0] < y[1]) {
        if (point[1] >= y[0] && point[1] < y[1]) {
            return TRUE;
        }
    } else {
        if (point[1] < y[0] && point[1] >= y[1]) {
            return TRUE;
        }
    }
    return FALSE;
}

/* the same as seen along x */
/* @hack type: as in triangle_holds_xz */
static Bool triangle_holds_zy(Float *p0, Float *p1, Float *p2,
                              const Float *point)
{
    struct edge edges[3];
    Float y[2];
    /* @hack type: i, first and second as Sint8, where int would do */
    Sint8 i;
    Sint8 first;
    Sint8 second;

    first = -1;
    second = -1;
    edge_set_zy(&edges[0], p0, p1);
    edge_set_zy(&edges[1], p1, p2);
    edge_set_zy(&edges[2], p2, p0);
    edges_test_parallel(edges);
    for (i = 0; i < 3; i++) {
        if (edges[i].from.x <= point[2] && edges[i].to.x > point[2]) {
            if (first < 0) {
                first = i;
            } else {
                second = i;
                break;
            }
        }
    }
    if (second < 0) {
        return FALSE;
    }
    y[0] = (edges[first].slope * point[2]) + edges[first].intercept;
    y[1] = (edges[second].slope * point[2]) + edges[second].intercept;
    if (y[0] < y[1]) {
        if (point[1] >= y[0] && point[1] < y[1]) {
            return TRUE;
        }
    } else {
        if (point[1] < y[0] && point[1] >= y[1]) {
            return TRUE;
        }
    }
    return FALSE;
}

Bool camera_probe_on_triangle(NJS_POINT3 *corners, NJS_POINT3 *point,
                              NJS_LINE *line)
{
    NJS_POINT3 middle;
    NJS_VECTOR from_middle;
    NJS_VECTOR half;
    Bool result;

    middle.x = line->px + (line->vx * 0.5F);
    middle.y = line->py + (line->vy * 0.5F);
    middle.z = line->pz + (line->vz * 0.5F);
    half.x = line->vx * 0.5F;
    half.y = line->vy * 0.5F;
    half.z = line->vz * 0.5F;
    vector_subtract(point, &middle, &from_middle);
    if (njScalor2(&from_middle) > njScalor2(&half)) {
        return FALSE;
    }
    /* see the triangle along an axis it isn't edge-on to: one its corners
     * all share, else y. two corners on one vertical line make y's view
     * edge-on. so do edges whose slopes come out parallel in it */
    if (njAbs(corners[0].x - corners[1].x) < FLAT_SLACK &&
        njAbs(corners[1].x - corners[2].x) < FLAT_SLACK) {
        return triangle_holds_zy((Float *)&corners[0], (Float *)&corners[1],
                                 (Float *)&corners[2], &point->x);
    } else if (njAbs(corners[0].y - corners[1].y) < FLAT_SLACK &&
               njAbs(corners[1].y - corners[2].y) < FLAT_SLACK) {
        return triangle_holds_xz((Float *)&corners[0], (Float *)&corners[1],
                                 (Float *)&corners[2], &point->x);
    } else if (njAbs(corners[0].z - corners[1].z) < FLAT_SLACK &&
               njAbs(corners[1].z - corners[2].z) < FLAT_SLACK) {
        return triangle_holds_xy((Float *)&corners[0], (Float *)&corners[1],
                                 (Float *)&corners[2], &point->x);
    } else if ((njAbs(corners[0].x - corners[1].x) < FLAT_SLACK &&
                njAbs(corners[0].z - corners[1].z) < FLAT_SLACK) ||
               (njAbs(corners[0].x - corners[2].x) < FLAT_SLACK &&
                njAbs(corners[0].z - corners[2].z) < FLAT_SLACK) ||
               (njAbs(corners[1].x - corners[2].x) < FLAT_SLACK &&
                njAbs(corners[1].z - corners[2].z) < FLAT_SLACK)) {
        if ((njAbs(corners[0].x - corners[1].x) < FLAT_SLACK &&
             njAbs(corners[0].y - corners[1].y) < FLAT_SLACK) ||
            (njAbs(corners[0].x - corners[2].x) < FLAT_SLACK &&
             njAbs(corners[0].y - corners[2].y) < FLAT_SLACK) ||
            (njAbs(corners[1].x - corners[2].x) < FLAT_SLACK &&
             njAbs(corners[1].y - corners[2].y) < FLAT_SLACK)) {
            return triangle_holds_zy((Float *)&corners[0], (Float *)&corners[1],
                                     (Float *)&corners[2], &point->x);
        } else {
            return triangle_holds_xy((Float *)&corners[0], (Float *)&corners[1],
                                     (Float *)&corners[2], &point->x);
        }
    } else {
        result = triangle_holds_xz((Float *)&corners[0], (Float *)&corners[1],
                                   (Float *)&corners[2], &point->x);
        if (edges_parallel) {
            return triangle_holds_xy((Float *)&corners[0], (Float *)&corners[1],
                                     (Float *)&corners[2], &point->x);
        }
        return result;
    }
}

/* copy into faces each contact of the ball's last sweep that pos isn't
 * behind. where probe's line crosses any of them, set crossing to the
 * crossing nearest pos, and return TRUE. otherwise return FALSE, with
 * crossing's pos at next and the rest of crossing as it was */
static Bool probe_cross_contacts(struct camera_probe *probe,
                                 struct crossing *crossing)
{
    NJS_LINE line;
    NJS_PLANE plane;
    NJS_POINT3 point;
    NJS_VECTOR to_point;
    Bool result;
    int i;
    Float best;
    Float length;

    best = FARTHEST2;
    crossing->pos = probe->next;
    result = FALSE;
    search[SEARCH_TESTED] = 0;
    search[SEARCH_HELD] = 0;
    search[SEARCH_NEARER] = 0;
    search[SEARCH_NEAREST] = -1;
    for (i = 0; i < ball.count; i++) {
        if (polygon_contacts[i].height < 0.0F) {
            continue;
        }
        faces[face_count].contact = polygon_contacts[i];
        faces[face_count].corners[0] = polygon_contact_corners[i][0];
        faces[face_count].corners[1] = polygon_contact_corners[i][1];
        faces[face_count].corners[2] = polygon_contact_corners[i][2];
        face_count++;
        if (camera_probe_debug) {
            face_draw(polygon_contact_corners[i],
                      &polygon_contacts[i].hit.front, 0);
        }
        plane.px = polygon_contact_corners[i][0].x;
        plane.py = polygon_contact_corners[i][0].y;
        plane.pz = polygon_contact_corners[i][0].z;
        plane.vx = polygon_contacts[i].hit.front.x;
        plane.vy = polygon_contacts[i].hit.front.y;
        plane.vz = polygon_contacts[i].hit.front.z;
        line.px = probe->pos.x;
        line.py = probe->pos.y;
        line.pz = probe->pos.z;
        line.vx = probe->dir.x;
        line.vy = probe->dir.y;
        line.vz = probe->dir.z;
        njDistanceL2PL(&line, &plane, &point);
        search[SEARCH_TESTED]++;
        if (camera_probe_on_triangle(polygon_contact_corners[i], &point,
                                     &line)) {
            vector_subtract(&point, &probe->pos, &to_point);
            search[SEARCH_HELD]++;
            /* @hack redundant: the store of the height, which the test
             * below overwrites */
            length = polygon_contacts[i].height;
            if (best > (length = njScalor2(&to_point))) {
                /* @hack spelling: += 1, where ++ would do */
                search[SEARCH_NEARER] += 1;
                best = length;
                crossing->pos = point;
                result = TRUE;
                search[SEARCH_NEAREST] = i;
                crossing->ang.x = polygon_contacts[i].hit.ang_x;
                crossing->ang.y = 0;
                crossing->ang.z = polygon_contacts[i].hit.ang_z;
                crossing->normal = polygon_contacts[i].hit.front;
                njUnitVector(&crossing->normal);
                crossing->corners[0] = polygon_contact_corners[i][0];
                crossing->corners[1] = polygon_contact_corners[i][1];
                crossing->corners[2] = polygon_contact_corners[i][2];
                crossed_face = face_count - 1;
            }
        }
    }
    /* @hack redundant: a read of the first face's first corner, which
     * nothing uses */
    length = polygon_contact_corners[0][0].x;
    if (camera_probe_debug) {
        njPrintD(NJM_LOCATION(PRINT_COLUMN, CAST_ROW + SEARCH_TESTED),
                 search[SEARCH_TESTED], PRINT_DIGITS);
        njPrintD(NJM_LOCATION(PRINT_COLUMN, CAST_ROW + SEARCH_HELD),
                 search[SEARCH_HELD], PRINT_DIGITS);
        njPrintD(NJM_LOCATION(PRINT_COLUMN, CAST_ROW + SEARCH_NEARER),
                 search[SEARCH_NEARER], PRINT_DIGITS);
        if (search[SEARCH_NEAREST] != -1) {
            face_draw(polygon_contact_corners[search[SEARCH_NEAREST]],
                      &polygon_contacts[search[SEARCH_NEAREST]].hit.front, 1);
        }
    }
    return result;
}

/* probe_cross_contacts with probe's line made its radius longer. each
 * crossing then moves back along the line by the radius */
static Bool probe_cross_contacts_long(struct camera_probe *probe,
                                      struct crossing *crossing)
{
    NJS_LINE line;
    NJS_PLANE plane;
    NJS_POINT3 point;
    NJS_VECTOR ahead;
    NJS_VECTOR to_point;
    Bool result;
    int i;
    Float best;
    Float length;

    best = FARTHEST2;
    crossing->pos = probe->next;
    result = FALSE;
    search[SEARCH_TESTED] = 0;
    search[SEARCH_HELD] = 0;
    search[SEARCH_NEARER] = 0;
    search[SEARCH_NEAREST] = -1;
    for (i = 0; i < ball.count; i++) {
        if (polygon_contacts[i].height < 0.0F) {
            continue;
        }
        faces[face_count].contact = polygon_contacts[i];
        faces[face_count].corners[0] = polygon_contact_corners[i][0];
        faces[face_count].corners[1] = polygon_contact_corners[i][1];
        faces[face_count].corners[2] = polygon_contact_corners[i][2];
        face_count++;
        if (camera_probe_debug) {
            face_draw(polygon_contact_corners[i],
                      &polygon_contacts[i].hit.front, 0);
        }
        plane.px = polygon_contact_corners[i][0].x;
        plane.py = polygon_contact_corners[i][0].y;
        plane.pz = polygon_contact_corners[i][0].z;
        plane.vx = polygon_contacts[i].hit.front.x;
        plane.vy = polygon_contacts[i].hit.front.y;
        plane.vz = polygon_contacts[i].hit.front.z;
        line.px = probe->pos.x;
        line.py = probe->pos.y;
        line.pz = probe->pos.z;
        line.vx = probe->dir.x;
        line.vy = probe->dir.y;
        line.vz = probe->dir.z;
        ahead.x = line.vx;
        ahead.y = line.vy;
        ahead.z = line.vz;
        njUnitVector(&ahead);
        ahead.x *= probe->radius;
        ahead.y *= probe->radius;
        ahead.z *= probe->radius;
        line.vx += ahead.x;
        line.vy += ahead.y;
        line.vz += ahead.z;
        njDistanceL2PL(&line, &plane, &point);
        search[SEARCH_TESTED]++;
        if (camera_probe_on_triangle(polygon_contact_corners[i], &point,
                                     &line)) {
            vector_subtract(&point, &ahead, &point);
            vector_subtract(&point, &probe->pos, &to_point);
            search[SEARCH_HELD]++;
            /* @hack redundant: the store of the height, which the test
             * below overwrites */
            length = polygon_contacts[i].height;
            if (best > (length = njScalor2(&to_point))) {
                /* @hack spelling: += 1, where ++ would do */
                search[SEARCH_NEARER] += 1;
                best = length;
                crossing->pos = point;
                result = TRUE;
                search[SEARCH_NEAREST] = i;
                crossing->ang.x = polygon_contacts[i].hit.ang_x;
                crossing->ang.y = 0;
                crossing->ang.z = polygon_contacts[i].hit.ang_z;
                crossing->normal = polygon_contacts[i].hit.front;
                njUnitVector(&crossing->normal);
                crossing->corners[0] = polygon_contact_corners[i][0];
                crossing->corners[1] = polygon_contact_corners[i][1];
                crossing->corners[2] = polygon_contact_corners[i][2];
                crossed_face = face_count - 1;
            }
        }
    }
    /* @hack redundant: a read of the first face's first corner, which
     * nothing uses */
    length = polygon_contact_corners[0][0].x;
    if (camera_probe_debug) {
        njPrintD(NJM_LOCATION(PRINT_COLUMN, LONG_ROW + SEARCH_TESTED),
                 search[SEARCH_TESTED], PRINT_DIGITS);
        njPrintD(NJM_LOCATION(PRINT_COLUMN, LONG_ROW + SEARCH_HELD),
                 search[SEARCH_HELD], PRINT_DIGITS);
        njPrintD(NJM_LOCATION(PRINT_COLUMN, LONG_ROW + SEARCH_NEARER),
                 search[SEARCH_NEARER], PRINT_DIGITS);
        if (search[SEARCH_NEAREST] != -1) {
            face_draw(polygon_contact_corners[search[SEARCH_NEAREST]],
                      &polygon_contacts[search[SEARCH_NEAREST]].hit.front, 1);
        }
    }
    return result;
}

Bool camera_probe_slide_hit(struct camera_probe *probe)
{
    NJS_PLANE wall;
    NJS_POINT3 foot;
    NJS_VECTOR along;
    NJS_POINT3 slid;
    NJS_POINT3 point;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR normal;
    Bool crossed;
    int i;
    Float best;

    crossed = FALSE;
    wall.px = probe->hit.x;
    wall.py = probe->hit.y;
    wall.pz = probe->hit.z;
    wall.vx = probe->normal.x;
    wall.vy = probe->normal.y;
    wall.vz = probe->normal.z;
    njDistanceP2PL(&probe->next, &wall, &foot);
    vector_subtract(&foot, &probe->hit, &along);
    /* @bug logic: draws the normal, the mark and SLIDE! whatever
     * camera_probe_debug holds. it tests face_draw's address, which is never
     * 0, here and twice below (TST at 0x8c0b1fc4, 0x8c0b2152 and
     * 0x8c0b2168) */
    /* NOLINTNEXTLINE(clang-diagnostic-pointer-bool-conversion) */
    if (face_draw) {
        NJS_POINT3 ends[2];

        ends[0] = probe->hit;
        ends[1].x = probe->hit.x + (probe->normal.x * NORMAL_LENGTH);
        ends[1].y = probe->hit.y + (probe->normal.y * NORMAL_LENGTH);
        ends[1].z = probe->hit.z + (probe->normal.z * NORMAL_LENGTH);
        line_draw_points(&ends[0], &ends[1]);
    }
    vector_add(&probe->hit, &along, &slid);
    best = SLIDE_FARTHEST2;
    for (i = 0; i < face_count; i++) {
        if (i != crossed_face) {
            NJS_LINE line;
            NJS_PLANE plane;
            NJS_VECTOR to_point;

            plane.px = faces[i].corners[0].x;
            plane.py = faces[i].corners[0].y;
            plane.pz = faces[i].corners[0].z;
            plane.vx = faces[i].contact.hit.front.x;
            plane.vy = faces[i].contact.hit.front.y;
            plane.vz = faces[i].contact.hit.front.z;
            line.px = probe->hit.x;
            line.py = probe->hit.y;
            line.pz = probe->hit.z;
            line.vx = along.x;
            line.vy = along.y;
            line.vz = along.z;
            njDistanceL2PL(&line, &plane, &point);
            if (camera_probe_on_triangle(faces[i].corners, &point, &line)) {
                vector_subtract(&point, &probe->hit, &to_point);
                if (best > njScalor2(&to_point)) {
                    /* @bug logic: keeps the face's index as the best
                     * squared distance, where the distance just tested was
                     * meant (FLOAT at 0x8c0b2118). a later face then wins
                     * only where its squared distance from hit is below
                     * that index */
                    best = (Float)i;
                    pos = point;
                    ang.x = faces[i].contact.hit.ang_x;
                    ang.y = 0;
                    ang.z = faces[i].contact.hit.ang_z;
                    normal = faces[i].contact.hit.front;
                    crossed = TRUE;
                }
            }
        }
    }
    /* NOLINTNEXTLINE(clang-diagnostic-pointer-bool-conversion) */
    if (face_draw) {
        debug_draw_mark(&probe->hit, MARK_SIZE);
    }
    if (crossed) {
        /* NOLINTNEXTLINE(clang-diagnostic-pointer-bool-conversion) */
        if (face_draw) {
            njPrintC(PRINT_SLIDE, "SLIDE!");
        }
        probe->hit = pos;
        probe->ang = ang;
        probe->normal = normal;
    } else {
        probe->hit = slid;
    }
    return crossed;
}

Bool camera_probe_cast_long(struct camera_probe *probe)
{
    NJS_VECTOR speed;
    NJS_VECTOR to_place;
    struct crossing crossing;
    Bool result;
    Bool crossed;
    Float best;
    Float length;
    int i;

    best = FARTHEST;
    face_count = 0;
    probe_start(probe);
    probe->hit = probe->next;
    result = FALSE;
    if (njScalor2(&probe->dir) > (CAMERA_PROBE_REACH * CAMERA_PROBE_REACH)) {
        return FALSE;
    }
    probe_sweep_start(probe);
    for (i = 0; i < world_count; i++) {
        if (world_objects[i].flags & probe->skip) {
            continue;
        }
        if (polygon_sweep(&ball, world_objects[i].object,
                          world_objects[i].flags, TRUE)) {
            speed.x = 0.0F;
            speed.y = 0.0F;
            speed.z = 0.0F;
            if (world_objects[i].owner != NULL) {
                struct task_force *force;

                force = world_objects[i].owner->force.p;
                if (force != NULL) {
                    speed = force->speed;
                }
            }
            /* probe_cross_contacts_long below replaces crossing.pos before
             * anything reads the sum, as in probe_cast */
            vector_add(&speed, &crossing.pos, &crossing.pos);
#ifdef NONMATCHING
            /* GCC can't see that crossed is set wherever it is read, as in
             * probe_cast */
            crossed = FALSE;
#endif
            if (ball.count > 0) {
                crossed = probe_cross_contacts_long(probe, &crossing);
            }
            vector_subtract(&crossing.pos, &probe->pos, &to_place);
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((length = njScalor(&to_place)) < best) {
                best = length;
                probe->hit = crossing.pos;
                probe->ang = crossing.ang;
                probe->normal = crossing.normal;
                probe->flags = world_objects[i].flags;
                if (crossed) {
                    result = TRUE;
                }
            }
        }
    }
    vector_subtract(&probe->hit, &probe->pos, &probe->moved);
    return result;
}

/* copy into faces the contacts of the ball's last sweep, until faces holds
 * MOVE_FACES. faces_take_fronts copies those the ball's pos isn't behind, and
 * faces_take_all every one. each returns -1 where it stops at MOVE_FACES
 * with contacts left, else 0 */
static int faces_take_fronts(void)
{
    NJS_VECTOR normal;
    int i;

    for (i = 0; i < ball.count; i++) {
        if (face_count >= MOVE_FACES) {
            return -1;
        }
        if (polygon_contacts[i].height < 0.0F) {
            continue;
        }
        faces[face_count].contact = polygon_contacts[i];
        faces[face_count].corners[0] = polygon_contact_corners[i][0];
        faces[face_count].corners[1] = polygon_contact_corners[i][1];
        faces[face_count].corners[2] = polygon_contact_corners[i][2];
        face_count++;
        if (camera_probe_debug) {
            normal = polygon_contacts[i].hit.front;
            normal.x *= NORMAL_LENGTH;
            normal.y *= NORMAL_LENGTH;
            normal.z *= NORMAL_LENGTH;
            face_draw(polygon_contact_corners[i], &normal, 0);
        }
    }
    return 0;
}

static int faces_take_all(void)
{
    NJS_VECTOR normal;
    int i;

    for (i = 0; i < ball.count; i++) {
        if (face_count >= MOVE_FACES) {
            return -1;
        }
        faces[face_count].contact = polygon_contacts[i];
        faces[face_count].corners[0] = polygon_contact_corners[i][0];
        faces[face_count].corners[1] = polygon_contact_corners[i][1];
        faces[face_count].corners[2] = polygon_contact_corners[i][2];
        face_count++;
        if (camera_probe_debug) {
            normal = polygon_contacts[i].hit.front;
            normal.x *= NORMAL_LENGTH;
            normal.y *= NORMAL_LENGTH;
            normal.z *= NORMAL_LENGTH;
            face_draw(polygon_contact_corners[i], &normal, 0);
        }
    }
    return 0;
}

/* one slide of a move, from *pos for *next, as camera_probe_move says.
 * where a face stops the sphere, it returns TRUE. hit is then where the
 * sphere slides to, and probe's slides[index] holds the face. otherwise it
 * returns FALSE and leaves hit as it was */
static Bool probe_slide(NJS_POINT3 *pos, NJS_POINT3 *next,
                        struct camera_probe *probe, int index)
{
    struct {
        NJS_POINT3 point;
        Float distance;
        Sint32 kind;
        Sint32 face;
    } best;
    NJS_PLANE plane;
    NJS_LINE line;
    NJS_POINT3 point;
    NJS_VECTOR ahead;
    NJS_VECTOR to_point;
    struct camera_probe_slide *slide;
    Float radius;
    Float distance;
    int i;
    int face;

    radius = probe->radius;
    slide = &probe->slides[index];
#ifdef NONMATCHING
    /* GCC can't see that best.face is set wherever it is read. best.kind leaves
     * SLIDE_NONE only where the loop sets the face as well */
    best.face = 0;
#endif
    best.kind = SLIDE_NONE;
    for (i = 0; i < face_count; i++) {
        if (i == probe->slid_face) {
            continue;
        }
        plane.px = faces[i].corners[0].x;
        plane.py = faces[i].corners[0].y;
        plane.pz = faces[i].corners[0].z;
        plane.vx = faces[i].contact.hit.front.x;
        plane.vy = faces[i].contact.hit.front.y;
        plane.vz = faces[i].contact.hit.front.z;
        line.px = pos->x;
        line.py = pos->y;
        line.pz = pos->z;
        vector_subtract(next, pos, (NJS_VECTOR *)&line.vx);
        ahead = *(NJS_VECTOR *)&line.vx;
        njUnitVector(&ahead);
        line.vx += ahead.x * radius;
        line.vy += ahead.y * radius;
        line.vz += ahead.z * radius;
        njDistanceL2PL(&line, &plane, &point);
        if (camera_probe_on_triangle(faces[i].corners, &point, &line)) {
            vector_subtract(pos, &point, &to_point);
            if (best.kind != SLIDE_NONE) {
                distance = njScalor(&to_point);
                if (best.distance > distance) {
                    best.point = point;
                    best.kind = SLIDE_CROSSED;
                    best.face = i;
                    best.distance = distance;
                }
            } else {
                best.point = point;
                best.kind = SLIDE_CROSSED;
                best.face = i;
                best.distance = njScalor(&to_point);
            }
        } else if (radius > njDistanceP2PL(next, &plane, &point)) {
            /* the segment from next through point and as far again. the
             * sphere on that segment holds point, so only the triangle test
             * decides */
            line.px = next->x;
            line.py = next->y;
            line.pz = next->z;
            line.vx = (point.x - next->x) * 2.0F;
            line.vy = (point.y - next->y) * 2.0F;
            line.vz = (point.z - next->z) * 2.0F;
            if (camera_probe_on_triangle(faces[i].corners, &point, &line)) {
                vector_subtract(pos, &point, &to_point);
                if (best.kind != SLIDE_NONE) {
                    distance = njScalor(&to_point);
                    if (best.distance > distance) {
                        best.point = point;
                        best.kind = SLIDE_NEAR;
                        best.face = i;
                        best.distance = distance;
                    }
                } else {
                    best.point = point;
                    best.kind = SLIDE_NEAR;
                    best.face = i;
                    best.distance = njScalor(&to_point);
                    /* @hack redundant: the continue at the loop's end */
                    continue;
                }
            }
        }
    }
    switch (best.kind) {
    case SLIDE_NONE:
        probe->slid_face = -1;
        return FALSE;
        /* @hack redundant: the break after the return */
        break;
    case SLIDE_CROSSED:
    case SLIDE_NEAR:
        face = best.face;
        plane.px = faces[face].corners[0].x;
        plane.py = faces[face].corners[0].y;
        plane.pz = faces[face].corners[0].z;
        plane.vx = faces[face].contact.hit.front.x;
        plane.vy = faces[face].contact.hit.front.y;
        plane.vz = faces[face].contact.hit.front.z;
        njUnitVector((NJS_VECTOR *)&plane.vx);
        slide->normal = *(NJS_VECTOR *)&plane.vx;
        plane.px += plane.vx * radius;
        plane.py += plane.vy * radius;
        plane.pz += plane.vz * radius;
        njDistanceP2PL(next, &plane, &probe->hit);
        slide->ang.x = faces[face].contact.hit.ang_x;
        slide->ang.y = 0;
        slide->ang.z = faces[face].contact.hit.ang_z;
        probe->slid_face = face;
        return TRUE;
        /* @hack redundant: the break after the return */
        break;
    /* @hack redundant: the case of SLIDE_UNK_2, which no slide takes */
    case SLIDE_UNK_2:
        probe->slid_face = i;
        break;
    }
#ifdef NONMATCHING
    /* GCC can't see that the switch returns: best.kind is only ever
     * SLIDE_NONE, SLIDE_CROSSED or SLIDE_NEAR */
    return FALSE;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static int probe_slide_all(struct camera_probe *probe)
{
    NJS_POINT3 pos;
    NJS_POINT3 next;
    int count;
    int i;

    count = 0;
    pos = probe->pos;
    next = probe->next;
    for (i = 0; i < CAMERA_PROBE_SLIDES; i++) {
        if (!probe_slide(&pos, &next, probe, i)) {
            probe->hit = next;
            break;
        }
        next = probe->hit;
        count++;
    }
    return count;
}

int camera_probe_move(struct camera_probe *probe)
{
    int i;

    face_count = 0;
    probe_start(probe);
    probe->hit = probe->next;
    if (njScalor2(&probe->dir) > (CAMERA_PROBE_REACH * CAMERA_PROBE_REACH)) {
        return 0;
    }
    probe_sweep_start(probe);
    for (i = 0; i < world_count; i++) {
        if (world_objects[i].flags & probe->skip) {
            continue;
        }
        if (polygon_sweep(&ball, world_objects[i].object,
                          world_objects[i].flags, TRUE)) {
            faces_take_fronts();
        }
    }
    return probe_slide_all(probe);
}

int camera_probe_move_two_sided(struct camera_probe *probe)
{
    int i;

    face_count = 0;
    probe_start(probe);
    probe->hit = probe->next;
    if (njScalor2(&probe->dir) > (CAMERA_PROBE_REACH * CAMERA_PROBE_REACH)) {
        return 0;
    }
    probe_sweep_start(probe);
    for (i = 0; i < world_count; i++) {
        if (world_objects[i].flags & probe->skip) {
            continue;
        }
        if (polygon_sweep(&ball, world_objects[i].object,
                          world_objects[i].flags, TRUE)) {
            faces_take_all();
        }
    }
    return probe_slide_all(probe);
}
