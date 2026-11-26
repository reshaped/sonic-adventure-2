/* a texture surface's description from its format and size, and the bytes a
 * texture takes */
#include <kamui.h>
#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>

/* the member exports these, though no header of the SDK declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the bytes surface's texture takes in texture memory, from the fields
 * njCheckTextureFormatLow sets. njTextureSizeAddDummy takes the size from one
 * of the other four, by the surface's type and flags */
Uint32 njTextureSizeAddDummy(NJS_TEXSURFACE *surface);
Uint32 njTextureSize_Normal(NJS_TEXSURFACE *surface);
Uint32 njTextureSize_VQ(NJS_TEXSURFACE *surface);
Uint32 njTextureSize_Palettized(NJS_TEXSURFACE *surface);
Uint32 njTextureSize_SVQ(NJS_TEXSURFACE *surface);
/* njCalcTextureSize of the width, height and type a PVR texture chunk's
 * header holds */
Uint32 njCalcTextureSize2(NJS_PVRHEADERDATA *pvrh);
/* NOLINTEND(misc-use-internal-linkage) */

/* the texels each byte of a VQ texture's indices stands for */
#define VQ_INDEX_TEXELS 4
/* the texels a byte of a 4-bit palettized texture holds */
#define PALETTIZE4_BYTE_TEXELS 2
/* the bytes a VQ texture's codebook takes */
#define VQ_CODEBOOK 0x800
/* the dummy bytes njTextureSizeAddDummy's sizes count beyond
 * njCalcTextureSize's, as nj_texlist.h's SMALLVQ_MM_DUMMY does for a small VQ
 * texture. a palettized texture's are counted at a byte a texel, before a
 * 4-bit texture's size is halved */
#define VQ_MM_DUMMY 10
#define VQ_8X8_DUMMY 16
#define PALETTIZE4_MM_DUMMY 40
#define PALETTIZE8_MM_DUMMY 8

/* return the bytes in texture memory of an nWidth by nHeight texture of type,
 * an NJD_TEXFMT_ texture type. level is the texture's, as njGetTextureLevel
 * gives it. a type the switch doesn't know returns 0. njCalcTextureSize and
 * njCalcTextureSize2 share it */
#define RETURN_TEXTURE_SIZE(nWidth, nHeight, type, level)                      \
    /* @hack redundant: a break after each case's return */                    \
    switch (type) {                                                            \
    case NJD_TEXFMT_TWIDDLED:                                                  \
    case NJD_TEXFMT_TWIDDLED_RECTANGLE:                                        \
    case NJD_TEXFMT_RECTANGLE:                                                 \
    case NJD_TEXFMT_STRIDE:                                                    \
        return (nWidth) * (nHeight) * TEXEL_BYTES;                             \
        break;                                                                 \
    case NJD_TEXFMT_TWIDDLED_MM:                                               \
        return _nj_ImageByteSize[level];                                       \
        break;                                                                 \
    case NJD_TEXFMT_VQ:                                                        \
        return ((nWidth) * (nHeight) / VQ_INDEX_TEXELS) + VQ_CODEBOOK;         \
        break;                                                                 \
    case NJD_TEXFMT_SMALLVQ:                                                   \
        return _nj_SVQCodeBookSize[level] + _nj_SVQIndexSize[level];           \
        break;                                                                 \
    case NJD_TEXFMT_VQ_MM:                                                     \
        return ((_nj_ImageByteSize[level] + TWIDDLED_MM_SKIP) /                \
                (TEXEL_BYTES * VQ_INDEX_TEXELS)) +                             \
               VQ_CODEBOOK;                                                    \
        break;                                                                 \
    case NJD_TEXFMT_SMALLVQ_MM:                                                \
        return _nj_SVQCodeBookSizeMM[level] + _nj_SVQIndexSizeMM[level];       \
        break;                                                                 \
    case NJD_TEXFMT_PALETTIZE4:                                                \
        return (nWidth) * (nHeight) / PALETTIZE4_BYTE_TEXELS;                  \
        break;                                                                 \
    case NJD_TEXFMT_PALETTIZE4_MM:                                             \
        return (_nj_ImageByteSize[level] + TWIDDLED_MM_SKIP) /                 \
               (TEXEL_BYTES * PALETTIZE4_BYTE_TEXELS);                         \
        break;                                                                 \
    case NJD_TEXFMT_PALETTIZE8:                                                \
        return (nWidth) * (nHeight);                                           \
        break;                                                                 \
    case NJD_TEXFMT_PALETTIZE8_MM:                                             \
        return (_nj_ImageByteSize[level] + TWIDDLED_MM_SKIP) / TEXEL_BYTES;    \
        break;                                                                 \
    default:                                                                   \
        break;                                                                 \
    }                                                                          \
    return 0

