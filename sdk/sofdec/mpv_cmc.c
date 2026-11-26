/* the macroblock makers. a maker makes a macroblock from its coded blocks and
 * the reference pictures. it writes the macroblock to the texture, at
 * request->output. one whose name ends in Rt also writes it into the
 * reference picture being made */
#include <machine.h>
#include <sg_xpt.h>
#include <sofdec.h>

#include "../sofdec/mpv_section.h"

/* the clip table's entries below 0, and as many above the last level */
#define CLIP_MARGIN 384
#define PIXEL_LEVELS 256
/* a block's pixels across and down */
#define BLOCK_SIDE 8

/* the store queues' address registers. a queue writes to a 64 MB area. its
 * register's QACR_AREA bits hold bits 26 to 28 of the area's address */
#define QACR0 (*(volatile Uint32 *)0xff000038)
#define QACR1 (*(volatile Uint32 *)0xff00003c)
#define QACR_AREA 0x1c
#define QACR_AREA_SHIFT 24
/* the store queue area, and an address's bits within its 64 MB area */
#define SQ_AREA 0xe0000000
#define SQ_OFFSET 0x03ffffff
/* point both store queues at the 64 MB area that holds address. out gets the
 * store queue address that writes there */
#define SELECT_QUEUE(out, address)                                             \
    do {                                                                       \
        Uint32 area = ((Uint32)(address) >> QACR_AREA_SHIFT) & QACR_AREA;      \
        QACR0 = area;                                                          \
        QACR1 = area;                                                          \
        (out) = (Uint8 *)(((Uint32)(address) & SQ_OFFSET) | SQ_AREA);          \
    } while (0)

/* the values of an output's subtract_at. where MPV_COND_LUMA_OFFSET is set,
 * the last LUMA_BLOCKS blocks, Y's, take mpvacmc.src's LUMA_OFFSET off each
 * sample. otherwise none does */
#define LUMA_BLOCKS 4
#define NO_BLOCKS (-1)
#define SUBTRACT_AT(mpv)                                                       \
    ((mpv)->cond[MPV_COND_LUMA_OFFSET] == 0 ? NO_BLOCKS : LUMA_BLOCKS)

/* a macroblock's byte offsets in Cb's and Cr's planes, and in Y's */
struct mb_offset {
    Sint32 chroma;
    Sint32 luma;
};

/* the parts of a motion vector, which mpvcmc_CalcMv splits from half pels.
 * the whole pels across and down come first, then the half pel left over
 * each way */
enum { MV_X, MV_Y, HALF_X, HALF_Y, MV_PARTS };

/* the assembly kernels, whose contracts mpvamc.src and the mpvacmc .src
 * files give. a predictor fills mc->block from mc's reference pixels */
typedef void (*predictor)(struct mpv_mc *);
void MPVAMC_OneRef1(struct mpv_mc *mc);
void MPVAMC_OneRefH2(struct mpv_mc *mc);
void MPVAMC_OneRefV2(struct mpv_mc *mc);
void MPVAMC_OneRef4(struct mpv_mc *mc);
void MPVACMC_OutputIntra6blk(const Sint16 *samples, Uint8 *out,
                             const struct mpv_output *output,
                             const Uint8 *clip);
void MPVACMC_OutputMbRfb(const Uint8 *blocks, const struct mb_offset *offset,
                         const struct mpv_frame *frame);
void MPVACMC_OneMakeMbTaSub(Uint8 *out, const struct mpv_mb *mb, Uint32 cbp);
void MPVACMC_BiMakeMbTaSub(Uint8 *out, const struct mpv_mb *mb, Uint32 cbp);
Uint8 *MPVACMC_PpicSkipBlk(struct mpv_mc *mc, Uint8 *block);

/* the predictors, by [MPV_COND_HALF_PEL_2][half_x + 2 * half_y]. with the
 * condition set, a half pel both ways averages two pixels, not four. the two
 * are a pixel and the one below and right of it */
static const predictor predictors[2][4] = {
    {MPVAMC_OneRef1, MPVAMC_OneRefH2, MPVAMC_OneRefV2, MPVAMC_OneRef4},
    {MPVAMC_OneRef1, MPVAMC_OneRefH2, MPVAMC_OneRefV2, MPVAMC_OneRefV2},
};

