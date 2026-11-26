/* 2D polygons, filled or outlined */
#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_km.h>
#include <sg_xpt.h>

/* a vertex's 32 bytes as the strips send them. pcw is a KM_VERTEXPARAM_
 * value, z the depth and col the packed color. the strips leave offset
 * unwritten */
struct vertex {
    Uint32 pcw;
    Float x, y, z;
    Float u, v;
    Uint32 col;
    Uint32 offset;
};
/* the words of a vertex, and of its u and v. an untextured vertex leaves its
 * u and v unwritten */
#define VERTEX_WORDS (sizeof(struct vertex) / sizeof(Uint32))
#define UV_WORDS 2

/* @hack spelling: a vertex's words stored as polygon2d_sq steps back from
 * its end, where indexed stores from its start would do */
#define SQ_PUT(type, value) (*(type *)--polygon2d_sq = (value))

/* half the edge width, in pixels */
#define HALF_WIDTH 0.525F

/* NJS_COLOR's tex.u and tex.v count 256 to the texture's side */
#define TEX_SCALE (1.0F / 256.0F)

/* the most corners a filled polygon may have */
#define CORNERS 32
/* an edge's ends, which an outline's corners take turns in. the slot after
 * them holds a copy of the outline's corner 0 */
#define EDGE_ENDS 2
/* plain and textured, the kinds polygon2d_begin holds. polygon2d_draw holds
 * the outlines' two kinds, then the fills' */
#define KINDS 2

static void polygon2d_begin_plain(Float pri, Int trans);
static void polygon2d_begin_textured(Float pri, Int trans);
static void polygon2d_outline_plain(NJS_POINT2COL *p, Int n);
static void polygon2d_outline_textured(NJS_POINT2COL *p, Int n);
static void polygon2d_fill_plain(NJS_POINT2COL *p, Int n);
static void polygon2d_fill_textured(NJS_POINT2COL *p, Int n);

static void (*polygon2d_begin[])(Float pri, Int trans) = {
    polygon2d_begin_plain, polygon2d_begin_textured};
static void (*polygon2d_draw[])(NJS_POINT2COL *p, Int n) = {
    polygon2d_outline_plain, polygon2d_outline_textured, polygon2d_fill_plain,
    polygon2d_fill_textured};
static Uint32 polygon2d_plain_modes[] = {NJD_COLOR_MODE_PACKED_NORMAL,
                                         NJD_COLOR_MODE_PACKED_NORMAL_TRANS};
static Uint32 polygon2d_textured_modes[] = {
    NJD_COLOR_MODE_PACKED_TEXTURE, NJD_COLOR_MODE_PACKED_TEXTURE_TRANS};
static Float polygon2d_depth;
/* where the next vertex starts in the store queue */
static Uint32 *polygon2d_sq;
/* an outline's corners: the two its edge joins, then a copy of its corner 0 */
static struct vertex polygon2d_plain_edge[EDGE_ENDS + 1];
static struct vertex polygon2d_textured_edge[EDGE_ENDS + 1];

static void polygon2d_begin_plain(Float pri, Int trans)
{
    njSetkmColorMode(polygon2d_plain_modes[trans]);
    if (trans) {
        polygon2d_sq = njStartVertexNonTexTrans();
    } else {
        polygon2d_sq = njStartVertexNonTexOpaque();
    }
    polygon2d_depth = pri < -1.0F ? -1.0F / pri : 1.0F;
}

static void polygon2d_begin_textured(Float pri, Int trans)
{
    njSetkmColorMode(polygon2d_textured_modes[trans]);
    if (trans) {
        polygon2d_sq = njStartVertexTexTrans();
    } else {
        polygon2d_sq = njStartVertexTexOpaque();
    }
    polygon2d_depth = pri < -1.0F ? -1.0F / pri : 1.0F;
}

/* send the edge from a to b as a strip of four vertices, HALF_WIDTH to either
 * side of it */
