/* Kamui, the graphics library under Ninja, as far as the sources use it */
#ifndef KAMUI_H
#define KAMUI_H

typedef unsigned int KMSTATUS;
#define KMSTATUS_SUCCESS 0
#define KMSTATUS_INVALID_DISPLAY_MODE 1
#define KMSTATUS_INVALID_VIDEO_MODE 2
#define KMSTATUS_NOT_ENOUGH_MEMORY 3 /* the texture memory is full */
#define KMSTATUS_INVALID_TEXTURE_TYPE 4 /* a texture the chip can't take */
#define KMSTATUS_INVALID_ADDRESS 7 /* an offset past a texture's end */
#define KMSTATUS_HARDWARE_NOT_PRESENTED 8
#define KMSTATUS_INVALID_VERTEX_TYPE 10
#define KMSTATUS_INVALID_LIST_TYPE 11
#define KMSTATUS_INVALID_BLOCKNUMBER 12
#define KMSTATUS_LAST_LIST_UNDER_DMA 13
#define KMSTATUS_INVALID_SURFACE 14
#define KMSTATUS_INVALID_SURFACE_TYPE 15
#define KMSTATUS_INVALID_SURFACE_SIZE 16
#define KMSTATUS_NOT_FINISH_DMA 17
#define KMSTATUS_NOT_IMPLEMENTED 120
#define KMSTATUS_INVALID_PARAMETER 0xdddd
typedef int KMINT32;
typedef int KMBOOLEAN;
typedef unsigned int KMUINT32;
typedef unsigned long KMDWORD;
typedef float KMFLOAT;
typedef KMFLOAT *PKMFLOAT;
typedef unsigned char KMBYTE;

/* Textures and frame buffers use the ten-word layout of NJS_TEXSURFACE.
 * pSurface is a byte offset in video memory. */
typedef struct tagKMSURFACEDESC {
    KMDWORD SurfaceType;
    KMDWORD BitDepth;
    KMDWORD PixelFormat;
    union {
        KMDWORD USize;
        KMDWORD Width;
    } u0;
    union {
        KMDWORD VSize;
        KMDWORD Height;
    } u1;
    union {
        KMDWORD dwTextureSize;
        KMDWORD dwFrameBufferSize;
    } uSize;
    KMDWORD fSurfaceFlags;
    KMDWORD *pSurface;
    KMDWORD *pVirtual;
    KMDWORD *pPhysical;
} KMSURFACEDESC, *PKMSURFACEDESC;

/* a vertex context: the state polygons draw with */
typedef struct tagKMVERTEXCONTEXT {
    KMDWORD RenderState; /* the fields kmProcessVertexRenderState is to pack */
    KMINT32 ParamType;
    KMINT32 ListType;
    KMINT32 ColorType;
    KMINT32 UVFormat;
    KMINT32 DepthMode;
    KMINT32 CullingMode;
    KMDWORD reserved00;
    KMINT32 ShadingMode;
    KMINT32 SelectModifier;
    KMBOOLEAN bZWriteDisable;
    KMINT32 SRCBlendingMode;
    KMINT32 DSTBlendingMode;
    KMBOOLEAN bSRCSel;
    KMBOOLEAN bDSTSel;
    KMINT32 FogMode;
    KMBOOLEAN bUseSpecular;
    KMBOOLEAN bUseAlpha;
    KMBOOLEAN bIgnoreTextureAlpha;
    KMINT32 ClampUV;
    KMINT32 FlipUV;
    KMINT32 FilterMode;
    KMBOOLEAN bSuperSample;
    KMDWORD MipMapAdjust;
    KMINT32 TextureShadingMode;
    KMBOOLEAN bColorClamp;
    KMDWORD PaletteBank;
    PKMSURFACEDESC pTextureSurfaceDesc;
    KMFLOAT fFaceColorAlpha;
    KMFLOAT fFaceColorRed;
    KMFLOAT fFaceColorGreen;
    KMFLOAT fFaceColorBlue;
    KMFLOAT fOffsetColorAlpha;
    KMFLOAT fOffsetColorRed;
    KMFLOAT fOffsetColorGreen;
    KMFLOAT fOffsetColorBlue;
    KMDWORD GLOBALPARAMBUFFER;
    KMDWORD ISPPARAMBUFFER;
    KMDWORD TSPPARAMBUFFER;
    KMDWORD TexturePARAMBUFFER;
    KMDWORD ModifierInstruction;
    KMFLOAT reserved01;
    KMFLOAT reserved02;
    KMFLOAT reserved03;
    KMFLOAT reserved04;
    KMBOOLEAN bDCalcExact;
    KMDWORD StripLength;
    KMINT32 UserClipMode;
} KMVERTEXCONTEXT, *PKMVERTEXCONTEXT;

