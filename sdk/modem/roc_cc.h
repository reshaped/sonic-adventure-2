/* roc_cc: the data pump's tone detectors, as far as the sources use them */
#ifndef MODEM_ROC_CC_H
#define MODEM_ROC_CC_H
#include <sg_xpt.h>

/* dp_set_handshake_tone_detector's filters for detector A: one for the dial
 * tone, and one for call progress, the busy and ringback tones */
#define DIAL_TONE_FILTER 0
#define CALL_PROGRESS_FILTER 1
/* program detector A with profile's dial tone filter where handshake is
 * DIAL_TONE_FILTER, and with its call progress filter otherwise */
void dp_set_handshake_tone_detector(Uint16 profile, Uint8 handshake);

/* return 1 while the pump's tone detector A hears its tone, and 0 while it
 * doesn't.
 *
 * the call may keep a cadence: the times of a tone and of the gap after it.
 * with a cadence kept, it times each tone and gap that detector A hears. the
 * pump's configuration is roc_k56.h's conf. while that is also CONF_TONE, the
 * call ignores detector A. it then returns 1 for the tone's time and 0 for
 * the gap's, in turn */
Uint8 dp_filter_test(void);

#endif
