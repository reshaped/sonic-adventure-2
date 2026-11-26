#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>

#ifdef NONMATCHING
/* this side's PVM loader, which records the handles each list took */
#include "engine/dc/pvm.h"
#endif

Sint32 njReleaseTexture(NJS_TEXLIST *texlist)
{
    Uint32 i;
    Sint32 result = 1;

#ifdef NONMATCHING
    /* this side refuses a null list, and releases a list the PVM loader filled
     * through its record of the handles the list took */
    if (texlist == NULL) {
        return -1;
    }
    if (texture_release_owned(texlist)) {
        return 1;
    }
#endif

    for (i = 0; i < texlist->nbTexture; i++) {
#ifdef NONMATCHING
        /* this side skips an entry with no texture, whose release would read
         * through a null record */
        if (texlist->textures[i].texaddr == 0) {
            continue;
        }
#endif
        if (njReleaseTextureLN(texlist->textures[i].texaddr) < 0) {
            _nj_texerr_.n = i;
            result = -1;
        }
    }
    return result;
}