const Uint8 *mpv_clip_0_255_base;
static Uint8 clip_table[CLIP_MARGIN + PIXEL_LEVELS + CLIP_MARGIN];

static void mpvcmc_InitClip0255(void);
static void mpvcmc_InitMcOiTa(struct mpv_obj *mpv);
static void mpvcmc_OneReadMb(struct mpv_obj *mpv, Uint8 *block,
                             struct mb_offset *offset, struct mpv_frame *frame,
                             const struct mpv_motion *motion);
static void mpvcmc_OneMakeMbTa(struct mpv_obj *mpv, const struct mpv_mb *mb);
static void mpvcmc_BiMakeMbTa(struct mpv_obj *mpv, const struct mpv_mb *mb);
static void mpvcmc_CalcOfs(struct mpv_obj *mpv, Sint32 index,
                           struct mb_offset *offset,
                           const struct mpv_frame *frame);
static void mpvcmc_CalcMv(Sint32 x, Sint32 y, Sint32 *parts);

void MPVCMC_Init(Uint8 *clip)
{
    mpvcmc_InitClip0255();
    if (clip != NULL) {
        UTY_MemcpyDword((Uint32 *)clip, (const Uint32 *)clip_table,
                        UTY_DWORDS(clip_table));
        mpv_clip_0_255_base = clip + CLIP_MARGIN;
    }
}

static void mpvcmc_InitClip0255(void)
{
    Uint8 *entry = clip_table;
    Sint32 i;

    for (i = 0; i < CLIP_MARGIN; i++) {
        *entry++ = 0;
    }
    for (i = 0; i < PIXEL_LEVELS; i++) {
        *entry++ = i;
    }
    for (i = 0; i < CLIP_MARGIN; i++) {
        *entry++ = PIXEL_LEVELS - 1;
    }
    mpv_clip_0_255_base = clip_table + CLIP_MARGIN;
}

void MPVCMC_InitObj(struct mpv_obj *mpv)
{
    MPVMC_Init(&mpv->mc);
    mpvcmc_InitMcOiTa(mpv);
    /* @hack redundant: a return at the function's end */
    return; /* NOLINT(readability-redundant-control-flow) */
}

/* MPVACMC_OutputIntra6blk writes each block both to the texture and to its
 * output's block. for MPVCMC_IntraRt that block lies in the reference
 * picture. MPVCMC_IntraTa wants the texture alone, so texture_output's six
 * blocks all go to the start of mb_work, as scratch */
static void mpvcmc_InitMcOiTa(struct mpv_obj *mpv)
{
    struct mpv_output *out = &mpv->texture_output;
    struct mpv_out_block *block = out->blocks;
    Uint8 *scratch = mpv->mb_work;

    out->subtract_at = SUBTRACT_AT(mpv);
    block[0].start = scratch;
    block[1].start = scratch;
    block[2].start = scratch;
    block[3].start = scratch;
    block[4].start = scratch;
    block[5].start = scratch;
    block[0].pitch = BLOCK_SIDE;
    block[1].pitch = BLOCK_SIDE;
    block[2].pitch = BLOCK_SIDE;
    block[3].pitch = BLOCK_SIDE;
    block[4].pitch = BLOCK_SIDE;
    block[5].pitch = BLOCK_SIDE;
}

void MPVCMC_InitMcOiRt(struct mpv_obj *mpv)
{
    struct mpv_output *out = &mpv->reference_output;
    struct mpv_out_block *block = out->blocks;
    struct mpv_request *request = &mpv->request;
    struct mpv_frame *frame = &request->second;
    Sint32 pitch;

    out->subtract_at = SUBTRACT_AT(mpv);
    pitch = frame->chroma_pitch;
    block[0].pitch = pitch;
    block[1].pitch = pitch;
    pitch = frame->luma_pitch;
    block[2].pitch = pitch;
    block[3].pitch = pitch;
    block[4].pitch = pitch;
    block[5].pitch = pitch;
}

void MPVCMC_SetCcnt(struct mpv_obj *mpv)
{
    struct mpv_output *texture = &mpv->texture_output;
    struct mpv_output *reference = &mpv->reference_output;
    Sint32 subtract_at = SUBTRACT_AT(mpv);

    texture->subtract_at = subtract_at;
    reference->subtract_at = subtract_at;
}

#pragma section SFD00

