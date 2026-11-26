/* the sound output's renderers (mw_rna.h). joints, chunks and their lines are
 * sj.h's. each channel's ring is its sound buffer's (mw_sdsb.h). while
 * transfer is on, a pass of mwRnaExecServer with no copy running takes a chunk
 * from each joint. that pass starts copying the chunks into the rings at
 * wrpos. a later pass finds the copy finished. it puts the bytes copied on
 * each joint's free line, and the rest back on its data line. while play is on
 * and transfer off, the passes copy silence after the last data instead. they
 * stop once a ring's length of silence is copied. a renderer's positions and
 * counts are in samples of one channel */
#include <cri_adxt.h>
#include <mw_rna.h>
#include <mw_sdsb.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <sj.h>
#include <string.h>

#define RENDERERS 8
#define MAX_CHANNELS 2
/* a stopsmpl that ndata never reaches, so that MW_RNA_SW_AUTO turns play on
 * once */
#define NO_AUTO_PLAY 0x7fffffff
/* a block is DMA_BYTES bytes of one channel's samples. copies run in whole
 * blocks, and rdpos stays on a block boundary */
#define DMA_BYTES 32
#define BYTE_BITS 8
#define ADPCM_BITS 4
/* the most bytes of silence one copy takes */
#define SILENCE_BYTES 2048
#define SILENCE_WORDS (SILENCE_BYTES / (Sint32)sizeof(Uint32))
/* four bytes of silence in 4-bit samples */
#define ADPCM_SILENCE 0x80808080UL
/* a wait for a copy tests it up to WAIT_POLLS times, WAIT_SPINS empty turns
 * of a loop apart */
#define WAIT_POLLS 200
#define WAIT_SPINS 5000
/* a new renderer's bits a sample, its rate in hertz, and its startsmpl */
#define DEFAULT_BITS 16
#define DEFAULT_SFREQ 44100
#define DEFAULT_START_SAMPLES 8192
#define MIN_VOLUME (-999)
#define FX_CHANNEL_MAX 16
#define BAD_HANDLE "E1205:Illigal parameter (MWRNA=NULL)"

static const char *volatile mwg_rna_build =
    "\nmwRna Ver.2.01 Build:Apr 13 2000 18:20:09\n";
static Sint32 mwg_rna_init_cnt = 0;
static struct mw_sblib_vtbl *mwg_sndlib_vtbl = NULL;
static void *mwg_sndlib_obj = NULL;
/* no code reads these counts. mwg_rna_wait_cnt counts the polls of the last
 * wait for a copy. mwg_rna_not_end_cnt counts the server passes in a row that
 * found a copy unfinished */
static Sint32 mwg_rna_wait_cnt = 0;
static Sint32 mwg_rna_not_end_cnt = 0;
static struct _adxrna_obj mwg_rna_obj[RENDERERS];
/* the silence a copy reads: mwg_rna_zero's zeros for 8-bit and 16-bit
 * samples, and mwg_rna_zero_adpcm's ADPCM_SILENCE for 4-bit ones. each holds
 * a block more than SILENCE_BYTES. a copy skips to the first block boundary
 * past the buffer's start */
static Uint32 mwg_rna_zero[(SILENCE_BYTES + DMA_BYTES) / sizeof(Uint32)];
static Uint32 mwg_rna_zero_adpcm[(SILENCE_BYTES + DMA_BYTES) / sizeof(Uint32)];
/* the timer's counts around the last call that starts a copy of samples, for
 * its last channel */
static Uint32 mwg_rna_tcnt[2];

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void mwRnaSetupSnd(struct mw_sblib_vtbl *port, void *obj);
void mwRnaFlush(ADXRNA rna);
void mwRnaPause(ADXRNA rna);
void mwRnaContinue(ADXRNA rna);
Sint32 mwRnaIsPause(ADXRNA rna);
Sint32 mwRnaGetTransSw(ADXRNA rna);
Sint32 mwRnaGetPlaySw(ADXRNA rna);
Sint32 mwRnaGetAutoSw(ADXRNA rna);
Sint32 mwRnaGetNumRoom(ADXRNA rna);
Sint32 mwRnaIsOvrflw(ADXRNA rna);
void mwRnaClearOvrflwFlg(ADXRNA rna);
/* NOLINTEND(misc-use-internal-linkage) */

static void mwlRnaSetSvrInt(void)
{
}

void mwRnaInit(void)
{
    Sint32 i;
    Uint32 *silence;
    Sint32 words;

    /* @hack redundant: the read of mwg_rna_build, which nothing uses, and
     * the volatile on mwg_rna_build that keeps it */
    (void)mwg_rna_build;
    if (mwg_rna_init_cnt == 0) {
        mwRnaInitCs();
        memset(mwg_rna_obj, 0, sizeof(mwg_rna_obj));
        memset(mwg_rna_zero, 0, sizeof(mwg_rna_zero));
        silence = mwg_rna_zero_adpcm;
        /* @hack spelling: the count held in words, where the loop could test
         * SILENCE_WORDS */
        words = SILENCE_WORDS;
        for (i = 0; i < words; i++) {
            *silence++ = ADPCM_SILENCE;
        }
        if (mwg_sndlib_vtbl == NULL) {
            mwg_sndlib_vtbl = mwSndGetLibHn();
        }
        mwg_sndlib_vtbl->init(mwg_sndlib_obj);
        mwlRnaSetSvrInt();
        mwRnaErrEntryFunc(NULL, NULL);
    }
    mwg_rna_init_cnt++;
}

