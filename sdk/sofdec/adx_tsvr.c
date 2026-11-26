/* the server's work on one stream: a step for each state, and an ADX step's
 * error check. AHX data has steps of its own. the decoder's traps and a file's
 * end callback carry it through a loop or into a linked play */
#include <adx_amp.h>
#include <adx_bsc.h>
#include <adx_crs.h>
#include <adx_dcd.h>
#include <adx_errs.h>
#include <adx_rnas.h>
#include <adx_sjd.h>
#include <adx_stmc.h>
#include <adx_tlk.h>
#include <adxahx.h>
#include <cri_adxt.h>
#include <mw_rna.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <sj.h>

#define SECTOR_BYTES 2048
#define NO_LIMIT 0x7fffffff
#define NO_TRAP (-1)
#define TRANSFER_BYTES 32
/* the offset of a header's encoding byte, and the encoding of AHX data */
#define ENCODING_AT 4
#define AHX_ENCODING 0x10
/* the bytes the input must hold before is_ahx reads its header */
#define AHX_TEST_BYTES 64
/* an ADX block's samples. the AHX steps size their limits by it too, though
 * an AHX granule holds 96 */
#define ADX_BLOCK_SAMPLES 32
/* a pass decodes at most the samples of LOOPED_PERIODS server periods of
 * looped data. data without a loop takes half as many. the limit then rises
 * to the next multiple of LIMIT_BLOCKS blocks above it */
#define LOOPED_PERIODS 3
#define LIMIT_BLOCKS 2
#define LOW_WATER_BYTES 64
#define STALL_PLAY_SECONDS 5
#define STALL_PREP_SECONDS 20
/* an error check runs only once CHECK_TENTHS tenths of a server period have
 * passed since the last */
#define SECOND_MICROSECONDS 1000000
#define CHECK_TENTHS 8
/* the text ADXERR_ItoA2 writes a channel count error's numbers into: the
 * size it is given, and the buffer's */
#define ITOA_SIZE 16
#define COUNTS_BYTES 32

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void adxt_trap_entry(void *obj);
void adxt_eos_entry(void *obj);
void adxt_set_outpan(ADXT adxt);
void adxt_nlp_trap_entry(void *obj);
void adxt_stat_decinfo(ADXT adxt);
void adxt_stat_prep(ADXT adxt);
void adxt_stat_playing(ADXT adxt);
void adxt_stat_decend(ADXT adxt);
void adxt_stat_playend(ADXT adxt);
void adxt_RcvrReplay(ADXT adxt);
void ADXT_ExecErrChk(ADXT adxt);
void adxt_ahx_outpan(ADXT adxt);
void ADXT_ExecHndlAhx(ADXT adxt);
/* NOLINTEND(misc-use-internal-linkage) */

/* the decoder's trap at the loop's end. it drops the input's lp_skiplen bytes
 * past that end, and sends the decoder back to the loop's start. with no
 * reader and looping off, it lets decoding run on instead */
void adxt_trap_entry(void *obj)
{
    ADXT adxt = obj;
    ADXSJD sjd = adxt->sjd;
    SJ input = adxt->sji;
    Sint32 start_sample;
    Sint32 start_offset;
    Sint32 end_sample;
    SJCK chunk;

    start_sample = ADXSJD_GetLpStartPos(sjd);
    start_offset = ADXSJD_GetLpStartOfst(sjd);
    end_sample = ADXSJD_GetLpEndPos(sjd);
    /* @hack redundant: the ADXSJD_GetLpEndOfst call, whose result nothing
     * reads */
    ADXSJD_GetLpEndOfst(sjd);
    if (!adxt->stm && !adxt->lpflg) {
        ADXSJD_SetTrapNumSmpl(adxt->sjd, NO_TRAP);
        return;
    }
    SJ_GetChunk(input, SJ_LIN_DATA, adxt->lp_skiplen, &chunk);
    if (chunk.len < adxt->lp_skiplen) {
        ADXERR_CallErrFunc1("E8101201 adxt_trap_entry: not enough data");
    }
    SJ_PutChunk(input, SJ_LIN_FREE, &chunk);
    ADXSJD_SetTrapCnt(sjd, 0);
    /* @hack spelling: trpnsmpl set inside the call's argument, where a
     * statement of its own would do */
    ADXSJD_SetTrapNumSmpl(sjd, adxt->trpnsmpl = end_sample - start_sample);
    ADXSJD_SetTrapDtLen(sjd, start_offset);
    ADXSJD_SetDecPos(sjd, start_sample);
    if (adxt->pmode == ADXT_PMODE_MEM) {
        SJ_Reset(input);
        SJ_GetChunk(input, SJ_LIN_DATA, start_offset, &chunk);
        SJ_PutChunk(input, SJ_LIN_FREE, &chunk);
    }
    adxt->lpcnt++;
}

