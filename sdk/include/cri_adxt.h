/* CRI's ADX streams (ADXT) and the expander that decodes their data, as far as
 * the sources use them */
#ifndef CRI_ADXT_H
#define CRI_ADXT_H
#include <adx_stmc.h>
#include <sg_xpt.h>
#include <sj.h>

/* a stream's handle */
typedef struct _adx_talk *ADXT;
/* the handles of a stream's parts. a decoder (ADXSJD) decodes the ADX data
 * of one joint into a joint of samples for each channel. a sound output
 * (ADXRNA) plays those joints. an amplitude monitor (ADXAMP) measures them */
typedef struct _adxsjd_obj *ADXSJD;
typedef struct _adxrna_obj *ADXRNA;
typedef struct _adxamp_obj *ADXAMP;

/* the streams the library holds at once */
#define ADXT_MAX_OBJ 8
/* the most channels a stream plays */
#define ADXT_MAX_NCH 2

/* ADXT_GetStat's states, in the order a stream passes them. a started
 * stream reads its data's header, decodes before it plays, and decodes as it
 * plays. after the data's end, it plays what it has decoded until it has
 * played to the end */
#define ADXT_STAT_STOP 0
#define ADXT_STAT_DECINFO 1
#define ADXT_STAT_PREP 2
#define ADXT_STAT_PLAYING 3
#define ADXT_STAT_DECEND 4
#define ADXT_STAT_PLAYEND 5

/* ADXT_SetAutoRcvr's mode in which a stream takes no action after an error */
#define ADXT_RMODE_NOACT 0
/* ADXT_SetAutoRcvr's modes, for what a stream does after an error: stop, or
 * play again */
#define ADXT_RMODE_STOP 1
#define ADXT_RMODE_REPLAY 2

/* ADXT_SetTimeMode's modes */
#define ADXT_TMODE_SFREQ 0
#define ADXT_TMODE_NTSC 1
#define ADXT_TMODE_PAL 2

/* the pan that follows the data. at each play's start, the server pans a
 * mono channel to the center. it pans stereo's two to the left and the
 * right */
#define ADXT_PAN_AUTO (-128)
/* the pans of the far left, the center and the far right */
#define ADXT_PAN_LEFT (-15)
#define ADXT_PAN_CENTER 0
#define ADXT_PAN_RIGHT 15

/* the samples of a channel's output ring. a stream's work area holds each
 * channel's ring ADXT_OBUF_DIST samples after the last */
#define ADXT_OBUF_SIZE 0x2000
#define ADXT_OBUF_DIST 0x2020
/* the samples of output the work area keeps for each channel. the AHX
 * joint's xsize takes those past ADXT_OBUF_SIZE */
#define ADXT_OBUF_WORK 0x2060
/* the bytes past the input ring's end that hold a copy of its start, as
 * SJRBF_Create's xsize */
#define ADXT_IBUF_XLEN 36

/* a stream's record. the library holds ADXT_MAX_OBJ of them, and
 * ADXT_Create hands one out. its fields take the names the SDK's header
 * gives them.
 *
 * a stream's decoder reads sji, which is sjf for a file. the decoder writes
 * sjo, a joint of samples for each channel, which the sound output plays.
 * the work area ADXT_Create is given holds obuf and then ibuf. sjo and
 * ahxsjo lie over obuf, and sjf over ibuf */