/* RenderState's bits, each named for the field it marks */
#define KM_DEPTHMODE 0x00000001
#define KM_CULLINGMODE 0x00000002
#define KM_SHADINGMODE 0x00000008
#define KM_MODIFIER 0x00000010
#define KM_ZWRITEDISABLE 0x00000020
#define KM_SRCBLENDINGMODE 0x00000040
#define KM_DSTBLENDINGMODE 0x00000080
#define KM_FOGMODE 0x00000100
#define KM_USESPECULAR 0x00000200
#define KM_USEALPHA 0x00000400
#define KM_IGNORETEXTUREALPHA 0x00000800
#define KM_CLAMPUV 0x00001000
#define KM_FLIPUV 0x00002000
#define KM_FILTERMODE 0x00004000
#define KM_SUPERSAMPLE 0x00008000
#define KM_MIPMAPDADJUST 0x00010000
#define KM_TEXTURESHADINGMODE 0x00020000
#define KM_COLORCLAMP 0x00040000
#define KM_PALETTEBANK 0x00080000
#define KM_PARAMTYPE 0x00100000
#define KM_LISTTYPE 0x00200000
#define KM_COLORTYPE 0x00400000
#define KM_UVFORMAT 0x00800000
#define KM_SRCSELECT 0x01000000
#define KM_DSTSELECT 0x02000000
#define KM_DCALCEXACT 0x04000000
#define KM_STRIPLENGTH 0x08000000
#define KM_USERCLIPMODE 0x10000000
/* ParamType's values */
#define KM_POLYGON 0
#define KM_MODIFIERVOLUME 1
#define KM_SPRITE 2 /* a quad */
/* ListType's values: the lists, by number. KMI_LISTS counts them, and each
 * per-list array below takes them in this order */
#define KM_OPAQUE_POLYGON 0
#define KM_OPAQUE_MODIFIER 1
#define KM_TRANS_POLYGON 2
#define KM_TRANS_MODIFIER 3
#define KM_PUNCHTHROUGH_POLYGON 4
#define KMI_LISTS 5
/* ColorType's values */
#define KM_PACKEDCOLOR 0 /* a packed ARGB word */
#define KM_FLOATINGCOLOR 1 /* four floats */
/* UVFormat's values */
#define KM_32BITUV 0 /* two floats */
#define KM_16BITUV 1 /* a 16-bit u and a 16-bit v, packed in one word */
/* DepthMode's values */
#define KM_GREATER 4
#define KM_GREATEREQUAL 6 /* draw where 1/w is at least the depth buffer's */
#define KM_ALWAYS 7 /* draw whatever the depth */
/* CullingMode's values, by what each culls */
#define KM_NOCULLING 0
#define KM_CULLSMALL 1 /* polygons by their size alone */
#define KM_CULLCCW 2 /* polygons that wind counterclockwise */
#define KM_CULLCW 3 /* polygons that wind clockwise */
/* ShadingMode's values */
#define KM_NOTEXTUREGOURAUD 1
#define KM_TEXTUREFLAT 2
#define KM_TEXTUREGOURAUD 3
/* SelectModifier's none */
#define KM_NOMODIFIER 0
/* the blend factors SRCBlendingMode and DSTBlendingMode take */
#define KM_BOTHINVSRCALPHA 0
#define KM_BOTHSRCALPHA 1
#define KM_DESTALPHA 2
#define KM_DESTCOLOR 3
#define KM_INVDESTALPHA 4
#define KM_INVDESTCOLOR 5
#define KM_INVSRCALPHA 6
#define KM_INVSRCCOLOR 7
#define KM_SRCALPHA 8
#define KM_SRCCOLOR 9
#define KM_ONE 10
#define KM_ZERO 11
/* FogMode's values */
#define KM_FOGTABLE 0 /* fog from the fog table */
#define KM_NOFOG 2
/* ClampUV's and FlipUV's values */
#define KM_NOCLAMP 0
#define KM_CLAMP_V 1
#define KM_CLAMP_U 2
#define KM_CLAMP_UV 3
#define KM_NOFLIP 0
#define KM_FLIP_V 1
#define KM_FLIP_U 2
#define KM_FLIP_UV 3
/* FilterMode's values. KM_TRILINEAR_A is a trilinear filter's first pass.
 * kmProcessVertexRenderState indexes the table at 0x8c17c5c4 by FilterMode,
 * and puts the entry in the TSP word's filter field. the table holds 0, 2, 1
 * and 3. so KM_BILINEAR gives the field 1, and KM_TRILINEAR_A gives it 2 */
