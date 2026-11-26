#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>

Sint32 njReLoadTextureNumG(Uint32 globalIndex, void *texaddr, Uint32 attr,
                           Uint32 lod)
{
    Sint32 n;
    Sint32 result;

    n = njSearchTexList(_nj_texmemlist, _nj_texmemsize, globalIndex);
    if (n < 0) {
        _nj_texerr_.globalIndex = globalIndex;
        _nj_texerr_.texerr = NJD_TEXERR_OTHER;
        return -1;
    }
    njInitTextureErrStat();
    result = njReLoadTextureLN(&_nj_texmemlist[n], texaddr, attr, lod);
    if (result < 0) {
        _nj_texerr_.globalIndex = globalIndex;
    }
    return result;
}
