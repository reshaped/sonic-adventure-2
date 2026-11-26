/* the middleware's sound buffers. mwsnd is a sound buffer mwSbOpenPort
 * returned, and ch a channel's index from 0. a call reports each failure it
 * detects to mwRnaErrCallFunc, in a text that names what failed */
#include <mw_rna.h>
#include <mw_sdbuf.h>
#include <mw_sdsb.h>
#include <sd_mw.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>
#include <string.h>

/* the bits of a byte, by which the calls turn samples into bytes */
#define BYTE_BITS 8
/* a semitone, in cents */
#define SEMITONE_CENTS 100

/* the member exports these functions and data, though no header of the SDK
 * declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void mwSbInit(void *obj);
void mwSbFinish(void);
struct mw_sb *mwlSbAlloc(void);
struct mw_sb *mwSbOpenPort(Sint32 nch);
void mwSbClosePort(void *mwsnd);
void mwlSbFlushCmd(void *mwsnd, Bool playing);
void mwSbPlay(void *mwsnd);
void mwSbStop(void *mwsnd);
Sint32 mwSbGetBufSize(void *mwsnd);
void mwSbSetNumChan(void *mwsnd, Sint32 nch);
Sint32 mwSbGetPlayPos(void *mwsnd, Sint32 ch);
void mwSbSetSfreq(void *mwsnd, Sint32 sfreq);
Sint32 mwSbGetSfreq(void *mwsnd);
void mwSbSetBitPerSmpl(void *mwsnd, Sint32 bits);
Sint32 mwSbGetBitPerSmpl(void *mwsnd);
void mwSbSetVol(void *mwsnd, Sint32 vol);
Sint32 mwSbGetVol(void *mwsnd);
void mwSbSetPan(void *mwsnd, Sint32 ch, Sint32 pan);
Sint32 mwSbGetPan(void *mwsnd, Sint32 ch);
void mwlSbSetFx(void *mwsnd, Sint32 fx_ch, Sint32 lvl);
void mwlSbGetFx(void *mwsnd, Sint32 *fx_ch, Sint32 *lvl);
void mwlSbSetPitch(void *mwsnd, Sint32 notes, Sint32 cents);
void mwlSbGetPitch(void *mwsnd, Sint32 *notes, Sint32 *cents);
void mwlSbSetDrctLvl(void *mwsnd, Sint32 lvl);
Sint32 mwlSbGetDrctLvl(void *mwsnd);
void mwSbSetExPrm(void *mwsnd, Sint32 kind, Sint32 a, Sint32 b);
void mwSbGetExPrm(void *mwsnd, Sint32 kind, Sint32 *a, Sint32 *b);
void mwlSbTrnsDat(struct mw_sb *sb, Sint32 ch, Sint32 ofs, void *src,
                  Sint32 nsmpl);
void mwlSbTrnsEx(struct mw_sb *sb, Sint32 ch, Sint32 ofs, void *src);
void mwSbStopTrns(void *mwsnd);
void mwSbTrns(void *mwsnd, Sint32 ch, Sint32 ofs, void *src, Sint32 nsmpl);
Sint32 mwSbGetTrnsStat(void *mwsnd, Sint32 ch);
void mwlSbCopySndToWrk(struct mw_sb *sb, Sint32 ch, void *dst, Sint32 ofs,
                       Sint32 nsmpl);
void mwlSbAlign256(struct mw_sb *sb, Sint32 ch, Sint32 nsmpl);
void mwlSbCalcNumTrans(Sint32 nsmpl, Sint32 nblock, Sint32 *num);
void mwlSbReplaceDat(struct mw_sb *sb, Sint32 ch, Sint32 shift, Sint32 num);
Sint32 mwSbAdjstBufPause(void *mwsnd, Sint32 nsmpl, Sint32 *adjusted);
Sint32 mwSbAdjstBufDscrd(void *mwsnd, Sint32 nsmpl);

const char *volatile mwg_sb_build =
    "\nmwSb Ver.2.00 Build:Nov 02 2000 20:44:34\n";
struct mw_sblib_vtbl mwg_sblib_vtbl = {
    {NULL, NULL, NULL},
    mwSbInit,
    mwSbFinish,
    mwSbOpenPort,
};
struct mw_sb_vtbl mwg_sb_vtbl = {
    {NULL, NULL, NULL}, mwSbClosePort,     mwSbPlay,          mwSbStop,
    mwSbGetBufSize,     mwSbSetNumChan,    mwSbGetPlayPos,    mwSbSetSfreq,
    mwSbGetSfreq,       mwSbSetBitPerSmpl, mwSbGetBitPerSmpl, mwSbSetVol,
    mwSbGetVol,         mwSbSetPan,        mwSbGetPan,        mwSbSetExPrm,
    mwSbGetExPrm,       mwSbTrns,          mwSbStopTrns,      mwSbGetTrnsStat,
    mwSbAdjstBufPause,  mwSbAdjstBufDscrd,
};
struct mw_sb mwg_sb_obj[MW_SB_NUM];
/* scratch for the two blocks of samples that a call rotating a ring holds
 * at once */
Uint8 mwg_sb_pbuf[2 * MW_SB_BLOCK_SIZE];
/* three words mwlSbTrnsDat sets to 0 in turn: before sdMemBlkSetPrm, before
 * sdMemBlkTransfer and after it. nothing reads them */
Uint32 mwg_sb_tcnt[3];
/* NOLINTEND(misc-use-internal-linkage) */