void mwRnaFinish(void)
{
    Sint32 i;
    Uint32 *silence;
    Sint32 words;

    mwg_rna_init_cnt--;
    if (mwg_rna_init_cnt == 0) {
        for (i = 0; i < RENDERERS; i++) {
            if (mwg_rna_obj[i].used == TRUE) {
                mwRnaStop(&mwg_rna_obj[i]);
            }
        }
        for (i = 0; i < RENDERERS; i++) {
            if (mwg_rna_obj[i].used == TRUE) {
                mwRnaDestroy(&mwg_rna_obj[i]);
            }
        }
        memset(mwg_rna_obj, 0, sizeof(mwg_rna_obj));
        memset(mwg_rna_zero, 0, sizeof(mwg_rna_zero));
        silence = mwg_rna_zero_adpcm;
        /* @hack spelling: the count held in words, where the loop could test
         * SILENCE_WORDS */
        words = SILENCE_WORDS;
        for (i = 0; i < words; i++) {
            *silence++ = ADPCM_SILENCE;
        }
        mwg_sndlib_vtbl->finish();
        mwRnaFinishCs();
    }
}

void mwRnaSetupSnd(struct mw_sblib_vtbl *port, void *obj)
{
    mwg_sndlib_vtbl = port;
    mwg_sndlib_obj = obj;
}

static void mwlRnaSetInitMember(ADXRNA rna)
{
    rna->stat = 0;
    rna->ovrflw = FALSE;
    rna->copying = FALSE;
    rna->flush = FALSE;
    rna->ndata = 0;
    rna->wrpos = 0;
    rna->rdpos = 0;
    rna->stopsmpl = DEFAULT_START_SAMPLES;
    rna->startsmpl = DEFAULT_START_SAMPLES;
    rna->nplay = 0;
    rna->outvol = 0;
    rna->fxch = 0;
    rna->fxlev = MW_SB_FX_LVL_OPEN;
    rna->semi = 0;
    rna->fine = 0;
    rna->coff = 0;
    rna->q = 0;
    rna->drctlvl = 0;
    rna->hold = FALSE;
    rna->pause = FALSE;
    rna->swsave = 0;
}

static ADXRNA mwlRnaAlloc(void)
{
    ADXRNA rna = NULL;
    Sint32 i;

    for (i = 0; i < RENDERERS; i++) {
        if (!mwg_rna_obj[i].used) {
            rna = &mwg_rna_obj[i];
            break;
        }
    }
    return rna;
}

ADXRNA mwRnaCreate(SJ *joints, Sint32 channels)
{
    ADXRNA rna;
    Sint32 mask;
    Sint32 i;
    SJ *unused_joints;

    if (channels < 1 || channels > MAX_CHANNELS) {
        mwRnaErrCallFunc("E1201:mwRnaCreate Illigal parameter (MAXNCH)");
        return NULL;
    }
    if (joints == NULL) {
        mwRnaErrCallFunc("E1202:mwRnaCreate Illigal parameter (SJ)");
        return NULL;
    }
    /* @hack redundant: unused_joints, a copy of joints that nothing reads */
    unused_joints = joints;
    (void)unused_joints;
    for (i = 0; i < channels; i++) {
        if (joints[i] == NULL) {
            mwRnaErrCallFunc("E1202:mwRnaCreate Illigal parameter (SJ)");
            return NULL;
        }
    }
    mwRnaLockCs(&mask);
    rna = mwlRnaAlloc();
    if (rna == NULL) {
        mwRnaErrCallFunc("E1203:mwRnaCreate Not enough instance (MWRNA)");
    } else {
        rna->nch = channels;
        rna->maxnch = channels;
        for (i = 0; i < channels; i++) {
            rna->sj[i] = joints[i];
        }
        rna->snd = mwg_sndlib_vtbl->open_port(channels);
        if (rna->snd == NULL) {
            mwRnaErrCallFunc("E1204:mwRnaCreate Cannot open MWSND");
            rna = NULL;
        } else {
            mwlRnaSetInitMember(rna);
            mwRnaSetBitPerSmpl(rna, DEFAULT_BITS);
            mwRnaSetSfreq(rna, DEFAULT_SFREQ);
            mwRnaSetNumChan(rna, rna->nch);
            rna->bufsize = rna->snd->vtbl->get_buf_size(rna->snd);
            rna->used = TRUE;
        }
    }
    mwRnaUnlockCs(&mask);
    return rna;
}

void mwRnaDestroy(ADXRNA rna)
{
    Sint32 mask;
    Sint32 i;

    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    mwRnaLockCs(&mask);
    if (rna->stat != 0) {
        mwRnaStop(rna);
    }
    rna->used = FALSE;
    for (i = 0; i < rna->maxnch; i++) {
        rna->sj[i] = NULL;
    }
    rna->snd->vtbl->close_port(rna->snd);
    memset(rna, 0, sizeof(*rna));
    mwRnaUnlockCs(&mask);
}

void mwRnaStart(ADXRNA rna)
{
    Sint32 mask;

    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    mwRnaLockCs(&mask);
    if (rna->stat != 0) {
        mwRnaStop(rna);
    }
    rna->stat = MW_RNA_SW_AUTO;
    rna->stopsmpl = rna->startsmpl;
    mwRnaSetTransSw(rna, TRUE);
    mwRnaUnlockCs(&mask);
}

