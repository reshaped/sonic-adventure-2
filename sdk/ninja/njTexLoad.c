/* a texture's texels loaded into its surface or the texture cache, from the
 * open PVR file or memory */
#include <kamui.h>
#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* create info's surface from its sides and Type, as njCreateTextureSurface
 * does, and return 1. where that fails, the call sets _nj_texerr_.texerr to
 * NJD_TEXERR_SURFACE and returns -NJD_TEXERR_SURFACE */
Sint32 njGetInfoMemory(NJS_TEXINFO *info);
/* NOLINTEND(misc-use-internal-linkage) */

/* #pragma section 32 puts the buffer in BSG32, a bss section of the SDK's
 * own. the image holds it there on a 32-byte boundary. gdFsRead reads sectors
 * straight into a buffer so aligned, and into others through a request of
 * its own */
#ifdef NONMATCHING
/* this side aligns the buffer by GCC's attribute, so gdFsRead reads sectors
 * straight into it */
Sint8 _nj_file_buffer[GDD_FS_SCTSIZE] __attribute__((aligned(32)));
#else
#pragma section 32
Sint8 _nj_file_buffer[GDD_FS_SCTSIZE];
#pragma section
#endif

Sint32 njLoadTexMemFromFile(NJS_TEXNAME *texname, NJS_TEXINFO *info)
{
    /* @hack redundant: name and pvrt, a PVRT chunk's name and the rest of
     * it, which nothing uses */
    /* NOLINTBEGIN(clang-diagnostic-unused-variable) */
    Uint8 name[4];
    struct pvrt_chunk pvrt;
    /* NOLINTEND(clang-diagnostic-unused-variable) */
    Sint32 size;
    Sint32 result;
    Sint32 texture_size;
    Uint32 part;
    Uint32 offset;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((result = njGetSurfacePvr(info, texname->attr, &size)) < 0) {
        return result;
    }
    texture_size = info->texsurface.TextureSize;
    size -= sizeof(NJS_PVRHEADERDATA);
    /* @hack spelling: size less texture_size as a subtraction of its own,
     * where size -= texture_size would do */
    if (size > texture_size) {
        size = size - texture_size;
    } else {
        texture_size = size;
        size = 0;
    }
    if ((Uint32)texture_size <= _nj_texture_buffer_max_size) {
        part = texture_size;
    } else {
        part = _nj_texture_buffer_max_size;
    }
    offset = 0;
    /* @hack spelling: a do-while under a test of the texture's size, where a
     * while loop would do */
    if (texture_size > 0) {
        do {
            if (njTexFread(_nj_texture_buffer_st, part) < 0) {
                kmFreeTexture((PKMSURFACEDESC)&info->texsurface);
                return -1;
            }
            syCacheOCWB(_nj_texture_buffer_st, part);
            njLoadTexturePart(&info->texsurface,
                              (Uint32 *)_nj_texture_buffer_st, offset, part);
            offset += part;
            if (texture_size - offset < _nj_texture_buffer_max_size) {
                part = texture_size - offset;
            }
            while (kmQueryFinishLastTextureDMA() != KMSTATUS_SUCCESS) {
            }
        } while (offset != (Uint32)texture_size);
    }
    if (size > 0) {
        njTexFseek(size, GDD_SEEK_CUR);
    }
    return 1;
}

Sint32 njLoadCacheFromFile(NJS_TEXNAME *texname, NJS_TEXINFO *info)
{
    /* @hack redundant: name and pvrt, a PVRT chunk's name and the rest of
     * it, which nothing uses */
    /* NOLINTBEGIN(clang-diagnostic-unused-variable) */
    Uint8 name[4];
    struct pvrt_chunk pvrt;
    /* NOLINTEND(clang-diagnostic-unused-variable) */
    Sint32 size;
    Sint32 result;
    Sint32 texture_size;
    void *cache;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((result = njGetInfoPvr(info, texname->attr, &size)) < 0) {
        return result;
    }
    texture_size = info->texsurface.TextureSize;
    size -= sizeof(NJS_PVRHEADERDATA);
    /* @hack spelling: size less texture_size as a subtraction of its own,
     * where size -= texture_size would do */
    if (size > texture_size) {
        size = size - texture_size;
    } else {
        texture_size = size;
        size = 0;
    }
    cache = njCacheTextureMalloc(texture_size);
    if (cache == NULL) {
        _nj_texerr_.texerr = NJD_TEXERR_MAINMEMORY;
        return -NJD_TEXERR_MAINMEMORY;
    }
    if (njTexFread(cache, texture_size) < 0) {
        return -NJD_TEXERR_FILELOAD;
    }
    syCacheOCWB(cache, texture_size);
    if (size > 0) {
        njTexFseek(size, GDD_SEEK_CUR);
    }
    info->texaddr = cache;
    return 1;
}

Sint32 njLoadCacheFromMemory(NJS_TEXNAME *texname, NJS_TEXINFO *info)
{
    /* @hack redundant: name and pvrt, a PVRT chunk's name and the rest of
     * it, which nothing uses */
    /* NOLINTBEGIN(clang-diagnostic-unused-variable) */
    Uint8 name[4];
    struct pvrt_chunk pvrt;
    /* NOLINTEND(clang-diagnostic-unused-variable) */
    Sint32 result;
    Uint32 size;
    void *cache;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((result = njGetInfoMemory(info)) < 0) {
        return result;
    }
    size = info->texsurface.TextureSize;
    cache = njCacheTextureMalloc(size);
    if (cache == NULL) {
        _nj_texerr_.texerr = NJD_TEXERR_MAINMEMORY;
        return -NJD_TEXERR_MAINMEMORY;
    }
    njMemCopy(cache, info->texaddr, size);
    syCacheOCWB(cache, size);
    info->texaddr = cache;
    return 1;
}

Sint32 njLoadTexMemFromMemory(NJS_TEXNAME *texname, NJS_TEXINFO *info)
{
    Sint32 result;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((result = njGetInfoMemory(info)) < 0) {
        return result;
    }
    if (njLoadTextureLow(&info->texsurface, info->texaddr) !=
        KMSTATUS_SUCCESS) {
        kmFreeTexture((PKMSURFACEDESC)&info->texsurface);
        _nj_texerr_.texerr = NJD_TEXERR_TEXMEMLOAD;
        return -NJD_TEXERR_TEXMEMLOAD;
    }
    while (kmQueryFinishLastTextureDMA() != KMSTATUS_SUCCESS) {
    }
    return 1;
}

Sint32 njGetInfoMemory(NJS_TEXINFO *info)
{
    /* @hack spelling: the arguments through locals, where reads in the call
     * would do */
    Uint32 width = info->texsurface.nWidth;
    Uint32 height = info->texsurface.nHeight;
    Uint32 type = info->texsurface.Type >> TEXSURFACE_FORMAT_SHIFT;

    if (njCreateTextureSurface(&info->texsurface, width, height, type) !=
        KMSTATUS_SUCCESS) {
        _nj_texerr_.texerr = NJD_TEXERR_SURFACE;
        return -NJD_TEXERR_SURFACE;
    }
    return 1;
}
