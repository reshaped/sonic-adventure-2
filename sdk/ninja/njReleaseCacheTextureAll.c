#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>

void njReleaseCacheTextureAll(void)
{
    Uint32 i;
    NJS_TEXMEMLIST *tex;
    Bool palettized;
    Uint32 globalIndex;

    for (i = 0; i < _nj_texmemsize; i++) {
        tex = &_nj_texmemlist[i];
        if (tex->texaddr & TEXMEM_CACHED) {
            palettized = njCheckPaletteType(
                (tex->texinfo.texsurface.Type >> TEXSURFACE_FORMAT_SHIFT) &
                NJD_TEXFMT_TYPE_MASK);
            njCacheTextureFree(tex->texinfo.texaddr);
            /* @hack spelling: palettized compared with TRUE, where a test of
             * palettized alone would do */
            if (palettized != TRUE) {
                tex->texaddr &= ~TEXMEM_CACHED;
                if (tex->texaddr == 0) {
                    njFreeTexMemList(tex);
                }
            } else {
                /* @hack spelling: globalIndex, a local for the record's
                 * index, where the index passed straight would do */
                globalIndex = tex->globalIndex;
                njClearFlagAddrAllTexture(globalIndex, TEXMEM_CACHED, NULL);
            }
        }
        if (tex->globalIndex != TEXMEM_FREE &&
            !(tex->texaddr & (TEXMEM_CACHED | TEXMEM_LOADED))) {
            njFreeTexMemList(tex);
        }
    }
}
