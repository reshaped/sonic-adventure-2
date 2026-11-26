#ifndef STG32_MACHINERY_H
#define STG32_MACHINERY_H

#include <ninja.h>

struct task;

extern NJS_TEXLIST herd_drill_texlist;
extern NJS_TEXLIST herd_beam_texlist;
extern NJS_TEXLIST herd_small_marker_texlist;
extern NJS_TEXLIST herd_large_marker_texlist;
extern NJS_TEXLIST herd_pole_texlist;
extern NJS_TEXLIST *herd_drill_textures[];
extern NJS_TEXLIST *herd_beam_textures[];
extern NJS_TEXLIST *herd_marker_textures[];
extern NJS_TEXLIST *herd_pole_textures[];

/* BORING, a drill whose head drops onto its base and bounces. once it
 * settles, the head rises and drops again. pos places it, and ang.y turns it */
void herd_drill_init(struct task *task);
/* BLINE, a line of light that slides along its own x from pos. its speed,
 * |scl.y| + 0.1, is how far it slides a step. it starts over at pos every
 * |scl.x| / speed steps, cut toward 0. |scl.x| must be at least the speed.
 * scl.z + 1 scales it along its z, and ang turns it */
void herd_beam_init(struct task *task);
/* DMARK, a solid marker whose lamp blinks. a scl.x of 1 or more draws the
 * large model, and less the small. pos places it, and ang.y turns it */
void herd_marker_init(struct task *task);
/* POLE, a solid post whose head turns and whose lamp pulses. pos places it,
 * and ang.y turns it */
void herd_pole_init(struct task *task);

#endif