/* the reader's end callback, at the end of the sector the loop ends in. it
 * seeks the reader back to the sector the loop starts in. with looping off,
 * it lets the stream read and decode on to the data's end instead */
void adxt_eos_entry(void *obj)
{
    ADXT adxt = obj;
    ADXSTM reader = adxt->stm;
    ADXSJD sjd = adxt->sjd;
    Sint32 start_offset;

    start_offset = ADXSJD_GetLpStartOfst(sjd);
    if (!adxt->lpflg) {
        ADXSJD_SetTrapNumSmpl(adxt->sjd, NO_TRAP);
        ADXSTM_SetEos(adxt->stm, NO_LIMIT);
        return;
    }
    ADXSTM_Seek(reader, start_offset / SECTOR_BYTES);
}

#define SET_PAN(adxt, channel, automatic)                                      \
    do {                                                                       \
        if ((adxt)->outpan[channel] == ADXT_PAN_AUTO) {                        \
            ADXRNA_SetOutPan((adxt)->rna, (channel), (automatic));             \
        } else {                                                               \
            ADXRNA_SetOutPan((adxt)->rna, (channel), (adxt)->outpan[channel]); \
        }                                                                      \
    } while (0)

void adxt_set_outpan(ADXT adxt)
{
    Sint32 channels = ADXSJD_GetNumChan(adxt->sjd);

    if (channels == 1) {
        SET_PAN(adxt, 0, ADXT_PAN_CENTER);
    } else {
        SET_PAN(adxt, 0, ADXT_PAN_LEFT);
        SET_PAN(adxt, 1, ADXT_PAN_RIGHT);
    }
}

/* the decoder's trap at the end of data without a loop. while lnkflg is set,
 * it drops the footer and the bytes up to the next ADX header. it then starts
 * the decoder on that header's data. the header may lie in either chunk of the
 * input's data, as its ring can wrap. with no footer, or no header the decoder
 * can read, it clears lnkflg */
void adxt_nlp_trap_entry(void *obj)
{
    ADXT adxt = obj;
    ADXSJD sjd = adxt->sjd;
    SJ input = adxt->sji;
    Sint16 first_offset;
    Sint16 second_offset;
    Sint16 first_skip;
#ifdef NONMATCHING
    /* this side starts second_skip at 0, so the add below reads no unset value
     * where the header lies in the first chunk */
    Sint16 second_skip = 0;
#else
    Sint16 second_skip;
#endif
    Sint32 first_result;
    Sint32 second_result;
    SJCK first;
    SJCK first_rest;
    SJCK second;
    SJCK second_rest;

    /* @hack type: ahxflg read through a byte pointer at its offset, where
     * adxt->ahxflg would do */
    if (!adxt->lnkflg ||
        ((Sint8 *)adxt)[(Sint32)(&((struct _adx_talk *)0)->ahxflg)]) {
        return;
    }
    SJ_GetChunk(input, SJ_LIN_DATA, NO_LIMIT, &first);
    SJ_GetChunk(input, SJ_LIN_DATA, NO_LIMIT, &second);
    first_offset = second_offset = 0;
    if (ADX_DecodeFooter((Sint8 *)first.data + first_offset,
                         first.len - first_offset, &first_skip) != 0) {
        adxt->lnkflg = FALSE;
        SJ_UngetChunk(input, SJ_LIN_DATA, &second);
        SJ_UngetChunk(input, SJ_LIN_DATA, &first);
        return;
    }
    first_offset += first_skip;
    first_result = ADX_ScanInfoCode((Sint8 *)first.data + first_offset,
                                    first.len - first_offset, &first_skip);
    /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
    if (first_result == 0) {
        second_result = -1;
    } else {
        second_result =
            ADX_ScanInfoCode((Sint8 *)second.data, second.len, &second_skip);
    }
    first_offset += first_skip;
    /* only the second scan sets second_skip. where the header lies in the
     * first chunk, the add reads it unset. that path never reads
     * second_offset */
    second_offset += second_skip;
    if (first_result != 0 && second_result != 0) {
        SJ_UngetChunk(input, SJ_LIN_DATA, &second);
        SJ_UngetChunk(input, SJ_LIN_DATA, &first);
        adxt->lnkflg = FALSE;
        return;
    } else if (first_result == 0) {
        SJ_UngetChunk(input, SJ_LIN_DATA, &second);
        SJ_SplitChunk(&first, first_offset, &first, &first_rest);
        SJ_PutChunk(input, SJ_LIN_FREE, &first);
        SJ_UngetChunk(input, SJ_LIN_DATA, &first_rest);
    } else {
        SJ_PutChunk(input, SJ_LIN_FREE, &first);
        SJ_SplitChunk(&second, second_offset, &second, &second_rest);
        SJ_PutChunk(input, SJ_LIN_FREE, &second);
        SJ_UngetChunk(input, SJ_LIN_DATA, &second_rest);
    }
    ADXSJD_Stop(sjd);
    ADXSJD_Start(sjd);
    ADXSJD_ExecHndl(sjd);
    if (ADXSJD_GetStat(sjd) != ADXSJD_STAT_DECODE) {
        adxt->lnkflg = FALSE;
        return;
    }
    ADXSJD_SetMaxDecSmpl(sjd, adxt->maxdecsmpl);
    ADXSJD_SetTrapNumSmpl(sjd, ADXSJD_GetTotalNumSmpl(sjd));
    ADXSJD_SetTrapDtLen(sjd, 0);
    ADXSJD_SetTrapCnt(sjd, 0);
}

