/* the movie renderer (sg_mwrv2.h). a frame mwRnv2Draw shows stays on shows
 * until mwg_rnv2_nfrm_rendering later frames join it. until then, no load can
 * write its texture, which may still be drawn */
#include <kamui.h>
#include <machine.h>
#include <mw_mvf.h>
#include <ninja.h>
#include <sg_mwrv2.h>
#include <sg_xpt.h>
#include <sj.h>
#include <string.h>

#define FRAME_BYTES ((Sint32)sizeof(struct rnv2_frame))
#define PICTURE_BYTES ((Sint32)sizeof(struct mw_mvf))
/* a new renderer's display, in pixels */
#define DISPLAY_WIDTH 640.0F
#define DISPLAY_HEIGHT 480.0F
#define OPAQUE_BLACK 0xff000000U
#define DEFAULT_BRIGHTNESS 0xffeaeaeaU

/* the member exports these names, though no header of the SDK declares
 * them but mwg_rnv_ndrop */
/* NOLINTBEGIN(misc-use-internal-linkage) */
const char mwg_rnv2_build[] = "\nmwRnv2 Ver.2.42 Build:Aug 31 2000 22:53:29\n";

/* mwRnv2SetLatency sets the two from a latency of 2 or 3, and leaves them
 * for any other. only an empty test in mwlRnv2ExecOne reads
 * mwg_rnv2_nfrm_pool */
Sint32 mwg_rnv2_nfrm_pool = 2;
Sint32 mwg_rnv2_nfrm_rendering = 1;
Sint32 mwg_rnv_ndrop = 0;
/* the sequence number of the frame the last draw showed */
Sint32 mwg_rnv_dfno = 0;
KMVERTEXBUFFDESC *mwg_DefVertexBuffer = NULL;
Sint32 mwg_rnv2_init_cnt = 0;
MWRNV2_OBJ mwg_rnv2_obj[RNV2_HANDLES];
/* NOLINTEND(misc-use-internal-linkage) */

void mwRnv2LockCs(Uint32 *mask)
{
    *mask = get_imask();
    set_imask(IMASK_ALL);
}

void mwRnv2UnlockCs(const Uint32 *mask)
{
    set_imask(*mask);
}

void mwRnv2SetVertexBuffer(KMVERTEXBUFFDESC *buffer)
{
    mwg_DefVertexBuffer = buffer;
}

void mwRnv2SetLatency(Sint32 latency)
{
    if (latency == 2) {
        mwg_rnv2_nfrm_rendering = 1;
        mwg_rnv2_nfrm_pool = 2;
    } else if (latency == 3) {
        mwg_rnv2_nfrm_rendering = 2;
        mwg_rnv2_nfrm_pool = 3;
    }
}

void mwRnv2Init(void)
{
    if (mwg_rnv2_init_cnt == 0) {
        memset(mwg_rnv2_obj, 0, sizeof(mwg_rnv2_obj));
    }
    mwg_rnv2_init_cnt++;
}

void mwRnv2Finish(void)
{
    MWRNV2 rnv;
    Sint32 i;

    if (--mwg_rnv2_init_cnt == 0) {
        for (i = 0; i < RNV2_HANDLES; i++) {
            rnv = &mwg_rnv2_obj[i];
            if (rnv->used == TRUE) {
                mwRnv2Destroy(rnv);
            }
        }
        memset(mwg_rnv2_obj, 0, sizeof(mwg_rnv2_obj));
    }
}

Sint32 mwRnv2CalcMvfWork(Sint32 format, Sint32 width, Sint32 height,
                         Sint32 count)
{
    return (mwMvfCalcImgSize(format, width, height) + PICTURE_BYTES) * count;
}

Sint32 mwRnv2CalcWork(MWS_RNV2_CPRM *params)
{
    Sint32 pictures;
    Sint32 frames;

    pictures = mwRnv2CalcMvfWork(params->format, params->width, params->height,
                                 params->picture_count);
    frames = params->frame_count * FRAME_BYTES;
    return pictures + frames;
}

