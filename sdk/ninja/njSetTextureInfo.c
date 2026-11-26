#include <ninja.h>
#include <sg_xpt.h>

void njSetTextureInfo(NJS_TEXINFO *info, Uint16 *tex, Sint32 type, Sint32 width,
                      Sint32 height)
{
    info->texaddr = tex;
    info->texsurface.nWidth = width;
    info->texsurface.nHeight = height;
    info->texsurface.Type = type << TEXSURFACE_FORMAT_SHIFT;
}

void njSetTextureName(NJS_TEXNAME *texname, void *addr, Uint32 globalIndex,
                      Uint32 attr)
{
    /* @hack redundant: attr tested for NJD_TEXATTR_TYPE_MEMORY with the same
     * store of addr in both arms */
    if (attr & NJD_TEXATTR_TYPE_MEMORY) {
        texname->filename = addr;
    } else {
        texname->filename = addr;
    }
    texname->attr = attr;
    texname->texaddr = globalIndex;
}