void adxt_stat_decinfo(ADXT adxt)
{
    ADXSJD sjd = adxt->sjd;
    /* @hack spelling: the sector's bytes through sector_bytes, where
     * SECTOR_BYTES in each use would do */
    Sint32 sector_bytes = SECTOR_BYTES;
    Sint32 state;
    Sint32 rate;
    Sint32 loops;
    Sint32 block_samples;
    Sint32 end_offset;
    Sint32 channels;
    Sint32 total;
    Sint32 bits;
    Sint32 end_sample;
    Sint32 end_sector;
    ADXRNA renderer;
    char counts[COUNTS_BYTES];

    state = ADXSJD_GetStat(sjd);
    if (state != ADXSJD_STAT_DECODE) {
        return;
    }
    channels = ADXSJD_GetNumChan(sjd);
    if (channels > adxt->maxnch) {
        ADXERR_ItoA2(channels, adxt->maxnch, counts, ITOA_SIZE);
        ADXERR_CallErrFunc2(
            "E9081001 adxt_stat_decinfo: can't play this number of channels",
            counts);
        ADXT_Stop(adxt);
        return;
    }
    rate = ADXSJD_GetSfreq(sjd);
    loops = ADXSJD_GetNumLoop(sjd);
    if (loops > 0) {
        adxt->maxdecsmpl = rate / adxt->svrfreq * LOOPED_PERIODS;
    } else {
        adxt->maxdecsmpl = rate / adxt->svrfreq * LOOPED_PERIODS / 2;
    }
    block_samples = ADXSJD_GetBlkSmpl(sjd) * LIMIT_BLOCKS;
    adxt->maxdecsmpl =
        (adxt->maxdecsmpl + block_samples) / block_samples * block_samples;
    ADXSJD_SetMaxDecSmpl(sjd, adxt->maxdecsmpl);
    if (loops > 0) {
        if (adxt->pmode == ADXT_PMODE_MEM) {
            adxt->lp_skiplen = 0;
        } else {
            end_offset = ADXSJD_GetLpEndOfst(sjd);
            /* a file's reads stop at the end of the loop's last sector. the
             * trap drops that sector's bytes past the loop's end */
            adxt->lp_skiplen = sector_bytes - (end_offset % sector_bytes);
            adxt->lp_skiplen %= sector_bytes;
            adxt->lesct = end_sector =
                (end_offset + sector_bytes - 1) / sector_bytes;
            ADXSTM_SetEos(adxt->stm, end_sector);
            ADXSTM_EntryEosFunc(adxt->stm, adxt_eos_entry, adxt);
        }
        end_sample = ADXSJD_GetLpEndPos(sjd);
        adxt->trpnsmpl = end_sample;
        ADXSJD_SetTrapNumSmpl(sjd, end_sample);
        ADXSJD_SetTrapDtLen(sjd, 0);
        ADXSJD_SetTrapCnt(sjd, 0);
        ADXSJD_EntryTrapFunc(sjd, adxt_trap_entry, adxt);
    } else {
        if (adxt->stm) {
            ADXSTM_SetEos(adxt->stm, NO_LIMIT);
        }
        ADXSJD_SetTrapNumSmpl(sjd, ADXSJD_GetTotalNumSmpl(sjd));
        ADXSJD_SetTrapDtLen(sjd, 0);
        ADXSJD_SetTrapCnt(sjd, 0);
        ADXSJD_EntryTrapFunc(sjd, adxt_nlp_trap_entry, adxt);
    }

    rate = ADXSJD_GetSfreq(sjd);
    channels = ADXSJD_GetNumChan(sjd);
    total = ADXSJD_GetTotalNumSmpl(sjd);
    bits = ADXSJD_GetOutBps(sjd);
    renderer = adxt->rna;
    renderer->sj[0] = adxt->sjo[0];
    renderer->sj[1] = adxt->sjo[1];
    ADXRNA_SetBitPerSmpl(adxt->rna, bits);
    ADXRNA_SetSfreq(adxt->rna, rate);
    ADXRNA_SetNumChan(adxt->rna, channels);
    ADXRNA_SetTotalNumSmpl(adxt->rna, total);
    ADXRNA_SetOutVol(adxt->rna, adxt->outvol);
    adxt_set_outpan(adxt);
    if (adxt->amp) {
        ADXAMP_SetSfreq(adxt->amp, rate);
    }
    if (ADXSJD_GetFormat(sjd) == ADXB_FORMAT_SPSD) {
        ADXRNA_SetStmHdInfo(adxt->rna, ADXSJD_GetSpsdInfo(sjd));
    }
    ADXRNA_SetTransSw(adxt->rna, TRUE);
    adxt->stat = ADXT_STAT_PREP;
}

