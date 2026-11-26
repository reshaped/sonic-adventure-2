/* load one texture of a texture list into a texture record, from a PVR file
 * or from memory */
#include <nj_global.h>
#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>

/* the member exports these, though no header of the SDK declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 njLoadFileTexture(NJS_TEXLIST *texlist, Uint32 n);
Sint32 njLoadMemoryTexture(NJS_TEXLIST *texlist, Uint32 n);
/* njLoadTextureLN's two loads, indexed by whether the name has
 * NJD_TEXATTR_TYPE_MEMORY */
Sint32 (*_njloadtexture[])(NJS_TEXLIST *texlist,
                           Uint32 n) = {njLoadFileTexture, njLoadMemoryTexture};
/* NOLINTEND(misc-use-internal-linkage) */

#ifdef NONMATCHING
/* lower _nj_user_globalIndex to the highest index at or below it that no
 * record holds, and return that index, as njCheckFileInfo does for a file
 * without a GBIX chunk. this side's repairs give the index to a failed
 * load's record */
static Uint32 lower_user_global_index(void)
{
    while (njSearchTexList(_nj_texmemlist, _nj_texmemsize,
                           _nj_user_globalIndex) != -1) {
        _nj_user_globalIndex--;
    }
    return _nj_user_globalIndex;
}
#endif

Sint32 njLoadTextureLN(NJS_TEXLIST *texlist, Uint32 n)
{
    Bool in_memory;

    in_memory = FALSE;
    if (texlist->textures[n].attr & NJD_TEXATTR_TYPE_MEMORY) {
        in_memory = TRUE;
    }
    return _njloadtexture[in_memory](texlist, n);
}

