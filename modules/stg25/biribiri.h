/* the electric spark, BIRIBIRI in Death Chamber's object list: a sprite
 * of crackling electricity that faces the camera. player 0 hears it buzz
 * near it. BIRIBIRI is Japanese for the tingle of a shock */
#ifndef STG25_BIRIBIRI_H
#define STG25_BIRIBIRI_H
#include <ninja.h>

struct task;

/* the spark's texture lists for texture_load_shared, a frame's at its index,
 * ended by null */
extern NJS_TEXLIST *chamber_biribiri_texlists[];

/* the object list's step for BIRIBIRI. task needs a work, whose pos places
 * the spark. player 0's work must not be null, unchecked. the spark shows
 * its 64 frames in turn, a frame a step, from a random first frame */
void chamber_biribiri_init(struct task *task);
/* draw a spark at pos, in the current matrix's space, as BIRIBIRI's draw
 * does. its frame is game_state.steps modulo 64 */
void chamber_biribiri_draw_at(NJS_POINT3 *pos);

#endif