static void polygon2d_edge_plain(struct vertex *a, struct vertex *b)
{
    Float dx;
    Float dy;
    Float inv_length;
    Float side_x;
    Float side_y;

    dx = b->x - a->x;
    dy = b->y - a->y;
    inv_length = 1.0F / _builtin_sqrtf((dx * dx) + (dy * dy));
    side_x = dy * inv_length * HALF_WIDTH;
    side_y = -(dx * inv_length * HALF_WIDTH);
    polygon2d_sq += VERTEX_WORDS - 1;
    SQ_PUT(Uint32, a->col);
    polygon2d_sq -= UV_WORDS;
    SQ_PUT(Float, polygon2d_depth);
    SQ_PUT(Float, a->y + side_y);
    SQ_PUT(Float, a->x + side_x);
    SQ_PUT(Uint32, KM_VERTEXPARAM_NORMAL);
    prefetch(polygon2d_sq);
    polygon2d_sq += VERTEX_WORDS;
    polygon2d_sq += VERTEX_WORDS - 1;
    SQ_PUT(Uint32, a->col);
    polygon2d_sq -= UV_WORDS;
    SQ_PUT(Float, polygon2d_depth);
    SQ_PUT(Float, a->y - side_y);
    SQ_PUT(Float, a->x - side_x);
    SQ_PUT(Uint32, KM_VERTEXPARAM_NORMAL);
    prefetch(polygon2d_sq);
    polygon2d_sq += VERTEX_WORDS;
    polygon2d_sq += VERTEX_WORDS - 1;
    SQ_PUT(Uint32, b->col);
    polygon2d_sq -= UV_WORDS;
    SQ_PUT(Float, polygon2d_depth);
    SQ_PUT(Float, b->y + side_y);
    SQ_PUT(Float, b->x + side_x);
    SQ_PUT(Uint32, KM_VERTEXPARAM_NORMAL);
    prefetch(polygon2d_sq);
    polygon2d_sq += VERTEX_WORDS;
    polygon2d_sq += VERTEX_WORDS - 1;
    SQ_PUT(Uint32, b->col);
    polygon2d_sq -= UV_WORDS;
    SQ_PUT(Float, polygon2d_depth);
    SQ_PUT(Float, b->y - side_y);
    SQ_PUT(Float, b->x - side_x);
    SQ_PUT(Uint32, KM_VERTEXPARAM_ENDOFSTRIP);
    prefetch(polygon2d_sq);
    polygon2d_sq += VERTEX_WORDS;
}

static void polygon2d_edge_textured(struct vertex *a, struct vertex *b)
{
    Float dx;
    Float dy;
    Float inv_length;
    Float side_x;
    Float side_y;

    dx = b->x - a->x;
    dy = b->y - a->y;
    inv_length = 1.0F / _builtin_sqrtf((dx * dx) + (dy * dy));
    side_x = dy * inv_length * HALF_WIDTH;
    side_y = -(dx * inv_length * HALF_WIDTH);
    polygon2d_sq += VERTEX_WORDS - 1;
    SQ_PUT(Uint32, a->col);
    SQ_PUT(Float, a->v);
    SQ_PUT(Float, a->u);
    SQ_PUT(Float, polygon2d_depth);
    SQ_PUT(Float, a->y + side_y);
    SQ_PUT(Float, a->x + side_x);
    SQ_PUT(Uint32, KM_VERTEXPARAM_NORMAL);
    prefetch(polygon2d_sq);
    polygon2d_sq += VERTEX_WORDS;
    polygon2d_sq += VERTEX_WORDS - 1;
    SQ_PUT(Uint32, a->col);
    SQ_PUT(Float, a->v);
    SQ_PUT(Float, a->u);
    SQ_PUT(Float, polygon2d_depth);
    SQ_PUT(Float, a->y - side_y);
    SQ_PUT(Float, a->x - side_x);
    SQ_PUT(Uint32, KM_VERTEXPARAM_NORMAL);
    prefetch(polygon2d_sq);
    polygon2d_sq += VERTEX_WORDS;
    polygon2d_sq += VERTEX_WORDS - 1;
    SQ_PUT(Uint32, b->col);
    SQ_PUT(Float, b->v);
    SQ_PUT(Float, b->u);
    SQ_PUT(Float, polygon2d_depth);
    SQ_PUT(Float, b->y + side_y);
    SQ_PUT(Float, b->x + side_x);
    SQ_PUT(Uint32, KM_VERTEXPARAM_NORMAL);
    prefetch(polygon2d_sq);
    polygon2d_sq += VERTEX_WORDS;
    polygon2d_sq += VERTEX_WORDS - 1;
    SQ_PUT(Uint32, b->col);
    SQ_PUT(Float, b->v);
    SQ_PUT(Float, b->u);
    SQ_PUT(Float, polygon2d_depth);
    SQ_PUT(Float, b->y - side_y);
    SQ_PUT(Float, b->x - side_x);
    SQ_PUT(Uint32, KM_VERTEXPARAM_ENDOFSTRIP);
    prefetch(polygon2d_sq);
    polygon2d_sq += VERTEX_WORDS;
}