typedef struct _adx_talk {
    Sint8 used; /* TRUE while the record is handed out */
    Sint8 stat; /* an ADXT_STAT_ state */
    Sint8 pmode; /* what the stream plays from, an ADXT_PMODE_ value */
    Sint8 maxnch; /* the channels ADXT_Create made the stream for */
    ADXSJD sjd;
    ADXSTM stm; /* the reader of the file played, or null */
    ADXRNA rna;
    SJ sjf; /* the joint stm reads into */
    SJ sji; /* null while stopped */
    SJ sjo[ADXT_MAX_NCH];
    char *ibuf;
    Sint32 ibuflen; /* ibuf's ring in bytes, a whole number of sectors */
    Sint32 ibufxlen; /* ADXT_IBUF_XLEN */
    Sint16 *obuf;
    Sint32 obufsize; /* ADXT_OBUF_SIZE */
    Sint32 obufdist; /* ADXT_OBUF_DIST */
    Sint32 svrfreq; /* as ADXT_SetSvrFreq sets */
    /* a file's stm asks for room of maxsct sectors. it reads while sjf holds
     * fewer than minsct */
    Sint16 maxsct;
    Sint16 minsct;
    Sint16 outvol; /* the vol ADXT_SetOutVol was last given */
    Sint16 outpan[ADXT_MAX_NCH]; /* each pan ADXT_SetOutPan was last given */
    Sint32 maxdecsmpl; /* the samples a server pass decodes at most */
    Sint32 lpcnt;
    Sint32 lp_skiplen;
    Sint32 trp;
    Sint32 wpos;
    Sint32 mofst;
    Sint16 ercode;
    Sint32 edecpos;
    Sint16 edeccnt;
    Sint16 eshrtcnt;
    Sint8 lpflg;
    Sint8 autorcvr; /* an ADXT_RMODE_ mode */
    Sint8 fltmode;
    Sint8 execflag;
    Sint8 pstwait_flag;
    Sint8 pstready_flag;
    Sint8 pause_flag; /* the sw ADXT_Pause was last given */
    ADXAMP amp; /* an amplitude monitor, or null */
    SJ ampsji[ADXT_MAX_NCH];
    SJ ampsjo[ADXT_MAX_NCH];
    Sint32 time_ofst; /* what ADXT_GetTime adds to each count */
    /* the loop's end sector and the decoder trap's sample count. a
     * start of a file or of memory sets them to 0x7fffffff and -1 */
    Sint32 lesct;
    Sint32 trpnsmpl;
    struct _lsc_obj *lsc; /* the stream's LSC, which reads into sjf */
    /* the switch for linked plays, which ADXT_SetLnkSw sets. ADXT_Create
     * and ADXT_StartSj set it, and the other starts clear it.
     * the server clears it at the data's end where no ADX footer, or no next
     * ADX header, follows */
    Sint8 lnkflg;
    /* TRUE while the data may be AHX. a start of a file or of memory sets
     * it. the server clears it once the data's header isn't AHX's */
    Sint8 ahxflg;
    Sint16 rsv2;
    /* in a time mode of V-blanks, ADXT_GetTime counts on from tvofst, a time
     * in its units. svcnt is adxt_vsync_cnt at that time */
    Sint32 tvofst;
    Sint32 svcnt;
    struct adxahx_player *ahx; /* the AHX decoder's handle, or null */
    SJ ahxsjo[ADXT_MAX_NCH]; /* the joints ahx writes, over obuf */
} ADX_TALK;

/* ADX_TALK's pmodes: a file, ADX data in memory, a joint the caller gives,
 * and the files of the stream's LSC */
#define ADXT_PMODE_FNAME 0
#define ADXT_PMODE_MEM 2
#define ADXT_PMODE_SJ 3
#define ADXT_PMODE_SLFILE 4
/* the length ADXT_StartMem and ADXT_StartMemIdx give SJMEM_Create, for data
 * whose end they aren't told */
#define ADXT_MAX_DATASIZE 0x40000000

/* pairs of ADXT_Init and ADXT_Finish nest. the outermost ADXT_Init starts
 * each part of the library, and frees every stream. it also adds
 * ADXT_VsyncProc to the V-blank's chain, unless ADXT_ConfigVsyncSvr was last
 * given a flag other than TRUE. the outermost ADXT_Finish takes
 * ADXT_VsyncProc off. it ends each part but the decoder of AHX data */
void ADXT_Init(void);
void ADXT_Finish(void);
/* start the library as the outermost ADXT_Init does, whatever ADXT_Init
 * calls came before. nothing is ended first. one ADXT_Finish then ends the
 * library */
void ADXT_ResetLibrary(void);
/* count a V-blank, and call ADXT_ExecServer */
void ADXT_VsyncProc(void);
/* move the library on once. the call runs the servers of the decoders,
 * the sound output, ADXF and adx_stmc.h's readers. it moves each stream in
 * use on. it also runs the server of LSC, cri_lsc.h's linking stream
 * controllers */
void ADXT_ExecServer(void);
/* move the stream on once, by its state */
void ADXT_ExecHndl(ADXT adxt);

