/* a PVR texture's header read from memory, and a record's surface made,
 * filled, read and freed */
#include <kamui.h>
#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>

/* DUMMY_SIDE is _nj_dummy_texsurface's side. DUMMY_SIZE, its TextureSize, is
 * the bytes of a 16-bit texture that size with its mipmaps, and 2 more */
#define DUMMY_SIDE 1024
#define DUMMY_SIZE 0x2aaaac

/* the member exports these functions and this datum, though no header of the
 * SDK declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* create tex's surface for the texture pvrh describes, as
 * njCreateTextureSurface does, and return 1. where that fails, the call
 * sets _nj_texerr_.texerr to NJD_TEXERR_SURFACE and returns -1 */
Sint32 njGetTexSurface(NJS_TEXMEMLIST *tex, NJS_PVRHEADERDATA *pvrh);
/* copy a part of the texture at pTexture into tex's surface, as
 * njLoadTexturePart does, and return 1. where that fails, the call
 * sets _nj_texerr_.texerr to NJD_TEXERR_TEXMEMLOAD and returns -1 */
Sint32 njLoadTexturePartLow(NJS_TEXMEMLIST *tex, Uint32 *pTexture,
                            Uint32 offset, Uint32 size);
/* copy as njLoadTexturePartLow does, but through kmLoadTexturePart, which
 * refuses a small VQ surface. a small VQ texture's part goes through
 * _nj_dummy_texsurface instead, a mipmapped, twiddled surface pointed
 * TWIDDLED_MM_SKIP bytes before tex's. a copy into such a surface skips that
 * many bytes, as njLoadTexturePart's does. the part so lands at tex's own */
Sint32 njLoadTexturePartLowOld(NJS_TEXMEMLIST *tex, Uint32 *pTexture,
                               Uint32 offset, Uint32 size);
/* copy size bytes of tex's surface, from offset bytes in, to pTexture, and
 * return 1. njTexMemCopy makes the copy, so size must be above 0. a null tex
 * returns -1 */
Sint32 njReadTexturePartLow(NJS_TEXMEMLIST *tex, Uint32 *pTexture,
                            Uint32 offset, Uint32 size);
/* free tex's texture memory through kmFreeTexture, and return 1, or -1 where
 * that fails */
Sint32 njReleaseTextureLow(NJS_TEXMEMLIST *tex);
/* clang-format off */
/* a mipmapped, twiddled surface DUMMY_SIDE texels a side.
 * njLoadTexturePartLowOld sets its pSurface for each small VQ part */
NJS_TEXSURFACE _nj_dummy_texsurface = {
    0, 0, 0, DUMMY_SIDE, DUMMY_SIDE, DUMMY_SIZE,
    NJD_SURFACEFLAGS_MIPMAPED | NJD_SURFACEFLAGS_TWIDDLED, NULL, NULL, NULL};
/* clang-format on */
/* NOLINTEND(misc-use-internal-linkage) */

/* the two small VQ formats, as a surface's Type holds them */
#define SURFACE_SMALLVQ (NJD_TEXFMT_SMALLVQ << TEXSURFACE_FORMAT_SHIFT)
#define SURFACE_SMALLVQ_MM (NJD_TEXFMT_SMALLVQ_MM << TEXSURFACE_FORMAT_SHIFT)
/* texture memory's 64-bit area, at its uncached P2 address */
#define TEXTURE_MEMORY_P2 0xa4000000

Uint32 *njLoadPvrHeader(NJS_PVRHEADERDATA *pvrh, Uint32 *globalIndex,
                        Uint32 *pTexture)
{
    Uint32 name;
    Uint32 size;

    *globalIndex = TEXMEM_FREE;
    do {
        name = *pTexture++;
        switch (name) {
        case iff_PVRT:
            pTexture++;
            njMemCopy(pvrh, pTexture, sizeof(NJS_PVRHEADERDATA));
            pTexture += sizeof(NJS_PVRHEADERDATA) / sizeof(Uint32);
            break;
        case iff_GBIX:
            size = *pTexture++;
            *globalIndex = *pTexture;
            pTexture += size / sizeof(Uint32);
            break;
        default:
            return NULL;
        }
    } while (name != iff_PVRT);
    return pTexture;
}

