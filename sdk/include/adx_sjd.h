/* ADX's stream decoders (ADXSJD). a decoder decodes the data of an input
 * joint into a joint of 16-bit samples for each channel. a basic decoder of
 * its own does the decoding. a decoder moves on only in ADXSJD_ExecServer */
#ifndef ADX_SJD_H
#define ADX_SJD_H
#include <cri_adxt.h>
#include <sg_xpt.h>
#include <sj.h>

/* a decoder's states: stopped, reading its data's header, decoding, and done
 * with the data */
#define ADXSJD_STAT_STOP 0
#define ADXSJD_STAT_DECINFO 1
#define ADXSJD_STAT_DECODE 2
#define ADXSJD_STAT_DECEND 3

/* start and end ADX's decoders. each call frees every decoder, and
 * ADXSJD_Init also starts the basic decoders */
void ADXSJD_Init(void);
void ADXSJD_Finish(void);
/* make a stopped decoder that reads sji and writes the maxnch ring buffer
 * joints at sjo. each ring after the first must start right past the extra
 * bytes of the ring before. ADXT_Create lays them out so. the call
 * returns null while every decoder is in use, or where no basic decoder is
 * free */
ADXSJD ADXSJD_Create(SJ sji, Sint32 maxnch, SJ *sjo);
/* destroy the decoder's basic decoder, and free the decoder. a null sjd is
 * ignored */
void ADXSJD_Destroy(ADXSJD sjd);
/* make sji the joint the decoder reads */
void ADXSJD_SetInSj(ADXSJD sjd, SJ sji);
/* clear the decoder's counts, its trap and its limit, and have it read its
 * data's header first */
void ADXSJD_Start(ADXSJD sjd);
/* stop the decoder and its basic decoder */
void ADXSJD_Stop(ADXSJD sjd);
/* move each decoder in use on once. a decoder reading its header waits for
 * it whole in one chunk of sji. that chunk holds up to 0x1000 bytes. a step
 * then decodes the one chunk sji's data line gives, up to maxdecsmpl samples of
 * each channel. it waits while the first output joint has no room for a
 * block. the decoder is done once it has decoded the data's total. ADX data
 * is also done at its end mark, and AU data once sji holds nothing */
void ADXSJD_ExecServer(void);
/* have the decoder call f with obj for each channel's samples a step puts
 * out. ch is the channel, and data the dtlen bytes */
void ADXSJD_EntryFltFunc(ADXSJD sjd,
                         void (*f)(void *obj, Sint32 ch, void *data,
                                   Sint32 dtlen),
                         void *obj);
/* the data's sampling rate, its channels, and the output's bits a sample.
 * the basic decoder reads each from the header */
Sint32 ADXSJD_GetSfreq(ADXSJD sjd);
Sint32 ADXSJD_GetNumChan(ADXSJD sjd);
Sint32 ADXSJD_GetOutBps(ADXSJD sjd);
/* the data's samples a channel */
Sint32 ADXSJD_GetTotalNumSmpl(ADXSJD sjd);
/* the header's bytes, or 0 before the decoder has read it */
Sint32 ADXSJD_GetHdrLen(ADXSJD sjd);
/* the bits a sample in the data */
Sint32 ADXSJD_GetFmtBps(ADXSJD sjd);

/* ADXT uses these to read the format, schedule a sample-count trap, and
 * resume a loop. loop positions count samples, offsets count input bytes */
Sint32 ADXSJD_GetStat(ADXSJD sjd);
Sint32 ADXSJD_GetFormat(ADXSJD sjd);
Sint32 ADXSJD_GetBlkSmpl(ADXSJD sjd);
Sint32 ADXSJD_GetDecNumSmpl(ADXSJD sjd);
Sint32 ADXSJD_GetNumLoop(ADXSJD sjd);
Sint32 ADXSJD_GetLpStartPos(ADXSJD sjd);
Sint32 ADXSJD_GetLpStartOfst(ADXSJD sjd);
Sint32 ADXSJD_GetLpEndPos(ADXSJD sjd);
Sint32 ADXSJD_GetLpEndOfst(ADXSJD sjd);
char *ADXSJD_GetSpsdInfo(ADXSJD sjd);
void ADXSJD_SetMaxDecSmpl(ADXSJD sjd, Sint32 nsmpl);
void ADXSJD_SetDecPos(ADXSJD sjd, Sint32 pos);
void ADXSJD_EntryTrapFunc(ADXSJD sjd, void (*func)(void *obj), void *obj);
void ADXSJD_SetTrapNumSmpl(ADXSJD sjd, Sint32 nsmpl);
void ADXSJD_SetTrapCnt(ADXSJD sjd, Sint32 cnt);
void ADXSJD_SetTrapDtLen(ADXSJD sjd, Sint32 len);
void ADXSJD_ExecHndl(ADXSJD sjd);

#endif