MWRNV2 mwRnv2Create(SJ pictures, MWS_RNV2_CPRM *params)
{
    Uint32 i;
    /* @hack redundant: unused, five words that only size the frame */
    Uint32 unused[5];
    Sint32 image_size;
    MWRNV2 rnv;
    Uint32 mask;
    SJCK chunk;
    struct mw_mvf *picture;
    char *cursor;
    char *work;
    struct rnv2_frame *frame;

    work = params->work;
    mwRnv2LockCs(&mask);
    for (i = 0; i < RNV2_HANDLES; i++) {
        rnv = &mwg_rnv2_obj[i];
        if (rnv->used == FALSE) {
            break;
        }
    }
    /* @bug logic: a full pool returns with every interrupt still masked. the
     * return at 0x8c1052ca passes over the unlock at 0x8c1055b2 */
    if (i == RNV2_HANDLES) {
        return NULL;
    }
    cursor = work;
    rnv->used = TRUE;
    rnv->pictures = pictures;
    rnv->display_enabled = TRUE;
    rnv->status = MWE_RNV2_STAT_STOP;
    rnv->frames =
        SJUNI_Create(0, rnv->frame_queue_work, sizeof(rnv->frame_queue_work));
    rnv->shows =
        SJUNI_Create(0, rnv->show_queue_work, sizeof(rnv->show_queue_work));
    /* a joint that can't be made stops the program here */
    if (rnv->frames == NULL || rnv->shows == NULL) {
        for (;;) {
        }
    }
    rnv->composition = MWD_RNV2_COMPO_OPEQ;
    rnv->display_width = DISPLAY_WIDTH;
    rnv->display_height = DISPLAY_HEIGHT;
    rnv->width = params->width;
    rnv->height = params->height;
    rnv->sequence = 0;
    rnv->shown = NULL;
    rnv->draw = NULL;
    rnv->draw_user = NULL;
    rnv->display = NULL;
    rnv->display_user = NULL;
    rnv->brightness = DEFAULT_BRIGHTNESS;
    rnv->brightness_offset = OPAQUE_BLACK;
    mwRnv2InitDisp(rnv, &rnv->context);
    rnv->active_context = &rnv->context;
    rnv->format = params->format;
    rnv->frame_count = params->frame_count;
    mwRnv2AllocTex(rnv->format, rnv->width, rnv->height, rnv->frame_count,
                   rnv->surfaces);
    rnv->texture_width = rnv->surfaces[0].u0.Width;
    rnv->texture_height = rnv->surfaces[0].u1.Height;
    rnv->active_surfaces = rnv->surfaces;
    rnv->frame_work = cursor;
    for (i = 0; i < rnv->frame_count; i++) {
        frame = (struct rnv2_frame *)cursor;
        cursor += FRAME_BYTES;
        memset(frame, 0, sizeof(*frame));
        frame->texture_index = i;
        chunk.data = (char *)frame;
        chunk.len = FRAME_BYTES;
        SJ_PutChunk(rnv->frames, SJ_LIN_FREE, &chunk);
    }
    rnv->picture_count = params->picture_count;
    image_size = mwMvfCalcImgSize(rnv->format, rnv->width, rnv->height);
    for (i = 0; i < rnv->picture_count; i++) {
        picture = (struct mw_mvf *)cursor;
        memset(picture, 0, sizeof(*picture));
        picture->fmt = rnv->format;
        picture->width = rnv->width;
        picture->height = rnv->height;
        picture->img = (Uint8 *)picture + PICTURE_BYTES;
        chunk.data = (char *)picture;
        /* @bug logic: each picture goes on the free line as a chunk the size
         * of a pointer, not of its struct mw_mvf, at 0x8c10550c */
        chunk.len = sizeof(struct mw_mvf *);
        SJ_PutChunk(rnv->pictures, SJ_LIN_FREE, &chunk);
        /* @bug logic: each picture's image but the last overlaps the next
         * picture's struct mw_mvf. the image starts past its own struct at
         * 0x8c105504. the cursor moves on by the image's size alone at
         * 0x8c10552e */
        cursor += image_size;
    }
    /* @hack spelling: point_count and vertex_count set in one assignment,
     * where two would do */
    rnv->point_count = rnv->vertex_count = RNV2_QUAD;
    rnv->points = rnv->corners;
    rnv->vertices = rnv->strip;
    mwRnv2SetWndDefConf(rnv, RNV2_WND_FULL);
    rnv->vertex_buffer = mwg_DefVertexBuffer;
    /* @bug memory: writes the frames and pictures past the end of a work too
     * small for them. the work's size is tested at 0x8c105598, after the
     * writes */
    /* @bug logic: a work too small returns with every interrupt still
     * masked. the return at 0x8c1055aa passes over the unlock at
     * 0x8c1055b2 */
    if (cursor - work > params->work_size) {
        mwRnv2Destroy(rnv);
        return NULL;
    }
    mwRnv2UnlockCs(&mask);
    return rnv;
}