#define PREP_FUNCTION void adxt_stat_prep
#define PREP_DECODER ADXSJD sjd = adxt->sjd
#define PREP_BLOCK_SAMPLES ADXSJD_GetBlkSmpl(sjd)
#define PREP_AT_END (ADXSJD_GetStat(adxt->sjd) == ADXSJD_STAT_DECEND)
#define PREP_OUTPUTS adxt->sjo
#include "../sofdec/adx_tsvr_prep_impl.h"

void adxt_stat_playing(ADXT adxt)
{
    Sint32 channel;
    Sint32 channels;

    if (ADXSJD_GetStat(adxt->sjd) == ADXSJD_STAT_DECEND) {
        channels = ADXSJD_GetNumChan(adxt->sjd);
        for (channel = 0; channel < channels; channel++) {
            if (SJ_GetNumData(adxt->sjo[channel], SJ_LIN_DATA) >=
                TRANSFER_BYTES) {
                break;
            }
        }
        if (channel == channels) {
            ADXRNA_SetTransSw(adxt->rna, FALSE);
            adxt->stat = ADXT_STAT_DECEND;
        }
    }
}

void adxt_stat_decend(ADXT adxt)
{
    if (ADXRNA_GetNumData(adxt->rna) <= 0) {
        ADXRNA_SetPlaySw(adxt->rna, FALSE);
        adxt->stat = ADXT_STAT_PLAYEND;
    }
}

void adxt_stat_playend(ADXT adxt)
{
}

void adxt_RcvrReplay(ADXT adxt)
{
    Sint32 channel;

    ADXCRS_Lock();
    ADXRNA_SetTransSw(adxt->rna, FALSE);
    ADXRNA_SetPlaySw(adxt->rna, FALSE);
    ADXSJD_Stop(adxt->sjd);
    if (adxt->stm) {
        ADXSTM_Stop(adxt->stm);
        SJ_Reset(adxt->sji);
    }
    for (channel = 0; channel < adxt->maxnch; channel++) {
        SJ_Reset(adxt->sjo[channel]);
    }
    if (adxt->stm) {
        ADXSTM_Seek(adxt->stm, 0);
        ADXSTM_Start(adxt->stm);
    }
    adxt_start_sj(adxt, adxt->sji);
    ADXCRS_Unlock();
}

