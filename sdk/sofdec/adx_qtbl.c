/* the sample difference of each four-bit ADX code, in ADX_SAMPLE_UNITs */
#include <adx_qtbl.h>

#define SAMPLE(level) ((level) * ADX_SAMPLE_UNIT)

#define REPEAT_16(value)                                                       \
    value, value, value, value, value, value, value, value, value, value,      \
        value, value, value, value, value, value

#define SIGNED_SAMPLES                                                         \
    SAMPLE(0), SAMPLE(1), SAMPLE(2), SAMPLE(3), SAMPLE(4), SAMPLE(5),          \
        SAMPLE(6), SAMPLE(7), SAMPLE(-8), SAMPLE(-7), SAMPLE(-6), SAMPLE(-5),  \
        SAMPLE(-4), SAMPLE(-3), SAMPLE(-2), SAMPLE(-1)

#define HIGH_NIBBLE_RUN(level) REPEAT_16(SAMPLE(level))

/* by code: 0 to 7, then -8 to -1 for codes 0x8 to 0xf */
const float AdxQtblFloat[16] = {SIGNED_SAMPLES};

/* by a byte of two codes: its high nibble's difference */
const float AdxQtblFloat0[256] = {
    HIGH_NIBBLE_RUN(0),  HIGH_NIBBLE_RUN(1),  HIGH_NIBBLE_RUN(2),
    HIGH_NIBBLE_RUN(3),  HIGH_NIBBLE_RUN(4),  HIGH_NIBBLE_RUN(5),
    HIGH_NIBBLE_RUN(6),  HIGH_NIBBLE_RUN(7),  HIGH_NIBBLE_RUN(-8),
    HIGH_NIBBLE_RUN(-7), HIGH_NIBBLE_RUN(-6), HIGH_NIBBLE_RUN(-5),
    HIGH_NIBBLE_RUN(-4), HIGH_NIBBLE_RUN(-3), HIGH_NIBBLE_RUN(-2),
    HIGH_NIBBLE_RUN(-1),
};

/* by a byte of two codes: its low nibble's difference */
const float AdxQtblFloat1[256] = {REPEAT_16(SIGNED_SAMPLES)};