Sint32 njCheckTextureFormatLow(NJS_TEXSURFACE *surface, Uint32 nWidth,
                               Uint32 nHeight, Uint32 nTextureType)
{
    Uint32 color;
    Uint32 type;
    Sint32 status;

    color = nTextureType & NJD_TEXFMT_COLOR_MASK;
    type = nTextureType & NJD_TEXFMT_TYPE_MASK;
    status = KMSTATUS_SUCCESS;
    surface->Type = nTextureType << TEXSURFACE_FORMAT_SHIFT;
    if (type == NJD_TEXFMT_SMALLVQ || type == NJD_TEXFMT_SMALLVQ_MM) {
        surface->Type |= KM_SURFACETYPE_SMALLVQ_TEXTURE;
    } else {
        surface->Type |= KM_SURFACETYPE_TEXTURE;
    }
    surface->BitDepth = KM_BITDEPTH_16;
    if (type == NJD_TEXFMT_PALETTIZE4 || type == NJD_TEXFMT_PALETTIZE4_MM) {
        surface->PixelFormat = KM_PIXELFORMAT_PALETTIZED_4BPP;
    } else if (type == NJD_TEXFMT_PALETTIZE8 ||
               type == NJD_TEXFMT_PALETTIZE8_MM) {
        surface->PixelFormat = KM_PIXELFORMAT_PALETTIZED_8BPP;
    } else {
        switch (color) {
        case NJD_TEXFMT_ARGB_1555:
            surface->PixelFormat = KM_PIXELFORMAT_ARGB1555;
            break;
        case NJD_TEXFMT_RGB_565:
            surface->PixelFormat = KM_PIXELFORMAT_RGB565;
            break;
        case NJD_TEXFMT_ARGB_4444:
            surface->PixelFormat = KM_PIXELFORMAT_ARGB4444;
            break;
        case NJD_TEXFMT_YUV_422:
            surface->PixelFormat = KM_PIXELFORMAT_YUV422;
            break;
        case NJD_TEXFMT_BUMP:
            surface->PixelFormat = KM_PIXELFORMAT_BUMP;
            break;
        default:
            status = KMSTATUS_INVALID_TEXTURE_TYPE;
            break;
        }
    }
    surface->nWidth = nWidth;
    surface->nHeight = nHeight;
    switch (nWidth) {
    case 8:
    case 16:
    case 32:
    case 64:
    case 128:
    case 256:
    case 512:
    case 1024:
        break;
    default:
        status = KMSTATUS_INVALID_TEXTURE_TYPE;
        break;
    }
    switch (nHeight) {
    case 8:
    case 16:
    case 32:
    case 64:
    case 128:
    case 256:
    case 512:
    case 1024:
        break;
    default:
        status = KMSTATUS_INVALID_TEXTURE_TYPE;
        break;
    }
    if (type != NJD_TEXFMT_RECTANGLE && type != NJD_TEXFMT_STRIDE &&
        type != NJD_TEXFMT_TWIDDLED_RECTANGLE &&
        type != NJD_TEXFMT_PALETTIZE4 && type != NJD_TEXFMT_PALETTIZE8 &&
        nWidth != nHeight) {
        status = KMSTATUS_INVALID_TEXTURE_TYPE;
    }
    surface->fSurfaceFlags = 0;
    if (type == NJD_TEXFMT_ABGR_MM || type == NJD_TEXFMT_TWIDDLED_MM ||
        type == NJD_TEXFMT_VQ_MM || type == NJD_TEXFMT_SMALLVQ_MM ||
        type == NJD_TEXFMT_PALETTIZE4_MM || type == NJD_TEXFMT_PALETTIZE8_MM) {
        surface->fSurfaceFlags |= NJD_SURFACEFLAGS_MIPMAPED;
    }
    if (type == NJD_TEXFMT_ABGR || type == NJD_TEXFMT_ABGR_MM ||
        type == NJD_TEXFMT_TWIDDLED || type == NJD_TEXFMT_TWIDDLED_MM ||
        type == NJD_TEXFMT_TWIDDLED_RECTANGLE) {
        surface->fSurfaceFlags |= NJD_SURFACEFLAGS_TWIDDLED;
    }
    if (type == NJD_TEXFMT_VQ || type == NJD_TEXFMT_VQ_MM ||
        type == NJD_TEXFMT_SMALLVQ || type == NJD_TEXFMT_SMALLVQ_MM) {
        surface->fSurfaceFlags |= NJD_SURFACEFLAGS_VQ;
        surface->fSurfaceFlags |= NJD_SURFACEFLAGS_TWIDDLED;
    }
    if (type == NJD_TEXFMT_STRIDE) {
        surface->fSurfaceFlags |= NJD_SURFACEFLAGS_STRIDE;
    }
    if (type == NJD_TEXFMT_PALETTIZE4 || type == NJD_TEXFMT_PALETTIZE4_MM ||
        type == NJD_TEXFMT_PALETTIZE8 || type == NJD_TEXFMT_PALETTIZE8_MM) {
        surface->fSurfaceFlags |= NJD_SURFACEFLAGS_PALETTIZED;
        surface->fSurfaceFlags |= NJD_SURFACEFLAGS_TWIDDLED;
    }
    if (type == NJD_TEXFMT_TWIDDLED_MM_DMA) {
        surface->fSurfaceFlags |= NJD_SURFACEFLAGS_MIPMAPED;
        surface->fSurfaceFlags |= NJD_SURFACEFLAGS_TWIDDLED;
    }
    surface->TextureSize = njTextureSizeAddDummy(surface);
    return status;
}