MPV_CODE(PSGSFD00)
void MPVCMC_IntraRt(struct mpv_obj *mpv)
{
    Sint32 mask = get_imask();
    Uint8 *address;
    Uint8 *write;
    struct mpv_request *request;
    struct mpv_frame *frame;
    struct mb_offset offset;
    struct mpv_output *out;
    struct mpv_out_block *block;
    Sint32 pitch;

    /* @hack redundant: write's address taken, and nothing done with it */
    (void)&write;
    set_imask(IMASK_ALL);
    request = &mpv->request;
    frame = &request->second;
    out = &mpv->reference_output;
    block = out->blocks;
    address = request->output;
    request->output = address + MPV_MB_BYTES;
    SELECT_QUEUE(write, address);
    mpvcmc_CalcOfs(mpv, mpv->index, &offset, frame);
    pitch = frame->luma_pitch;
    block[0].start = frame->chroma_b + offset.chroma;
    block[1].start = frame->chroma_r + offset.chroma;
    block[2].start = frame->luma + offset.luma;
    block[3].start = block[2].start + BLOCK_SIDE;
    block[4].start = block[2].start + (BLOCK_SIDE * pitch);
    block[5].start = block[4].start + BLOCK_SIDE;
    MPVACMC_OutputIntra6blk(mpv->blocks[0], write, out, mpv->clip);
    set_imask(mask);
}

MPV_CODE(PSGSFD00)
void MPVCMC_IntraTa(struct mpv_obj *mpv)
{
    Sint32 mask = get_imask();
    Uint8 *address;
    Uint8 *write;
    struct mpv_request *request;

    /* @hack redundant: write's address taken, and nothing done with it */
    (void)&write;
    set_imask(IMASK_ALL);
    request = &mpv->request;
    address = request->output;
    request->output = address + MPV_MB_BYTES;
    SELECT_QUEUE(write, address);
    MPVACMC_OutputIntra6blk(mpv->blocks[0], write, &mpv->texture_output,
                            mpv->clip);
    set_imask(mask);
}

MPV_CODE(PSGSFD00)
void MPVCMC_ForwardRt(struct mpv_obj *mpv)
{
    struct mb_offset offset;
    struct mpv_mb *mb = &mpv->mb;
    struct mpv_request *request = &mpv->request;

    mpvcmc_OneReadMb(mpv, mb->prediction, &offset, &request->first,
                     &mpv->forward);
    mpvcmc_OneMakeMbTa(mpv, mb);
    MPVACMC_OutputMbRfb(mb->prediction, &offset, &request->second);
}

MPV_CODE(PSGSFD00)
void MPVCMC_ForwardTa(struct mpv_obj *mpv)
{
    struct mb_offset offset;
    struct mpv_mb *mb = &mpv->mb;
    struct mpv_request *request = &mpv->request;

    mpvcmc_OneReadMb(mpv, mb->prediction, &offset, &request->first,
                     &mpv->forward);
    mpvcmc_OneMakeMbTa(mpv, mb);
}

MPV_CODE(PSGSFD00)
void MPVCMC_Backward(struct mpv_obj *mpv)
{
    struct mb_offset offset;
    struct mpv_mb *mb = &mpv->mb;
    struct mpv_request *request = &mpv->request;

    mpvcmc_OneReadMb(mpv, mb->prediction, &offset, &request->second,
                     &mpv->backward);
    mpvcmc_OneMakeMbTa(mpv, mb);
}

MPV_CODE(PSGSFD00)
void MPVCMC_BiDirect(struct mpv_obj *mpv)
{
    struct mb_offset offset;
    struct mpv_mb *mb = &mpv->mb;
    struct mpv_request *request = &mpv->request;

    mpvcmc_OneReadMb(mpv, mb->prediction, &offset, &request->first,
                     &mpv->forward);
    mpvcmc_OneReadMb(mpv, mb->backward_prediction, &offset, &request->second,
                     &mpv->backward);
    mpvcmc_BiMakeMbTa(mpv, mb);
}

/* predict the current macroblock into block from frame, displaced by motion.
 * block takes Cb's block, Cr's, then Y's four, MPV_COEFS pixels each.
 * *offset gets the macroblock's offsets */
