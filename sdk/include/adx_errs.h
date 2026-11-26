/* CRI ADX's error reports */
#ifndef ADX_ERRS_H
#define ADX_ERRS_H
#include <sg_xpt.h>

/* ADXERR_Init and ADXERR_Finish each clear the error text, and set no
 * function to take errors */
void ADXERR_Init(void);
void ADXERR_Finish(void);
/* have func take each error, with obj, from now on. a null func takes none */
void ADXERR_EntryErrFunc(void (*func)(void *obj, char *msg), void *obj);
/* copy msg, cut to 255 bytes, into the library's error text. then pass the
 * text to the function ADXERR_EntryErrFunc was given, if any.
 * ADXERR_CallErrFunc2 copies msg2 after msg1, and the two together must fit
 * in 255 bytes */
void ADXERR_CallErrFunc1(char *msg);
void ADXERR_CallErrFunc2(char *msg1, char *msg2);
/* meant to write num1 and num2 to str in decimal, with a space between, in
 * size bytes. by a bug, each number's text comes out empty, so str gets the
 * string " ". size must be 2 or more. str must hold 11 bytes, whatever size
 * is. each number's digits reach str before its text is emptied */
void ADXERR_ItoA2(Sint32 num1, Sint32 num2, char *str, Sint32 size);

#endif