/* mark every sound buffer and every ring free */
void mwSbInit(void *obj)
{
    /* @hack redundant: the read of mwg_sb_build, which nothing uses, and
     * the volatile on mwg_sb_build that keeps it */
    (void)mwg_sb_build;
    memset(mwg_sb_obj, 0, sizeof(mwg_sb_obj));
    mwSndBufInit();
}

/* mark every sound buffer free. the rings are left as they are */
void mwSbFinish(void)
{
    memset(mwg_sb_obj, 0, sizeof(mwg_sb_obj));
}

/* the first sound buffer that isn't open, or null. the call doesn't mark
 * the sound buffer open */
struct mw_sb *mwlSbAlloc(void)
{
    Sint32 i;
    /* @hack spelling: found, a result the loop breaks out with, where the
     * sound buffer returned from the loop would do */
    struct mw_sb *found = NULL;

    for (i = 0; i < MW_SB_NUM; i++) {
        if (!mwg_sb_obj[i].used) {
            found = &mwg_sb_obj[i];
            break;
        }
    }
    return found;
}

/* open a sound buffer of nch channels, from 1 to MW_SB_CHAN_MAX, and return
 * it. each channel takes a ring and two of the sound library's memory
 * blocks. mwSbSetBitPerSmpl must set the sound buffer's bits before any call
 * that counts samples.
 *
 * the call returns null where nch is out of range or no sound buffer is
 * free. where taking a ring or a memory block fails, it returns null. it
 * gives back the rings and memory blocks it took before it does. where the
 * stream port won't open, it returns null and keeps the rings and the ex_blk
 * memory blocks. a failure of sdSndMemAllocateBank, sdPstmResetPrmMw or
 * sdPstmSetBasePrmMw still returns the sound buffer */
struct mw_sb *mwSbOpenPort(Sint32 nch)
{
    struct mw_sb *sb;
    Sint32 i;
    Sint32 j;
    Sint32 bank[MW_SB_CHAN_MAX];
    Sint32 adr;
    SDMEMBLK blk;
    SDPSTM port;
    SDE_ERR result;

    if (nch < 1 || nch > MW_SB_CHAN_MAX) {
        mwRnaErrCallFunc("E1221:Illigal parameter(NCH) in mwSndOpenPort");
        return NULL;
    }
    sb = mwlSbAlloc();
    if (sb == NULL) {
        mwRnaErrCallFunc("E1222:Not enough instance(MWSND) in mwSndOpenPort");
        return NULL;
    }
    sb->vtbl = &mwg_sb_vtbl;
    for (i = 0; i < nch; i++) {
        sb->buf[i] = mwSndBufCreate();
        if (sb->buf[i] == NULL) {
            mwRnaErrCallFunc("E1223:Cannot create MWSBUF in mwSndOpenPort");
            for (j = 0; j < i; j++) {
                mwSndBufDestroy(sb->buf[j]);
            }
            return NULL;
        }
        bank[i] = mwSndBufGetNo(sb->buf[i]);
    }
    for (i = 0; i < nch; i++) {
        adr = SD_SND_MEM_SIZE - ((SD_BANK_MAX - bank[i]) * MW_SB_RING_PITCH);
        result = sdSndMemAllocateBank(SDE_DATA_TYPE_PSTM_RING_BUF, bank[i], adr,
                                      MW_SB_RING_SIZE);
        if (result != SDE_ERR_NOTHING) {
            mwRnaErrCallFunc(
                "E1224:sdSndMemAllocateBank return error in mwSndOpenPort");
        }
        sb->ring_adr[i] = adr;
    }
    sb->ring_size = MW_SB_RING_SIZE;
    for (i = 0; i < nch; i++) {
        result = sdMemBlkCreate(&blk);
        if (result != SDE_ERR_NOTHING) {
            mwRnaErrCallFunc(
                "E1225:sdMemBlkCreate return error in mwSndOpenPort");
            for (j = 0; j < i; j++) {
                result = sdMemBlkDestroy(sb->trns_blk[j]);
                if (result != SDE_ERR_NOTHING) {
                    mwRnaErrCallFunc(
                        "E1229:sdMemBlkDestroy return error in mwSndOpenPort");
                }
            }
            for (j = 0; j < nch; j++) {
                mwSndBufDestroy(sb->buf[j]);
            }
            return NULL;
        }
        sb->trns_blk[i] = blk;
    }
    for (i = 0; i < nch; i++) {
        result = sdMemBlkCreate(&blk);
        if (result != SDE_ERR_NOTHING) {
            mwRnaErrCallFunc(
                "E1225:sdMemBlkCreate return error in mwSndOpenPort");
            /* @bug logic: reports every sdMemBlkDestroy in this loop and the
             * trns_blk loop after it as failed. neither loop stores what
             * sdMemBlkDestroy returns, so each test reads sdMemBlkCreate's
             * failure (TST at 0x8c16cb8c and 0x8c16cbbc) */
            for (j = 0; j < i; j++) {
                sdMemBlkDestroy(sb->ex_blk[j]);
                if (result != SDE_ERR_NOTHING) {
                    mwRnaErrCallFunc(
                        "E1229:sdMemBlkDestroy return error in mwSndOpenPort");
                }
            }
            for (j = 0; j < nch; j++) {
                sdMemBlkDestroy(sb->trns_blk[j]);
                if (result != SDE_ERR_NOTHING) {
                    mwRnaErrCallFunc(
                        "E1229:sdMemBlkDestroy return error in mwSndOpenPort");
                }
            }
            for (j = 0; j < nch; j++) {
                mwSndBufDestroy(sb->buf[j]);
            }
            return NULL;
        }
        sb->ex_blk[i] = blk;
    }
    /* @hack type: the casts of bank[0] and bank[1] to Sint8, where the
     * Sint32 values would do */
    if (nch == 1) {
        result = sdPstmOpenPortMw(&port, nch, (Sint8)bank[0]);
    } else {
        result = sdPstmOpenPortMw(&port, nch, (Sint8)bank[0], (Sint8)bank[1]);
    }
    if (result != SDE_ERR_NOTHING) {
        mwRnaErrCallFunc("E1226:sdPstmOpenPort return error in mwSndOpenPort");
        /* @bug logic: reports every sdMemBlkDestroy in this loop as failed.
         * the loop doesn't store what sdMemBlkDestroy returns, so each test
         * reads sdPstmOpenPortMw's failure (TST at 0x8c16cc6a) */
        for (j = 0; j < nch; j++) {
            sdMemBlkDestroy(sb->trns_blk[j]);
            if (result != SDE_ERR_NOTHING) {
                mwRnaErrCallFunc(
                    "E1229:sdMemBlkDestroy return error in mwSndOpenPort");
            }
            /* @bug memory: gives back sb->buf[i] in place of each ring this
             * call took (JSR at 0x8c16cc7c). the ex_blk loop above left i at
             * nch, so the rings stay taken. mwSndBufDestroy writes a 0
             * through sb->buf[nch], which lies past buf's end where nch is
             * 2. where nch is 1, sb->buf[1] holds no ring this call took */
#ifdef NONMATCHING
            /* this side repairs the give-back: it gives back sb->buf[j],
             * each ring this call took, once */
            mwSndBufDestroy(sb->buf[j]);
#else
            mwSndBufDestroy(sb->buf[i]);
#endif
        }
        return NULL;
    }
    sb->port = port;
    sb->num_chan = nch;
    sb->max_chan = nch;
    for (i = 0; i < sb->max_chan; i++) {
        result = sdPstmResetPrmMw(sb->port, i);
        if (result != SDE_ERR_NOTHING) {
            mwRnaErrCallFunc(
                "E1227:sdPstmResetPrm return error in mwSndOpenPort");
        }
    }
    for (i = 0; i < sb->max_chan; i++) {
        result = sdPstmSetBasePrmMw(sb->port, i, MW_SB_SD_LVL_MAX, 0, 0, 0,
                                    MW_SB_SD_LVL_MAX);
        if (result != SDE_ERR_NOTHING) {
            mwRnaErrCallFunc(
                "E1228:sdPstmSetBasePrmMw return error in mwSndOpenPort");
        }
    }
    sb->to_bound_nsmpl = 0;
    sb->vol = 0;
    sb->fx_ch = 0;
    sb->fx_lvl = MW_SB_FX_LVL_OPEN;
    sb->pitch_notes = 0;
    sb->pitch_cents = 0;
    sb->drct_lvl = 0;
    sb->used = TRUE;
    return sb;
}