#define KM_POINT_SAMPLE 0
#define KM_TRILINEAR_A 1
#define KM_BILINEAR 2
/* MipMapAdjust's D adjust of 1.00, in quarters */
#define KM_MIPMAP_D_ADJUST_1_00 4
/* TextureShadingMode's values: how the texture combines with the polygon's
 * color */
#define KM_DECAL 0
#define KM_MODULATE 1
#define KM_DECAL_ALPHA 2
#define KM_MODULATE_ALPHA 3
/* pack the fields the context's RenderState marks into its
 * GLOBALPARAMBUFFER, ISPPARAMBUFFER, TSPPARAMBUFFER and TexturePARAMBUFFER,
 * which kmSetVertexRenderState reads. it returns KMSTATUS_SUCCESS */
KMSTATUS kmProcessVertexRenderState(PKMVERTEXCONTEXT pVertexContext);
/* make the context's state the one that polygons draw with from now on */
KMSTATUS kmSetVertexRenderState(PKMVERTEXCONTEXT pVertexContext);
/* set the background plane's state from the context */
KMSTATUS kmSetBackGroundRenderState(PKMVERTEXCONTEXT pVertexContext);
/* a vertex of type KM_VERTEXTYPE_00: a screen point, its 1/w in fZ, and a
 * packed color */
typedef struct {
    KMDWORD ParamControlWord;
    KMFLOAT fX;
    KMFLOAT fY;
    KMFLOAT fZ;
    KMDWORD Reserved0;
    KMDWORD Reserved1;
    KMDWORD dwBaseRGB;
    KMDWORD Reserved2;
} KMVERTEX0;
#define KM_VERTEXTYPE_00 0
#define KM_VERTEXTYPE_03 3 /* packed base and offset colors with 32-bit UV */
/* a sprite, vertex type 16: four corners A to D, of which D has no z. each of
 * dwUVA, dwUVB and dwUVC packs the texture's u and v at its corner */
typedef struct {
    KMDWORD ParamControlWord;
    KMFLOAT fAX;
    KMFLOAT fAY;
    KMFLOAT fAZ;
    KMFLOAT fBX;
    KMFLOAT fBY;
    KMFLOAT fBZ;
    KMFLOAT fCX;
    KMFLOAT fCY;
    KMFLOAT fCZ;
    KMFLOAT fDX;
    KMFLOAT fDY;
    KMDWORD Reserved0;
    KMDWORD dwUVA;
    KMDWORD dwUVB;
    KMDWORD dwUVC;
} KMVERTEX16;
/* make the three vertices that pVertex points to the background plane's
 * corners. each is a vertex of VertexType, StructSize bytes long */
KMSTATUS kmSetBackGroundPlane(void *pVertex[3], KMINT32 VertexType,
                              KMINT32 StructSize);

/* a color as one word, or as its four bytes */
typedef union {
    KMDWORD dwPacked;
    struct {
        KMBYTE bBlue;
        KMBYTE bGreen;
        KMBYTE bRed;
        KMBYTE bAlpha;
    } byte;
} KMPACKEDARGB;

/* Tile accelerator vertices. Types 9 through 14 hold two sets of shading
 * attributes. The background plane reads the first volume (0m). */
