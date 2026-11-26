/* CRI's Sofdec movie renderer, a pool of RNV2_HANDLES. a renderer loads each
 * picture of its picture joint into a texture. its frames joint holds the
 * textures' frames, free or waiting to be shown. its shows joint holds those
 * being shown */
#ifndef SG_MWRV2_H
#define SG_MWRV2_H
#include <kamui.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <sj.h>

#define RNV2_HANDLES 8
#define RNV2_SURFACES 16
#define RNV2_QUAD 4

typedef enum {
    MWE_RNV2_STAT_STOP = 0,
    MWE_RNV2_STAT_WAIT = 5,
    MWE_RNV2_STAT_EXEC = 7
} MWE_RNV2_STAT;

typedef struct {
    Sint32 format;
    Sint32 width;
    Sint32 height;
    Uint32 frame_count;
    Uint32 picture_count;
    Sint32 work_size;
    char *work;
} MWS_RNV2_CPRM;

/* a vertex as the renderer gives it to Kamui, or a surface point it makes one
 * from. a vertex holds a KM_VERTEXPARAM_ word in control and the reciprocal of
 * its depth in z. a point holds RNV2_CONTINUE_STRIP or RNV2_END_STRIP in
 * control and the depth itself in z. u and v run from 0 to 1 across the
 * texture */
struct rnv2_vertex {
    Uint32 control;
    Float x, y, z;
    Float u, v;
    Uint32 base;
    Uint32 offset;
};
struct rnv2_frame {
    Sint32 sequence;
    Sint32 time;
    Sint32 time_unit;
    Sint32 texture_index;
};

/* a draw callback is asked of each frame that waits to be shown, in order.
 * it returns one of the RNV2_DRAW_ results. a display callback hears of each
 * shown frame as the renderer frees it */
typedef Sint32 (*rnv2_draw_fn)(void *user, Sint32 time, Sint32 time_unit,
                               Sint32 sequence);
typedef void (*rnv2_display_fn)(void *user, Sint32 time, Sint32 time_unit,
                                Sint32 sequence);
#define RNV2_DRAW_SHOW 0 /* show the frame */
#define RNV2_DRAW_HOLD 1 /* leave it for the next draw */
#define RNV2_DRAW_SKIP 2 /* pass over it to the next waiting frame */

typedef struct mw_rnv2 {
    Uint8 used;
    Sint32 status;
    Uint8 display_enabled;
    KMVERTEXCONTEXT *active_context;
    KMSURFACEDESC *active_surfaces;
    KMVERTEXBUFFDESC *vertex_buffer;
    Sint32 composition;
    Sint32 format;
    Uint32 frame_count;
    char *frame_work;
    Uint32 picture_count;
    Sint32 width, height;
    Sint32 texture_width, texture_height;
    Float display_width, display_height;
    Float window_x, window_y, window_z;
    Float window_width, window_height;
    Float image_x, image_y, image_width, image_height;
    Uint32 brightness, brightness_offset;
    rnv2_draw_fn draw;
    void *draw_user;
    rnv2_display_fn display;
    void *display_user;
    Sint32 sequence;
    struct rnv2_frame *shown;
    SJ pictures, frames, shows;
    Sint32 point_count;
    struct rnv2_vertex *points;
    Sint32 vertex_count;
    struct rnv2_vertex *vertices;
    struct rnv2_vertex corners[RNV2_QUAD];
    struct rnv2_vertex strip[RNV2_QUAD];
    char frame_queue_work[256];
    char show_queue_work[64];
    KMVERTEXCONTEXT context;
    KMSURFACEDESC surfaces[RNV2_SURFACES];
} MWRNV2_OBJ;
typedef MWRNV2_OBJ *MWRNV2;

/* how many draws since the movie's frame 0 showed the same frame as the
 * draw before them */
extern Sint32 mwg_rnv_ndrop;

void mwRnv2LockCs(Uint32 *mask);
void mwRnv2UnlockCs(const Uint32 *mask);
void mwRnv2SetVertexBuffer(KMVERTEXBUFFDESC *buffer);
void mwRnv2SetLatency(Sint32 latency);
void mwRnv2Init(void);
void mwRnv2Finish(void);
/* mwRnv2CalcMvfWork gives the bytes of count pictures, each an image and
 * its struct mw_mvf. mwRnv2CalcWork adds a struct rnv2_frame for each
 * texture frame */
Sint32 mwRnv2CalcMvfWork(Sint32 format, Sint32 width, Sint32 height,
                         Sint32 count);
