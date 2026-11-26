/* Kamui's global render settings: fog, the palette, clipping, the background
 * plane and the chip's other render registers */
#include <kamui.h>
#include <kamui_internal.h>
#include <sg_xpt.h>

/* the video chip's registers, as offsets into its register block */
#define BORDER_COLOR 0x040
#define GUN_POSITION 0x0c4
#define PALETTE_FORMAT 0x108
#define PALETTE_RAM 0x1000
/* the bytes from one register of KMI_FOG_TABLE or PALETTE_RAM to the next */
#define REGISTER_BYTES 4
/* KMI_FB_WRITE_CONTROL's alpha threshold, from bit 16 */
#define THRESHOLD_FIELD 0x00ff0000
/* KMI_FB_READ_SIZE's line count, from bit 10 */
#define READ_LINES 0x3ff
#define READ_LINES_SHIFT 10
/* GUN_POSITION's coordinates: x in the low bits, y from bit 16 */
#define GUN_COORDINATE 0x3ff
#define GUN_Y_SHIFT 16
#define MAGNITUDE 0x7fffffff /* a float's bits but its sign */
/* the largest clip coordinate. a clip register holds the least coordinate in
 * its low bits, and the largest from CLIP_MAX_SHIFT */
#define CLIP_LIMIT 0x7ff
#define CLIP_MAX_SHIFT 16
#define BOTH_BANKS 3 /* regions_stale's bits for the two banks */
/* the bits a palette mode takes, from KM_PALETTE_16BPP_ARGB1555 to
 * KM_PALETTE_32BPP_ARGB8888 */
#define PALETTE_MODE_MASK 3
/* the ISP word's fields that kmSetBackGroundPlane keeps in the plane's
 * packet */
#define ISP_FIELDS 0xfff00000
/* fb_write_stride's units, in bytes */
#define STRIDE_BYTES 8
/* the lines kmSetUserClipLevelAdjust skips, in a mode of 480 lines and in one
 * of 240 */
#define SKIP_LINES_480 16
#define SKIP_LINES_240 8
/* a list type's bit that the two modifier lists set */
#define LIST_MODIFIER 1

/* names Kamui exports that no header declares */
/* NOLINTBEGIN(misc-use-internal-linkage) */
KMSTATUS kmSetAlphaThreshold(KMINT32 threshold);
KMSTATUS kmSetFogVertexColor(KMPACKEDARGB color);
KMSTATUS kmSetPaletteData(PKMPALETTEDATA palette);
KMSTATUS kmSetAutoSortMode(KMBOOLEAN enable);
KMSTATUS kmSetGlobalClipping(KMINT32 width, KMINT32 height);
KMSTATUS kmSetUserClipLevelAdjust(KMBOOLEAN enable, KMINT32 *lines);
KMSTATUS kmSetForwardClipping(KMFLOAT depth);
KMSTATUS kmResetRenderer(void);
KMSTATUS kmUseAnotherModifier(KMINT32 list);
KMSTATUS kmGetGunTriggerPos(KMUINT32 *x, KMUINT32 *y);
/* NOLINTEND(misc-use-internal-linkage) */

