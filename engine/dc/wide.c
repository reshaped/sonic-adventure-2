/* the port's 16:9: the picture's aspect, and the squeeze of the 2D draws.
 * port.h gives the rule. make dc links this into 1ST_READ.BIN after the
 * image's sources */
#include <kamui.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct_draw.h>
#include <nj_global.h>
#include <nj_km.h>
#include <sg_xpt.h>
#ifdef DREAMCAST
#include <machine.h>
#else
/* off the console no store queue takes the vertices */
#define prefetch(p) ((void)(p))
#endif

#include "draw/screen.h"
#include "engine/dc/port.h"
#include "system/game_state.h"

/* the two draws below call Ninja's own, past port.h's macros */
#undef njDrawSprite2D
#undef njProjectScreen

Float port_wide_anchor = WIDE_MIDDLE;
Int port_wide_cover;
Int port_wide;
Int port_pillarbox;
Float port_view_aspect = 1.0F;

/* pillarbox's bands are opaque black */
#define BAND_COLOR 0xff000000

Float port_view_x(Float x)
{
    /* cx + (x - cx) can round away from x */
    if (port_view_aspect == 1.0F) {
        return x;
    }
    return _nj_screen_.cx + ((x - _nj_screen_.cx) * port_view_aspect);
}

Int port_wide_squeezes(void)
{
    return port_wide && (port_pillarbox || !port_wide_cover);
}

Float port_wide_x(Float x)
{
    Float pivot;

    if (!port_wide_squeezes()) {
        return x;
    }
    pivot = _nj_screen_.cx + (port_pillarbox ? WIDE_MIDDLE : port_wide_anchor);
    return pivot + ((x - pivot) * WIDE_ASPECT);
}

Int port_wide_covers(const Float *x, Int count, Int stride)
{
    Int left = FALSE;
    Int right = FALSE;

    if (port_pillarbox) {
        return FALSE;
    }
    for (; count > 0; count--, x += stride) {
        if (*x >= -1.0F && *x <= 1.0F) {
            left = TRUE;
        } else if (*x >= SCREEN_WIDTH - 1.0F && *x <= SCREEN_WIDTH + 1.0F) {
            right = TRUE;
        } else {
            return FALSE;
        }
    }
    return left && right;
}

/* an opaque quad from x0 to x1 down the whole screen, at a 1/z of 1, nearer
 * than the game's 3D */
static void band_draw(Float x0, Float x1)
{
    NJS_POINT2 points[4];
    NJS_COLOR colors[4];
    NJS_POINT2COL band;
    Int i;

    points[0].x = points[3].x = x0;
    points[1].x = points[2].x = x1;
    points[0].y = points[1].y = 0.0F;
    points[2].y = points[3].y = SCREEN_HEIGHT;
    for (i = 0; i < 4; i++) {
        colors[i].color = BAND_COLOR;
    }
    band.p = points;
    band.col = colors;
    band.tex = NULL;
    band.num = 4;
    njDrawPolygon2D(&band, 4, -1.0F, NJD_FILL);
}

void wide_end(void)
{
    Float side = SCREEN_WIDTH * 0.5F * (1.0F - WIDE_ASPECT);
    KMINT32 fog;

    if (!port_pillarbox) {
        return;
    }
    /* with pillarbox ended and the cover on, the bands keep x */
    port_pillarbox = FALSE;
    port_wide_cover = TRUE;
    /* the bands take no fog, which would tint them with a stage's color */
    fog = _nj_current_ctx_->FogMode;
    njFogDisable();
    band_draw(0.0F, side);
    band_draw(SCREEN_WIDTH - side, SCREEN_WIDTH);
    if (fog != KM_NOFOG) {
        njFogEnable();
    }
    port_wide_cover = FALSE;
}

/* a vertex of a sprite's strip, as njDrawSprite2D sends it: its place, its u
 * and v, and a base of 1. the offset stays unwritten */
struct sprite_vertex {
    Uint32 pcw;
    Float x, y, z;
    Float u, v;
    Float base;
    Float offset;
};
/* a cell's u and v count 256 to the texture's side */
#define UV_UNIT (1.0F / 256.0F)

/* the squeezed sprite goes out from here, as sdk/ninja/njSprite.src's
 * sprite_emit_2d sends one: the same texture, color mode, corners, u and v,
 * and depth. only x differs. the cell's point lands where port_wide_x puts
 * p.x, and each corner's x scales about that point by WIDE_ASPECT, after
 * NJD_SPRITE_ANGLE's turn. so a turned cell keeps its shape on the 16:9
 * screen as an upright one does */