Sint32 mwRnv2CalcWork(MWS_RNV2_CPRM *params);
MWRNV2 mwRnv2Create(SJ pictures, MWS_RNV2_CPRM *params);
void mwRnv2Destroy(MWRNV2 rnv);
void mwRnv2Start(MWRNV2 rnv);
void mwRnv2Stop(MWRNV2 rnv);
MWE_RNV2_STAT mwRnv2GetStat(MWRNV2 rnv);
void mwRnv2EntryDrawCallbackFn(MWRNV2 rnv, rnv2_draw_fn draw, void *user);
void mwRnv2EntryDispCallbackFn(MWRNV2 rnv, rnv2_display_fn display, void *user);
void mwRnv2GetNumTxfrm(MWRNV2 rnv, Sint32 *ready, Sint32 *shown);
Sint32 mwRnv2GetNumRoomTex(MWRNV2 rnv);
/* mwRnv2SetCompoMode's modes, by the color each leaves. screen is the color
 * under the picture, a is the picture's alpha, and sa the screen's. every
 * mode but MWD_RNV2_COMPO_OPEQ draws on the translucent list */
#define MWD_RNV2_COMPO_OPEQ 0 /* picture */
#define MWD_RNV2_COMPO_ADD 1 /* screen + picture * a */
#define MWD_RNV2_COMPO_ALPH 2 /* screen * (1 - a) + picture * a */
#define MWD_RNV2_COMPO_INVALPH 3 /* screen * a + picture * (1 - a) */
#define MWD_RNV2_COMPO_DSTALPH 4 /* screen + picture * sa */
#define MWD_RNV2_COMPO_INVDSTALPH 5 /* screen + picture * (1 - sa) */
void mwRnv2SetCompoMode(MWRNV2 rnv, Sint32 mode);
void mwRnv2SetDispSw(MWRNV2 rnv, Sint32 enabled);
Sint32 mwRnv2GetDispSw(MWRNV2 rnv);
void mwRnv2SetWndPos(MWRNV2 rnv, Float x, Float y);
void mwRnv2SetWndSize(MWRNV2 rnv, Float width, Float height);
void mwRnv2SetWndZ(MWRNV2 rnv, Float depth);
void mwRnv2SetWndBright(MWRNV2 rnv, Uint32 level);
Uint32 mwRnv2GetWndBright(MWRNV2 rnv);
void mwRnv2SetWndBrightOfst(MWRNV2 rnv, Uint32 level);
Uint32 mwRnv2GetWndBrightOfst(MWRNV2 rnv);
void mwRnv2SetWndImgPos(MWRNV2 rnv, Float u, Float v);
void mwRnv2SetWndImgSize(MWRNV2 rnv, Float width, Float height);
NJS_TEXSURFACE *mwRnv2GetSrf(MWRNV2 rnv, Sint32 index);
NJS_TEXSURFACE *mwRnv2GetCurSrf(MWRNV2 rnv);
/* a surface point's control. RNV2_END_STRIP ends the strip at the point's
 * vertex, and any other control continues it */
#define RNV2_CONTINUE_STRIP 0
#define RNV2_END_STRIP 1
void mwRnv2SetSrfCtrl(MWRNV2 rnv, Uint32 index, Uint32 control);
Uint32 mwRnv2GetSrfCtrl(MWRNV2 rnv, Uint32 index);
void mwRnv2SetSrfPos(MWRNV2 rnv, Uint32 index, Float x, Float y, Float z);
void mwRnv2GetSrfPos(MWRNV2 rnv, Uint32 index, Float *x, Float *y, Float *z);
void mwRnv2SetSrfBright(MWRNV2 rnv, Uint32 index, Float alpha, Float red,
                        Float green, Float blue);
void mwRnv2GetSrfBright(MWRNV2 rnv, Uint32 index, Float *alpha, Float *red,
                        Float *green, Float *blue);
void mwRnv2SetSrfBrightOfst(MWRNV2 rnv, Uint32 index, Float alpha, Float red,
                            Float green, Float blue);
void mwRnv2GetSrfBrightOfst(MWRNV2 rnv, Uint32 index, Float *alpha, Float *red,
                            Float *green, Float *blue);