/* give back the sound buffer's rings, memory blocks and stream port, and
 * mark it free. a null mwsnd does nothing */
void mwSbClosePort(void *mwsnd)
{
    struct mw_sb *sb = mwsnd;
    Sint32 i;
    SDE_ERR result;

    if (sb != NULL) {
        for (i = 0; i < sb->max_chan; i++) {
            mwSndBufDestroy(sb->buf[i]);
            result = sdMemBlkDestroy(sb->trns_blk[i]);
            if (result != SDE_ERR_NOTHING) {
                mwRnaErrCallFunc(
                    "E1230:sdMemBlkDestroy return error in mwSndClosePort");
            }
        }
        for (i = 0; i < sb->max_chan; i++) {
            result = sdMemBlkDestroy(sb->ex_blk[i]);
            if (result != SDE_ERR_NOTHING) {
                mwRnaErrCallFunc(
                    "E1230:sdMemBlkDestroy return error in mwSndClosePort");
            }
        }
        for (i = 0; i < sb->max_chan; i++) {
            result = sdPstmResetPrmMw(sb->port, i);
            if (result != SDE_ERR_NOTHING) {
                mwRnaErrCallFunc(
                    "E1231:sdPstmResetPrm return error in mwSndClosePort");
            }
        }
        result = sdPstmClosePortMw(sb->port);
        if (result != SDE_ERR_NOTHING) {
            mwRnaErrCallFunc(
                "E1232:sdPstmClosePort return error in mwSndClosePort");
        }
        /* @hack redundant: stores of 0 before the memset and after it,
         * where the memset alone would do */
        sb->max_chan = 0;
        sb->num_chan = 0;
        memset(sb, 0, sizeof(*sb));
        sb->used = FALSE;
    }
}

/* send the sound driver the commands queued for it. the call uses neither
 * mwsnd nor playing */
void mwlSbFlushCmd(void *mwsnd, Bool playing)
{
    SDE_ERR result = sdSysFlushHostCmd();

    if (result != SDE_ERR_NOTHING && result != SDE_ERR_DRV_BUSY) {
        mwRnaErrCallFunc(
            "E1241:sdSysFlushHostCmd return error mwSndPlay or mwSndStop");
    }
}

/* start the stream port playing at the sound buffer's bits and rate. the
 * command goes at once. the bits must be 16, 8 or 4 */
