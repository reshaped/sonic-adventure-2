/* Both platform texture lists use this bank of filenames. */
#ifndef STG32_PLATFORM_NAMES_H
#define STG32_PLATFORM_NAMES_H
#include <ninja.h>

struct platform_filenames {
    char meteo128_10[12];
    char meteo256_1[12];
    char meteo64_3[12];
    char miu128_fr002[16];
    char miu128_meh004[16];
    char miu128_meh008[16];
    char miu256_meh001[14];
};

extern const struct platform_filenames meteor_platform_filenames;

static NJS_TEXNAME textures[7] = {
#include "stg32/platform_names/textures.inc"
};

#endif
