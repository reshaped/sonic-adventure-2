/* the basic decoder. ADX data goes through the block expander. each other
 * format's step, in a source of its own, copies or converts its samples into
 * the rings */
#include <adx_bfmt.h>
#include <adx_bsc.h>
#include <adx_dcd.h>
#include <adx_xpnd.h>
#include <sg_xpt.h>
#include <string.h>

#define ADXB_MAX_OBJ 8

static struct adxb_obj adxb_obj[ADXB_MAX_OBJ];

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void ADXB_EntryAddWrFunc(ADXB adxb, ADXB_ADDWR func, void *obj);
ADXPD ADXB_GetAdxpd(ADXB adxb);
void ADXB_ExecServer(void);
/* NOLINTEND(misc-use-internal-linkage) */

void ADXB_Init(void)
{
    ADXPD_Init();
    memset(adxb_obj, 0, sizeof(adxb_obj));
}

static Sint16 *adxb_DefGetWr(void *obj, Sint32 *position, Sint32 *room,
                             Sint32 *left)
{
    ADXB adxb = obj;

    *position = adxb->wpos;
    *room = adxb->pcmbsize - adxb->wpos;
    *left = adxb->total - adxb->nsmpl;
    return adxb->pcmbuf;
}

static void adxb_DefAddWr(void *obj, Sint32 bytes, Sint32 samples)
{
    ADXB adxb = obj;

    adxb->wpos += samples;
    adxb->nsmpl += samples;
}

ADXB ADXB_Create(Sint32 maxnch, Sint16 *pcmbuf, Sint32 pcmbsize,
                 Sint32 pcmbdist)
{
    ADXB adxb;
    Sint32 i;

    for (i = 0; i < ADXB_MAX_OBJ; i++) {
        if (!adxb_obj[i].used) {
            break;
        }
    }
    if (i == ADXB_MAX_OBJ) {
        return NULL;
    }
    adxb = &adxb_obj[i];
    memset(adxb, 0, sizeof(*adxb));
    adxb->used = TRUE;
    adxb->xpd = ADXPD_Create();
    if (!adxb->xpd) {
        ADXB_Destroy(adxb);
        return NULL;
    }
    adxb->maxnch = maxnch;
    adxb->pcmbuf = pcmbuf;
    adxb->pcmbsize = pcmbsize;
    adxb->pcmbdist = pcmbdist;
    adxb->get_wr = adxb_DefGetWr;
    adxb->get_obj = adxb;
    adxb->add_wr = adxb_DefAddWr;
    adxb->add_obj = adxb;
    return adxb;
}

void ADXB_Destroy(ADXB adxb)
{
    if (adxb) {
        ADXPD_Destroy(adxb->xpd);
        memset(adxb, 0, sizeof(*adxb));
        /* @hack redundant: used cleared, which the memset has cleared */
        adxb->used = FALSE;
    }
}

static Sint32 ADXB_DecodeHeaderAdx(ADXB adxb, Sint8 *header, Sint32 size)
{
    Sint16 offset;
    Sint32 status;

    adxb->unk_02 = 1;
    status = ADX_DecodeInfo(header, size, &offset, &adxb->enc, &adxb->bps,
                            &adxb->blklen, &adxb->nch, &adxb->sfreq,
                            &adxb->total, &adxb->blksmpl);
    if (status < 0) {
        return 0;
    }
    status = ADX_DecodeInfoExADPCM2(header, size, &adxb->cof);
    if (status < 0) {
        return 0;
    }
    ADXPD_SetCoef(adxb->xpd, adxb->sfreq, adxb->cof);
    ADX_DecodeInfoExLoop(header, size, &adxb->lp_ins_nsmpl, &adxb->nloop,
                         &adxb->lp_type, &adxb->lp_start_pos,
                         &adxb->lp_start_ofst, &adxb->lp_end_pos,
                         &adxb->lp_end_ofst);
    adxb->dec.nch = adxb->nch;
    adxb->dec.blklen = adxb->blklen;
    adxb->dec.blksmpl = adxb->blksmpl;
    adxb->dec.obuf = adxb->pcmbuf;
    adxb->dec.obufsize = adxb->pcmbsize;
    adxb->dec.obufdist = adxb->pcmbdist;
    adxb->dec.coef[0] = adxb->k0;
    adxb->dec.coef[1] = adxb->k1;
    adxb->dec.dly = adxb->dly;
    adxb->wpos = 0;
    adxb->format = ADXB_FORMAT_ADX;
    memset(adxb->dly, 0, sizeof(adxb->dly));
    return offset;
}

