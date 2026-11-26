/* the renderer's window and its strip (sg_mwrv2.h). the window's place and
 * size are in screen pixels. its image's place and size are in texture
 * coordinates, with 1.0 at the padded texture's far edge. a surface point
 * holds its depth in z, and the vertex made from it holds the reciprocal */
#include <kamui.h>
#include <sg_mwrv2.h>
#include <sg_xpt.h>

#define SAMPLE_INSET 0.25F

/* @hack spelling: the body in a bare block, where do { } while (0) would do */
#define STRETCH_WIDTH(rnv)                                                     \
    {                                                                          \
        (rnv)->window_x = 0.0F;                                                \
        (rnv)->window_width = (rnv)->display_width;                            \
        (rnv)->image_x = 0.0F;                                                 \
        (rnv)->image_width =                                                   \
            (Float)(rnv)->width / (Float)(rnv)->texture_width;                 \
    }

/* @hack spelling: the body in a bare block, where do { } while (0) would do */
#define DOUBLE_WIDTH(rnv, span)                                                \
    {                                                                          \
        (span) = (Float)((rnv)->width - 1);                                    \
        (rnv)->window_x =                                                      \
            ((rnv)->display_width - (Float)((rnv)->width * 2)) / 2.0F;         \
        (rnv)->window_width = (span) * 2.0F;                                   \
        (rnv)->image_x = SAMPLE_INSET / (Float)(rnv)->texture_width;           \
        (rnv)->image_width = (span) / (Float)(rnv)->texture_width;             \
    }

void mwRnv2SetWndDefConf(MWRNV2 rnv, Sint32 mode)
{
    Float sample_width;
    Float sample_height;
    /* @hack redundant: unused, two words that only size the frame */
    Float unused[2];

    rnv->window_z = RNV2_DEFAULT_DEPTH;
    if (mode == RNV2_WND_FULL) {
        rnv->window_x = 0.0F;
        rnv->window_y = 0.0F;
        rnv->window_width = rnv->display_width;
        rnv->window_height = rnv->display_height;
        rnv->image_x = 0.0F;
        rnv->image_y = 0.0F;
        rnv->image_width = (Float)rnv->width / (Float)rnv->texture_width;
        rnv->image_height = (Float)rnv->height / (Float)rnv->texture_height;
    } else {
        if (rnv->width == 320) {
            DOUBLE_WIDTH(rnv, sample_width);
        } else if (rnv->width == 352) {
            /* a 352-column picture shows its middle 320 columns */
            sample_width = 320.0F;
            rnv->window_x = 0.0F;
            rnv->window_width = rnv->display_width;
            rnv->image_x = (((352.0F - 320.0F) / 2.0F) + SAMPLE_INSET) /
                           (Float)rnv->texture_width;
            rnv->image_width = sample_width / (Float)rnv->texture_width;
        } else if (rnv->width == 480) {
            /* @hack redundant: a test for 480 columns, whose branch does what
             * the else does */
            STRETCH_WIDTH(rnv);
        } else {
            STRETCH_WIDTH(rnv);
        }
        if (rnv->height == 224 || rnv->height == 240) {
            sample_height = (Float)(rnv->height - 1);
            rnv->window_y =
                (rnv->display_height - (sample_height * 2.0F)) / 2.0F;
            rnv->window_height = sample_height * 2.0F;
            rnv->image_y = SAMPLE_INSET / (Float)rnv->texture_height;
            rnv->image_height = sample_height / (Float)rnv->texture_height;
        } else if (rnv->height == 320) {
            rnv->window_y = (rnv->display_height - (Float)rnv->height) / 2.0F;
            rnv->window_height = (Float)rnv->height;
            rnv->image_y = 0.0F;
            rnv->image_height = (Float)rnv->height / (Float)rnv->texture_height;
        } else if (rnv->height == 336) {
            sample_height = ((Float)rnv->height / 3.0F) * 4.0F;
            rnv->window_y = (rnv->display_height - sample_height) / 2.0F;
            rnv->window_height = sample_height;
            rnv->image_y = 0.0F;
            rnv->image_height = (Float)rnv->height / (Float)rnv->texture_height;
        } else if (rnv->height == 448) {
            rnv->window_y = 16.0F;
            rnv->window_height = 448.0F;
            rnv->image_y = 0.0F;
            rnv->image_height = 448.0F / (Float)rnv->texture_height;
        } else {
            rnv->window_y = 0.0F;
            rnv->window_height = rnv->display_height;
            rnv->image_y = 0.0F;
            rnv->image_height = (Float)rnv->height / (Float)rnv->texture_height;
        }
    }
    UPDATE_WINDOW(rnv);
}