Sint32 njGetTextureLevel(Uint32 nWidth, Uint32 nHeight)
{
    if (nWidth >= nHeight) {
        nWidth = nHeight;
    }
    switch (nWidth) {
    case 1:
        return 0;
    case 2:
        return 1;
    case 4:
        return 2;
    case 8:
        return 3;
    case 16:
        return 4;
    case 32:
        return 5;
    case 64:
        return 6;
    case 128:
        return 7;
    case 256:
        return 8;
    case 512:
        return 9;
    case 1024:
        return 10;
    }
    return -1;
}

Uint32 njTextureSizeAddDummy(NJS_TEXSURFACE *surface)
{
    Uint32 size;

    if (SURFACE_KIND(surface) == KM_SURFACETYPE_SMALLVQ_TEXTURE) {
        size = njTextureSize_SVQ(surface);
    } else if (surface->fSurfaceFlags & NJD_SURFACEFLAGS_VQ) {
        size = njTextureSize_VQ(surface);
    } else if (surface->fSurfaceFlags & NJD_SURFACEFLAGS_PALETTIZED) {
        size = njTextureSize_Palettized(surface);
    } else {
        size = njTextureSize_Normal(surface);
    }
    return size;
}

Uint32 njCalcTextureSize(Uint32 nWidth, Uint32 nHeight, Uint32 nTextureType)
{
    Sint32 level;
    Uint32 type;

    level = njGetTextureLevel(nWidth, nHeight);
    type = nTextureType & NJD_TEXFMT_TYPE_MASK;
    RETURN_TEXTURE_SIZE(nWidth, nHeight, type, level);
}

