/* the MPEG layer II bit allocation table AHX streams use */
#ifndef MPEG_ALLOC_H
#define MPEG_ALLOC_H
#include <sg_xpt.h>

#define SBLIMIT 32
#define ALLOC_CODES 16

/* a quantizer of steps levels, evenly spaced on [-1, 1]. a sample coded s
 * decodes to c * s + d. a subband's three samples take group codes of bits
 * bits. a group of 3 codes each sample alone, and a group of 1 all three
 * together. quant indexes the decoder's dequantizer tables */
struct mpeg_quantizer {
    Sint32 steps;
    Uint8 bits;
    Uint8 group;
    Uint8 quant;
    float c;
    float d;
};
/* a table's row for each subband, indexed by allocation code. a row's entry
 * 0 codes no samples, and its bits is the width of the subband's allocation
 * code */
typedef struct mpeg_quantizer MPEG_ALLOC_TABLE[SBLIMIT][ALLOC_CODES];

/* point *alloc at table table and return the subbands it codes. table 0,
 * of 30 subbands, is the only one, so table must be 0 */
Sint32 mpeg_read_bit_alloc(Sint32 table, MPEG_ALLOC_TABLE **alloc);

#endif