KMSTATUS kmSetAlphaThreshold(KMINT32 threshold)
{
    KMUINT32 control;

    /* @hack spelling: two tests of threshold's bits, where a clamp to 0 to 255
     * would do */
    if (threshold & 0x80000000) {
        threshold = 0;
    }
    if (threshold & 0x7fffff00) {
        threshold = 255;
    }
    kmiDeviceExtension.alpha_threshold = threshold;
    control = kmiDeviceExtension.fb_write_control & ~THRESHOLD_FIELD;
    /* @bug logic: the threshold goes in at bit 15 (the shift at 0x8c12bfe2),
     * one bit below the field the mask clears (0x8c12bfe4). the chip's
     * threshold is half the one asked, and an odd one sets bit 15, which the
     * mask never clears */
    control |= (threshold & 0xff) << 15;
    kmiDeviceExtension.fb_write_control = control;
    kmiWriteRegister(KMI_FB_WRITE_CONTROL, control);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetCullingRegister(KMFLOAT fCullVal)
{
#ifdef NONMATCHING
    union {
        KMFLOAT value;
        KMUINT32 bits;
    } cull;
    cull.value = fCullVal;
    kmiWriteRegister(KMI_CULL_VALUE, cull.bits & MAGNITUDE);
#else
    /* @hack type: a float's bits read through its address cast to a word
     * pointer, where the union of the NONMATCHING side would do */
    kmiWriteRegister(KMI_CULL_VALUE, *(KMUINT32 *)&fCullVal & MAGNITUDE);
#endif
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetColorClampValue(KMPACKEDARGB MaxVal, KMPACKEDARGB MinVal)
{
    kmiWriteRegister(KMI_FOG_CLAMP_MAX, MaxVal.dwPacked);
    kmiWriteRegister(KMI_FOG_CLAMP_MIN, MinVal.dwPacked);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetFogTableColor(KMPACKEDARGB FogTableColor)
{
    kmiWriteRegister(KMI_FOG_TABLE_COLOR, FogTableColor.dwPacked);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetFogVertexColor(KMPACKEDARGB color)
{
    kmiWriteRegister(KMI_FOG_VERTEX_COLOR, color.dwPacked);
    return KMSTATUS_SUCCESS;
}

/* kamui.h declares pfFogTable a PKMFLOAT, not a pointer to const */
/* NOLINTNEXTLINE(readability-non-const-parameter) */
KMSTATUS kmSetFogTable(PKMFLOAT pfFogTable)
{
    /* fog register i holds entry i in its high byte, and entry i + 1 in its
     * low. the last register holds the last entry twice */
    KMINT32 i = KM_FOG_ENTRY;
    KMUINT32 next;
    KMUINT32 value;
    KMUINT32 pair;

    value = (long)(pfFogTable[i - 1] * 255.0F) & 0xff;
    do {
        --i;
        next = value;
        value = (long)(pfFogTable[i] * 255.0F) & 0xff;
        pair = (next | (value << 8)) & 0xffff;
        kmiWriteRegister(KMI_FOG_TABLE + (i * REGISTER_BYTES), pair);
    } while (i != 0);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetFogDensity(KMDWORD fogDensity)
{
    kmiWriteRegister(KMI_FOG_DENSITY, fogDensity);
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetPaletteMode(KMINT32 Palettemode)
{
    if (Palettemode & ~PALETTE_MODE_MASK) {
        /* @hack redundant: an empty if, with the work in its else */
    } else {
        kmiDeviceExtension.palette_mode = Palettemode;
        kmiWriteRegister(PALETTE_FORMAT, Palettemode);
    }
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetPaletteData(PKMPALETTEDATA palette)
{
    KMINT32 i;

    /* @hack spelling: count, where KM_PALETTE_ENTRY would do */
    KMINT32 count = KM_PALETTE_ENTRY;

    for (i = 0; i < count; i++) {
        kmiWriteRegister(PALETTE_RAM + (i * REGISTER_BYTES),
                         palette->dwPaletteData[i]);
    }
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetPaletteBankData(KMINT32 PaletteEntry, KMINT32 DataSize,
                              PKMPALETTEDATA pPaletteTable)
{
    KMINT32 i;
    KMUINT32 reg;
    KMDWORD *color;
    KMINT32 end;

    DataSize += PaletteEntry;
    /* @bug logic: a range that runs past the palette leaves out its last
     * entry. the clamp at 0x8c12c1aa makes the exclusive end
     * KM_PALETTE_ENTRY - 1 */
    end = DataSize > KM_PALETTE_ENTRY ? KM_PALETTE_ENTRY - 1 : DataSize;
    i = PaletteEntry;
    reg = PALETTE_RAM + (PaletteEntry * REGISTER_BYTES);
    color = &pPaletteTable->dwPaletteData[PaletteEntry];
    /* @hack spelling: a do-while under its own test, where while would do */
    if (i < end) {
        do {
            kmiWriteRegister(reg, *color);
            color++;
            i++;
            reg += REGISTER_BYTES;
        } while (i < end);
    }
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetBorderColor(KMPACKEDARGB BorderColor)
{
    kmiWriteRegister(BORDER_COLOR, BorderColor.dwPacked);
    return KMSTATUS_SUCCESS;
}

#define CHANNEL(component) ((KMDWORD)(long)((component) * 255.0F))
#define PACK_ARGB(alpha, red, green, blue)                                     \
    ((CHANNEL(alpha) << 24) | (CHANNEL(red) << 16) | (CHANNEL(green) << 8) |   \
     CHANNEL(blue))

KMSTATUS kmSetBackGroundRenderState(PKMVERTEXCONTEXT pVertexContext)
{
    kmiDeviceExtension.background_base_color = PACK_ARGB(
        pVertexContext->fFaceColorAlpha, pVertexContext->fFaceColorRed,
        pVertexContext->fFaceColorGreen, pVertexContext->fFaceColorBlue);
    kmiDeviceExtension.background_offset_color = PACK_ARGB(
        pVertexContext->fOffsetColorAlpha, pVertexContext->fOffsetColorRed,
        pVertexContext->fOffsetColorGreen, pVertexContext->fOffsetColorBlue);
    kmiDeviceExtension.background_isp = pVertexContext->ISPPARAMBUFFER;
    kmiDeviceExtension.background_tsp = pVertexContext->TSPPARAMBUFFER;
    kmiDeviceExtension.background_texture = pVertexContext->TexturePARAMBUFFER;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetBackGroundPlane(void *pVertex[3], KMINT32 VertexType,
                              KMINT32 StructSize)
{
    KMSTATUS result = kmiBackGroundPlaneFuncs[VertexType](pVertex);
    kmiDeviceExtension.background_packet[0] &= ISP_FIELDS;
    return result;
}

KMSTATUS kmSetAutoSortMode(KMBOOLEAN enable)
{
    KMSTATUS status;

    if (kmiDeviceExtension.presort == (KMUINT32)enable) {
        kmiDeviceExtension.regions_stale = BOTH_BANKS;
        if (enable == TRUE) {
            kmiDeviceExtension.presort = FALSE;
        } else {
            kmiDeviceExtension.presort = TRUE;
        }
    }
#ifdef NONMATCHING
    /* this side repairs the tag below: the call returns KMSTATUS_SUCCESS */
    status = KMSTATUS_SUCCESS;
#endif
    /* @bug uninitialized: no path sets status. the return at 0x8c12c326
     * gives the caller the stack pointer, in R0 */
    /* NOLINTNEXTLINE(clang-diagnostic-uninitialized) */
    return status;
}

KMSTATUS kmSetGlobalClipping(KMINT32 width, KMINT32 height)
{
    kmiDeviceExtension.global_clip = TRUE;
    kmiDeviceExtension.global_clip_width = width;
    kmiDeviceExtension.global_clip_height = height;
    kmiDeviceExtension.regions_stale = BOTH_BANKS;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetPixelClipping(KMINT32 left, KMINT32 top, KMINT32 right,
                            KMINT32 bottom)
{
    KMUINT32 clip;

    if (left > right || top > bottom) {
        return KMSTATUS_INVALID_PARAMETER;
    }
    if (right > CLIP_LIMIT) {
        right = CLIP_LIMIT;
    }
    if (bottom > CLIP_LIMIT) {
        bottom = CLIP_LIMIT;
    }
    if (kmiDeviceExtension.bpp == KM_DSPBPP_RGB888) {
        left = (left + 1) & ~1;
        top = (top + 1) & ~1;
        right &= ~1;
        bottom &= ~1;
    }
    clip = ((right & CLIP_LIMIT) << CLIP_MAX_SHIFT) | (left & CLIP_LIMIT);
    kmiDeviceExtension.fb_clip_x = clip;
    clip = ((bottom & CLIP_LIMIT) << CLIP_MAX_SHIFT) | (top & CLIP_LIMIT);
    kmiDeviceExtension.fb_clip_y = clip;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetUserClipLevelAdjust(KMBOOLEAN enable, KMINT32 *lines)
{
    KMUINT32 offset;
    KMUINT32 control;
    KMUINT32 height;

    if (kmiDeviceExtension.level_adjust == TRUE) {
        offset = kmiDeviceExtension.skip_lines *
                 kmiDeviceExtension.fb_write_stride * STRIDE_BYTES;
        /* @hack spelling: field_offset[n] = field_offset[n] - offset, where
         * -= would do */
        kmiDeviceExtension.field_offset[0] =
            kmiDeviceExtension.field_offset[0] - offset;
        kmiDeviceExtension.field_offset[1] =
            kmiDeviceExtension.field_offset[1] - offset;
    }
    switch (enable) {
    case FALSE:
        kmiDeviceExtension.skip_lines = 0;
        break;
    case TRUE:
        if (KMI_VGA_MODE == TRUE ||
            (kmiDeviceExtension.display_mode & KMI_HEIGHT_480)) {
            kmiDeviceExtension.skip_lines = SKIP_LINES_480;
        } else {
            kmiDeviceExtension.skip_lines = SKIP_LINES_240;
        }
        offset = kmiDeviceExtension.skip_lines *
                 kmiDeviceExtension.fb_write_stride * STRIDE_BYTES;
        /* @hack spelling: field_offset[n] = field_offset[n] + offset, where
         * += would do */
        kmiDeviceExtension.field_offset[0] =
            kmiDeviceExtension.field_offset[0] + offset;
        kmiDeviceExtension.field_offset[1] =
            kmiDeviceExtension.field_offset[1] + offset;
        break;
    }
    control =
        kmiDeviceExtension.fb_read_size & ~(READ_LINES << READ_LINES_SHIFT);
    height = kmiDeviceExtension.fb_lines;
    if (kmiDeviceExtension.skip_lines < kmiDeviceExtension.offset_y) {
        height -= kmiDeviceExtension.offset_y;
    } else {
        height -= kmiDeviceExtension.skip_lines;
    }
    if (height + kmiDeviceExtension.offset_y > kmiDeviceExtension.fb_lines) {
        height = kmiDeviceExtension.fb_lines - kmiDeviceExtension.skip_lines;
    }
    if (kmiDeviceExtension.fb_lines < height) {
        kmiDeviceExtension.fb_lines = height;
    }
    kmiDeviceExtension.fb_read_size =
        control | ((height & READ_LINES) << READ_LINES_SHIFT);
    kmiWriteRegister(KMI_FB_READ_CONTROL, 0);
    kmiWriteRegister(KMI_FB_READ_SIZE, kmiDeviceExtension.fb_read_size);
    kmiWriteRegister(KMI_FB_READ_CONTROL, kmiDeviceExtension.fb_read_control);
    kmiDeviceExtension.level_adjust = enable;
    if (lines) {
        *lines = kmiDeviceExtension.skip_lines;
    }
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmSetForwardClipping(KMFLOAT depth)
{
    return KMSTATUS_NOT_IMPLEMENTED;
}

KMSTATUS kmResetRenderer(void)
{
    kmiDiscardRenderPipeline();
    kmiResetBoard();
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmUseAnotherModifier(KMINT32 list)
{
    KMSTATUS status = KMSTATUS_INVALID_LIST_TYPE;

    if (list & LIST_MODIFIER) {
        kmiDeviceExtension.another_modifier = TRUE;
        kmiDeviceExtension.another_modifier_list = list;
        status = KMSTATUS_SUCCESS;
    } else {
        kmiDeviceExtension.another_modifier = FALSE;
    }
    kmiDeviceExtension.regions_stale = BOTH_BANKS;
    return status;
}

KMSTATUS kmSetWaitVsyncCount(KMINT32 nCount)
{
    kmiDeviceExtension.vsync_wait = nCount - 1;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmGetGunTriggerPos(KMUINT32 *x, KMUINT32 *y)
{
    KMUINT32 position = kmiReadRegister(GUN_POSITION);
    *x = position & GUN_COORDINATE;
    *y = (position >> GUN_Y_SHIFT) & GUN_COORDINATE;
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmQueryFinishLastTextureDMA(void)
{
    if (kmiDeviceExtension.frame_dma_wait == 1) {
        return KMSTATUS_SUCCESS;
    }
    if (kmiQueryFinishDMA(kmiDeviceExtension.frame_dma) == 1) {
        return KMSTATUS_SUCCESS;
    }
    return KMSTATUS_NOT_FINISH_DMA;
}