Sint32 njLoadFileTexture(NJS_TEXLIST *texlist, Uint32 n)
{
    Uint32 *handle;
    NJS_TEXMEMLIST *tex;
    NJS_TEXNAME *texname;
    NJS_TEXNAME *previous;
    Sint32 result;
    char path[256];
    Sint32 index;
    /* @hack spelling: bank_index, which keeps each njSearchTexList3 result
     * only for its test, where the search tested in place would do */
    Sint32 bank_index;
    /* TRUE where the texture is palettized, else FALSE. it takes
     * njCheckFileInfo's result, so a failed check leaves -NJD_TEXERR_HEADER */
    Sint32 palettized;
    Uint32 globalIndex;
    Uint32 bank;
    Sint32 listing;
    /* @hack redundant: unused, two frame words that nothing reads or writes */
    Sint32 unused[2];
    NJS_TEXINFO info;
    NJS_TEXINFO *texinfo;

    palettized = FALSE;
    bank = NO_BANK;
#ifdef NONMATCHING
    /* this side repairs the tag at the first failed open: each failed
     * open's or check's record takes a null texaddr, as a record without a
     * cached copy holds */
    info.texaddr = NULL;
#endif
    texname = &texlist->textures[n];
    handle = &texname->texaddr;
    /* @hack spelling: the two tests nested, where one && would do */
    if (texname->attr & NJD_TEXATTR_TEXCONTINUE) {
        if (n > 0) {
            previous = &texlist->textures[n - 1];
            tex = (NJS_TEXMEMLIST *)previous->texaddr;
            bank = (texname->texaddr & NJD_TEXBANK_MASK) >> NJD_TEXBANK_SHIFT;
#ifdef NONMATCHING
            /* this side repairs the bug tagged below: the record takes the
             * global index of the record it shares */
            globalIndex = tex->globalIndex;
#endif
            /* @bug uninitialized: the record takes globalIndex, which
             * nothing on this path sets. 0x8c138c0e reads it for the
             * njEntryTexMemList call at 0x8c138c18 */
            /* NOLINTBEGIN(clang-diagnostic-uninitialized) */
            if (tex->texaddr & TEXMEM_CACHED) {
                njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                  TEXMEM_CACHED | TEXMEM_LOADED, bank);
            } else {
                njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                  TEXMEM_LOADED, bank);
            }
            /* NOLINTEND(clang-diagnostic-uninitialized) */
            RECORD(handle)->dummy = 0;
            /* @hack redundant: the second setting of the parameter words,
             * which njEntryTexMemList has set */
            njSetTextureParam(RECORD(handle));
            return 1;
        }
    }
    njCheckFileName(path, texname->filename, ".PVR");
    /* @hack spelling: the name read through texlist->textures[n] in both
     * tests and in globalIndex's read, where texname would do */
    if (texlist->textures[n].attr & NJD_TEXATTR_GLOBALINDEX) {
        globalIndex = texlist->textures[n].texaddr;
        index = njSearchTexList(_nj_texmemlist, _nj_texmemsize, globalIndex);
        if (index < 0) {
            listing = UNLISTED;
        } else {
            listing = LISTED;
        }
        result = njTexFopen(path);
        if (result < 0) {
            njMemCopy(&info.texsurface, &_nj_tex_err_surface,
                      sizeof(NJS_TEXSURFACE));
            /* @bug uninitialized: each failed open's or check's record takes
             * info's texaddr, which nothing sets. the njEntryTexMemList calls
             * at 0x8c138cfe, 0x8c138e20, 0x8c138e76 and 0x8c138ecc copy it */
            njEntryTexMemList(handle, &info, globalIndex, 0, NO_BANK);
            RECORD(handle)->dummy = -result;
            RECORD(handle)->count = 0;
            return -1;
        }
    } else if (texlist->textures[n].attr & NJD_TEXATTR_PALGLOBALINDEX) {
        globalIndex = texname->texaddr & NJD_TEXPALGLOBAL_MASK;
        bank = (texname->texaddr & NJD_TEXBANK_MASK) >> NJD_TEXBANK_SHIFT;
        index = njSearchTexList(_nj_texmemlist, _nj_texmemsize, globalIndex);
        if (index < 0) {
            listing = UNLISTED;
        } else if ((bank_index =
                        njSearchTexList3(_nj_texmemlist, _nj_texmemsize,
                                         globalIndex, bank)) < 0) {
            listing = OTHER_BANK;
        } else {
            listing = LISTED;
        }
        palettized = TRUE;
        result = njTexFopen(path);
        if (result < 0) {
            njMemCopy(&info.texsurface, &_nj_tex_err_surface,
                      sizeof(NJS_TEXSURFACE));
            njEntryTexMemList(handle, &info, globalIndex, 0, bank);
            RECORD(handle)->dummy = -result;
            RECORD(handle)->count = 0;
            return -1;
        }
    } else {
        result = njTexFopen(path);
        if (result < 0) {
            njMemCopy(&info.texsurface, &_nj_tex_err_surface,
                      sizeof(NJS_TEXSURFACE));
            /* @bug uninitialized: the record takes globalIndex, which
             * nothing on this path sets. 0x8c138e6c reads it for the
             * njEntryTexMemList call at 0x8c138e76 */
#ifdef NONMATCHING
            /* this side repairs the tag above: the record takes a user global
             * index that no record holds */
            globalIndex = lower_user_global_index();
#endif
            njEntryTexMemList(handle, &info, globalIndex, 0, NO_BANK);
            RECORD(handle)->dummy = -result;
            RECORD(handle)->count = 0;
            return -1;
        }
        palettized = njCheckFileInfo(&globalIndex);
        if (palettized < 0) {
            njMemCopy(&info.texsurface, &_nj_tex_err_surface,
                      sizeof(NJS_TEXSURFACE));
            /* @bug uninitialized: the record takes globalIndex, which a
             * failed njCheckFileInfo leaves unset. 0x8c138ec2 reads it for
             * the njEntryTexMemList call at 0x8c138ecc */
#ifdef NONMATCHING
            /* this side repairs the tag above: the record takes a user global
             * index that no record holds */
            globalIndex = lower_user_global_index();
#endif
            njEntryTexMemList(handle, &info, globalIndex, 0, NO_BANK);
            _nj_texerr_.texerr = -palettized;
            njTexFclose();
            return -1;
        }
        index = njSearchTexList(_nj_texmemlist, _nj_texmemsize, globalIndex);
        /* @hack spelling: palettized compared with TRUE, where palettized alone
         * would do */
        if (palettized == TRUE) {
            bank = (texname->texaddr & NJD_TEXBANK_MASK) >> NJD_TEXBANK_SHIFT;
            if (index < 0) {
                listing = UNLISTED;
            } else {
                bank_index = njSearchTexList3(_nj_texmemlist, _nj_texmemsize,
                                              globalIndex, bank);
                if (bank_index < 0) {
                    listing = OTHER_BANK;
                } else {
                    listing = LISTED;
                }
            }
        } else if (index < 0) {
            listing = UNLISTED;
        } else {
            listing = LISTED;
        }
    }
    switch (listing) {
    case OTHER_BANK:
        tex = &_nj_texmemlist[index];
        if (tex->texaddr & TEXMEM_LOADED) {
            switch (texname->attr & NJD_TEXATTR_READAREA_MASK) {
            case READAREA_TEXMEM:
                njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                  tex->texaddr, bank);
                RECORD(handle)->count = tex->count;
                RECORD(handle)->dummy = 0;
                njAddTextureCount(globalIndex);
                break;
            case NJD_TEXATTR_CACHE:
                if (!(tex->texaddr & TEXMEM_CACHED)) {
                    result = njLoadCacheFromFile(texname, &tex->texinfo);
                    /* @hack redundant: the else after the arm that returns */
                    if (result < 0) {
                        njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                          TEXMEM_LOADED, bank);
                        RECORD(handle)->count = tex->count;
                        RECORD(handle)->dummy = -result;
                        _nj_texerr_.n = n;
                        _nj_texerr_.globalIndex = globalIndex;
                        njTexFclose();
                        return -1;
                    } else {
                        njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                          TEXMEM_LOADED, bank);
                        RECORD(handle)->count = tex->count;
                        RECORD(handle)->dummy = 0;
                        /* @hack spelling: texinfo.texaddr read through the new
                         * record, where tex's, its equal, would do */
                        njSetFlagAddrAllTexture(
                            globalIndex, TEXMEM_CACHED,
                            RECORD(handle)->texinfo.texaddr);
                    }
                } else {
                    njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                      TEXMEM_CACHED | TEXMEM_LOADED, bank);
                    RECORD(handle)->count = tex->count;
                    RECORD(handle)->dummy = 0;
                }
                break;
            case NJD_TEXATTR_BOTH:
                if (!(tex->texaddr & TEXMEM_CACHED)) {
                    result = njLoadCacheFromFile(texname, &tex->texinfo);
                    /* @hack redundant: the else after the arm that returns */
                    if (result < 0) {
                        njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                          TEXMEM_LOADED, bank);
                        RECORD(handle)->count = tex->count;
                        RECORD(handle)->dummy = -result;
                        _nj_texerr_.n = n;
                        _nj_texerr_.globalIndex = globalIndex;
                        njTexFclose();
                        return -1;
                    } else {
                        njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                          TEXMEM_LOADED, bank);
                        RECORD(handle)->count = tex->count;
                        RECORD(handle)->dummy = 0;
                        /* @hack spelling: texinfo.texaddr read through the new
                         * record, where tex's, its equal, would do */
                        njSetFlagAddrAllTexture(
                            globalIndex, TEXMEM_CACHED | TEXMEM_LOADED,
                            RECORD(handle)->texinfo.texaddr);
                    }
                } else {
                    njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                      TEXMEM_CACHED | TEXMEM_LOADED, bank);
                    RECORD(handle)->count = tex->count;
                    RECORD(handle)->dummy = 0;
                    njAddTextureCount(globalIndex);
                }
                break;
            }
        } else {
            switch (texname->attr & NJD_TEXATTR_READAREA_MASK) {
            case READAREA_TEXMEM:
            case NJD_TEXATTR_BOTH:
                result = njLoadTexMemFromMemory(texname, &tex->texinfo);
                /* @hack redundant: the else after the arm that returns */
                if (result < 0) {
                    njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                      TEXMEM_CACHED, bank);
                    RECORD(handle)->dummy = -result;
                    _nj_texerr_.n = n;
                    _nj_texerr_.globalIndex = globalIndex;
                    njTexFclose();
                    return -1;
                } else {
                    njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                      TEXMEM_CACHED, bank);
                    RECORD(handle)->dummy = 0;
                    /* @hack spelling: texinfo.texaddr read through the new
                     * record, where tex's, its equal, would do */
                    njSetFlagAddrAllTexture(globalIndex, TEXMEM_LOADED,
                                            RECORD(handle)->texinfo.texaddr);
                    tex->texaddr |= TEXMEM_SHARED_SURFACE;
                }
                break;
            case NJD_TEXATTR_CACHE:
                njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                  TEXMEM_CACHED, bank);
                RECORD(handle)->dummy = 0;
                break;
            }
        }
        njTexFclose();
        break;
    case LISTED:
        /* @bug logic: a palettized name takes the first record of its global
         * index, which may be another bank's. bank_index holds the record of
         * the name's own bank. 0x8c1393a4 reads index, not bank_index */
        tex = &_nj_texmemlist[index];
        texname->texaddr = (Uint32)tex;
        switch (texname->attr & NJD_TEXATTR_READAREA_MASK) {
        case READAREA_TEXMEM:
            if (tex->texaddr & TEXMEM_LOADED) {
                if (palettized) {
                    njAddTextureCount(globalIndex);
                } else {
                    tex->count++;
                }
            } else {
                result = njLoadTexMemFromMemory(texname, &tex->texinfo);
                /* @hack redundant: the else after the arm that returns */
                if (result < 0) {
                    tex->dummy = -result;
                    _nj_texerr_.n = n;
                    _nj_texerr_.globalIndex = globalIndex;
                    njTexFclose();
                    return -1;
                } else {
                    tex->dummy = 0;
                    if (palettized) {
                        njSetFlagAddrAllTexture(globalIndex, TEXMEM_LOADED,
                                                tex->texinfo.texaddr);
                        tex->texaddr |= TEXMEM_SHARED_SURFACE;
                    } else {
                        njSetTextureParam(tex);
                        tex->texaddr |= TEXMEM_LOADED;
                        tex->count++;
                    }
                }
            }
            break;
        case NJD_TEXATTR_CACHE:
            if (!(tex->texaddr & TEXMEM_CACHED)) {
                result = njLoadCacheFromFile(texname, &tex->texinfo);
                /* @hack redundant: the else after the arm that returns */
                if (result < 0) {
                    tex->dummy = -result;
                    _nj_texerr_.n = n;
                    _nj_texerr_.globalIndex = globalIndex;
                    njTexFclose();
                    return -1;
                } else {
                    tex->dummy = 0;
                    if (palettized) {
                        njSetFlagAddrAllTexture(globalIndex, TEXMEM_CACHED,
                                                tex->texinfo.texaddr);
                    } else {
                        tex->texaddr |= TEXMEM_CACHED;
                    }
                }
            }
            break;
        case NJD_TEXATTR_BOTH:
            if (tex->texaddr & TEXMEM_LOADED) {
                if (!(tex->texaddr & TEXMEM_CACHED)) {
                    result = njLoadCacheFromFile(texname, &tex->texinfo);
                    /* @hack redundant: the else after the arm that returns */
                    if (result < 0) {
                        tex->dummy = -result;
                        _nj_texerr_.n = n;
                        _nj_texerr_.globalIndex = globalIndex;
                        njTexFclose();
                        return -1;
                    } else {
                        tex->dummy = 0;
                        if (palettized) {
                            njSetFlagAddrAllTexture(
                                globalIndex, TEXMEM_CACHED | TEXMEM_LOADED,
                                tex->texinfo.texaddr);
                        } else {
                            tex->texaddr |= TEXMEM_CACHED;
                            tex->count++;
                        }
                    }
                } else if (palettized) {
                    njAddTextureCount(globalIndex);
                } else {
                    tex->count++;
                }
            } else {
                result = njLoadTexMemFromMemory(texname, &tex->texinfo);
                /* @hack redundant: the else after the arm that returns */
                if (result < 0) {
                    tex->dummy = -result;
                    _nj_texerr_.n = n;
                    _nj_texerr_.globalIndex = globalIndex;
                    njTexFclose();
                    return -1;
                } else {
                    tex->dummy = 0;
                    if (palettized) {
                        njSetFlagAddrAllTexture(globalIndex, TEXMEM_LOADED,
                                                tex->texinfo.texaddr);
                        tex->texaddr |= TEXMEM_SHARED_SURFACE;
                    } else {
                        njSetTextureParam(tex);
                        tex->texaddr |= TEXMEM_LOADED;
                        tex->count = 1;
                    }
                }
            }
        }
        njTexFclose();
        break;
    case UNLISTED:
        texinfo = njTexMemList(globalIndex);
        switch (texname->attr & NJD_TEXATTR_READAREA_MASK) {
        case READAREA_TEXMEM:
            result = njLoadTexMemFromFile(texname, texinfo);
            /* @hack redundant: the else after the arm that returns */
            if (result < 0) {
                njEntryTexMemList(handle, texinfo, globalIndex, 0, bank);
                RECORD(handle)->dummy = -result;
                RECORD(handle)->count = 0;
                _nj_texerr_.n = n;
                _nj_texerr_.globalIndex = globalIndex;
                njTexFclose();
                return -1;
            } else {
                njEntryTexMemList(handle, texinfo, globalIndex, TEXMEM_LOADED,
                                  bank);
                RECORD(handle)->dummy = 0;
                if (palettized) {
                    RECORD(handle)->texaddr |= TEXMEM_SHARED_SURFACE;
                }
            }
            break;
        case NJD_TEXATTR_CACHE:
            result = njLoadCacheFromFile(texname, texinfo);
            /* @hack redundant: the else after the arm that returns */
            if (result < 0) {
                njEntryTexMemList(handle, texinfo, globalIndex, 0, bank);
                RECORD(handle)->dummy = -result;
                _nj_texerr_.n = n;
                _nj_texerr_.globalIndex = globalIndex;
                njTexFclose();
                return -1;
            } else {
                njEntryTexMemList(handle, texinfo, globalIndex, TEXMEM_CACHED,
                                  bank);
                RECORD(handle)->dummy = 0;
            }
            break;
        case NJD_TEXATTR_BOTH:
            result = njLoadCacheFromFile(texname, texinfo);
            if (result < 0) {
                /* @bug uninitialized: the record takes info, which nothing on
                 * this path sets. every other failed load passes texinfo.
                 * 0x8c13986a points R5 at info for the njEntryTexMemList call
                 * at 0x8c13986c */
#ifdef NONMATCHING
                /* this side repairs the tag above: the record takes texinfo,
                 * as every other failed load's does */
                njEntryTexMemList(handle, texinfo, globalIndex, 0, bank);
#else
                njEntryTexMemList(handle, &info, globalIndex, 0, bank);
#endif
                RECORD(handle)->dummy = -result;
                RECORD(handle)->count = 0;
                _nj_texerr_.n = n;
                _nj_texerr_.globalIndex = globalIndex;
                njTexFclose();
                return -1;
            }
            result = njLoadTexMemFromMemory(texname, texinfo);
            if (result < 0) {
                njEntryTexMemList(handle, texinfo, globalIndex, TEXMEM_CACHED,
                                  bank);
                RECORD(handle)->dummy = -result;
                _nj_texerr_.n = n;
                _nj_texerr_.globalIndex = globalIndex;
                njTexFclose();
                return -1;
            }
            njEntryTexMemList(handle, texinfo, globalIndex,
                              TEXMEM_CACHED | TEXMEM_LOADED, bank);
            RECORD(handle)->dummy = 0;
            if (palettized) {
                RECORD(handle)->texaddr |= TEXMEM_SHARED_SURFACE;
            }
            break;
        }
        njTexFclose();
        break;
    }
    return 1;
}

