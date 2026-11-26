/* Ninja's texture palette: its color format, its colors, and the banks
 * palettized textures draw from */
#include <kamui.h>
#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* the address of the palette's 1024 colors, a word each, in the video chip's
 * register space */
#define PALETTE 0x005f9000

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* set tex's palette bank, as njSetPaletteBankNum sets a texture's */
void njSetPaletteBankLN(NJS_TEXMEMLIST *tex, Sint32 bank);
/* NOLINTEND(misc-use-internal-linkage) */

void njSetPaletteBank(Uint32 bank)
{
    _nj_current_ctx_->PaletteBank = bank;
    kmSetVertexRenderState(_nj_current_ctx_);
}

void njSetPaletteBankLN(NJS_TEXMEMLIST *tex, Sint32 bank)
{
    /* @hack spelling: bits reused, first for the texture's type and then for
     * the bank's mask, where the type tested in place and bits for the mask
     * alone would do */
    Sint32 bits = (tex->texinfo.texsurface.Type >> TEXSURFACE_FORMAT_SHIFT) &
                  NJD_TEXFMT_TYPE_MASK;

    switch (bits) {
    case NJD_TEXFMT_PALETTIZE4:
    case NJD_TEXFMT_PALETTIZE4_MM:
        bits = BANK_MASK_4BPP;
        break;
    case NJD_TEXFMT_PALETTIZE8:
    case NJD_TEXFMT_PALETTIZE8_MM:
        bits = BANK_MASK_8BPP;
        break;
    default:
        return;
    }
    bits &= bank;
    tex->texparambuffer &= ~TEX_PALETTE;
    tex->texparambuffer |= bits << TEX_PALETTE_SHIFT;
    tex->bank = bank;
}

void njSetPaletteBankNum(Uint32 n, Sint32 bank)
{
    /* @hack spelling: tex, a local for the record, where the record passed
     * in place would do */
    NJS_TEXMEMLIST *tex;

    tex = (NJS_TEXMEMLIST *)_nj_current_texlist->textures[n].texaddr;
    njSetPaletteBankLN(tex, bank);
}

void njSetPaletteBankNumG(Uint32 globalIndex, Sint32 bank)
{
    Sint32 i;

    i = njSearchTexList(_nj_texmemlist, _nj_texmemsize, globalIndex);
    if (i >= 0) {
        njSetPaletteBankLN(&_nj_texmemlist[i], bank);
    }
}

void njSetPaletteMode(Uint32 mode)
{
    Int km_mode;

    _nj_palette_mode_ = mode;
    switch (mode) {
    /* @hack redundant: an arm for NJD_TEXFMT_ARGB_1555 that does what the
     * default does */
    case NJD_TEXFMT_ARGB_1555:
        km_mode = KM_PALETTE_16BPP_ARGB1555;
        _nj_palette_bpp_ = NJD_TEXPALETTE_16BPP;
        break;
    case NJD_TEXFMT_RGB_565:
        km_mode = KM_PALETTE_16BPP_RGB565;
        _nj_palette_bpp_ = NJD_TEXPALETTE_16BPP;
        break;
    case NJD_TEXFMT_ARGB_4444:
        km_mode = KM_PALETTE_16BPP_ARGB4444;
        _nj_palette_bpp_ = NJD_TEXPALETTE_16BPP;
        break;
    case NJD_TEXFMT_ARGB_8888:
        km_mode = KM_PALETTE_32BPP_ARGB8888;
        _nj_palette_bpp_ = NJD_TEXPALETTE_32BPP;
        break;
    default:
        km_mode = KM_PALETTE_16BPP_ARGB1555;
        _nj_palette_bpp_ = NJD_TEXPALETTE_16BPP;
        break;
    }
    kmSetPaletteMode(km_mode);
}

Uint32 njGetPaletteMode(void)
{
    return _nj_palette_mode_;
}

void njSetPaletteData(Sint32 offset, Sint32 count, void *data)
{
    Sint32 i;
    Uint16 *color;

    if (_nj_palette_bpp_ == NJD_TEXPALETTE_16BPP) {
        color = data;
        for (i = offset; i < offset + count; i++) {
            *(Uint32 *)((PALETTE + (i * sizeof(Uint32))) | P2_BASE) = *color++;
        }
    } else {
        njMemCopy4((void *)((PALETTE + (offset * sizeof(Uint32))) | P2_BASE),
                   data, count);
    }
}

void njGetPaletteData(Sint32 offset, Sint32 count, void *data)
{
    Sint32 i;
    Uint16 *color;

    if (_nj_palette_bpp_ == NJD_TEXPALETTE_16BPP) {
        color = data;
        for (i = offset; i < offset + count; i++) {
            *color++ = *(Uint32 *)(PALETTE + (i * sizeof(Uint32)));
        }
    } else {
        njMemCopy4(data, (void *)(PALETTE + (offset * sizeof(Uint32))), count);
    }
}

Sint32 njLoadPaletteFile(const char *filename, NJS_TEXPALETTE *pl)
{
    Uint32 name;
    struct pvpl_header header;

    njInitTextureErrStat();
    if (njTexFopen(filename) < 0) {
        return -1;
    }
    if (njTexFread(&name, sizeof(name)) < 0) {
        njTexFclose();
        return -1;
    }
    /* @hack spelling: a switch with one case, where if (name != iff_PVPL)
     * with an early return would do */
    switch (name) {
    case iff_PVPL:
        if (njTexFread(&header, sizeof(header)) < 0) {
            njTexFclose();
            return -1;
        }
        njSetPaletteBank(header.bank);
        if (njTexFread(pl->palette, header.size - PVPL_HEADER_REST) < 0) {
            njTexFclose();
            return -1;
        }
        pl->bank = header.bank;
        pl->offset = header.offset;
        pl->count = header.count;
        pl->mode = (Uint8)header.mode;
        break;
    default:
        njTexFclose();
        return -1;
    }
    njTexFclose();
    return 1;
}