Sint32 ADXB_DecodeHeader(ADXB adxb, Sint8 *header, Sint32 size)
{
    /* @hack type: the mark read as an aligned Uint16 and its bytes swapped,
     * where a read of its two bytes would do for an even header */
    Uint16 mark = ((*(const Uint16 *)header >> 8) & 0xff) |
                  ((*(const Uint16 *)header << 8) & 0xff00);

    if (mark == ADX_MARK) {
        return ADXB_DecodeHeaderAdx(adxb, header, size);
    } else if (ADXB_CheckSpsd((Uint8 *)header)) {
        return ADXB_DecodeHeaderSpsd(adxb, (Uint8 *)header, size);
    } else if (ADXB_CheckWav((Uint8 *)header)) {
        return ADXB_DecodeHeaderWav(adxb, (Uint8 *)header, size);
    } else if (ADXB_CheckAiff(header)) {
        return ADXB_DecodeHeaderAiff(adxb, header, size);
    } else if (ADXB_CheckAu(header)) {
        return ADXB_DecodeHeaderAu(adxb, header, size);
    }
    return 0;
}

void ADXB_EntryGetWrFunc(ADXB adxb, ADXB_GETWR func, void *obj)
{
    adxb->get_wr = func;
    adxb->get_obj = obj;
}

void ADXB_EntryAddWrFunc(ADXB adxb, ADXB_ADDWR func, void *obj)
{
    adxb->add_wr = func;
    adxb->add_obj = obj;
}

Sint16 *ADXB_GetPcmBuf(ADXB adxb)
{
    return adxb->pcmbuf;
}

Sint32 ADXB_GetFormat(ADXB adxb)
{
    return adxb->format;
}

Sint32 ADXB_GetSfreq(ADXB adxb)
{
    return adxb->sfreq;
}

Sint32 ADXB_GetNumChan(ADXB adxb)
{
    return adxb->nch;
}

Sint32 ADXB_GetFmtBps(ADXB adxb)
{
    return adxb->bps;
}

Sint32 ADXB_GetOutBps(ADXB adxb)
{
    /* @hack redundant: the ADX test, which returns the 16 the last line
     * returns anyway */
    if (adxb->format == ADXB_FORMAT_ADX) {
        return 16;
    } else if (adxb->format == ADXB_FORMAT_SPSD) {
        if (adxb->pcm_encoding == ADXB_PCM4) {
            return 4;
        } else if (adxb->pcm_encoding == ADXB_PCM8) {
            return 8;
        }
    } else if (adxb->format == ADXB_FORMAT_WAV &&
               adxb->pcm_encoding == ADXB_PCM4) {
        return 4;
    }
    return 16;
}

Sint32 ADXB_GetBlkSmpl(ADXB adxb)
{
    return adxb->blksmpl;
}

Sint32 ADXB_GetBlkLen(ADXB adxb)
{
    return adxb->blklen;
}

Sint32 ADXB_GetTotalNumSmpl(ADXB adxb)
{
    return adxb->total;
}

Sint32 ADXB_GetCof(ADXB adxb)
{
    return adxb->cof;
}

Sint32 ADXB_GetLpInsNsmpl(ADXB adxb)
{
    return adxb->lp_ins_nsmpl;
}

Sint32 ADXB_GetNumLoop(ADXB adxb)
{
    return adxb->nloop;
}

Sint32 ADXB_GetLpStartPos(ADXB adxb)
{
    return adxb->lp_start_pos;
}

Sint32 ADXB_GetLpStartOfst(ADXB adxb)
{
    return adxb->lp_start_ofst;
}

Sint32 ADXB_GetLpEndPos(ADXB adxb)
{
    return adxb->lp_end_pos;
}

Sint32 ADXB_GetLpEndOfst(ADXB adxb)
{
    return adxb->lp_end_ofst;
}

Sint32 ADXB_GetStat(ADXB adxb)
{
    return adxb->stat;
}

void ADXB_EntryData(ADXB adxb, Sint8 *ibuf, Sint32 ibuflen)
{
    /* @hack spelling: ibuf set in each branch, where one store before the
     * test would do */
    if (adxb->format == ADXB_FORMAT_ADX) {
        adxb->dec.ibuf = ibuf;
        adxb->dec.nblk = ibuflen / adxb->blklen;
    } else {
        adxb->dec.ibuf = ibuf;
        adxb->dec.nblk = ibuflen / ((adxb->bps / 8) * adxb->nch);
    }
    adxb->dec.unk_34 = 0;
    adxb->dec_nsmpl = 0;
    adxb->dec_dtlen = 0;
}

