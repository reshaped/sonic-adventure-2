/* CRI ADX's decoders that predict in floats */
#ifndef ADX_DCD3_H
#define ADX_DCD3_H
#include <sg_xpt.h>

/* decode nblk blocks of ADX data at ibuf into obuf, 32 samples to a block.
 * each sample is predicted from the two before it. k0 and k1 are the
 * coefficients, as ADX_GetCoefficient makes them. a sample past a Sint16's
 * range is clamped to that range.
 *
 * dly[0] holds the sample just before the first block, and dly[1] the one
 * before that. the call leaves its last sample in dly[0], and the one
 * before in dly[1].
 *
 * the call returns nblk. a block whose header has its top bit set ends the
 * data. the decode stops at that block, and returns the count of blocks
 * before it. dly then keeps what it held before the call */
Sint32 ADX_DecodeMonoFloat(Sint8 *ibuf, Sint32 nblk, Sint16 *obuf, Sint16 *dly,
                           Sint16 k0, Sint16 k1);
/* decode as ADX_DecodeMonoFloat does, in nblk / 2 pairs of blocks. a pair's
 * first block goes to obufl, and its second to obufr. dlyl and dlyr each
 * hold their side's two samples, as dly does. an odd last block is left
 * alone. the call returns as ADX_DecodeMonoFloat does */
Sint32 ADX_DecodeSteFloat(Sint8 *ibuf, Sint32 nblk, Sint16 *obufl, Sint16 *dlyl,
                          Sint16 *obufr, Sint16 *dlyr, Sint16 k0, Sint16 k1);

#endif
