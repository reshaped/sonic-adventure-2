/* the MPEG video decoder library: its work, its decoders and their
 * conditions */
#include <machine.h>
#include <sg_xpt.h>
#include <sofdec.h>

#ifdef NONMATCHING
/* this side checks the sections by sofdec_sections.h's mpv_check_section_sizes:
 * MPVASEC_CheckSectionSiz wants each size within bytes of SHC's, and GCC's code
 * has sizes of its own */
#include "engine/dc/sofdec_sections.h"
#else
#define mpv_check_section_sizes MPVASEC_CheckSectionSiz
#endif

enum handle_state { HANDLE_FREE = 1, HANDLE_USED };
/* the word after the initial conditions */
#define CONDITIONS_GUARD 0x5a5a5a5a
/* the boundary a decoder's blocks must lie on */
#define BLOCKS_ALIGN 32

/* log2 of a decoder's bytes, 8 KB. the decoders start at the work's first
 * boundary of that size */
#define DECODER_SHIFT 13
/* the bytes of the auxiliary area between the decoders and the shared
 * tables. with the tables it takes 8 KB */
#define AUX_BYTES 0x1560

/* the cache control register, and its bits that put the instruction and
 * operand caches in index mode */
#define CCR (*(volatile Uint32 *)0xff00001c)
#define CCR_IIX 0x8000
#define CCR_OIX 0x80
/* the address bit that picks a cache's half in index mode */
#define CACHE_HALF 0x02000000
/* the entries of run_level_4, and of run_level_2 and run_level_1 */
#define RUN_LEVEL_4_ENTRIES 8
#define RUN_LEVEL_ENTRIES 16

/* the library's record. aux, tables and decoders point into the work
 * MPV_Init is given */
struct mpv_work {
    char version[32];
    char copyright[32];
    Sint32 cond[MPV_CONDITIONS]; /* the conditions MPV_Create gives */
    Bool instruction_index; /* CCR_IIX, as MPV_Init read it */
    Bool operand_index; /* CCR_OIX, as MPV_Init read it */
    Uint8 *aux;
    struct mpv_ixa *tables;
    Sint32 count;
    struct mpv_obj *decoders;
};

/* the library's conditions at MPV_Init, by MPV_COND_*, then CONDITIONS_GUARD */
static const Sint32 initial_conditions[MPV_CONDITIONS + 1] = {
    0, 1, 1, 0, 0, 1, 0, 0, CONDITIONS_GUARD,
};

/* the member exports these functions and data, the statics aside, though
 * no header of the SDK declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
const Sint32 mpvlib_siz_mpvwork = sizeof(struct mpv_work);
const Sint32 mpvlib_siz_mpvobj = sizeof(struct mpv_obj);
const Sint32 mpvlib_siz_mpvobj_mbpos = sizeof(((struct mpv_obj *)0)->mbpos);
const Sint32 mpvlib_siz_mpvobj_dmy1 = sizeof(((struct mpv_obj *)0)->unk_1bc0);
const Sint32 mpvlib_siz_mpvixa = sizeof(struct mpv_ixa);
static struct mpv_work library;
/* named for CCR_IIX and CCR_OIX, but no code reads or writes them:
 * library.instruction_index and operand_index hold those bits */
Bool mpvlib_iix;
Bool mpvlib_oix;
/* the handle MPVLIB_CheckHn checked last */
static struct mpv_obj *last_handle;
void mpvlib_InitUseLib(struct mpv_ixa *tables);
/* NOLINTEND(misc-use-internal-linkage) */

static Sint32 mpvlib_ChkFatal(void);
static void mpvlib_ChkCacheMode(void);
static void mpvlib_InitObjTbl(void);
static void mpvlib_InitObj(struct mpv_obj *decoder);
static void mpvlib_InitWork(Sint32 count, void *work);
static void mpvlib_InitDctPa(struct mpv_obj *decoder);
static void mpvlib_InitPicAtr(struct mpv_picture *picture);
static void mpvlib_SetCondAll(Sint32 id, Sint32 value);

Sint32 MPV_Init(Sint32 count, void *work)
{
    Sint32 error;
    struct mpv_ixa *tables;
    /* @hack spelling: address, a copy of work, where work would do */
    Uint32 address = (Uint32)work;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((error = mpvlib_ChkFatal()) != 0) {
        if (error == MPV_ERR_SECTION_ORDER) {
            return error;
        }
        for (;;) {
        }
    }
    mpvlib_ChkCacheMode();
    mpvlib_InitWork(count, (void *)MPVLIB_ConvOix(address));
    tables = library.tables;
    MPVERR_Init();
    MPVFRM_Init();
    MPVVLC_Init(tables->vlc);
    MPVBDEC_Init(tables);
    MPVCMC_Init(tables->clip);
    mpvlib_InitObjTbl();
    DCT_FzInit(tables->dct);
    return 0;
}