void mwSbPlay(void *mwsnd)
{
    struct mw_sb *sb = mwsnd;
    Sint32 bits = sb->bit_per_smpl;
    /* @hack spelling: sfreq, a local read ahead of the tests, where
     * sb->sfreq in sdPstmPlayMw's call would do */
    Sint32 sfreq = sb->sfreq;
    SDE_PCM_TYPE type;
    SDE_ERR result;

#ifdef NONMATCHING
    /* this side repairs the unset type: it plays any other bits as 16. the
     * console passes R5 as its caller left it, which C can't give */
    type = SDE_PCM_TYPE_16BIT_LINEAR;
#endif
    /* @bug uninitialized: a bit_per_smpl that isn't 16, 8 or 4 leaves type
     * unset (BF at 0x8c16ce70) */
    /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
    if (bits == 16) {
        type = SDE_PCM_TYPE_16BIT_LINEAR;
    } else if (bits == 8) {
        type = SDE_PCM_TYPE_8BIT_LINEAR;
    } else if (bits == 4) {
        type = SDE_PCM_TYPE_4BIT_ADPCM;
    }
    /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
    /* @hack redundant: bits stored back where it was read, unchanged */
    sb->bit_per_smpl = bits;
    result = sdPstmPlayMw(sb->port, type, sfreq, 0);
    if (result != SDE_ERR_NOTHING) {
        mwRnaErrCallFunc("E1233:sdPstmPlay return error in mwSndPlay");
    }
    mwlSbFlushCmd(sb, TRUE);
}

/* stop the stream port, and send the command at once */
void mwSbStop(void *mwsnd)
{
    struct mw_sb *sb = mwsnd;
    SDE_ERR result = sdPstmStopMw(sb->port);

    if (result != SDE_ERR_NOTHING) {
        mwRnaErrCallFunc("E1234:sdPstmStop return error in mwSndStop");
    }
    mwlSbFlushCmd(sb, FALSE);
}

/* how many samples a channel's ring holds */
Sint32 mwSbGetBufSize(void *mwsnd)
{
    struct mw_sb *sb = mwsnd;

    return sb->ring_size / sb->bit_per_smpl * BYTE_BITS;
}

/* set how many channels play, from 1 to the count the sound buffer was
 * opened with. sdPstmSetUseChMw resets every channel's levels, so the call
 * sets them again from the values last set. the pans instead go to 0 for
 * one channel, and to -MW_SB_PAN_MAX and MW_SB_PAN_MAX for two */
void mwSbSetNumChan(void *mwsnd, Sint32 nch)
{
    struct mw_sb *sb = mwsnd;
    Sint32 i;
    Sint32 a;
    Sint32 b;
    SDE_ERR result;

    sb->num_chan = nch;
    result = sdPstmSetUseChMw(sb->port, nch);
    if (result != SDE_ERR_NOTHING) {
        mwRnaErrCallFunc(
            "E1240:sdPstmSetUseCh return error in mwSndSetNumChan");
    }
    mwSbSetVol(mwsnd, mwSbGetVol(mwsnd));
    if (nch == 1) {
        sb->pan[0] = 0;
        sb->pan[1] = 0;
    } else {
        sb->pan[0] = -MW_SB_PAN_MAX;
        sb->pan[1] = MW_SB_PAN_MAX;
    }
    for (i = 0; i < sb->num_chan; i++) {
        mwSbSetPan(mwsnd, i, sb->pan[i]);
    }
    mwSbGetExPrm(mwsnd, MW_SB_EX_PRM_FX, &a, &b);
    mwSbSetExPrm(mwsnd, MW_SB_EX_PRM_FX, a, b);
    mwSbGetExPrm(mwsnd, MW_SB_EX_PRM_PITCH, &a, &b);
    mwSbSetExPrm(mwsnd, MW_SB_EX_PRM_PITCH, a, b);
    mwSbGetExPrm(mwsnd, MW_SB_EX_PRM_DRCT_LVL, &a, &b);
    mwSbSetExPrm(mwsnd, MW_SB_EX_PRM_DRCT_LVL, a, b);
}

/* where in its ring channel ch plays, in samples. it returns 0 while
 * channel 0 isn't playing, whatever ch is */
Sint32 mwSbGetPlayPos(void *mwsnd, Sint32 ch)
{
    struct mw_sb *sb = mwsnd;
    Uint32 flg = SDD_PORT_FLG_PLAY;
    Sint32 adr;
    Sint32 nsmpl;
    SDE_ERR result;

    sdPstmGetFlg(sb->port, 0, &flg);
    if (flg & SDD_PORT_FLG_PLAY) {
        /* @bug uninitialized: divides an unset adr where sdPstmGetCurAdrMw
         * fails (MOV.L @(4,R15),R1 at 0x8c16cfd4). each failure of
         * sdPstmGetCurAdr, which sdPstmGetCurAdrMw calls, skips the store
         * to adr (BRA at 0x8c1783e4, 0x8c1783ee, 0x8c1783fa and 0x8c17842a) */
#ifdef NONMATCHING
        /* this side repairs the unset adr: it starts at 0, so a failed read
         * gives position 0, as a port that isn't playing does */
        adr = 0;
#endif
        result = sdPstmGetCurAdrMw(sb->port, ch, &adr);
        if (result != SDE_ERR_NOTHING) {
            mwRnaErrCallFunc(
                "E1236:sdPstmGetCurAdr return error in mwSndGetPlayPos");
        }
        nsmpl = adr / sb->bit_per_smpl * BYTE_BITS;
    } else {
        nsmpl = 0;
    }
    return nsmpl;
}

/* set the sample rate that mwSbPlay gives the stream port, in hertz */
void mwSbSetSfreq(void *mwsnd, Sint32 sfreq)
{
    struct mw_sb *sb = mwsnd;

    sb->sfreq = sfreq;
}

