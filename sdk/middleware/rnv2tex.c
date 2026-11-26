/* the renderer's textures and palette (sg_mwrv2.h). a texture's surface has
 * sides that are powers of two, and can be longer than the picture's */
#include <kamui.h>
#include <mw_mvf.h>
#include <sg_mw.h>
#include <sg_mwrv2.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <sj.h>

#define P2_TEXTURE(image) ((KMDWORD *)((Uint32)(image) | P2_BASE))
#define ALLOC_ERROR "mwRnv2AllocTex: can't alloc KMSURFACEDESC"
#define YUV420_ERROR "mwRnv2LoadTexYuv420: can't load Yuv420"
/* the bytes of an ARGB8888 color */
#define COLOR_BYTES 4
static KMPALETTEDATA rnv2_palette;

/* the member exports these names, though no header of the SDK declares
 * them. no code reads or sets them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Uint32 mwg_loadyuv420_td1;
Uint32 mwg_loadyuv420_td2;
Uint32 mwg_loadyuv422_td1;
Uint32 mwg_loadyuv422_td2;
Uint32 mwg_loadplt_td1;
Uint32 mwg_loadplt_td2;
/* NOLINTEND(misc-use-internal-linkage) */

void mwlrnv2_calc_texsize(Sint32 width, Sint32 height, Sint32 *padded_width,
                          Sint32 *padded_height)
{
    Sint32 bits;

    if (width > 0) {
        width--;
        for (bits = 0; width != 0; bits++) {
            /* @hack spelling: width = width >> 1, where width >>= 1 would do */
            width = width >> 1;
        }
        *padded_width = 1 << bits;
    } else {
        *padded_width = 0;
    }
    if (height > 0) {
        height--;
        for (bits = 0; height != 0; bits++) {
            /* @hack spelling: height = height >> 1, where height >>= 1 would
             * do */
            height = height >> 1;
        }
        *padded_height = 1 << bits;
    } else {
        *padded_height = 0;
    }
}

/* a YUV picture narrower than its surface takes a stride texture.
 * kmSetStrideWidth sets the stride, which every stride texture shares */
KMSURFACEDESC *mwRnv2AllocTex(Sint32 format, Sint32 width, Sint32 height,
                              Sint32 count, KMSURFACEDESC *surfaces)
{
    KMSTATUS status;
    KMTEXTURETYPE type;
    Sint32 i;
    /* @hack redundant: unused, a chunk that only sizes the frame */
    SJCK unused;
    Sint32 padded_width;
    Sint32 padded_height;
    Sint32 side;
    /* @hack redundant: reserved, 44 bytes that only size the frame */
    Uint8 reserved[44];

    if (format == MVF_FMT_PLT8) {
        side = width > height ? width : height;
        mwlrnv2_calc_texsize(side, side, &padded_width, &padded_height);
        type = KM_TEXTURE_PALETTIZE8;
    } else {
        mwlrnv2_calc_texsize(width, height, &padded_width, &padded_height);
        type = KM_TEXTURE_YUV422;
        if (width != padded_width) {
            type |= KM_TEXTURE_STRIDE;
            kmSetStrideWidth(width);
        } else {
            type |= KM_TEXTURE_RECTANGLE;
        }
    }
    for (i = 0; i < count; i++) {
        status = kmCreateTextureSurface(&surfaces[i], padded_width,
                                        padded_height, type);
        if (status != KMSTATUS_SUCCESS) {
            mwMngError(ALLOC_ERROR);
            return NULL;
        }
    }
    return surfaces;
}

void mwRnv2ChangeTex(MWRNV2 rnv, Sint32 format, Sint32 width, Sint32 height)
{
    KMSTATUS status;
    KMTEXTURETYPE type;
    Sint32 i;
    /* @hack redundant: unused, a chunk that only sizes the frame */
    SJCK unused;
    Sint32 padded_width;
    Sint32 padded_height;
    Sint32 count;
    KMSURFACEDESC *surfaces;

    mwRnv2FreeTex(rnv->surfaces, rnv->frame_count);
    count = rnv->frame_count;
    surfaces = rnv->surfaces;
    padded_width = rnv->texture_width;
    padded_height = rnv->texture_height;
    if (format == MVF_FMT_PLT8) {
        type = KM_TEXTURE_PALETTIZE8;
    } else {
        type = KM_TEXTURE_YUV422;
        if (width != padded_width) {
            type |= KM_TEXTURE_STRIDE;
            kmSetStrideWidth(width);
        } else {
            type |= KM_TEXTURE_RECTANGLE;
        }
    }
    for (i = 0; i < count; i++) {
        status = kmCreateTextureSurface(&surfaces[i], padded_width,
                                        padded_height, type);
        if (status != KMSTATUS_SUCCESS) {
            mwMngError(ALLOC_ERROR);
            return;
        }
    }
}