void mwRnv2Destroy(MWRNV2 rnv)
{
    /* @hack redundant: unused, a word that only sizes the frame */
    Uint32 unused;
    Uint32 mask;

    mwRnv2LockCs(&mask);
    if (rnv != NULL) {
        rnv->used = FALSE;
        SJ_Destroy(rnv->frames);
        SJ_Destroy(rnv->shows);
        mwRnv2FreeTex(rnv->surfaces, rnv->frame_count);
    }
    mwRnv2UnlockCs(&mask);
    /* @hack redundant: a return at the end of a void function */
    return; /* NOLINT(readability-redundant-control-flow) */
}

void mwRnv2Start(MWRNV2 rnv)
{
    /* @hack redundant: unused, a word that only sizes the frame */
    Uint32 unused;

    mwRnv2Stop(rnv);
    rnv->shown = NULL;
    rnv->status = MWE_RNV2_STAT_WAIT;
}

void mwRnv2Stop(MWRNV2 rnv)
{
    SJCK chunk;

    for (;;) {
        SJ_GetChunk(rnv->shows, SJ_LIN_DATA, FRAME_BYTES, &chunk);
        if (chunk.len == 0) {
            break;
        }
        SJ_PutChunk(rnv->frames, SJ_LIN_FREE, &chunk);
    }
    for (;;) {
        SJ_GetChunk(rnv->frames, SJ_LIN_DATA, FRAME_BYTES, &chunk);
        if (chunk.len == 0) {
            break;
        }
        SJ_PutChunk(rnv->frames, SJ_LIN_FREE, &chunk);
    }
    rnv->status = MWE_RNV2_STAT_STOP;
}

MWE_RNV2_STAT mwRnv2GetStat(MWRNV2 rnv)
{
    return rnv->status;
}

void mwRnv2EntryDrawCallbackFn(MWRNV2 rnv, rnv2_draw_fn draw, void *user)
{
    rnv->draw = draw;
    rnv->draw_user = user;
}

void mwRnv2EntryDispCallbackFn(MWRNV2 rnv, rnv2_display_fn display, void *user)
{
    rnv->display = display;
    rnv->display_user = user;
}

void mwRnv2GetNumTxfrm(MWRNV2 rnv, Sint32 *ready, Sint32 *shown)
{
    if (ready != NULL) {
        *ready = (Uint32)SJ_GetNumData(rnv->frames, SJ_LIN_DATA) / FRAME_BYTES;
    }
    if (shown != NULL) {
        *shown = (Uint32)SJ_GetNumData(rnv->shows, SJ_LIN_DATA) / FRAME_BYTES;
    }
}