/* make a stream of up to maxnch channels in the worksize bytes at work. it
 * returns the stream, or null while the library holds ADXT_MAX_OBJ streams. it
 * also returns null where a part of the stream can't be made, after it destroys
 * the stream. the parts are its joints, its decoder, its sound output and its
 * LSC, a linking stream controller of cri_lsc.h.
 *
 * maxnch is 1 or ADXT_MAX_NCH. the stream takes the work area from its first
 * ADXT_WORK_ALIGN-byte boundary. there it keeps ADXT_OBUF_WORK samples of
 * output a channel. an input ring of the whole sectors left follows, after
 * ADXT_IBUF_XLEN bytes are set aside. worksize must leave room for a sector. a
 * stream whose AHX decoder can't be made plays ADX data alone.
 *
 * the new stream is stopped, with lpflg and lnkflg set. it recovers by
 * ADXT_RMODE_STOP. its volume is 0, and each channel's pan ADXT_PAN_AUTO.
 * its svrfreq is ADXT_DEF_SVRFREQ. its maxsct is its input ring's sectors,
 * and its minsct ADXT_RELOAD_SHARE of maxsct, truncated */
ADXT ADXT_Create(Sint32 maxnch, void *work, Sint32 worksize);
#define ADXT_WORK_ALIGN 32
#define ADXT_DEF_SVRFREQ 60
#define ADXT_RELOAD_SHARE 0.85F
/* stop the stream, destroy what ADXT_Create made for it, and free it. a
 * null adxt is ignored */
void ADXT_Destroy(ADXT adxt);
/* destroy each stream in use. ADXT_CloseAllHandles does the same, and closes
 * no ADXF handle */
void ADXT_DestroyAll(void);
void ADXT_CloseAllHandles(void);

/* each start stops the stream, and then plays what it is given.
 * ADXT_StartAfs plays file fid of the ADXF partition patid.
 * ADXT_StartFname plays the file fname. ADXT_StartMem2 plays the datlen
 * bytes of ADX data at adxdat. ADXT_StartMem plays the data at adxdat as
 * ADXT_StartMem2 does, with a datlen of ADXT_MAX_DATASIZE. ADXT_StartMemIdx
 * plays entry no of the ACX data at acx, a set of ADX data. ADXT_StartSj
 * plays the ADX data that sj holds, and never destroys sj.
 *
 * two starts refuse their arguments, and play nothing. ADXT_StartAfs refuses
 * a patid or fid that ADXF_GetFnameRangeEx refuses, which reports it.
 * ADXT_StartMemIdx refuses a no outside the set's entries, with no report.
 *
 * a start fails where adx_stmc.h's open of its file returns null, or where
 * SJMEM_Create returns null. a failed start reports the failure, and plays
 * nothing. a failed start also calls ADXCRS_Unlock once more than
 * ADXCRS_Lock: a spare ADXCRS_Unlock, as adx_crs.h names it.
 *
 * ADXT_StartSj starts its decoder itself, and sets lnkflg. it leaves
 * ahxflg, lesct, trpnsmpl and pstready_flag as the last play left them. the
 * other starts reset them as adx_tlk.h's adxt_start_sj does, and clear
 * lnkflg */
void ADXT_StartAfs(ADXT adxt, Sint32 patid, Sint32 fid);
void ADXT_StartFname(ADXT adxt, char *fname);
void ADXT_StartMem(ADXT adxt, void *adxdat);
void ADXT_StartMem2(ADXT adxt, void *adxdat, Sint32 datlen);
void ADXT_StartMemIdx(ADXT adxt, void *acx, Sint32 no);
void ADXT_StartSj(ADXT adxt, SJ sj);
/* stop the stream and its parts. the stream becomes ADXT_STAT_STOP, and sji
 * null. the call closes the file the stream reads, and destroys the joint an
 * ADXT_PMODE_MEM start made. it stops an ADXT_PMODE_SLFILE stream's LSC */
void ADXT_Stop(ADXT adxt);
/* the stream's state, an ADXT_STAT_ value */
Sint32 ADXT_GetStat(ADXT adxt);

/* count each stream's time in mode's unit. ADXT_TMODE_SFREQ counts the
 * samples the sound output has played. ADXT_TMODE_NTSC counts NTSC's
 * V-blanks, 59.94 a second, as 100 units each. a second is then 5994 units.
 * ADXT_TMODE_PAL counts PAL's V-blanks, 50 a second, alike. a second is then
 * 5000 units. a mode other than ADXT_TMODE_SFREQ needs a call with
 * ADXT_TMODE_NTSC or ADXT_TMODE_PAL first, which sets the unit */
void ADXT_SetTimeMode(Sint32 mode);
/* the time played: *ncount in units of 1 / *tscale seconds. *ncount adds
 * time_ofst, in those units. a stream that isn't playing has a time of 0.
 * one played to its end has its whole length.
 *
 * in a mode of V-blanks, a playing stream counts on from tvofst by the
 * V-blanks since svcnt. the call compares that count with the sound output's
 * time. past 60 milliseconds of drift, it sets tvofst and svcnt from the
 * sound output's time again. the comparison adds time_ofst to the sound
 * output's time as samples, but not to the count. so a time_ofst past 60
 * milliseconds of samples, either way, resets tvofst and svcnt at every
 * call. one played to its end gives its length plus tvofst plus 1 */
