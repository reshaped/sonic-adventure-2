/* the polygon header, the TA's global parameter, that Kamui's list and
 * vertex buffer members share */
#ifndef KAMUI_VERTEX_H
#define KAMUI_VERTEX_H
#include <kamui_buffer.h>

struct kmi_global_header {
    KMDWORD word[16];
};
extern struct kmi_global_header kmiGlobalWork[1];
extern KMDWORD kmiGlobalNum;
extern KMDWORD kmiCurrentListType;

/* how many words list ListType holds: its write pointer less the start of
 * its buffer */
KMDWORD kmGetCurrentVertexOffset(KMINT32 ListType);
KMDWORD kmiConvertUVnum(KMDWORD dimension);
void kmiCreateGlobalParameter(void);
KMDWORD **kmiPutGlobalParam(void);
KMSTATUS kmSetModifierRenderState(PKMVERTEXCONTEXT context);
KMSTATUS kmSetStripLength(KMDWORD length);
KMSTATUS kmChangeContextColorType(KMDWORD color);
KMSTATUS kmChangeContextDepthMode(KMINT32 compare);
KMSTATUS kmChangeContextCullingMode(KMINT32 culling);
KMSTATUS kmChangeContextZWriteDisable(KMBOOLEAN disable);
KMSTATUS kmChangeContextSRCBlendMode(KMINT32 mode);
KMSTATUS kmChangeContextDSTBlendMode(KMINT32 mode);
KMSTATUS kmChangeContextFogMode(KMDWORD fog);
KMSTATUS kmChangeContextFlipUV(KMINT32 flip);
KMSTATUS kmChangeContextClampUV(KMINT32 clamp);
KMSTATUS kmChangeContextFilterMode(KMINT32 filter);
KMSTATUS kmChangeContextSuperSample(KMBOOLEAN enable);
KMSTATUS kmChangeContextTextureShadingMode(KMINT32 shading);
KMSTATUS kmChangeContextColorClamp(KMBOOLEAN enable);
KMSTATUS kmChangeContextPaletteBank(KMDWORD bank);
KMSTATUS kmChangeContextStripLength(KMDWORD length);
KMSTATUS kmChangeContextUserClipMode(KMDWORD clip);

#endif
