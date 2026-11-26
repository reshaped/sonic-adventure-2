/* Sofdec's test of an MPEG video sequence header */
#ifndef SFD_MPVD_H
#define SFD_MPVD_H
#include <sg_xpt.h>
/* whether the size bytes at data start an MPEG video sequence header. it
 * returns FALSE where data doesn't start with a sequence start code. it also
 * returns FALSE where a field within size is 0. the fields are the aspect
 * ratio, the frame rate, the bit rate and the marker bit. a header cut short
 * passes on the fields it holds. the four bytes at data must be readable,
 * whatever size */
Bool SFMPVD_IsShc(Uint8 *data, Sint32 size);
#endif