void ADXT_ExecErrChk(ADXT adxt)
{
    static Uint32 last_count = 0;
    Sint32 state = adxt->stat;
    /* @hack redundant: count declared register, where a plain local would do */
    register Uint32 count;
    Sint32 elapsed;
    Sint32 position;
    Sint32 bytes;
    Sint32 recovery;

    /* every stream shares last_count */
    count = syTmrGetCount();
    elapsed = syTmrCountToMicro(syTmrDiffCount(last_count, count));
    if (elapsed < SECOND_MICROSECONDS / adxt->svrfreq * CHECK_TENTHS / 10) {
        return;
    }
    last_count = count;
    if (state == ADXT_STAT_PLAYING && adxt->pause_flag == 0 &&
        ADXSJD_GetStat(adxt->sjd) != ADXSJD_STAT_DECEND) {
        position = ADXSJD_GetDecNumSmpl(adxt->sjd);
        if (adxt->edecpos == position) {
            adxt->edeccnt++;
            if (adxt->edeccnt > adxt->svrfreq * STALL_PLAY_SECONDS) {
                adxt->ercode = ADXT_ERR_SNDBLK;
            }
        } else {
            adxt->edeccnt = 0;
        }
        adxt->edecpos = position;
        if (adxt->ercode != ADXT_ERR_OK) {
            recovery = adxt->autorcvr;
            if (recovery == ADXT_RMODE_STOP) {
                ADXT_Stop(adxt);
                /* @bug logic: a stalled decoder stops the stream in replay
                 * mode too, where the other two checks replay it. the replay
                 * test at 0x8c1577be reaches the stop at 0x8c1577c2 */
            } else if (recovery == ADXT_RMODE_REPLAY) {
                ADXT_Stop(adxt);
            }
            if (adxt->autorcvr) {
                adxt->ercode = ADXT_ERR_OK;
                adxt->edeccnt = 0;
            }
        }
    } else {
        adxt->edeccnt = 0;
    }

    if (state >= ADXT_STAT_DECINFO && state <= ADXT_STAT_PLAYING &&
        adxt->pause_flag == 0 &&
        ADXSJD_GetStat(adxt->sjd) != ADXSJD_STAT_DECEND) {
        if (adxt->sji) {
            bytes = SJ_GetNumData(adxt->sji, SJ_LIN_DATA);
        } else {
            bytes = 0;
        }
        if (bytes < LOW_WATER_BYTES) {
            adxt->eshrtcnt++;
            if (state == ADXT_STAT_PLAYING) {
                if (adxt->eshrtcnt > adxt->svrfreq * STALL_PLAY_SECONDS) {
                    adxt->ercode = ADXT_ERR_SHRTBUF;
                }
            } else if (adxt->eshrtcnt > adxt->svrfreq * STALL_PREP_SECONDS) {
                adxt->ercode = ADXT_ERR_SHRTBUF;
            }
            if (adxt->ercode != ADXT_ERR_OK) {
                if (adxt->autorcvr == ADXT_RMODE_STOP) {
                    ADXT_Stop(adxt);
                } else if (adxt->autorcvr == ADXT_RMODE_REPLAY) {
                    adxt_RcvrReplay(adxt);
                }
                if (adxt->autorcvr) {
                    adxt->ercode = ADXT_ERR_OK;
                    adxt->eshrtcnt = 0;
                }
            }
        } else {
            adxt->eshrtcnt = 0;
        }
    } else {
        adxt->eshrtcnt = 0;
    }
    if (adxt->stm) {
        if (ADXSTM_GetStat(adxt->stm) == ADXSTM_STAT_ERROR) {
            if (adxt->autorcvr == ADXT_RMODE_STOP) {
                ADXT_Stop(adxt);
            } else if (adxt->autorcvr == ADXT_RMODE_REPLAY) {
                adxt_RcvrReplay(adxt);
            }
            if (adxt->autorcvr) {
                adxt->ercode = ADXT_ERR_OK;
                adxt->eshrtcnt = 0;
            }
        }
    }
}

