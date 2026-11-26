/* what the direct compile, its size calls, and its vertex and strip
 * compilers share */
#ifndef NJ_DIRECT_H
#define NJ_DIRECT_H
#include <nj_cnk_set_material.h>
#include <ninja.h>
#include <sg_xpt.h>

/* a compiled model's head. the model's global records follow the head: nOP
 * opaque ones, then nTR translucent ones. each global record's vertex
 * records follow that global record.
 *
 * center is the model's center, transformed as its points are. r is the
 * model's radius. next points to the next model's head.
 * the compile's last model has a null next. mode is TRUE where the model's
 * records take the D8 formats, and FALSE for the plain formats */
struct direct_head {
    NJS_POINT3 center;
    Float r;
    struct direct_head *next;
    Uint32 nOP;
    Uint32 nTR;
    Bool mode;
};
/* a strip chunk's global record in the plain formats. GP and GLOBALPARAM are
 * two copies of the global word of the chunk's polygons. TSPPARAM,
 * TEXTUREADDR and ISPPARAM are the polygons' other three parameter words.
 *
 * sg, sb, sa and sr are a specular color's channels. dg, db, da and dr are a
 * diffuse color's. both colors are the last that njCnkSetMaterial gave any
 * compile.
 *
 * mode is TRUE where the chunk has NJD_FST_ENV, for an environment map, and
 * FALSE otherwise. num counts the vertex records that follow the record */
typedef struct {
    Uint32 GP;
    Bool mode;
    Float sg, sb, sa, sr;
    Float dg, db, da, dr;
    Uint32 TSPPARAM, TEXTUREADDR;
    Uint32 GLOBALPARAM, ISPPARAM;
    Uint32 num;
    Uint32 unk_3c;
} NJS_DIRECT_GLOBAL;
/* a strip chunk's global record in the D8 formats. its fields are
 * NJS_DIRECT_GLOBAL's, without the specular and diffuse colors */
typedef struct {
    Uint32 GP;
    Bool mode;
    Uint32 TSPPARAM, TEXTUREADDR;
    Uint32 GLOBALPARAM, ISPPARAM;
    Uint32 num;
    Uint32 unk_1c;
} NJS_DIRECT_GLOBAL_D8;

/* the culling mode, one of the NJD_POLYGON_CULLING values, that the compile
 * writes into each global record's ISPPARAM. the mode is
 * NJD_POLYGON_CULLINGSMALL until njCnkDirectCullingMode sets it */
extern Uint32 _nj_direct_culling_mode_;
/* the diffuse color that njCnkSetMaterial last gave any compile, as four
 * floats that ARGB_A to ARGB_B index. each float is 1 until njCnkSetMaterial
 * sets it */
/* @hack type: four floats, where an NJS_ARGB would do */
extern Float _nj_direct_col[4];
/* the mode of the compile or size count in progress: DIRECT_COMPILE_PLAIN, or
 * DIRECT_COMPILE_LIGHT for a lit one. each compile and size call sets it
 * first */
extern Sint32 _nj_direct_compile_mode_;
#define DIRECT_COMPILE_PLAIN 0
#define DIRECT_COMPILE_LIGHT 1
/* the light a lit compile was given */
extern NJS_DIRECT_COMPILE_LIGHT *_nj_direct_compile_light_;

/* the masks of the two counts in the halfword that NJD_UFO_SHIFT describes.
 * STRIP_COUNT masks the halfword as it stands. STRIP_USER masks the halfword
 * shifted right by NJD_UFO_SHIFT */
#define STRIP_COUNT ((1 << NJD_UFO_SHIFT) - 1)
#define STRIP_USER 3

/* CHANNEL_BYTE(c) is the byte of a channel's float c, its fraction dropped.
 * CHANNEL_BYTE clamps nothing, so c must lie from 0 to COLOR_MAX, where
 * CLAMP_CHANNEL puts it */
#define COLOR_MAX 255.0F
#define CHANNEL_BYTE(c) ((Uint32)(c) & 0xff)
#define RED_BYTE(color) (((color) >> 16) & 0xff)
#define GREEN_BYTE(color) (((color) >> 8) & 0xff)
#define BLUE_BYTE(color) ((color) & 0xff)
#define CLAMP_CHANNEL(c)                                                       \
    {                                                                          \
        if ((c) < 0.0F) {                                                      \
            (c) = 0.0F;                                                        \
        }                                                                      \
        if ((c) > COLOR_MAX) {                                                 \
            (c) = COLOR_MAX;                                                   \
        }                                                                      \
    }
