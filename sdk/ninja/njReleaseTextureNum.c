#include <kamui.h>
#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>

Sint32 njReleaseTextureLN(Uint32 texaddr)
{
    NJS_TEXMEMLIST *tex = (NJS_TEXMEMLIST *)texaddr;
    Uint32 i;
    Uint32 globalIndex;
    Bool palettized;

    if (tex->texaddr & TEXMEM_LOADED) {
        palettized = njCheckPaletteType(
            (tex->texinfo.texsurface.Type >> TEXSURFACE_FORMAT_SHIFT) &
            NJD_TEXFMT_TYPE_MASK);
        /* @hack spelling: palettized compared with TRUE, where a test of
         * palettized alone would do */
        if (palettized != TRUE) {
            if (tex->count != 0) {
                tex->count--;
            }
            if (tex->count == 0) {
                kmFreeTexture((PKMSURFACEDESC)&tex->texinfo.texsurface);
                tex->texaddr &= ~TEXMEM_LOADED;
                if (_nj_current_texture == tex->globalIndex) {
                    _nj_current_texture = -1;
                }
                if (!(tex->texaddr & TEXMEM_CACHED)) {
                    njFreeTexMemList(tex);
                }
            }
        } else {
            globalIndex = tex->globalIndex;
            if (tex->count > 1) {
                for (i = 0; i < _nj_texmemsize; i++) {
                    if (_nj_texmemlist[i].globalIndex == globalIndex) {
                        _nj_texmemlist[i].count--;
                    }
                }
            } else {
                kmFreeTexture((PKMSURFACEDESC)&tex->texinfo.texsurface);
                if (tex->texaddr & TEXMEM_CACHED) {
                    for (i = 0; i < _nj_texmemsize; i++) {
                        if (_nj_texmemlist[i].globalIndex == globalIndex) {
                            _nj_texmemlist[i].count = 0;
                            _nj_texmemlist[i].texaddr &=
                                ~(TEXMEM_LOADED | TEXMEM_SHARED_SURFACE);
                        }
                    }
                } else {
                    for (i = 0; i < _nj_texmemsize; i++) {
                        if (_nj_texmemlist[i].globalIndex == globalIndex) {
                            njFreeTexMemList(&_nj_texmemlist[i]);
                        }
                    }
                }
            }
        }
    } else {
        if (!(tex->texaddr & TEXMEM_CACHED)) {
            /* @hack spelling: a goto into the error's arm, where one test
             * of both conditions would do */
            if (tex->globalIndex == TEXMEM_FREE) {
                goto error;
            }
            njFreeTexMemList(tex);
        } else {
        error:
            _nj_texerr_.texerr = NJD_TEXERR_OTHER;
            return -1;
        }
    }
    return 1;
}

Sint32 njReleaseTextureNum(Uint32 n)
{
    Sint32 result;

    result = njReleaseTextureLN(_nj_current_texlist->textures[n].texaddr);
    if (result < 0) {
        _nj_texerr_.n = n;
    }
    return result;
}
