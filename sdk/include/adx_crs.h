/* CRI ADX's critical sections */
#ifndef ADX_CRS_H
#define ADX_CRS_H

/* treat every ADXCRS_Lock as closed, so that the next ADXCRS_Lock is the
 * outermost. the interrupt mask stays as it is */
void ADXCRS_Init(void);
/* ADXCRS_Lock and ADXCRS_Unlock nest in pairs. the outermost ADXCRS_Lock
 * masks every interrupt, and its ADXCRS_Unlock restores the mask it found.
 * each ADXCRS_Unlock must close an ADXCRS_Lock. after a spare
 * ADXCRS_Unlock, which closes none, the next ADXCRS_Lock masks no
 * interrupt */
void ADXCRS_Lock(void);
void ADXCRS_Unlock(void);

#endif