static void mwlRnaClearBuf(ADXRNA rna)
{
}

void mwRnaStop(ADXRNA rna)
{
    Sint32 mask;

    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    if (rna->stat != 0) {
        mwRnaLockCs(&mask);
        mwRnaSetPlaySw(rna, FALSE);
        mwRnaSetTransSw(rna, FALSE);
        mwlRnaClearBuf(rna);
        rna->pause = FALSE;
        rna->stat = 0;
        mwRnaUnlockCs(&mask);
    }
}

void mwRnaFlush(ADXRNA rna)
{
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    if (rna->flush != TRUE) {
        rna->flush = TRUE;
    }
}

static Sint32 mwlRnaStartCopy(ADXRNA rna, Sint32 channel, Sint32 pos,
                              void *source, Sint32 samples)
{
    if (samples <= 0) {
        return 0;
    }
    rna->snd->vtbl->trns(rna->snd, channel, pos, source, samples);
    rna->trnsing = TRUE;
    return samples;
}

static Bool mwlRnaIsCopyEnd(ADXRNA rna)
{
    Bool done[MAX_CHANNELS];
    Bool complete;
    Sint32 status;
    Sint32 i;

    if (!rna->trnsing) {
        return FALSE;
    }
    complete = TRUE;
    /* @hack spelling: both flags set in one assignment, where two would do */
    done[0] = done[1] = TRUE;
    for (i = 0; i < rna->nch; i++) {
        status = rna->snd->vtbl->get_trns_stat(rna->snd, i);
        if (status != TRUE) {
            done[i] = FALSE;
        }
    }
    if (!done[0] || !done[1]) {
        complete = FALSE;
    }
    if (complete == TRUE) {
        rna->trnsing = FALSE;
    }
    return complete;
}

/* move rdpos to channel 0's play position, rounded down to a block. the
 * samples played go to nplay, which no code reads. a play past the data
 * leaves ndata 0. with transfer on, it also moves wrpos to rdpos and sets
 * ovrflw */
static void mwlRnaAddRdPos(ADXRNA rna)
{
    Sint32 mask;
    Sint32 previous;
    Sint32 position;
    Sint32 block;
    Sint32 played;
    Sint32 remaining;

    mwRnaLockCs(&mask);
    previous = rna->rdpos;
    position = rna->snd->vtbl->get_play_pos(rna->snd, 0);
    block = DMA_BYTES / rna->bps * BYTE_BITS;
    rna->rdpos = position / block * block;
    played = rna->rdpos - previous;
    if (played < 0) {
        played += rna->bufsize;
    }
    remaining = rna->ndata - played;
    if (remaining < 0) {
        played = rna->wrpos - previous;
        remaining = 0;
        if (mwRnaGetTransSw(rna) == TRUE) {
            rna->wrpos = rna->rdpos;
            rna->ovrflw = TRUE;
        }
    }
    rna->ndata = remaining;
    rna->nplay += played;
    mwRnaUnlockCs(&mask);
}

static void mwlRnaAddWrPos(ADXRNA rna)
{
    Sint32 mask;
    SJCK chunk;
    SJCK consumed;
    SJCK remaining;
    Sint32 i;

    mwRnaLockCs(&mask);
    for (i = 0; i < rna->nch; i++) {
        if (rna->sj[i] == NULL) {
            /* @bug memory: reads a null joint after reporting it. the error
             * call at 0x8c132e16 returns, and 0x8c132e78 reads the joint */
            mwRnaErrCallFunc("E1213:mwlRnaAddWrPos rna->sj=NULL");
        }
    }
    for (i = 0; i < rna->nch; i++) {
        /* @hack spelling: the chunk copied a field at a time, where
         * chunk = rna->ck[i] would do */
        chunk.data = rna->ck[i].data;
        chunk.len = rna->ck[i].len;
        SJ_SplitChunk(&chunk, rna->trnsnsmpl * rna->bps / BYTE_BITS, &consumed,
                      &remaining);
        SJ_PutChunk(rna->sj[i], SJ_LIN_FREE, &consumed);
        SJ_UngetChunk(rna->sj[i], SJ_LIN_DATA, &remaining);
    }
    rna->wrpos = (rna->wrpos + rna->trnsnsmpl) % rna->bufsize;
    rna->ndata += rna->trnsnsmpl;
    rna->ntotal += rna->trnsnsmpl;
    rna->trnsnsmpl = 0;
    mwRnaUnlockCs(&mask);
}

static void mwlRnaAddFlshPos(ADXRNA rna)
{
    rna->flshpos = (rna->flshpos + rna->flshnsmpl) % rna->bufsize;
    rna->nflsh += rna->flshnsmpl;
    rna->flshnsmpl = 0;
}

static void mwlRnaWaitTransEnd(ADXRNA rna)
{
    Sint32 poll;
    Sint32 spin;

    mwg_rna_wait_cnt = 0;
    for (poll = 0; poll < WAIT_POLLS; poll++) {
        if (mwlRnaIsCopyEnd(rna) == TRUE) {
            break;
        }
        for (spin = 0; spin < WAIT_SPINS; spin++) {
        }
        mwg_rna_wait_cnt++;
    }
    /* @hack redundant: a test of poll against WAIT_POLLS whose branch is
     * empty */
    if (poll == WAIT_POLLS) {
    }
}

