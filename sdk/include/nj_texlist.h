/* Ninja's texture records, the NJS_TEXMEMLIST entries of _nj_texmemlist, and
 * the calls that take, fill and free them. the header also holds the loads of a
 * texture's texels, and the state the texture calls share */
#ifndef NJ_TEXLIST_H
#define NJ_TEXLIST_H
#include <ninja.h>
#include <nj_render_types.h> /* IWYU pragma: export */
#include <sg_xpt.h>
#include <shinobi.h>

/* a record's bank where its texture isn't palettized */
#define NO_BANK 0xffffffff
/* the bits of a bank number a palettized texture draws with. a 4-bit texture
 * takes bits 0 to 5, one of 64 banks of 16 colors. an 8-bit texture takes
 * bits 4 and 5, one of 4 banks of 256 colors */
#define BANK_MASK_4BPP 0x3f
#define BANK_MASK_8BPP 0x30
/* the field of a record's texparambuffer, the texture word, that holds those
 * bits */
#define TEX_PALETTE 0x07e00000
#define TEX_PALETTE_SHIFT 21
/* the record whose address *handle holds, as a loaded texture name's texaddr
 * holds it */
#define RECORD(handle) ((NJS_TEXMEMLIST *)*(handle))
/* where a load finds a name's global index among the records. OTHER_BANK: only
 * in records of other palette banks. LISTED: in a record of the name's bank, or
 * in any record where the texture isn't palettized. UNLISTED: in no record */
#define OTHER_BANK 0
#define LISTED 1
#define UNLISTED 2
/* the flags in a record's texaddr. TEXMEM_CACHED: texinfo.texaddr points to a
 * copy of the texture's texels in the texture cache. TEXMEM_LOADED:
 * texinfo.texsurface holds the texture in texture memory. count then holds its
 * users, the loads of the texture not yet released. a record with neither flag
 * holds no copy of its texture.
 *
 * TEXMEM_SHARED_SURFACE marks the record whose surface the other records of a
 * palettized texture's global index share. njEntryTexMemList gives a record
 * entered with TEXMEM_UNK_4 a user and its parameter words, as under
 * TEXMEM_LOADED. no load enters a record with it */
#define TEXMEM_CACHED 0x1
#define TEXMEM_LOADED 0x2
#define TEXMEM_UNK_4 0x4
#define TEXMEM_SHARED_SURFACE 0x8

/* a surface's KM_SURFACETYPE_, the low half of its Type */
#define SURFACE_KIND(surface) ((Uint16)(surface)->Type)
/* the bits of an NJS_TEXSURFACE's fSurfaceFlags. a twiddled texture's texels
 * lie in the order the hardware reads them, and any other's row after row */
#define NJD_SURFACEFLAGS_MIPMAPED 0x01
#define NJD_SURFACEFLAGS_TWIDDLED 0x04
#define NJD_SURFACEFLAGS_VQ 0x08
#define NJD_SURFACEFLAGS_STRIDE 0x10
#define NJD_SURFACEFLAGS_PALETTIZED 0x20

/* the chunk names of a PVR texture, each its four ASCII bytes as a
 * little-endian word. a chunk is its name, its size, then its data. the size
 * counts the bytes after it. a GBIX chunk holds the texture's global index, and
 * a PVRT chunk its header and texels */
#define iff_GBIX 0x58494247
#define iff_PVRT 0x54525650
struct gbix_chunk {
    Uint32 name;
    Uint32 size;
    Uint32 globalIndex;
};
/* a PVRT chunk after its name, up to its texels */
struct pvrt_chunk {
    Uint32 size;
    NJS_PVRHEADERDATA header;
};

/* a PVPL chunk holds a palette's colors */
#define iff_PVPL 0x4c505650
/* a PVPL chunk after its name, before the colors */
struct pvpl_header {
    Uint32 size; /* the bytes after this word */
    Uint16 mode; /* the colors' format */
    Sint16 bank;
    Sint16 offset;
    Sint16 count;
};
/* the header's bytes after its size word. size counts them, then the colors */
#define PVPL_HEADER_REST (sizeof(struct pvpl_header) - sizeof(Uint32))

/* make the n records at addr _nj_texmemlist, each free, with no flags and no
 * users */
void njInitTexMemList(NJS_TEXMEMLIST *addr, Uint32 n);
/* free tex: its globalIndex and bank become TEXMEM_FREE. the rest of it becomes
 * 0, but for dummy and its surface's pVirtual and pPhysical */
