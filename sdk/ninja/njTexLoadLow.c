/* a texture's texels copied into its surface in texture memory, whole or in
 * part */
#include <kamui.h>
#include <kamui_internal.h>
#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* copy size bytes of a small VQ texture's data from source into surface, and
 * return 0. the bytes start offset bytes into the data, which holds the
 * codebook and then the indices. surface holds the codebook at its start, and
 * the indices from SMALLVQ_INDICES on. the call copies no more indices than the
 * texture has. a part lands in its place where it starts at 0, at the
 * codebook's end or past it. one that starts past the codebook can still run
 * past the indices' end. it runs past by as much as its offset less the
 * codebook's size. one that starts inside the codebook past 0 puts its indices
 * offset bytes past their place */
Sint32 njConvertSmallVQToMem(NJS_TEXSURFACE *surface, Uint32 *source,
                             Uint32 offset, Uint32 size);
/* copy all of a small VQ texture's data from source into surface, and return
 * 0. the bytes land where njConvertSmallVQToMem places them */
Sint32 njConvertSmallVQToMemAll(NJS_TEXSURFACE *surface, Uint32 *source);
/* NOLINTEND(misc-use-internal-linkage) */

/* the byte of a small VQ texture's surface where its indices start */
#define SMALLVQ_INDICES 0x800
/* an offset or a size ANDed with WORD_MASK is cut down to whole words */
#define WORD_MASK (~3)

Sint32 njConvertSmallVQToMem(NJS_TEXSURFACE *surface, Uint32 *source,
                             Uint32 offset, Uint32 size)
{
    Sint32 level;
    Uint32 book_size;
    Uint32 index_size;
    Uint32 book_part;
    Uint32 index_part;
    Uint32 start;
    Uint8 *from;

    level = njGetTextureLevel(surface->nWidth, surface->nHeight);
    if (surface->fSurfaceFlags & NJD_SURFACEFLAGS_MIPMAPED) {
        book_size = _nj_SVQCodeBookSizeMM[level];
        index_size = _nj_SVQIndexSizeMM[level] + SMALLVQ_MM_DUMMY;
    } else {
        book_size = _nj_SVQCodeBookSize[level];
        index_size = _nj_SVQIndexSize[level];
    }
    book_part = 0;
    index_part = 0;
    if (book_size >= offset) {
        book_part = book_size - offset;
        if (size <= book_part) {
            book_part = size;
        } else {
            index_part = size - book_part;
            if (index_part > index_size) {
                index_part = index_size;
            }
        }
    } else {
        index_part = size;
        /* @bug logic: a part that starts past the codebook can run past the
         * indices' end. the cut takes the indices' whole size, not what is
         * left of them: CMP/HI R5,R14 at 0x8c13ad84. njLoadTexMemFromFile,
         * the one caller the game can reach, sends no such part. it ends its
         * last part at the surface's TextureSize or before, and that size
         * counts the codebook and the indices */
        if (index_part > index_size) {
            index_part = index_size;
        }
    }
    if (book_part) {
        from = (Uint8 *)source;
        start = (Uint32)surface->pSurface + offset;
        kmiCopyToFrameBuffer(start, (Uint32 *)from, book_part);
        if (index_part) {
            from += book_part;
            /* @bug logic: a part that starts inside the codebook past 0 puts
             * its indices offset bytes past their place. start takes offset,
             * ADD R11,R9 at 0x8c13ad96, and keeps it when ADD R3,R9 at
             * 0x8c13adba moves it on to the indices. njLoadTexMemFromFile,
             * the one caller the game can reach, sends no such part. it starts
             * each part at 0 or at a whole number of 0x800-byte sectors. the
             * codebook lies below SMALLVQ_INDICES, 0x800, so no such start
             * falls inside it */
            start += SMALLVQ_INDICES;
            kmiCopyToFrameBuffer(start, (Uint32 *)from, index_part);
        }
    } else if (index_part) {
        from = (Uint8 *)source;
        start =
            (Uint32)surface->pSurface + offset - book_size + SMALLVQ_INDICES;
        kmiCopyToFrameBuffer(start, (Uint32 *)from, index_part);
    }
    return 0;
}