void mwRnaSetTransSw(ADXRNA rna, Sint32 enabled)
{
    Sint32 mask;

    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    rna->hold = TRUE;
    if (rna->copying == TRUE) {
        mwlRnaWaitTransEnd(rna);
        if (mwRnaGetTransSw(rna) == TRUE) {
            mwlRnaAddWrPos(rna);
        } else {
            mwlRnaAddFlshPos(rna);
        }
        rna->copying = FALSE;
    }
    mwRnaLockCs(&mask);
    rna->hold = FALSE;
    if (enabled == FALSE) {
        if (mwRnaGetTransSw(rna) != FALSE) {
            rna->flshpos = rna->wrpos;
            rna->nflsh = 0;
            rna->flshnsmpl = 0;
            rna->stat &= MW_RNA_SW_PLAY | MW_RNA_SW_AUTO;
        }
    } else if (enabled == TRUE) {
        if (mwRnaGetTransSw(rna) != TRUE) {
            rna->wrpos = 0;
            rna->rdpos = 0;
            rna->ndata = 0;
            rna->flush = FALSE;
            rna->trnsing = FALSE;
            rna->nplay = 0;
            rna->ntotal = 0;
            rna->trnsnsmpl = 0;
            rna->ovrflw = FALSE;
            rna->flshpos = rna->wrpos;
            rna->nflsh = 0;
            rna->stat |= MW_RNA_SW_TRANSFER;
        }
    }
    mwRnaUnlockCs(&mask);
}

void mwRnaSetPlaySw(ADXRNA rna, Sint32 enabled)
{
    Sint32 mask;

    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    mwRnaLockCs(&mask);
    if (enabled == FALSE) {
        if (mwRnaGetPlaySw(rna) != FALSE) {
            rna->snd->vtbl->stop(rna->snd);
            rna->stat &= MW_RNA_SW_TRANSFER | MW_RNA_SW_AUTO;
        }
    } else if (enabled == TRUE) {
        if (mwRnaGetPlaySw(rna) != TRUE) {
            rna->snd->vtbl->play(rna->snd);
            rna->stat |= MW_RNA_SW_PLAY;
        }
    }
    mwRnaUnlockCs(&mask);
}

/* start copying one count of samples from each channel's joint into its
 * ring at wrpos. the count is in whole blocks. it is at most the joints'
 * shortest chunk, the ring's room and the space before its end. the
 * function returns the count, or 0 once it has given the chunks back */
static Sint32 mwlRnaStartTrans(ADXRNA rna)
{
    Sint32 block;
    Sint32 room;
    Sint32 samples;
    Sint32 contiguous;
    /* @bug uninitialized: with no channels, size is stored unset at
     * 0x8c133234. the loop that sets it, at 0x8c133224, never runs */
#ifdef NONMATCHING
    /* this side repairs the count: with no channels, it is 0 */
    Sint32 size = 0;
#else
    Sint32 size;
#endif
    Sint32 i;
    Sint32 position;
    char *source;

    block = DMA_BYTES / rna->bps * BYTE_BITS;
    room = rna->bufsize - rna->ndata;
    room = room / block * block;
    for (i = 0; i < rna->nch; i++) {
        if (rna->sj[i] == NULL) {
            mwRnaErrCallFunc("E1212:mwlRnaStartTrans rna->sj=NULL");
            /* @bug memory: reads a null joint after reporting it. the error
             * call at 0x8c13310e returns, and 0x8c13312a reads the joint */
#ifdef NONMATCHING
            /* this side repairs the read: a null joint lends an empty chunk,
             * so the count is 0 and no copy starts */
            rna->ck[i].len = 0;
            continue;
#endif
        }
        SJ_GetChunk(rna->sj[i], SJ_LIN_DATA, room * rna->bps / BYTE_BITS,
                    &rna->ck[i]);
    }
    if (rna->nch == 1) {
        samples = rna->ck[0].len / rna->bps * BYTE_BITS;
    } else {
        samples = (rna->ck[0].len < rna->ck[1].len ? rna->ck[0].len
                                                   : rna->ck[1].len) /
                  rna->bps * BYTE_BITS;
    }
    samples = samples < room ? samples : room;
    /* @hack spelling: contiguous assigned inside the comparison, where a
     * statement of its own would do */
    samples = samples < (contiguous = rna->bufsize - rna->wrpos) ? samples
                                                                 : contiguous;
    samples = samples / block * block;
    if (samples <= 0) {
        for (i = 0; i < rna->nch; i++) {
            if (rna->sj[i] == NULL) {
                mwRnaErrCallFunc("E1212:mwlRnaStartTrans rna->sj=NULL");
                /* @bug memory: reads a null joint after reporting it. the
                 * error call at 0x8c1331be returns, and 0x8c1331d2 reads the
                 * joint */
#ifdef NONMATCHING
                /* this side repairs the read: a null joint lent no chunk to
                 * give back */
                continue;
#endif
            }
            SJ_UngetChunk(rna->sj[i], SJ_LIN_DATA, &rna->ck[i]);
        }
        return 0;
    }
    for (i = 0; i < rna->nch; i++) {
        source = rna->ck[i].data;
        position = rna->wrpos;
        mwg_rna_tcnt[0] = syTmrGetCount();
        size = mwlRnaStartCopy(rna, i, position, source, samples);
        mwg_rna_tcnt[1] = syTmrGetCount();
    }
    rna->trnsnsmpl = size;
    /* @hack spelling: the count returned from rna->trnsnsmpl, where size
     * would do */
    return rna->trnsnsmpl;
}

