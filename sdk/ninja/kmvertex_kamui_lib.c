/* Kamui's polygon headers. kmProcessVertexRenderState packs a vertex
 * context's modes into its four header words, and kmSetVertexRenderState
 * makes them the current header. the kmChangeContext calls change one mode of
 * the current header. kmiGlobalWork holds the current header as
 * kmStartVertexStrip sends it */
#include <kamui.h>
#include <kamui_buffer.h>
#include <kamui_internal.h>
#include <kamui_vertex.h>
#include <nj_km.h>

/* the fields of a header's words that nj_km.h doesn't name. FIELD puts a
 * field's value at its _SHIFT, the field's lowest bit */
#define GLOBAL_PARA 0xe0000000
#define GLOBAL_STRIP 0x000c0000 /* the strip length */
#define GLOBAL_STRIP_SHIFT 18
#define GLOBAL_USER_CLIP_SHIFT 16
#define GLOBAL_SHADOW_SHIFT 7
#define GLOBAL_VOLUME_SHIFT 6
#define GLOBAL_COLOR 0x00000030 /* the color type */
#define GLOBAL_COLOR_SHIFT 4
#define GLOBAL_COLOR_HIGH 0x00000020
#define GLOBAL_COLOR_LOW 0x00000010
#define GLOBAL_TEXTURE_SHIFT 3
#define GLOBAL_GOURAUD 0x00000002
#define GLOBAL_GOURAUD_SHIFT 1
#define GLOBAL_UV16_SHIFT 0
#define ISP_DEPTH 0xe0000000 /* the depth compare */
#define ISP_DEPTH_SHIFT 29
#define ISP_CULL_SHIFT 27
#define ISP_TEXTURE 0x02000000
#define ISP_TEXTURE_SHIFT 25
#define ISP_OFFSET 0x01000000
#define ISP_OFFSET_SHIFT 24
#define ISP_GOURAUD 0x00800000
#define ISP_GOURAUD_SHIFT 23
#define ISP_UV16 0x00400000
#define ISP_UV16_SHIFT 22
#define ISP_DCALC_EXACT 0x00100000
#define TSP_SRC_SELECT 0x02000000
#define TSP_DST_SELECT 0x01000000
#define TSP_FOG 0x00c00000
#define TSP_FOG_SHIFT 22
#define TSP_CLAMP_COLOR 0x00200000
#define TSP_FLIP_SHIFT 17
#define TSP_CLAMP_SHIFT 15
#define TSP_FILTER_SHIFT 13
#define TSP_MIPMAP_SHIFT 8
#define TSP_SHADING_SHIFT 6
#define TSP_U_SIZE 0x00000038
#define TSP_U_SIZE_SHIFT 3
#define TSP_V_SIZE 0x00000007
#define TSP_V_SIZE_SHIFT 0
#define TEX_PALETTE 0x07e00000
#define TEX_PALETTE_SHIFT 21
/* the address field takes pSurface in 8-byte units */
#define TEX_ADDRESS 0x001fffff
#define TEX_ADDRESS_SHIFT 3
/* the texture word's fields that copy a surface flag. FLAG_FIELD moves a
 * KM_SURFACEFLAGS_ bit up to its field by the field's _FROM_FLAG.
 * TEX_SCAN_ORDER marks texels that lie row after row, not twiddled */
#define TEX_MIPMAP 0x80000000
#define TEX_MIPMAP_FROM_FLAG 31
#define TEX_VQ 0x40000000
#define TEX_VQ_FROM_FLAG 27
#define TEX_SCAN_ORDER 0x04000000
#define TEX_SCAN_ORDER_FROM_FLAG 24
#define TEX_STRIDE 0x02000000
#define TEX_STRIDE_FROM_FLAG 21
/* the bits kmProcessVertexRenderState keeps of the control and ISP words */
#define GLOBAL_KEPT 0xe78f00ff
#define ISP_KEPT 0xffd00000

/* a polygon's parameter type, which a modifier volume's shares. a sprite's,
 * KM_PARAM_SPRITE, differs from it by PARA_SPRITE_BIT */
#define PARA_POLYGON 4
#define PARA_SPRITE_BIT 0x20000000
/* the ListType bit of the two modifier volume lists, KM_OPAQUE_MODIFIER and
 * KM_TRANS_MODIFIER */
#define LIST_MODIFIER 1
#define MODIFIER_LAST                                                          \
    (KM_MODIFIER_INCLUDE_LAST_POLY | KM_MODIFIER_EXCLUDE_LAST_POLY)
/* SelectModifier's bit for a polygon that modifier volumes change */
#define MODIFIER_SELECTED 1
/* ShadingMode's values hold a texture bit at SHADING_TEXTURE_SHIFT and a
 * Gouraud bit, SHADING_GOURAUD */
