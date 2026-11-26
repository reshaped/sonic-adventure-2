/* load a texture list's names and textures from a PVM archive, in a file or in
 * memory. the archive opens with a PVMH chunk, which holds an entry for each
 * texture. the textures' PVRT chunks follow in the order of their numbers */
#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

/* the archive's chunk names, as nj_texlist.h's iff_ names are. each is its
 * four ASCII bytes as a little-endian word. njSearchPvrChunk and
 * njSearchPvrChunkM skip IMGC, PVMI, COMM, MDLN and CONV chunks as they skip
 * any other */
#define iff_PVMH 0x484d5650
#define iff_IMGC 0x43474d49
#define iff_PVMI 0x494d5650
#define iff_COMM 0x4d4d4f43
#define iff_MDLN 0x4e4c444d
#define iff_CONV 0x564e4f43
#define CHUNK_NAME 4 /* the bytes of a chunk's name */
#define CHUNK_HEAD 8 /* the bytes of a chunk's name and size */

/* a PVMH chunk after its name, before its count entries */
struct pvmh_header {
    Uint32 size; /* the bytes after this word */
    Uint16 status;
    Sint16 count;
};
/* an entry is its texture's number, then a field for each of these bits that
 * status sets. the fields follow in the bits' order */
#define PVMH_NAME 0x0001 /* the texture's name */
#define PVMH_CATEGORYCODE 0x0002 /* a field the loads skip */
#define PVMH_ENTRYINFO 0x0004 /* a field the loads skip */
#define PVMH_GLOBALINDEX 0x0008 /* the texture's global index */
/* each global index holds a palette bank too, as a name's texaddr does under
 * NJD_TEXATTR_PALGLOBALINDEX. the bit adds no field */
#define PVMH_PALGLOBALINDEX 0x0400
/* the bytes of an entry's fields. its name takes PVM_NAME_SIZE */
#define NUMBER_SIZE 2
#define CATEGORYCODE_SIZE 2
#define ENTRYINFO_SIZE 2
#define GLOBALINDEX_SIZE 4
/* the bytes of an entry with every field */
#define ENTRY_ROOM                                                             \
    (NUMBER_SIZE + PVM_NAME_SIZE + CATEGORYCODE_SIZE + ENTRYINFO_SIZE +        \
     GLOBALINDEX_SIZE)
/* the bits of a name's attr that hold its entry's number */
#define NUMBER_MASK 0x0000ffff
/* the palette's colors, and room for them at 32 bits each */
#define PALETTE_COLORS 1024
#define PALETTE_ROOM (PALETTE_COLORS * sizeof(Uint32))
/* the colors of a palette bank */
#define BANK_COLORS 16

/* the member exports these, though no header of the SDK declares them. a
 * function whose name ends in M reads the archive in memory that njMemOpen
 * opened. its namesake without the M reads the file njTexFopen opened */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* open the archive at addr, and start njMemRead's reads at its first byte */
void njMemOpen(Uint8 *addr);
/* copy size bytes into buf from the next byte to read, and move past them.
 * size must be above 0, since njMemCopy copies 2^32 bytes for a size of 0 */
void njMemRead(void *buf, Sint32 size);
/* move the next byte to read, by type. GDD_SEEK_SET puts it offset past the
 * archive's start, and GDD_SEEK_CUR moves it by offset. another type leaves
 * it */
void njMemFseek(Sint32 offset, Sint32 type);
/* get_w copies the halfword at src to dst, and get_l the word. each copies a
 * byte at a time, so either address may be unaligned */
void get_w(void *dst, void *src);
void get_l(void *dst, void *src);
/* the bytes of an entry with the fields status sets */
Sint32 njCalcPvmStatusSize(Uint16 status);
/* read pvmh's entries, size bytes each, from the open file into texlist's
 * first names, and return 1. name i takes entry i. its attr takes the entry's
 * number in the NUMBER_MASK bits. under PVMH_NAME, its filename takes the
 * entry's name but for the NUL. under PVMH_GLOBALINDEX, its texaddr takes the
 * entry's global index. PVMH_PALGLOBALINDEX then adds
 * NJD_TEXATTR_PALGLOBALINDEX to its attr. the call returns -1 where a read
 * fails. the names read before then keep their entries */
Sint32 njSetTexListToPvm(NJS_TEXLIST *texlist, Sint32 size,
                         struct pvmh_header *pvmh);
/* read the open file's chunks up to its next PVRT chunk, and return 1. the file
 * is left at that chunk's name. a PVPL chunk on the way sets the palette's
 * mode. the chunk's colors then fill the palette from entry offset + bank *
 * BANK_COLORS on. they must fit PALETTE_ROOM bytes. any other chunk is skipped
 * by its size. the call returns -1 where a read fails */
Sint32 njSearchPvrChunk(void);
/* the index in texlist of the first name whose attr's NUMBER_MASK bits are
 * number. some name must hold number: the search stops only at a match */
Sint32 njSearchEntryCount(NJS_TEXLIST *texlist, Uint32 number);
/* read the header of the PVRT chunk at the open file's next byte. the file is
 * left at the chunk's name. the call returns TRUE where the texture is
 * palettized, else FALSE. it returns -NJD_TEXERR_HEADER where a read fails or
 * the chunk isn't PVRT. the file is then left past what the call read */