/* start copying silence into each channel's ring at flshpos. the count is
 * in whole blocks. it is at most SILENCE_BYTES of samples. it fits the room
 * data and silence leave, and the space before the ring's end. the function
 * returns the count, or 0 for none */
static Sint32 mwlRnaInsrtSlntDat(ADXRNA rna)
{
    Sint32 bits;
    Sint32 capacity;
    Sint32 room;
    Sint32 block;
    Sint32 contiguous;
    Sint32 samples;
    Sint32 position;
    /* @bug uninitialized: with no channels, size is stored unset at
     * 0x8c133316. the loop that sets it, at 0x8c13330a, never runs */
#ifdef NONMATCHING
    /* this side repairs the count: with no channels, it is 0 */
    Sint32 size = 0;
#else
    Sint32 size;
#endif
    Uint32 *silence;
    Sint32 i;

    capacity = rna->bufsize;
    room = capacity - rna->ndata - rna->nflsh;
    bits = rna->bps;
    block = DMA_BYTES / bits * BYTE_BITS;
    room = room / block * block;
    /* @hack spelling: contiguous assigned inside the comparison, where a
     * statement of its own would do */
    samples = room < (contiguous = capacity - rna->flshpos) ? room : contiguous;
    samples = samples < SILENCE_BYTES / bits * BYTE_BITS
                  ? samples
                  : SILENCE_BYTES / bits * BYTE_BITS;
    samples = samples / block * block;
    if (samples <= 0) {
        return 0;
    }
    if (rna->bps == ADPCM_BITS) {
        silence = mwg_rna_zero_adpcm;
    } else {
        silence = mwg_rna_zero;
    }
    silence = (Uint32 *)(((Uint32)silence + DMA_BYTES) & ~(DMA_BYTES - 1));
    for (i = 0; i < rna->nch; i++) {
        position = rna->flshpos;
        size = mwlRnaStartCopy(rna, i, position, silence, samples);
    }
    rna->flshnsmpl = size;
    return size;
}

static void mwlRnaExecSvrHndl(ADXRNA rna)
{
    /* while hold is set, mwRnaSetTransSw, mwRnaPause or mwRnaDiscardData
     * waits for rna's copy and finishes it */
    if (rna->hold == TRUE) {
        return;
    }
    if (mwRnaGetTransSw(rna) == TRUE && rna->copying == TRUE) {
        if (mwlRnaIsCopyEnd(rna) == TRUE) {
            mwlRnaAddWrPos(rna);
            rna->copying = FALSE;
            mwg_rna_not_end_cnt = 0;
        } else {
            mwg_rna_not_end_cnt++;
        }
    }
    if (rna->flush == TRUE && rna->copying == FALSE) {
        if (SJ_GetNumData(rna->sj[0], SJ_LIN_DATA) < DMA_BYTES) {
            mwRnaSetTransSw(rna, FALSE);
            rna->flush = FALSE;
        }
    }
    if (mwRnaGetPlaySw(rna) == TRUE) {
        mwlRnaAddRdPos(rna);
    }
    if (mwRnaGetTransSw(rna) == TRUE) {
        if (rna->copying == FALSE && mwlRnaStartTrans(rna) > 0) {
            rna->copying = TRUE;
        }
    } else if (mwRnaGetPlaySw(rna) == TRUE && rna->nflsh < rna->bufsize) {
        if (rna->copying == TRUE && mwlRnaIsCopyEnd(rna) == TRUE) {
            mwlRnaAddFlshPos(rna);
            rna->copying = FALSE;
        }
        if (rna->copying == FALSE && mwlRnaInsrtSlntDat(rna) > 0) {
            rna->copying = TRUE;
        }
    }
    if (rna->stat == (MW_RNA_SW_AUTO | MW_RNA_SW_TRANSFER) &&
        rna->ndata >= rna->stopsmpl && rna->pause == 0) {
        mwRnaSetPlaySw(rna, TRUE);
        rna->stopsmpl = NO_AUTO_PLAY;
    }
    if (rna->stat == (MW_RNA_SW_AUTO | MW_RNA_SW_PLAY) && rna->ndata == 0) {
        mwRnaSetPlaySw(rna, FALSE);
        mwlRnaClearBuf(rna);
    }
}

void mwRnaExecServer(void)
{
    Sint32 mask;
    Sint32 i;

    mwRnaLockCs(&mask);
    for (i = 0; i < RENDERERS; i++) {
        if (mwg_rna_obj[i].used == TRUE) {
            mwlRnaExecSvrHndl(&mwg_rna_obj[i]);
        }
    }
    mwRnaUnlockCs(&mask);
}

