/* Ninja's texture records: taking, freeing and finding them, and their flags
 * and users */
#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>

#ifdef NONMATCHING
/* this side's PVM loader, which records the handles each list took */
#include "engine/dc/pvm.h"
#endif

/* _nj_tex_count is one past the last record in use. njEntryTexMemList takes
 * the lowest free record, so it never takes one past the count */
extern Sint32 _nj_tex_count;

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the index of the record at tex among the n records at list. it returns -1
 * for a null tex or list, or where tex isn't one of them */
Sint32 njSearchTexList2(NJS_TEXMEMLIST *list, Uint32 n, NJS_TEXMEMLIST *tex);
/* NOLINTEND(misc-use-internal-linkage) */

/* the record njEntryTexMemList and njTexMemList try first. every record below
 * it is in use */
static Uint32 next_free = 0;

void njInitTexMemList(NJS_TEXMEMLIST *addr, Uint32 n)
{
    Uint32 i;

#ifdef NONMATCHING
    /* this side drops the PVM loader's records of the handles each list took,
     * which the table's reset below makes stale */
    texture_forget_all_pvms();
#endif
    _nj_texmemlist = addr;
    _nj_texmemsize = n;
    for (i = 0; i < _nj_texmemsize; i++) {
        _nj_texmemlist[i].globalIndex = TEXMEM_FREE;
        _nj_texmemlist[i].bank = TEXMEM_FREE;
        _nj_texmemlist[i].texaddr = 0;
        _nj_texmemlist[i].count = 0;
    }
    next_free = 0;
    _nj_tex_count = 0;
}

void njEntryTexMemList(Uint32 *handle, NJS_TEXINFO *info, Uint32 globalIndex,
                       Uint32 flag, Uint32 bank)
{
    Uint32 i;

    if (_nj_texmemlist[next_free].globalIndex == TEXMEM_FREE) {
        i = next_free;
    } else {
        for (i = 0; i < _nj_texmemsize; i++) {
            if (_nj_texmemlist[i].globalIndex == TEXMEM_FREE) {
                break;
            }
        }
    }
    if (i != _nj_texmemsize - 1) {
        next_free = i + 1;
    } else {
        next_free = 0;
    }
    *handle = (Uint32)&_nj_texmemlist[i];
    _nj_texmemlist[i].texinfo.texsurface.Type = info->texsurface.Type;
    _nj_texmemlist[i].texinfo.texsurface.BitDepth = info->texsurface.BitDepth;
    _nj_texmemlist[i].texinfo.texsurface.PixelFormat =
        info->texsurface.PixelFormat;
    _nj_texmemlist[i].texinfo.texsurface.nWidth = info->texsurface.nWidth;
    _nj_texmemlist[i].texinfo.texsurface.nHeight = info->texsurface.nHeight;
    _nj_texmemlist[i].texinfo.texsurface.TextureSize =
        info->texsurface.TextureSize;
    _nj_texmemlist[i].texinfo.texsurface.fSurfaceFlags =
        info->texsurface.fSurfaceFlags;
    _nj_texmemlist[i].texinfo.texsurface.pSurface = info->texsurface.pSurface;
    _nj_texmemlist[i].texinfo.texaddr = info->texaddr;
    _nj_texmemlist[i].globalIndex = globalIndex;
    _nj_texmemlist[i].bank = bank;
    /* @hack spelling: the flags stored, then cleared of
     * TEXMEM_SHARED_SURFACE, where one store would do */
    _nj_texmemlist[i].texaddr = flag;
    _nj_texmemlist[i].texaddr &= ~TEXMEM_SHARED_SURFACE;
    if (flag & (TEXMEM_LOADED | TEXMEM_UNK_4)) {
        _nj_texmemlist[i].count = 1;
        njSetTextureParam(&_nj_texmemlist[i]);
    } else {
        _nj_texmemlist[i].count = 0;
    }
    if ((Uint32)_nj_tex_count <= i) {
        _nj_tex_count++;
    }
}