void njFreeTexMemList(NJS_TEXMEMLIST *tex);
/* clear flag in each record of globalIndex. a record left without TEXMEM_LOADED
 * takes a null pSurface and no users. one left without TEXMEM_CACHED takes a
 * null texinfo.texaddr. one left with neither is freed. addr is never read */
void njClearFlagAddrAllTexture(Uint32 globalIndex, Uint32 flag, void *addr);
/* fill a free record, and store its address in *handle. with no record free, it
 * fills the one past the list's end. the record takes info's texaddr, and its
 * surface but for pVirtual and pPhysical. it takes globalIndex and bank. its
 * texaddr becomes flag less TEXMEM_SHARED_SURFACE. where flag holds
 * TEXMEM_LOADED or TEXMEM_UNK_4, the record takes one user. njSetTextureParam
 * then sets its parameter words. otherwise the record takes no users */
void njEntryTexMemList(Uint32 *handle, NJS_TEXINFO *info, Uint32 globalIndex,
                       Uint32 flag, Uint32 bank);
/* the index of the first record with globalIndex and bank among the n at list.
 * it returns -1 where none has both, or where list is null */
Sint32 njSearchTexList3(NJS_TEXMEMLIST *list, Uint32 n, Uint32 globalIndex,
                        Uint32 bank);
/* add flag to each record of globalIndex, and make addr its texinfo.texaddr.
 * where flag holds TEXMEM_LOADED, each also takes a user. njSetTextureParam
 * then sets its parameter words */
void njSetFlagAddrAllTexture(Uint32 globalIndex, Uint32 flag, void *addr);
/* give each record of globalIndex one more user */
void njAddTextureCount(Uint32 globalIndex);
/* the texinfo of the record that njEntryTexMemList would take next. globalIndex
 * is never read */
NJS_TEXINFO *njTexMemList(Uint32 globalIndex);

/* release the record at texaddr, an NJS_TEXMEMLIST's address, and return 1.
 * a free record, or a cached one that isn't loaded, is an error. the call then
 * sets _nj_texerr_.texerr to NJD_TEXERR_OTHER, and returns -1. any other
 * record that is neither loaded nor cached is freed.
 *
 * a loaded record of a texture that isn't palettized drops a user. at its last
 * user its surface is freed, and it loses TEXMEM_LOADED. it is then freed
 * unless it is cached. _nj_current_texture becomes -1 where it was the
 * record's global index.
 *
 * the records of a palettized texture's global index share one surface.
 * while the released record has more than one user, each of them drops a
 * user. at its last user the surface is freed. where the released record is
 * cached, each of them then loses TEXMEM_LOADED, TEXMEM_SHARED_SURFACE and its
 * users. where it isn't, each of them is freed */
Sint32 njReleaseTextureLN(Uint32 texaddr);
/* load texture n of texlist into a texture record, and return 1, or -1 where
 * the load fails. either way, the name's texaddr takes the record's address.
 * the name must not set both NJD_TEXATTR_CACHE and NJD_TEXATTR_BOTH.
 *
 * under NJD_TEXATTR_GLOBALINDEX, texaddr gives the texture's global index, and
 * the texture counts as not palettized. otherwise, under
 * NJD_TEXATTR_PALGLOBALINDEX, texaddr holds the index and a bank. the texture
 * then counts as palettized. otherwise the texture's GBIX chunk gives the
 * index, and its format says whether it is palettized. without a GBIX chunk,
 * the index is the highest at or below _nj_user_globalIndex that no record
 * holds. a palettized texture's bank is texaddr's NJD_TEXBANK_MASK bits, and
 * any other's NO_BANK.
 *
 * a name with NJD_TEXATTR_TYPE_MEMORY loads from the NJS_TEXINFO its filename
 * points to. where that info's texaddr holds a PVR texture's chunks, the call
 * points texaddr at the texels. it also sets the info's sides and Type, as
 * njSetTextureInfo would. any other name loads from the PVR file
 * njCheckFileName makes of its filename and .PVR.
 *
 * the name takes the index's first record where a record of the texture's bank
 * holds the index. for a texture that isn't palettized, any record of the index
 * serves. where only records of other banks hold the index, the name takes a
 * new record. it shares their surface and cached copy. otherwise it takes a new
 * record. the texture then loads where the name's NJD_TEXATTR_READAREA_MASK
 * bits say, unless its record holds it there already. a texture the cache holds
 * loads into texture memory from the cached copy. unless the name loads into
 * the cache alone, its record gains a user. for a palettized texture, each
 * record of the index gains one.
 *
 * under NJD_TEXATTR_TEXCONTINUE, a name after the first takes a new record. the
 * record shares the surface and cached copy of the previous name's record,
 * which must exist. its bank is texaddr's NJD_TEXBANK_MASK bits. its
 * globalIndex is whatever the stack held, a defect the bug tag in
 * njLoadTextureLN.c gives.
 *
 * where a load fails, _nj_texerr_.texerr holds the error. the record's dummy
 * holds the failed call's result as a positive number. a file that won't open,
 * or whose header can't be read, gives the name a new record. that record has
 * no flags, and holds _nj_tex_err_surface's surface. a header that can't be
 * read leaves dummy as it was. where the texels fail to load, _nj_texerr_'s n
 * and globalIndex take the name's. the record lacks the copy that failed */