static void step_adx(ADXT adxt)
{
    if (adxt->stat == ADXT_STAT_PLAYING) {
        adxt_stat_playing(adxt);
    } else if (adxt->stat == ADXT_STAT_DECINFO) {
        if (ADXSJD_GetStat(adxt->sjd) == ADXSJD_STAT_STOP) {
            ADXSJD_Start(adxt->sjd);
        }
        adxt_stat_decinfo(adxt);
    } else if (adxt->stat == ADXT_STAT_PREP) {
        adxt_stat_prep(adxt);
    } else if (adxt->stat == ADXT_STAT_DECEND) {
        adxt_stat_decend(adxt);
    } else if (adxt->stat == ADXT_STAT_PLAYEND) {
        adxt_stat_playend(adxt);
    }
    ADXT_ExecErrChk(adxt);
}

void adxt_ahx_outpan(ADXT adxt)
{
    Sint32 channels = adxahx_channels(adxt->ahx);

    if (channels == 1) {
        SET_PAN(adxt, 0, ADXT_PAN_CENTER);
    } else {
        SET_PAN(adxt, 0, ADXT_PAN_LEFT);
        SET_PAN(adxt, 1, ADXT_PAN_RIGHT);
    }
}

static void ahx_decinfo(ADXT adxt)
{
    ADXAHX ahx = adxt->ahx;
    Sint32 state;
    Sint32 block_samples;
    Sint32 rate;
    Sint32 channels;
    Sint32 total;
    Sint32 bits;
    Sint32 volume;
    Sint32 direct;
    Sint32 pan;
    Sint32 effect_channel;
    Sint32 effect_level;
    ADXRNA renderer;
    char counts[COUNTS_BYTES];

    state = adxahx_state(ahx);
    if (state != ADXAHX_DECODE) {
        return;
    }
    channels = adxahx_channels(ahx);
    if (channels > adxt->maxnch) {
        ADXERR_ItoA2(channels, adxt->maxnch, counts, ITOA_SIZE);
        ADXERR_CallErrFunc2(
            "E9081001 adxt_stat_decinfo: can't play this number of channels",
            counts);
        ADXT_Stop(adxt);
        return;
    }
    if (adxt->stm) {
        ADXSTM_SetEos(adxt->stm, NO_LIMIT);
    }
    /* @hack redundant: the first adxahx_sample_rate call, whose result the
     * second replaces */
    rate = adxahx_sample_rate(ahx);
    block_samples = ADX_BLOCK_SAMPLES * LIMIT_BLOCKS;
    adxt->maxdecsmpl =
        (adxt->maxdecsmpl + block_samples) / block_samples * block_samples;
    adxahx_set_sample_limit(ahx, adxt->maxdecsmpl);
    rate = adxahx_sample_rate(ahx);
    channels = adxahx_channels(ahx);
    total = adxahx_total_samples(ahx);
    bits = adxahx_sample_bits(ahx);
    volume = adxahx_output_volume(ahx);
    direct = adxahx_direct_level(ahx);
    adxahx_effect(ahx, &effect_channel, &effect_level);
    pan = adxahx_pan(ahx, 0);
    renderer = adxt->rna;
    renderer->sj[0] = adxt->ahxsjo[0];
    renderer->sj[1] = adxt->ahxsjo[1];
    ADXRNA_SetBitPerSmpl(adxt->rna, bits);
    ADXRNA_SetSfreq(adxt->rna, rate);
    ADXRNA_SetNumChan(adxt->rna, channels);
    ADXRNA_SetTotalNumSmpl(adxt->rna, total);
    ADXT_SetOutVol(adxt, volume);
    ADXT_SetOutPan(adxt, 0, pan);
    ADXT_SetFx(adxt, effect_channel, effect_level);
    ADXT_SetDrctLvl(adxt, direct);
    if (adxt->amp) {
        ADXAMP_SetSfreq(adxt->amp, rate);
    }
    ADXRNA_SetTransSw(adxt->rna, TRUE);
    adxt->stat = ADXT_STAT_PREP;
}

#define PREP_FUNCTION static void ahx_prep
#define PREP_DECODER ADXAHX ahx = adxt->ahx
#define PREP_BLOCK_SAMPLES ADX_BLOCK_SAMPLES
#define PREP_AT_END (adxahx_state(ahx) == ADXAHX_END)
#define PREP_OUTPUTS adxt->ahxsjo
#include "../sofdec/adx_tsvr_prep_impl.h"

