/* the renderer's Kamui vertex context (sg_mwrv2.h) */
#include <kamui.h>
#include <sg_mw.h>
#include <sg_mwrv2.h>
#include <sg_xpt.h>

/* every RenderState bit from KM_DEPTHMODE to KM_DSTSELECT, but KM_COLORCLAMP
 * and KM_PALETTEBANK. it holds 0x4 too, which no KM_ name gives */
#define OPAQUE_STATE                                                           \
    (((KM_DSTSELECT << 1) - 1) & ~(KM_COLORCLAMP | KM_PALETTEBANK))

void mwRnv2InitVertexContextOpaq(KMVERTEXCONTEXT *context)
{
    context->RenderState = OPAQUE_STATE;
    context->ParamType = KM_POLYGON;
    context->ListType = KM_OPAQUE_POLYGON;
    context->ColorType = KM_PACKEDCOLOR;
    context->bUseSpecular = TRUE;
    context->UVFormat = KM_32BITUV;
    context->DepthMode = KM_GREATER;
    context->CullingMode = KM_NOCULLING;
    context->reserved00 = 0;
    context->ShadingMode = KM_TEXTUREGOURAUD;
    context->SelectModifier = KM_NOMODIFIER;
    context->bZWriteDisable = FALSE;
    context->SRCBlendingMode = KM_ONE;
    context->DSTBlendingMode = KM_ZERO;
    context->bSRCSel = FALSE;
    context->bDSTSel = FALSE;
    context->FogMode = KM_NOFOG;
    /* @hack redundant: a second store of TRUE to bUseSpecular */
    context->bUseSpecular = TRUE;
    context->bUseAlpha = TRUE;
    context->FlipUV = KM_NOFLIP;
    context->ClampUV = KM_NOCLAMP;
    context->FilterMode = KM_BILINEAR;
    context->bSuperSample = FALSE;
    context->MipMapAdjust = KM_MIPMAP_D_ADJUST_1_00;
    context->TextureShadingMode = KM_MODULATE_ALPHA;
    context->bColorClamp = FALSE;
    context->PaletteBank = 0;
    context->fFaceColorAlpha = 1.0F;
    context->fFaceColorRed = 1.0F;
    context->fFaceColorGreen = 1.0F;
    context->fFaceColorBlue = 1.0F;
    context->fOffsetColorAlpha = 0.0F;
    context->fOffsetColorRed = 0.0F;
    context->fOffsetColorGreen = 0.0F;
    context->fOffsetColorBlue = 0.0F;
    kmProcessVertexRenderState(context);
}

void mwRnv2InitDisp(MWRNV2 rnv, KMVERTEXCONTEXT *context)
{
    mwRnv2InitVertexContextOpaq(context);
    /* @hack spelling: RenderState built up a bit at a time, where one
     * assignment of KM_SHADINGMODE | KM_CLAMPUV would do */
    context->RenderState = 0;
    context->RenderState |= KM_SHADINGMODE;
    context->ShadingMode = KM_TEXTUREGOURAUD;
    context->RenderState |= KM_CLAMPUV;
    context->ClampUV = KM_CLAMP_UV;
    kmProcessVertexRenderState(context);
}

void mwRnv2SetVctxBlendingMode(MWRNV2 rnv, KMVERTEXCONTEXT *context)
{
    KMSTATUS status;
    Sint32 mode;
    /* @hack redundant: unused, two words that only size the frame */
    Uint32 unused[2];

    mode = rnv->composition;
    context->RenderState = KM_LISTTYPE | KM_TEXTURESHADINGMODE |
                           KM_SRCBLENDINGMODE | KM_DSTBLENDINGMODE;
    if (mode == MWD_RNV2_COMPO_OPEQ) {
        context->ListType = KM_OPAQUE_POLYGON;
        context->TextureShadingMode = KM_MODULATE;
        context->SRCBlendingMode = KM_ONE;
        context->DSTBlendingMode = KM_ZERO;
    } else {
        context->ListType = KM_TRANS_POLYGON;
        context->TextureShadingMode = KM_MODULATE_ALPHA;
        if (mode == MWD_RNV2_COMPO_ADD) {
            context->SRCBlendingMode = KM_SRCALPHA;
            context->DSTBlendingMode = KM_ONE;
        } else if (mode == MWD_RNV2_COMPO_ALPH) {
            context->SRCBlendingMode = KM_SRCALPHA;
            context->DSTBlendingMode = KM_INVSRCALPHA;
        } else if (mode == MWD_RNV2_COMPO_INVALPH) {
            context->SRCBlendingMode = KM_INVSRCALPHA;
            context->DSTBlendingMode = KM_SRCALPHA;
        } else if (mode == MWD_RNV2_COMPO_DSTALPH) {
            context->SRCBlendingMode = KM_DESTALPHA;
            context->DSTBlendingMode = KM_ONE;
        } else if (mode == MWD_RNV2_COMPO_INVDSTALPH) {
            context->SRCBlendingMode = KM_INVDESTALPHA;
            context->DSTBlendingMode = KM_ONE;
        }
    }
    status = kmProcessVertexRenderState(context);
    if (status != KMSTATUS_SUCCESS) {
        mwMngError(
            "mwRnv2SetVertexContextBlendingMode: can't process render state");
    }
}

void mwRnv2SetVctxFilterMode(MWRNV2 rnv, Sint32 filter)
{
    KMVERTEXCONTEXT *context;
    KMSTATUS status;

    context = &rnv->context;
    context->RenderState = KM_FILTERMODE;
    if (filter == TRUE) {
        context->FilterMode = KM_BILINEAR;
    } else {
        context->FilterMode = KM_POINT_SAMPLE;
    }
    status = kmProcessVertexRenderState(context);
    if (status != KMSTATUS_SUCCESS) {
        mwMngError("mwRnv2SetVctxFilterMode: can't process render state");
    }
}

void mwRnv2SetVertexContextPlt8(MWRNV2 rnv, Uint32 bank)
{
    KMVERTEXCONTEXT *context;
    KMSTATUS status;

    context = &rnv->context;
    /* @hack spelling: RenderState built up a bit at a time, where one
     * assignment of KM_SHADINGMODE | KM_PALETTEBANK | KM_CLAMPUV |
     * KM_SRCBLENDINGMODE | KM_DSTBLENDINGMODE | KM_LISTTYPE | KM_USEALPHA
     * would do */
    context->RenderState = 0;
    context->RenderState |= KM_SHADINGMODE;
    context->ShadingMode = KM_TEXTUREGOURAUD;
    context->RenderState |= KM_PALETTEBANK;
    context->PaletteBank = bank;
    context->RenderState |= KM_CLAMPUV;
    context->ClampUV = KM_CLAMP_UV;
    context->RenderState |= KM_SRCBLENDINGMODE;
    context->SRCBlendingMode = KM_SRCALPHA;
    context->RenderState |= KM_DSTBLENDINGMODE;
    context->DSTBlendingMode = KM_INVSRCALPHA;
    context->RenderState |= KM_LISTTYPE;
    context->ListType = KM_TRANS_POLYGON;
    context->RenderState |= KM_USEALPHA;
    context->bUseAlpha = TRUE;
    status = kmProcessVertexRenderState(context);
    if (status != KMSTATUS_SUCCESS) {
        mwMngError("mwRnv2SetVertexContextPlt8: can't process render state");
    }
}