Sint32 mwRnv2GetNumRoomTex(MWRNV2 rnv)
{
    Uint32 bytes;
    bytes = SJ_GetNumData(rnv->frames, SJ_LIN_FREE);
    return bytes / FRAME_BYTES;
}

void mwRnv2SetCompoMode(MWRNV2 rnv, Sint32 mode)
{
    rnv->composition = mode;
    mwRnv2SetVctxBlendingMode(rnv, &rnv->context);
}

void mwRnv2SetDispSw(MWRNV2 rnv, Sint32 enabled)
{
    rnv->display_enabled = enabled;
}

Sint32 mwRnv2GetDispSw(MWRNV2 rnv)
{
    return rnv->display_enabled;
}

#define UPDATE_POINT(rnv, index)                                               \
    mwRnv2SrfToVtex(&(rnv)->points[index], &(rnv)->vertices[index])
#define GRAY(level) (((level) << 16) | ((level) << 8) | (level) | OPAQUE_BLACK)
#define GRAY_LEVEL(color) ((color) & 0xff)

void mwRnv2SetWndPos(MWRNV2 rnv, Float x, Float y)
{
    rnv->window_x = x;
    rnv->window_y = y;
    UPDATE_WINDOW(rnv);
}

void mwRnv2SetWndSize(MWRNV2 rnv, Float width, Float height)
{
    rnv->window_width = width;
    rnv->window_height = height;
    UPDATE_WINDOW(rnv);
}

void mwRnv2SetWndZ(MWRNV2 rnv, Float depth)
{
    rnv->window_z = depth;
    UPDATE_WINDOW(rnv);
}

void mwRnv2SetWndBright(MWRNV2 rnv, Uint32 level)
{
    rnv->brightness = GRAY(level);
    UPDATE_WINDOW(rnv);
}

Uint32 mwRnv2GetWndBright(MWRNV2 rnv)
{
    return GRAY_LEVEL(rnv->brightness);
}

void mwRnv2SetWndBrightOfst(MWRNV2 rnv, Uint32 level)
{
    rnv->brightness_offset = GRAY(level);
    UPDATE_WINDOW(rnv);
}

Uint32 mwRnv2GetWndBrightOfst(MWRNV2 rnv)
{
    return GRAY_LEVEL(rnv->brightness_offset);
}

void mwRnv2SetWndImgPos(MWRNV2 rnv, Float u, Float v)
{
    rnv->image_x = u;
    rnv->image_y = v;
    UPDATE_WINDOW(rnv);
}

void mwRnv2SetWndImgSize(MWRNV2 rnv, Float width, Float height)
{
    rnv->image_width = width / (Float)rnv->texture_width;
    rnv->image_height = height / (Float)rnv->texture_height;
    UPDATE_WINDOW(rnv);
}

NJS_TEXSURFACE *mwRnv2GetSrf(MWRNV2 rnv, Sint32 index)
{
    return (NJS_TEXSURFACE *)&rnv->surfaces[index];
}

NJS_TEXSURFACE *mwRnv2GetCurSrf(MWRNV2 rnv)
{
    if (rnv->shown == NULL) {
        return NULL;
    }
    return mwRnv2GetSrf(rnv, rnv->shown->texture_index);
}

void mwRnv2SetSrfCtrl(MWRNV2 rnv, Uint32 index, Uint32 control)
{
    rnv->points[index].control = control;
    UPDATE_POINT(rnv, index);
}

Uint32 mwRnv2GetSrfCtrl(MWRNV2 rnv, Uint32 index)
{
    return rnv->points[index].control;
}

void mwRnv2SetSrfPos(MWRNV2 rnv, Uint32 index, Float x, Float y, Float z)
{
    rnv->points[index].x = x;
    rnv->points[index].y = y;
    rnv->points[index].z = z;
    UPDATE_POINT(rnv, index);
}