typedef struct {
    KMDWORD ParamControlWord;
    KMFLOAT fX, fY, fZ;
    KMFLOAT fBaseAlpha, fBaseRed, fBaseGreen, fBaseBlue;
} KMVERTEX1;
typedef struct {
    KMDWORD ParamControlWord;
    KMFLOAT fX, fY, fZ;
    KMDWORD Reserved0, Reserved1;
    KMFLOAT fBaseIntensity;
    KMDWORD Reserved2;
} KMVERTEX2;
typedef struct {
    KMDWORD ParamControlWord;
    KMFLOAT fX, fY, fZ;
    KMFLOAT fU, fV;
    KMPACKEDARGB uBaseRGB, uOffsetRGB;
} KMVERTEX3;
typedef struct {
    KMDWORD ParamControlWord;
    KMFLOAT fX, fY, fZ;
    KMDWORD dwUV, Reserved0;
    KMPACKEDARGB uBaseRGB, uOffsetRGB;
} KMVERTEX4;
typedef struct {
    KMDWORD ParamControlWord;
    KMFLOAT fX, fY, fZ;
    KMFLOAT fU, fV;
    KMDWORD Reserved0, Reserved1;
    KMFLOAT fBaseAlpha, fBaseRed, fBaseGreen, fBaseBlue;
    KMFLOAT fOffsetAlpha, fOffsetRed, fOffsetGreen, fOffsetBlue;
} KMVERTEX5;
typedef struct {
    KMDWORD ParamControlWord;
    KMFLOAT fX, fY, fZ;
    KMDWORD dwUV, Reserved0, Reserved1, Reserved2;
    KMFLOAT fBaseAlpha, fBaseRed, fBaseGreen, fBaseBlue;
    KMFLOAT fOffsetAlpha, fOffsetRed, fOffsetGreen, fOffsetBlue;
} KMVERTEX6;
typedef struct {
    KMDWORD ParamControlWord;
    KMFLOAT fX, fY, fZ;
    KMFLOAT fU, fV;
    KMFLOAT fBaseIntensity, fOffsetIntensity;
} KMVERTEX7;
typedef struct {
    KMDWORD ParamControlWord;
    KMFLOAT fX, fY, fZ;
    KMDWORD dwUV, Reserved0;
    KMFLOAT fBaseIntensity, fOffsetIntensity;
} KMVERTEX8;
typedef struct {
    KMDWORD ParamControlWord;
    KMFLOAT fX, fY, fZ;
    KMPACKEDARGB uBaseRGB0m, uBaseRGB1m;
    KMDWORD Reserved0, Reserved1;
} KMVERTEX9;
typedef struct {
    KMDWORD ParamControlWord;
    KMFLOAT fX, fY, fZ;
    KMFLOAT fBaseIntensity0m, fBaseIntensity1m;
    KMDWORD Reserved0, Reserved1;
} KMVERTEX10;
typedef struct {
    KMDWORD ParamControlWord;
    KMFLOAT fX, fY, fZ;
    KMFLOAT fU0m, fV0m;
    KMPACKEDARGB uBaseRGB0m, uOffsetRGB0m;
    KMFLOAT fU1m, fV1m;
    KMPACKEDARGB uBaseRGB1m, uOffsetRGB1m;
    KMDWORD Reserved0, Reserved1, Reserved2, Reserved3;
} KMVERTEX11;
typedef struct {
    KMDWORD ParamControlWord;
    KMFLOAT fX, fY, fZ;
    KMDWORD dwUV0m, Reserved0;
    KMPACKEDARGB uBaseRGB0m, uOffsetRGB0m;
    KMDWORD dwUV1m, Reserved1;
    KMPACKEDARGB uBaseRGB1m, uOffsetRGB1m;
    KMDWORD Reserved2, Reserved3, Reserved4, Reserved5;
} KMVERTEX12;
typedef struct {
    KMDWORD ParamControlWord;
    KMFLOAT fX, fY, fZ;
    KMFLOAT fU0m, fV0m;
    KMFLOAT fBaseIntensity0m, fOffsetIntensity0m;
    KMFLOAT fU1m, fV1m;
    KMFLOAT fBaseIntensity1m, fOffsetIntensity1m;
    KMDWORD Reserved0, Reserved1, Reserved2, Reserved3;
} KMVERTEX13;
typedef struct {
    KMDWORD ParamControlWord;
    KMFLOAT fX, fY, fZ;
    KMDWORD dwUV0m, Reserved0;
    KMFLOAT fBaseIntensity0m, fOffsetIntensity0m;
    KMDWORD dwUV1m, Reserved1;
    KMFLOAT fBaseIntensity1m, fOffsetIntensity1m;
    KMDWORD Reserved2, Reserved3, Reserved4, Reserved5;
} KMVERTEX14;

/* set the fog table's color. it returns KMSTATUS_SUCCESS */
KMSTATUS kmSetFogTableColor(KMPACKEDARGB FogTableColor);
/* the number of entries in the fog table */
#define KM_FOG_ENTRY 128
/* set the fog table from the KM_FOG_ENTRY floats at pfFogTable. each must
 * lie from 0 to 1. the table holds each as a byte, the float times 255
 * truncated. it returns KMSTATUS_SUCCESS */
KMSTATUS kmSetFogTable(PKMFLOAT pfFogTable);
/* write fogDensity to the fog density register unconverted. it returns
 * KMSTATUS_SUCCESS */
KMSTATUS kmSetFogDensity(KMDWORD fogDensity);
/* kmSetColorClampValue writes MaxVal and MinVal to the color clamp's top and
 * bottom registers. kmSetBorderColor writes BorderColor to the border color
 * register. each returns KMSTATUS_SUCCESS */
KMSTATUS kmSetColorClampValue(KMPACKEDARGB MaxVal, KMPACKEDARGB MinVal);
KMSTATUS kmSetBorderColor(KMPACKEDARGB BorderColor);

/* palette color formats, for kmSetPaletteMode */
#define KM_PALETTE_16BPP_ARGB1555 0
#define KM_PALETTE_16BPP_RGB565 1
#define KM_PALETTE_16BPP_ARGB4444 2
#define KM_PALETTE_32BPP_ARGB8888 3
#define KM_PALETTE_ENTRY 1024
typedef union {
    KMDWORD wPaletteData[KM_PALETTE_ENTRY];
    KMDWORD dwPaletteData[KM_PALETTE_ENTRY];
} KMPALETTEDATA, *PKMPALETTEDATA;
/* make Palettemode, a KM_PALETTE_ format, the palette's color format. any
 * other value changes nothing. it returns KMSTATUS_SUCCESS */
