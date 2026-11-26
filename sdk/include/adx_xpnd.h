/* CRI ADX's block expander, a pool of eight handles. a handle decodes the
 * run of blocks its entry gives it in one ADXPD_ExecHndl. a stereo entry's
 * blocks alternate left and right, and its nblk counts both channels' */
#ifndef ADX_XPND_H
#define ADX_XPND_H
#include <sg_xpt.h>

typedef struct _adxpd_obj *ADXPD;
/* a handle's states. ADXPD_ExecHndl takes a started handle through
 * decoding to done in one call, and ADXPD_Reset stops a done one */
#define ADXPD_STAT_STOP 0
#define ADXPD_STAT_START 1
#define ADXPD_STAT_EXEC 2
#define ADXPD_STAT_END 3

/* free every handle */
void ADXPD_Init(void);
/* a free handle, stopped, or null when every handle is in use. its
 * coefficients are for a cutoff of ADXPD_DEF_COF hertz at ADXPD_DEF_SFREQ
 * samples a second */
ADXPD ADXPD_Create(void);
#define ADXPD_DEF_COF 500
#define ADXPD_DEF_SFREQ 44100
/* make the coefficients for a cutoff of cof hertz at sfreq samples a
 * second */
void ADXPD_SetCoef(ADXPD xpd, Sint32 sfreq, Sint32 cof);
/* free the handle. a null xpd is ignored */
void ADXPD_Destroy(ADXPD xpd);
Sint32 ADXPD_GetStat(ADXPD xpd);
/* ADXPD_EntryMono sets the next decode to nblk blocks of mono at ibuf, into
 * obuf. ADXPD_EntrySte sets it to nblk blocks of stereo, into obufl and
 * obufr. each returns 1. on a handle that isn't ADXPD_STAT_STOP, each sets
 * nothing and returns 0 */
Sint32 ADXPD_EntryMono(ADXPD xpd, Sint8 *ibuf, Sint32 nblk, Sint16 *obuf);
Sint32 ADXPD_EntrySte(ADXPD xpd, Sint8 *ibuf, Sint32 nblk, Sint16 *obufl,
                      Sint16 *obufr);
/* have a stopped handle decode at its next ADXPD_ExecHndl, and clear the
 * count ADXPD_GetNumBlk gives */
void ADXPD_Start(ADXPD xpd);
/* stop the handle, and clear the samples it predicts from */
void ADXPD_Stop(ADXPD xpd);
/* stop a handle that has decoded its blocks, so that it takes another entry.
 * the samples it predicts from stay */
void ADXPD_Reset(ADXPD xpd);
/* the blocks the last decode gave */
Sint32 ADXPD_GetNumBlk(ADXPD xpd);
/* decode a started handle's entry, as adx_dcd3.h's decoders do. the handle
 * becomes ADXPD_STAT_END, with the decoder's result in ADXPD_GetNumBlk. an
 * odd result of a stereo decode sets cri_adxt.h's adxpd_internal_error */
void ADXPD_ExecHndl(ADXPD xpd);

#endif