Uint32 njCalcTextureSize2(NJS_PVRHEADERDATA *pvrh)
{
    Uint32 nWidth;
    Uint32 nHeight;
    Uint32 nTextureType;
    Sint32 level;
    Uint32 type;

    nWidth = pvrh->nWidth;
    nHeight = pvrh->nHeight;
    nTextureType = pvrh->nTextureType;
    level = njGetTextureLevel(nWidth, nHeight);
    type = nTextureType & NJD_TEXFMT_TYPE_MASK;
    RETURN_TEXTURE_SIZE(nWidth, nHeight, type, level);
}

Uint32 njTextureSize_Normal(NJS_TEXSURFACE *surface)
{
    Uint32 nWidth;
    Uint32 nHeight;
    Sint32 level;
    Uint32 size;

    nWidth = surface->nWidth;
    nHeight = surface->nHeight;
    if (surface->fSurfaceFlags & NJD_SURFACEFLAGS_MIPMAPED) {
        level = njGetTextureLevel(nWidth, nHeight);
        size = _nj_ImageByteSize[level] + TWIDDLED_MM_SKIP;
    } else {
        size = nWidth * nHeight * TEXEL_BYTES;
    }
    /* @hack redundant: a read of surface's nWidth, which nothing uses */
    (void)surface->nWidth;
    return size;
}

Uint32 njTextureSize_VQ(NJS_TEXSURFACE *surface)
{
    Uint32 nWidth;
    Uint32 nHeight;
    Sint32 level;
    Uint32 size;

    nWidth = surface->nWidth;
    nHeight = surface->nHeight;
    if (surface->fSurfaceFlags & NJD_SURFACEFLAGS_MIPMAPED) {
        level = njGetTextureLevel(nWidth, nHeight);
        size = ((_nj_ImageByteSize[level] + TWIDDLED_MM_SKIP) /
                (TEXEL_BYTES * VQ_INDEX_TEXELS)) +
               VQ_MM_DUMMY;
    } else {
        size = nWidth * nHeight / VQ_INDEX_TEXELS;
        /* @hack spelling: surface's nWidth and nHeight read again, where the
         * locals would do */
        if (surface->nWidth == 8 && surface->nHeight == 8) {
            size += VQ_8X8_DUMMY;
        }
    }
    size += VQ_CODEBOOK;
    return size;
}

Uint32 njTextureSize_Palettized(NJS_TEXSURFACE *surface)
{
    Uint32 nWidth;
    Uint32 nHeight;
    Sint32 level;
    Uint32 size;

    nWidth = surface->nWidth;
    nHeight = surface->nHeight;
    if (surface->fSurfaceFlags & NJD_SURFACEFLAGS_MIPMAPED) {
        level = njGetTextureLevel(nWidth, nHeight);
        size = (_nj_ImageByteSize[level] + TWIDDLED_MM_SKIP) / TEXEL_BYTES;
        if (surface->PixelFormat == KM_PIXELFORMAT_PALETTIZED_4BPP) {
            size += PALETTIZE4_MM_DUMMY;
        } else {
            size += PALETTIZE8_MM_DUMMY;
        }
    } else {
        size = nWidth * nHeight;
    }
    if (surface->PixelFormat == KM_PIXELFORMAT_PALETTIZED_4BPP) {
        size /= PALETTIZE4_BYTE_TEXELS;
    }
    return size;
}

Uint32 njTextureSize_SVQ(NJS_TEXSURFACE *surface)
{
    Sint32 level;
    Uint32 size;

    level = njGetTextureLevel(surface->nWidth, surface->nHeight);
    if (surface->fSurfaceFlags & NJD_SURFACEFLAGS_MIPMAPED) {
        size = _nj_SVQCodeBookSizeMM[level] + _nj_SVQIndexSizeMM[level] +
               SMALLVQ_MM_DUMMY;
    } else {
        size = _nj_SVQCodeBookSize[level] + _nj_SVQIndexSize[level];
    }
    return size;
}