KMSTATUS kmSetPaletteMode(KMINT32 Palettemode);
KMSTATUS kmSetPaletteBankData(KMINT32 PaletteEntry, KMINT32 DataSize,
                              PKMPALETTEDATA pPaletteTable);

/* a vertex's first word: whether its strip goes on, or ends with it */
#define KM_VERTEXPARAM_NORMAL 0xe0000000
#define KM_VERTEXPARAM_ENDOFSTRIP 0xf0000000

/* a vertex buffer's current list */
typedef struct {
    KMDWORD ListType;
    KMDWORD GlobalParamSize; /* a strip's global parameter: 32 or 64 bytes */
    KMDWORD Reserved0; /* the VertexType of the last kmSetVertex call */
    KMDWORD VertexBank;
} KMCURRENTLISTSTATE, *PKMCURRENTLISTSTATE;
typedef struct {
    KMDWORD **pCurrentPtr; /* where each list writes next, by list number */
    KMDWORD *pGlobalParam; /* the next strip's global parameter */
    PKMCURRENTLISTSTATE pCurrentListState;
    struct kmi_list_bases **ppBuffer;
    KMDWORD Reserved0; /* enabled-list mask for a direct TA buffer */
    KMDWORD Reserved1;
    KMDWORD Reserved2;
    KMDWORD Reserved3;
} KMVERTEXBUFFDESC;
typedef KMVERTEXBUFFDESC *PKMVERTEXBUFFDESC;
/* kmStartVertexStrip opens a strip in pBufferDesc's current list with the
 * buffer's global parameter. kmSetVertex adds the StructSize bytes at pVertex
 * to that list. it records VertexType in kmiCurrentListState, Kamui's own list
 * state. StructSize must be 32 or 64. both write through the store queues,
 * and return KMSTATUS_SUCCESS */
KMSTATUS kmStartVertexStrip(PKMVERTEXBUFFDESC pBufferDesc);
KMSTATUS kmSetVertex(PKMVERTEXBUFFDESC pBufferDesc, void *pVertex,
                     KMINT32 VertexType, KMINT32 StructSize);
/* send pBufferDesc's list ListType a user clip of the tiles from (Xmin, Ymin)
 * to (Xmax, Ymax). a tile is 32 pixels a side. unless the list holds only its
 * opening header, a blank header and two off-screen sprites precede the clip.
 * the blank header takes the group bit and user clip mode of the
 * buffer's global parameter. in a modifier list it heads a modifier volume,
 * and in any other list a sprite. the call returns KMSTATUS_SUCCESS */
KMSTATUS kmSetUserClipping(PKMVERTEXBUFFDESC pBufferDesc, KMINT32 ListType,
                           KMINT32 Xmin, KMINT32 Ymin, KMINT32 Xmax,
                           KMINT32 Ymax);
/* give up the vertex buffer that pBufferDesc describes. its memory stays
 * the caller's to free */
KMSTATUS kmDiscardVertexBuffer(PKMVERTEXBUFFDESC pBufferDesc);
/* the setup kmSetSystemConfiguration takes */
typedef struct {
    KMDWORD dwSize;
    KMDWORD flags; /* KM_CONFIGFLAG_ bits */
    PKMSURFACEDESC *ppSurfaceDescArray; /* the two frame buffers' surfaces */
    KMDWORD nNumOfFrameBuffer;
    KMDWORD nWidth;
    KMDWORD nHeight;
    KMINT32 nBpp;
    KMDWORD nTextureMemorySize;
    PKMVERTEXBUFFDESC pBufferDesc;
    KMDWORD *pVertexBuffer;
    KMDWORD nVertexBufferSize; /* pVertexBuffer's size, in words */
    KMFLOAT fBufferSize[KMI_LISTS]; /* each list's percent of the buffer */
    KMDWORD reserved01;
    KMDWORD reserved02;
    KMDWORD reserved03;
    KMDWORD reserved04;
    KMDWORD reserved05;
    KMDWORD reserved06;
    KMDWORD reserved07;
    KMINT32 nVbufModel;
} KMSYSTEMCONFIGSTRUCT, *PKMSYSTEMCONFIGSTRUCT;
#define KM_CONFIGFLAG_ENABLE_CLEAR_FRAMEBUFFER 0x00000001
#define KM_CONFIGFLAG_ENABLE_STRIPBUFFER 0x00000002
#define KM_CONFIGFLAG_ENABLE_2V_LATENCY 0x00000010
#define KM_CONFIGFLAG_NOWAITVSYNC 0x00000020
#define KM_CONFIGFLAG_USEDIRECTMODE 0x00010000
#define KM_CONFIGFLAG_NOWAIT_FINISH_TEXTUREDMA 0x00100000
/* nVbufModel's values. KM_VERTEXBUFMODEL_NORMAL puts every list through the
 * vertex buffer. under KM_CONFIGFLAG_ENABLE_2V_LATENCY, each other value
 * leaves the list it names with no buffer */
