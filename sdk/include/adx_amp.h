/* CRI ADX's amplitude monitor (ADXAMP) */
#ifndef ADX_AMP_H
#define ADX_AMP_H
#include <cri_adxt.h>
#include <sg_xpt.h>

/* free the monitor. a null amp is ignored */
void ADXAMP_Destroy(ADXAMP amp);

/* the sample rate used to count each amplitude frame */
void ADXAMP_SetSfreq(ADXAMP amp, Sint32 sfreq);

#endif