void mwRnv2FreeTex(KMSURFACEDESC *surfaces, Sint32 count)
{
    KMSTATUS status;
    Sint32 i;
    /* @hack spelling: limit, a copy of count, where count would do */
    Sint32 limit;

    limit = count;
    for (i = 0; i < limit; i++) {
        status = kmFreeTexture(&surfaces[i]);
        if (status != KMSTATUS_SUCCESS) {
            mwMngError("mwRnv2FreeTex: can't free KMSURFACEDESC");
            return;
        }
    }
}

/* Kamui loads a YUV picture of the size in the surface's width and height.
 * mwRnv2LoadTexYuv420 and mwRnv2LoadTexYuv422 set them to the
 * picture's for the load, and put them back after it */
void mwRnv2LoadTexYuv420(MWRNV2 rnv, Sint32 index, Uint8 *image)
{
    /* @hack spelling: texture, a copy of image, where image would do */
    Uint8 *texture;
    KMSURFACEDESC *surface;
    KMSTATUS status;
    Uint32 width;
    Uint32 height;
    /* @hack redundant: unused, three words that only size the frame */
    Uint32 unused[3];

    texture = image;
    surface = &rnv->surfaces[index];
    width = surface->u0.Width;
    height = surface->u1.Height;
    surface->u0.Width = rnv->width;
    surface->u1.Height = rnv->height;
    status = kmLoadYUVTexture(surface, P2_TEXTURE(texture), 1, 1,
                              KM_TEXTURE_YUV420, TRUE);
    surface->u0.Width = width;
    surface->u1.Height = height;
    if (status != KMSTATUS_SUCCESS) {
        mwMngError(YUV420_ERROR);
    }
}

void mwRnv2LoadTexYuv422(MWRNV2 rnv, Sint32 index, Uint8 *image)
{
    /* @hack spelling: texture, a copy of image, where image would do */
    Uint8 *texture;
    KMSURFACEDESC *surface;
    KMSTATUS status;
    Uint32 width;
    Uint32 height;
    /* @hack redundant: unused, three words that only size the frame */
    Uint32 unused[3];

    texture = image;
    surface = &rnv->surfaces[index];
    width = surface->u0.Width;
    height = surface->u1.Height;
    surface->u0.Width = rnv->width;
    surface->u1.Height = rnv->height;
    status = kmLoadTexture(surface, P2_TEXTURE(texture), FALSE, FALSE);
    if (status != KMSTATUS_SUCCESS) {
        mwMngError("mwRnv2LoadTexYuv422: can't load Yuv422");
    }
    surface->u0.Width = width;
    surface->u1.Height = height;
}

void mwRnv2LoadTex(MWRNV2 rnv, Sint32 index, Uint8 *image)
{
    /* @hack spelling: texture, a copy of image, where image would do */
    Uint8 *texture;
    KMSURFACEDESC *surface;
    KMSTATUS status;
    /* @hack redundant: unused, three words that only size the frame */
    Uint32 unused[3];

    texture = image;
    surface = &rnv->surfaces[index];
    status = kmLoadTexture(surface, P2_TEXTURE(texture), FALSE, FALSE);
    if (status != KMSTATUS_SUCCESS) {
        mwMngError(YUV420_ERROR);
    }
}

void mwRnv2LoadPltARGB8888(MWRNV2 rnv, Uint8 *palette, Sint32 start,
                           Sint32 count)
{
    Sint32 offset;
    Sint32 entry;
    /* @hack spelling: colors, a copy of palette, where palette would do */
    Uint8 *colors;
    KMSTATUS status;

    colors = palette;
    entry = start;
    for (offset = 0; offset < count * COLOR_BYTES; offset += COLOR_BYTES) {
        rnv2_palette.dwPaletteData[entry++] =
            ((Uint32)colors[offset + 3] << 24) +
            ((Uint32)colors[offset + 2] << 16) +
            ((Uint32)colors[offset + 1] << 8) + (Uint32)colors[offset];
    }
    status = kmSetPaletteMode(KM_PALETTE_32BPP_ARGB8888);
    if (status != KMSTATUS_SUCCESS) {
        mwMngError("mwRnv2LoadPltARGB888: can't load pallete ARGB8888");
    }
    kmSetPaletteBankData(start, count, &rnv2_palette);
}