#define KM_VERTEXBUFMODEL_NORMAL 0x00
#define KM_VERTEXBUFMODEL_NOBUF_OPAQUE_POLYGON 0x20
#define KM_VERTEXBUFMODEL_NOBUF_TRANS_POLYGON 0x22
#define KM_VERTEXBUFMODEL_NOBUF_PUNCH_THROUGH 0x24
/* Two calls supply the front and back surfaces. A strip buffer rounds its
 * height up to a whole group of 32 lines. */
KMSTATUS kmCreateFrameBufferSurface(PKMSURFACEDESC surface, KMINT32 width,
                                    KMINT32 height, KMBOOLEAN strip_buffer,
                                    KMBOOLEAN clear);
/* set up the frame buffers, the texture memory and the vertex buffer that
 * setup describes */
KMSTATUS kmSetSystemConfiguration(PKMSYSTEMCONFIGSTRUCT setup);
/* a global parameter's type, in the top three bits of its first word */
#define KM_PARAM_MODIFIERVOLUME 4
#define KM_PARAM_SPRITE 5
/* a modifier volume parameter's second word. KM_MODIFIER_INCLUDE_LAST_POLY
 * and KM_MODIFIER_EXCLUDE_LAST_POLY mark the triangle after the parameter as
 * the volume's last */
#define KM_MODIFIER_NORMAL_POLY 0
#define KM_MODIFIER_INCLUDE_LAST_POLY (1 << 29)
#define KM_MODIFIER_EXCLUDE_LAST_POLY (2 << 29)

/* write fCullVal, with its sign bit cleared, to the hardware's culling
 * register. it returns KMSTATUS_SUCCESS */
KMSTATUS kmSetCullingRegister(KMFLOAT fCullVal);
/* set the intensity of the cheap shadow, the shade a modifier volume casts */
KMSTATUS kmSetCheapShadowMode(KMINT32 nIntensity);

/* a callback, which Kamui passes the argument registered with it. the PAL
 * stretch callback gets a KMPALEXTINFO pointer instead */
typedef void (*PKMCALLBACKFUNC)(void *pArgs);

/* each of these five calls registers callback for its event, to be called
 * with argument. a null callback restores the event's default handler, which
 * does nothing. the events, in the calls' order, are the vertical sync, the
 * horizontal sync, the end of a render (EOR), the end of the vertices, and a
 * wait on the vertical sync. each call returns KMSTATUS_SUCCESS */
KMSTATUS kmSetVSyncCallback(PKMCALLBACKFUNC callback, void *argument);
KMSTATUS kmSetHSyncCallback(PKMCALLBACKFUNC callback, void *argument);
KMSTATUS kmSetEORCallback(PKMCALLBACKFUNC callback, void *argument);
KMSTATUS kmSetEndOfVertexCallback(PKMCALLBACKFUNC callback, void *argument);
KMSTATUS kmSetWaitVsyncCallback(PKMCALLBACKFUNC callback, void *argument);
/* register the callback that sets a PAL display's stretch. Kamui passes it a
 * pointer to a KMPALEXTINFO, and ignores Reserved. it returns
 * KMSTATUS_SUCCESS */
KMSTATUS kmSetPALEXTCallback(PKMCALLBACKFUNC callback, void *Reserved);
/* what the PAL stretch callback's argument points to */
typedef struct {
    KMINT32 nDisplayMode; /* the display mode being set */
    KMINT32 nPALExtMode; /* the stretch, which the callback sets */
} KMPALEXTINFO, *PKMPALEXTINFO;

/* a surface's kind, in the low half of its Type. a small VQ texture's
 * memory comes from its own allocator */