void mwRnv2GetSrfPos(MWRNV2 rnv, Uint32 index, Float *x, Float *y, Float *z)
{
    *x = rnv->points[index].x;
    *y = rnv->points[index].y;
    *z = rnv->points[index].z;
}

#define CHANNEL(value) ((Uint32)(Sint32)((value) * 255.0F))
#define ARGB(alpha, red, green, blue)                                          \
    ((CHANNEL(alpha) << 24) | (CHANNEL(red) << 16) | (CHANNEL(green) << 8) |   \
     CHANNEL(blue))
#define UNPACK_COLOR(color, alpha, red, green, blue)                           \
    do {                                                                       \
        *(alpha) = (Float)(((color) >> 24) & 0xff) / 255.0F;                   \
        *(red) = (Float)(((color) >> 16) & 0xff) / 255.0F;                     \
        *(green) = (Float)(((color) >> 8) & 0xff) / 255.0F;                    \
        *(blue) = (Float)((color) & 0xff) / 255.0F;                            \
    } while (0)

void mwRnv2SetSrfBright(MWRNV2 rnv, Uint32 index, Float alpha, Float red,
                        Float green, Float blue)
{
    rnv->points[index].base = ARGB(alpha, red, green, blue);
    UPDATE_POINT(rnv, index);
}

void mwRnv2GetSrfBright(MWRNV2 rnv, Uint32 index, Float *alpha, Float *red,
                        Float *green, Float *blue)
{
    Uint32 color;
    color = rnv->points[index].base;
    UNPACK_COLOR(color, alpha, red, green, blue);
    UPDATE_POINT(rnv, index);
}

void mwRnv2SetSrfBrightOfst(MWRNV2 rnv, Uint32 index, Float alpha, Float red,
                            Float green, Float blue)
{
    rnv->points[index].offset = ARGB(alpha, red, green, blue);
    UPDATE_POINT(rnv, index);
}

void mwRnv2GetSrfBrightOfst(MWRNV2 rnv, Uint32 index, Float *alpha, Float *red,
                            Float *green, Float *blue)
{
    Uint32 color;
    color = rnv->points[index].offset;
    UNPACK_COLOR(color, alpha, red, green, blue);
}

void mwRnv2SetImgPos(MWRNV2 rnv, Uint32 index, Float u, Float v)
{
    rnv->points[index].u = u / (Float)rnv->texture_width;
    rnv->points[index].v = v / (Float)rnv->texture_height;
    UPDATE_POINT(rnv, index);
}

void mwRnv2GetImgPos(MWRNV2 rnv, Uint32 index, Float *u, Float *v)
{
    *u = rnv->points[index].u * (Float)rnv->texture_width;
    *v = rnv->points[index].v * (Float)rnv->texture_height;
}

void mwRnv2SetImgSize(MWRNV2 rnv, Sint32 width, Sint32 height)
{
    Sint32 i;
    Float u;
    Float v;
    Float max_u;
    Float max_v;

    rnv->width = width;
    rnv->height = height;
    max_u = (Float)width;
    max_v = (Float)height;
    mwRnv2ChangeTex(rnv, rnv->format, rnv->width, rnv->height);
    for (i = 0; i < rnv->point_count; i++) {
        mwRnv2GetImgPos(rnv, i, &u, &v);
        u = (u > max_u) ? max_u : u;
        v = (v > max_v) ? max_v : v;
        mwRnv2SetImgPos(rnv, i, u, v);
    }
    UPDATE_VERTICES(rnv);
}

void mwRnv2GetImgSize(MWRNV2 rnv, Sint32 *width, Sint32 *height)
{
    *width = rnv->width;
    *height = rnv->height;
}

/* the buffer mwRnv2SetSrfPntBuf takes holds count points, then their count
 * vertices */
