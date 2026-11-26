#include <kamui.h>
#include <math.h>
#include <ninja.h>
#include <sg_xpt.h>

/* a near depth -n past NEAR_LIMIT makes the density FOG_DENSITY_65536 */
#define NEAR_LIMIT 65535.0F
/* fraction times MANTISSA_SCALE puts fraction's first eight bits where
 * FOG_DENSITY_MANTISSA masks them */
#define MANTISSA_SCALE 65536.0F
/* ENTRY_DIVISOR(i) is the s of ninja.h's njGenerateFogTable3 for entry i.
 * the entry stands for the depth far / s. s is (STEPS + (i & STEP_MASK)) /
 * STEPS, times 2 to the power i >> POWER_SHIFT */
#define POWER_SHIFT 4
#define STEPS (1 << POWER_SHIFT)
#define STEP_MASK (STEPS - 1)
#define ENTRY_DIVISOR(i)                                                       \
    (((Float)(1 << ((i) >> POWER_SHIFT)) *                                     \
      ((Float)((i) & STEP_MASK) + STEPS)) /                                    \
     STEPS)

void njGenerateFogTable3(NJS_FOG_TABLE fog, Float n, Float f)
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
    /* @hack type: power is volatile, where a plain Sint8 would do */
    volatile Sint8 power;
    Float scale;
    Float span;
    Float *p;
    Int i;
    Int deeper = 0;

    if (n < -NEAR_LIMIT) {
        density = FOG_DENSITY_65536;
    } else {
        /* the fog density word for -f, as njGenerateFogDensity makes it
         * where -f is 1 or more */
        fraction = frexp(-f, &exponent);
        mantissa = (Sint32)(fraction * MANTISSA_SCALE) & FOG_DENSITY_MANTISSA;
        density |= mantissa;
        /* @hack spelling: exponent lowered in place, where power = exponent
         * - 1 would do */
        exponent--;
        power = exponent;
        /* @bug arithmetic: a far depth -f below 1 sets every bit of density
         * above its low byte, the mantissa's among them. power is negative
         * there, and the OR takes it with its sign (MOV.B at 0x8c11c500, OR at
         * 0x8c11c50a) */
        density |= power;
    }
    kmSetFogDensity(density);
    /* @hack spelling: p, set here and stepped through the table beside i,
     * where fog[i] in each loop would do */
    p = fog;
    /* a ramp needs -n deeper than -f / 256. that is the depth an entry
     * after the last would stand for */
    if (f / ENTRY_DIVISOR(FOG_TABLE_SIZE) > n) {
        /* each entry takes its depth, which the ramp below starts from */
        scale = -f;
    } else {
        /* no ramp: each entry takes its depth over -f */
        scale = 1.0F;
    }
    /* deeper counts the entries whose depths are deeper than -n. they are
     * the table's first, since the depths fall as i rises */
    for (i = 0; i < FOG_TABLE_SIZE; i++, p++) {
        *p = scale / ENTRY_DIVISOR(i);
        if (*p > -n) {
            deeper++;
        }
    }
    if (scale != -f) {
        return;
    }
    /* @hack spelling: i zeroed ahead of p and span, where for (i = 0; i <
     * deeper; i++, p++) would do */
    i = 0;
    p = fog;
    /* the ramp is 0 at the depth in fog[deeper], the deepest entry not
     * deeper than -n. it is 1 at the depth -f. span is the first depth less
     * the second */
    /* @bug memory: reads fog[FOG_TABLE_SIZE], the float after the table, as
     * the depth where the ramp is 0. it does so where every entry is deeper
     * than -n, as a -n below -f / 248 makes them (FMOV.S at 0x8c11c574) */
    /* @hack spelling: f added to span in a statement of its own, where span
     * = fog[deeper] + f would do */
    span = fog[deeper];
    span += f;
    for (; i < deeper; i++, p++) {
        *p = 1.0F - ((*p + f) / span);
    }
    for (i = deeper; i < FOG_TABLE_SIZE; i++, p++) {
        *p = 0.0F;
    }
}