/* send a strip of n triangles from the first n + 2 points of p */
static void polygon2d_strip_textured(NJS_POINT2COL *p, Int n)
{
    Int i;

    /* @hack spelling: n raised before the loop, where for (i = 0; i <= n;
     * i++) would do */
    n++;
    for (i = 0; i < n; i++) {
        polygon2d_sq += VERTEX_WORDS - 1;
        SQ_PUT(Uint32, p->col[i].color);
        SQ_PUT(Float, p->tex[i].tex.v * TEX_SCALE);
        SQ_PUT(Float, p->tex[i].tex.u * TEX_SCALE);
        SQ_PUT(Float, polygon2d_depth);
        SQ_PUT(Float, p->p[i].y);
        SQ_PUT(Float, p->p[i].x);
        SQ_PUT(Uint32, KM_VERTEXPARAM_NORMAL);
        prefetch(polygon2d_sq);
        polygon2d_sq += VERTEX_WORDS;
    }
    polygon2d_sq += VERTEX_WORDS - 1;
    SQ_PUT(Uint32, p->col[i].color);
    SQ_PUT(Float, p->tex[i].tex.v * TEX_SCALE);
    SQ_PUT(Float, p->tex[i].tex.u * TEX_SCALE);
    SQ_PUT(Float, polygon2d_depth);
    SQ_PUT(Float, p->p[i].y);
    SQ_PUT(Float, p->p[i].x);
    SQ_PUT(Uint32, KM_VERTEXPARAM_ENDOFSTRIP);
    prefetch(polygon2d_sq);
    polygon2d_sq += VERTEX_WORDS;
}

static void polygon2d_strip_plain(NJS_POINT2COL *p, Int n)
{
    Int i;

    /* @hack spelling: n raised before the loop, where for (i = 0; i <= n;
     * i++) would do */
    n++;
    for (i = 0; i < n; i++) {
        polygon2d_sq += VERTEX_WORDS - 1;
        SQ_PUT(Uint32, p->col[i].color);
        polygon2d_sq -= UV_WORDS;
        SQ_PUT(Float, polygon2d_depth);
        SQ_PUT(Float, p->p[i].y);
        SQ_PUT(Float, p->p[i].x);
        SQ_PUT(Uint32, KM_VERTEXPARAM_NORMAL);
        prefetch(polygon2d_sq);
        polygon2d_sq += VERTEX_WORDS;
    }
    polygon2d_sq += VERTEX_WORDS - 1;
    SQ_PUT(Uint32, p->col[i].color);
    polygon2d_sq -= UV_WORDS;
    SQ_PUT(Float, polygon2d_depth);
    SQ_PUT(Float, p->p[i].y);
    SQ_PUT(Float, p->p[i].x);
    SQ_PUT(Uint32, KM_VERTEXPARAM_ENDOFSTRIP);
    prefetch(polygon2d_sq);
    polygon2d_sq += VERTEX_WORDS;
}

static void polygon2d_fill_plain(NJS_POINT2COL *p, Int n)
{
    NJS_POINT2COL strip;
    NJS_POINT2 points[CORNERS];
    NJS_COLOR colors[CORNERS];
    Int front;
    Int back;
    Int i;

    /* @hack spelling: front and i zeroed in one chained assignment, where
     * two statements would do */
    front = i = 0;
    back = n - 1;
    while (front < ((n + 1) >> 1)) {
        points[i] = p->p[front];
        colors[i++].color = p->col[front++].color;
        points[i] = p->p[back];
        colors[i++].color = p->col[back--].color;
    }
    strip.p = points;
    strip.col = colors;
    polygon2d_strip_plain(&strip, n - 2);
}

static void polygon2d_fill_textured(NJS_POINT2COL *p, Int n)
{
    NJS_POINT2COL strip;
    NJS_POINT2 points[CORNERS];
    NJS_COLOR colors[CORNERS];
    NJS_COLOR texs[CORNERS];
    Int front;
    Int back;
    Int i;

    /* @hack spelling: front and i zeroed in one chained assignment, where
     * two statements would do */
    front = i = 0;
    back = n - 1;
    while (front < ((n + 1) >> 1)) {
        points[i] = p->p[front];
        /* @hack spelling: u and v copied one by one, here and below, where
         * texs[i] = p->tex[front] would do */
        texs[i].tex.u = p->tex[front].tex.u;
        texs[i].tex.v = p->tex[front].tex.v;
        colors[i++].color = p->col[front++].color;
        points[i] = p->p[back];
        texs[i].tex.u = p->tex[back].tex.u;
        texs[i].tex.v = p->tex[back].tex.v;
        colors[i++].color = p->col[back--].color;
    }
    strip.p = points;
    strip.col = colors;
    strip.tex = texs;
    polygon2d_strip_textured(&strip, n - 2);
}

