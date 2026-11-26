#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_km.h>
#include <sg_xpt.h>

/* a 16-bit u or v is the high 16 bits of its float. a packed pair holds u in
 * its high half, and v UV16_SHIFT bits lower */
#define UV16_BITS 0xffff0000
#define UV16_SHIFT 16
/* the control word njDrawQuadTexture sends ahead of a quad's corners. its top
 * four bits are those of KM_VERTEXPARAM_ENDOFSTRIP, which njDrawQuadTextureEx
 * sends in this word's place */
#define QUAD_VERTEXPARAM 0xffff0000
/* the words SEND_SPRITE writes of a sprite's parameter: its four opening
 * words, then the color */
#define SPRITE_PARAM_WORDS 5

/* the store queue address the run's next word goes to */
static Uint32 *quad_sq;
/* set for a run whose quads go to the translucent list */
static Bool quad_trans;

void njQuadTextureStart(Sint32 trans)
{
    njGetkmPointer();
    quad_trans = trans;
    if (trans) {
        quad_sq = SQ_ADDRESS((Uint32)_nj_parameterkm_.trans);
        _nj_sq_base_ = (Uint32)_nj_parameterkm_.trans & ~SQ_OFFSET;
        SQ_MAP((Uint32)_nj_parameterkm_.trans);
    } else {
        quad_sq = SQ_ADDRESS((Uint32)_nj_parameterkm_.opaque);
        _nj_sq_base_ = (Uint32)_nj_parameterkm_.opaque & ~SQ_OFFSET;
        SQ_MAP((Uint32)_nj_parameterkm_.opaque);
    }
}

void njQuadTextureEnd(void)
{
    njEndVertex(quad_sq);
    njSetkmPointer();
}

/* send a textured sprite's parameter, colored col, at quad_sq, and move
 * quad_sq a block on. the TSP word is the one njStartVertexTexSpriteOpaque
 * or njStartVertexTexSpriteTrans sends for the copy. the global word keeps
 * the copy's user clip and shadow bit. those starters take the shadow bit from
 * NJD_CONTROL_3D_SHADOW instead. words 5 to 7 stay unwritten */
#define SEND_SPRITE(col)                                                       \
    {                                                                          \
        Uint32 *sq = quad_sq;                                                  \
        if (quad_trans) {                                                      \
            sq += SPRITE_PARAM_WORDS;                                          \
            *--sq = (col);                                                     \
            *--sq = _nj_parameterkm_.texture;                                  \
            *--sq = (_nj_parameterkm_.tsp & ~TSP_IGNORE_TEX_ALPHA) |           \
                    TSP_USE_ALPHA;                                             \
            *--sq = _nj_parameterkm_.isp;                                      \
            *--sq = (_nj_parameterkm_.global &                                 \
                     (GLOBAL_USER_CLIP | GLOBAL_SHADOW)) |                     \
                    GLOBAL_SPRITE | GLOBAL_TRANS | GLOBAL_TEXTURE |            \
                    GLOBAL_UV16;                                               \
            prefetch(sq);                                                      \
            sq += SQ_WORDS;                                                    \
            _nj_listtype_ = &_nj_parameterkm_.trans;                           \
        } else {                                                               \
            sq += SPRITE_PARAM_WORDS;                                          \
            *--sq = (col);                                                     \
            *--sq = _nj_parameterkm_.texture;                                  \
            *--sq = TSP_OPAQUE(_nj_parameterkm_.tsp);                          \
            *--sq = _nj_parameterkm_.isp;                                      \
            *--sq = (_nj_parameterkm_.global &                                 \
                     (GLOBAL_USER_CLIP | GLOBAL_SHADOW)) |                     \
                    GLOBAL_SPRITE | GLOBAL_TEXTURE | GLOBAL_UV16;              \
            prefetch(sq);                                                      \
            sq += SQ_WORDS;                                                    \
            _nj_listtype_ = &_nj_parameterkm_.opaque;                          \
        }                                                                      \
        quad_sq = sq;                                                          \
    }

void njSetQuadTexture(Sint32 texid, Uint32 col)
{
    njSetkmTextureNum(texid);
    SEND_SPRITE(col)
}

void njSetQuadTextureG(Sint32 gid, Uint32 col)
{
    njSetkmTextureNumG(gid);
    SEND_SPRITE(col)
}

void njSetQuadTextureColor(Uint32 col)
{
    SEND_SPRITE(col)
}

/* a quad goes out at quad_sq as a 64-byte sprite, each block stored from its
 * last word down. word 0 is the control word. words 1 to 9 hold the x, y and z
 * of corners a, b and c. words 10 and 11 hold d's x and y, and d takes no
 * depth. word 12 stays unwritten. words 13 to 15 hold the u and v of a, b and
 * c. a is (x1, y1), b (x2, y1), c (x2, y2) and d (x1, y2) */