#define SHADING_TEXTURE_SHIFT 1
#define SHADING_GOURAUD 1

/* KMI_SHADOW_SCALE, the cheap shadow register, holds the intensity in its low
 * byte, and the bit at SHADOW_ENABLE_SHIFT turns cheap shadows on */
#define SHADOW_ENABLE_SHIFT 8

struct face_color {
    KMFLOAT alpha;
    KMFLOAT red;
    KMFLOAT green;
    KMFLOAT blue;
};
struct header_volume {
    KMDWORD isp;
    KMDWORD tsp;
    KMDWORD texture;
    KMDWORD reserved;
    struct face_color face;
    struct face_color offset;
};
/* a polygon header as Kamui keeps it. a header with two volumes takes
 * volume[1]'s TSP and texture words and face color for its second. a modifier
 * list's header takes volume[1].isp as its instruction word */
struct polygon_header {
    KMDWORD control;
    KMDWORD reserved[3];
    struct header_volume volume[2];
};

/* a header's layout after its first four words, by its control word */
enum header_format {
    HEADER_PLAIN = 0,
    HEADER_INTENSITY = 1,
    HEADER_INTENSITY_OFFSET = 2,
    HEADER_TWO_VOLUMES = 3,
    HEADER_TWO_INTENSITIES = 4,
    HEADER_SPRITE = 16,
    HEADER_MODIFIER = 32,
    HEADER_MODIFIER_SPRITE = 255 /* a sprite in a modifier list */
};
enum header_word { WORD_CONTROL, WORD_ISP, WORD_TSP, WORD_TEXTURE };

/* 1 while cheap shadows are off. a polygon that modifier volumes change then
 * takes two volumes. under cheap shadows it takes one, which the volumes
 * darken */
static KMDWORD shadow_disabled = 1;
/* the words kmiPutGlobalParam copies. no code writes it */
KMDWORD kmiGlobalNum = 0;
#define FOUR_SET_WORDS ~0UL, ~0UL, ~0UL, ~0UL
/* @hack type: kmiGlobalWork, an array of one, whose words the kmChangeContext
 * calls store through its address, where a plain record would do */
struct kmi_global_header kmiGlobalWork[1] = {
    {{FOUR_SET_WORDS, FOUR_SET_WORDS, FOUR_SET_WORDS, FOUR_SET_WORDS}}};
#define REPEAT_FOUR(format) format, format, format, format
#define REPEAT_SIXTEEN(format)                                                 \
    REPEAT_FOUR(format), REPEAT_FOUR(format), REPEAT_FOUR(format),             \
        REPEAT_FOUR(format)
/* the format for each index HEADER_INDEX makes of a control word. the index's
 * six bits, from bit 5 down, are PARA_SPRITE_BIT, the modifier list's bit,
 * nj_km.h's GLOBAL_VOLUME, GLOBAL_COLOR's two bits, and nj_km.h's
 * GLOBAL_OFFSET */
static enum header_format header_formats[64] = {
    REPEAT_FOUR(HEADER_PLAIN),
    HEADER_INTENSITY,
    HEADER_INTENSITY_OFFSET,
    HEADER_PLAIN,
    HEADER_PLAIN,
    REPEAT_FOUR(HEADER_TWO_VOLUMES),
    HEADER_TWO_INTENSITIES,
    HEADER_TWO_INTENSITIES,
    HEADER_TWO_VOLUMES,
    HEADER_TWO_VOLUMES,
    REPEAT_SIXTEEN(HEADER_MODIFIER),
    REPEAT_SIXTEEN(HEADER_SPRITE),
    REPEAT_SIXTEEN(HEADER_MODIFIER_SPRITE)};
static struct polygon_header current;
/* current's format when kmiGlobalWork was last built from it */
static KMDWORD active_format;

#ifdef NONMATCHING
static KMDWORD color_bits(KMFLOAT color)
{
    union {
        KMFLOAT channel;
        KMDWORD word;
    } bits;
    bits.channel = color;
    return bits.word;
}
#define COLOR_BITS(color) color_bits(color)
#else
/* @hack type: a float's bits read through its address cast to a word
 * pointer, where the union of the NONMATCHING side would do */
#define COLOR_BITS(color) (*(KMDWORD *)&(color))
#endif
#define PUT_COLOR(first, rgb)                                                  \
    *out++ = COLOR_BITS((first).alpha);                                        \
    *out++ = COLOR_BITS((rgb).red);                                            \
    *out++ = COLOR_BITS((rgb).green);                                          \
    *out++ = COLOR_BITS((rgb).blue)
