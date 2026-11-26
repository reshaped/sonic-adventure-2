/* the open PVR file's texture header, and the surface made from it */
#include <kamui.h>
#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* read the open file's chunks through the PVRT chunk's header, and return 1. a
 * chunk of another name is passed over, and the file is left at the texels.
 * *size takes the PVRT chunk's size, *width and *height the texture's sides,
 * and *type its NJD_TEXFMT_ format. where a read fails, the call returns -1 and
 * writes none of the four. each side must be a power of two from 8 to 1024.
 * where one isn't, the call sets _nj_texerr_.texerr to NJD_TEXERR_HEADER and
 * returns -NJD_TEXERR_HEADER. *type is then left unwritten, and so is *height
 * where the width fails */
Sint32 _njLoadPvrHeader(Sint32 *width, Sint32 *height, Sint32 *type,
                        Sint32 *size);
/* NOLINTEND(misc-use-internal-linkage) */

/* the halves of a surface's Type: its format from bit TEXSURFACE_FORMAT_SHIFT
 * up, and its KM_SURFACETYPE_ below */
#define TYPE_FORMAT 0xffff0000
#define TYPE_KIND 0x0000ffff

Sint32 _njLoadPvrHeader(Sint32 *width, Sint32 *height, Sint32 *type,
                        Sint32 *size)
{
    Uint32 name;
    Uint32 skip;
    struct pvrt_chunk pvrt;

    do {
        if (njTexFread(&name, sizeof(name)) < 0) {
            return -1;
        }
        switch (name) {
        case iff_PVRT:
            if (njTexFread(&pvrt, sizeof(pvrt)) < 0) {
                return -1;
            }
            break;
        default:
            if (njTexFread(&skip, sizeof(skip)) < 0) {
                return -1;
            }
            njTexFseek(skip, GDD_SEEK_CUR);
            break;
        }
    } while (name != iff_PVRT);
    *size = pvrt.size;
    *width = pvrt.header.nWidth;
    switch (*width) {
    case 8:
    case 16:
    case 32:
    case 64:
    case 128:
    case 256:
    case 512:
    case 1024:
        break;
    default:
        _nj_texerr_.texerr = NJD_TEXERR_HEADER;
        return -NJD_TEXERR_HEADER;
    }
    *height = pvrt.header.nHeight;
    switch (*height) {
    case 8:
    case 16:
    case 32:
    case 64:
    case 128:
    case 256:
    case 512:
    case 1024:
        break;
    default:
        _nj_texerr_.texerr = NJD_TEXERR_HEADER;
        return -NJD_TEXERR_HEADER;
    }
    *type = pvrt.header.nTextureType;
    return 1;
}

Sint32 njGetSurfacePvr(NJS_TEXINFO *info, Uint32 attr, Sint32 *size)
{
    Sint32 width;
    Sint32 height;
    Sint32 type;
    Sint32 result;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((result = _njLoadPvrHeader(&width, &height, &type, size)) < 0) {
        return result;
    }
    /* @bug logic: under NJD_TEXATTR_AUTOMIPMAP, an ABGR texture of any color
     * format but ARGB_1555 takes no mipmaps. CMP/EQ R3,R2 at 0x8c15d3ea
     * compares the whole format with NJD_TEXFMT_ABGR, whose color format is
     * ARGB_1555's 0. EXTU.B R1,R1 at 0x8c15d3fa keeps any color format for
     * the mipmapped type */
    if (type == NJD_TEXFMT_ABGR) {
        if (attr & NJD_TEXATTR_AUTOMIPMAP) {
            type = (type & NJD_TEXFMT_COLOR_MASK) | NJD_TEXFMT_ABGR_MM;
        }
    }
    if (njCreateTextureSurface(&info->texsurface, width, height, type) !=
        KMSTATUS_SUCCESS) {
        _nj_texerr_.texerr = NJD_TEXERR_SURFACE;
        return -NJD_TEXERR_SURFACE;
    }
    /* @hack redundant: Type ORed with the format and the KM_SURFACETYPE_ that
     * njCreateTextureSurface gave it, which changes no bit */
    info->texsurface.Type |=
        (((Uint32)type << TEXSURFACE_FORMAT_SHIFT) & TYPE_FORMAT) |
        (info->texsurface.Type & TYPE_KIND);
    return 1;
}

Sint32 njGetInfoPvr(NJS_TEXINFO *info, Uint32 attr, Sint32 *size)
{
    Sint32 width;
    Sint32 height;
    Sint32 type;
    Sint32 result;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((result = _njLoadPvrHeader(&width, &height, &type, size)) < 0) {
        return result;
    }
    /* @bug logic: under NJD_TEXATTR_AUTOMIPMAP, an ABGR texture of any color
     * format but ARGB_1555 takes no mipmaps, as in njGetSurfacePvr. CMP/EQ
     * R3,R2 at 0x8c15d46a compares the whole format */
    if (type == NJD_TEXFMT_ABGR) {
        if (attr & NJD_TEXATTR_AUTOMIPMAP) {
            type = (type & NJD_TEXFMT_COLOR_MASK) | NJD_TEXFMT_ABGR_MM;
        }
    }
    if (njCheckTextureFormatLow(&info->texsurface, width, height, type) !=
        KMSTATUS_SUCCESS) {
        _nj_texerr_.texerr = NJD_TEXERR_SURFACE;
        return -NJD_TEXERR_SURFACE;
    }
    return 1;
}
