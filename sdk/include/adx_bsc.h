/* CRI's basic audio decoder, a pool of eight. a decoder decodes the formats
 * ADXB_FORMAT_ names into a ring of samples for each channel */
#ifndef ADX_BSC_H
#define ADX_BSC_H
#include <sg_xpt.h>

typedef struct adxb_obj *ADXB;
#define ADXB_STAT_STOP 0
#define ADXB_STAT_PREP 1
#define ADXB_STAT_EXEC 2
#define ADXB_STAT_END 3
#define ADXB_FORMAT_ADX 0
#define ADXB_FORMAT_WAV 1
#define ADXB_FORMAT_SPSD 2
#define ADXB_FORMAT_AIFF 3
#define ADXB_FORMAT_AU 4
/* the function a decoder asks for room before each step. it returns channel 0's
 * ring and sets *wpos to the sample a step writes first. *nroom gets the
 * samples of room from there. *nleft gets the samples the decoder may write
 * before it must stop */
typedef Sint16 *(*ADXB_GETWR)(void *obj, Sint32 *wpos, Sint32 *nroom,
                              Sint32 *nleft);
void ADXB_Init(void);
/* a decoder of up to maxnch channels. it is NULL where all eight decoders are
 * in use, or all eight block expanders. channel 0's ring is pcmbsize samples at
 * pcmbuf, and channel 1's starts pcmbdist samples after it */
ADXB ADXB_Create(Sint32 maxnch, Sint16 *pcmbuf, Sint32 pcmbsize,
                 Sint32 pcmbdist);
void ADXB_Destroy(ADXB adxb);
/* read the header at header, size bytes long, as whichever of the five
 * formats its signature names. it returns the header's length, where the
 * data starts, or 0 for a header it can't read */
Sint32 ADXB_DecodeHeader(ADXB adxb, Sint8 *header, Sint32 size);
void ADXB_EntryGetWrFunc(ADXB adxb, ADXB_GETWR func, void *obj);
Sint16 *ADXB_GetPcmBuf(ADXB adxb);
Sint32 ADXB_GetFormat(ADXB adxb);
Sint32 ADXB_GetSfreq(ADXB adxb);
Sint32 ADXB_GetNumChan(ADXB adxb);
Sint32 ADXB_GetFmtBps(ADXB adxb);
Sint32 ADXB_GetOutBps(ADXB adxb);
Sint32 ADXB_GetBlkSmpl(ADXB adxb);
Sint32 ADXB_GetBlkLen(ADXB adxb);
Sint32 ADXB_GetTotalNumSmpl(ADXB adxb);
Sint32 ADXB_GetCof(ADXB adxb);
Sint32 ADXB_GetLpInsNsmpl(ADXB adxb);
Sint32 ADXB_GetNumLoop(ADXB adxb);
Sint32 ADXB_GetLpStartPos(ADXB adxb);
Sint32 ADXB_GetLpStartOfst(ADXB adxb);
Sint32 ADXB_GetLpEndPos(ADXB adxb);
Sint32 ADXB_GetLpEndOfst(ADXB adxb);
Sint32 ADXB_GetStat(ADXB adxb);
/* give the next step its input, ibuflen bytes at ibuf, and clear the counts
 * ADXB_GetDecDtLen and ADXB_GetDecNumSmpl give */
void ADXB_EntryData(ADXB adxb, Sint8 *ibuf, Sint32 ibuflen);
void ADXB_Start(ADXB adxb);
void ADXB_Stop(ADXB adxb);
void ADXB_Reset(ADXB adxb);
Sint32 ADXB_GetDecDtLen(ADXB adxb);
Sint32 ADXB_GetDecNumSmpl(ADXB adxb);
void ADXB_ExecHndl(ADXB adxb);

#endif