void mwRnv2SetWndExpand(MWRNV2 rnv, Sint32 horizontal, Sint32 vertical)
{
    Float sample_width;
    Float sample_height;

    rnv->window_z = RNV2_DEFAULT_DEPTH;
    if (horizontal == 2) {
        DOUBLE_WIDTH(rnv, sample_width);
    } else {
        sample_width = (Float)(rnv->width * horizontal);
        rnv->window_x = (rnv->display_width - sample_width) / 2.0F;
        /* @bug logic: a horizontal other than 1 or 2 leaves the window
         * unscaled and off center. window_x centers a window horizontal times
         * as wide, as the height scales by vertical. the store at 0x8c10bafc
         * gives the window the picture's width */
        rnv->window_width = (Float)rnv->width;
        rnv->image_x = 0.0F;
        rnv->image_width = (Float)rnv->width / (Float)rnv->texture_width;
    }
    if (vertical == 2) {
        sample_height = (Float)(rnv->height - 1);
        rnv->window_y = (rnv->display_height - (Float)(rnv->height * 2)) / 2.0F;
        rnv->window_height = sample_height * 2.0F;
        rnv->image_y = SAMPLE_INSET / (Float)rnv->texture_height;
        rnv->image_height = sample_height / (Float)rnv->texture_height;
    } else {
        sample_height = (Float)(rnv->height * vertical);
        rnv->window_y = (rnv->display_height - sample_height) / 2.0F;
        rnv->window_height = sample_height;
        rnv->image_y = 0.0F;
        rnv->image_height = (Float)rnv->height / (Float)rnv->texture_height;
    }
    UPDATE_WINDOW(rnv);
}

#define LEFT(rnv) (rnv)->window_x
#define RIGHT(rnv) ((rnv)->window_x + (rnv)->window_width)
#define TOP(rnv) (rnv)->window_y
#define BOTTOM(rnv) ((rnv)->window_y + (rnv)->window_height)
#define U_LEFT(rnv) (rnv)->image_x
#define U_RIGHT(rnv) ((rnv)->image_x + (rnv)->image_width)
#define V_TOP(rnv) (rnv)->image_y
#define V_BOTTOM(rnv) ((rnv)->image_y + (rnv)->image_height)
#define SET_CORNER(out, sx, sy, tu, tv, color, add_color)                      \
    do {                                                                       \
        (out).x = (sx);                                                        \
        (out).y = (sy);                                                        \
        (out).u = (tu);                                                        \
        (out).v = (tv);                                                        \
        (out).base = (color);                                                  \
        (out).offset = (add_color);                                            \
    } while (0)

void mwRnv2SetVtex(MWRNV2 rnv, struct rnv2_vertex *vertices)
{
    Sint32 i;
    Uint32 color;
    Uint32 offset;

    color = rnv->brightness;
    offset = rnv->brightness_offset;
    /* @hack spelling: a loop over the corners that tests i for each, where
     * a statement for each corner would do */
    for (i = 0; i < RNV2_QUAD; i++) {
        if (i == RNV2_QUAD - 1) {
            vertices[i].control = KM_VERTEXPARAM_ENDOFSTRIP;
        } else {
            vertices[i].control = KM_VERTEXPARAM_NORMAL;
        }
        if (i == 0) {
            SET_CORNER(vertices[i], LEFT(rnv), BOTTOM(rnv), U_LEFT(rnv),
                       V_BOTTOM(rnv), color, offset);
        } else if (i == 1) {
            SET_CORNER(vertices[i], LEFT(rnv), TOP(rnv), U_LEFT(rnv),
                       V_TOP(rnv), color, offset);
        } else if (i == 2) {
            SET_CORNER(vertices[i], RIGHT(rnv), BOTTOM(rnv), U_RIGHT(rnv),
                       V_BOTTOM(rnv), color, offset);
        } else if (i == 3) {
            SET_CORNER(vertices[i], RIGHT(rnv), TOP(rnv), U_RIGHT(rnv),
                       V_TOP(rnv), color, offset);
        }
        vertices[i].z = 1.0F / rnv->window_z;
    }
}