void mwRnaPause(ADXRNA rna)
{
    Sint32 mask;

    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    mwRnaLockCs(&mask);
    if (rna->pause == 0) {
        rna->swsave =
            (mwRnaGetPlaySw(rna) << MW_RNA_PLAY_SHIFT) | mwRnaGetTransSw(rna);
        mwlRnaAddRdPos(rna);
        mwRnaSetPlaySw(rna, FALSE);
        rna->pause = TRUE;
        if (rna->copying == TRUE) {
            rna->hold = TRUE;
            mwRnaUnlockCs(&mask);
            mwlRnaWaitTransEnd(rna);
            mwRnaLockCs(&mask);
            rna->hold = FALSE;
        }
        mwRnaAdjustBuf(rna);
    }
    mwRnaUnlockCs(&mask);
}

void mwRnaContinue(ADXRNA rna)
{
    Sint32 mask;

    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    mwRnaLockCs(&mask);
    if (rna->pause == TRUE) {
        if (rna->swsave != 0 && rna->swsave != MW_RNA_SW_TRANSFER) {
            mwRnaSetPlaySw(rna, TRUE);
        }
        rna->pause = FALSE;
    }
    mwRnaUnlockCs(&mask);
}

Sint32 mwRnaIsPause(ADXRNA rna)
{
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return FALSE;
    }
    return rna->pause;
}

/* rotate the rings so that they start at rdpos, rounded up to the port's
 * block. mwSbAdjstBufPause's block is MW_SB_BLOCK_SIZE bytes. rdpos becomes
 * 0, wrpos moves with the rings, and ndata loses the samples the rounding
 * passes over. flshpos moves too where swsave is MW_RNA_SW_PLAY alone. a copy
 * still running is finished without a test that it ended. rings with no data,
 * or with rdpos 0, stay as they are */
void mwRnaAdjustBuf(ADXRNA rna)
{
    Sint32 adjusted;
    Sint32 mask;
    Sint32 dropped;
    Sint32 position;

    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    mwRnaLockCs(&mask);
    if (rna->copying == TRUE) {
        if (mwRnaGetTransSw(rna) == TRUE) {
            mwlRnaAddWrPos(rna);
        } else {
            mwlRnaAddFlshPos(rna);
        }
        rna->copying = FALSE;
    }
    /* @hack spelling: an empty branch for rdpos 0, where rdpos != 0 in the
     * test would do */
    if (rna->rdpos == 0) {
    } else if (rna->ndata > 0) {
        position = rna->rdpos;
        dropped =
            rna->snd->vtbl->adjst_buf_pause(rna->snd, position, &adjusted);
        if (rna->swsave == MW_RNA_SW_PLAY) {
            rna->flshpos =
                (rna->flshpos + rna->bufsize - adjusted) % rna->bufsize;
        }
        rna->wrpos = (rna->wrpos + rna->bufsize - adjusted) % rna->bufsize;
        rna->rdpos = rna->rdpos + dropped - adjusted;
        rna->ndata -= dropped;
    }
    mwRnaUnlockCs(&mask);
}

/* drop samples, rounded down to whole blocks, from the rings and then from
 * the joints. with play on, the call reports an error and drops none. it
 * returns the samples the port drops. where the drop passes ndata, it returns
 * the samples taken from the joints plus bufsize */
Sint32 mwRnaDiscardData(ADXRNA rna, Sint32 samples)
{
    Sint32 mask;
    SJCK chunks[MAX_CHANNELS];
    SJCK consumed;
    SJCK remaining;
    Sint32 block;
    Sint32 count;
    Sint32 bytes;
    Sint32 available;
    Sint32 dropped;
    Sint32 i;
    Sint32 position;

    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return 0;
    }
    if (mwRnaGetPlaySw(rna) == TRUE) {
        mwRnaErrCallFunc("E1210:mwRnaDiscardData Play Switch is ON");
        return 0;
    }
    if (samples <= 0) {
        return 0;
    }
    rna->hold = TRUE;
    if (rna->copying == TRUE) {
        mwlRnaWaitTransEnd(rna);
        if (mwRnaGetTransSw(rna) == TRUE) {
            mwlRnaAddWrPos(rna);
        } else {
            mwlRnaAddFlshPos(rna);
        }
        rna->copying = FALSE;
    }
    mwRnaLockCs(&mask);
    rna->hold = FALSE;
    block = DMA_BYTES / rna->bps * BYTE_BITS;
    count = samples / block * block;
    if (count > rna->ndata) {
        bytes = (count - rna->ndata) * rna->bps / BYTE_BITS;
        SJ_GetChunk(rna->sj[0], SJ_LIN_DATA, bytes, &chunks[0]);
        /* @bug memory: a mono renderer's discard of more than ndata calls
         * through its null sj[1]. the joint is loaded at 0x8c13374e and read
         * at 0x8c13375a */
#ifdef NONMATCHING
        /* this side repairs the call: a mono renderer takes no chunk from
         * its null sj[1] */
        if (rna->nch > 1)
#endif
        {
            SJ_GetChunk(rna->sj[1], SJ_LIN_DATA, bytes, &chunks[1]);
        }
        if (rna->nch == 1) {
            available = chunks[0].len;
        } else {
            available =
                chunks[0].len < chunks[1].len ? chunks[0].len : chunks[1].len;
        }
        for (i = 0; i < rna->nch; i++) {
            SJ_SplitChunk(&chunks[i], available, &consumed, &remaining);
            SJ_PutChunk(rna->sj[i], SJ_LIN_FREE, &consumed);
            SJ_UngetChunk(rna->sj[i], SJ_LIN_DATA, &remaining);
        }
        /* @hack spelling: ndata and wrpos cleared in one assignment, where
         * two would do */
        rna->ndata = rna->wrpos = 0;
        /* @bug logic: counts the ring's whole size as dropped, where it
         * dropped the ndata the ring held. the add at 0x8c133806 reads
         * bufsize */
        dropped = (available / rna->bps * BYTE_BITS) + rna->bufsize;
    } else {
        position = (rna->rdpos + count) % rna->bufsize;
        dropped = rna->snd->vtbl->adjst_buf_dscrd(rna->snd, position);
        rna->ndata = rna->ndata - dropped;
        rna->wrpos = rna->ndata % rna->bufsize;
    }
    rna->rdpos = 0;
    mwRnaUnlockCs(&mask);
    return dropped;
}