#define KM_SURFACETYPE_FRAMEBUFFER 1
#define KM_SURFACETYPE_TEXTURE 2
#define KM_SURFACETYPE_SMALLVQ_TEXTURE 3
/* a surface's BitDepth for 16-bit texels */
#define KM_BITDEPTH_16 1
#define KM_BITDEPTH_32 3
#define KM_SURFACEFLAGS_MIPMAPED 0x01
#define KM_SURFACEFLAGS_TWIDDLED 0x04
#define KM_SURFACEFLAGS_VQ 0x08
#define KM_SURFACEFLAGS_STRIDE 0x10
#define KM_SURFACEFLAGS_PALETTIZED 0x20
/* a surface's PixelFormat, as a polygon's texture word holds it */
#define KM_PIXELFORMAT_ARGB1555 0x00000000
#define KM_PIXELFORMAT_RGB565 0x08000000
#define KM_PIXELFORMAT_ARGB4444 0x10000000
#define KM_PIXELFORMAT_YUV422 0x18000000
#define KM_PIXELFORMAT_BUMP 0x20000000
#define KM_PIXELFORMAT_MASK 0x38000000
#define KM_PIXELFORMAT_PALETTIZED_4BPP 0x28000000
#define KM_PIXELFORMAT_PALETTIZED_8BPP 0x30000000
/* free the texture memory that pSurfaceDesc's surface holds */
KMSTATUS kmFreeTexture(PKMSURFACEDESC pSurfaceDesc);
/* a texture's format, as a surface's Type takes it */
typedef KMUINT32 KMTEXTURETYPE;
#define KM_TEXTURE_ARGB1555 0x00
#define KM_TEXTURE_RGB565 0x01
#define KM_TEXTURE_ARGB4444 0x02
#define KM_TEXTURE_YUV422 0x03
#define KM_TEXTURE_BUMP 0x04
#define KM_TEXTURE_YUV420 0x06
#define KM_TEXTURE_TWIDDLED 0x0100
#define KM_TEXTURE_TWIDDLED_MM 0x0200
#define KM_TEXTURE_VQ 0x0300
#define KM_TEXTURE_VQ_MM 0x0400
#define KM_TEXTURE_PALETTIZE4 0x0500
#define KM_TEXTURE_PALETTIZE4_MM 0x0600
#define KM_TEXTURE_PALETTIZE8 0x0700
#define KM_TEXTURE_PALETTIZE8_MM 0x0800
#define KM_TEXTURE_RECTANGLE 0x0900
#define KM_TEXTURE_STRIDE 0x0b00
#define KM_TEXTURE_TWIDDLED_RECTANGLE 0x0d00
#define KM_TEXTURE_BMP 0x0e00
#define KM_TEXTURE_BMP_MM 0x0f00
#define KM_TEXTURE_SMALLVQ 0x1000
#define KM_TEXTURE_SMALLVQ_MM 0x1100
KMSTATUS kmSetStrideWidth(KMINT32 nWidth);
/* describe pSurfaceDesc as an nWidth by nHeight texture of nTextureType, and
 * allocate its texture memory. a description that fails returns its status,
 * and allocates nothing. where the memory runs out, the call runs the
 * callback that kmSetTexOverflowCallback sets, then returns
 * KMSTATUS_NOT_ENOUGH_MEMORY */
KMSTATUS kmCreateTextureSurface(PKMSURFACEDESC pSurfaceDesc, KMINT32 nWidth,
                                KMINT32 nHeight, KMTEXTURETYPE nTextureType);
/* copy the texture at pTexture into pSurfaceDesc's texture memory. it returns
 * KMSTATUS_SUCCESS. fAutoMipMap and fDither must be FALSE: where either is
 * TRUE, it copies nothing and returns KMSTATUS_INVALID_TEXTURE_TYPE */
KMSTATUS kmLoadTexture(PKMSURFACEDESC pSurfaceDesc, KMDWORD *pTexture,
                       KMBOOLEAN fAutoMipMap, KMBOOLEAN fDither);
KMSTATUS kmLoadYUVTexture(PKMSURFACEDESC pSurfaceDesc, KMDWORD *pTexture,
                          KMINT32 nWidth, KMINT32 nHeight, KMINT32 nFormat,
                          KMBOOLEAN bWaitEndOfDMA);
/* store the free texture memory in *pSizeOfTexture, and the largest free
 * block in *pMaxBlockSizeOfTexture */
KMSTATUS kmGetFreeTextureMem(KMUINT32 *pSizeOfTexture,
                             KMUINT32 *pMaxBlockSizeOfTexture);
/* gather the free texture memory into one block */
KMSTATUS kmGarbageCollectTexture(void);
/* KMSTATUS_SUCCESS once the last transfer to texture memory has ended, and
 * another status while it runs */
KMSTATUS kmQueryFinishLastTextureDMA(void);
/* copy nSize bytes of the texture at pTexture into pSurfaceDesc's texture
 * memory, from nOffset on in the texture. it returns
 * KMSTATUS_INVALID_TEXTURE_TYPE for a small VQ surface */
KMSTATUS kmLoadTexturePart(PKMSURFACEDESC pSurfaceDesc, KMDWORD *pTexture,
                           KMUINT32 nOffset, KMUINT32 nSize);