Sint32 mwRnv2CalcSrfBufSize(MWRNV2 rnv, Sint32 count)
{
    return count * (Sint32)(2 * sizeof(struct rnv2_vertex));
}

void mwRnv2SetSrfPntBuf(MWRNV2 rnv, Sint32 count, void *buffer, Sint32 size)
{
    struct rnv2_vertex *points;
    Sint32 i;
    Sint32 j;
    Sint32 quads;

    points = buffer;
    /* @hack spelling: point_count and vertex_count set in one assignment,
     * where two would do */
    rnv->point_count = rnv->vertex_count = count;
    rnv->points = points;
    rnv->vertices = &points[count];
    quads = count / RNV2_QUAD;
    for (i = 0; i < quads; i++) {
        for (j = 0; j < RNV2_QUAD; j++) {
            rnv->points[(i * RNV2_QUAD) + j] = rnv->corners[j];
            rnv->vertices[(i * RNV2_QUAD) + j] = rnv->strip[j];
        }
    }
    for (i = count - 1, j = RNV2_QUAD - 1; i >= quads * RNV2_QUAD; i--, j--) {
        rnv->points[i] = rnv->corners[j];
        rnv->vertices[i] = rnv->strip[j];
    }
}

void mwRnv2Draw(MWRNV2 rnv)
{
    Sint32 ready;
    Sint32 shown;
    Sint32 result;
    SJCK chunk;
    struct rnv2_frame *previous;
    struct rnv2_frame *frame;

    previous = rnv->shown;
    for (;;) {
        SJ_GetChunk(rnv->frames, SJ_LIN_DATA, FRAME_BYTES, &chunk);
        if (chunk.len == 0) {
            if (previous == NULL) {
                return;
            }
            frame = previous;
            break;
        } else {
            frame = (struct rnv2_frame *)chunk.data;
        }
        if (rnv->draw != NULL) {
            result = rnv->draw(rnv->draw_user, frame->time, frame->time_unit,
                               frame->sequence);
        } else {
            result = RNV2_DRAW_SHOW;
        }
        if (result == RNV2_DRAW_SHOW) {
            break;
        }
        if (result == RNV2_DRAW_HOLD) {
            SJ_UngetChunk(rnv->frames, SJ_LIN_DATA, &chunk);
            if (previous == NULL) {
                return;
            }
            frame = previous;
            break;
        } else if (result == RNV2_DRAW_SKIP) {
            if (rnv->shown != previous) {
                chunk.data = (char *)previous;
                chunk.len = FRAME_BYTES;
                SJ_PutChunk(rnv->frames, SJ_LIN_FREE, &chunk);
            }
            previous = frame;
            /* @hack redundant: a continue at the end of the loop's body */
            continue;
        }
    }
    if (frame->sequence == 0) {
        mwg_rnv_dfno = 0;
        mwg_rnv_ndrop = 0;
    } else if (frame->sequence == mwg_rnv_dfno) {
        mwg_rnv_ndrop++;
    }
    mwg_rnv_dfno = frame->sequence;
    mwRnv2DispTex(rnv, frame->texture_index);
    if (rnv->shown != frame) {
        rnv->shown = frame;
        chunk.data = (char *)rnv->shown;
        chunk.len = FRAME_BYTES;
        SJ_PutChunk(rnv->shows, SJ_LIN_DATA, &chunk);
        mwRnv2GetNumTxfrm(rnv, &ready, &shown);
        if (shown > mwg_rnv2_nfrm_rendering) {
            SJ_GetChunk(rnv->shows, SJ_LIN_DATA, FRAME_BYTES, &chunk);
            frame = (struct rnv2_frame *)chunk.data;
            if (rnv->display != NULL) {
                /* @bug logic: the display callback gets draw_user, not
                 * display_user. the load at 0x8c106528 reads the draw slot */
                rnv->display(rnv->draw_user, frame->time, frame->time_unit,
                             frame->sequence);
            }
            SJ_PutChunk(rnv->frames, SJ_LIN_FREE, &chunk);
        }
    }
}

