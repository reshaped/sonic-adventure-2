/* the pulse that tints Death Chamber's and Pyramid Cave's lights and scenery,
 * and Egg Quarters' arrows and bands */
#ifndef SHARED_TINT_PULSE_H
#define SHARED_TINT_PULSE_H
#include <sg_xpt.h>

#include "draw/color.h"

/* the pulse repeats every TINT_PULSE_STEPS of game_state.steps. its alpha
 * rises from 0 to 255 over the first TINT_PULSE_RISE steps, by
 * TINT_PULSE_RISE_RATE a step. it falls back to 0 over the rest, by
 * TINT_PULSE_FALL_RATE a step. the tint's red, green and blue stay
 * TINT_PULSE_COLORS */
#define TINT_PULSE_STEPS 60
#define TINT_PULSE_RISE 5
#define TINT_PULSE_RISE_RATE 63.75F
#define TINT_PULSE_FALL_RATE 4.7222223F
#define TINT_PULSE_COLORS 0xffffff
/* the pulse's alpha at step, shifted into a color's alpha byte.
 * TINT_PULSE_RISE_ALPHA takes a step below TINT_PULSE_RISE, and
 * TINT_PULSE_FALL_ALPHA one from there to TINT_PULSE_STEPS - 1 */
/* @hack type: each alpha converted through int, where a conversion to Uint32
 * would do */
#define TINT_PULSE_RISE_ALPHA(step)                                            \
    ((Uint32)(int)((step) * TINT_PULSE_RISE_RATE) << ALPHA_SHIFT)
#define TINT_PULSE_FALL_ALPHA(step)                                            \
    ((Uint32)(int)((TINT_PULSE_STEPS - 1 - (step)) * TINT_PULSE_FALL_RATE)     \
     << ALPHA_SHIFT)

#endif