Sint32 njLoadTextureLN(NJS_TEXLIST *texlist, Uint32 n);

/* describe in surface a texture of nTextureType, an NJD_TEXFMT_ format, and
 * nWidth by nHeight texels, and return KMSTATUS_SUCCESS. Type takes the format
 * from bit TEXSURFACE_FORMAT_SHIFT up, and a KM_SURFACETYPE_ below it.
 * BitDepth, PixelFormat, fSurfaceFlags and TextureSize follow from the format.
 *
 * the call returns KMSTATUS_INVALID_TEXTURE_TYPE for a texture it can't
 * describe, and fills surface still. each side must be a power of two from 8 to
 * 1024, or 8 to 64 for a small VQ texture. only RECTANGLE, STRIDE,
 * TWIDDLED_RECTANGLE, PALETTIZE4 and PALETTIZE8 textures may have sides that
 * differ. a type that isn't palettized fails with a color format no
 * KM_PIXELFORMAT_ names, such as ARGB_8888. PixelFormat then stays as it was */
Sint32 njCheckTextureFormatLow(NJS_TEXSURFACE *surface, Uint32 nWidth,
                               Uint32 nHeight, Uint32 nTextureType);
/* log2 of the smaller of nWidth and nHeight, for a side from 1 to 1024. it
 * returns -1 where that side isn't a power of two */
Sint32 njGetTextureLevel(Uint32 nWidth, Uint32 nHeight);
/* describe surface as njCheckTextureFormatLow does, take its texture memory,
 * and return KMSTATUS_SUCCESS. where the description fails, the call returns
 * its error, and takes no memory. where the memory is full, it calls the
 * callback that kmSetTexOverflowCallback sets, then returns
 * KMSTATUS_NOT_ENOUGH_MEMORY */
Sint32 njCreateTextureSurface(NJS_TEXSURFACE *surface, Uint32 nWidth,
                              Uint32 nHeight, Uint32 nTextureType);

/* copy the whole texture at source into surface's texture memory, and return 0.
 * a small VQ texture goes as njConvertSmallVQToMemAll copies it. a TWIDDLED_MM
 * texture's copy starts 4 bytes into the surface. it ends 4 bytes past the
 * surface's TextureSize, a defect the bug tag in njTexLoadLow.c describes */
Sint32 njLoadTextureLow(NJS_TEXSURFACE *surface, Uint32 *source);
/* copy size bytes of the texture at source into surface's texture memory, and
 * return 0. the bytes lie offset bytes into the texture. a size below 4 copies
 * nothing. a small VQ texture's part goes as njConvertSmallVQToMem copies it.
 * any other texture's offset and size are cut to whole words. a TWIDDLED_MM
 * texture's part lands 4 bytes further on. the copy stops at the surface's
 * TextureSize. a part that starts at or past it copies nothing, and returns
 * KMSTATUS_INVALID_ADDRESS */
Sint32 njLoadTexturePart(NJS_TEXSURFACE *surface, Uint32 *source, Uint32 offset,
                         Uint32 size);