Sint32 njConvertSmallVQToMemAll(NJS_TEXSURFACE *surface, Uint32 *source)
{
    Sint32 level;
    Uint32 book_size;
    Uint32 index_size;
    Uint32 start;
    Uint8 *from;

    level = njGetTextureLevel(surface->nWidth, surface->nHeight);
    if (surface->fSurfaceFlags & NJD_SURFACEFLAGS_MIPMAPED) {
        book_size = _nj_SVQCodeBookSizeMM[level];
        index_size = _nj_SVQIndexSizeMM[level] + SMALLVQ_MM_DUMMY;
    } else {
        book_size = _nj_SVQCodeBookSize[level];
        index_size = _nj_SVQIndexSize[level];
    }
    from = (Uint8 *)source;
    start = (Uint32)surface->pSurface;
    kmiCopyToFrameBuffer(start, (Uint32 *)from, book_size);
    from += book_size;
    start += SMALLVQ_INDICES;
    kmiCopyToFrameBuffer(start, (Uint32 *)from, index_size);
    return 0;
}

Sint32 njLoadTexturePart(NJS_TEXSURFACE *surface, Uint32 *source, Uint32 offset,
                         Uint32 size)
{
    Uint32 position;
    Uint32 length;
    Uint32 type;
    Uint32 format;
    Uint32 skip;
    Uint32 start;
    Uint32 rest;

    position = offset & WORD_MASK;
    length = size & WORD_MASK;
    if (length == 0) {
        return 0;
    }
    /* @hack spelling: the assignment to type inside the expression, where a
     * statement of its own would do */
    format = ((type = surface->Type) >> TEXSURFACE_FORMAT_SHIFT) &
             NJD_TEXFMT_TYPE_MASK;
    if ((Uint16)type == KM_SURFACETYPE_SMALLVQ_TEXTURE) {
        njConvertSmallVQToMem(surface, source, offset, size);
        return 0;
    } else if (format == NJD_TEXFMT_TWIDDLED_MM) {
        skip = TWIDDLED_MM_SKIP;
    } else {
        skip = 0;
    }
    start = position + skip;
    rest = surface->TextureSize - start;
    if (start >= surface->TextureSize) {
        return KMSTATUS_INVALID_ADDRESS;
    }
    if (rest > length) {
        rest = length;
    }
    start += (Uint32)surface->pSurface;
    kmiCopyToFrameBuffer(start, source, rest);
    return 0;
}

Sint32 njLoadTextureLow(NJS_TEXSURFACE *surface, Uint32 *source)
{
    Uint32 format;
    Sint32 level;

    format = (surface->Type >> TEXSURFACE_FORMAT_SHIFT) & NJD_TEXFMT_TYPE_MASK;
    if (SURFACE_KIND(surface) == KM_SURFACETYPE_SMALLVQ_TEXTURE) {
        njConvertSmallVQToMemAll(surface, source);
    } else if (format == NJD_TEXFMT_TWIDDLED_MM) {
        level = njGetTextureLevel(surface->nWidth, surface->nHeight);
        /* @bug logic: the copy runs 4 bytes past the surface's TextureSize.
         * its size adds the 4 bytes the start skips, ADD #4,R6 at
         * 0x8c13af0a, though TextureSize counts them already. every load of
         * a TWIDDLED_MM texture through njLoadTexMemFromMemory reaches this.
         * the 4 bytes stay in the surface's block. kmiTXalloc rounds each
         * block up to 32 bytes, ADD R3,R12 at 0x8c169282, and this type's
         * TextureSize lies 16 or 24 bytes short of a multiple of 32 */
        kmiCopyToFrameBuffer((Uint32)surface->pSurface + TWIDDLED_MM_SKIP,
                             source,
                             _nj_ImageByteSize[level] + TWIDDLED_MM_SKIP);
    } else {
        kmiCopyToFrameBuffer((Uint32)surface->pSurface, source,
                             surface->TextureSize);
    }
    return 0;
}
