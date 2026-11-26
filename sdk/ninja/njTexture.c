/* the texture system's setup and teardown, the current texture list and
 * texture, and the texture error status */
#include <kamui.h>
#include <machine.h>
#include <nj_global.h>
#include <nj_km.h>
#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

extern Int _nj_loadtexreq_mode_;
extern Int _nj_loadtexreq_i_;

/* _nj_user_globalIndex stays below this global index, and so below
 * TEXMEM_FREE */
#define GLOBALINDEX_RESERVED 0xfffffff0
/* an error status's n and globalIndex where it names no texture */
#define NO_TEXTURE 0xffffffff

void njInitTextureGlobalIndex(Uint32 globalIndex)
{
    if (globalIndex >= GLOBALINDEX_RESERVED) {
        _nj_user_globalIndex = GLOBALINDEX_RESERVED - 1;
    } else {
        _nj_user_globalIndex = globalIndex;
    }
}

void njInitTextureBuffer(Sint8 *addr, Uint32 size)
{
    _nj_texture_buffer_st = addr;
    _nj_texture_buffer_max_size = (size >> SCT_SHIFT) << SCT_SHIFT;
}

void njInitTexture(NJS_TEXMEMLIST *addr, Uint32 n)
{
    njInitTexMemList(addr, n);
    _nj_current_texture_path[0] = '\0';
    _nj_loadtexreq_mode_ = 2;
    _nj_loadtexreq_i_ = 0;
    _nj_tex_err_surface.Type = 0;
    _nj_tex_err_surface.BitDepth = 0;
    _nj_tex_err_surface.PixelFormat = 0;
    _nj_tex_err_surface.nWidth = 0;
    _nj_tex_err_surface.nHeight = 0;
    _nj_tex_err_surface.TextureSize = 0;
    _nj_tex_err_surface.fSurfaceFlags = 0;
    _nj_tex_err_surface.pSurface = NULL;
}

void njExitTexture(void)
{
    njReleaseTextureAll();
    njReleaseCacheTextureAll();
    _nj_tex_err_surface.Type = 0;
    _nj_tex_err_surface.BitDepth = 0;
    _nj_tex_err_surface.PixelFormat = 0;
    _nj_tex_err_surface.nWidth = 0;
    _nj_tex_err_surface.nHeight = 0;
    _nj_tex_err_surface.TextureSize = 0;
    _nj_tex_err_surface.fSurfaceFlags = 0;
    _nj_tex_err_surface.pSurface = NULL;
}

Sint32 njLoadTexture(NJS_TEXLIST *texlist)
{
    Uint32 i;

    njInitTextureErrStat();
    /* @hack spelling: nbTexture, which is unsigned, tested as <= 0, where ==
     * 0 would do */
    if (texlist->nbTexture <= 0) {
        _nj_texerr_.texerr = NJD_TEXERR_OTHER;
        return -1;
    }
    for (i = 0; i < texlist->nbTexture; i++) {
        if (njLoadTextureLN(texlist, i) < 0) {
            _nj_texerr_.n = i;
            return -1;
        }
    }
    return 1;
}

Sint32 njSetTexture(NJS_TEXLIST *texlist)
{
    if (texlist == NULL) {
        return -1;
    }
    _nj_current_texlist = texlist;
    return 1;
}

Sint32 njSetTextureNum(Uint32 n)
{
    NJS_TEXMEMLIST *tex;
    /* @hack redundant: register on tsp, where a plain local would do */
    register Uint32 tsp;
    Uint32 texture;

#ifdef NONMATCHING
    /* this side refuses a missing list, an n past its textures, and an entry
     * with no texture, which the matching side reads through */
    if (_nj_current_texlist == NULL || n >= _nj_current_texlist->nbTexture ||
        _nj_current_texlist->textures == NULL ||
        _nj_current_texlist->textures[n].texaddr == 0) {
        return -1;
    }
#endif
    tex = (NJS_TEXMEMLIST *)_nj_current_texlist->textures[n].texaddr;
    if (tex->globalIndex != _nj_current_texture ||
        _nj_current_texture_bank != tex->bank) {
        _nj_current_texture = tex->globalIndex;
        _nj_current_texture_bank = tex->bank;
        tsp = _nj_current_ctx_->TSPPARAMBUFFER;
        tsp = (tsp & ~TSP_SIZE) | (tex->tspparambuffer & TSP_SIZE);
        _nj_current_ctx_->TSPPARAMBUFFER = tsp;
        /* @hack spelling: texture, a local for the record's texture word,
         * where the word stored straight would do */
        texture = tex->texparambuffer;
        _nj_texglobal_color_ =
            tex->texinfo.texsurface.Type >> TEXSURFACE_FORMAT_SHIFT;
        _nj_current_ctx_->TexturePARAMBUFFER = texture;
        kmSetVertexRenderState(_nj_current_ctx_);
    }
    return 1;
}

Uint32 njCalcTexture(Uint32 flag)
{
    KMUINT32 size;
    KMUINT32 block;

    /* @hack redundant: the else after the return */
    if (flag == NJD_TEXMEM_MAXSIZE) {
        return _nj_tex_max_free_size;
    } else {
        kmGetFreeTextureMem(&size, &block);
        if (flag == NJD_TEXMEM_FREESIZE) {
            return size;
        }
        return block;
    }
}

void njSetTexturePath(const char *path)
{
    _builtin_strcpy(_nj_current_texture_path, path);
}

void njGarbageTexture(NJS_TEXMEMLIST *addr, Uint32 n)
{
    Uint32 i;
    Uint32 j;
    Uint32 globalIndex;
    Uint32 *surface;

    kmGarbageCollectTexture();
    for (i = 0; i < n; i++) {
        globalIndex = addr[i].globalIndex;
        if (globalIndex != TEXMEM_FREE && (addr[i].texaddr & TEXMEM_LOADED)) {
            if (addr[i].texaddr & TEXMEM_SHARED_SURFACE) {
                surface = addr[i].texinfo.texsurface.pSurface;
                for (j = 0; j < n; j++) {
                    if (globalIndex == addr[j].globalIndex) {
                        addr[j].texinfo.texsurface.pSurface = surface;
                    }
                }
            }
            njSetTextureParam(&addr[i]);
        }
    }
}

NJS_TEXERRSTAT *njGetTextureErrStat(void)
{
    return &_nj_texerr_;
}

void njInitTextureErrStat(void)
{
    _nj_texerr_.n = NO_TEXTURE;
    _nj_texerr_.globalIndex = NO_TEXTURE;
    _nj_texerr_.texerr = 0;
    _nj_texerr_.gdstat = 0;
    _nj_texerr_.gderr = 0;
}