/* read the open file's chunks through the PVRT chunk's header, which leaves the
 * file at the texels. *size takes the PVRT chunk's size. an ABGR texture of
 * ARGB_1555 under NJD_TEXATTR_AUTOMIPMAP takes the ABGR_MM type.
 * njGetSurfacePvr then creates info's surface, as njCreateTextureSurface does.
 * njGetInfoPvr only describes it, as njCheckTextureFormatLow does.
 *
 * each returns 1, or -1 where a read fails. each returns -NJD_TEXERR_HEADER for
 * a side that isn't a power of two from 8 to 1024. each returns
 * -NJD_TEXERR_SURFACE where the surface fails. either code also goes to
 * _nj_texerr_.texerr */
Sint32 njGetSurfacePvr(NJS_TEXINFO *info, Uint32 attr, Sint32 *size);
Sint32 njGetInfoPvr(NJS_TEXINFO *info, Uint32 attr, Sint32 *size);
/* load the open file's texture into a surface it creates in info, and return 1.
 * it creates the surface as njGetSurfacePvr does. it returns njGetSurfacePvr's
 * error where that fails. it returns -1 where a read of the texels fails, which
 * frees the surface.
 *
 * the texels go a buffer at a time through _nj_texture_buffer_st. the call
 * waits for each part's transfer to end. where the chunk holds more than the
 * texture, the file is left after the chunk. where it holds less, only its
 * bytes load */
Sint32 njLoadTexMemFromFile(NJS_TEXNAME *texname, NJS_TEXINFO *info);
/* read the open file's texture into a new block of the texture cache, and
 * return 1. info's surface is described as njGetInfoPvr describes it. info's
 * texaddr takes the block. the file is left as njLoadTexMemFromFile leaves it.
 *
 * the call returns njGetInfoPvr's error where that fails. it returns
 * -NJD_TEXERR_MAINMEMORY where the cache has no block for the texels, and
 * _nj_texerr_.texerr takes that code. it returns -NJD_TEXERR_FILELOAD where the
 * read fails, which leaves the block taken */
Sint32 njLoadCacheFromFile(NJS_TEXNAME *texname, NJS_TEXINFO *info);
/* copy the texels at info's texaddr into a new block of the texture cache, and
 * return 1. texaddr then points to the block. the call first creates info's
 * surface from its sides and Type, as njCreateTextureSurface does. that
 * surface's texture memory stays taken, whether or not the copy succeeds.
 *
 * the call returns -NJD_TEXERR_SURFACE where the surface fails. it returns
 * -NJD_TEXERR_MAINMEMORY where the cache has no block for the texels. either
 * code also goes to _nj_texerr_.texerr */
Sint32 njLoadCacheFromMemory(NJS_TEXNAME *texname, NJS_TEXINFO *info);
/* load the texels at info's texaddr into a surface it creates in info, and
 * return 1. the surface is made from info's sides and Type, as
 * njCreateTextureSurface makes it. the texels load as njLoadTextureLow loads
 * them, and the call waits for the transfer to end. it returns
 * -NJD_TEXERR_SURFACE where the surface fails, and _nj_texerr_.texerr takes
 * that code */
Sint32 njLoadTexMemFromMemory(NJS_TEXNAME *texname, NJS_TEXINFO *info);
/* copy size bytes from src to dst. each move is of the widest unit up to 32
 * bytes that divides dst, src and size. size must be above 0. FPSCR's SZ must
 * be clear, and stays clear */
void njTexMemCopy(void *dst, void *src, Uint32 size);

/* TRUE where type, an NJD_TEXFMT_ texture type, is one of the four palettized
 * types, else FALSE */
Sint32 njCheckPaletteType(Uint32 type);
/* store the open texture file's global index in *globalIndex. the call returns
 * TRUE where its texture is palettized, else FALSE. a GBIX chunk ahead of the
 * PVRT chunk gives the index. without one, the index is the highest at or below
 * _nj_user_globalIndex that no record holds. _nj_user_globalIndex then takes
 * it. the call leaves the file at its start. it returns -NJD_TEXERR_HEADER
 * where a read fails, which leaves the file where the read stopped */
Sint32 njCheckFileInfo(Uint32 *globalIndex);
/* make bank the current vertex context's palette bank, and set the context
 * through kmSetVertexRenderState */
void njSetPaletteBank(Uint32 bank);

/* the texture cache, in the buffer njInitCacheTextureBuffer gave it.
 * njCacheTextureMalloc takes size bytes from the first free block that holds
 * them. it returns their address, or NULL where no block holds them. a block's
 * bytes follow a 32-byte header. njCacheTextureFree frees the block whose bytes
 * start at addr. it merges the block with a free block on either side. it
 * ignores an addr whose header lacks the allocator's mark. a block must not be
 * freed twice */