Sint32 mwSbGetSfreq(void *mwsnd)
{
    struct mw_sb *sb = mwsnd;

    return sb->sfreq;
}

void mwSbSetBitPerSmpl(void *mwsnd, Sint32 bits)
{
    struct mw_sb *sb = mwsnd;

    sb->bit_per_smpl = bits;
}

Sint32 mwSbGetBitPerSmpl(void *mwsnd)
{
    struct mw_sb *sb = mwsnd;

    return sb->bit_per_smpl;
}

/* set every channel's volume. where the sound library refuses a channel's
 * volume, the call stops at that channel. the volume mwSbGetVol gives then
 * stays as it was */
void mwSbSetVol(void *mwsnd, Sint32 vol)
{
    struct mw_sb *sb = mwsnd;
    Sint32 lvl;
    Sint32 i;
    SDE_ERR result;

    if (vol <= MW_SB_VOL_MIN) {
        lvl = -MW_SB_SD_LVL_MAX;
    } else if (vol >= 0) {
        lvl = MW_SB_SD_LVL_MAX;
    } else {
        Sint32 scaled = (128 - (vol * 255)) / MW_SB_VOL_MIN;

        scaled = scaled * 255 / 168;
        scaled = scaled < MW_SB_SD_LVL_MAX ? scaled : MW_SB_SD_LVL_MAX;
        lvl = scaled > -MW_SB_SD_LVL_MAX ? scaled : -MW_SB_SD_LVL_MAX;
    }
    for (i = 0; i < sb->num_chan; i++) {
        result = sdPstmSetVolMw(sb->port, i, lvl, 0);
        if (result != SDE_ERR_NOTHING) {
            mwRnaErrCallFunc("E1237:sdPstmSetVol return error in mwSndSetVol");
            return;
        }
    }
    sb->vol = vol;
}

Sint32 mwSbGetVol(void *mwsnd)
{
    struct mw_sb *sb = mwsnd;

    return sb->vol;
}

void mwSbSetPan(void *mwsnd, Sint32 ch, Sint32 pan)
{
    struct mw_sb *sb = mwsnd;
    /* @hack spelling: sd_pan, a Sint8 local, where the quotient in
     * sdPstmSetPanMw's call would do */
    Sint8 sd_pan = pan * MW_SB_SD_LVL_MAX / MW_SB_PAN_MAX;
    SDE_ERR result = sdPstmSetPanMw(sb->port, ch, sd_pan, 0);

    if (result != SDE_ERR_NOTHING) {
        mwRnaErrCallFunc("E1238:sdPstmSetPan return error in mwSndSetPan");
    }
    sb->pan[ch] = pan;
}

Sint32 mwSbGetPan(void *mwsnd, Sint32 ch)
{
    struct mw_sb *sb = mwsnd;

    return sb->pan[ch];
}

/* make every channel feed effect channel fx_ch at level lvl */
void mwlSbSetFx(void *mwsnd, Sint32 fx_ch, Sint32 lvl)
{
    struct mw_sb *sb = mwsnd;
    Sint32 sd_lvl;
    Sint32 i;
    SDE_ERR result;

    if (lvl < MW_SB_LVL_MIN) {
        sd_lvl = -MW_SB_SD_LVL_MAX;
    } else if (lvl >= 0) {
        sd_lvl = MW_SB_SD_LVL_MAX;
    } else {
        sd_lvl = (lvl * 256 / 45) + MW_SB_SD_LVL_MAX;
    }
    for (i = 0; i < sb->num_chan; i++) {
        result = sdPstmSetFxChMw(sb->port, i, fx_ch, sd_lvl);
        if (result != SDE_ERR_NOTHING) {
            mwRnaErrCallFunc(
                "E1245:sdPstmSetFxCh return error in mwSndSetExPrm");
        }
    }
    sb->fx_ch = fx_ch;
    sb->fx_lvl = lvl;
}

void mwlSbGetFx(void *mwsnd, Sint32 *fx_ch, Sint32 *lvl)
{
    struct mw_sb *sb = mwsnd;

    *fx_ch = sb->fx_ch;
    *lvl = sb->fx_lvl;
}

/* set every channel's pitch shift to notes semitones plus cents hundredths
 * of a semitone. notes is clamped to MW_SB_PITCH_NOTES_MAX either way, and
 * cents to MW_SB_PITCH_CENTS_MAX */
void mwlSbSetPitch(void *mwsnd, Sint32 notes, Sint32 cents)
{
    struct mw_sb *sb = mwsnd;
    Sint32 pitch;
    Sint32 coarse;
    Sint32 fine;
    Sint32 i;
    SDE_ERR result;

    notes = notes < MW_SB_PITCH_NOTES_MAX ? notes : MW_SB_PITCH_NOTES_MAX;
    notes = notes > -MW_SB_PITCH_NOTES_MAX ? notes : -MW_SB_PITCH_NOTES_MAX;
    coarse = notes * MW_SB_SD_SEMITONE;
    cents = cents < MW_SB_PITCH_CENTS_MAX ? cents : MW_SB_PITCH_CENTS_MAX;
    cents = cents > -MW_SB_PITCH_CENTS_MAX ? cents : -MW_SB_PITCH_CENTS_MAX;
    fine = cents * MW_SB_SD_SEMITONE / SEMITONE_CENTS;
    pitch = coarse + fine;
    for (i = 0; i < sb->num_chan; i++) {
        result = sdPstmSetPitchMw(sb->port, i, pitch, 0);
        if (result != SDE_ERR_NOTHING) {
            mwRnaErrCallFunc(
                "E1239:sdPstmSetPitch return error in mwSndSetExPrm");
        }
    }
    sb->pitch_notes = notes;
    sb->pitch_cents = cents;
}