MPV_CODE(PSGSFD00)
static void mpvcmc_OneReadMb(struct mpv_obj *mpv, Uint8 *block,
                             struct mb_offset *offset, struct mpv_frame *frame,
                             const struct mpv_motion *motion)
{
    Sint32 two_samples = mpv->cond[MPV_COND_HALF_PEL_2];
    Sint32 luma_pitch = frame->luma_pitch;
    Sint32 chroma_pitch = frame->chroma_pitch;
    Sint32 x;
    Sint32 y;
    Sint32 chroma_offset;
    Sint32 luma_offset;
    Sint32 chroma[MV_PARTS];
    Sint32 luma[MV_PARTS];
    predictor predict_chroma;
    predictor predict_luma;
    struct mpv_mc *mc;
    const Uint8 *source;

    mpvcmc_CalcOfs(mpv, mpv->index, offset, frame);
    x = motion->x;
    y = motion->y;
    /* Cb and Cr move half as far as Y */
    mpvcmc_CalcMv(x / 2, y / 2, chroma);
    chroma_offset =
        offset->chroma + chroma[MV_X] + (chroma[MV_Y] * chroma_pitch);
    mpvcmc_CalcMv(x, y, luma);
    luma_offset = offset->luma + luma[MV_X] + (luma[MV_Y] * luma_pitch);
    predict_chroma =
        predictors[two_samples][chroma[HALF_X] + (2 * chroma[HALF_Y])];
    predict_luma = predictors[two_samples][luma[HALF_X] + (2 * luma[HALF_Y])];
    mc = &mpv->mc;

    mc->pitch = chroma_pitch;
    mc->block = block;
    source = frame->chroma_b + chroma_offset;
    mc->src = source;
    /* mc->src2 lies a row below mc->src, and with two_samples set,
     * chroma[HALF_X] more to the right */
    source += (two_samples ? chroma[HALF_X] : 0) + chroma_pitch;
    mc->src2 = source;
    predict_chroma(mc);
    mc->block = block + MPV_COEFS;
    source = frame->chroma_r + chroma_offset;
    mc->src = source;
    source += (two_samples ? chroma[HALF_X] : 0) + chroma_pitch;
    mc->src2 = source;
    predict_chroma(mc);

    mc->pitch = luma_pitch;
    mc->block = block + (2 * MPV_COEFS);
    source = frame->luma + luma_offset;
    mc->src = source;
    source += (two_samples ? luma[HALF_X] : 0) + luma_pitch;
    mc->src2 = source;
    predict_luma(mc);
    mc->block = block + (3 * MPV_COEFS);
    mc->src += BLOCK_SIDE;
    mc->src2 += BLOCK_SIDE;
    predict_luma(mc);
    mc->block = block + (4 * MPV_COEFS);
    mc->src += (luma_pitch - 1) * BLOCK_SIDE;
    mc->src2 += (luma_pitch - 1) * BLOCK_SIDE;
    predict_luma(mc);
    mc->block = block + (5 * MPV_COEFS);
    mc->src += BLOCK_SIDE;
    mc->src2 += BLOCK_SIDE;
    predict_luma(mc);
}

MPV_CODE(PSGSFD00)
static void mpvcmc_OneMakeMbTa(struct mpv_obj *mpv, const struct mpv_mb *mb)
{
    Sint32 mask = get_imask();
    Uint8 *address;
    Uint8 *write;
    struct mpv_request *request;

    /* @hack redundant: write's address taken, and nothing done with it */
    (void)&write;
    set_imask(IMASK_ALL);
    request = &mpv->request;
    address = request->output;
    request->output = address + MPV_MB_BYTES;
    SELECT_QUEUE(write, address);
    MPVACMC_OneMakeMbTaSub(write, mb, mpv->cbp);
    set_imask(mask);
}

MPV_CODE(PSGSFD00)
static void mpvcmc_BiMakeMbTa(struct mpv_obj *mpv, const struct mpv_mb *mb)
{
    Sint32 mask = get_imask();
    Uint8 *address;
    Uint8 *write;
    struct mpv_request *request;

    /* @hack redundant: write's address taken, and nothing done with it */
    (void)&write;
    set_imask(IMASK_ALL);
    request = &mpv->request;
    address = request->output;
    request->output = address + MPV_MB_BYTES;
    SELECT_QUEUE(write, address);
    MPVACMC_BiMakeMbTaSub(write, mb, mpv->cbp);
    set_imask(mask);
}

