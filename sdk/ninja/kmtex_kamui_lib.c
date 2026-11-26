/* Kamui's texture surfaces: their description, allocation, loads, reloads and
 * reads */
#include <kamui.h>
#include <kamui_internal.h>
#include <kamui_texture.h>
#include <sg_xpt.h>

/* the bytes of a full VQ codebook. a VQ texture's indices start this far into
 * its surface. so do a small VQ texture's, though its codebook is shorter */
#define VQ_CODEBOOK_BYTES 2048
/* where loads and reads of a twiddled 16-bit mip chain start. the chain's first
 * long holds no level */
#define MIP16_START 4
/* KMI_TEXTURE_CONTROL's stride field: the width of every stride texture, in
 * units of 1 << STRIDE_SHIFT texels */
#define STRIDE 0x1f
#define STRIDE_SHIFT 5
/* KMI_TA_YUV_CONTROL's fields. the last macroblock column is in bits 0 to 5.
 * the last row is in bits 8 to 13 */
#define YUV_DIMENSION_MASK 0x3f
#define YUV_ROWS_SHIFT 8
#define YUV_MULTIPLE_SHIFT 16 /* several textures rather than one */
#define YUV_422_SHIFT 24 /* 4:2:2 texels rather than 4:2:0 */
/* KMI_TA_YUV_BASE's field: an offset below 16 MB, on 8 bytes */
#define YUV_BASE_BITS 0x00fffff8
/* a macroblock's side in texels, and its bytes of 4:2:0 and of 4:2:2 texels */
#define MACROBLOCK_SIDE 16
#define MACROBLOCK_420 384
#define MACROBLOCK_422 512
/* the tile accelerator's YUV converter input, as a DMA target */
#define TA_YUV_INPUT 0x10800000
/* the bytes of a DMA transfer unit, which a DMA source starts on */
#define TRANSFER_BYTES 32
/* kmLoadTextureBlock's block_size counts units of this many bytes */
#define BLOCK_UNIT 32
#define TEXEL_BYTES 2 /* a 16-bit texel's bytes */
#define CACHE_LINE 32 /* the bytes of an operand cache line */
/* a texture type's layout, in its high byte, and its texel format, in its low
 * byte */
#define TYPE_LAYOUT 0xff00
#define TYPE_FORMAT 0xff
/* the units ADD_SMALL_LEVELS adds for a square chain's last level, in the
 * format's units. a 16-bit chain's 1 by 1 level takes the room of 4 texels, 3
 * of them padding. a VQ chain counts index bytes, one for each 2 by 2 block of
 * texels. its 2 by 2 level takes one, and its 1 by 1 level one more. an 8-bit
 * palettized chain counts bytes, and a 4-bit one texels, one each */
#define MIP16_LAST 4
#define VQ_LAST 2
#define PALETTE_LAST 1
/* the units that pad a mip chain to a multiple of 32 bytes. a VQ chain's are
 * index bytes, an 8-bit palettized chain's bytes, and a 4-bit one's texels.
 * VQ_8_PAD pads the indices of an 8 by 8 VQ texture without mipmaps */
#define VQ_MIP_PAD 10
#define PALETTE8_MIP_PAD 11
#define PALETTE4_MIP_PAD 43
#define VQ_8_PAD 16
/* SurfaceType's kind, a KM_SURFACETYPE_ value, in its low half. kmGetTexture
 * tests only the kind's low bits, TYPE_LOW_BITS, for a frame buffer */
#define SURFACE_KIND_MASK 0xffff
#define TYPE_LOW_BITS 3
#define SURFACE_KIND(surface) ((surface)->SurfaceType & SURFACE_KIND_MASK)
#define VRAM(surface) ((KMDWORD)(surface)->pSurface)
/* a twiddled 16-bit texture with mipmaps */
#define MIP16_CHAIN(surface)                                                   \
    (((surface)->fSurfaceFlags & KM_SURFACEFLAGS_TWIDDLED) &&                  \
     ((surface)->fSurfaceFlags & KM_SURFACEFLAGS_MIPMAPED) &&                  \
     !((surface)->fSurfaceFlags & KM_SURFACEFLAGS_VQ) &&                       \
     !((surface)->fSurfaceFlags & KM_SURFACEFLAGS_PALETTIZED))

static KMSTATUS describe_surface(PKMSURFACEDESC surface, KMINT32 width,
                                 KMINT32 height, KMTEXTURETYPE type);
static KMINT32 side_level(KMINT32 width, KMINT32 height);
static void size_surface(PKMSURFACEDESC surface);
static KMDWORD plain_size(PKMSURFACEDESC surface);
static KMDWORD vq_size(PKMSURFACEDESC surface);
static KMDWORD palette_size(PKMSURFACEDESC surface);
static KMDWORD small_vq_size(PKMSURFACEDESC surface);
static KMINT32 write_mip16(KMDWORD offset, KMDWORD *texels, KMINT32 width);
static KMINT32 read_mip16(KMDWORD *texels, KMDWORD offset, KMINT32 width);
static KMINT32 write_small_vq(KMDWORD offset, KMBYTE *texels, KMINT32 width,
                              KMDWORD flags);
static KMINT32 read_small_vq(KMBYTE *texels, KMDWORD offset, KMINT32 width,
                             KMDWORD flags);
static KMSTATUS reload_palette4(PKMSURFACEDESC surface, KMBYTE *texels,
                                KMINT32 width);
static KMSTATUS reload_palette8(PKMSURFACEDESC surface, KMBYTE *texels,
                                KMINT32 width);
static KMSTATUS reload_twiddled(PKMSURFACEDESC surface, KMBYTE *texels,
                                KMINT32 width);
static KMSTATUS reload_vq(PKMSURFACEDESC surface, KMBYTE *texels,
                          KMINT32 width);
#ifdef NONMATCHING
#else
static KMINT32 quantize_channel(KMBYTE value);
#endif

/* the tables below take a level. level n is 1 << n texels a side, from 0 to
 * 10 */

/* a small VQ texture's codebook bytes, without mipmaps and with them, up to
 * level 6 */
static const KMDWORD small_codebook[7] = {0, 0, 0, 128, 128, 256, 1024};
static const KMDWORD small_mip_codebook[7] = {0, 0, 0, 128, 128, 512, 2048};
/* a twiddled 16-bit level's offset in its chain, and its bytes */
static const KMDWORD texel16_offset[11] = {
    6, 8, 16, 48, 176, 688, 2736, 10928, 43696, 174768, 699056};
static const KMDWORD texel16_bytes[11] = {
    2, 8, 32, 128, 512, 2048, 8192, 32768, 131072, 524288, 2097152};
