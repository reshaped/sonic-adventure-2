#include <kamui.h>
#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>

#ifdef NONMATCHING
/* this side's PVM loader, which records the handles each list took */
#include "engine/dc/pvm.h"
#endif

void njReleaseTextureAll(void)
{
    Uint32 i;
    Uint32 j;
    NJS_TEXMEMLIST *tex;
    Bool palettized;
    Uint32 globalIndex;

#ifdef NONMATCHING
    /* this side drops the PVM loader's records of the handles each list took,
     * which the release of every texture below makes stale */
    texture_forget_all_pvms();
#endif
    for (i = 0; i < _nj_texmemsize; i++) {
        tex = &_nj_texmemlist[i];
        if (tex->texaddr & TEXMEM_LOADED) {
            palettized = njCheckPaletteType(
                (tex->texinfo.texsurface.Type >> TEXSURFACE_FORMAT_SHIFT) &
                NJD_TEXFMT_TYPE_MASK);
            kmFreeTexture((PKMSURFACEDESC)&tex->texinfo.texsurface);
            /* @hack spelling: palettized compared with TRUE, where a test of
             * palettized alone would do */
            if (palettized != TRUE) {
                tex->texaddr &= ~TEXMEM_LOADED;
                if (tex->texaddr == 0) {
                    njFreeTexMemList(tex);
                } else {
                    tex->count = 0;
                }
            } else {
                globalIndex = tex->globalIndex;
                if (tex->texaddr & TEXMEM_CACHED) {
                    for (j = 0; j < _nj_texmemsize; j++) {
                        if (_nj_texmemlist[j].globalIndex == globalIndex) {
                            _nj_texmemlist[j].count = 0;
                            _nj_texmemlist[j].texaddr &=
                                ~(TEXMEM_LOADED | TEXMEM_SHARED_SURFACE);
                        }
                    }
                } else {
                    for (j = 0; j < _nj_texmemsize; j++) {
                        if (_nj_texmemlist[j].globalIndex == globalIndex) {
                            njFreeTexMemList(&_nj_texmemlist[j]);
                        }
                    }
                }
            }
        }
        if (tex->globalIndex != TEXMEM_FREE &&
            !(tex->texaddr & (TEXMEM_CACHED | TEXMEM_LOADED))) {
            njFreeTexMemList(tex);
        }
    }
    _nj_current_texture = -1;
}