/* store in *notes and *cents what mwlSbSetPitch last set, once clamped */
void mwlSbGetPitch(void *mwsnd, Sint32 *notes, Sint32 *cents)
{
    struct mw_sb *sb = mwsnd;

    *notes = sb->pitch_notes;
    *cents = sb->pitch_cents;
}

/* set every channel's direct level to lvl */
void mwlSbSetDrctLvl(void *mwsnd, Sint32 lvl)
{
    struct mw_sb *sb = mwsnd;
    Sint32 sd_lvl;
    Sint32 i;
    SDE_ERR result;

    if (lvl < MW_SB_LVL_MIN) {
        sd_lvl = -MW_SB_SD_LVL_MAX;
    } else if (lvl >= 0) {
        sd_lvl = MW_SB_SD_LVL_MAX;
    } else {
        sd_lvl = lvl * 3;
    }
    for (i = 0; i < sb->num_chan; i++) {
        result = sdPstmSetDrctLevMw(sb->port, i, sd_lvl);
        if (result != SDE_ERR_NOTHING) {
            mwRnaErrCallFunc(
                "E1247:sdPstmSetDrctLev return error in mwSndSetExPrm");
        }
    }
    sb->drct_lvl = lvl;
}

Sint32 mwlSbGetDrctLvl(void *mwsnd)
{
    struct mw_sb *sb = mwsnd;

    return sb->drct_lvl;
}

/* set the values of kind from a and b. MW_SB_EX_PRM_FILTER,
 * MW_SB_EX_PRM_STM_HD and any other kind set nothing */
void mwSbSetExPrm(void *mwsnd, Sint32 kind, Sint32 a, Sint32 b)
{
    if (kind == MW_SB_EX_PRM_FX) {
        mwlSbSetFx(mwsnd, a, b);
    } else if (kind == MW_SB_EX_PRM_PITCH) {
        mwlSbSetPitch(mwsnd, a, b);
    } else if (kind == MW_SB_EX_PRM_DRCT_LVL) {
        mwlSbSetDrctLvl(mwsnd, a);
    }
}

/* store the values of kind in *a and *b. the direct level's *b is 0.
 * MW_SB_EX_PRM_FILTER, MW_SB_EX_PRM_STM_HD and any other kind set both to 0 */
void mwSbGetExPrm(void *mwsnd, Sint32 kind, Sint32 *a, Sint32 *b)
{
    if (kind == MW_SB_EX_PRM_FX) {
        mwlSbGetFx(mwsnd, a, b);
    } else if (kind == MW_SB_EX_PRM_PITCH) {
        mwlSbGetPitch(mwsnd, a, b);
    } else if (kind == MW_SB_EX_PRM_DRCT_LVL) {
        *a = mwlSbGetDrctLvl(mwsnd);
        *b = 0;
    } else {
        *a = 0;
        *b = 0;
    }
}

/* start the transfer of nsmpl samples at src into channel ch's ring, ofs
 * samples from its start */
void mwlSbTrnsDat(struct mw_sb *sb, Sint32 ch, Sint32 ofs, void *src,
                  Sint32 nsmpl)
{
    SDE_ERR result;
    Sint32 size;
    Sint32 adr;

    size = nsmpl * sb->bit_per_smpl / BYTE_BITS;
    mwg_sb_tcnt[0] = 0;
    result = sdMemBlkSetPrm(sb->trns_blk[ch], src, size, NULL, NULL);
    if (result != SDE_ERR_NOTHING) {
        mwRnaErrCallFunc("E1242:sdMemBlkSetPrm return error in mwSndTrns");
        return;
    }
    adr = sb->ring_adr[ch] + (ofs * sb->bit_per_smpl / BYTE_BITS);
    mwg_sb_tcnt[1] = 0;
    result = sdMemBlkTransfer(sb->trns_blk[ch], adr);
    if (result != SDE_ERR_NOTHING) {
        mwRnaErrCallFunc("E1243:sdMemBlkTransfer return error in mwSndTrns");
    }
    mwg_sb_tcnt[2] = 0;
}

/* start the transfer of the MW_SB_EX_SIZE bytes at src into channel ch's
 * ring, ofs samples from its start */
void mwlSbTrnsEx(struct mw_sb *sb, Sint32 ch, Sint32 ofs, void *src)
{
    SDE_ERR result =
        sdMemBlkSetPrm(sb->ex_blk[ch], src, MW_SB_EX_SIZE, NULL, NULL);

    if (result != SDE_ERR_NOTHING) {
        mwRnaErrCallFunc("E1242:sdMemBlkSetPrm return error in mwSndTrns");
        return;
    }
    result = sdMemBlkTransfer(sb->ex_blk[ch],
                              sb->ring_adr[ch] +
                                  (ofs * sb->bit_per_smpl / BYTE_BITS));
    if (result != SDE_ERR_NOTHING) {
        mwRnaErrCallFunc("E1243:sdMemBlkTransfer return error in mwSndTrns");
    }
}

void mwSbStopTrns(void *mwsnd)
{
}

/* start the transfer as mwlSbTrnsDat does. a transfer to the ring's start
 * also repeats src's first MW_SB_EX_SIZE bytes past the ring's end */
