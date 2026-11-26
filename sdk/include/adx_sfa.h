/* SFA, Sofdec's ADX audio, which carries no ADX header of its own */
#ifndef ADX_SFA_H
#define ADX_SFA_H
#include <cri_adxt.h>
#include <sg_xpt.h>

void SFA_Init(void);
void SFA_Finish(void);
/* encode an ADX header for channels channels of rate hertz and samples samples.
 * put it on player's input joint, ahead of the SFA data. it hangs where the
 * encoder writes nothing or the joint lacks room for the header */
void ADXT_InsertHdrSfa(ADXT player, Sint32 channels, Sint32 rate,
                       Sint32 samples);

#endif