void mwRnaGetTime(ADXRNA rna, Sint32 *count, Sint32 *scale)
{
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    mwlRnaAddRdPos(rna);
    *count = rna->ntotal - rna->ndata;
    *scale = mwRnaGetSfreq(rna);
}

Sint32 mwRnaGetStat(ADXRNA rna)
{
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return -1;
    }
    return rna->stat;
}

Sint32 mwRnaGetTransSw(ADXRNA rna)
{
    Sint32 result;
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return -1;
    }
    /* @hack spelling: result, a local for the switch's bit, where returning
     * the expression would do */
    result = rna->stat & MW_RNA_SW_TRANSFER;
    return result;
}

Sint32 mwRnaGetPlaySw(ADXRNA rna)
{
    Sint32 result;
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return -1;
    }
    /* @hack spelling: result, a local for the switch's bit, where returning
     * (rna->stat & MW_RNA_SW_PLAY) >> MW_RNA_PLAY_SHIFT would do */
    result = rna->stat & MW_RNA_SW_PLAY;
    result >>= MW_RNA_PLAY_SHIFT;
    return result;
}

Sint32 mwRnaGetAutoSw(ADXRNA rna)
{
    Sint32 result;
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return -1;
    }
    /* @hack spelling: result, a local for the switch's bit, where returning
     * (rna->stat & MW_RNA_SW_AUTO) >> MW_RNA_AUTO_SHIFT would do */
    result = rna->stat & MW_RNA_SW_AUTO;
    result >>= MW_RNA_AUTO_SHIFT;
    return result;
}

Sint32 mwRnaGetNumData(ADXRNA rna)
{
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return -1;
    }
    return rna->ndata;
}

Sint32 mwRnaGetNumRoom(ADXRNA rna)
{
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return -1;
    }
    return rna->bufsize - rna->ndata;
}

void mwRnaSetNumChan(ADXRNA rna, Sint32 channels)
{
    Sint32 i;

    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    if (channels > rna->maxnch) {
        mwRnaErrCallFunc("E1211:mwRnaSetNumChan Illigal parameter(NCH>MAXNCH)");
        return;
    }
    rna->nch = channels;
    rna->snd->vtbl->set_num_chan(rna->snd, (Sint8)channels);
    if (channels == 1) {
        rna->outpan[0] = 0;
        rna->outpan[1] = 0;
    } else {
        rna->outpan[0] = -MW_SB_PAN_MAX;
        rna->outpan[1] = MW_SB_PAN_MAX;
    }
    for (i = 0; i < rna->nch; i++) {
        mwRnaSetOutPan(rna, i, rna->outpan[i]);
    }
}

void mwRnaSetStartSmpl(ADXRNA rna, Sint32 samples)
{
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    rna->startsmpl = samples;
}

void mwRnaSetBitPerSmpl(ADXRNA rna, Sint32 bits)
{
    Sint32 bytes;

    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    bytes = rna->bufsize * rna->bps / BYTE_BITS;
    rna->bufsize = bytes / bits * BYTE_BITS;
    rna->snd->vtbl->set_bit_per_smpl(rna->snd, bits);
    rna->bps = bits;
}

void mwRnaSetSfreq(ADXRNA rna, Sint32 rate)
{
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    rna->snd->vtbl->set_sfreq(rna->snd, rate);
    rna->sfreq = rate;
}

void mwRnaSetOutVol(ADXRNA rna, Sint32 volume)
{
    Sint32 clamped;

    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    if (volume >= 0) {
        clamped = 0;
    } else if (volume <= MIN_VOLUME) {
        clamped = MIN_VOLUME;
    } else {
        clamped = volume;
    }
    rna->snd->vtbl->set_vol(rna->snd, clamped);
    rna->outvol = clamped;
}

void mwRnaSetOutPan(ADXRNA rna, Sint32 channel, Sint32 pan)
{
    Sint32 clamped;

    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    if (channel >= 0 && channel < rna->nch) {
        if (pan < -MW_SB_PAN_MAX) {
            clamped = -MW_SB_PAN_MAX;
        } else if (pan > MW_SB_PAN_MAX) {
            clamped = MW_SB_PAN_MAX;
        } else {
            clamped = pan;
        }
        rna->snd->vtbl->set_pan(rna->snd, channel, clamped);
        rna->outpan[channel] = clamped;
    }
}