void ADXB_Start(ADXB adxb)
{
    if (adxb->stat == ADXB_STAT_STOP) {
        adxb->stat = ADXB_STAT_PREP;
    }
}

void ADXB_Stop(ADXB adxb)
{
    ADXPD_Stop(adxb->xpd);
    adxb->stat = ADXB_STAT_STOP;
}

void ADXB_Reset(ADXB adxb)
{
    if (adxb->stat == ADXB_STAT_END) {
        ADXPD_Reset(adxb->xpd);
        adxb->wpos = 0;
        adxb->stat = ADXB_STAT_STOP;
    }
}

Sint32 ADXB_GetDecDtLen(ADXB adxb)
{
    return adxb->dec_dtlen;
}

Sint32 ADXB_GetDecNumSmpl(ADXB adxb)
{
    return adxb->dec_nsmpl;
}

ADXPD ADXB_GetAdxpd(ADXB adxb)
{
    return adxb->xpd;
}

static void ADXB_EvokeExpandMono(ADXB adxb, Sint32 blocks)
{
    ADXB_DEC *step = &adxb->dec;
    ADXPD expander = adxb->xpd;
    /* @hack spelling: ibuf read through adxb, where step would do */
    Sint8 *input = adxb->dec.ibuf;
    Sint16 *output = step->obuf;

    /* @hack spelling: output advanced in a statement of its own, where its
     * initializer would do */
    output += step->wpos;
    ADXPD_EntryMono(expander, input, blocks, output);
    ADXPD_Start(expander);
}

static void ADXB_EvokeExpandSte(ADXB adxb, Sint32 blocks)
{
    ADXB_DEC *step = &adxb->dec;
    ADXPD expander = adxb->xpd;
    /* @hack spelling: ibuf read through adxb, where step would do */
    Sint8 *input = adxb->dec.ibuf;
    Sint16 *left = step->obuf;
    Sint16 *right;

    /* @hack spelling: left advanced in a statement of its own, where its
     * initializer would do */
    left += step->wpos;
    right = left + step->obufdist;
    ADXPD_EntrySte(expander, input, 2 * blocks, left, right);
    ADXPD_Start(expander);
}

/* start the expander on as many blocks of each channel as the input, the
 * room and nleft allow. every block starts before the ring's end. the last
 * may run past it, and ADXB_EndDecode moves what it writes there to the
 * ring's start */
static void ADXB_EvokeDecode(ADXB adxb)
{
    ADXB_DEC *step = &adxb->dec;
    Sint32 blocks;
    Sint32 ring_blocks;
    Sint32 end_sample;
    Sint32 padding;
    Sint32 position;
    Sint32 room;
    Sint32 left;
    Sint32 samples_per_block;
    Sint32 left_blocks;
    Sint32 room_blocks;

    blocks = step->nblk / step->nch;
    ring_blocks = step->obufsize;
    position = step->wpos;
    room = step->nroom;
    samples_per_block = step->blksmpl;
    left = step->nleft;
    left_blocks = (left + samples_per_block - 1) / samples_per_block;
    padding = samples_per_block -
              ((left + samples_per_block - 1) % samples_per_block) - 1;
    ring_blocks -= position;
    ring_blocks = (ring_blocks + samples_per_block - 1) / samples_per_block;
    end_sample = ring_blocks * samples_per_block;
    /* @hack spelling: a nested if, where one test of both conditions would
     * do */
    if (left_blocks < ring_blocks) {
        if (end_sample - padding + position < step->obufsize) {
            ring_blocks++;
        }
    }
    if (left < room) {
        room += padding;
    }
    room_blocks = room / samples_per_block;
    if (blocks > room_blocks) {
        blocks = room_blocks;
    }
    if (blocks > left_blocks) {
        blocks = left_blocks;
    }
    if (blocks > ring_blocks) {
        blocks = ring_blocks;
    }
    if (step->nch == 2) {
        ADXB_EvokeExpandSte(adxb, blocks);
    } else {
        ADXB_EvokeExpandMono(adxb, blocks);
    }
}

static void memcpy2(Sint16 *destination, const Sint16 *source, Sint32 count)
{
    for (; count > 0; count--) {
        *destination++ = *source++;
    }
}