#define LOAD_IMAGE(rnv, format, frame, image)                                  \
    do {                                                                       \
        if ((format) == MVF_FMT_YUV420) {                                      \
            mwRnv2LoadTexYuv420(rnv, (frame)->texture_index, image);           \
        }                                                                      \
        /* @hack spelling: a test of MVF_FMT_YUV422 after the MVF_FMT_YUV420   \
         * load, where else if would do */                                     \
        if ((format) == MVF_FMT_YUV422) {                                      \
            mwRnv2LoadTexYuv422(rnv, (frame)->texture_index, image);           \
        } else if ((format) == MVF_FMT_PLT8) {                                 \
            mwRnv2LoadTex(rnv, (frame)->texture_index, image);                 \
        }                                                                      \
    } while (0)

void mwRnv2Transfer(MWRNV2 rnv)
{
    /* @hack redundant: unused, three words that only size the frame */
    Uint32 unused[3];
    SJCK chunk;
    struct rnv2_frame *frame;
    struct mw_mvf *picture;
    SJ pictures;

    pictures = rnv->pictures;
    SJ_GetChunk(rnv->frames, SJ_LIN_FREE, FRAME_BYTES, &chunk);
    if (chunk.len == 0) {
        return;
    }
    picture = mwMvfGetDispFrm(pictures);
    if (picture == NULL) {
        SJ_UngetChunk(rnv->frames, SJ_LIN_FREE, &chunk);
        return;
    }
    frame = (struct rnv2_frame *)chunk.data;
    LOAD_IMAGE(rnv, picture->fmt, frame, picture->img);
    mwMvfRelDispFrm(pictures, picture);
    SJ_PutChunk(rnv->frames, SJ_LIN_DATA, &chunk);
}

void mwRnv2Load(MWRNV2 rnv, Uint8 *image, Sint32 time, Sint32 time_unit)
{
    /* @hack redundant: unused, three words that only size the frame */
    Uint32 unused[3];
    SJCK chunk;
    struct rnv2_frame *frame;
    SJ unused_pictures;

    /* @hack redundant: unused_pictures, a copy of rnv->pictures that nothing
     * reads */
    unused_pictures = rnv->pictures;
    (void)unused_pictures;
    SJ_GetChunk(rnv->frames, SJ_LIN_FREE, FRAME_BYTES, &chunk);
    if (chunk.len == 0) {
        return;
    }
    frame = (struct rnv2_frame *)chunk.data;
    LOAD_IMAGE(rnv, rnv->format, frame, image);
    frame->sequence = rnv->sequence++;
    frame->time = time;
    frame->time_unit = time_unit;
    SJ_PutChunk(rnv->frames, SJ_LIN_DATA, &chunk);
}

void mwlRnv2ExecOne(MWRNV2 rnv)
{
    Sint32 ready;
    Sint32 shown;
    Uint32 mask;

    mwRnv2LockCs(&mask);
    if (rnv->status == MWE_RNV2_STAT_EXEC) {
        mwRnv2Draw(rnv);
    } else if (rnv->status == MWE_RNV2_STAT_WAIT) {
        mwRnv2GetNumTxfrm(rnv, &ready, &shown);
        /* @hack redundant: a test of ready against mwg_rnv2_nfrm_pool whose
         * branch is empty */
        if (ready >= mwg_rnv2_nfrm_pool) {
        }
        rnv->status = MWE_RNV2_STAT_EXEC;
    }
    mwRnv2UnlockCs(&mask);
}

void mwRnv2ExecServer(void)
{
    Sint32 i;
    for (i = RNV2_HANDLES - 1; i >= 0; i--) {
        if (mwg_rnv2_obj[i].used == TRUE) {
            mwlRnv2ExecOne(&mwg_rnv2_obj[i]);
        }
    }
}