/* a twiddled 16-bit chain's bytes from MIP16_START through the end of a level.
 * below level 3, smaller than any texture describe_surface takes, it is 0 */
static const KMDWORD mip16_bytes[11] = {
    0, 0, 0, 172, 684, 2732, 10924, 43692, 174764, 699052, 2796204};
#ifdef NONMATCHING
#else
/* a level's number mod 2 */
static const KMDWORD level_parity[11] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
#endif
/* a VQ level's index bytes, and where its indices end, counted from the chain's
 * first index */
static const KMDWORD index_bytes[11] = {0,    1,    4,     16,    64,    256,
                                        1024, 4096, 16384, 65536, 262144};
static const KMDWORD index_end[11] = {1,    2,    6,     22,    86,    342,
                                      1366, 5462, 21846, 87382, 349526};
/* an 8-bit palettized level's offset in its chain, and its bytes */
static const KMDWORD palette8_offset[11] = {
    3, 4, 8, 24, 88, 344, 1368, 5464, 21848, 87384, 349528};
static const KMDWORD palette8_bytes[11] = {
    1, 4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 1048576};
/* a 4-bit palettized level's offset in its chain, and its bytes */
static const KMDWORD palette4_offset[11] = {1,   2,    4,     12,    44,    172,
                                            684, 2732, 10924, 43692, 174764};
static const KMDWORD palette4_bytes[11] = {
    1, 2, 8, 32, 128, 512, 2048, 8192, 32768, 131072, 524288};