static void ahx_playing(ADXT adxt)
{
    ADXAHX ahx = adxt->ahx;
    Sint32 channel;
    Sint32 channels;

    if (adxahx_state(ahx) == ADXAHX_END) {
        channels = adxahx_channels(ahx);
        for (channel = 0; channel < channels; channel++) {
            if (SJ_GetNumData(adxt->ahxsjo[channel], SJ_LIN_DATA) >=
                TRANSFER_BYTES) {
                break;
            }
        }
        if (channel == channels) {
            ADXRNA_SetTransSw(adxt->rna, FALSE);
            adxt->stat = ADXT_STAT_DECEND;
        }
    }
}

static void ahx_decend(ADXT adxt)
{
    if (ADXRNA_GetNumData(adxt->rna) <= 0) {
        ADXRNA_SetPlaySw(adxt->rna, FALSE);
        adxt->stat = ADXT_STAT_PLAYEND;
    }
}

static void ahx_playend(ADXT adxt)
{
}

static Bool is_ahx(ADXT adxt)
{
    Bool result = TRUE;
    const Uint8 *header;
    Uint16 mark;
    Uint8 encoding;
    SJCK chunk;

    SJ_GetChunk(adxt->sji, SJ_LIN_DATA, AHX_TEST_BYTES, &chunk);
    /* @bug logic: is_ahx hangs where the input holds AHX_TEST_BYTES but its
     * first chunk holds fewer, as where the data wraps the ring. the test at
     * 0x8c157d6e then enters the endless loop at 0x8c157d70 */
    if (chunk.len < AHX_TEST_BYTES) {
        for (;;) {
        }
    }
    header = (const Uint8 *)chunk.data;
#ifdef NONMATCHING
    /* this side reads the mark's two bytes, the plain form the hack tag below
     * gives, which needs no alignment */
    mark = (header[0] << 8) | header[1];
#else
    /* @hack type: the mark read as an aligned Sint16 and its bytes swapped,
     * where a read of its two bytes would do for an even chunk */
    mark = ((*(const Sint16 *)header >> 8) & 0xff) |
           ((*(const Sint16 *)header << 8) & 0xff00);
#endif
    if (mark != ADX_MARK) {
        result = FALSE;
    } else {
        encoding = header[ENCODING_AT];
        if (encoding != AHX_ENCODING) {
            result = FALSE;
        }
    }
    SJ_UngetChunk(adxt->sji, SJ_LIN_DATA, &chunk);
    return result;
}

void ADXT_ExecHndlAhx(ADXT adxt)
{
    if (adxt->ahx) {
        /* @hack spelling: five adxahx_step calls written out, where a loop
         * would do */
        adxahx_step(adxt->ahx);
        adxahx_step(adxt->ahx);
        adxahx_step(adxt->ahx);
        adxahx_step(adxt->ahx);
        adxahx_step(adxt->ahx);
    }
    if (adxt->stat == ADXT_STAT_PLAYING) {
        ahx_playing(adxt);
    } else if (adxt->stat == ADXT_STAT_DECINFO) {
        if (SJ_GetNumData(adxt->sji, SJ_LIN_DATA) >= AHX_TEST_BYTES) {
            if (!is_ahx(adxt)) {
                adxt->ahxflg = FALSE;
            } else if (!adxt->ahx) {
                ADXERR_CallErrFunc1("E0122801 ADXT_ExecHndlAhx: can't play AHX "
                                    "data by this handle");
                adxt->ahxflg = FALSE;
            } else {
                if (adxahx_state(adxt->ahx) == ADXAHX_STOP) {
                    adxahx_start(adxt->ahx);
                }
                adxahx_step(adxt->ahx);
                ahx_decinfo(adxt);
            }
        }
    } else if (adxt->stat == ADXT_STAT_PREP) {
        ahx_prep(adxt);
    } else if (adxt->stat == ADXT_STAT_DECEND) {
        ahx_decend(adxt);
    } else if (adxt->stat == ADXT_STAT_PLAYEND) {
        ahx_playend(adxt);
    }
}

void ADXT_ExecHndl(ADXT adxt)
{
    if (adxt->ahxflg == TRUE) {
        ADXT_ExecHndlAhx(adxt);
    }
    if (adxt->ahxflg == FALSE) {
        step_adx(adxt);
    }
}
