#include <kamui.h>
#include <kamui_internal.h>
#include <kamui_texture.h>
#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>

/* @hack spelling: the body in a function SHC expands, its parameters in
 * this order, where the body in njCreateTextureSurface would do */
#pragma inline(create_surface)
static Sint32 create_surface(Uint32 nTextureType, Uint32 nHeight,
                             NJS_TEXSURFACE *surface, Uint32 nWidth)
{
    Sint32 status;

    status = njCheckTextureFormatLow(surface, nWidth, nHeight, nTextureType);
    if (status != KMSTATUS_SUCCESS) {
        return status;
    }
    if (SURFACE_KIND(surface) == KM_SURFACETYPE_SMALLVQ_TEXTURE) {
        status = kmiVQalloc((PKMSURFACEDESC)surface);
    } else {
        status = kmiTXalloc(surface->TextureSize, (PKMSURFACEDESC)surface);
    }
    if (status == KMSTATUS_NOT_ENOUGH_MEMORY) {
        kmiDefaultInterruptHandler(TEXTURE_OVERFLOW_CALLBACK);
    }
    return status;
}

Sint32 njCreateTextureSurface(NJS_TEXSURFACE *surface, Uint32 nWidth,
                              Uint32 nHeight, Uint32 nTextureType)
{
    return create_surface(nTextureType, nHeight, surface, nWidth);
}
