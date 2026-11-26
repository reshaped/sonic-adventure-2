/* the ADX file system's calls on the SH-4's operand cache */
#ifndef ADX_FCCH_H
#define ADX_FCCH_H
#include <sg_xpt.h>

#define ADXF_OCBI_WALK_MIN 0x30000
/* drop the operand cache's lines over the bsize bytes at buf. the cache
 * discards what it holds of those bytes, and writes none of it to memory. a
 * bsize of 0 does nothing. from a bsize of ADXF_OCBI_WALK_MIN on, the call
 * walks the cache's entries instead. it masks every interrupt while it walks.
 *
 * the OIX bit of CCR, the cache control register, selects index mode. in
 * index mode, the walk drops only part of the range. it keeps each line
 * whose address has bit 25 set, but the range's last. it drops that last
 * line only where its address has bit 13 set too */
void ADXF_Ocbi(void *buf, Sint32 bsize);

#endif