KMSTATUS kmSetStrideWidth(KMINT32 nWidth)
{
    KMINT32 stride = (nWidth & (STRIDE << STRIDE_SHIFT)) >> STRIDE_SHIFT;
    KMDWORD value;
    value = (kmiReadRegister(KMI_TEXTURE_CONTROL) & ~STRIDE) | stride;
    kmiWriteRegister(KMI_TEXTURE_CONTROL, value);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmCreateTextureSurface(PKMSURFACEDESC pSurfaceDesc, KMINT32 nWidth,
                                KMINT32 nHeight, KMTEXTURETYPE nTextureType)
{
    KMSTATUS status;
    status = describe_surface(pSurfaceDesc, nWidth, nHeight, nTextureType);
    if (status != KMSTATUS_SUCCESS) {
        return status;
    }
    if (SURFACE_KIND(pSurfaceDesc) == KM_SURFACETYPE_SMALLVQ_TEXTURE) {
        status = kmiVQalloc(pSurfaceDesc);
    } else {
        status = kmiTXalloc(pSurfaceDesc->uSize.dwTextureSize, pSurfaceDesc);
    }
    if (status == KMSTATUS_NOT_ENOUGH_MEMORY) {
        kmiDefaultInterruptHandler(TEXTURE_OVERFLOW_CALLBACK);
    }
    return status;
}

#define COPY_DESCRIPTION(to, from)                                             \
    (to)->SurfaceType = (from)->SurfaceType;                                   \
    (to)->BitDepth = (from)->BitDepth;                                         \
    (to)->PixelFormat = (from)->PixelFormat;                                   \
    (to)->u0.USize = (from)->u0.USize;                                         \
    (to)->u1.VSize = (from)->u1.VSize;                                         \
    (to)->uSize.dwTextureSize = (from)->uSize.dwTextureSize;                   \
    (to)->fSurfaceFlags = (from)->fSurfaceFlags

KMSTATUS kmCreateCombinedTextureSurface(PKMSURFACEDESC first,
                                        PKMSURFACEDESC second, KMINT32 width,
                                        KMINT32 height, KMTEXTURETYPE type)
{
    KMSTATUS status;
    status = describe_surface(first, width, height, type);
    if (status != KMSTATUS_SUCCESS) {
        return status;
    }
    COPY_DESCRIPTION(second, first);
    if (SURFACE_KIND(first) == KM_SURFACETYPE_SMALLVQ_TEXTURE) {
        status = kmiVQalloc(first);
        if (status == KMSTATUS_NOT_ENOUGH_MEMORY) {
            kmiDefaultInterruptHandler(TEXTURE_OVERFLOW_CALLBACK);
        } else {
            status = kmiVQalloc(second);
            if (status == KMSTATUS_NOT_ENOUGH_MEMORY) {
                kmiVQFBfree(first);
                kmiDefaultInterruptHandler(TEXTURE_OVERFLOW_CALLBACK);
            }
        }
    } else {
        status = kmiTXalloc(first->uSize.dwTextureSize, first);
        if (status == KMSTATUS_NOT_ENOUGH_MEMORY) {
            kmiDefaultInterruptHandler(TEXTURE_OVERFLOW_CALLBACK);
        } else {
            status = kmiTXalloc(second->uSize.dwTextureSize, second);
            if (status == KMSTATUS_NOT_ENOUGH_MEMORY) {
                kmiFBfree((KMINT32)first->pSurface);
                kmiDefaultInterruptHandler(TEXTURE_OVERFLOW_CALLBACK);
            }
        }
    }
    return status;
}

/* the surfaces' block is found by one kmiTXalloc and freed again. each surface
 * then claims its part with kmiABSalloc, so that each can be freed alone */
KMSTATUS kmCreateContiguousTextureSurface(PKMSURFACEDESC surfaces,
                                          KMINT32 count, KMINT32 width,
                                          KMINT32 height, KMTEXTURETYPE type)
{
    /* @hack redundant: status set before describe_surface's result replaces
     * it */
    KMSTATUS status = KMSTATUS_SUCCESS;
    KMINT32 i;
    KMINT32 j;
    KMDWORD size;
    KMDWORD *address;
#ifdef NONMATCHING
    if (count <= 0) {
        return KMSTATUS_INVALID_TEXTURE_TYPE;
    }
#endif
    status = describe_surface(&surfaces[0], width, height, type);
    if (status != KMSTATUS_SUCCESS) {
        return status;
    }
    if (SURFACE_KIND(surfaces) == KM_SURFACETYPE_SMALLVQ_TEXTURE) {
        return KMSTATUS_INVALID_TEXTURE_TYPE;
    }
    /* @hack redundant: the loop's test made ahead of it too, as count > 1 */
    if (count > 1) {
        /* @hack spelling: i = i + 1, where i++ would do */
        for (i = 1; i < count; i = i + 1) {
            COPY_DESCRIPTION(&surfaces[i], &surfaces[0]);
        }
    }
    size = surfaces[0].uSize.dwTextureSize * count;
    status = kmiTXalloc(size, &surfaces[0]);
    if (status == KMSTATUS_NOT_ENOUGH_MEMORY) {
        kmiDefaultInterruptHandler(TEXTURE_OVERFLOW_CALLBACK);
    } else {
        address = surfaces[0].pSurface;
        kmiFBfree((KMINT32)surfaces[0].pSurface);
        for (i = 0; i < count; i++) {
            surfaces[i].pSurface = address;
            status = kmiABSalloc(surfaces[i].uSize.dwTextureSize, &surfaces[i]);
            address += surfaces[i].uSize.dwTextureSize / sizeof(KMDWORD);
            if (status == KMSTATUS_NOT_ENOUGH_MEMORY) {
#ifdef NONMATCHING
                /* this side repairs the tag below: it frees each surface
                 * before the failed one */
                for (j = 0; j < i; j++) {
                    kmiFBfree((KMINT32)surfaces[j].pSurface);
                }
#else
                /* @hack spelling: surfaces stepped to the failed surface,
                 * where surfaces[i] would do */
                surfaces += i;
                /* @bug memory: the surfaces before the failed one stay
                 * allocated. the loop frees the failed surface's address, which
                 * kmiABSalloc left unallocated, i times (0x8c14e3c4,
                 * 0x8c14e3cc) */
                for (j = 0; j < i; j++) {
                    kmiFBfree((KMINT32)surfaces->pSurface);
                }
#endif
                kmiDefaultInterruptHandler(TEXTURE_OVERFLOW_CALLBACK);
                return status;
            }
        }
    }
    return status;
}

KMSTATUS kmLoadTexture(PKMSURFACEDESC pSurfaceDesc, KMDWORD *pTexture,
                       KMBOOLEAN fAutoMipMap, KMBOOLEAN fDither)
{
    if (fAutoMipMap == TRUE || fDither == TRUE) {
        return KMSTATUS_INVALID_TEXTURE_TYPE;
    }
    if (SURFACE_KIND(pSurfaceDesc) == KM_SURFACETYPE_SMALLVQ_TEXTURE) {
        write_small_vq(VRAM(pSurfaceDesc), (KMBYTE *)pTexture,
                       pSurfaceDesc->u0.USize, pSurfaceDesc->fSurfaceFlags);
    } else if (MIP16_CHAIN(pSurfaceDesc)) {
        write_mip16(VRAM(pSurfaceDesc), pTexture, pSurfaceDesc->u0.USize);
    } else {
        kmiCopyToFrameBuffer(VRAM(pSurfaceDesc), pTexture,
                             pSurfaceDesc->uSize.dwTextureSize);
    }
#ifdef NONMATCHING
#else
    /* @hack redundant: a call of quantize_channel that never runs, which keeps
     * the function nothing else calls */
    if (0) {
        quantize_channel(0);
    }
#endif
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmLoadTextureBlock(PKMSURFACEDESC surface, KMDWORD *texture,
                            KMUINT32 block, KMUINT32 block_size)
{
    KMDWORD offset;
    KMDWORD remaining;
    if (SURFACE_KIND(surface) == KM_SURFACETYPE_SMALLVQ_TEXTURE) {
        return KMSTATUS_INVALID_TEXTURE_TYPE;
    } else if (MIP16_CHAIN(surface)) {
        offset = MIP16_START;
    } else {
        offset = 0;
    }
    block_size *= BLOCK_UNIT;
    offset += block * block_size;
    remaining = surface->uSize.dwTextureSize - offset;
    if (offset >= surface->uSize.dwTextureSize) {
        return KMSTATUS_INVALID_BLOCKNUMBER;
    }
    if (remaining > block_size) {
        remaining = block_size;
    }
    offset += VRAM(surface);
    kmiCopyToFrameBuffer(offset, texture, remaining);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmLoadTexturePart(PKMSURFACEDESC pSurfaceDesc, KMDWORD *pTexture,
                           KMUINT32 nOffset, KMUINT32 nSize)
{
    KMDWORD position;
    KMDWORD skip;
    KMDWORD offset;
    KMDWORD remaining;
    nSize &= ~3;
    position = nOffset & ~3;
    if (!nSize) {
        return KMSTATUS_SUCCESS;
    }
    if (SURFACE_KIND(pSurfaceDesc) == KM_SURFACETYPE_SMALLVQ_TEXTURE) {
        return KMSTATUS_INVALID_TEXTURE_TYPE;
    } else if (MIP16_CHAIN(pSurfaceDesc)) {
        skip = MIP16_START;
    } else {
        skip = 0;
    }
    offset = position + skip;
    remaining = pSurfaceDesc->uSize.dwTextureSize - offset;
    if (offset >= pSurfaceDesc->uSize.dwTextureSize) {
        return KMSTATUS_INVALID_ADDRESS;
    }
    if (remaining > nSize) {
        remaining = nSize;
    }
    offset += VRAM(pSurfaceDesc);
    kmiCopyToFrameBuffer(offset, pTexture, remaining);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmLoadRectangleTexturePart(PKMSURFACEDESC surface, KMDWORD *texture,
                                    KMUINT32 width, KMUINT32 height,
                                    KMUINT32 dst_x, KMUINT32 dst_y,
                                    KMUINT32 src_x, KMUINT32 src_y,
                                    KMUINT32 src_width)
{
    KMDWORD destination;
    KMDWORD row_bytes;
    KMDWORD pitch;
    KMDWORD source_pitch;
    KMDWORD texels;
    KMDWORD *source;
    KMUINT32 row;
    if ((surface->fSurfaceFlags & KM_SURFACEFLAGS_TWIDDLED) ||
        (surface->fSurfaceFlags & KM_SURFACEFLAGS_MIPMAPED) ||
        (surface->fSurfaceFlags & KM_SURFACEFLAGS_VQ) ||
        (surface->fSurfaceFlags & KM_SURFACEFLAGS_PALETTIZED)) {
        return KMSTATUS_INVALID_TEXTURE_TYPE;
    }
    /* @hack spelling: the pitch made from USize through texels, then doubled
     * in place, where USize * TEXEL_BYTES would do */
    texels = surface->u0.USize;
    pitch = texels;
    pitch <<= 1;
    row_bytes = width * TEXEL_BYTES;
    destination = (dst_y * pitch) + (dst_x * TEXEL_BYTES);
    destination += VRAM(surface);
    if (src_width == 0) {
        source = texture;
        source_pitch = row_bytes;
    } else {
        source_pitch = src_width * TEXEL_BYTES;
        source = texture + (((src_y * source_pitch) + (src_x * TEXEL_BYTES)) /
                            sizeof(KMDWORD));
    }
    for (row = 0; row < height; row++) {
        kmiCopyToFrameBuffer(destination, source, row_bytes);
        destination += pitch;
        source += source_pitch / sizeof(KMDWORD);
    }
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmLoadYUVTexture(PKMSURFACEDESC pSurfaceDesc, KMDWORD *pTexture,
                          KMINT32 nWidth, KMINT32 nHeight, KMINT32 nFormat,
                          KMBOOLEAN bWaitEndOfDMA)
{
    KMINT32 yuv422;
    KMINT32 multiple;
    KMDWORD last_column;
    KMDWORD last_row;
    KMDWORD blocks;
    KMDWORD bytes;
    KMDWORD control;
    KMI_DMA_HANDLE handle;
    KMI_DMA_HANDLE *queued;
    KMUINT32 result;
    if ((KMDWORD)pTexture & (TRANSFER_BYTES - 1)) {
        return KMSTATUS_INVALID_ADDRESS;
    }
    /* @bug logic: an RGB565 or ARGB4444 surface takes the converter's YUV422
     * texels. the format test holds for either of KM_PIXELFORMAT_YUV422's two
     * bits (0x8c14e66e) */
    if ((pSurfaceDesc->fSurfaceFlags & KM_SURFACEFLAGS_MIPMAPED) ||
        (pSurfaceDesc->fSurfaceFlags & KM_SURFACEFLAGS_TWIDDLED) ||
        (pSurfaceDesc->fSurfaceFlags & KM_SURFACEFLAGS_VQ) ||
        (pSurfaceDesc->fSurfaceFlags & KM_SURFACEFLAGS_PALETTIZED) ||
        !(pSurfaceDesc->PixelFormat & KM_PIXELFORMAT_YUV422)) {
        return KMSTATUS_INVALID_TEXTURE_TYPE;
    }
    switch (nFormat) {
    case KM_TEXTURE_YUV420:
        yuv422 = 0;
        break;
    case KM_TEXTURE_YUV422:
        yuv422 = 1;
        break;
    default:
        return KMSTATUS_INVALID_TEXTURE_TYPE;
    }
    multiple = 0;
    if (nWidth < 1 || nWidth > YUV_DIMENSION_MASK + 1) {
        return KMSTATUS_INVALID_TEXTURE_TYPE;
    }
    if (nHeight < 1 || nHeight > YUV_DIMENSION_MASK + 1) {
        return KMSTATUS_INVALID_TEXTURE_TYPE;
    }
    if (nWidth * nHeight >= 2) {
        multiple = 1;
        last_column = nWidth - 1;
        last_row = nHeight - 1;
    } else {
        last_column = (pSurfaceDesc->u0.USize / MACROBLOCK_SIDE) - 1;
        last_row = (pSurfaceDesc->u1.VSize / MACROBLOCK_SIDE) - 1;
    }
    blocks = (last_column + 1) * (last_row + 1);
    if (!yuv422) {
        if (!multiple) {
            bytes = blocks * MACROBLOCK_420;
        } else {
            bytes = nWidth * nHeight * MACROBLOCK_420;
        }
    } else {
        if (!multiple) {
            bytes = blocks * MACROBLOCK_422;
        } else {
            bytes = nWidth * nHeight * MACROBLOCK_422;
        }
    }
    control = ((yuv422 & 1) << YUV_422_SHIFT) |
              ((multiple & 1) << YUV_MULTIPLE_SHIFT) |
              ((last_row & YUV_DIMENSION_MASK) << YUV_ROWS_SHIFT) |
              (last_column & YUV_DIMENSION_MASK);
    kmiWriteRegister(KMI_TA_YUV_CONTROL, control);
    kmiWriteRegister(KMI_TA_YUV_BASE, VRAM(pSurfaceDesc) & YUV_BASE_BITS);
    result = KMI_DMA_QUEUE_FULL;
    while (result == KMI_DMA_QUEUE_FULL) {
        result =
            kmiDMAtoTARequest(TA_YUV_INPUT, pTexture, bytes,
                              KMI_DMA_SECOND_RING | KMI_DMA_SOURCE_UP, &queued);
#ifdef NONMATCHING
        /* this side repairs the tag below: it drops the read, which the
         * wait makes once a request has queued */
#else
        /* @bug memory: reads through queued before a request has set it
         * (0x8c14e76e, 0x8c14e776). a request leaves it unset for a full DMA
         * queue, and for no bytes, which a lone macroblock on a surface under
         * 16 texels a side gives */
        handle = *queued;
#endif
    }
    if (result != KMI_DMA_QUEUED) {
        return KMSTATUS_HARDWARE_NOT_PRESENTED;
    }
    if (bWaitEndOfDMA == TRUE) {
#ifdef NONMATCHING
        /* the read the loop drops. result is KMI_DMA_QUEUED, so the request
         * has set queued */
        handle = *queued;
#endif
        kmiWaitForFinishDMA(handle);
    }
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmLoadVQCodebook(PKMSURFACEDESC surface, KMDWORD *texture)
{
    KMSTATUS status = KMSTATUS_SUCCESS;
    KMDWORD size;
    KMINT32 level;
    if (SURFACE_KIND(surface) == KM_SURFACETYPE_SMALLVQ_TEXTURE) {
        level = side_level(surface->u0.USize, surface->u0.USize);
        if (surface->fSurfaceFlags & KM_SURFACEFLAGS_MIPMAPED) {
            size = small_mip_codebook[level];
        } else {
            size = small_codebook[level];
        }
        kmiCopyToFrameBuffer(VRAM(surface), texture, size);
    } else if (surface->fSurfaceFlags & KM_SURFACEFLAGS_VQ) {
        kmiCopyToFrameBuffer(VRAM(surface), texture, VQ_CODEBOOK_BYTES);
    } else {
        status = KMSTATUS_INVALID_TEXTURE_TYPE;
    }
    return status;
}

KMSTATUS kmFreeTexture(PKMSURFACEDESC pSurfaceDesc)
{
    KMSTATUS status;
    if (SURFACE_KIND(pSurfaceDesc) == KM_SURFACETYPE_SMALLVQ_TEXTURE) {
        status = kmiVQFBfree(pSurfaceDesc);
    } else {
        status = kmiFBfree((KMINT32)pSurfaceDesc->pSurface);
    }
    return status;
}

KMSTATUS kmGetFreeTextureMem(KMUINT32 *pSizeOfTexture,
                             KMUINT32 *pMaxBlockSizeOfTexture)
{
    KMINT32 first;
    KMINT32 second;
    KMINT32 vq;
    KMINT32 largest;
    KMSTATUS status;
    status = kmiFBgetfreemem(&first, &second, &vq, &largest);
    *pSizeOfTexture = first + second + vq;
    *pMaxBlockSizeOfTexture = largest;
    return status;
}

KMSTATUS kmReLoadMipmap(PKMSURFACEDESC pSurfaceDesc, void *pTexture,
                        KMINT32 nMipmapCount)
{
    KMSTATUS status;
    if (pSurfaceDesc->PixelFormat == KM_PIXELFORMAT_PALETTIZED_4BPP &&
        (pSurfaceDesc->fSurfaceFlags & KM_SURFACEFLAGS_MIPMAPED)) {
        status = reload_palette4(pSurfaceDesc, pTexture, nMipmapCount);
    } else if (pSurfaceDesc->PixelFormat == KM_PIXELFORMAT_PALETTIZED_8BPP &&
               (pSurfaceDesc->fSurfaceFlags & KM_SURFACEFLAGS_MIPMAPED)) {
        status = reload_palette8(pSurfaceDesc, pTexture, nMipmapCount);
    } else if (SURFACE_KIND(pSurfaceDesc) == KM_SURFACETYPE_SMALLVQ_TEXTURE) {
        status = reload_vq(pSurfaceDesc, pTexture, nMipmapCount);
    } else if (pSurfaceDesc->fSurfaceFlags & KM_SURFACEFLAGS_VQ) {
        status = reload_vq(pSurfaceDesc, pTexture, nMipmapCount);
    } else if ((pSurfaceDesc->fSurfaceFlags & KM_SURFACEFLAGS_TWIDDLED) &&
               (pSurfaceDesc->fSurfaceFlags & KM_SURFACEFLAGS_MIPMAPED)) {
        status = reload_twiddled(pSurfaceDesc, pTexture, nMipmapCount);
    } else {
        status = KMSTATUS_INVALID_TEXTURE_TYPE;
    }
    return status;
}

KMSTATUS kmGetTexture(KMDWORD *texture, PKMSURFACEDESC surface)
{
    if ((surface->SurfaceType & TYPE_LOW_BITS) == KM_SURFACETYPE_FRAMEBUFFER) {
        kmiCopyFromPVRMemory(texture, surface->pSurface,
                             surface->uSize.dwTextureSize);
    } else if (SURFACE_KIND(surface) == KM_SURFACETYPE_SMALLVQ_TEXTURE) {
        read_small_vq((KMBYTE *)texture, VRAM(surface), surface->u0.USize,
                      surface->fSurfaceFlags);
    } else if (MIP16_CHAIN(surface)) {
        read_mip16(texture, VRAM(surface), surface->u0.USize);
    } else {
        kmiCopyFromFrameBuffer(texture, VRAM(surface),
                               surface->uSize.dwTextureSize);
    }
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmGarbageCollectTexture(void)
{
    /* @hack spelling: the two calls in one comma expression, where a statement
     * for kmiFBSortQueue would do */
    return kmiFBSortQueue(), kmiFBGarbageCollect();
}

/* describe surface as a width by height texture of type, its size included.
 * the result is KMSTATUS_SUCCESS, or KMSTATUS_INVALID_TEXTURE_TYPE where a
 * rule fails. each side must be a power of two from 8 to 1024. a texture must
 * be square, unless its layout is a rectangle, a stride, a twiddled rectangle,
 * or 4-bit or 8-bit palettized without mipmaps. a layout that isn't
 * palettized must name one of the five texel formats from
 * KM_TEXTURE_ARGB1555 to KM_TEXTURE_BUMP. a failed rule still leaves the
 * description filled in, all but PixelFormat where the texel format failed */
static KMSTATUS describe_surface(PKMSURFACEDESC surface, KMINT32 width,
                                 KMINT32 height, KMTEXTURETYPE type)
{
    KMUINT32 layout = type & TYPE_LAYOUT;
    KMUINT32 format = type & TYPE_FORMAT;
    KMSTATUS status = KMSTATUS_SUCCESS;

    if (layout == KM_TEXTURE_SMALLVQ || layout == KM_TEXTURE_SMALLVQ_MM) {
        surface->SurfaceType = KM_SURFACETYPE_SMALLVQ_TEXTURE;
    } else {
        surface->SurfaceType = KM_SURFACETYPE_TEXTURE;
    }
    if (layout == KM_TEXTURE_PALETTIZE4 || layout == KM_TEXTURE_PALETTIZE4_MM) {
        if (kmiHardwareResource->palette_mode == KM_PALETTE_32BPP_ARGB8888) {
            surface->BitDepth = KM_BITDEPTH_32;
        } else {
            surface->BitDepth = KM_BITDEPTH_16;
        }
        surface->PixelFormat = KM_PIXELFORMAT_PALETTIZED_4BPP;
    } else if (layout == KM_TEXTURE_PALETTIZE8 ||
               layout == KM_TEXTURE_PALETTIZE8_MM) {
        if (kmiHardwareResource->palette_mode == KM_PALETTE_32BPP_ARGB8888) {
            surface->BitDepth = KM_BITDEPTH_32;
        } else {
            surface->BitDepth = KM_BITDEPTH_16;
        }
        surface->PixelFormat = KM_PIXELFORMAT_PALETTIZED_8BPP;
    } else {
        surface->BitDepth = KM_BITDEPTH_16;
        switch (format) {
        case KM_TEXTURE_ARGB1555:
            surface->PixelFormat = KM_PIXELFORMAT_ARGB1555;
            break;
        case KM_TEXTURE_RGB565:
            surface->PixelFormat = KM_PIXELFORMAT_RGB565;
            break;
        case KM_TEXTURE_ARGB4444:
            surface->PixelFormat = KM_PIXELFORMAT_ARGB4444;
            break;
        case KM_TEXTURE_YUV422:
            surface->PixelFormat = KM_PIXELFORMAT_YUV422;
            break;
        case KM_TEXTURE_BUMP:
            surface->PixelFormat = KM_PIXELFORMAT_BUMP;
            break;
        default:
            status = KMSTATUS_INVALID_TEXTURE_TYPE;
            break;
        }
    }
    surface->u0.USize = width;
    surface->u1.VSize = height;
#define TEXTURE_SIDES                                                          \
    case 8:                                                                    \
    case 16:                                                                   \
    case 32:                                                                   \
    case 64:                                                                   \
    case 128:                                                                  \
    case 256:                                                                  \
    case 512:                                                                  \
    case 1024:
    switch (width) {
        TEXTURE_SIDES break;
    default:
        status = KMSTATUS_INVALID_TEXTURE_TYPE;
        break;
    }
    switch (height) {
        TEXTURE_SIDES break;
    default:
        status = KMSTATUS_INVALID_TEXTURE_TYPE;
        break;
    }
#undef TEXTURE_SIDES
    if (layout != KM_TEXTURE_RECTANGLE && layout != KM_TEXTURE_STRIDE &&
        layout != KM_TEXTURE_TWIDDLED_RECTANGLE &&
        layout != KM_TEXTURE_PALETTIZE4 && layout != KM_TEXTURE_PALETTIZE8 &&
        width != height) {
        status = KMSTATUS_INVALID_TEXTURE_TYPE;
    }
    surface->fSurfaceFlags = 0;
    if (layout == KM_TEXTURE_BMP_MM || layout == KM_TEXTURE_TWIDDLED_MM ||
        layout == KM_TEXTURE_VQ_MM || layout == KM_TEXTURE_SMALLVQ_MM ||
        layout == KM_TEXTURE_PALETTIZE4_MM ||
        layout == KM_TEXTURE_PALETTIZE8_MM) {
        surface->fSurfaceFlags |= KM_SURFACEFLAGS_MIPMAPED;
    }
    if (layout == KM_TEXTURE_BMP || layout == KM_TEXTURE_BMP_MM ||
        layout == KM_TEXTURE_TWIDDLED || layout == KM_TEXTURE_TWIDDLED_MM ||
        layout == KM_TEXTURE_TWIDDLED_RECTANGLE) {
        surface->fSurfaceFlags |= KM_SURFACEFLAGS_TWIDDLED;
    }
    if (layout == KM_TEXTURE_VQ || layout == KM_TEXTURE_VQ_MM ||
        layout == KM_TEXTURE_SMALLVQ || layout == KM_TEXTURE_SMALLVQ_MM) {
        /* @hack spelling: the VQ and twiddled flags set in two statements,
         * where one OR of both would do */
        surface->fSurfaceFlags |= KM_SURFACEFLAGS_VQ;
        surface->fSurfaceFlags |= KM_SURFACEFLAGS_TWIDDLED;
    }
    if (layout == KM_TEXTURE_STRIDE) {
        surface->fSurfaceFlags |= KM_SURFACEFLAGS_STRIDE;
    }
    if (layout == KM_TEXTURE_PALETTIZE4 || layout == KM_TEXTURE_PALETTIZE4_MM ||
        layout == KM_TEXTURE_PALETTIZE8 || layout == KM_TEXTURE_PALETTIZE8_MM) {
        /* @hack spelling: the palettized and twiddled flags set in two
         * statements, where one OR of both would do */
        surface->fSurfaceFlags |= KM_SURFACEFLAGS_PALETTIZED;
        surface->fSurfaceFlags |= KM_SURFACEFLAGS_TWIDDLED;
    }
#ifdef NONMATCHING
    /* the small VQ tables end at level 6 */
    if (SURFACE_KIND(surface) == KM_SURFACETYPE_SMALLVQ_TEXTURE &&
        (width > 64 || height > 64)) {
        return KMSTATUS_INVALID_TEXTURE_TYPE;
    }
#endif
    size_surface(surface);
    return status;
}

/* the level of the shorter side. a side that is no power of two from 1 to 1024
 * is level 3 */
static KMINT32 side_level(KMINT32 width, KMINT32 height)
{
    if (width >= height) {
        width = height;
    }
    switch (width) {
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
    default:
        return 3;
    }
}

static void size_surface(PKMSURFACEDESC surface)
{
    KMDWORD size;
    if (SURFACE_KIND(surface) == KM_SURFACETYPE_SMALLVQ_TEXTURE) {
        size = small_vq_size(surface);
    } else if (surface->fSurfaceFlags & KM_SURFACEFLAGS_VQ) {
        size = vq_size(surface);
    } else if (surface->fSurfaceFlags & KM_SURFACEFLAGS_PALETTIZED) {
        size = palette_size(surface);
    } else {
        size = plain_size(surface);
    }
    surface->uSize.dwTextureSize = size;
}

/* add each level below the first to bytes, in the format's units. each level
 * halves width and height, until a side reaches 1. a square chain's last level,
 * 1 by 1, adds last_square units rather than 1 */
#define ADD_SMALL_LEVELS(last_square)                                          \
    width >>= 1;                                                               \
    height >>= 1;                                                              \
    shortest = width < height ? width : height;                                \
    while (shortest > 1) {                                                     \
        bytes += width * height;                                               \
        width >>= 1;                                                           \
        height >>= 1;                                                          \
        shortest >>= 1;                                                        \
    }                                                                          \
    if (width == height)                                                       \
        bytes += (last_square);                                                \
    else                                                                       \
        bytes += width * height

static KMDWORD plain_size(PKMSURFACEDESC surface)
{
    KMDWORD width = surface->u0.USize;
    KMDWORD height = surface->u1.VSize;
    KMDWORD shortest;
    KMDWORD bytes = width * height;
    if (surface->fSurfaceFlags & KM_SURFACEFLAGS_MIPMAPED) {
        ADD_SMALL_LEVELS(MIP16_LAST);
    }
    if (surface->BitDepth == KM_BITDEPTH_16) {
        bytes <<= 1;
    } else {
        bytes <<= 2;
    }
    return bytes;
}
static KMDWORD vq_size(PKMSURFACEDESC surface)
{
    KMDWORD width = surface->u0.USize >> 1;
    KMDWORD height = surface->u1.VSize >> 1;
    KMDWORD shortest;
    KMDWORD bytes = width * height;
    if (surface->fSurfaceFlags & KM_SURFACEFLAGS_MIPMAPED) {
        ADD_SMALL_LEVELS(VQ_LAST);
        bytes += VQ_MIP_PAD;
    } else if (surface->u0.USize == 8 && surface->u1.VSize == 8) {
        bytes += VQ_8_PAD;
    }
    bytes += VQ_CODEBOOK_BYTES;
    return bytes;
}
static KMDWORD palette_size(PKMSURFACEDESC surface)
{
    KMDWORD width = surface->u0.USize;
    KMDWORD height = surface->u1.VSize;
    KMDWORD shortest;
    KMDWORD bytes = width * height;
    if (surface->fSurfaceFlags & KM_SURFACEFLAGS_MIPMAPED) {
        ADD_SMALL_LEVELS(PALETTE_LAST);
        if (surface->PixelFormat == KM_PIXELFORMAT_PALETTIZED_4BPP) {
            bytes += PALETTE4_MIP_PAD;
        } else {
            bytes += PALETTE8_MIP_PAD;
        }
    }
    /* at 4 bits, two texels take a byte, and a lone texel one */
    if (surface->PixelFormat == KM_PIXELFORMAT_PALETTIZED_4BPP) {
        /* @hack redundant: a size of 1 stored again, where only halving
         * another would do */
        if (bytes == 1) {
            bytes = 1;
        } else {
            bytes >>= 1;
        }
    }
    return bytes;
}
static KMDWORD small_vq_size(PKMSURFACEDESC surface)
{
    KMDWORD width;
    KMDWORD height;
    KMDWORD shortest;
    KMDWORD bytes;
    KMINT32 level;
    width = surface->u0.USize >> 1;
    height = surface->u1.VSize >> 1;
    bytes = width * height;
    level = side_level(surface->u0.USize, surface->u1.VSize);
    if (surface->fSurfaceFlags & KM_SURFACEFLAGS_MIPMAPED) {
        ADD_SMALL_LEVELS(VQ_LAST);
        bytes += VQ_MIP_PAD;
        bytes += small_mip_codebook[level];
    } else {
        bytes += small_codebook[level];
    }
    return bytes;
}
static KMINT32 write_mip16(KMDWORD offset, KMDWORD *texels, KMINT32 width)
{
    /* @hack spelling: the copy's two addresses through locals, where the
     * arguments would do */
    KMDWORD *source = texels;
    KMDWORD target = offset + MIP16_START;
    KMINT32 level = side_level(width, width);
    /* @bug memory: copies 4 bytes past the chain. it copies the chain's size,
     * from MIP16_START on, so it reads 4 bytes past the levels in texels and
     * writes 4 past the surface's dwTextureSize (the size at 0x8c14ef80) */
    return kmiCopyToFrameBuffer(target, source,
                                mip16_bytes[level] + MIP16_START);
}
static KMINT32 read_mip16(KMDWORD *texels, KMDWORD offset, KMINT32 width)
{
    /* @hack spelling: the copy's two addresses through locals, where the
     * arguments would do */
    KMDWORD source = offset + MIP16_START;
    KMDWORD *target = texels;
    KMINT32 level = side_level(width, width);
    return kmiCopyFromFrameBuffer(target, source, mip16_bytes[level]);
}
#define SMALL_VQ_SIZES()                                                       \
    if (flags & KM_SURFACEFLAGS_MIPMAPED) {                                    \
        codebook = small_mip_codebook[level];                                  \
        indices = index_end[level] + VQ_MIP_PAD;                               \
    } else {                                                                   \
        codebook = small_codebook[level];                                      \
        indices = index_bytes[level];                                          \
    }

/* texels holds the codebook, then the indices */
static KMINT32 write_small_vq(KMDWORD offset, KMBYTE *texels, KMINT32 width,
                              KMDWORD flags)
{
    /* @hack spelling: offset copied to destination, and both copies' sources
     * stepped in statements of their own, where offset + VQ_CODEBOOK_BYTES and
     * texels + codebook would do */
    KMDWORD destination = offset;
    KMDWORD codebook;
    KMDWORD indices;
    KMINT32 level = side_level(width, width);
    SMALL_VQ_SIZES()
    kmiCopyToFrameBuffer(destination, (KMDWORD *)texels, codebook);
    texels += codebook;
    destination = destination + VQ_CODEBOOK_BYTES;
    return kmiCopyToFrameBuffer(destination, (KMDWORD *)texels, indices);
}
static KMINT32 read_small_vq(KMBYTE *texels, KMDWORD offset, KMINT32 width,
                             KMDWORD flags)
{
    /* @hack spelling: texels copied to destination, and both copies' places
     * stepped in statements of their own, where texels + codebook and
     * offset + VQ_CODEBOOK_BYTES would do */
    KMBYTE *destination = texels;
    KMDWORD codebook;
    KMDWORD indices;
    KMINT32 level = side_level(width, width);
    SMALL_VQ_SIZES()
    kmiCopyFromFrameBuffer((KMDWORD *)destination, offset, codebook);
    destination = destination + codebook;
    offset += VQ_CODEBOOK_BYTES;
    return kmiCopyFromFrameBuffer((KMDWORD *)destination, offset, indices);
}

/* a chain's levels lie end to end, so a block of texture memory can hold bytes
 * of two. a reload reads such a block into merge, puts its level's bytes in,
 * and writes merge back whole. a block is MERGE_BYTES, but PALETTE8_MERGE_BYTES
 * at the start of an 8-bit palettized chain */
#define MERGE_BYTES 16
#define PALETTE8_MERGE_BYTES 8
#define READ_MERGED(offset, count)                                             \
    to = (KMDWORD)(&merge[0]);                                                 \
    from = VRAM(surface);                                                      \
    from += (offset);                                                          \
    kmiCopyFromFrameBuffer((KMDWORD *)to, from, (count))
#define WRITE_MERGED(offset, count)                                            \
    from = (KMDWORD)(&merge[0]);                                               \
    to = VRAM(surface);                                                        \
    to += (offset);                                                            \
    kmiPurgeCache((void *)from);                                               \
    kmiPurgeCache((void *)(from + CACHE_LINE));                                \
    kmiCopyToFrameBuffer(to, (KMDWORD *)from, (count))
#define REPLACE_BYTES(start, count)                                            \
    for (i = 0; i < (count); i++) {                                            \
        merge[(start) + i] = *input++;                                         \
    }
/* where a 4-bit level ends */
#ifdef NONMATCHING
/* this side repairs the tag below: a level ends palette4_bytes[level] past
 * its start. below level 10, the next level starts there */
#define PALETTE4_END(level) (palette4_offset[level] + palette4_bytes[level])
#else
/* @bug memory: at level 10, palette4_offset[level + 1] reads past the table
 * into palette4_bytes[0], which is 1. reload_palette4 then reads and writes
 * the level's last block 11 bytes before the surface (the loads at 0x8c14f20e
 * and 0x8c14f23e) */
#define PALETTE4_END(level) palette4_offset[(level) + 1]
#endif

/* the levels of 1, 2 and 4 texels a side lie in the chain's first block. they
 * take byte 1, bytes 2 to 3, and bytes 4 to 11. a larger level starts
 * PALETTE4_START bytes into a block, which holds its first PALETTE4_HEAD
 * bytes. it ends PALETTE4_TAIL bytes into another */
#define PALETTE4_START 12
#define PALETTE4_HEAD 4
#define PALETTE4_TAIL 12
static KMSTATUS reload_palette4(PKMSURFACEDESC surface, KMBYTE *texels,
                                KMINT32 width)
{
    /* @hack type: level a KMDWORD, though side_level returns a KMINT32 */
    KMDWORD level;
    KMDWORD from;
    KMDWORD to;
    KMBYTE merge[MERGE_BYTES];
    KMBYTE *input;
    KMINT32 i;
    level = side_level(width, width);
    if (width <= 4) {
        READ_MERGED(0, MERGE_BYTES);
        input = texels;
        if (width == 1) {
            merge[1] = *input++;
        } else if (width == 2) {
            REPLACE_BYTES(2, 2);
        } else {
            REPLACE_BYTES(4, 8);
        }
        WRITE_MERGED(0, MERGE_BYTES);
    } else {
        READ_MERGED(palette4_offset[level] - PALETTE4_START, MERGE_BYTES);
        input = texels;
        REPLACE_BYTES(PALETTE4_START, PALETTE4_HEAD);
        WRITE_MERGED(palette4_offset[level] - PALETTE4_START, MERGE_BYTES);
        if (palette4_bytes[level] - MERGE_BYTES > 0) {
            to = VRAM(surface);
            to += palette4_offset[level] + PALETTE4_HEAD;
            from = (KMDWORD)texels + PALETTE4_HEAD;
            kmiCopyToFrameBuffer(to, (KMDWORD *)from,
                                 palette4_bytes[level] - MERGE_BYTES);
        }
        READ_MERGED(PALETTE4_END(level) - PALETTE4_TAIL, MERGE_BYTES);
        input = texels + palette4_bytes[level] - PALETTE4_TAIL;
        REPLACE_BYTES(0, PALETTE4_TAIL);
        WRITE_MERGED(PALETTE4_END(level) - PALETTE4_TAIL, MERGE_BYTES);
    }
    return KMSTATUS_SUCCESS;
}

/* the levels of 1 and 2 texels a side lie in the chain's first block. they
 * take byte 3, and bytes 4 to 7 */
static KMSTATUS reload_palette8(PKMSURFACEDESC surface, KMBYTE *texels,
                                KMINT32 width)
{
    /* @hack type: level a KMDWORD, though side_level returns a KMINT32 */
    KMDWORD level;
    KMDWORD bytes;
    KMDWORD from;
    KMDWORD to;
    KMBYTE merge[PALETTE8_MERGE_BYTES];
    KMBYTE *input;
    KMINT32 i;
    level = side_level(width, width);
    if (width <= 2) {
        READ_MERGED(0, PALETTE8_MERGE_BYTES);
        /* @hack spelling: input set in each branch, where once above them would
         * do */
        if (width == 1) {
            input = texels;
            merge[3] = *input++;
        } else {
            input = texels;
            REPLACE_BYTES(4, 4);
        }
        WRITE_MERGED(0, PALETTE8_MERGE_BYTES);
    } else {
        bytes = palette8_bytes[level];
        from = (KMDWORD)texels;
        to = VRAM(surface) + palette8_offset[level];
        kmiCopyToFrameBuffer(to, (KMDWORD *)from, bytes);
    }
    return KMSTATUS_SUCCESS;
}

/* the levels of 1 and 2 texels a side lie in the chain's first block. they
 * take bytes 6 to 7, and bytes 8 to 15 */
static KMSTATUS reload_twiddled(PKMSURFACEDESC surface, KMBYTE *texels,
                                KMINT32 width)
{
    /* @hack type: level a KMDWORD, though side_level returns a KMINT32 */
    KMDWORD level;
    KMDWORD bytes;
    KMDWORD from;
    KMDWORD to;
    KMBYTE merge[MERGE_BYTES];
    KMBYTE *input;
    KMINT32 i;
    level = side_level(width, width);
    if (width <= 2) {
        READ_MERGED(0, MERGE_BYTES);
        /* @hack spelling: input set in each branch, where once above them would
         * do */
        if (width == 1) {
            input = texels;
            REPLACE_BYTES(6, 2);
        } else {
            input = texels;
            REPLACE_BYTES(8, 8);
        }
        WRITE_MERGED(0, MERGE_BYTES);
    } else {
        bytes = texel16_bytes[level];
        from = (KMDWORD)texels;
        to = VRAM(surface) + texel16_offset[level];
        kmiCopyToFrameBuffer(to, (KMDWORD *)from, bytes);
    }
    return KMSTATUS_SUCCESS;
}

/* the indices of the levels 2 and 4 texels a side lie in the block after the
 * codebook. they take byte 1, and bytes 2 to 5. a larger level's
 * indices start VQ_START bytes into a block, which holds their first
 * VQ_LEAD_BYTES bytes. they end VQ_TAIL bytes into another. the level of 1
 * texel isn't reloaded */
#define VQ_START 6
#define VQ_LEAD_BYTES 10
#define VQ_TAIL 6
static KMSTATUS reload_vq(PKMSURFACEDESC surface, KMBYTE *texels, KMINT32 width)
{
    /* @hack type: level a KMDWORD, though side_level returns a KMINT32 */
    KMDWORD level;
    KMDWORD from;
    KMDWORD to;
    KMBYTE merge[MERGE_BYTES];
    KMBYTE *input;
    KMINT32 i;
    level = side_level(width, width);
    if (width != 1) {
        if (width <= 4) {
            READ_MERGED(VQ_CODEBOOK_BYTES, MERGE_BYTES);
            input = texels;
            if (width == 2) {
                merge[1] = *input++;
            } else {
                REPLACE_BYTES(2, 4);
            }
            WRITE_MERGED(VQ_CODEBOOK_BYTES, MERGE_BYTES);
        } else {
            READ_MERGED(VQ_CODEBOOK_BYTES + index_end[level - 1] - VQ_START,
                        MERGE_BYTES);
            input = texels;
            REPLACE_BYTES(VQ_START, VQ_LEAD_BYTES);
            WRITE_MERGED(VQ_CODEBOOK_BYTES + index_end[level - 1] - VQ_START,
                         MERGE_BYTES);
            if (index_bytes[level] - MERGE_BYTES > 0) {
                to = VRAM(surface);
                to += VQ_CODEBOOK_BYTES + index_end[level - 1] + VQ_LEAD_BYTES;
                from = (KMDWORD)texels + VQ_LEAD_BYTES;
                kmiCopyToFrameBuffer(to, (KMDWORD *)from,
                                     index_bytes[level] - MERGE_BYTES);
            }
            READ_MERGED(VQ_CODEBOOK_BYTES + index_end[level] - VQ_TAIL,
                        MERGE_BYTES);
            input = texels + index_bytes[level] - VQ_TAIL;
            REPLACE_BYTES(0, VQ_TAIL);
            WRITE_MERGED(VQ_CODEBOOK_BYTES + index_end[level] - VQ_TAIL,
                         MERGE_BYTES);
        }
    }
    return KMSTATUS_SUCCESS;
}
#ifdef NONMATCHING
#else
/* value reduced to a 3-bit level, 0 to 7. no call of it runs */
static KMINT32 quantize_channel(KMBYTE value)
{
    KMINT32 band;
    if (value < 128) {
        band = 3;
    } else {
        band = 4;
    }
    if (value < 112) {
        band = (value + 16) >> 5;
    }
    if (value > 143) {
        band = (value - 15) >> 5;
    }
    return band;
}
#endif
