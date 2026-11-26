/* roc_wa: the monitors of the data pump's trainings and rate renegotiations,
 * as far as the sources use them */
#ifndef MODEM_ROC_WA_H
#define MODEM_ROC_WA_H
#include <sg_xpt.h>

/* reset the state the monitors below keep from pass to pass. that state
 * includes K56_training_cnt and bits of rcflags and k56flags */
void InitWaVariables(void);
/* follow one pass of the V.8 exchange */
void MonitorV8(void);
/* return the far end's V.90 capabilities. RAM 0x6c1 gives bits 0 to 7, 0x6c2
 * bits 8 to 15 and 0x6c3 bits 16 to 23 */
Uint32 V90GetRemoteCaps(void);
/* follow one pass of a training under V.90, K56flex, V.34 or V.32. of the
 * four, only MonitorV32Train sets handshake_status */
void MonitorV90Train(void);
void MonitorK56Train(void);
void MonitorV34Train(void);
void MonitorV32Train(void);
/* follow one pass of a rate renegotiation under V.34 or V.90. where the
 * renegotiation turns into a retrain, the call runs InitWaVariables. it then
 * sets rcflags' retraining and renegotiation_retrained, and clears
 * renegotiating. where renegotiation_started is set, it sets retrain_started
 * too */
void MonitorV34RR(void);
void MonitorV90RR(void);
/* do nothing */
void WaV34Disc(void);
/* count each change of the pump's abcode */
void MonitorABcode(void);

#endif
