#include <math.h>
#include <ninja.h>
#include <sg_xpt.h>

/* past the depth FAR_LIMIT, the density is FOG_DENSITY_65536 */
#define FAR_LIMIT 65536.0F
/* fraction times MANTISSA_SCALE puts fraction's first eight bits where
 * FOG_DENSITY_MANTISSA masks them */
#define MANTISSA_SCALE 65536.0F

Uint32 njGenerateFogDensity(Float f)
{
    /* @hack spelling: density, zeroed here and built up by |=, where one
     * assignment on each path would do */
    Uint32 density = 0;
    Int exponent;
    /* @hack spelling: fraction, a local for frexp's result, where the call
     * multiplied in place would do */
    double fraction;
    /* @hack type: mantissa, a Uint16 for bits the mask already holds to 16,
     * where the masked bits or-ed into density in place would do */
    Uint16 mantissa;
    Uint8 power;
    Uint8 *p;

    if (f < -FAR_LIMIT) {
        density = FOG_DENSITY_65536;
    } else {
        /* frexp gives -f as fraction times 2 to the power exponent. for f
         * below 0, fraction lies from 1/2 up to 1. the fog density word's
         * mantissa, which counts 128ths, takes fraction's first eight bits,
         * which count 256ths. so the mantissa stands for 2 * fraction, and
         * the word's power is exponent - 1 */
        fraction = frexp(-f, &exponent);
        mantissa = (Sint32)(fraction * MANTISSA_SCALE) & FOG_DENSITY_MANTISSA;
        density |= mantissa;
        /* @hack spelling: exponent lowered in place, where power = exponent
         * - 1 would do */
        exponent--;
        power = exponent;
        /* @hack spelling: p, a pointer that power is read back through,
         * where density |= power would do */
        p = &power;
        density |= *p;
    }
    return density;
}
