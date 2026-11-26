/* CRI ADX's four-bit codes as sample differences, for the floating-point
 * decoder */
#ifndef ADX_QTBL_H
#define ADX_QTBL_H

/* the floating-point decoder predicts with each sample shifted up by
 * ADX_FRAC_BITS. ADX_SAMPLE_UNIT is then a sample of 1 */
#define ADX_FRAC_BITS 16
#define ADX_SAMPLE_UNIT ((float)(1 << ADX_FRAC_BITS))

/* a four-bit code's difference in ADX_SAMPLE_UNITs, codes 8 to 15 being -8 to
 * -1. AdxQtblFloat0 gives the difference of a code byte's high four bits, and
 * AdxQtblFloat1 that of its low four. both take the whole byte as their index
 */
extern const float AdxQtblFloat[16];
extern const float AdxQtblFloat0[256];
extern const float AdxQtblFloat1[256];

#endif