/* set out's base color from the channels a, r, g and b. each channel is
 * clamped to 0..COLOR_MAX first, in place */
#define PUT_BASE(out, a, r, g, b)                                              \
    {                                                                          \
        CLAMP_CHANNEL(r);                                                      \
        CLAMP_CHANNEL(g);                                                      \
        CLAMP_CHANNEL(b);                                                      \
        CLAMP_CHANNEL(a);                                                      \
        /* @hack spelling: the bytes ORed green first, where alpha first       \
         * would do */                                                         \
        (out)->base = (CHANNEL_BYTE(g) << 8) | (CHANNEL_BYTE(r) << 16) |       \
                      (CHANNEL_BYTE(a) << 24) | CHANNEL_BYTE(b);               \
    }

/* the record writers. each writes fields of out, a compiled record. v is the
 * vertex of the buffer that the record sends. END_RECORD writes the record's
 * last two fields, and steps out to the next record */
#define PUT_POINT_NORMAL(out, v)                                               \
    {                                                                          \
        (out)->px = (v)->px;                                                   \
        (out)->py = (v)->py;                                                   \
        (out)->pz = (v)->pz;                                                   \
        (out)->nx = (v)->nx;                                                   \
        (out)->ny = (v)->ny;                                                   \
        (out)->nz = (v)->nz;                                                   \
    }
#define PUT_POINT_D8(out, v)                                                   \
    {                                                                          \
        (out)->px = (v)->px;                                                   \
        (out)->py = (v)->py;                                                   \
        (out)->pz = (v)->pz;                                                   \
        (out)->one = 1.0F;                                                     \
    }
/* PUT_POINT_D8, then the record's colors. in a lit compile, PUT_BASE makes the
 * base color from the vertex's red, green and blue. its alpha is 255 times the
 * diffuse alpha. otherwise the base color is the vertex's color. the offset
 * color is 0. a, r, g and b are the caller's Float locals, which the macro
 * overwrites */
#define PUT_POINT_COLOR_D8(out, v, a, r, g, b)                                 \
    {                                                                          \
        PUT_POINT_D8(out, v);                                                  \
        if (_nj_direct_compile_mode_ == DIRECT_COMPILE_LIGHT) {                \
            (a) = _nj_direct_col[ARGB_A] * COLOR_MAX;                          \
            (r) = RED_BYTE((v)->col);                                          \
            (g) = GREEN_BYTE((v)->col);                                        \
            (b) = BLUE_BYTE((v)->col);                                         \
            /* @hack redundant: the clamps PUT_BASE puts on r, g and b,        \
             * which are bytes already */                                      \
            PUT_BASE(out, a, r, g, b);                                         \
        } else {                                                               \
            (out)->base = (v)->col;                                            \
        }                                                                      \
        (out)->off = 0;                                                        \
    }
#define END_RECORD(out, packed, vertex_param)                                  \
    {                                                                          \
        (out)->uv = (packed).word;                                             \
        (out)->param = (vertex_param);                                         \
        (out)++;                                                               \
    }

/* a texture coordinate as two floats, or as the four halfwords they lie in.
 * each float's low halfword lies first in memory */
union direct_uv {
    struct {
        Float u, v;
    } f;
    struct {
        Uint16 u_low, u_high, v_low, v_high;
    } h;
};
/* a texture coordinate as a compiled vertex's uv word holds it */
union direct_uv_word {
    struct {
        Uint16 v, u;
    } h;
    Uint32 word;
};
/* pack uv's two floats into packed, each cut to its high halfword. a float's
 * high halfword holds its sign, its exponent and the top 7 bits of its
 * mantissa */
#define PACK_UV(packed, uv)                                                    \
    {                                                                          \
        (packed).h.u = (uv).h.u_high;                                          \
        (packed).h.v = (uv).h.v_high;                                          \
    }
/* map vertex's normal onto uv for an environment map */
#define ENV_UV(uv, vertex)                                                     \
    {                                                                          \
        (uv).f.u = ((vertex)->nx * 0.5F) + 0.5F;                               \
        (uv).f.v = 0.5F - ((vertex)->ny * 0.5F);                               \
    }

#endif
