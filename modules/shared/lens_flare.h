/* the lens flare, EFLENSF0 in a stage's object list: a glow at a light. its
 * ghosts draw on the line from the light through the screen's center */
#ifndef SHARED_LENS_FLARE_H
#define SHARED_LENS_FLARE_H
#include <ninja.h>

struct task;

/* in the ang.x of a flare's work, the bit that makes the flare a rival */
#define LENS_FLARE_RIVAL 0x1

/* the flares' texture list, which names lens01 to lens07, then panlight */
extern NJS_TEXLIST lens_flare_texlist;

/* the object list's step for EFLENSF0. it gives task the flare's own step,
 * draw and destructor. the flare's step does nothing, so the flare never ends
 * its own task. task needs a work, whose pos is the light's. each draw of the
 * task draws the flare at pos with lens_flare_draw.
 *
 * where ang.x has LENS_FLARE_RIVAL, the flare is a rival. the call lists a
 * rival unless six are listed already. a rival, listed or not, draws only
 * where no listed rival lies nearer the current camera. the destructor takes
 * the flare off the list */
void lens_flare_init(struct task *task);
/* draw the flare of a light at pos, as the current camera sees it. the flare
 * draws only where the light lies less than 90 degrees from the camera's
 * view. a glow draws at the light's place on the screen. ghosts draw on the
 * line from there through the screen's center. the ghosts show once the light
 * lies within about 53 degrees of the view. they brighten as the angle
 * closes. within about 8 degrees, a glare draws over the light too.
 *
 * the current camera's entry in camera_views must not be null, unchecked. pos
 * goes onto the screen through the current matrix, which must be the current
 * camera's view, unchecked.
 *
 * the call leaves Ninja's fog on, whether or not it was on before. it leaves
 * Ninja's blending as source alpha over inverse source alpha, and the constant
 * material at 0. where the flare draws, lens_flare_texlist stays the current
 * texture list */
void lens_flare_draw(NJS_POINT3 *pos);

#endif