Sint32 njLoadMemoryTexture(NJS_TEXLIST *texlist, Uint32 n)
{
    Uint32 *handle;
    NJS_TEXMEMLIST *tex;
    NJS_TEXNAME *texname;
    NJS_TEXNAME *previous;
    NJS_TEXINFO *info;
    NJS_TEXINFO *texinfo;
    Uint32 *pixels;
    NJS_PVRHEADERDATA pvrh;
    Uint32 gbix_index;
    Sint32 result;
    Sint32 index;
    /* @hack spelling: bank_index, which keeps each njSearchTexList3 result
     * only for its test, where the search tested in place would do */
    Sint32 bank_index;
    Sint32 palettized;
    Uint32 globalIndex;
    Uint32 bank;
    Sint32 listing;

    bank = NO_BANK;
    texname = &texlist->textures[n];
    handle = &texname->texaddr;
    /* @hack spelling: the two tests nested, where one && would do */
    if (texname->attr & NJD_TEXATTR_TEXCONTINUE) {
        if (n > 0) {
            previous = &texlist->textures[n - 1];
            tex = (NJS_TEXMEMLIST *)previous->texaddr;
            bank = (texname->texaddr & NJD_TEXBANK_MASK) >> NJD_TEXBANK_SHIFT;
#ifdef NONMATCHING
            /* this side repairs the bug tagged below: the record takes the
             * global index of the record it shares */
            globalIndex = tex->globalIndex;
#endif
            /* @bug uninitialized: the record takes globalIndex, which
             * nothing on this path sets. 0x8c1399d6 reads it for the
             * njEntryTexMemList call at 0x8c1399da */
            /* NOLINTBEGIN(clang-diagnostic-uninitialized) */
            if (tex->texaddr & TEXMEM_CACHED) {
                njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                  TEXMEM_CACHED | TEXMEM_LOADED, bank);
            } else {
                njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                  TEXMEM_LOADED, bank);
            }
            /* NOLINTEND(clang-diagnostic-uninitialized) */
            RECORD(handle)->dummy = 0;
            /* @hack redundant: the second setting of the parameter words,
             * which njEntryTexMemList has set */
            njSetTextureParam(RECORD(handle));
            return 1;
        }
    }
    /* @hack spelling: the second test's read of the name through
     * texlist->textures[n], where texname would do */
    if (texname->attr & NJD_TEXATTR_GLOBALINDEX) {
        globalIndex = texname->texaddr;
        info = (NJS_TEXINFO *)texname->filename;
        pixels = njLoadPvrHeader(&pvrh, &gbix_index, info->texaddr);
        if (pixels) {
            info->texaddr = pixels;
            info->texsurface.nWidth = pvrh.nWidth;
            info->texsurface.nHeight = pvrh.nHeight;
            info->texsurface.Type = pvrh.nTextureType
                                    << TEXSURFACE_FORMAT_SHIFT;
        }
        index = njSearchTexList(_nj_texmemlist, _nj_texmemsize, globalIndex);
        if (index < 0) {
            listing = UNLISTED;
        } else {
            listing = LISTED;
        }
        palettized = FALSE;
    } else if (texlist->textures[n].attr & NJD_TEXATTR_PALGLOBALINDEX) {
        globalIndex = texname->texaddr & NJD_TEXPALGLOBAL_MASK;
        bank = (texname->texaddr & NJD_TEXBANK_MASK) >> NJD_TEXBANK_SHIFT;
        index = njSearchTexList(_nj_texmemlist, _nj_texmemsize, globalIndex);
        info = (NJS_TEXINFO *)texname->filename;
        pixels = njLoadPvrHeader(&pvrh, &gbix_index, info->texaddr);
        if (pixels) {
            info->texaddr = pixels;
            info->texsurface.nWidth = pvrh.nWidth;
            info->texsurface.nHeight = pvrh.nHeight;
            info->texsurface.Type = pvrh.nTextureType
                                    << TEXSURFACE_FORMAT_SHIFT;
        }
        if (index < 0) {
            listing = UNLISTED;
        } else {
            bank_index = njSearchTexList3(_nj_texmemlist, _nj_texmemsize,
                                          globalIndex, bank);
            if (bank_index < 0) {
                listing = OTHER_BANK;
            } else {
                listing = LISTED;
            }
        }
        palettized = TRUE;
    } else {
        info = (NJS_TEXINFO *)texname->filename;
        pixels = njLoadPvrHeader(&pvrh, &globalIndex, info->texaddr);
        if (pixels) {
            info->texaddr = pixels;
            info->texsurface.nWidth = pvrh.nWidth;
            info->texsurface.nHeight = pvrh.nHeight;
            info->texsurface.Type = pvrh.nTextureType
                                    << TEXSURFACE_FORMAT_SHIFT;
        }
        /* @bug uninitialized: the test for a palettized type reads
         * pvrh.nTextureType, which a null return from njLoadPvrHeader leaves
         * unset. 0x8c139b4a and 0x8c139b66 read it for njCheckPaletteType */
#ifdef NONMATCHING
        /* this side repairs the tag above: where no header was read, the test
         * takes the type the name's info holds */
        if (pixels == NULL) {
            pvrh.nTextureType =
                info->texsurface.Type >> TEXSURFACE_FORMAT_SHIFT;
        }
#endif
        if (globalIndex == TEXMEM_FREE) {
            /* @hack spelling: index, which the search stores and nothing
             * reads, where the search tested alone would do */
            while ((index = njSearchTexList(_nj_texmemlist, _nj_texmemsize,
                                            _nj_user_globalIndex)) != -1) {
                _nj_user_globalIndex--;
            }
            globalIndex = _nj_user_globalIndex;
            palettized =
                njCheckPaletteType(pvrh.nTextureType & NJD_TEXFMT_TYPE_MASK);
            /* @hack spelling: palettized compared with TRUE, where palettized
             * alone would do */
            if (palettized == TRUE) {
                bank =
                    (texname->texaddr & NJD_TEXBANK_MASK) >> NJD_TEXBANK_SHIFT;
            }
            listing = UNLISTED;
        } else {
            palettized =
                njCheckPaletteType(pvrh.nTextureType & NJD_TEXFMT_TYPE_MASK);
            index =
                njSearchTexList(_nj_texmemlist, _nj_texmemsize, globalIndex);
            /* @hack spelling: palettized compared with TRUE, where palettized
             * alone would do */
            if (palettized == TRUE) {
                bank =
                    (texname->texaddr & NJD_TEXBANK_MASK) >> NJD_TEXBANK_SHIFT;
                if (index < 0) {
                    listing = UNLISTED;
                } else {
                    bank_index = njSearchTexList3(
                        _nj_texmemlist, _nj_texmemsize, globalIndex, bank);
                    if (bank_index < 0) {
                        listing = OTHER_BANK;
                    } else {
                        listing = LISTED;
                    }
                }
            } else {
                /* @hack redundant: this test for TEXMEM_FREE, which never
                 * holds in the else of the same test */
                if (globalIndex == TEXMEM_FREE) {
                    njEntryTexMemList(handle, info, globalIndex, 0, NO_BANK);
                    RECORD(handle)->dummy = NJD_TEXERR_GLOBALINDEX;
                    RECORD(handle)->count = 0;
                    _nj_texerr_.texerr = NJD_TEXERR_GLOBALINDEX;
                    return -1;
                }
                if (index < 0) {
                    listing = UNLISTED;
                } else {
                    listing = LISTED;
                }
            }
        }
    }
    switch (listing) {
    case OTHER_BANK:
        tex = &_nj_texmemlist[index];
        if (tex->texaddr & TEXMEM_LOADED) {
            switch (texname->attr & NJD_TEXATTR_READAREA_MASK) {
            case READAREA_TEXMEM:
                njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                  tex->texaddr, bank);
                RECORD(handle)->count = tex->count;
                RECORD(handle)->dummy = 0;
                njAddTextureCount(globalIndex);
                break;
            case NJD_TEXATTR_CACHE:
                if (!(tex->texaddr & TEXMEM_CACHED)) {
                    /* @bug memory: reads the texels through the record's
                     * texinfo.texaddr, which is null where a file loaded the
                     * record into texture memory alone. the tests at
                     * 0x8c139c42, 0x8c139d0a, 0x8c139e2c and 0x8c139e62 pass
                     * only a record without a cached copy to the
                     * njLoadCacheFromMemory calls at 0x8c139c4c, 0x8c139d14,
                     * 0x8c139e3a and 0x8c139e6c */
                    /* @hack spelling: the assignment inside the test, where a
                     * statement of its own would do */
                    if ((result = njLoadCacheFromMemory(texname,
                                                        &tex->texinfo)) < 0) {
                        njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                          TEXMEM_LOADED, bank);
                        RECORD(handle)->count = tex->count;
                        RECORD(handle)->dummy = -result;
                        /* @hack redundant: the clear of
                         * TEXMEM_SHARED_SURFACE, which njEntryTexMemList has
                         * left clear */
                        RECORD(handle)->texaddr &= ~TEXMEM_SHARED_SURFACE;
                        _nj_texerr_.n = n;
                        _nj_texerr_.globalIndex = globalIndex;
                        return -1;
                    }
                    njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                      TEXMEM_LOADED, bank);
                    RECORD(handle)->count = tex->count;
                    RECORD(handle)->dummy = 0;
                    /* @hack redundant: the clear of TEXMEM_SHARED_SURFACE,
                     * which njEntryTexMemList has left clear */
                    RECORD(handle)->texaddr &= ~TEXMEM_SHARED_SURFACE;
                    njSetFlagAddrAllTexture(globalIndex, TEXMEM_CACHED,
                                            tex->texinfo.texaddr);
                } else {
                    njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                      TEXMEM_CACHED | TEXMEM_LOADED, bank);
                    RECORD(handle)->count = tex->count;
                    RECORD(handle)->dummy = 0;
                }
                break;
            case NJD_TEXATTR_BOTH:
                if (!(tex->texaddr & TEXMEM_CACHED)) {
                    result = njLoadCacheFromMemory(texname, &tex->texinfo);
                    if (result < 0) {
                        njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                          TEXMEM_LOADED, bank);
                        RECORD(handle)->count = tex->count;
                        RECORD(handle)->dummy = -result;
                        _nj_texerr_.n = n;
                        _nj_texerr_.globalIndex = globalIndex;
                        return -1;
                    }
                    njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                      TEXMEM_LOADED, bank);
                    RECORD(handle)->count = tex->count;
                    RECORD(handle)->dummy = 0;
                    njSetFlagAddrAllTexture(globalIndex,
                                            TEXMEM_CACHED | TEXMEM_LOADED,
                                            tex->texinfo.texaddr);
                } else {
                    njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                      TEXMEM_CACHED | TEXMEM_LOADED, bank);
                    RECORD(handle)->count = tex->count;
                    RECORD(handle)->dummy = 0;
                    njAddTextureCount(globalIndex);
                }
                break;
            }
        } else {
            switch (texname->attr & NJD_TEXATTR_READAREA_MASK) {
            case READAREA_TEXMEM:
            case NJD_TEXATTR_BOTH:
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((result = njLoadTexMemFromMemory(texname, &tex->texinfo)) <
                    0) {
                    njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                      TEXMEM_CACHED, bank);
                    RECORD(handle)->dummy = -result;
                    _nj_texerr_.n = n;
                    _nj_texerr_.globalIndex = globalIndex;
                    return -1;
                }
                njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                  TEXMEM_CACHED, bank);
                RECORD(handle)->dummy = 0;
                njSetFlagAddrAllTexture(globalIndex, TEXMEM_LOADED,
                                        tex->texinfo.texaddr);
                tex->texaddr |= TEXMEM_SHARED_SURFACE;
                break;
            case NJD_TEXATTR_CACHE:
                njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                  TEXMEM_CACHED, bank);
                RECORD(handle)->dummy = 0;
                break;
            }
        }
        break;
    case LISTED:
        /* @bug logic: a palettized name takes the first record of its global
         * index, as in njLoadFileTexture's LISTED case. 0x8c139be8 reads
         * index, not bank_index, for the store at 0x8c139de0 */
        tex = &_nj_texmemlist[index];
        texname->texaddr = (Uint32)tex;
        switch (texname->attr & NJD_TEXATTR_READAREA_MASK) {
        case READAREA_TEXMEM:
            if (tex->texaddr & TEXMEM_LOADED) {
                if (palettized) {
                    njAddTextureCount(globalIndex);
                } else {
                    tex->count++;
                }
            } else {
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((result = njLoadTexMemFromMemory(texname, &tex->texinfo)) <
                    0) {
                    tex->dummy = -result;
                    _nj_texerr_.n = n;
                    _nj_texerr_.globalIndex = globalIndex;
                    return -1;
                }
                tex->dummy = 0;
                if (palettized) {
                    njSetFlagAddrAllTexture(globalIndex, TEXMEM_LOADED,
                                            tex->texinfo.texaddr);
                    tex->texaddr |= TEXMEM_SHARED_SURFACE;
                } else {
                    njSetTextureParam(tex);
                    tex->texaddr |= TEXMEM_LOADED;
                    tex->count++;
                }
            }
            break;
        case NJD_TEXATTR_CACHE:
            if (!(tex->texaddr & TEXMEM_CACHED)) {
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((result = njLoadCacheFromMemory(texname, &tex->texinfo)) <
                    0) {
                    tex->dummy = -result;
                    _nj_texerr_.n = n;
                    _nj_texerr_.globalIndex = globalIndex;
                    return -1;
                }
                tex->dummy = 0;
                if (palettized) {
                    njSetFlagAddrAllTexture(globalIndex, TEXMEM_CACHED,
                                            tex->texinfo.texaddr);
                } else {
                    tex->texaddr |= TEXMEM_CACHED;
                }
            }
            break;
        case NJD_TEXATTR_BOTH:
            if (tex->texaddr & TEXMEM_LOADED) {
                if (!(tex->texaddr & TEXMEM_CACHED)) {
                    /* @hack spelling: the assignment inside the test, where a
                     * statement of its own would do */
                    if ((result = njLoadCacheFromMemory(texname,
                                                        &tex->texinfo)) < 0) {
                        tex->dummy = -result;
                        _nj_texerr_.n = n;
                        _nj_texerr_.globalIndex = globalIndex;
                        return -1;
                    }
                    tex->dummy = 0;
                    if (palettized) {
                        njSetFlagAddrAllTexture(globalIndex,
                                                TEXMEM_CACHED | TEXMEM_LOADED,
                                                tex->texinfo.texaddr);
                    } else {
                        tex->texaddr |= TEXMEM_CACHED;
                        tex->count++;
                    }
                } else if (palettized) {
                    njAddTextureCount(globalIndex);
                } else {
                    tex->count++;
                }
            } else {
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((result = njLoadTexMemFromMemory(texname, &tex->texinfo)) <
                    0) {
                    tex->dummy = -result;
                    _nj_texerr_.n = n;
                    _nj_texerr_.globalIndex = globalIndex;
                    return -1;
                }
                tex->dummy = 0;
                if (palettized) {
                    njSetFlagAddrAllTexture(globalIndex, TEXMEM_LOADED,
                                            tex->texinfo.texaddr);
                    tex->texaddr |= TEXMEM_SHARED_SURFACE;
                } else {
                    njSetTextureParam(tex);
                    tex->texaddr |= TEXMEM_LOADED;
                    tex->count++;
                }
            }
        }
        break;
    case UNLISTED:
        texinfo = njTexMemList(globalIndex);
        njMemCopy(texinfo, info, sizeof(NJS_TEXINFO));
        switch (texname->attr & NJD_TEXATTR_READAREA_MASK) {
        case READAREA_TEXMEM:
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((result = njLoadTexMemFromMemory(texname, texinfo)) < 0) {
                njEntryTexMemList(handle, texinfo, globalIndex, 0, bank);
                RECORD(handle)->dummy = -result;
                _nj_texerr_.n = n;
                _nj_texerr_.globalIndex = globalIndex;
                return -1;
            }
            njEntryTexMemList(handle, texinfo, globalIndex, TEXMEM_LOADED,
                              bank);
            RECORD(handle)->dummy = 0;
            if (palettized) {
                RECORD(handle)->texaddr |= TEXMEM_SHARED_SURFACE;
            }
            break;
        case NJD_TEXATTR_CACHE:
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((result = njLoadCacheFromMemory(texname, texinfo)) < 0) {
                njEntryTexMemList(handle, texinfo, globalIndex, 0, bank);
                RECORD(handle)->dummy = -result;
                _nj_texerr_.n = n;
                _nj_texerr_.globalIndex = globalIndex;
                return -1;
            }
            njEntryTexMemList(handle, texinfo, globalIndex, TEXMEM_CACHED,
                              bank);
            RECORD(handle)->dummy = 0;
            break;
        case NJD_TEXATTR_BOTH:
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((result = njLoadCacheFromMemory(texname, texinfo)) < 0) {
                njEntryTexMemList(handle, texinfo, globalIndex, 0, bank);
                RECORD(handle)->dummy = -result;
                _nj_texerr_.n = n;
                _nj_texerr_.globalIndex = globalIndex;
                return -1;
            }
            /* @bug memory: leaks the texture memory of the surface that
             * njLoadCacheFromMemory creates at 0x8c139f9a.
             * njLoadTexMemFromMemory at 0x8c139fb8 writes a second surface
             * over the first in texinfo. the file load's NJD_TEXATTR_BOTH
             * case creates one surface, at 0x8c1398ae */
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((result = njLoadTexMemFromMemory(texname, texinfo)) < 0) {
                njEntryTexMemList(handle, texinfo, globalIndex, TEXMEM_CACHED,
                                  bank);
                RECORD(handle)->dummy = -result;
                _nj_texerr_.n = n;
                _nj_texerr_.globalIndex = globalIndex;
                return -1;
            }
            njEntryTexMemList(handle, texinfo, globalIndex,
                              TEXMEM_CACHED | TEXMEM_LOADED, bank);
            RECORD(handle)->dummy = 0;
            if (palettized) {
                RECORD(handle)->texaddr |= TEXMEM_SHARED_SURFACE;
            }
        }
        break;
    }
    return 1;
}