void mwSbTrns(void *mwsnd, Sint32 ch, Sint32 ofs, void *src, Sint32 nsmpl)
{
    struct mw_sb *sb = mwsnd;

    mwlSbTrnsDat(sb, ch, ofs, src, nsmpl);
    if (ofs == 0) {
        /* @hack redundant: end, a local that adds ofs to the ring's count
         * of samples, where the count alone would do. this branch runs only
         * where ofs is 0 */
        Sint32 end = ofs + (sb->ring_size / sb->bit_per_smpl * BYTE_BITS);

        mwlSbTrnsEx(sb, ch, end, src);
    }
}

/* run sdMemBlkServer once, with every interrupt masked. then return where
 * the transfer of channel ch's samples stands, as an SDE_MEMBLK_STAT */
Sint32 mwSbGetTrnsStat(void *mwsnd, Sint32 ch)
{
    struct mw_sb *sb = mwsnd;
    SDE_MEMBLK_STAT stat;
    Sint32 mask;
    SDE_ERR result;

    mwRnaLockCs(&mask);
    sdMemBlkServer();
    mwRnaUnlockCs(&mask);
    /* @bug uninitialized: returns an unset stat where sdMemBlkGetStat fails
     * on the handle (MOV.L @R15,R0 at 0x8c16d55a). that failure skips
     * sdMemBlkGetStat's store to stat (BF at 0x8c153c98) */
#ifdef NONMATCHING
    /* this side repairs the unset stat: it starts as finished, so a block
     * the library refuses ends its renderer's copy */
    stat = SDE_MEMBLK_STAT_TRANSFER_FINISHED;
#endif
    result = sdMemBlkGetStat(sb->trns_blk[ch], &stat);
    if (result != SDE_ERR_NOTHING) {
        mwRnaErrCallFunc(
            "E1244:sdMemBlkGetStat return error in mwSndGetTrnsStat");
    }
    return stat;
}

/* copy nsmpl samples to dst from channel ch's ring, ofs samples from its
 * start */
void mwlSbCopySndToWrk(struct mw_sb *sb, Sint32 ch, void *dst, Sint32 ofs,
                       Sint32 nsmpl)
{
    Sint32 bits;
    /* @hack spelling: bits assigned inside adr's initializer, where an
     * initializer of its own would do */
    Sint32 adr =
        (ofs * (bits = sb->bit_per_smpl) / BYTE_BITS) + sb->ring_adr[ch];
    Sint32 size = nsmpl * bits / BYTE_BITS;
    SDE_ERR result = sdMoveSndMemToWrkMem(adr, dst, size);

    if (result != SDE_ERR_NOTHING) {
        mwRnaErrCallFunc("E1227:sdMoveSndMemToWrkMem return error");
    }
}

/* rotate channel ch's ring so that the sample at nsmpl moves back to the
 * start of its block. the samples rotated past the ring's start go to its
 * end. where nsmpl starts a block, the call moves nothing */
void mwlSbAlign256(struct mw_sb *sb, Sint32 ch, Sint32 nsmpl)
{
    Sint32 block_nsmpl = MW_SB_BLOCK_SIZE / sb->bit_per_smpl * BYTE_BITS;
    Sint32 rest;
    Sint32 read;
    Sint32 write;
    Sint32 i;
    Sint32 num;
    Uint8 *buf[2];

    rest = nsmpl % block_nsmpl;
    if (rest > 0) {
        Uint8 *base;

        /* @hack spelling: base, a copy of mwg_sb_pbuf that buf[0] and buf[1]
         * are set from, where mwg_sb_pbuf itself would do */
        buf[0] = base = mwg_sb_pbuf;
        /* @hack spelling: read and write zeroed in one chained assignment,
         * where two statements would do */
        read = write = 0;
        mwlSbCopySndToWrk(sb, ch, buf[0], read, rest);
        read += rest;
        /* @bug memory: puts buf[1] block_nsmpl bytes past buf[0], though
         * block_nsmpl counts samples (ADD R10,R2 at 0x8c16d644). with 16-bit
         * samples, buf[1] overlaps buf[0] past its first 64 samples. the
         * copies into buf[1] overwrite those samples before the last
         * mwlSbTrnsDat sends buf[0]. with 4-bit samples, buf[1] starts at
         * mwg_sb_pbuf's end. each copy into it writes up to 256 bytes past
         * that end, and mwg_sb_tcnt lies in the first 12. nothing in the game
         * calls the function: no word in any image holds its address, and no
         * call in this source names it */
        buf[1] = base + block_nsmpl;
        for (i = 0; i < MW_SB_BLOCK_NUM; i++) {
            if (i == MW_SB_BLOCK_NUM - 1) {
                num = block_nsmpl - rest;
            } else {
                num = block_nsmpl;
            }
            mwlSbCopySndToWrk(sb, ch, buf[1], read, num);
            mwlSbTrnsDat(sb, ch, write, buf[1], num);
            read += num;
            write += num;
        }
        mwlSbTrnsDat(sb, ch, write, buf[0], rest);
    }
}

/* store in *num how many cycles mwlSbReplaceDat follows to rotate a ring by
 * nblock blocks. for an nblock from 1 to MW_SB_BLOCK_NUM, *num is the
 * largest power of two that divides nblock. an nblock of 0 gives 0. the
 * call ignores nsmpl */
void mwlSbCalcNumTrans(Sint32 nsmpl, Sint32 nblock, Sint32 *num)
{
    Sint32 divisor;

    if (nblock == 0) {
        *num = 0;
        /* @hack redundant: a return from a branch whose else holds the rest
         * of the function */
        return;
    } else {
        for (*num = 1, divisor = 2; *num <= MW_SB_BLOCK_NUM; divisor *= 2) {
            if (nblock % divisor != 0) {
                break;
            }
            *num *= 2;
        }
    }
}

