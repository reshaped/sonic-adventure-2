/* layer II's bit allocation table 0, the one AHX streams use */
#include <mpeg_alloc.h>
#include <sg_xpt.h>

/* c and d put code s at the middle of part s of [-1, 1]. the levels parts
 * are equal, and count from 0 */
#define QUANTIZER(levels, bits, group, quant)                                  \
    {levels, bits, group, quant, 2.0F / (levels), (1.0F / (levels)) - 1.0F}
#define NO_SAMPLES(code_bits) {0, code_bits, 0, 0, 0, 0}
#define GROUP_3 QUANTIZER(3, 5, 1, 0)
#define GROUP_5 QUANTIZER(5, 7, 1, 1)
#define GROUP_9 QUANTIZER(9, 10, 1, 3)
#define DIRECT(bits) QUANTIZER((1 << (bits)) - 1, bits, 3, bits)

/* clang-format off */
#define ALLOC_4                                                                \
    {NO_SAMPLES(4), GROUP_3, GROUP_5, QUANTIZER(7, 3, 3, 2), GROUP_9,          \
     DIRECT(4), DIRECT(5), DIRECT(6), DIRECT(7), DIRECT(8), DIRECT(9),         \
     DIRECT(10), DIRECT(11), DIRECT(12), DIRECT(13), DIRECT(14)}
#define ALLOC_3                                                                \
    {NO_SAMPLES(3), GROUP_3, GROUP_5, GROUP_9, DIRECT(4), DIRECT(5),           \
     DIRECT(6), DIRECT(7)}
#define ALLOC_2 {NO_SAMPLES(2), GROUP_3, GROUP_5, GROUP_9}

static Sint32 subband_limits[] = {30};
static MPEG_ALLOC_TABLE tables[] = {{
    ALLOC_4, ALLOC_4, ALLOC_4, ALLOC_4,
    ALLOC_3, ALLOC_3, ALLOC_3, ALLOC_3, ALLOC_3, ALLOC_3, ALLOC_3,
    ALLOC_2, ALLOC_2, ALLOC_2, ALLOC_2, ALLOC_2, ALLOC_2, ALLOC_2,
    ALLOC_2, ALLOC_2, ALLOC_2, ALLOC_2, ALLOC_2, ALLOC_2, ALLOC_2,
    ALLOC_2, ALLOC_2, ALLOC_2, ALLOC_2, ALLOC_2,
}};
/* clang-format on */

Sint32 mpeg_read_bit_alloc(Sint32 table, MPEG_ALLOC_TABLE **alloc)
{
    *alloc = &tables[table];
    return subband_limits[table];
}