#define CHANNEL_BYTE(channel) ((KMDWORD)(long)((channel) * 255.0F))
#define PACK_COLOR(color)                                                      \
    ((CHANNEL_BYTE((color).alpha) << 24) | (CHANNEL_BYTE((color).red) << 16) | \
     (CHANNEL_BYTE((color).green) << 8) | CHANNEL_BYTE((color).blue))

/* @hack spelling: GLOBAL_COLOR's two bits masked and shifted one at a time,
 * where one mask of GLOBAL_COLOR, shifted the same, would do */
#define HEADER_INDEX(control)                                                  \
    (((control) & PARA_SPRITE_BIT) >> 24 |                                     \
     ((control) & ((KMDWORD)LIST_MODIFIER << GLOBAL_LIST_SHIFT)) >> 20 |       \
     ((control) & GLOBAL_VOLUME) >> 3 | ((control) & GLOBAL_COLOR_HIGH) >> 3 | \
     ((control) & GLOBAL_COLOR_LOW) >> 3 | ((control) & GLOBAL_OFFSET) >> 2)

/* write the header in state into kmiGlobalWork, laid out as its format says.
 * out starts at base->word and ends past the header. the header takes 32
 * bytes, or 64 for HEADER_INTENSITY_OFFSET and HEADER_TWO_INTENSITIES. a word
 * a format skips keeps what it held. a modifier list's header is its control
 * word, its instruction word, and six skipped words. kmiCurrentListType gets
 * the header's list, and GlobalParamSize the header's size */
#define BUILD_GLOBAL()                                                         \
    active_format = header_formats[HEADER_INDEX(state->control)];              \
    *out++ = state->control;                                                   \
    kmiCurrentListType = (state->control & GLOBAL_LIST) >> GLOBAL_LIST_SHIFT;  \
    if (!(kmiCurrentListType & LIST_MODIFIER)) {                               \
        struct header_volume *first;                                           \
        *out++ = state->volume[0].isp;                                         \
        *out++ = state->volume[0].tsp;                                         \
        *out++ = state->volume[0].texture;                                     \
        /* @hack spelling: first, where &state->volume[0] would do */          \
        first = &state->volume[0];                                             \
        switch (active_format) {                                               \
        case HEADER_PLAIN:                                                     \
            out += 4;                                                          \
            break;                                                             \
        case HEADER_INTENSITY:                                                 \
            PUT_COLOR(first->face, state->volume[0].face);                     \
            break;                                                             \
        case HEADER_INTENSITY_OFFSET:                                          \
            out += 4;                                                          \
            PUT_COLOR(first->face, state->volume[0].face);                     \
            PUT_COLOR(state->volume[0].offset, state->volume[0].offset);       \
            break;                                                             \
        case HEADER_TWO_VOLUMES:                                               \
            *out++ = state->volume[1].tsp;                                     \
            *out++ = state->volume[1].texture;                                 \
            out += 2;                                                          \
            break;                                                             \
        case HEADER_TWO_INTENSITIES:                                           \
            *out++ = state->volume[1].tsp;                                     \
            *out++ = state->volume[1].texture;                                 \
            out += 2;                                                          \
            PUT_COLOR(state->volume[0].face, state->volume[0].face);           \
            PUT_COLOR(state->volume[1].face, state->volume[1].face);           \
            break;                                                             \
        case HEADER_SPRITE:                                                    \
            *out++ = PACK_COLOR(first->face);                                  \
            *out++ = PACK_COLOR(state->volume[0].offset);                      \
            out += 2;                                                          \
            break;                                                             \
        }                                                                      \
    } else {                                                                   \
        *out++ = state->volume[1].isp;                                         \
        out += 6;                                                              \
    }                                                                          \
    kmiCurrentListState.ListType = kmiCurrentListType;                         \
    kmiCurrentListState.GlobalParamSize = (char *)out - (char *)base

/* the TSP word's size code for a side of dimension texels, log2(dimension)
 * - 3. it runs from 0 for 8 to 7 for 1024. any other dimension is returned
 * unchanged */
KMDWORD kmiConvertUVnum(KMDWORD dimension)
{
    if (dimension == 8) {
        dimension = 0;
    } else if (dimension == 16) {
        dimension = 1;
    } else if (dimension == 32) {
        dimension = 2;
    } else if (dimension == 64) {
        dimension = 3;
    } else if (dimension == 128) {
        dimension = 4;
    } else if (dimension == 256) {
        dimension = 5;
    } else if (dimension == 512) {
        dimension = 6;
    } else if (dimension == 1024) {
        dimension = 7;
    }
    return dimension;
}

void kmiCreateGlobalParameter(void)
{
    struct kmi_global_header *base = kmiGlobalWork;
    /* @hack redundant: register on out */
    register KMDWORD *out = base->word;
    struct polygon_header *state = &current;
    BUILD_GLOBAL();
}

/* word with field replaced by value. value must fit the field: FIELD doesn't
 * mask it */