void njFreeTexMemList(NJS_TEXMEMLIST *tex)
{
    Uint32 i;

    tex->globalIndex = TEXMEM_FREE;
    tex->bank = TEXMEM_FREE;
    tex->tspparambuffer = 0;
    tex->texparambuffer = 0;
    tex->texinfo.texaddr = NULL;
    tex->texinfo.texsurface.Type = 0;
    tex->texinfo.texsurface.BitDepth = 0;
    tex->texinfo.texsurface.PixelFormat = 0;
    tex->texinfo.texsurface.nWidth = 0;
    tex->texinfo.texsurface.nHeight = 0;
    tex->texinfo.texsurface.TextureSize = 0;
    tex->texinfo.texsurface.fSurfaceFlags = 0;
    tex->texinfo.texsurface.pSurface = NULL;
    tex->texaddr = 0;
    tex->count = 0;
    /* @hack type: the index from the records' addresses as integers, where
     * tex - _nj_texmemlist would do */
    i = ((Uint32)tex - (Uint32)_nj_texmemlist) / sizeof(NJS_TEXMEMLIST);
    if (_nj_tex_count > 0 && i == (Uint32)(_nj_tex_count - 1)) {
        while (_nj_tex_count > 0 &&
               _nj_texmemlist[_nj_tex_count - 1].globalIndex == TEXMEM_FREE) {
            _nj_tex_count--;
        }
    }
    for (i = 0; i < _nj_texmemsize; i++) {
        if (_nj_texmemlist[i].globalIndex == TEXMEM_FREE) {
            break;
        }
    }
    next_free = i;
}

Sint32 njSearchTexList(NJS_TEXMEMLIST *list, Uint32 n, Uint32 globalIndex)
{
    Uint32 i;

    if (list == NULL) {
        return -1;
    }
    for (i = 0; i < n; i++) {
        if (list[i].globalIndex == globalIndex) {
            return i;
        }
    }
    return -1;
}

Sint32 njSearchTexList2(NJS_TEXMEMLIST *list, Uint32 n, NJS_TEXMEMLIST *tex)
{
    Uint32 offset;
    Uint32 rest;
    Uint32 size;
    Uint32 i;

    if (tex == NULL) {
        return -1;
    }
    if (list == NULL) {
        return -1;
    }
    /* @hack spelling: the record's size through size, set before offset,
     * where sizeof in both operations would do */
    size = sizeof(NJS_TEXMEMLIST);
    offset = (Uint32)tex - (Uint32)list;
    rest = offset % size;
    if (rest != 0) {
        return -1;
    }
    i = offset / size;
    if (i >= n) {
        return -1;
    }
    return i;
}

Sint32 njSearchTexList3(NJS_TEXMEMLIST *list, Uint32 n, Uint32 globalIndex,
                        Uint32 bank)
{
    Uint32 i;

    if (list == NULL) {
        return -1;
    }
    for (i = 0; i < n; i++) {
        /* @hack spelling: two tests, where one && would do */
        if (list[i].globalIndex == globalIndex) {
            if (list[i].bank == bank) {
                return i;
            }
        }
    }
    return -1;
}

void njSetFlagAddrAllTexture(Uint32 globalIndex, Uint32 flag, void *addr)
{
    Uint32 i;
    NJS_TEXMEMLIST *tex;

    for (i = 0; i < _nj_texmemsize; i++) {
        tex = &_nj_texmemlist[i];
        if (tex->globalIndex == globalIndex) {
            tex->texaddr |= flag;
            tex->texinfo.texaddr = addr;
            if (flag & TEXMEM_LOADED) {
                tex->count++;
                njSetTextureParam(tex);
            }
        }
    }
}

void njClearFlagAddrAllTexture(Uint32 globalIndex, Uint32 flag, void *addr)
{
    Uint32 i;
    NJS_TEXMEMLIST *tex;

    for (i = 0; i < _nj_texmemsize; i++) {
        tex = &_nj_texmemlist[i];
        if (tex->globalIndex == globalIndex) {
            tex->texaddr &= ~flag;
            if (!(tex->texaddr & TEXMEM_LOADED)) {
                tex->texinfo.texsurface.pSurface = NULL;
                tex->count = 0;
            }
            if (!(tex->texaddr & TEXMEM_CACHED)) {
                tex->texinfo.texaddr = NULL;
            }
            if (!(tex->texaddr & (TEXMEM_CACHED | TEXMEM_LOADED))) {
                njFreeTexMemList(tex);
            }
        }
    }
}

void njAddTextureCount(Uint32 globalIndex)
{
    Uint32 i;
    NJS_TEXMEMLIST *tex = _nj_texmemlist;

    for (i = 0; i < _nj_texmemsize; i++) {
        if (tex->globalIndex == globalIndex) {
            tex->count++;
        }
        tex++;
    }
}

NJS_TEXINFO *njTexMemList(Uint32 globalIndex)
{
    Uint32 i;

    if (_nj_texmemlist[next_free].globalIndex == TEXMEM_FREE) {
        i = next_free;
    } else {
        for (i = 0; i < _nj_texmemsize; i++) {
            if (_nj_texmemlist[i].globalIndex == TEXMEM_FREE) {
                break;
            }
        }
    }
    return &_nj_texmemlist[i].texinfo;
}