void ADXT_GetTime(ADXT adxt, Sint32 *ncount, Sint32 *tscale);
/* the time ADXT_GetTime gives, in hundredths of a second, truncated */
Sint32 ADXT_GetTimeReal(ADXT adxt);

/* ADXT_GetNumSmpl returns the data's samples a channel, ADXT_GetSfreq its
 * sampling rate and ADXT_GetNumChan its channels. ADXT_GetHdrLen returns its
 * header's bytes, and ADXT_GetFmtBps its bits a sample. each returns 0
 * before ADXT_STAT_PREP */
Sint32 ADXT_GetNumSmpl(ADXT adxt);
Sint32 ADXT_GetSfreq(ADXT adxt);
Sint32 ADXT_GetNumChan(ADXT adxt);
Sint32 ADXT_GetHdrLen(ADXT adxt);
Sint32 ADXT_GetFmtBps(ADXT adxt);

/* record pan as channel ch's, and pass it to the sound output. the sound
 * output clamps pan to -15 through 15, from full left to full right. a ch
 * of maxnch or more is reported, and pans nothing. by a bug, a ch of
 * ADXT_MAX_NCH or more is still recorded, past outpan's end.
 * ADXT_GetOutPan returns the pan last recorded for ch */
void ADXT_SetOutPan(ADXT adxt, Sint32 ch, Sint32 pan);
Sint32 ADXT_GetOutPan(ADXT adxt, Sint32 ch);
/* set the stream's output volume to vol, where 0 is full volume. the sound
 * output clamps vol to -999 through 0. the SDK's header gives vol in tenths
 * of a decibel.
 *
 * ADXT_GetOutVol returns the last vol ADXT_SetOutVol was given, before the
 * clamp. the stream keeps vol in a Sint16, so a vol outside a Sint16's range
 * comes back changed */
void ADXT_SetOutVol(ADXT adxt, Sint32 vol);
Sint32 ADXT_GetOutVol(ADXT adxt);
/* set and get the sound output's direct level, effect send, filter and
 * transposition. each call passes its arguments on to the mwRna call of the
 * same name. the transposition's go to mwRnaSetPitch and mwRnaGetPitch */
void ADXT_SetDrctLvl(ADXT adxt, Sint32 drctlvl);
Sint32 ADXT_GetDrctLvl(ADXT adxt);
void ADXT_SetFx(ADXT adxt, Sint32 fxch, Sint32 fxlvl);
void ADXT_GetFx(ADXT adxt, Sint32 *fxch, Sint32 *fxlvl);
void ADXT_SetFilter(ADXT adxt, Sint32 coff, Sint32 q);
void ADXT_GetFilter(ADXT adxt, Sint32 *coff, Sint32 *q);
void ADXT_SetTranspose(ADXT adxt, Sint32 transps, Sint32 detune);
void ADXT_GetTranspose(ADXT adxt, Sint32 *transps, Sint32 *detune);

/* record on the stream that the caller runs the library's server freq
 * times a second. ADXT_Create records ADXT_DEF_SVRFREQ */
void ADXT_SetSvrFreq(ADXT adxt, Sint32 freq);
/* set minsct to the sectors that time seconds of ADX data fill, at most
 * maxsct. the data has nch channels at sfreq samples a second. it takes 18
 * bytes a channel for each 32 samples, and each step truncates.
 * ADXT_SetReloadSct sets minsct to its argument. a file's start reads
 * minsct */
void ADXT_SetReloadTime(ADXT adxt, Float time, Sint32 nch, Sint32 sfreq);
void ADXT_SetReloadSct(ADXT adxt, Sint32 minsct);
/* the whole sectors of data sji holds, or 0 while sji is null */
Sint32 ADXT_GetNumSctIbuf(ADXT adxt);
/* the seconds of ADX data sji holds, by the data's channels and sampling
 * rate. the call returns 0 before ADXT_STAT_PREP, or while sji is null */
Float ADXT_GetIbufRemainTime(ADXT adxt);
/* TRUE where sji holds minsct sectors of data or more. FALSE where it holds
 * fewer, or is null */
Sint32 ADXT_IsIbufSafety(ADXT adxt);