Sint32 njCheckFileInfoPVM(void);
/* njSetTexListToPvm in the archive in memory, with no result */
void njSetTexListToPvmM(NJS_TEXLIST *texlist, Sint32 size,
                        struct pvmh_header *pvmh);
/* njSearchPvrChunk in the archive in memory, with no result. it sizes a PVPL
 * chunk's colors by the chunk's bank, a defect its bug tag gives */
void njSearchPvrChunkM(void);
/* read the chunks of the archive in memory through the end of its next PVRT
 * chunk. the call describes that chunk's texture in info, as njSetTextureInfo
 * does. texaddr points at the texels in the archive. the rest of info is left
 * as it was */
void njSetInfo(NJS_TEXINFO *info);
/* njCheckFileInfoPVM in the archive in memory, where no read fails. a chunk
 * other than PVRT leaves the archive past what it read, the chunk's name */
Sint32 njCheckFileInfoPVMM(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* the archive njMemOpen opened: its first byte, and the next byte to read */
static Uint8 *mem_start;
static Uint8 *mem_ptr;

void njMemOpen(Uint8 *addr)
{
    mem_start = addr;
    /* @hack spelling: mem_ptr set from mem_start, where addr would do */
    mem_ptr = mem_start;
}

void njMemRead(void *buf, Sint32 size)
{
    njMemCopy(buf, mem_ptr, size);
    mem_ptr += size;
}

void njMemFseek(Sint32 offset, Sint32 type)
{
    switch (type) {
    case GDD_SEEK_CUR:
        mem_ptr += offset;
        break;
    case GDD_SEEK_SET:
        mem_ptr = mem_start + offset;
        break;
    }
}

void get_w(void *dst, void *src)
{
    Uint8 *d = dst;
    Uint8 *s = src;

    *d++ = *s++;
    *d = *s;
}

void get_l(void *dst, void *src)
{
    Uint8 *d = dst;
    Uint8 *s = src;

    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    *d = *s;
}

Sint32 njCalcPvmStatusSize(Uint16 status)
{
    Sint32 size = NUMBER_SIZE;

    if (status & PVMH_NAME) {
        size = NUMBER_SIZE + PVM_NAME_SIZE;
    }
    if (status & PVMH_CATEGORYCODE) {
        size += CATEGORYCODE_SIZE;
    }
    if (status & PVMH_ENTRYINFO) {
        size += ENTRYINFO_SIZE;
    }
    if (status & PVMH_GLOBALINDEX) {
        size += GLOBALINDEX_SIZE;
    }
    return size;
}

Sint32 njSetTexListToPvm(NJS_TEXLIST *texlist, Sint32 size,
                         struct pvmh_header *pvmh)
{
    Uint16 number;
    Sint16 count;
    Uint16 status;
    char entry[ENTRY_ROOM];
    NJS_TEXNAME *texname;
    Sint32 i;
    Sint32 offset;
    Sint32 length;

    status = pvmh->status;
    count = pvmh->count;
    for (i = 0; i < count; i++) {
        if (njTexFread(entry, size) < 0) {
            return -1;
        }
        texname = &texlist->textures[i];
        get_w(&number, entry);
        texname->attr = (texname->attr & ~NUMBER_MASK) | number;
        offset = NUMBER_SIZE;
        if (status & PVMH_NAME) {
            length = strlen(entry + NUMBER_SIZE);
            /* njMemCopy copies 2^32 bytes for a length of 0 */
            if (length > 0) {
                njMemCopy(texname->filename, entry + NUMBER_SIZE, length);
            }
            offset = NUMBER_SIZE + PVM_NAME_SIZE;
        }
        if (status & PVMH_CATEGORYCODE) {
            offset += CATEGORYCODE_SIZE;
        }
        if (status & PVMH_ENTRYINFO) {
            offset += ENTRYINFO_SIZE;
        }
        /* @hack spelling: the global index tested in both arms, where one test
         * around the bank's would do */
        if (status & PVMH_PALGLOBALINDEX) {
            if (status & PVMH_GLOBALINDEX) {
                get_l(&texname->texaddr, entry + offset);
                texname->attr |= NJD_TEXATTR_PALGLOBALINDEX;
            }
        } else if (status & PVMH_GLOBALINDEX) {
            get_l(&texname->texaddr, entry + offset);
        }
    }
    return 1;
}

Sint32 njSearchPvrChunk(void)
{
    struct pvpl_header pvpl;
    Uint8 colors[PALETTE_ROOM];
    /* @hack redundant: unused, 12 bytes of frame that nothing reads or
     * writes */
    Uint8 unused[12];
    Uint32 name;
    Sint32 skip;
    Sint32 bytes;

    do {
        if (njTexFread(&name, sizeof(name)) < 0) {
            return -1;
        }
        switch (name) {
        case iff_MDLN:
        case iff_CONV:
        case iff_PVMI:
        case iff_IMGC:
        case iff_COMM:
            if (njTexFread(&skip, sizeof(skip)) < 0) {
                return -1;
            }
            njTexFseek(skip, GDD_SEEK_CUR);
            break;
        case iff_PVPL:
            /* @hack spelling: the read's result tested against 1, where the
             * other reads' < 0 would do */
            if (njTexFread(&pvpl, sizeof(pvpl)) != 1) {
                return -1;
            }
            /* @bug logic: the current vertex context's palette bank takes the
             * chunk's mode, where njLoadPaletteFile gives it the chunk's bank.
             * 0x8c114916 reads the mode for JSR @R12 at 0x8c114918 */
            njSetPaletteBank(pvpl.mode);
            if (pvpl.mode == NJD_TEXFMT_ARGB_8888) {
                bytes = sizeof(Uint32);
            } else {
                bytes = sizeof(Uint16);
            }
            if (njTexFread(colors, pvpl.count * bytes) < 0) {
                return -1;
            }
            skip = pvpl.size - (pvpl.count * bytes) - PVPL_HEADER_REST;
            njTexFseek(skip, GDD_SEEK_CUR);
            njSetPaletteMode(pvpl.mode);
            njSetPaletteData(pvpl.offset + (pvpl.bank * BANK_COLORS),
                             pvpl.count, colors);
            break;
        case iff_PVRT:
            njTexFseek(-CHUNK_NAME, GDD_SEEK_CUR);
            break;
        default:
            if (njTexFread(&skip, sizeof(skip)) < 0) {
                return -1;
            }
            njTexFseek(skip, GDD_SEEK_CUR);
            break;
        }
    } while (name != iff_PVRT);
    return 1;
}

Sint32 njSearchEntryCount(NJS_TEXLIST *texlist, Uint32 number)
{
    Sint32 i = 0;

    /* @hack spelling: i stepped inside the test and taken back after the
     * loop, where a step in its body would do */
    while ((texlist->textures[i++].attr & NUMBER_MASK) != number) {
    }
    return i - 1;
}

void njSetPvmTextureList(NJS_TEXLIST *texlist, NJS_TEXNAME *texname,
                         char *filename, Sint32 size)
{
    Sint32 i;

    for (i = 0; i < size; i++) {
        texname[i].filename = filename + (i * PVM_NAME_SIZE);
    }
    texlist->textures = texname;
}

Sint32 njCheckFileInfoPVM(void)
{
    Uint32 name;
    struct pvrt_chunk pvrt;
    Uint32 type;

    if (njTexFread(&name, sizeof(name)) < 0 || name != iff_PVRT) {
        return -NJD_TEXERR_HEADER;
    }
    if (njTexFread(&pvrt, sizeof(pvrt)) < 0) {
        return -NJD_TEXERR_HEADER;
    }
    njTexFseek(-(CHUNK_NAME + (Sint32)sizeof(pvrt)), GDD_SEEK_CUR);
    /* @hack spelling: type, a copy of pvrt.header.nTextureType, where the
     * field itself would do */
    type = pvrt.header.nTextureType;
    switch (type & NJD_TEXFMT_TYPE_MASK) {
    case NJD_TEXFMT_PALETTIZE4:
    case NJD_TEXFMT_PALETTIZE4_MM:
    case NJD_TEXFMT_PALETTIZE8:
    case NJD_TEXFMT_PALETTIZE8_MM:
        return TRUE;
    default:
        return FALSE;
    }
}

Sint32 njLoadTexturePvmFile(const char *filename, NJS_TEXLIST *texlist)
{
    char path[256];
    Uint32 name;
    Sint16 count;
    Uint16 status;
    struct pvmh_header pvmh;
    Uint32 *handle;
    NJS_TEXMEMLIST *tex;
    NJS_TEXNAME *texname;
    /* @hack redundant: unused_word, a frame word that nothing reads or
     * writes */
    Uint32 unused_word;
    Sint32 index;
    /* @hack spelling: bank_index, which keeps each njSearchTexList3 result
     * only for its test, where the search tested in place would do */
    Sint32 bank_index;
    Uint32 globalIndex;
    Sint32 result;
    Sint32 i;
    Sint32 n;
    Sint32 size;
    Sint32 skip;
    Uint32 bank;
    /* TRUE where the texture is palettized, else FALSE. it takes
     * njCheckFileInfoPVM's result, so a failed check leaves
     * -NJD_TEXERR_HEADER */
    Sint32 palettized;
    Sint32 listing;
    /* @hack redundant: unused_info, a texture info's room in the frame that
     * nothing reads or writes */
    NJS_TEXINFO unused_info;
    NJS_TEXINFO *texinfo;

    bank = NO_BANK;
    /* @hack redundant: a store of palettized that the loop replaces before
     * any read */
    palettized = FALSE;
    njInitTextureErrStat();
    njCheckFileName(path, filename, ".PVM");
    result = njTexFopen(path);
    if (result < 0) {
        return -1;
    }
    if (njTexFread(&name, sizeof(name)) < 0) {
        njTexFclose();
        return -1;
    }
    /* @hack spelling: the PVMH test's arms in this order, the error last,
     * where a return ahead of the rest would do */
    if (name == iff_PVMH) {
        if (njTexFread(&pvmh, sizeof(pvmh)) < 0) {
            njTexFclose();
            return -1;
        }
        /* @hack spelling: status, a copy of pvmh.status, where pvmh.status
         * would do */
        status = pvmh.status;
        count = pvmh.count;
        texlist->nbTexture = count;
        size = njCalcPvmStatusSize(status);
        /* @bug logic: ignores a failed read of the entries, and loads on with
         * the names as the read left them. nothing tests the result of the BSR
         * at 0x8c114b5e: the seek at 0x8c114b62 follows it */
        njSetTexListToPvm(texlist, size, &pvmh);
        njTexFseek(0, GDD_SEEK_SET);
        njTexFseek(pvmh.size + CHUNK_HEAD, GDD_SEEK_CUR);
    } else {
        _nj_texerr_.texerr = NJD_TEXERR_HEADER;
        njTexFclose();
        return -1;
    }
    if (njSearchPvrChunk() < 0) {
        njTexFclose();
        return -1;
    }
    /* @hack redundant: a second store of the count, which nothing has
     * changed */
    texlist->nbTexture = count;
    for (i = 0; i < count; i++) {
        n = njSearchEntryCount(texlist, i);
        texname = &texlist->textures[n];
        handle = &texname->texaddr;
        /* @hack redundant: a store of palettized that both arms below
         * replace */
        palettized = FALSE;
        if (!(texname->attr & NJD_TEXATTR_PALGLOBALINDEX)) {
            palettized = njCheckFileInfoPVM();
            if (palettized < 0) {
                _nj_texerr_.n = n;
                njTexFclose();
                return -1;
            }
            /* @hack spelling: palettized compared with TRUE, where palettized
             * alone would do */
            if (palettized == TRUE) {
                globalIndex = texname->texaddr & NJD_TEXPALGLOBAL_MASK;
                bank =
                    (texname->texaddr & NJD_TEXBANK_MASK) >> NJD_TEXBANK_SHIFT;
                index = njSearchTexList(_nj_texmemlist, _nj_texmemsize,
                                        globalIndex);
                if (index < 0) {
                    listing = UNLISTED;
                } else if ((bank_index =
                                njSearchTexList3(_nj_texmemlist, _nj_texmemsize,
                                                 globalIndex, bank)) < 0) {
                    listing = OTHER_BANK;
                } else {
                    listing = LISTED;
                }
            } else {
                /* @bug logic: a texture that isn't palettized takes the bank
                 * of the last palettized texture before it, where
                 * njLoadTextureLN gives it NO_BANK. bank takes NO_BANK once, at
                 * 0x8c114aca, and only the palettized textures' stores at
                 * 0x8c114c58 and 0x8c114d0c change it */
                globalIndex = texname->texaddr;
                index = njSearchTexList(_nj_texmemlist, _nj_texmemsize,
                                        globalIndex);
                if (index < 0) {
                    listing = UNLISTED;
                } else {
                    listing = LISTED;
                }
            }
        } else {
            globalIndex = texname->texaddr & NJD_TEXPALGLOBAL_MASK;
            bank = (texname->texaddr & NJD_TEXBANK_MASK) >> NJD_TEXBANK_SHIFT;
            index =
                njSearchTexList(_nj_texmemlist, _nj_texmemsize, globalIndex);
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
        }
        switch (listing) {
        case OTHER_BANK:
            tex = &_nj_texmemlist[index];
            njTexFseek(CHUNK_NAME, GDD_SEEK_CUR);
            if (njTexFread(&skip, sizeof(skip)) < 0) {
                njEntryTexMemList(handle, &tex->texinfo, globalIndex, 0, bank);
                /* @bug logic: the record's dummy takes the negated result of
                 * the open or of an earlier texture's load. the failed read
                 * sets no result. 0x8c114db8 reads result for the store at
                 * 0x8c114dc2 */
                RECORD(handle)->dummy = -result;
                _nj_texerr_.n = n;
                _nj_texerr_.globalIndex = globalIndex;
                njTexFclose();
                return -1;
            }
            /* @bug logic: the cache loads below read the chunk after this
             * texture's. the seek at 0x8c114de0 passes this texture's chunk,
             * and the calls at 0x8c114e64 and 0x8c114fa4 read on from there */
            njTexFseek(skip, GDD_SEEK_CUR);
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
                        /* @hack redundant: the else after the arm that
                         * returns */
                        if (result < 0) {
                            njEntryTexMemList(handle, &tex->texinfo,
                                              globalIndex, TEXMEM_LOADED, bank);
                            RECORD(handle)->count = tex->count;
                            RECORD(handle)->dummy = -result;
                            _nj_texerr_.n = n;
                            _nj_texerr_.globalIndex = globalIndex;
                            njTexFclose();
                            return -1;
                        } else {
                            njEntryTexMemList(handle, &tex->texinfo,
                                              globalIndex, TEXMEM_LOADED, bank);
                            RECORD(handle)->count = tex->count;
                            RECORD(handle)->dummy = 0;
                            /* @hack spelling: texinfo.texaddr read through the
                             * new record, where tex's, its equal, would do */
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
                        /* @hack redundant: the else after the arm that
                         * returns */
                        if (result < 0) {
                            njEntryTexMemList(handle, &tex->texinfo,
                                              globalIndex, TEXMEM_LOADED, bank);
                            RECORD(handle)->count = tex->count;
                            RECORD(handle)->dummy = -result;
                            _nj_texerr_.n = n;
                            _nj_texerr_.globalIndex = globalIndex;
                            njTexFclose();
                            return -1;
                        } else {
                            njEntryTexMemList(handle, &tex->texinfo,
                                              globalIndex, TEXMEM_LOADED, bank);
                            RECORD(handle)->count = tex->count;
                            RECORD(handle)->dummy = 0;
                            njSetFlagAddrAllTexture(
                                globalIndex, TEXMEM_CACHED | TEXMEM_LOADED,
                                tex->texinfo.texaddr);
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
                        njSetFlagAddrAllTexture(globalIndex, TEXMEM_LOADED,
                                                tex->texinfo.texaddr);
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
            break;
        case LISTED:
            /* @bug logic: a palettized name takes the first record of its
             * global index, which may be another bank's. bank_index holds the
             * record of the name's own bank. 0x8c11517c reads index, not
             * bank_index */
            tex = &_nj_texmemlist[index];
            texname->texaddr = (Uint32)tex;
            switch (texname->attr & NJD_TEXATTR_READAREA_MASK) {
            case READAREA_TEXMEM:
                njTexFseek(CHUNK_NAME, GDD_SEEK_CUR);
                if (njTexFread(&skip, sizeof(skip)) < 0) {
                    _nj_texerr_.n = n;
                    _nj_texerr_.globalIndex = globalIndex;
                    njTexFclose();
                    return -1;
                }
                njTexFseek(skip, GDD_SEEK_CUR);
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
                } else {
                    njTexFseek(CHUNK_NAME, GDD_SEEK_CUR);
                    if (njTexFread(&skip, sizeof(skip)) < 0) {
                        _nj_texerr_.n = n;
                        _nj_texerr_.globalIndex = globalIndex;
                        njTexFclose();
                        return -1;
                    }
                    njTexFseek(skip, GDD_SEEK_CUR);
                }
                break;
            case NJD_TEXATTR_BOTH:
                if (tex->texaddr & TEXMEM_LOADED) {
                    if (!(tex->texaddr & TEXMEM_CACHED)) {
                        result = njLoadCacheFromFile(texname, &tex->texinfo);
                        /* @hack redundant: the else after the arm that
                         * returns */
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
                    } else {
                        if (palettized) {
                            njAddTextureCount(globalIndex);
                        } else {
                            tex->count++;
                        }
                        njTexFseek(CHUNK_NAME, GDD_SEEK_CUR);
                        if (njTexFread(&skip, sizeof(skip)) < 0) {
                            _nj_texerr_.n = n;
                            _nj_texerr_.globalIndex = globalIndex;
                            njTexFclose();
                            return -1;
                        }
                        njTexFseek(skip, GDD_SEEK_CUR);
                    }
                } else {
                    njTexFseek(CHUNK_NAME, GDD_SEEK_CUR);
                    if (njTexFread(&skip, sizeof(skip)) < 0) {
                        _nj_texerr_.n = n;
                        _nj_texerr_.globalIndex = globalIndex;
                        njTexFclose();
                        return -1;
                    }
                    njTexFseek(skip, GDD_SEEK_CUR);
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
                    _nj_texerr_.n = n;
                    _nj_texerr_.globalIndex = globalIndex;
                    njTexFclose();
                    return -1;
                } else {
                    njEntryTexMemList(handle, texinfo, globalIndex,
                                      TEXMEM_LOADED, bank);
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
                    njEntryTexMemList(handle, texinfo, globalIndex,
                                      TEXMEM_CACHED, bank);
                    RECORD(handle)->dummy = 0;
                }
                break;
            case NJD_TEXATTR_BOTH:
                result = njLoadCacheFromFile(texname, texinfo);
                if (result < 0) {
                    njEntryTexMemList(handle, texinfo, globalIndex, 0, bank);
                    RECORD(handle)->dummy = -result;
                    _nj_texerr_.n = n;
                    _nj_texerr_.globalIndex = globalIndex;
                    njTexFclose();
                    return -1;
                }
                result = njLoadTexMemFromMemory(texname, texinfo);
                if (result < 0) {
                    njEntryTexMemList(handle, texinfo, globalIndex,
                                      TEXMEM_CACHED, bank);
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
            break;
        }
    }
    njTexFclose();
    return 1;
}

void njSetTexListToPvmM(NJS_TEXLIST *texlist, Sint32 size,
                        struct pvmh_header *pvmh)
{
    Uint16 number;
    Sint16 count;
    Uint16 status;
    char entry[ENTRY_ROOM];
    NJS_TEXNAME *texname;
    Sint32 i;
    Sint32 offset;
    Sint32 length;

    status = pvmh->status;
    count = pvmh->count;
    for (i = 0; i < count; i++) {
        njMemRead(entry, size);
        texname = &texlist->textures[i];
        get_w(&number, entry);
        texname->attr = (texname->attr & ~NUMBER_MASK) | number;
        offset = NUMBER_SIZE;
        if (status & PVMH_NAME) {
#ifdef NONMATCHING
            /* this side counts the name's bytes up to PVM_NAME_SIZE. a name
             * that fills its field has no NUL, and strlen would read past it */
            for (length = 0;
                 length < PVM_NAME_SIZE && entry[NUMBER_SIZE + length] != 0;
                 length++) {
            }
#else
            length = strlen(entry + NUMBER_SIZE);
#endif
            if (length > 0) {
                njMemCopy(texname->filename, entry + NUMBER_SIZE, length);
            }
            offset = NUMBER_SIZE + PVM_NAME_SIZE;
        }
        if (status & PVMH_CATEGORYCODE) {
            offset += CATEGORYCODE_SIZE;
        }
        if (status & PVMH_ENTRYINFO) {
            offset += ENTRYINFO_SIZE;
        }
        /* @hack spelling: the global index tested in both arms, where one test
         * around the bank's would do */
        if (status & PVMH_PALGLOBALINDEX) {
            if (status & PVMH_GLOBALINDEX) {
                get_l(&texname->texaddr, entry + offset);
                texname->attr |= NJD_TEXATTR_PALGLOBALINDEX;
            }
        } else if (status & PVMH_GLOBALINDEX) {
            get_l(&texname->texaddr, entry + offset);
        }
    }
}

void njSearchPvrChunkM(void)
{
    struct pvpl_header pvpl;
    Uint8 colors[PALETTE_ROOM];
    Uint32 name;
    Sint32 skip;
    Sint32 bytes;

    do {
        njMemRead(&name, sizeof(name));
        switch (name) {
        case iff_MDLN:
        case iff_CONV:
        case iff_PVMI:
        case iff_IMGC:
        case iff_COMM:
            njMemRead(&skip, sizeof(skip));
            njMemFseek(skip, GDD_SEEK_CUR);
            break;
        case iff_PVPL:
            njMemRead(&pvpl, sizeof(pvpl));
            /* @bug logic: the current vertex context's palette bank takes the
             * chunk's mode, as in njSearchPvrChunk. 0x8c11596a reads the mode
             * for JSR @R14 at 0x8c11596c */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: the palette bank is the
             * chunk's bank */
            njSetPaletteBank(pvpl.bank);
#else
            njSetPaletteBank(pvpl.mode);
#endif
            /* @bug logic: sizes the colors by the chunk's bank, where
             * njSearchPvrChunk tests its mode. a palette of 32-bit colors
             * outside bank 6 reads 2 bytes a color, where njSetPaletteData
             * copies 4. one of 16-bit colors in bank 6 reads 4, where
             * njSetPaletteData copies 2. 0x8c115972 reads the bank for CMP/EQ
             * #6 at 0x8c115974, which compares it with NJD_TEXFMT_ARGB_8888 */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: it sizes the colors by
             * the chunk's mode, as njSearchPvrChunk does */
            bytes = pvpl.mode == NJD_TEXFMT_ARGB_8888 ? sizeof(Uint32)
                                                      : sizeof(Uint16);
#else
            if (pvpl.bank == NJD_TEXFMT_ARGB_8888) {
                bytes = sizeof(Uint32);
            } else {
                bytes = sizeof(Uint16);
            }
#endif
            njMemRead(colors, pvpl.count * bytes);
            skip = pvpl.size - (pvpl.count * bytes) - PVPL_HEADER_REST;
            njMemFseek(skip, GDD_SEEK_CUR);
            njSetPaletteMode(pvpl.mode);
            njSetPaletteData(pvpl.offset + (pvpl.bank * BANK_COLORS),
                             pvpl.count, colors);
            break;
        case iff_PVRT:
            njMemFseek(-CHUNK_NAME, GDD_SEEK_CUR);
            break;
        default:
            njMemRead(&skip, sizeof(skip));
            njMemFseek(skip, GDD_SEEK_CUR);
            break;
        }
    } while (name != iff_PVRT);
}

void njSetInfo(NJS_TEXINFO *info)
{
    struct pvrt_chunk pvrt;
    Sint32 skip;
    Uint32 name;

    do {
        njMemRead(&name, sizeof(name));
        switch (name) {
        case iff_PVRT:
            njMemRead(&pvrt, sizeof(pvrt));
            info->texsurface.nWidth = pvrt.header.nWidth;
            info->texsurface.nHeight = pvrt.header.nHeight;
            info->texsurface.Type = pvrt.header.nTextureType
                                    << TEXSURFACE_FORMAT_SHIFT;
            break;
        default:
            njMemRead(&skip, sizeof(skip));
            njMemFseek(skip, GDD_SEEK_CUR);
        }
    } while (name != iff_PVRT);
    info->texaddr = mem_ptr;
    njMemFseek(pvrt.size - sizeof(pvrt.header), GDD_SEEK_CUR);
}

Sint32 njCheckFileInfoPVMM(void)
{
    Uint32 name;
    struct pvrt_chunk pvrt;
    Uint32 type;

    njMemRead(&name, sizeof(name));
    if (name != iff_PVRT) {
        return -NJD_TEXERR_HEADER;
    }
    njMemRead(&pvrt, sizeof(pvrt));
    njMemFseek(-(CHUNK_NAME + (Sint32)sizeof(pvrt)), GDD_SEEK_CUR);
    /* @hack spelling: type, a copy of pvrt.header.nTextureType, where the
     * field itself would do */
    type = pvrt.header.nTextureType;
    switch (type & NJD_TEXFMT_TYPE_MASK) {
    case NJD_TEXFMT_PALETTIZE4:
    case NJD_TEXFMT_PALETTIZE4_MM:
    case NJD_TEXFMT_PALETTIZE8:
    case NJD_TEXFMT_PALETTIZE8_MM:
        return TRUE;
    default:
        return FALSE;
    }
}

Sint32 njLoadTexturePvmMemory(Uint8 *addr, NJS_TEXLIST *texlist)
{
    NJS_TEXNAME *texname;
    Uint32 *handle;
    NJS_TEXMEMLIST *tex;
    NJS_TEXINFO *texinfo;
    Uint32 bank;
    Sint32 size;
    Sint32 i;
    /* TRUE where the texture is palettized, else FALSE. it takes
     * njCheckFileInfoPVMM's result, so a failed check leaves
     * -NJD_TEXERR_HEADER, which the bug tag on the check follows */
    Sint32 palettized;
    Sint32 n;
    Sint32 listing;
    Sint32 result;
    Sint32 index;
    /* @hack spelling: bank_index, which keeps each njSearchTexList3 result
     * only for its test, where the search tested in place would do */
    Sint32 bank_index;
    Uint32 globalIndex;
    Uint32 name;
    Uint16 status;
    Sint16 count;
    struct pvmh_header pvmh;
    NJS_TEXINFO info;

    bank = NO_BANK;
    njInitTextureErrStat();
    njMemOpen(addr);
    njMemRead(&name, sizeof(name));
    /* @hack spelling: the PVMH test's arms in this order, the error last,
     * where a return ahead of the rest would do */
    if (name == iff_PVMH) {
        njMemRead(&pvmh, sizeof(pvmh));
        /* @hack spelling: status, a copy of pvmh.status, where pvmh.status
         * would do */
        status = pvmh.status;
        count = pvmh.count;
        texlist->nbTexture = count;
        size = njCalcPvmStatusSize(status);
        njSetTexListToPvmM(texlist, size, &pvmh);
        njMemFseek(0, GDD_SEEK_SET);
        njMemFseek(pvmh.size + CHUNK_HEAD, GDD_SEEK_CUR);
    } else {
        _nj_texerr_.texerr = NJD_TEXERR_HEADER;
        return -1;
    }
    njSearchPvrChunkM();
    /* @hack redundant: a second store of the count, which nothing has
     * changed */
    texlist->nbTexture = count;
    for (i = 0; i < count; i++) {
#ifdef NONMATCHING
        /* this side finds each texture's PVRT chunk, past any chunk before it.
         * the matching side searches only before the first, and takes whatever
         * chunk follows a texture as the next one's */
        njSearchPvrChunkM();
#endif
        n = njSearchEntryCount(texlist, i);
        texname = &texlist->textures[n];
        handle = &texname->texaddr;
        if (!(texname->attr & NJD_TEXATTR_PALGLOBALINDEX)) {
            /* @bug logic: a failed check doesn't stop the load, where
             * njLoadTexturePvmFile's stops. palettized then holds
             * -NJD_TEXERR_HEADER.
             * the test for TRUE below takes the texture as one that isn't
             * palettized. each later test of palettized alone takes it as one
             * that is. CMP/EQ #1 at 0x8c115bf8 is the only test of the
             * result of the call at 0x8c115bf4 */
            palettized = njCheckFileInfoPVMM();
            if (palettized == TRUE) {
                globalIndex = texname->texaddr & NJD_TEXPALGLOBAL_MASK;
                bank =
                    (texname->texaddr & NJD_TEXBANK_MASK) >> NJD_TEXBANK_SHIFT;
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((index = njSearchTexList(_nj_texmemlist, _nj_texmemsize,
                                             globalIndex)) < 0) {
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
                /* @bug logic: a texture that isn't palettized takes the bank
                 * of the last palettized texture before it, as in
                 * njLoadTexturePvmFile. bank takes NO_BANK once, at 0x8c115b4e,
                 * and only the palettized textures' shifts at 0x8c115c12 and
                 * 0x8c115c6e change it */
                globalIndex = texname->texaddr;
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((index = njSearchTexList(_nj_texmemlist, _nj_texmemsize,
                                             globalIndex)) < 0) {
                    listing = UNLISTED;
                } else {
                    listing = LISTED;
                }
            }
        } else {
            /* @hack spelling: the name's texaddr read through
             * texlist->textures[n], where texname would do */
            globalIndex = texlist->textures[n].texaddr & NJD_TEXPALGLOBAL_MASK;
            bank = (texname->texaddr & NJD_TEXBANK_MASK) >> NJD_TEXBANK_SHIFT;
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((index = njSearchTexList(_nj_texmemlist, _nj_texmemsize,
                                         globalIndex)) < 0) {
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
        }
#ifdef NONMATCHING
        /* this side fails a texture that takes a new record where the table has
         * no free one. njEntryTexMemList would write that record past the
         * table's end */
        if (listing != LISTED &&
            njSearchTexList(_nj_texmemlist, _nj_texmemsize, TEXMEM_FREE) < 0) {
            _nj_texerr_.n = n;
            _nj_texerr_.globalIndex = globalIndex;
            _nj_texerr_.texerr = NJD_TEXERR_OTHER;
            return -1;
        }
#endif
        switch (listing) {
        case OTHER_BANK:
            tex = &_nj_texmemlist[index];
            njSetInfo(&info);
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
                         * texinfo.texaddr, which is null where a file loaded
                         * the record into texture memory alone. the tests at
                         * 0x8c115d06, 0x8c115daa, 0x8c115ee6 and 0x8c115f1e
                         * pass only a record without a cached copy to the
                         * njLoadCacheFromMemory calls at 0x8c115d10,
                         * 0x8c115db4, 0x8c115ef4 and 0x8c115f28 */
                        result = njLoadCacheFromMemory(texname, &tex->texinfo);
                        if (result < 0) {
                            njEntryTexMemList(handle, &tex->texinfo,
                                              globalIndex, TEXMEM_LOADED, bank);
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
                            njEntryTexMemList(handle, &tex->texinfo,
                                              globalIndex, TEXMEM_LOADED, bank);
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
                    result = njLoadTexMemFromMemory(texname, &tex->texinfo);
                    /* @hack redundant: the else after the arm that returns */
                    if (result < 0) {
#ifdef NONMATCHING
#else
                        njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                          TEXMEM_CACHED, bank);
                        RECORD(handle)->dummy = -result;
#endif
                        _nj_texerr_.n = n;
                        _nj_texerr_.globalIndex = globalIndex;
                        return -1;
                    } else {
                        njEntryTexMemList(handle, &tex->texinfo, globalIndex,
                                          TEXMEM_CACHED, bank);
                        RECORD(handle)->dummy = 0;
                        njSetFlagAddrAllTexture(globalIndex, TEXMEM_LOADED,
                                                tex->texinfo.texaddr);
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
            break;
        case LISTED:
            njSetInfo(&info);
            /* @bug logic: a palettized name takes the first record of its
             * global index, as in njLoadTexturePvmFile's LISTED case.
             * 0x8c115c9a reads index, not bank_index, for the store at
             * 0x8c115e98 */
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
                    if ((result = njLoadTexMemFromMemory(texname,
                                                         &tex->texinfo)) < 0) {
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
                    if ((result = njLoadCacheFromMemory(texname,
                                                        &tex->texinfo)) < 0) {
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
                        /* @hack spelling: the assignment inside the test,
                         * where a statement of its own would do */
                        if ((result = njLoadCacheFromMemory(
                                 texname, &tex->texinfo)) < 0) {
                            tex->dummy = -result;
                            _nj_texerr_.n = n;
                            _nj_texerr_.globalIndex = globalIndex;
                            return -1;
                        }
                        tex->dummy = 0;
                        if (palettized) {
                            njSetFlagAddrAllTexture(
                                globalIndex, TEXMEM_CACHED | TEXMEM_LOADED,
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
                    if ((result = njLoadTexMemFromMemory(texname,
                                                         &tex->texinfo)) < 0) {
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
            njSetInfo(&info);
            texinfo = njTexMemList(globalIndex);
            njMemCopy(texinfo, &info, sizeof(NJS_TEXINFO));
            switch (texname->attr & NJD_TEXATTR_READAREA_MASK) {
            case READAREA_TEXMEM:
                /* @hack redundant: the else after the arm that returns */
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((result = njLoadTexMemFromMemory(texname, texinfo)) < 0) {
#ifdef NONMATCHING
#else
                    njEntryTexMemList(handle, texinfo, globalIndex, 0, bank);
                    RECORD(handle)->dummy = -result;
#endif
                    _nj_texerr_.n = n;
                    _nj_texerr_.globalIndex = globalIndex;
                    return -1;
                } else {
                    /* @hack spelling: &texname->texaddr, where handle, which
                     * holds it, would do */
                    njEntryTexMemList(&texname->texaddr, texinfo, globalIndex,
                                      TEXMEM_LOADED, bank);
                    RECORD(handle)->dummy = 0;
                    if (palettized) {
                        RECORD(handle)->texaddr |= TEXMEM_SHARED_SURFACE;
                    }
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
                /* @hack spelling: the assignments inside both tests, where
                 * statements of their own would do */
                /* @hack redundant: the else after the arm that returns */
                if ((result = njLoadTexMemFromMemory(texname, texinfo)) < 0) {
                    njEntryTexMemList(handle, texinfo, globalIndex, 0, bank);
                    RECORD(handle)->dummy = -result;
                    _nj_texerr_.n = n;
                    _nj_texerr_.globalIndex = globalIndex;
                    return -1;
                    /* @bug memory: leaks the texture memory of the surface that
                     * njLoadTexMemFromMemory creates and loads at 0x8c116062.
                     * njLoadCacheFromMemory at 0x8c116090 writes a second
                     * surface over the first in texinfo. the record keeps the
                     * second, which holds no texels. the file load's
                     * NJD_TEXATTR_BOTH case creates one surface, at
                     * 0x8c11574e */
                } else if ((result = njLoadCacheFromMemory(texname, texinfo)) <
                           0) {
                    njEntryTexMemList(handle, texinfo, globalIndex,
                                      TEXMEM_LOADED, bank);
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
                break;
            }
            break;
        }
    }
    return 1;
}