void *njCacheTextureMalloc(Uint32 size);
void njCacheTextureFree(void *addr);

/* load the texture of the record at tex again, into the surface it holds, and
 * return 1.
 *
 * under NJD_TEXATTR_TYPE_MEMORY in attr, texaddr points to the texels. they
 * load whole, as njLoadTextureLow loads them. under a nonzero lod,
 * kmReLoadMipmap instead loads the mipmaps lod gives, of a surface that has
 * them. the call waits for that transfer to end. where it fails,
 * _nj_texerr_.texerr takes NJD_TEXERR_TEXMEMLOAD, and the call returns -1.
 *
 * without NJD_TEXATTR_TYPE_MEMORY, texaddr is a file's name, which
 * njCheckFileName makes a path with .PVR. the file's texels load a buffer at a
 * time through _nj_texture_buffer_st, as njLoadTexturePart loads them. the call
 * returns -1 where the file won't open or a read fails, which closes the file.
 * lod must be 0 or a power of two from 2 to 1024. VQ, small VQ and palettized
 * textures load whole whatever lod is. under a nonzero lod, any other texture
 * with mipmaps reads only its mipmap of side lod. it writes those texels over
 * the mipmap of side 2 * lod, not lod's own. the bug tag in
 * njReLoadTextureNum.c gives that defect */
Sint32 njReLoadTextureLN(NJS_TEXMEMLIST *tex, void *texaddr, Uint32 attr,
                         Uint32 lod);

/* write _nj_current_texture_path and name to path, then ext, and return 1. ext
 * is an extension of 4 characters, its dot included, in capitals. the call
 * leaves it off where name's last 4 characters are ext in any case. name must
 * hold 4 characters or more: a shorter one reads the bytes before it */
Sint32 njCheckFileName(char *path, const char *name, const char *ext);
/* convert s to uppercase in place, by toupper */
void strupr(char *s);

/* the sector buffer the texture file reads go through */
extern Sint8 _nj_file_buffer[GDD_FS_SCTSIZE];

/* Each table is indexed by a level, as njGetTextureLevel gives it.
 * _nj_ImageByteSize holds the bytes of a 16-bit texture of that level and its
 * mipmaps, with 2 bytes more. _nj_SVQCodeBookSize and _nj_SVQIndexSize hold the
 * bytes of a small VQ texture's codebook and of its indices. the MM tables hold
 * the same for one with mipmaps. a mipmapped small VQ texture's surface takes
 * SMALLVQ_MM_DUMMY bytes past its indices */
extern Uint32 _nj_SVQCodeBookSize[7];
extern Uint32 _nj_SVQCodeBookSizeMM[7];
extern Uint32 _nj_SVQIndexSize[7];
extern Uint32 _nj_SVQIndexSizeMM[7];
extern Uint32 _nj_ImageByteSize[11];
#define SMALLVQ_MM_DUMMY 10
/* the bytes a TWIDDLED_MM texture's surface holds ahead of the data its file
 * gives, which _nj_ImageByteSize doesn't count. the sizes of VQ_MM and
 * palettized MM textures scale a TWIDDLED_MM surface's, these bytes included */
#define TWIDDLED_MM_SKIP 4
/* the bytes of a 16-bit texel, as _nj_ImageByteSize counts them */
#define TEXEL_BYTES 2

/* the texture loads read a file's texels through the buffer at
 * _nj_texture_buffer_st, of _nj_texture_buffer_max_size bytes. that size is a
 * whole number of sectors. _nj_user_globalIndex is the highest global index a
 * texture without a GBIX chunk may take. _nj_current_texture_path is the text
 * each texture file's name follows. _nj_palette_mode_ is the mode
 * njSetPaletteMode was last given. _nj_palette_bpp_ is the size of the
 * palette's colors, NJD_TEXPALETTE_16BPP or NJD_TEXPALETTE_32BPP */
extern Sint8 *_nj_texture_buffer_st;
extern Uint32 _nj_texture_buffer_max_size;
extern Uint32 _nj_user_globalIndex;
extern char _nj_current_texture_path[TEXTURE_PATH_SIZE];
extern Uint32 _nj_palette_mode_;
extern Sint32 _nj_palette_bpp_;

#endif
