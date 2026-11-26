/* Ninja's texture reloads, into the surface each texture's record holds */
#include <kamui.h>
#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* the dummy bytes before a mipmapped texture's 1 by 1 mipmap in its file */
#define MIPMAP_FILE_DUMMY 2

Sint32 njReLoadTextureLN(NJS_TEXMEMLIST *tex, void *texaddr, Uint32 attr,
                         Uint32 lod)
{
    Uint32 offset;
    Uint32 size;
    Uint32 name;
    Sint32 level;
    Uint32 length;
    char path[512];
    NJS_TEXINFO info;
    struct pvrt_chunk pvrt;
    Uint32 type;
    Uint32 part;
    char *filename;
    /* @hack spelling: info through texinfo, where info itself would do */
    NJS_TEXINFO *texinfo;

    type = (tex->texinfo.texsurface.Type >> TEXSURFACE_FORMAT_SHIFT) &
           NJD_TEXFMT_TYPE_MASK;
    if (attr & NJD_TEXATTR_TYPE_MEMORY) {
        if ((tex->texinfo.texsurface.fSurfaceFlags &
             NJD_SURFACEFLAGS_MIPMAPED) &&
            lod) {
            if (kmReLoadMipmap((PKMSURFACEDESC)&tex->texinfo.texsurface,
                               texaddr, (KMINT32)lod) != KMSTATUS_SUCCESS) {
                _nj_texerr_.texerr = NJD_TEXERR_TEXMEMLOAD;
                return -1;
            }
            while (kmQueryFinishLastTextureDMA() != KMSTATUS_SUCCESS) {
            }
        } else {
            if (njLoadTextureLow(&tex->texinfo.texsurface, texaddr) !=
                KMSTATUS_SUCCESS) {
                _nj_texerr_.texerr = NJD_TEXERR_TEXMEMLOAD;
                return -1;
            }
        }
    } else {
        /* @hack spelling: filename, a copy of texaddr, where texaddr passed
         * straight would do */
        filename = texaddr;
        njCheckFileName(path, filename, ".PVR");
        if (njTexFopen(path) < 0) {
            return -1;
        }
        texinfo = &info;
        /* @hack redundant: a test of texinfo that never runs */
        if (0) {
            if (texinfo) {
            }
        }
        texinfo->texsurface = tex->texinfo.texsurface;
        njTexFseek(0, GDD_SEEK_SET);
        do {
            if (njTexFread(&name, sizeof(name)) < 0) {
                njTexFclose();
                return -1;
            }
            switch (name) {
            case iff_PVRT:
                if (njTexFread(&pvrt, sizeof(pvrt)) < 0) {
                    njTexFclose();
                    return -1;
                }
                break;
            default:
                /* @bug logic: a failed read of a chunk's size closes the file
                 * and goes on, where every other failed read returns.
                 * JSR @R2 at 0x8c116242 closes it, and 0x8c116246 then seeks
                 * the closed file by length, which the read didn't set */
                if (njTexFread(&length, sizeof(length)) < 0) {
                    njTexFclose();
                }
                njTexFseek(length, GDD_SEEK_CUR);
                break;
            }
        } while (name != iff_PVRT);
        switch (type) {
        case NJD_TEXFMT_SMALLVQ:
        case NJD_TEXFMT_SMALLVQ_MM:
        case NJD_TEXFMT_VQ:
        case NJD_TEXFMT_VQ_MM:
        case NJD_TEXFMT_PALETTIZE4:
        case NJD_TEXFMT_PALETTIZE4_MM:
        case NJD_TEXFMT_PALETTIZE8:
        case NJD_TEXFMT_PALETTIZE8_MM:
            size = texinfo->texsurface.TextureSize;
            if (size <= _nj_texture_buffer_max_size) {
                part = size;
            } else {
                part = _nj_texture_buffer_max_size;
            }
            offset = 0;
            do {
                if (njTexFread(_nj_texture_buffer_st, part) < 0) {
                    njTexFclose();
                    return -1;
                }
                syCacheOCWB(_nj_texture_buffer_st, part);
                njLoadTexturePart(&texinfo->texsurface,
                                  (Uint32 *)_nj_texture_buffer_st, offset,
                                  part);
                offset += part;
                if (size - offset < _nj_texture_buffer_max_size) {
                    part = size - offset;
                }
                while (kmQueryFinishLastTextureDMA() != KMSTATUS_SUCCESS) {
                }
            } while (offset != size);
            break;
        default:
            if ((texinfo->texsurface.fSurfaceFlags &
                 NJD_SURFACEFLAGS_MIPMAPED) &&
                lod) {
                if (lod == 1) {
                    njTexFseek(MIPMAP_FILE_DUMMY, GDD_SEEK_CUR);
#ifdef NONMATCHING
                    /* this side repairs the uninitialized read below: it gives
                     * level 0, the 1 by 1 mipmap's level, which the console
                     * reads unset on this path. it keeps the logic bug below,
                     * so the texels land at the 2 by 2 mipmap's place */
                    level = 0;
#endif
                } else {
                    level = njGetTextureLevel(lod, lod);
                    /* @bug logic: a lod that isn't a power of two returns
                     * with the file still open, where each other return
                     * after the open closes it. BF/S at 0x8c116340 goes
                     * to 0x8c1163e8, which returns without njTexFclose */
                    if (level < 0) {
                        return -1;
                    }
                    njTexFseek(_nj_ImageByteSize[level - 1], GDD_SEEK_CUR);
                }
                size = lod * lod * TEXEL_BYTES;
                if (size < _nj_texture_buffer_max_size) {
                    part = size;
                } else {
                    part = _nj_texture_buffer_max_size;
                }
                offset = 0;
                do {
                    if (njTexFread(_nj_texture_buffer_st, part) < 0) {
                        njTexFclose();
                        return -1;
                    }
                    syCacheOCWB(_nj_texture_buffer_st, part);
                    /* @bug logic: writes the mipmap of side lod over the
                     * mipmap of side 2 * lod. the seek passes
                     * _nj_ImageByteSize[level - 1] bytes: the dummy bytes and
                     * the mipmaps smaller than lod's (ADD #-1,R4 at
                     * 0x8c11634a). the copy's offset is
                     * _nj_ImageByteSize[level], which counts lod's mipmap too
                     * (MOV.L @(12,R15),R2 at 0x8c11636e). for a TWIDDLED_MM
                     * texture of side lod, the copy starts at or past the
                     * surface's end, and copies nothing. the game's one call,
                     * by njReLoadTextureNumG, loads from memory with a lod of
                     * 0 */
                    /* @bug uninitialized: a lod of 1 indexes
                     * _nj_ImageByteSize by level, which only the other lods
                     * set. MOV.L R0,@(12,R15) at 0x8c116342 is level's one
                     * store, and MOV.L @(12,R15),R2 at 0x8c11636e reads it */
                    njLoadTexturePart(&texinfo->texsurface,
                                      (Uint32 *)_nj_texture_buffer_st,
                                      _nj_ImageByteSize[level] + offset, part);
                    offset += part;
                    if (size - offset < _nj_texture_buffer_max_size) {
                        part = size - offset;
                    }
                    while (kmQueryFinishLastTextureDMA() != KMSTATUS_SUCCESS) {
                    }
                } while (offset != size);
            } else {
                size = texinfo->texsurface.TextureSize;
                if (size < _nj_texture_buffer_max_size) {
                    part = size;
                } else {
                    part = _nj_texture_buffer_max_size;
                }
                offset = 0;
                do {
                    if (njTexFread(_nj_texture_buffer_st, part) < 0) {
                        njTexFclose();
                        return -1;
                    }
                    syCacheOCWB(_nj_texture_buffer_st, part);
                    njLoadTexturePart(&texinfo->texsurface,
                                      (Uint32 *)_nj_texture_buffer_st, offset,
                                      part);
                    offset += part;
                    if (size - offset < _nj_texture_buffer_max_size) {
                        part = size - offset;
                    }
                    while (kmQueryFinishLastTextureDMA() != KMSTATUS_SUCCESS) {
                    }
                } while (offset != size);
            }
        }
        njTexFclose();
    }
    return 1;
}

Sint32 njReLoadTextureNum(Uint32 n, void *texaddr, Uint32 attr, Uint32 lod)
{
    NJS_TEXMEMLIST *tex;
    Sint32 result;

    tex = (NJS_TEXMEMLIST *)_nj_current_texlist->textures[n].texaddr;
    njInitTextureErrStat();
    result = njReLoadTextureLN(tex, texaddr, attr, lod);
    if (result < 0) {
        _nj_texerr_.n = n;
    }
    return result;
}