void mwRnaSetFx(ADXRNA rna, Sint32 channel, Sint32 level)
{
    Sint32 clamped;

    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    if (channel < 0 || channel > FX_CHANNEL_MAX) {
        mwRnaErrCallFunc("E1207:mwRnaSetFx Illigal parameter (FXCH)");
        return;
    }
    if (level < MW_SB_FX_LVL_OPEN) {
        clamped = MW_SB_FX_LVL_OPEN;
    } else if (level > 0) {
        clamped = 0;
    } else {
        clamped = level;
    }
    rna->snd->vtbl->set_ex_prm(rna->snd, MW_SB_EX_PRM_FX, channel, clamped);
    rna->fxch = channel;
    rna->fxlev = clamped;
}

void mwRnaSetPitch(ADXRNA rna, Sint32 semitones, Sint32 cents)
{
    Sint32 coarse;
    Sint32 fine;

    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    rna->snd->vtbl->set_ex_prm(rna->snd, MW_SB_EX_PRM_PITCH, semitones, cents);
    rna->snd->vtbl->get_ex_prm(rna->snd, MW_SB_EX_PRM_PITCH, &coarse, &fine);
    rna->semi = coarse;
    rna->fine = fine;
}

void mwRnaSetDrctLvl(ADXRNA rna, Sint32 level)
{
    Sint32 clamped;

    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    if (level < MW_SB_FX_LVL_OPEN) {
        clamped = MW_SB_FX_LVL_OPEN;
    } else if (level > 0) {
        clamped = 0;
    } else {
        clamped = level;
    }
    rna->snd->vtbl->set_ex_prm(rna->snd, MW_SB_EX_PRM_DRCT_LVL, clamped, 0);
    rna->drctlvl = clamped;
}

Sint32 mwRnaGetBitPerSmpl(ADXRNA rna)
{
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return -1;
    }
    return rna->bps;
}

Sint32 mwRnaGetSfreq(ADXRNA rna)
{
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return -1;
    }
    return rna->sfreq;
}

Sint32 mwRnaGetOutVol(ADXRNA rna)
{
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return -1;
    }
    return rna->outvol;
}

Sint32 mwRnaGetOutPan(ADXRNA rna, Sint32 channel)
{
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return -1;
    }
    return rna->outpan[channel];
}

void mwRnaGetFx(ADXRNA rna, Sint32 *channel, Sint32 *level)
{
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        *channel = -1;
        *level = -1;
        return;
    }
    *channel = rna->fxch;
    *level = rna->fxlev;
}

void mwRnaGetPitch(ADXRNA rna, Sint32 *semitones, Sint32 *cents)
{
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        *semitones = -1;
        *cents = -1;
        return;
    }
    *semitones = rna->semi;
    *cents = rna->fine;
}

Sint32 mwRnaGetDrctLvl(ADXRNA rna)
{
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return -1;
    }
    return rna->drctlvl;
}

void mwRnaEntryErrFunc(MW_RNA_ERR_FUNC func, void *obj)
{
    mwRnaErrEntryFunc(func, obj);
}

Sint32 mwRnaIsOvrflw(ADXRNA rna)
{
    return rna->ovrflw;
}

void mwRnaClearOvrflwFlg(ADXRNA rna)
{
    rna->ovrflw = FALSE;
}

void mwRnaSetFilter(ADXRNA rna, Sint32 cutoff, Sint32 resonance)
{
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    rna->coff = cutoff;
    rna->q = resonance;
    rna->snd->vtbl->set_ex_prm(rna->snd, MW_SB_EX_PRM_FILTER, cutoff,
                               resonance);
}

void mwRnaGetFilter(ADXRNA rna, Sint32 *cutoff, Sint32 *resonance)
{
    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    *cutoff = rna->coff;
    *resonance = rna->q;
}

void mwRnaSetStmHdInfo(ADXRNA rna, char *header, Sint32 *count)
{
    Sint32 unused;
    Sint32 bits;
    Sint32 rate;
    Sint32 volume;
    Sint32 pan[MAX_CHANNELS];
    Sint32 effect_channel;
    Sint32 effect_level;
    Sint32 cutoff;
    Sint32 resonance;

    if (rna == NULL) {
        mwRnaErrCallFunc(BAD_HANDLE);
        return;
    }
    rna->snd->vtbl->set_ex_prm(rna->snd, MW_SB_EX_PRM_STM_HD, (Sint32)header,
                               0);
    rna->snd->vtbl->get_ex_prm(rna->snd, MW_SB_EX_PRM_STM_HD, count, &unused);
    if (*count > 0) {
        bits = rna->snd->vtbl->get_bit_per_smpl(rna->snd);
        rate = rna->snd->vtbl->get_sfreq(rna->snd);
        volume = rna->snd->vtbl->get_vol(rna->snd);
        pan[0] = rna->snd->vtbl->get_pan(rna->snd, 0);
        pan[1] = rna->snd->vtbl->get_pan(rna->snd, 1);
        rna->snd->vtbl->get_ex_prm(rna->snd, MW_SB_EX_PRM_FX, &effect_channel,
                                   &effect_level);
        rna->snd->vtbl->get_ex_prm(rna->snd, MW_SB_EX_PRM_FILTER, &cutoff,
                                   &resonance);
        mwRnaSetBitPerSmpl(rna, bits);
        mwRnaSetSfreq(rna, rate);
        rna->outvol = volume;
        rna->outpan[0] = pan[0];
        rna->outpan[1] = pan[1];
        rna->fxch = effect_channel;
        rna->fxlev = effect_level;
        rna->coff = cutoff;
        rna->q = resonance;
    }
}