static void ADXB_CopyExtraBufSte(Sint16 *buffer, Sint32 size, Sint32 distance,
                                 Sint32 samples)
{
    memcpy2(buffer, buffer + size, samples);
    /* @hack spelling: size + distance in parentheses, where buffer + size +
     * distance would do */
    memcpy2(buffer + distance, buffer + (size + distance), samples);
}

static void ADXB_CopyExtraBufMono(Sint16 *buffer, Sint32 size, Sint32 distance,
                                  Sint32 samples)
{
    memcpy2(buffer, buffer + size, samples);
}

/* record the step the expander finished: dec_nsmpl gets its samples of each
 * channel, and dec_dtlen its input bytes. the block that holds the nleft-th
 * sample counts only up to that sample. samples written past the ring's end
 * go back to its start */
static void ADXB_EndDecode(ADXB adxb)
{
    ADXB_DEC *step = &adxb->dec;
    Sint32 block_samples = step->blksmpl;
    Sint32 block_bytes = step->blklen;
    Sint32 left = step->nleft;
    Sint32 position = step->wpos;
    Sint16 *buffer = step->obuf;
    Sint32 size = adxb->pcmbsize;
    Sint32 distance = adxb->pcmbdist;
    Sint32 left_blocks = (left + block_samples - 1) / block_samples;
    Sint32 padding =
        block_samples - ((left + block_samples - 1) % block_samples) - 1;
    Sint32 blocks = ADXPD_GetNumBlk(adxb->xpd);
    Sint32 samples = block_samples * blocks / step->nch;

    /* @bug logic: a stereo step of left_blocks / 2 or more blocks of each
     * channel, but fewer than left_blocks, leaves its last padding samples of
     * each channel uncounted, and the next step writes over them. blocks
     * counts both channels' blocks, and left_blocks one channel's: the test
     * at 0x8c16dfbe lacks the division by nch at 0x8c16dfb6 */
    if (left_blocks <= blocks) {
        samples -= padding;
    }
    /* @hack spelling: position = position + samples, where += would do */
    position = position + samples;
    adxb->dec_nsmpl = samples;
    adxb->dec_dtlen = block_bytes * blocks;
    if (position >= size) {
        position -= size;
        if (step->nch == 2) {
            ADXB_CopyExtraBufSte(buffer, size, distance, position);
        } else {
            ADXB_CopyExtraBufMono(buffer, size, distance, position);
        }
    }
}

static void ADXB_ExecOneAdx(ADXB adxb)
{
    ADXB_DEC *step = &adxb->dec;

    if (adxb->stat == ADXB_STAT_PREP &&
        ADXPD_GetStat(adxb->xpd) == ADXPD_STAT_STOP) {
        adxb->get_wr(adxb->get_obj, &step->wpos, &step->nroom, &step->nleft);
        ADXB_EvokeDecode(adxb);
        adxb->stat = ADXB_STAT_EXEC;
    }
    if (adxb->stat == ADXB_STAT_EXEC) {
        ADXPD_ExecHndl(adxb->xpd);
        if (ADXPD_GetStat(adxb->xpd) == ADXPD_STAT_END) {
            ADXB_EndDecode(adxb);
            ADXPD_Reset(adxb->xpd);
            adxb->add_wr(adxb->add_obj, adxb->dec_dtlen, adxb->dec_nsmpl);
            adxb->stat = ADXB_STAT_END;
        }
    }
}

void ADXB_ExecHndl(ADXB adxb)
{
    if (adxb->format == ADXB_FORMAT_ADX) {
        ADXB_ExecOneAdx(adxb);
    } else if (adxb->format == ADXB_FORMAT_SPSD) {
        ADXB_ExecOneSpsd(adxb);
    } else if (adxb->format == ADXB_FORMAT_AIFF) {
        ADXB_ExecOneAiff(adxb);
    } else if (adxb->format == ADXB_FORMAT_AU) {
        ADXB_ExecOneAu(adxb);
    } else if (adxb->format == ADXB_FORMAT_WAV) {
        ADXB_ExecOneWav(adxb);
    }
}

void ADXB_ExecServer(void)
{
    Sint32 i;
    ADXB adxb;

    for (i = 0; i < ADXB_MAX_OBJ; i++) {
        adxb = &adxb_obj[i];
        if (adxb->used == TRUE) {
            ADXB_ExecHndl(adxb);
        }
    }
}