/* set the stream's ADXT_RMODE_ mode */
void ADXT_SetAutoRcvr(ADXT adxt, Sint32 rmode);
/* TRUE where the stream is ADXT_STAT_PLAYEND */
Sint32 ADXT_IsCompleted(ADXT adxt);
/* the stream's last error, ercode, or 0 for none. ADXT_ClearErrCode sets
 * ercode to 0, and clears edecpos, edeccnt and eshrtcnt with it */
Sint32 ADXT_GetErrCode(ADXT adxt);
void ADXT_ClearErrCode(ADXT adxt);
/* ercode where the stream has no error */
#define ADXT_ERR_OK 0
/* ADXT_GetErrCode's errors: an input that has run short, and a decoder that
 * has stalled */
#define ADXT_ERR_SHRTBUF (-1)
#define ADXT_ERR_SNDBLK (-2)
/* the loops the stream has played since its start */
Sint32 ADXT_GetLpCnt(ADXT adxt);
/* with flg FALSE, the stream plays its data once, past its loop */
void ADXT_SetLpFlg(ADXT adxt, Sint32 flg);
/* sji: the joint the decoder reads, or null while stopped */
SJ ADXT_GetInputSj(ADXT adxt);
/* with flg TRUE, a stream that has decoded enough to play stays
 * ADXT_STAT_PREP, and ADXT_IsReadyPlayStart returns TRUE. a start of a file
 * or of memory has ADXT_IsReadyPlayStart return FALSE again */
void ADXT_SetWaitPlayStart(ADXT adxt, Sint32 flg);
Sint32 ADXT_IsReadyPlayStart(ADXT adxt);
/* pause the stream with sw TRUE, and resume it with FALSE. any other sw
 * resumes the play, but ADXT_GetTime and ADXT_DiscardSmpl take the stream
 * as paused.
 *
 * a pause turns the sound output's play off, then spins 500000 empty passes
 * of a loop. it then adjusts the output's buffer with mwRnaAdjustBuf. a
 * stream that isn't ADXT_STAT_PLAYING or ADXT_STAT_DECEND only records sw,
 * and pauses from its play's start */
void ADXT_Pause(ADXT adxt, Sint32 sw);
/* have func take each error, with obj, from now on, through
 * ADXERR_EntryErrFunc */
void ADXT_EntryErrFunc(void (*func)(void *obj, char *msg), void *obj);
/* while the stream is paused, drop up to nsmpl samples of what is decoded,
 * and run ADXT_ExecServer once. it returns the samples dropped. while the
 * stream isn't paused, it returns 0 and does nothing */
Sint32 ADXT_DiscardSmpl(ADXT adxt, Sint32 nsmpl);
void ADXT_SetTimeOfst(ADXT adxt, Sint32 ofst);
/* set and get lnkflg. while it is set, the server looks past the data's end
 * for more ADX data in sji. it plays what it finds on. ADXT_GetLnkSw can
 * return FALSE after ADXT_SetLnkSw(adxt, TRUE), where the server cleared
 * it */
void ADXT_SetLnkSw(ADXT adxt, Sint32 sw);
Sint32 ADXT_GetLnkSw(ADXT adxt);
/* have the decoder call f with obj each time it decodes a channel's
 * samples. ch is the channel, and data the dtlen bytes it wrote */
void ADXT_EntryFltFunc(ADXT adxt,
                       void (*f)(void *obj, Sint32 ch, void *data,
                                 Sint32 dtlen),
                       void *obj);

/* TRUE where the siz bytes at adr open with an ADX header that
 * ADX_DecodeInfo reads. *hdrsiz then gets the header's bytes. FALSE where
 * they don't, and *hdrsiz is left alone */
Sint32 ADXT_IsHeader(char *adr, Sint32 siz, Sint32 *hdrsiz);
/* TRUE where the siz bytes at adr open with ADX's end code. *endsiz then gets
 * siz. FALSE where they don't, and *endsiz is left alone */
Sint32 ADXT_IsEndcode(char *adr, Sint32 siz, Sint32 *endsiz);
/* put silence for nsmpl samples of nch channels on sji's data line. the
 * silence is whole ADX blocks of 32 samples. the call returns the samples a
 * channel it put. that is nsmpl rounded down to a block, or less where sji's
 * free line holds less room. it returns 0 while sji is null */
Sint32 ADXT_InsertSilence(ADXT adxt, Sint32 nch, Sint32 nsmpl);

/* TRUE once the expander, ADXPD, has met a stereo decode of an odd count of
 * blocks. nothing in the library clears it */
extern Bool adxpd_internal_error;

#endif