/* rotate channel ch's ring by shift blocks, from 0 to MW_SB_BLOCK_NUM.
 * every block moves shift blocks toward the ring's start, and the first
 * shift blocks go to the end. the blocks' moves form num cycles, which the
 * call follows one at a time. num must be what mwlSbCalcNumTrans stores for
 * shift */
void mwlSbReplaceDat(struct mw_sb *sb, Sint32 ch, Sint32 shift, Sint32 num)
{
    Sint32 block_nsmpl = MW_SB_BLOCK_SIZE / sb->bit_per_smpl * BYTE_BITS;
    Uint8 *base;
    Sint32 buf_block[2];
    Uint8 *buf[2];
    Uint8 *swap;
    Sint32 i;
    Sint32 j;
    Sint32 moves;
    Sint32 ofs;

    /* @hack spelling: base, a copy of mwg_sb_pbuf that buf[0] and buf[1] are
     * set from, where mwg_sb_pbuf itself would do */
    buf[0] = base = mwg_sb_pbuf;
    buf[1] = base + MW_SB_BLOCK_SIZE;
    for (i = 0; i < num; i++) {
        buf_block[0] = (shift + i) % MW_SB_BLOCK_NUM;
        ofs = buf_block[0] * block_nsmpl;
        mwlSbCopySndToWrk(sb, ch, buf[0], ofs, block_nsmpl);
        moves = MW_SB_BLOCK_NUM / num;
        for (j = 0; j < moves; j++) {
            buf_block[1] =
                (buf_block[0] - shift + MW_SB_BLOCK_NUM) % MW_SB_BLOCK_NUM;
            ofs = buf_block[1] * block_nsmpl;
            mwlSbCopySndToWrk(sb, ch, buf[1], ofs, block_nsmpl);
            mwlSbTrnsDat(sb, ch, ofs, buf[0], block_nsmpl);
            buf_block[0] = buf_block[1];
            swap = buf[0];
            buf[0] = buf[1];
            buf[1] = swap;
        }
    }
}

/* rotate every channel's ring so that the ring starts at sample nsmpl,
 * rounded up to a block boundary. nsmpl must be from 0 to the ring's count
 * of samples. *adjusted gets the rounded nsmpl. the call returns how many
 * samples the rounding added */
Sint32 mwSbAdjstBufPause(void *mwsnd, Sint32 nsmpl, Sint32 *adjusted)
{
    struct mw_sb *sb = mwsnd;
    SDE_MEMBLK_TRANSFER_MODE mode;
    Sint32 block_nsmpl;
    Sint32 nblock;
    Sint32 rest;
    Sint32 i;
    Sint32 num;
    Sint32 shift;

    sdMemBlkGetTransferMode(&mode);
    sdMemBlkSetTransferMode(SDE_MEMBLK_TRANSFER_MODE_CPU);
    block_nsmpl = MW_SB_BLOCK_SIZE / sb->bit_per_smpl * BYTE_BITS;
    /* @hack spelling: i set to 0 here, where for (i = 0; ...) at the loop
     * would do */
    i = 0;
    nblock = nsmpl / block_nsmpl;
    rest = (block_nsmpl - (nsmpl % block_nsmpl)) % block_nsmpl;
    for (; i < sb->num_chan; i++) {
        shift = nblock;
        sb->to_bound_nsmpl = rest;
        /* @hack spelling: sb->to_bound_nsmpl read back right after rest is
         * stored there, where rest would do */
        if (sb->to_bound_nsmpl > 0) {
            shift++;
        }
        mwlSbCalcNumTrans(nsmpl, shift, &num);
        mwlSbReplaceDat(sb, i, shift, num);
    }
    sdMemBlkSetTransferMode(mode);
    *adjusted = nsmpl + sb->to_bound_nsmpl;
    return sb->to_bound_nsmpl;
}

/* rotate every channel's ring so that the ring starts at sample nsmpl,
 * rounded down to a block boundary. nsmpl must be from 0 to the ring's count
 * of samples. the call returns the rounded nsmpl */
Sint32 mwSbAdjstBufDscrd(void *mwsnd, Sint32 nsmpl)
{
    struct mw_sb *sb = mwsnd;
    Sint32 nblock;
    Sint32 block_nsmpl = MW_SB_BLOCK_SIZE / sb->bit_per_smpl * BYTE_BITS;
    Sint32 num;
    Sint32 i;
    Sint32 result;
    SDE_MEMBLK_TRANSFER_MODE mode;

    nblock = nsmpl / block_nsmpl;
    if (nblock <= 0) {
        return 0;
    }
    sb->to_bound_nsmpl = block_nsmpl - (nsmpl % block_nsmpl);
    mwlSbCalcNumTrans(nsmpl, nblock, &num);
    sdMemBlkGetTransferMode(&mode);
    sdMemBlkSetTransferMode(SDE_MEMBLK_TRANSFER_MODE_CPU);
    for (i = 0; i < sb->num_chan; i++) {
        mwlSbReplaceDat(sb, i, nblock, num);
    }
    sdMemBlkSetTransferMode(mode);
    /* @hack spelling: result, a local for the product, where return nblock
     * * block_nsmpl would do */
    result = nblock * block_nsmpl;
    return result;
}

struct mw_sblib_vtbl *mwSndGetLibHn(void)
{
    return &mwg_sblib_vtbl;
}