/* load the mipmaps of pSurfaceDesc's texture again, from the texture at
 * pTexture. nMipmapCount says which. it returns
 * KMSTATUS_INVALID_TEXTURE_TYPE for a surface whose kind it doesn't
 * reload */
KMSTATUS kmReLoadMipmap(PKMSURFACEDESC pSurfaceDesc, void *pTexture,
                        KMINT32 nMipmapCount);

/* start each list of the current vertex buffer over, and drop what it holds
 * unrendered. under KM_CONFIGFLAG_ENABLE_2V_LATENCY, the call first resets the
 * tile accelerator and sets up its lists again. the tile accelerator is the
 * hardware the lists go to */
void kmResetPointer(void);

/* a video signal, the low two bits of a display mode */
typedef enum { KM_NTSC = 0, KM_VGA = 1, KM_PAL = 2 } KMVIDEOMODE;
/* a frame buffer's pixel format */
typedef enum {
    KM_DSPBPP_RGB565 = 0,
    KM_DSPBPP_RGB555 = 1,
    KM_DSPBPP_ARGB1555 = 3,
    KM_DSPBPP_RGB888 = 4,
    KM_DSPBPP_ARGB8888 = 5
} KMBPPMODE;
/* set up Kamui's device record, then start the display hardware for
 * nVideoMode. a mode outside KMVIDEOMODE returns KMSTATUS_INVALID_VIDEO_MODE
 * after the record's setup, and starts no display */
KMSTATUS kmInitDevice(KMVIDEOMODE nVideoMode);
/* set the display to nDisplayMode, with frame buffers in pixel format nBpp.
 * the mode's bits are those of Ninja's NJD_DSP_ values below NJD_DSP_ANTI.
 * fDither and fAntiAlias turn on the output's dither and the anti-aliasing */
KMSTATUS kmSetDisplayMode(KMINT32 nDisplayMode, KMBPPMODE nBpp,
                          KMBOOLEAN fDither, KMBOOLEAN fAntiAlias);
/* make a frame last nCount vertical blanks. the call stores nCount - 1, and
 * returns KMSTATUS_SUCCESS */
KMSTATUS kmSetWaitVsyncCount(KMINT32 nCount);
/* render the lists the vertex buffer holds */
KMSTATUS kmRender(void);
KMSTATUS kmSetPixelClipping(KMINT32 left, KMINT32 top, KMINT32 right,
                            KMINT32 bottom);
/* show the frame buffer the last render drew into */
KMSTATUS kmFlipFrameBuffer(void);
/* stop showing the frame buffer */
KMSTATUS kmStopDisplayFrameBuffer(void);
/* move the picture's origin by nXAdjust across and nYAdjust down */
KMSTATUS kmAdjustDisplayCenter(KMINT32 nXAdjust, KMINT32 nYAdjust);
/* register pFatalErrorCallback for Kamui's fatal errors, with null as its
 * registered argument. it returns KMSTATUS_SUCCESS */
KMSTATUS kmfSetFatalErrorCallback(PKMCALLBACKFUNC pFatalErrorCallback);
/* while flag is TRUE, kmSetDisplayMode changes the mode without waiting for
 * a vertical sync. the call returns KMSTATUS_SUCCESS */
KMSTATUS kmfSetNoWaitVSyncAtDisplayModeChange(KMBOOLEAN flag);
/* The two tile-accelerator banks appear in reverse order. FBLineStride
 * holds each field's byte offset from the start of its frame buffer. */
typedef struct {
    struct {
        KMDWORD NativeITPBase;
        KMDWORD NativeOBJBase;
        KMDWORD NativeRegBase;
        KMDWORD NativeITPLimit;
        KMDWORD NativeOBJLimit;
        KMDWORD NativeLastITPCurrent;
    } NativeParams[2];
    struct {
        KMDWORD FBStartAddr[2];
        KMDWORD FBLineStride[2];
        KMDWORD XClip;
        KMDWORD YClip;
    } FBParams;
    KMDWORD RenderTimeOutCount;
    KMDWORD VertexTimeOutCount;
    KMDWORD OBJOverflowCount;
    KMDWORD ITPOverflowCount;
    KMDWORD DMADataAddress[2][KMI_LISTS];
    KMDWORD InternalVertexBufSize[KMI_LISTS];
    KMDWORD MaxVertexBufSize[KMI_LISTS];
    KMDWORD LastTATime; /* in units of 1.28 microseconds */
    KMDWORD LastRenderTime; /* in units of 1.28 microseconds */
    KMDWORD reserved[6];
} KMISYSTEMMETRICS, *PKMISYSTEMMETRICS;
/* fill pSystemMetrics with Kamui's internal state */
KMSTATUS kmfGetInternalSystemMetrics(PKMISYSTEMMETRICS pSystemMetrics);

#endif
