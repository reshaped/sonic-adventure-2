#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>

/* the member exports ConvertUVnum, though no header of the SDK declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the TSP word's size code for a side of size texels, log2(size) - 3. it
 * runs from 0 for 8 to 7 for 1024. any other size is returned unchanged */
Int ConvertUVnum(Int size);
/* NOLINTEND(misc-use-internal-linkage) */

/* the TSP word's size codes: the height's in TSP_V_SIZE, and the width's in
 * the three bits above it */
#define TSP_V_SIZE 0x00000007
#define TSP_U_SIZE_SHIFT 3
/* the fields of the texture word, texparambuffer. TEX_FORMAT holds the pixel
 * format as a surface's PixelFormat holds it. TEX_SCAN_ORDER marks pixels
 * that lie row after row, not twiddled. a flag's _SHIFT carries its
 * NJD_SURFACEFLAGS_ bit to its field. TEX_SCAN_ORDER and TEX_STRIDE lie
 * inside nj_texlist.h's TEX_PALETTE, which holds a palettized texture's bank
 * bits instead */
#define TEX_MIPMAP_SHIFT 31
#define TEX_VQ 0x40000000
#define TEX_VQ_SHIFT 27
#define TEX_FORMAT 0x38000000
#define TEX_SCAN_ORDER 0x04000000
#define TEX_SCAN_ORDER_SHIFT 24
#define TEX_STRIDE 0x02000000
#define TEX_STRIDE_SHIFT 21
/* the address field, bits 0 to 20, takes pSurface in 8-byte units. the mask
 * also covers bits 21 and 22, which no field set before the address holds */
#define TEX_ADDRESS 0x007fffff
#define TEX_ADDRESS_SHIFT 3

Int ConvertUVnum(Int size)
{
    if (size == 8) {
        size = 0;
    } else if (size == 16) {
        size = 1;
    } else if (size == 32) {
        size = 2;
    } else if (size == 64) {
        size = 3;
    } else if (size == 128) {
        size = 4;
    } else if (size == 256) {
        size = 5;
    } else if (size == 512) {
        size = 6;
    } else if (size == 1024) {
        size = 7;
    }
    return size;
}

void njSetTextureParam(NJS_TEXMEMLIST *tex)
{
    /* @hack redundant: register on surface, where a plain local would do */
    register NJS_TEXSURFACE *surface = &tex->texinfo.texsurface;
    /* @hack redundant: register on param, where a plain local would do */
    register Uint32 param;
    Int format;
    Int bank;

    param = ConvertUVnum(surface->nWidth) << TSP_U_SIZE_SHIFT;
    /* @hack redundant: a second mask of bits already clear */
    param = (param & ~TSP_V_SIZE) | ConvertUVnum(surface->nHeight);
    tex->tspparambuffer = param;
    param = (surface->fSurfaceFlags & NJD_SURFACEFLAGS_MIPMAPED)
            << TEX_MIPMAP_SHIFT;
    param = (param & ~TEX_VQ) |
            ((surface->fSurfaceFlags & NJD_SURFACEFLAGS_VQ) << TEX_VQ_SHIFT);
    param = (param & ~TEX_FORMAT) | (surface->PixelFormat & TEX_FORMAT);
    param = (param & ~TEX_SCAN_ORDER) |
            ((~(surface->fSurfaceFlags & NJD_SURFACEFLAGS_TWIDDLED) &
              NJD_SURFACEFLAGS_TWIDDLED)
             << TEX_SCAN_ORDER_SHIFT);
    param = (param & ~TEX_STRIDE) |
            ((surface->fSurfaceFlags & NJD_SURFACEFLAGS_STRIDE)
             << TEX_STRIDE_SHIFT);
    param = (param & ~TEX_ADDRESS) |
            ((Uint32)surface->pSurface >> TEX_ADDRESS_SHIFT);
    /* @hack spelling: format, a local for the switch's value, where the value
     * switched on straight would do */
    /* @hack spelling: Type read through tex, where surface would do */
    format = (tex->texinfo.texsurface.Type >> TEXSURFACE_FORMAT_SHIFT) &
             NJD_TEXFMT_TYPE_MASK;
    switch (format) {
    case NJD_TEXFMT_PALETTIZE4:
    case NJD_TEXFMT_PALETTIZE4_MM:
        bank = tex->bank & BANK_MASK_4BPP;
        param = (param & ~TEX_PALETTE) | (bank << TEX_PALETTE_SHIFT);
        break;
    case NJD_TEXFMT_PALETTIZE8:
    case NJD_TEXFMT_PALETTIZE8_MM:
        bank = tex->bank & BANK_MASK_8BPP;
        param = (param & ~TEX_PALETTE) | (bank << TEX_PALETTE_SHIFT);
        break;
    }
    tex->texparambuffer = param;
}