#define FIELD(word, field, value)                                              \
    (((word) & ~(KMDWORD)(field)) | ((KMDWORD)(value) << field##_SHIFT))
/* word with field replaced by flag, a KM_SURFACEFLAGS_ bit or 0, moved up to
 * it */
#define FLAG_FIELD(word, field, flag)                                          \
    (((word) & ~(KMDWORD)(field)) | ((KMDWORD)(flag) << field##_FROM_FLAG))
#ifdef NONMATCHING
#define SIGNED_FIELD(word, field, value)                                       \
    (((word) & ~(KMDWORD)(field)) | ((KMDWORD)(value) << field##_SHIFT))
#else
/* @hack type: value shifted as the KMINT32 it is, where FIELD's KMDWORD would
 * do */
#define SIGNED_FIELD(word, field, value)                                       \
    (((word) & ~(KMDWORD)(field)) | ((value) << field##_SHIFT))
#endif
#define SET_FLAG(word, mask, enabled)                                          \
    if (enabled) {                                                             \
        (word) = (word) | (mask);                                              \
    } else {                                                                   \
        (word) = (word) & ~(KMDWORD)(mask);                                    \
    }

/* KM_BOTHINVSRCALPHA and KM_BOTHSRCALPHA set both factors, whichever of
 * SRCBlendingMode and DSTBlendingMode names them */
#define PAIRED_BLEND(word)                                                     \
    case KM_BOTHINVSRCALPHA:                                                   \
        (word) = FIELD(word, TSP_SRC, BLEND_INV_SRC_ALPHA);                    \
        (word) = FIELD(word, TSP_DST, BLEND_SRC_ALPHA);                        \
        break;                                                                 \
    case KM_BOTHSRCALPHA:                                                      \
        (word) = FIELD(word, TSP_SRC, BLEND_SRC_ALPHA);                        \
        (word) = FIELD(word, TSP_DST, BLEND_INV_SRC_ALPHA);                    \
        break
#define SRC_FACTORS(word)                                                      \
    PAIRED_BLEND(word);                                                        \
    case KM_DESTALPHA:                                                         \
        (word) = FIELD(word, TSP_SRC, BLEND_DST_ALPHA);                        \
        break;                                                                 \
    case KM_DESTCOLOR:                                                         \
        (word) = FIELD(word, TSP_SRC, BLEND_OTHER_COLOR);                      \
        break;                                                                 \
    case KM_INVDESTALPHA:                                                      \
        (word) = FIELD(word, TSP_SRC, BLEND_INV_DST_ALPHA);                    \
        break;                                                                 \
    case KM_INVDESTCOLOR:                                                      \
        (word) = FIELD(word, TSP_SRC, BLEND_INV_OTHER_COLOR);                  \
        break;                                                                 \
    case KM_INVSRCALPHA:                                                       \
        (word) = FIELD(word, TSP_SRC, BLEND_INV_SRC_ALPHA);                    \
        break;                                                                 \
    case KM_SRCALPHA:                                                          \
        (word) = FIELD(word, TSP_SRC, BLEND_SRC_ALPHA);                        \
        break;                                                                 \
    case KM_ONE:                                                               \
        (word) = FIELD(word, TSP_SRC, BLEND_ONE);                              \
        break;                                                                 \
    case KM_ZERO:                                                              \
        (word) = FIELD(word, TSP_SRC, BLEND_ZERO);                             \
        break
#define DST_FACTORS(word)                                                      \
    PAIRED_BLEND(word);                                                        \
    case KM_DESTALPHA:                                                         \
        (word) = FIELD(word, TSP_DST, BLEND_DST_ALPHA);                        \
        break;                                                                 \
    case KM_INVDESTALPHA:                                                      \
        (word) = FIELD(word, TSP_DST, BLEND_INV_DST_ALPHA);                    \
        break;                                                                 \
    case KM_INVSRCALPHA:                                                       \
        (word) = FIELD(word, TSP_DST, BLEND_INV_SRC_ALPHA);                    \
        break;                                                                 \
    case KM_INVSRCCOLOR:                                                       \
        (word) = FIELD(word, TSP_DST, BLEND_INV_OTHER_COLOR);                  \
        break;                                                                 \
    case KM_SRCALPHA:                                                          \
        (word) = FIELD(word, TSP_DST, BLEND_SRC_ALPHA);                        \
        break;                                                                 \
    case KM_SRCCOLOR:                                                          \
        (word) = FIELD(word, TSP_DST, BLEND_OTHER_COLOR);                      \
        break;                                                                 \
    case KM_ONE:                                                               \
        (word) = FIELD(word, TSP_DST, BLEND_ONE);                              \
        break;                                                                 \
    case KM_ZERO:                                                              \
        (word) = FIELD(word, TSP_DST, BLEND_ZERO);                             \
        break
#define IS_PALETTIZED(texture)                                                 \
    (((texture) & KM_PIXELFORMAT_MASK) == KM_PIXELFORMAT_PALETTIZED_4BPP ||    \
     ((texture) & KM_PIXELFORMAT_MASK) == KM_PIXELFORMAT_PALETTIZED_8BPP)
#define SET_PALETTE(texture, bank)                                             \
    FIELD(texture, TEX_PALETTE, (bank) & (TEX_PALETTE >> TEX_PALETTE_SHIFT))
/* a context whose polygons take its surface's texture: KM_TEXTUREFLAT or
 * KM_TEXTUREGOURAUD, and a surface */
#define TEXTURED(ctx)                                                          \
    (((ctx)->ShadingMode >> SHADING_TEXTURE_SHIFT) == 1 &&                     \
     (ctx)->pTextureSurfaceDesc)
/* the TSP word's filter code for each FilterMode */
#define FILTER_CODES {0, 2, 1, 3}

/* a textured context takes its texture's size, format and address from its
 * surface, whatever RenderState marks. the control and ISP words keep only
 * the bits of GLOBAL_KEPT and ISP_KEPT */
KMSTATUS kmProcessVertexRenderState(PKMVERTEXCONTEXT pVertexContext)
{
    KMDWORD value;
    KMDWORD changes;
    KMDWORD isp = pVertexContext->ISPPARAMBUFFER;
    KMDWORD tsp = pVertexContext->TSPPARAMBUFFER;
    KMDWORD texture = pVertexContext->TexturePARAMBUFFER;
    KMDWORD control = pVertexContext->GLOBALPARAMBUFFER;
    KMDWORD filter_codes[4] = FILTER_CODES;

    changes = pVertexContext->RenderState;
    if (changes & KM_DEPTHMODE) {
        isp = FIELD(isp, ISP_DEPTH, pVertexContext->DepthMode);
    }
    if (changes & KM_CULLINGMODE) {
        isp = FIELD(isp, ISP_CULL, pVertexContext->CullingMode);
    }
    if (changes & KM_ZWRITEDISABLE) {
        SET_FLAG(isp, ISP_Z_WRITE_DISABLE, pVertexContext->bZWriteDisable);
    }
    if (changes & KM_SHADINGMODE) {
        isp = FIELD(isp, ISP_TEXTURE,
                    (KMDWORD)pVertexContext->ShadingMode >>
                        SHADING_TEXTURE_SHIFT);
    }
    if (changes & KM_USESPECULAR) {
        isp = FIELD(isp, ISP_OFFSET, pVertexContext->bUseSpecular);
    }
    if (changes & KM_SHADINGMODE) {
        isp = FIELD(isp, ISP_GOURAUD,
                    (KMDWORD)pVertexContext->ShadingMode & SHADING_GOURAUD);
    }
    if (changes & KM_UVFORMAT) {
        isp = FIELD(isp, ISP_UV16, pVertexContext->UVFormat);
    }
    if (changes & KM_DCALCEXACT) {
        SET_FLAG(isp, ISP_DCALC_EXACT, pVertexContext->bDCalcExact);
    }

    if (changes & KM_SRCBLENDINGMODE) {
        switch ((KMDWORD)pVertexContext->SRCBlendingMode) {
            SRC_FACTORS(tsp);
        }
    }
    if (changes & KM_DSTBLENDINGMODE) {
        switch ((KMDWORD)pVertexContext->DSTBlendingMode) {
            DST_FACTORS(tsp);
        }
    }
    if (changes & KM_SRCSELECT) {
        SET_FLAG(tsp, TSP_SRC_SELECT, pVertexContext->bSRCSel);
    }
    if (changes & KM_DSTSELECT) {
        SET_FLAG(tsp, TSP_DST_SELECT, pVertexContext->bDSTSel);
    }
    if (changes & KM_FOGMODE) {
        value = (KMDWORD)pVertexContext->FogMode & (TSP_FOG >> TSP_FOG_SHIFT);
        tsp = FIELD(tsp, TSP_FOG, value);
    }
    if (changes & KM_COLORCLAMP) {
        SET_FLAG(tsp, TSP_CLAMP_COLOR, pVertexContext->bColorClamp);
    }
    if (changes & KM_USEALPHA) {
        SET_FLAG(tsp, TSP_USE_ALPHA, pVertexContext->bUseAlpha);
    }
    if (changes & KM_IGNORETEXTUREALPHA) {
        SET_FLAG(tsp, TSP_IGNORE_TEX_ALPHA,
                 pVertexContext->bIgnoreTextureAlpha);
    }
    if (changes & KM_CLAMPUV) {
        tsp = FIELD(tsp, TSP_CLAMP, pVertexContext->ClampUV);
    }
    if (changes & KM_FLIPUV) {
        tsp = FIELD(tsp, TSP_FLIP, pVertexContext->FlipUV);
    }
    if (changes & KM_FILTERMODE) {
        /* @hack spelling: the mode, then its entry in filter_codes, through
         * value in two steps, where one would do */
        value = pVertexContext->FilterMode;
        value = filter_codes[value];
        tsp = FIELD(tsp, TSP_FILTER, value);
    }
    if (changes & KM_SUPERSAMPLE) {
        SET_FLAG(tsp, TSP_SUPER_SAMPLE, pVertexContext->bSuperSample);
    }
    if (changes & KM_MIPMAPDADJUST) {
        value = pVertexContext->MipMapAdjust & (TSP_MIPMAP >> TSP_MIPMAP_SHIFT);
        if (value == 0) {
            value = KM_MIPMAP_D_ADJUST_1_00;
        }
        tsp = FIELD(tsp, TSP_MIPMAP, value);
    }
    if (changes & KM_TEXTURESHADINGMODE) {
        value = ((KMDWORD)pVertexContext->TextureShadingMode &
                 (TSP_SHADING >> TSP_SHADING_SHIFT))
                << TSP_SHADING_SHIFT;
        tsp = (tsp & ~(KMDWORD)TSP_SHADING) | value;
    }
    if (TEXTURED(pVertexContext)) {
        value = FIELD(
            tsp, TSP_U_SIZE,
            kmiConvertUVnum(pVertexContext->pTextureSurfaceDesc->u0.USize));
        tsp = FIELD(
            value, TSP_V_SIZE,
            kmiConvertUVnum(pVertexContext->pTextureSurfaceDesc->u1.VSize));
    }
    if (TEXTURED(pVertexContext)) {
        texture =
            FLAG_FIELD(texture, TEX_MIPMAP,
                       pVertexContext->pTextureSurfaceDesc->fSurfaceFlags &
                           KM_SURFACEFLAGS_MIPMAPED);
        texture =
            FLAG_FIELD(texture, TEX_VQ,
                       pVertexContext->pTextureSurfaceDesc->fSurfaceFlags &
                           KM_SURFACEFLAGS_VQ);
        texture = (texture & ~(KMDWORD)KM_PIXELFORMAT_MASK) |
                  (pVertexContext->pTextureSurfaceDesc->PixelFormat &
                   KM_PIXELFORMAT_MASK);
        if (IS_PALETTIZED(texture)) {
            texture = SET_PALETTE(texture, pVertexContext->PaletteBank);
        } else {
            texture = FLAG_FIELD(
                texture, TEX_SCAN_ORDER,
                ~(pVertexContext->pTextureSurfaceDesc->fSurfaceFlags &
                  KM_SURFACEFLAGS_TWIDDLED) &
                    KM_SURFACEFLAGS_TWIDDLED);
            texture =
                FLAG_FIELD(texture, TEX_STRIDE,
                           pVertexContext->pTextureSurfaceDesc->fSurfaceFlags &
                               KM_SURFACEFLAGS_STRIDE);
        }
        texture = (texture & ~(KMDWORD)TEX_ADDRESS) |
                  ((KMDWORD)pVertexContext->pTextureSurfaceDesc->pSurface >>
                   TEX_ADDRESS_SHIFT);
    }
    if (changes & KM_PARAMTYPE) {
        value = pVertexContext->ParamType;
        if (value == KM_POLYGON) {
            control = FIELD(control, GLOBAL_PARA, PARA_POLYGON);
        }
        if (value == KM_MODIFIERVOLUME) {
            control = FIELD(control, GLOBAL_PARA, KM_PARAM_MODIFIERVOLUME);
        }
        if (value == KM_SPRITE) {
            control = FIELD(control, GLOBAL_PARA, KM_PARAM_SPRITE);
        }
    }
    if (changes & KM_LISTTYPE) {
        control = FIELD(control, GLOBAL_LIST, pVertexContext->ListType);
    }
    control = control | GLOBAL_GROUP_EN;
    if (changes & KM_STRIPLENGTH) {
        control = FIELD(control, GLOBAL_STRIP, pVertexContext->StripLength);
    } else {
        control = FIELD(control, GLOBAL_STRIP, kmiDeviceExtension.strip_length);
    }
    if (changes & KM_USERCLIPMODE) {
        control =
            FIELD(control, GLOBAL_USER_CLIP, pVertexContext->UserClipMode);
    }
    if ((pVertexContext->ModifierInstruction & MODIFIER_LAST) &&
        ((KMDWORD)pVertexContext->ListType & LIST_MODIFIER)) {
        control = (control & ~(KMDWORD)(GLOBAL_SHADOW | GLOBAL_VOLUME)) |
                  GLOBAL_VOLUME;
    } else {
        control = control & ~(KMDWORD)(GLOBAL_SHADOW | GLOBAL_VOLUME);
    }
    if (changes & KM_MODIFIER) {
        if ((KMDWORD)pVertexContext->SelectModifier & MODIFIER_SELECTED) {
            control =
                FIELD(control, GLOBAL_SHADOW, pVertexContext->SelectModifier);
            control = FIELD(control, GLOBAL_VOLUME, shadow_disabled);
        }
    }
    if (changes & KM_COLORTYPE) {
        control = FIELD(control, GLOBAL_COLOR, pVertexContext->ColorType);
    }
    if (changes & KM_SHADINGMODE) {
        control = FIELD(control, GLOBAL_TEXTURE,
                        (KMDWORD)pVertexContext->ShadingMode >>
                            SHADING_TEXTURE_SHIFT);
    }
    if (changes & KM_SHADINGMODE) {
        control = FIELD(control, GLOBAL_GOURAUD,
                        (KMDWORD)pVertexContext->ShadingMode & SHADING_GOURAUD);
    }
    if (changes & KM_USESPECULAR) {
        if (pVertexContext->bUseSpecular) {
            control |= GLOBAL_OFFSET;
        } else {
            control &= ~((KMDWORD)GLOBAL_OFFSET);
        }
    }
    if (changes & KM_UVFORMAT) {
        control = FIELD(control, GLOBAL_UV16, pVertexContext->UVFormat);
    }

    value = control & GLOBAL_KEPT;
    pVertexContext->ISPPARAMBUFFER = isp & ISP_KEPT;
    pVertexContext->TSPPARAMBUFFER = tsp;
    pVertexContext->TexturePARAMBUFFER = texture;
    pVertexContext->GLOBALPARAMBUFFER = value;
    return KMSTATUS_SUCCESS;
}

#define LOAD_COLOR(color, field)                                               \
    (color).alpha = pVertexContext->field##Alpha;                              \
    (color).red = pVertexContext->field##Red;                                  \
    (color).green = pVertexContext->field##Green;                              \
    (color).blue = pVertexContext->field##Blue

KMSTATUS kmSetVertexRenderState(PKMVERTEXCONTEXT pVertexContext)
{
    struct polygon_header *state = &current;
    struct kmi_global_header *base;
    /* @hack redundant: register on out */
    register KMDWORD *out;

    state->control = pVertexContext->GLOBALPARAMBUFFER;
    state->volume[0].isp = pVertexContext->ISPPARAMBUFFER;
    state->volume[0].tsp = pVertexContext->TSPPARAMBUFFER;
    if (!((state->control >> GLOBAL_LIST_SHIFT) &
          (GLOBAL_LIST >> GLOBAL_LIST_SHIFT))) {
        state->volume[0].tsp = (state->volume[0].tsp & ~(KMDWORD)TSP_BLEND) |
                               ((KMDWORD)BLEND_ONE << TSP_SRC_SHIFT);
    }
    state->volume[0].texture = pVertexContext->TexturePARAMBUFFER;
    LOAD_COLOR(state->volume[0].face, fFaceColor);
    LOAD_COLOR(state->volume[0].offset, fOffsetColor);
    state->volume[1].isp = pVertexContext->ModifierInstruction;
    base = kmiGlobalWork;
    out = base->word;
    BUILD_GLOBAL();
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetModifierRenderState(PKMVERTEXCONTEXT pVertexContext)
{
    struct polygon_header *state = &current;
    struct kmi_global_header *base;
    /* @hack redundant: register on out */
    register KMDWORD *out;

    state->volume[1].tsp = pVertexContext->TSPPARAMBUFFER;
    state->volume[1].texture = pVertexContext->TexturePARAMBUFFER;
    LOAD_COLOR(state->volume[1].face, fFaceColor);
    base = kmiGlobalWork;
    out = base->word;
    BUILD_GLOBAL();
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetStripLength(KMDWORD length)
{
    kmiDeviceExtension.strip_length = length;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetCheapShadowMode(KMINT32 nIntensity)
{
    KMDWORD value;
    if (nIntensity < 0) {
        shadow_disabled = 1;
    } else {
        shadow_disabled = 0;
    }
    value = ((1 - shadow_disabled) << SHADOW_ENABLE_SHIFT) | (KMBYTE)nIntensity;
    kmiWriteRegister(KMI_SHADOW_SCALE, value);
    return KMSTATUS_SUCCESS;
}

KMDWORD **kmiPutGlobalParam(void)
{
    kmiDstVertexBuffer = kmiCurrentVertexBuff + kmiCurrentListType;
    kmiMemCopy8(*kmiDstVertexBuffer, kmiGlobalWork->word,
                kmiGlobalNum * sizeof(KMDWORD));
    *kmiDstVertexBuffer += kmiGlobalNum;
    return kmiDstVertexBuffer;
}

#define PRIMARY_ISP current.volume[0].isp
#define PRIMARY_TSP current.volume[0].tsp
#define PRIMARY_TEXTURE current.volume[0].texture

KMSTATUS kmChangeContextColorType(KMDWORD color)
{
    current.control = FIELD(current.control, GLOBAL_COLOR, color);
    kmiGlobalWork->word[WORD_CONTROL] = current.control;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmChangeContextDepthMode(KMINT32 compare)
{
    PRIMARY_ISP = SIGNED_FIELD(PRIMARY_ISP, ISP_DEPTH, compare);
    kmiGlobalWork->word[WORD_ISP] = PRIMARY_ISP;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmChangeContextCullingMode(KMINT32 culling)
{
    PRIMARY_ISP = SIGNED_FIELD(PRIMARY_ISP, ISP_CULL, culling);
    kmiGlobalWork->word[WORD_ISP] = PRIMARY_ISP;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmChangeContextZWriteDisable(KMBOOLEAN disable)
{
    SET_FLAG(PRIMARY_ISP, ISP_Z_WRITE_DISABLE, disable);
    kmiGlobalWork->word[WORD_ISP] = PRIMARY_ISP;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmChangeContextSRCBlendMode(KMINT32 mode)
{
    switch (mode) {
        SRC_FACTORS(PRIMARY_TSP);
    /* @hack redundant: an empty default */
    default:
        break;
    }
    kmiGlobalWork->word[WORD_TSP] = PRIMARY_TSP;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmChangeContextDSTBlendMode(KMINT32 mode)
{
    switch (mode) {
        DST_FACTORS(PRIMARY_TSP);
    /* @hack redundant: an empty default */
    default:
        break;
    }
    kmiGlobalWork->word[WORD_TSP] = PRIMARY_TSP;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmChangeContextFogMode(KMDWORD fog)
{
    PRIMARY_TSP = FIELD(PRIMARY_TSP, TSP_FOG, fog);
    kmiGlobalWork->word[WORD_TSP] = PRIMARY_TSP;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmChangeContextFlipUV(KMINT32 flip)
{
    PRIMARY_TSP = FIELD(PRIMARY_TSP, TSP_FLIP, flip);
    kmiGlobalWork->word[WORD_TSP] = PRIMARY_TSP;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmChangeContextClampUV(KMINT32 clamp)
{
    PRIMARY_TSP = SIGNED_FIELD(PRIMARY_TSP, TSP_CLAMP, clamp);
    kmiGlobalWork->word[WORD_TSP] = PRIMARY_TSP;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmChangeContextFilterMode(KMINT32 filter)
{
    KMDWORD codes[4] = FILTER_CODES;
    filter = codes[filter];
    PRIMARY_TSP = FIELD(PRIMARY_TSP, TSP_FILTER, filter);
    kmiGlobalWork->word[WORD_TSP] = PRIMARY_TSP;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmChangeContextSuperSample(KMBOOLEAN enable)
{
    SET_FLAG(PRIMARY_TSP, TSP_SUPER_SAMPLE, enable);
    kmiGlobalWork->word[WORD_TSP] = PRIMARY_TSP;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmChangeContextTextureShadingMode(KMINT32 shading)
{
    PRIMARY_TSP = FIELD(PRIMARY_TSP, TSP_SHADING, shading);
    kmiGlobalWork->word[WORD_TSP] = PRIMARY_TSP;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmChangeContextColorClamp(KMBOOLEAN enable)
{
    SET_FLAG(PRIMARY_TSP, TSP_CLAMP_COLOR, enable);
    kmiGlobalWork->word[WORD_TSP] = PRIMARY_TSP;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmChangeContextPaletteBank(KMDWORD bank)
{
    if (IS_PALETTIZED(PRIMARY_TEXTURE)) {
        PRIMARY_TEXTURE = SET_PALETTE(PRIMARY_TEXTURE, bank);
        kmiGlobalWork->word[WORD_TEXTURE] = PRIMARY_TEXTURE;
    }
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmChangeContextStripLength(KMDWORD length)
{
    current.control =
        FIELD(current.control, GLOBAL_STRIP, length) | GLOBAL_GROUP_EN;
    kmiGlobalWork->word[WORD_CONTROL] = current.control;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmChangeContextUserClipMode(KMDWORD clip)
{
    current.control =
        FIELD(current.control, GLOBAL_USER_CLIP, clip) | GLOBAL_GROUP_EN;
    kmiGlobalWork->word[WORD_CONTROL] = current.control;
    return KMSTATUS_SUCCESS;
}