void mwRnv2SetImgPos(MWRNV2 rnv, Uint32 index, Float u, Float v);
void mwRnv2GetImgPos(MWRNV2 rnv, Uint32 index, Float *u, Float *v);
void mwRnv2SetImgSize(MWRNV2 rnv, Sint32 width, Sint32 height);
void mwRnv2GetImgSize(MWRNV2 rnv, Sint32 *width, Sint32 *height);
Sint32 mwRnv2CalcSrfBufSize(MWRNV2 rnv, Sint32 count);
void mwRnv2SetSrfPntBuf(MWRNV2 rnv, Sint32 count, void *buffer, Sint32 size);
void mwRnv2Draw(MWRNV2 rnv);
void mwRnv2Transfer(MWRNV2 rnv);
void mwRnv2Load(MWRNV2 rnv, Uint8 *image, Sint32 time, Sint32 time_unit);
void mwlRnv2ExecOne(MWRNV2 rnv);
void mwRnv2ExecServer(void);

/* the renderer's texture and vertex calls. mwlrnv2_calc_texsize rounds
 * each side up to a power of two. mwRnv2AllocTex creates count texture
 * surfaces in the array at surfaces, square ones for a palette's images. it
 * returns surfaces, or NULL where Kamui can't create one. mwRnv2FreeTex
 * frees them */
void mwlrnv2_calc_texsize(Sint32 width, Sint32 height, Sint32 *padded_width,
                          Sint32 *padded_height);
KMSURFACEDESC *mwRnv2AllocTex(Sint32 format, Sint32 width, Sint32 height,
                              Sint32 count, KMSURFACEDESC *surfaces);
void mwRnv2ChangeTex(MWRNV2 rnv, Sint32 format, Sint32 width, Sint32 height);
void mwRnv2FreeTex(KMSURFACEDESC *surfaces, Sint32 count);
void mwRnv2LoadTexYuv420(MWRNV2 rnv, Sint32 index, Uint8 *image);
void mwRnv2LoadTexYuv422(MWRNV2 rnv, Sint32 index, Uint8 *image);
void mwRnv2LoadTex(MWRNV2 rnv, Sint32 index, Uint8 *image);
void mwRnv2LoadPltARGB8888(MWRNV2 rnv, Uint8 *palette, Sint32 start,
                           Sint32 count);
void mwRnv2InitVertexContextOpaq(KMVERTEXCONTEXT *context);
void mwRnv2InitDisp(MWRNV2 rnv, KMVERTEXCONTEXT *context);
void mwRnv2SetVctxBlendingMode(MWRNV2 rnv, KMVERTEXCONTEXT *context);
/* filter bilinearly where filter is TRUE, and point-sample for any other
 * value */
void mwRnv2SetVctxFilterMode(MWRNV2 rnv, Sint32 filter);
void mwRnv2SetVertexContextPlt8(MWRNV2 rnv, Uint32 bank);
/* the window's depth after mwRnv2SetWndDefConf or mwRnv2SetWndExpand */
#define RNV2_DEFAULT_DEPTH 65536.0F
/* mwRnv2SetWndDefConf's modes. RNV2_WND_FULL stretches the picture over the
 * whole display. RNV2_WND_BY_SIZE, like any mode but RNV2_WND_FULL, picks the
 * window and the part of the picture it shows by the picture's size */
#define RNV2_WND_BY_SIZE 0
#define RNV2_WND_FULL 1
void mwRnv2SetWndDefConf(MWRNV2 rnv, Sint32 mode);
void mwRnv2SetWndExpand(MWRNV2 rnv, Sint32 horizontal, Sint32 vertical);
void mwRnv2SetVtex(MWRNV2 rnv, struct rnv2_vertex *vertices);
void mwRnv2WndToSrf(MWRNV2 rnv);
void mwRnv2SrfToVtex(struct rnv2_vertex *point, struct rnv2_vertex *vertex);
void mwRnv2SrfToVtexAll(struct rnv2_vertex *points, Sint32 point_count,
                        struct rnv2_vertex *vertices, Sint32 vertex_count);
/* the window's fields make the surface points, and the points make the
 * vertices Kamui draws */
#define UPDATE_VERTICES(rnv)                                                   \
    mwRnv2SrfToVtexAll((rnv)->points, (rnv)->point_count, (rnv)->vertices,     \
                       (rnv)->vertex_count)
#define UPDATE_WINDOW(rnv)                                                     \
    do {                                                                       \
        mwRnv2WndToSrf(rnv);                                                   \
        UPDATE_VERTICES(rnv);                                                  \
    } while (0)
void mwRnv2DispTex(MWRNV2 rnv, Sint32 index);

#endif
