/* CRI ADX's sound output (ADXRNA), over mw_rna, the middleware's sound
 * output. each call passes its arguments on to the mwRna call of its name */
#ifndef ADX_RNAS_H
#define ADX_RNAS_H
#include <cri_adxt.h>
#include <sg_xpt.h>
#include <sj.h>

void ADXRNA_Init(void);
void ADXRNA_Finish(void);
/* make a sound output that plays the maxnch joints at sjo, one for each
 * channel. it returns null where mwRnaCreate does. ADXRNA_Destroy stops the
 * output and frees it */
ADXRNA ADXRNA_Create(SJ *sjo, Sint32 maxnch);
void ADXRNA_Destroy(ADXRNA rna);
/* ADXRNA_SetTransSw turns the output's transfer of samples on with sw TRUE,
 * and off with FALSE. ADXRNA_SetPlaySw turns its play on and off alike */
void ADXRNA_SetTransSw(ADXRNA rna, Sint32 sw);
void ADXRNA_SetPlaySw(ADXRNA rna, Sint32 sw);
/* the time played: *ncount in units of 1 / *tscale seconds */
void ADXRNA_GetTime(ADXRNA rna, Sint32 *ncount, Sint32 *tscale);
/* move each sound output on once */
void ADXRNA_ExecServer(void);
/* set the output's volume, and channel ch's pan, as cri_adxt.h's
 * ADXT_SetOutVol and ADXT_SetOutPan say */
void ADXRNA_SetOutVol(ADXRNA rna, Sint32 vol);
void ADXRNA_SetOutPan(ADXRNA rna, Sint32 ch, Sint32 pan);
/* drop up to nsmpl samples of the output's joints. it returns the samples
 * dropped */
Sint32 ADXRNA_DiscardData(ADXRNA rna, Sint32 nsmpl);

/* samples buffered per channel. the sound output reports unlimited room.
 * the format calls prepare the sound port. total sample count is ignored */
Sint32 ADXRNA_GetNumData(ADXRNA rna);
Sint32 ADXRNA_GetNumRoom(ADXRNA rna);
void ADXRNA_SetNumChan(ADXRNA rna, Sint32 nch);
void ADXRNA_SetSfreq(ADXRNA rna, Sint32 sfreq);
void ADXRNA_SetBitPerSmpl(ADXRNA rna, Sint32 bps);
void ADXRNA_SetTotalNumSmpl(ADXRNA rna, Sint32 nsmpl);
/* pass hdinfo to mwRnaSetStmHdInfo, and return the count that call stores.
 * rna must not be null. mwRnaSetStmHdInfo then stores nothing, and the
 * result is a stack word as it was */
Sint32 ADXRNA_SetStmHdInfo(ADXRNA rna, char *hdinfo);

#endif