void wide_draw_sprite_2d(NJS_SPRITE *sp, Int n, Float pri, Uint32 attr)
{
    const NJS_TEXANIM *cell;
    struct sprite_vertex *sq;
    Float *face;
    Float xs[2];
    Float ys[2];
    Float us[2];
    Float vs[2];
    Float sine = 0.0F;
    Float cosine = 1.0F;
    Float depth = -pri;
    Float centre;
    Float x;
    Float y;
    Int i;

    if (!port_wide_squeezes()) {
        njDrawSprite2D(sp, n, pri, attr);
        return;
    }
    cell = &sp->tanim[n];
    njGetkmPointer();
    njSetTexture(sp->tlist);
    njSetkmTextureNum(cell->texid);
    face = attr & NJD_SPRITE_COLOR ? (Float *)&_nj_constant_material_
                                   : (Float *)&sprite_white;
    if (attr & NJD_SPRITE_ALPHA) {
        njSetkmColorMode(NJD_COLOR_MODE_FLAT_INTENSITY_TEXTURE_TRANS);
        sq = (struct sprite_vertex *)njStartVertexTexTransIntens(face);
    } else {
        njSetkmColorMode(NJD_COLOR_MODE_FLAT_INTENSITY_TEXTURE);
        sq = (struct sprite_vertex *)njStartVertexTexOpaqueIntens(face);
    }
    xs[0] = (Float)-cell->cx * sp->sx;
    xs[1] = (Float)(cell->sx - cell->cx) * sp->sx;
    ys[0] = (Float)-cell->cy * sp->sy;
    ys[1] = (Float)(cell->sy - cell->cy) * sp->sy;
    us[(attr & NJD_SPRITE_HFLIP) != 0] = (Float)cell->u1 * UV_UNIT;
    us[(attr & NJD_SPRITE_HFLIP) == 0] = (Float)cell->u2 * UV_UNIT;
    vs[(attr & NJD_SPRITE_VFLIP) != 0] = (Float)cell->v1 * UV_UNIT;
    vs[(attr & NJD_SPRITE_VFLIP) == 0] = (Float)cell->v2 * UV_UNIT;
    if (attr & NJD_SPRITE_ANGLE) {
        sine = njSin(sp->ang);
        cosine = njCos(sp->ang);
    }
    depth = 1.0F > depth ? 1.0F : 1.0F / depth;
    centre = port_wide_x(sp->p.x);
    /* the corners run top left, bottom left, top right, bottom right */
    for (i = 0; i < 4; i++, sq++) {
        x = xs[i >> 1];
        y = ys[i & 1];
        sq->pcw = i == 3 ? KM_VERTEXPARAM_ENDOFSTRIP : KM_VERTEXPARAM_NORMAL;
        sq->x = centre + (((x * cosine) - (y * sine)) * WIDE_ASPECT);
        sq->y = sp->p.y + ((x * sine) + (y * cosine));
        sq->z = depth;
        sq->u = us[i >> 1];
        sq->v = vs[i & 1];
        sq->base = 1.0F;
        prefetch(sq);
    }
    njEndVertex((Uint32 *)sq);
    njSetkmPointer();
}

void wide_project_screen(NJS_MATRIX *m, NJS_POINT3 *p3, NJS_POINT2 *p2)
{
    Float aspect = _nj_scr_aspect_x_;

    _nj_scr_aspect_x_ = 1.0F;
    njProjectScreen(m, p3, p2);
    _nj_scr_aspect_x_ = aspect;
}

void wide_apply(void)
{
    Int wide = port_toggles[TOGGLE_WIDE] != 0;

    port_pillarbox = wide && game_state.demo != 0;
    port_view_aspect = wide && !port_pillarbox ? WIDE_ASPECT : 1.0F;
    if (wide == port_wide) {
        return;
    }
    port_wide = wide;
    _nj_scr_aspect_x_ = wide ? WIDE_ASPECT : 1.0F;
    /* the chunk and the direct draws project by copies of the screen's
     * scale, which their lights' setters take. play sets the lights at each
     * camera, but a menu sets none. so set each again as it stands */
    njCnkSetEasyLight(_nj_vertex_easy.light.x, _nj_vertex_easy.light.y,
                      _nj_vertex_easy.light.z);
    njCnkSetSimpleLight(_nj_vertex_simple.light.x, _nj_vertex_simple.light.y,
                        _nj_vertex_simple.light.z);
    njSetDirectLight(direct_light.direction.x, direct_light.direction.y,
                     direct_light.direction.z);
    njSetDirectClip();
}
