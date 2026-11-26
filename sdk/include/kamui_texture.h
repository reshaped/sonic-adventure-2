/* Kamui's texture memory allocators and texture loads, which Kamui's
 * members and Ninja's call */
#ifndef KAMUI_TEXTURE_H
#define KAMUI_TEXTURE_H
#include <kamui.h>

extern KMBOOLEAN KM_TEXTURE_ONE_BANK;
extern KMDWORD KM_TEXTURE_BANK_SIZE;

KMSTATUS kmiFBreset(void);
KMSTATUS kmiFBallocEx(KMINT32 bytes, PKMSURFACEDESC first,
                      PKMSURFACEDESC second);
KMSTATUS kmiCBalloc(KMINT32 bytes, PKMSURFACEDESC first, PKMSURFACEDESC second);
KMSTATUS kmiMIPalloc(KMINT32 bytes, PKMSURFACEDESC surface);
KMSTATUS kmiFBgetfreememEx(KMINT32 *first, KMINT32 *second);
KMSTATUS kmiFBinit(void);
KMSTATUS kmiTAallocEx(KMINT32 bytes, KMDWORD *first, KMDWORD *second);
KMSTATUS kmiFBfreeEx(KMDWORD first, KMDWORD second);
KMSTATUS kmiTXalloc(KMINT32 bytes, PKMSURFACEDESC surface);
KMSTATUS kmiABSalloc(KMINT32 bytes, PKMSURFACEDESC surface);
KMSTATUS kmiVQalloc(PKMSURFACEDESC surface);
KMSTATUS kmiFBfree(KMINT32 offset);
KMSTATUS kmiVQFBfree(PKMSURFACEDESC surface);
KMSTATUS kmiFBgetfreemem(KMINT32 *first, KMINT32 *second, KMINT32 *vq,
                         KMINT32 *largest);
KMSTATUS kmiFBSortQueue(void);
KMSTATUS kmiFBGarbageCollect(void);

KMSTATUS kmCreateCombinedTextureSurface(PKMSURFACEDESC first,
                                        PKMSURFACEDESC second, KMINT32 width,
                                        KMINT32 height, KMTEXTURETYPE type);
KMSTATUS kmCreateContiguousTextureSurface(PKMSURFACEDESC surfaces,
                                          KMINT32 count, KMINT32 width,
                                          KMINT32 height, KMTEXTURETYPE type);
KMSTATUS kmLoadTextureBlock(PKMSURFACEDESC surface, KMDWORD *texture,
                            KMUINT32 block, KMUINT32 block_size);
KMSTATUS kmLoadRectangleTexturePart(PKMSURFACEDESC surface, KMDWORD *texture,
                                    KMUINT32 width, KMUINT32 height,
                                    KMUINT32 dst_x, KMUINT32 dst_y,
                                    KMUINT32 src_x, KMUINT32 src_y,
                                    KMUINT32 src_width);
KMSTATUS kmLoadVQCodebook(PKMSURFACEDESC surface, KMDWORD *texture);
KMSTATUS kmGetTexture(KMDWORD *texture, PKMSURFACEDESC surface);
#endif