static Sint32 mpvlib_ChkFatal(void)
{
    /* @hack spelling: offset, a local for the blocks' offset, where the
     * expression itself would do */
    Sint32 offset = (Sint32)((struct mpv_obj *)0)->blocks;

    if ((offset & (BLOCKS_ALIGN - 1)) != 0) {
        return MPVERR_SetCode(NULL, MPV_ERR_ALIGNMENT);
    }
    if (initial_conditions[MPV_CONDITIONS] != CONDITIONS_GUARD) {
        return MPVERR_SetCode(NULL, MPV_ERR_GUARD);
    }
    if (MPVVLC_IsVlcSizErr()) {
        return MPVERR_SetCode(NULL, MPV_ERR_VLC_SIZE);
    }
    if (mpv_check_section_sizes() != 0) {
        return MPVERR_SetCode(NULL, MPV_ERR_SECTION_SIZE);
    }
    if (MPVASEC_CheckSectionSeq() != 0) {
        return MPVERR_SetCode(NULL, MPV_ERR_SECTION_ORDER);
    }
    return 0;
}

static void mpvlib_ChkCacheMode(void)
{
    Uint32 control;

    library.instruction_index = FALSE;
    library.operand_index = FALSE;
    control = CCR;
    if (control & CCR_IIX) {
        library.instruction_index = TRUE;
    }
    if (control & CCR_OIX) {
        library.operand_index = TRUE;
    }
}

Uint32 MPVLIB_ConvIix(Uint32 address)
{
    if (library.instruction_index) {
        address |= CACHE_HALF;
    }
    return address;
}

Uint32 MPVLIB_ConvOix(Uint32 address)
{
    if (library.operand_index) {
        address |= CACHE_HALF;
    }
    return address;
}

static void mpvlib_InitObjTbl(void)
{
    Sint32 count = library.count;
    struct mpv_obj *decoder = library.decoders;
    Sint32 i;

    for (i = 0; i < count; i++) {
        mpvlib_InitObj(decoder++);
    }
}

static void mpvlib_InitObj(struct mpv_obj *decoder)
{
    struct mpv_ixa *tables;
    struct mpv_mb *mb;

    decoder->state = HANDLE_FREE;
    tables = library.tables;
    /* the block decoder indexes run_level_4, 2 and 1 from the 1 that ends a
     * code's zeros. an index is then at least its table's length. each
     * pointer stands that many entries below its table */
    decoder->run_level_8 = mpvvlc_run_level_8;
    decoder->run_level_4 = mpvvlc_run_level_4 - RUN_LEVEL_4_ENTRIES;
    decoder->run_level_2 = mpvvlc_run_level_2 - RUN_LEVEL_ENTRIES;
    decoder->run_level_1 = mpvvlc_run_level_1 - RUN_LEVEL_ENTRIES;
    decoder->run_level_0a = mpvvlc_run_level_0a;
    decoder->run_level_0b = mpvvlc_run_level_0b;
    decoder->run_level_0c = mpvvlc_run_level_0c;
    decoder->zigzag = tables->zigzag;
    decoder->ixa = tables;
    decoder->dct_work = decoder->ixa->dct;
    mb = &decoder->mb;
    decoder->clip = mpv_clip_0_255_base;
    mb->clip = decoder->clip;
    mb->blocks = decoder->blocks[0];
    mb->prediction = decoder->mb_work;
    mb->backward_prediction = mb->prediction + MPV_MB_BYTES;
    /* the inverse DCT writes Y's four blocks, then Cb's and Cr's. blocks
     * holds Cb's and Cr's first */
    decoder->block_ptr[0] = decoder->blocks[2];
    decoder->block_ptr[1] = decoder->blocks[3];
    decoder->block_ptr[2] = decoder->blocks[4];
    decoder->block_ptr[3] = decoder->blocks[5];
    decoder->block_ptr[4] = decoder->blocks[0];
    decoder->block_ptr[5] = decoder->blocks[1];
}

void mpvlib_InitUseLib(struct mpv_ixa *tables)
{
    DCT_FzInit(tables->dct);
    /* @hack redundant: a return at the function's end */
    return; /* NOLINT(readability-redundant-control-flow) */
}

/* clear work, and lay it out. count decoders start at its first 8 KB
 * boundary. the auxiliary area and the shared tables follow them. the memset
 * covers one decoder more than count, room for the decoders' alignment */