void mwRnv2WndToSrf(MWRNV2 rnv)
{
    Sint32 i;
    Uint32 color;
    Uint32 offset;
    struct rnv2_vertex *points;

    color = rnv->brightness;
    offset = rnv->brightness_offset;
    points = rnv->points;
    /* @hack spelling: a loop over the corners that tests i for each, where
     * a statement for each corner would do */
    for (i = 0; i < RNV2_QUAD; i++) {
        if (i == 0) {
            points[i].control = RNV2_CONTINUE_STRIP;
            SET_CORNER(points[i], LEFT(rnv), TOP(rnv), U_LEFT(rnv), V_TOP(rnv),
                       color, offset);
        } else if (i == 1) {
            points[i].control = RNV2_CONTINUE_STRIP;
            SET_CORNER(points[i], RIGHT(rnv), TOP(rnv), U_RIGHT(rnv),
                       V_TOP(rnv), color, offset);
        } else if (i == 2) {
            points[i].control = RNV2_CONTINUE_STRIP;
            SET_CORNER(points[i], LEFT(rnv), BOTTOM(rnv), U_LEFT(rnv),
                       V_BOTTOM(rnv), color, offset);
        } else if (i == 3) {
            points[i].control = RNV2_END_STRIP;
            SET_CORNER(points[i], RIGHT(rnv), BOTTOM(rnv), U_RIGHT(rnv),
                       V_BOTTOM(rnv), color, offset);
        }
        points[i].z = rnv->window_z;
    }
}

void mwRnv2SrfToVtex(struct rnv2_vertex *point, struct rnv2_vertex *vertex)
{
    /* @hack redundant: unused, a word that only sizes the frame */
    Float unused;

    if (point->control == RNV2_END_STRIP) {
        vertex->control = KM_VERTEXPARAM_ENDOFSTRIP;
    } else {
        vertex->control = KM_VERTEXPARAM_NORMAL;
    }
    vertex->x = point->x;
    vertex->y = point->y;
    vertex->z = 1.0F / point->z;
    vertex->u = point->u;
    vertex->v = point->v;
    vertex->base = point->base;
    vertex->offset = point->offset;
}

void mwRnv2SrfToVtexAll(struct rnv2_vertex *points, Sint32 point_count,
                        struct rnv2_vertex *vertices, Sint32 vertex_count)
{
    Sint32 i;

    for (i = 0; i < point_count; i++) {
        mwRnv2SrfToVtex(points, vertices);
        vertices++;
        points++;
    }
}

void mwRnv2DispTex(MWRNV2 rnv, Sint32 index)
{
    KMSURFACEDESC *surface;
    Sint32 i;

    if (rnv->display_enabled == FALSE) {
        return;
    }
    if (rnv->vertex_buffer == NULL) {
        return;
    }
    /* @hack type: frame_count cast to Sint32, where an unsigned comparison
     * would do. the two agree for the nonnegative index and any frame_count
     * an Sint32 holds */
    if (index < 0 || index >= (Sint32)rnv->frame_count) {
        return;
    }
    surface = &rnv->surfaces[index];
    rnv->context.pTextureSurfaceDesc = surface;
    kmProcessVertexRenderState(&rnv->context);
    kmSetVertexRenderState(&rnv->context);
    kmStartVertexStrip(rnv->vertex_buffer);
    for (i = 0; i < rnv->vertex_count; i++) {
#ifdef NONMATCHING
        /* this side squeezes the movie's picture under the port's 16:9, so
         * that it keeps its shape (engine/dc/port.h) */
        struct rnv2_vertex squeezed = rnv->vertices[i];

        squeezed.x = port_wide_x(squeezed.x);
        kmSetVertex(rnv->vertex_buffer, &squeezed, KM_VERTEXTYPE_03,
                    sizeof(struct rnv2_vertex));
#else
        kmSetVertex(rnv->vertex_buffer, &rnv->vertices[i], KM_VERTEXTYPE_03,
                    sizeof(struct rnv2_vertex));
#endif
    }
}
