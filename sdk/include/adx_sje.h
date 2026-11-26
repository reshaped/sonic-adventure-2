/* CRI's ADX encoder, a pool of eight. each input joint gives one channel of
 * 16-bit PCM. the output joint takes the ADX header, the blocks and the end
 * code */
#ifndef ADX_SJE_H
#define ADX_SJE_H
#include <sg_xpt.h>
#include <sj.h>

typedef struct adxsje_object *ADXSJE;

#define ADXSJE_STAT_STOP 0
#define ADXSJE_STAT_PREP 1
#define ADXSJE_STAT_EXEC 2
#define ADXSJE_STAT_END 3

extern char *adxenc_build;

Sint32 ADXSJE_GetVersion(void);
const char *ADXSJE_GetVerStr(void);
Sint32 ADXSJE_GetInfo(Sint8 *header, Sint32 header_bytes, Sint32 *bits,
                      Sint32 *channels, Sint32 *sample_rate, Sint32 *samples);

/* ADXSJE_Init and ADXSJE_Finish each clear the pool of eight encoders */
void ADXSJE_Init(void);
void ADXSJE_Finish(void);

/* an encoder that reads inputs[0] to inputs[max_channels - 1] and writes
 * output. it is NULL where all eight are in use. max_channels must be 1 or 2.
 * the header's fields start at defaults the setters below change, 44100 Hz
 * among them */
ADXSJE ADXSJE_Create(Sint32 max_channels, SJ *inputs, SJ output);
void ADXSJE_Destroy(ADXSJE encoder);
Sint32 ADXSJE_GetStat(ADXSJE encoder);

/* ADXSJE_Start clears the counts, and the next step writes the header.
 * ADXSJE_Stop has the next step write the end code in place of more blocks */
void ADXSJE_Start(ADXSJE encoder);
void ADXSJE_Stop(ADXSJE encoder);

/* the header's fields, which the first step after ADXSJE_Start writes.
 * ADXSJE_SetTotalNumSmpl also sets the sample at which encoding ends */
void ADXSJE_SetSfreq(ADXSJE encoder, Sint32 sample_rate);
void ADXSJE_SetHdInfoSize(ADXSJE encoder, Sint32 size);
void ADXSJE_SetNumChan(ADXSJE encoder, Sint32 channels);
void ADXSJE_SetOutBps(ADXSJE encoder, Sint32 bps);
void ADXSJE_SetBlkSmpl(ADXSJE encoder, Sint32 samples);
void ADXSJE_SetBlkLen(ADXSJE encoder, Sint32 len);
void ADXSJE_SetTotalNumSmpl(ADXSJE encoder, Sint32 samples);
void ADXSJE_SetCof(ADXSJE encoder, Sint32 cutoff);
void ADXSJE_SetNumLoop(ADXSJE encoder, Sint32 loop_count);
void ADXSJE_SetLpInsNsmpl(ADXSJE encoder, Sint32 samples);
void ADXSJE_SetLpStartPos(ADXSJE encoder, Sint32 pos);
void ADXSJE_SetLpStartOfst(ADXSJE encoder, Sint32 ofst);
void ADXSJE_SetLpEndPos(ADXSJE encoder, Sint32 pos);
void ADXSJE_SetLpEndOfst(ADXSJE encoder, Sint32 ofst);
void ADXSJE_SetConfigSfa(ADXSJE encoder, Sint32 channels, Sint32 sample_rate,
                         Sint32 samples);

/* ADXSJE_GetEncDtLen gives the bytes written since ADXSJE_Start.
 * ADXSJE_GetEncNumSmpl and ADXSJE_GetEncPos give the samples encoded, which
 * ADXSJE_SetEncPos sets */
Sint32 ADXSJE_GetEncDtLen(ADXSJE encoder);
Sint32 ADXSJE_GetEncNumSmpl(ADXSJE encoder);
void ADXSJE_SetEncPos(ADXSJE encoder, Sint32 pos);
Sint32 ADXSJE_GetEncPos(ADXSJE encoder);

/* replace channel's input joint, or the output joint. ADXSJE_SetInSj
 * ignores a channel other than 0 or 1 */
void ADXSJE_SetInSj(ADXSJE encoder, Sint32 channel, SJ sj);
void ADXSJE_SetOutSj(ADXSJE encoder, SJ sj);

/* run a step of encoder. the first step after ADXSJE_Start writes the
 * header. each later step writes as many blocks as the inputs' data and the
 * output's room allow. it writes the end code once the samples run out or
 * ADXSJE_Stop asks. ADXSJE_ExecServer runs a step of every encoder */
void ADXSJE_ExecHndl(ADXSJE encoder);
void ADXSJE_ExecServer(void);

#endif