static void mpvlib_InitWork(Sint32 count, void *work)
{
    struct mpv_obj *decoders;
    Uint8 *aux;
    struct mpv_ixa *tables;

    UTY_MemsetDword(work, 0,
                    ((count * sizeof(struct mpv_obj)) + sizeof(struct mpv_obj) +
                     AUX_BYTES + sizeof(struct mpv_ixa)) /
                        sizeof(Uint32));
    /* @hack spelling: work rounded up by two shifts, where a mask would do */
    work =
        (void *)((((Uint32)work + sizeof(struct mpv_obj) - 1) >> DECODER_SHIFT)
                 << DECODER_SHIFT);
    decoders = work;
    aux = (Uint8 *)(decoders + count);
    tables = (struct mpv_ixa *)(aux + AUX_BYTES);
    /* @hack spelling: _builtin_strcpy, where strcpy would do */
    _builtin_strcpy(library.version, "MPV Version 1.31 1999-11-26");
    _builtin_strcpy(library.copyright, "Copyright (c) 1997,1999 CRI");
    UTY_MemcpyDword((Uint32 *)library.cond, (const Uint32 *)initial_conditions,
                    MPV_CONDITIONS);
    library.aux = aux;
    library.tables = tables;
    library.count = count;
    library.decoders = decoders;
}

static void mpvlib_InitPicAtr(struct mpv_picture *picture)
{
    picture->width = 0;
    picture->height = 0;
    picture->mb_width = 0;
    picture->mb_height = 0;
    picture->frame_rate = 0;
    picture->temporal_ref = 0;
    picture->coding_type = 0;
    picture->drop_frame = 0;
    picture->hours = 0;
    picture->minutes = 0;
    picture->seconds = 0;
    picture->pictures = 0;
    picture->gop = 0;
}

void MPV_Finish(void)
{
}

struct mpv_obj *MPV_Create(void)
{
    Sint32 count = library.count;
    struct mpv_obj *decoder = library.decoders;
    Sint32 i;

    for (i = 0; i < count; i++, decoder++) {
        if (decoder->state == HANDLE_FREE) {
            decoder->state = HANDLE_USED;
            decoder->busy_count = 0;
            UTY_MemcpyDword((Uint32 *)decoder->cond,
                            (const Uint32 *)library.cond, MPV_CONDITIONS);
            MPVERR_InitErrInf(&decoder->err);
            MPVCMC_InitObj(decoder);
            mpvlib_InitDctPa(decoder);
            mpvlib_InitPicAtr(&decoder->picture);
            return decoder;
        }
    }
    return NULL;
}

static void mpvlib_InitDctPa(struct mpv_obj *decoder)
{
    struct dctfz_pa *pa = &decoder->dct;

    DCT_FzInitPa(pa);
    pa->work = (Float *)decoder->mb_work;
    pa->coef = decoder->coef[0];
    pa->blk = decoder->block_ptr;
}

void MPV_GetDctCnt(struct mpv_obj *mpv, Sint32 *first, Sint32 *second)
{
    struct dctfz_pa *pa = &mpv->dct;

    *first = pa->count[0];
    *second = pa->count[1];
}

Sint32 MPV_Destroy(struct mpv_obj *handle)
{
    /* @hack spelling: decoder, a copy of handle, where handle would do */
    struct mpv_obj *decoder = handle;

    if (MPVLIB_CheckHn(decoder) != 0) {
        return MPVERR_SetCode(NULL, MPV_ERR_DESTROY);
    }
    decoder->state = HANDLE_FREE;
    return 0;
}

Sint32 MPV_SetCond(struct mpv_obj *handle, Sint32 id, Sint32 value)
{
    /* @hack spelling: decoder, a copy of handle, where handle would do */
    struct mpv_obj *decoder = handle;
    Sint32 *conditions;

    if (decoder == NULL) {
        mpvlib_SetCondAll(id, value);
        conditions = library.cond;
    } else {
        if (MPVLIB_CheckHn(decoder) != 0) {
            return MPVERR_SetCode(NULL, MPV_ERR_SET_COND);
        }
        conditions = decoder->cond;
    }
    conditions[id] = value;
    return 0;
}

static void mpvlib_SetCondAll(Sint32 id, Sint32 value)
{
    Sint32 count = library.count;
    struct mpv_obj *decoder = library.decoders;
    Sint32 i;

    for (i = 0; i < count; i++, decoder++) {
        if (decoder->state == HANDLE_USED) {
            decoder->cond[id] = value;
        }
    }
}

Sint32 MPVLIB_CheckHn(struct mpv_obj *mpv)
{
    last_handle = mpv;
    if (mpv == NULL || mpv->state != HANDLE_USED) {
        return -1;
    }
    return 0;
}