/* each corner from 1 on takes the place in polygon2d_plain_edge of the
 * corner two before it. each edge then joins a corner to the one before. a
 * last edge, from the last corner to the copy of corner 0, closes the
 * outline */
static void polygon2d_outline_plain(NJS_POINT2COL *p, Int n)
{
    Int i;
    Int place = 0;

    polygon2d_plain_edge[0].x = p->p[0].x;
    polygon2d_plain_edge[0].y = p->p[0].y;
    polygon2d_plain_edge[0].col = p->col[0].color;
    polygon2d_plain_edge[0].pcw = KM_VERTEXPARAM_NORMAL;
    polygon2d_plain_edge[EDGE_ENDS] = polygon2d_plain_edge[0];
    for (i = 1; i < n; i++) {
        place = i % EDGE_ENDS;
        polygon2d_plain_edge[place].x = p->p[i].x;
        polygon2d_plain_edge[place].y = p->p[i].y;
        polygon2d_plain_edge[place].col = p->col[i].color;
        polygon2d_plain_edge[place].pcw = KM_VERTEXPARAM_NORMAL;
        polygon2d_edge_plain(&polygon2d_plain_edge[0],
                             &polygon2d_plain_edge[1]);
    }
    polygon2d_edge_plain(&polygon2d_plain_edge[place],
                         &polygon2d_plain_edge[EDGE_ENDS]);
}

static void polygon2d_outline_textured(NJS_POINT2COL *p, Int n)
{
    Int i;
    Int place = 0;

    polygon2d_textured_edge[0].x = p->p[0].x;
    polygon2d_textured_edge[0].y = p->p[0].y;
    polygon2d_textured_edge[0].u = p->tex[0].tex.u * TEX_SCALE;
    polygon2d_textured_edge[0].v = p->tex[0].tex.v * TEX_SCALE;
    polygon2d_textured_edge[0].col = p->col[0].color;
    polygon2d_textured_edge[0].pcw = KM_VERTEXPARAM_NORMAL;
    polygon2d_textured_edge[EDGE_ENDS] = polygon2d_textured_edge[0];
    for (i = 1; i < n; i++) {
        place = i % EDGE_ENDS;
        polygon2d_textured_edge[place].x = p->p[i].x;
        polygon2d_textured_edge[place].y = p->p[i].y;
        polygon2d_textured_edge[place].u = p->tex[i].tex.u * TEX_SCALE;
        polygon2d_textured_edge[place].v = p->tex[i].tex.v * TEX_SCALE;
        polygon2d_textured_edge[place].col = p->col[i].color;
        polygon2d_textured_edge[place].pcw = KM_VERTEXPARAM_NORMAL;
        polygon2d_edge_textured(&polygon2d_textured_edge[0],
                                &polygon2d_textured_edge[1]);
    }
    polygon2d_edge_textured(&polygon2d_textured_edge[place],
                            &polygon2d_textured_edge[EDGE_ENDS]);
}

void njDrawPolygon2D(NJS_POINT2COL *p, Int n, Float pri, Uint32 attr)
{
    Int kind;
#ifdef NONMATCHING
    /* this side squeezes the corners under the port's 16:9, but for an
     * untextured polygon that covers the screen (engine/dc/port.h). it
     * squeezes a copy of at most CORNERS corners, and a polygon of more
     * keeps x */
    NJS_POINT2COL squeezed;
    NJS_POINT2 points[CORNERS];
    Int i;

    if (n <= CORNERS && port_wide_squeezes() &&
        ((attr & NJD_USE_TEXTURE) ||
         !port_wide_covers(&p->p->x, n, sizeof(*p->p) / sizeof(Float)))) {
        squeezed = *p;
        for (i = 0; i < n; i++) {
            points[i].x = port_wide_x(p->p[i].x);
            points[i].y = p->p[i].y;
        }
        squeezed.p = points;
        p = &squeezed;
    }
#endif

    njGetkmPointer();
    /* @hack spelling: > 0 in the three tests of attr's bits, where != 0
     * would do */
    kind = (attr & NJD_USE_TEXTURE) > 0;
    polygon2d_begin[kind](pri, (attr & NJD_TRANSPARENT) > 0);
    kind += ((attr & NJD_FILL) > 0) * KINDS;
    polygon2d_draw[kind](p, n);
    njEndVertex(polygon2d_sq);
    njSetkmPointer();
}