/* count is the macroblock address increment, so the count - 1 macroblocks
 * before mpv->index were skipped. a skipped macroblock of a P picture is its
 * forward reference's, unmoved. each goes to the texture and into the
 * reference picture being made */
MPV_CODE(PSGSFD00)
void MPVCMC_PpicSkipped(struct mpv_obj *mpv, Sint32 count)
{
    Uint8 *block = mpv->mb_work;
    struct mpv_request *request = &mpv->request;
    struct mpv_frame *frame = &request->first;
    struct mpv_frame *output = &request->second;
    struct mpv_mc *mc = &mpv->mc;
    Sint32 luma_pitch = frame->luma_pitch;
    Sint32 chroma_pitch = frame->chroma_pitch;
    Uint8 *write = request->output;
    struct mb_offset offset;

    request->output = write + (count * MPV_MB_BYTES) - MPV_MB_BYTES;
    while (--count) {
        mpvcmc_CalcOfs(mpv, mpv->index - count, &offset, frame);
        mc->skip_pitch = chroma_pitch;
        mc->out = write;
        mc->src = frame->chroma_b + offset.chroma;
        write = MPVACMC_PpicSkipBlk(mc, block);
        block += MPV_COEFS;
        mc->out = write;
        mc->src = frame->chroma_r + offset.chroma;
        write = MPVACMC_PpicSkipBlk(mc, block);
        block += MPV_COEFS;

        mc->skip_pitch = luma_pitch;
        mc->out = write;
        mc->src = frame->luma + offset.luma;
        write = MPVACMC_PpicSkipBlk(mc, block);
        block += MPV_COEFS;
        mc->out = write;
        mc->src += BLOCK_SIDE;
        write = MPVACMC_PpicSkipBlk(mc, block);
        block += MPV_COEFS;
        mc->out = write;
        mc->src += (luma_pitch - 1) * BLOCK_SIDE;
        write = MPVACMC_PpicSkipBlk(mc, block);
        block += MPV_COEFS;
        mc->out = write;
        mc->src += BLOCK_SIDE;
        write = MPVACMC_PpicSkipBlk(mc, block);
        block = mpv->mb_work;
        MPVACMC_OutputMbRfb(block, &offset, output);
    }
}

/* count - 1 macroblocks were skipped, as in MPVCMC_PpicSkipped. a skipped
 * macroblock of a B picture repeats the prediction before it, with no coded
 * blocks. makers[0] holds the maker the last non-intra macroblock of a B
 * picture took */
MPV_CODE(PSGSFD00)
void MPVCMC_BpicSkipped(struct mpv_obj *mpv, Sint32 count)
{
    Sint32 end = mpv->index;
    Sint32 index;
    MPV_MB_MAKE make;

    mpv->cbp = 0;
    make = mpv->makers[0];
    index = end - count + 1;
    while (index < end) {
        mpv->index = index;
        make(mpv);
        index++;
    }
    mpv->index = end;
}

/* *offset gets macroblock index's byte offsets in frame's planes */
MPV_CODE(PSGSFD00)
static void mpvcmc_CalcOfs(struct mpv_obj *mpv, Sint32 index,
                           struct mb_offset *offset,
                           const struct mpv_frame *frame)
{
    Sint32 row;
    Sint32 column;
    const Sint8 *position = mpv->mbpos[index];
    Sint32 chroma_pitch = frame->chroma_pitch;
    Sint32 luma_pitch = frame->luma_pitch;

    row = position[MPV_MB_ROW] * BLOCK_SIDE;
    column = position[MPV_MB_COLUMN] * BLOCK_SIDE;
    offset->chroma = (row * chroma_pitch) + column;
    /* Y has twice the pixels of Cb and Cr each way */
    /* @hack spelling: row and column doubled by unsigned shifts, where
     * row * 2 and column * 2 would do */
    row = (Uint32)row << 1;
    column = (Uint32)column << 1;
    offset->luma = (row * luma_pitch) + column;
}

MPV_CODE(PSGSFD00)
static void mpvcmc_CalcMv(Sint32 x, Sint32 y, Sint32 *parts)
{
    parts[MV_X] = x >> 1;
    parts[MV_Y] = y >> 1;
    parts[HALF_X] = x & 1;
    parts[HALF_Y] = y & 1;
}
#pragma section