void njDrawQuadTexture(NJS_QUAD_TEXTURE *q, Float z)
{
    /* @hack redundant: register on p */
    register Float *p = &q->x1;
    Uint32 *sq;
    Float x1;
    Float y1;
    Float x2;
    Float y2;
    /* @hack redundant: register on u1, v1, u2 and v2 */
    register Uint32 u1;
    register Uint32 v1;
    register Uint32 u2;
    register Uint32 v2;

    /* @hack spelling: q's floats read through p, a cursor, where reads of
     * q's fields would do */
    x1 = *p++;
    y1 = *p++;
    x2 = *p++;
    y2 = *p++;
#ifdef NONMATCHING
    /* this side squeezes the quad under the port's 16:9
     * (engine/dc/port.h) */
    x1 = port_wide_x(x1);
    x2 = port_wide_x(x2);
#endif
    sq = quad_sq + SQ_WORDS;
    *(Float *)--sq = x2;
    *(Float *)--sq = z;
    *(Float *)--sq = y1;
    *(Float *)--sq = x2;
    *(Float *)--sq = z;
    *(Float *)--sq = y1;
    *(Float *)--sq = x1;
    *--sq = QUAD_VERTEXPARAM;
    prefetch(sq);
    sq += 2 * SQ_WORDS;
    /* @hack spelling: each u and v word read whole, then masked and shifted in
     * an order of its own, where an expression for each would do */
    u1 = *(Uint32 *)p++;
    v1 = *(Uint32 *)p++;
    u2 = *(Uint32 *)p++;
    v2 = *(Uint32 *)p >> UV16_SHIFT;
    u1 &= UV16_BITS;
    /* @hack spelling: v1 = v1 >> UV16_SHIFT, where v1 >>= UV16_SHIFT would
     * do */
    v1 = v1 >> UV16_SHIFT;
    u2 &= UV16_BITS;
    *--sq = u2 | v2;
    *--sq = u2 | v1;
    *--sq = u1 | v1;
    --sq;
    *(Float *)--sq = y2;
    *(Float *)--sq = x1;
    *(Float *)--sq = z;
    *(Float *)--sq = y2;
    prefetch(sq);
    sq += SQ_WORDS;
    quad_sq = sq;
}

#define STEP(q) ((q) = (NJS_QUAD_TEXTURE_EX *)((Float *)(q) + 1))

/* the quad goes out as njDrawQuadTexture lays one out. its corners are
 * a = (x, y), b = a + (vx1, vy1), c = b + (vx2, vy2) and d = a + (vx2, vy2).
 * u and v step from a to b to c the same way */
void njDrawQuadTextureEx(NJS_QUAD_TEXTURE_EX *q)
{
    /* @hack redundant: register on sq and mask */
    register Uint32 *sq;
    register Uint32 mask;
    /* @hack spelling: bx and by declared ahead of the other floats, where any
     * order would do */
    Float bx;
    Float by;
    /* @hack redundant: register on x, y, z, the edges and the u and v values */
    register Float x;
    register Float y;
    register Float z;
    register Float vx1;
    register Float vy1;
    register Float vx2;
    register Float vy2;
    register Float u;
    register Float v;
    register Float bu;
    register Float bv;
    register Float cu;
    register Float cv;
    /* a u and v pair, as floats and as their bits */
    union {
        Float f[2];
        Uint32 bits[2];
    } uv;

    /* @hack type: q stepped float by float as its own cursor, where a Float
     * cursor set from q would do */
    x = *(Float *)q;
    STEP(q);
    y = *(Float *)q;
    STEP(q);
    z = *(Float *)q;
    STEP(q);
    vx1 = *(Float *)q;
    STEP(q);
    vy1 = *(Float *)q;
    STEP(q);
    vx2 = *(Float *)q;
    STEP(q);
    vy2 = *(Float *)q;
    STEP(q);
    bx = x + vx1;
    /* @hack redundant: a test of vx1 that never runs */
    if (0) {
        if (vx1) {
        }
    }
    sq = quad_sq + SQ_WORDS;
    *(Float *)--sq = bx + vx2;
    *(Float *)--sq = z;
    /* @hack spelling: by worked out after two of the stores, where beside bx
     * would do */
    by = y + vy1;
    *(Float *)--sq = by;
    *(Float *)--sq = bx;
    *(Float *)--sq = z;
    *(Float *)--sq = y;
    *(Float *)--sq = x;
    *--sq = KM_VERTEXPARAM_ENDOFSTRIP;
    prefetch(sq);
    sq += 2 * SQ_WORDS;
    /* @hack spelling: mask, a local for UV16_BITS, where the constant would
     * do */
    mask = UV16_BITS;
    STEP(q);
    u = *(Float *)q;
    STEP(q);
    v = *(Float *)q;
    STEP(q);
    /* @hack spelling: bu, bv and cu each copied, then summed in place, where a
     * sum of two would do */
    bu = u;
    bu += *(Float *)q;
    STEP(q);
    bv = v;
    bv += *(Float *)q;
    STEP(q);
    /* @hack redundant: a test of v that never runs */
    if (0) {
        if (v) {
        }
    }
    cu = bu;
    cu += *(Float *)q;
    STEP(q);
    /* @hack redundant: a test of bu that never runs */
    if (0) {
        if (bu) {
        }
    }
    cv = bv + *(Float *)q;
    uv.f[0] = cu;
    uv.f[1] = cv;
    *--sq = (uv.bits[0] & mask) | (uv.bits[1] >> UV16_SHIFT);
    uv.f[0] = bu;
    uv.f[1] = bv;
    *--sq = (uv.bits[0] & mask) | (uv.bits[1] >> UV16_SHIFT);
    uv.f[0] = u;
    uv.f[1] = v;
    *--sq = (uv.bits[0] & mask) | (uv.bits[1] >> UV16_SHIFT);
    /* @hack redundant: a test of z + z + y that never runs */
    if (0) {
        if (z + z + y) {
        }
    }
    --sq;
    *(Float *)--sq = y + vy2;
    *(Float *)--sq = x + vx2;
    *(Float *)--sq = z;
    *(Float *)--sq = by + vy2;
    prefetch(sq);
    sq += SQ_WORDS;
    quad_sq = sq;
}