Sint32 njGetTexSurface(NJS_TEXMEMLIST *tex, NJS_PVRHEADERDATA *pvrh)
{
    NJS_TEXSURFACE *surface;
    Uint32 width;
    Uint32 height;
    Uint32 type;

    /* @hack spelling: the arguments through locals, where reads in the call
     * would do */
    surface = &tex->texinfo.texsurface;
    width = pvrh->nWidth;
    height = pvrh->nHeight;
    type = pvrh->nTextureType;
    if (njCreateTextureSurface(surface, width, height, type) !=
        KMSTATUS_SUCCESS) {
        _nj_texerr_.texerr = NJD_TEXERR_SURFACE;
        return -1;
    }
    return 1;
}

Sint32 njLoadTexturePartLow(NJS_TEXMEMLIST *tex, Uint32 *pTexture,
                            Uint32 offset, Uint32 size)
{
    if (njLoadTexturePart(&tex->texinfo.texsurface, pTexture, offset, size) !=
        KMSTATUS_SUCCESS) {
        _nj_texerr_.texerr = NJD_TEXERR_TEXMEMLOAD;
        return -1;
    }
    return 1;
}

Sint32 njLoadTexturePartLowOld(NJS_TEXMEMLIST *tex, Uint32 *pTexture,
                               Uint32 offset, Uint32 size)
{
    /* @bug logic: any texture whose Type has bit 24 or 28 set goes as a small
     * VQ one, a TWIDDLED texture among them. the test ANDs Type with each
     * small VQ format, where it means to compare Type's format with them:
     * TST R4,R3 at 0x8c15d1ce and TST R4,R1 at 0x8c15d1d4 */
    if ((tex->texinfo.texsurface.Type & SURFACE_SMALLVQ) ||
        (tex->texinfo.texsurface.Type & SURFACE_SMALLVQ_MM)) {
        _nj_dummy_texsurface.pSurface = tex->texinfo.texsurface.pSurface -
                                        (TWIDDLED_MM_SKIP / sizeof(Uint32));
        if (kmLoadTexturePart((PKMSURFACEDESC)&_nj_dummy_texsurface, pTexture,
                              offset, size) != KMSTATUS_SUCCESS) {
            _nj_texerr_.texerr = NJD_TEXERR_TEXMEMLOAD;
            return -1;
        }
    } else {
        if (kmLoadTexturePart((PKMSURFACEDESC)&tex->texinfo.texsurface,
                              pTexture, offset, size) != KMSTATUS_SUCCESS) {
            _nj_texerr_.texerr = NJD_TEXERR_TEXMEMLOAD;
            return -1;
        }
    }
    return 1;
}

Sint32 njReadTexturePartLow(NJS_TEXMEMLIST *tex, Uint32 *pTexture,
                            Uint32 offset, Uint32 size)
{
    void *addr;

    if (tex == NULL) {
        return -1;
    }
    addr = (void *)(((Uint32)tex->texinfo.texsurface.pSurface |
                     TEXTURE_MEMORY_P2) +
                    offset);
    njTexMemCopy(pTexture, addr, size);
    return 1;
}

Sint32 njReleaseTextureLow(NJS_TEXMEMLIST *tex)
{
    if (kmFreeTexture((PKMSURFACEDESC)&tex->texinfo.texsurface) !=
        KMSTATUS_SUCCESS) {
        return -1;
    }
    return 1;
}

Sint32 njLoadTextureFinishDMA(void)
{
    if (kmQueryFinishLastTextureDMA() == KMSTATUS_SUCCESS) {
        return 1;
    }
    return -1;
}
