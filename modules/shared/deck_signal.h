/* SIGNAL and RADERS: the carrier stages' signal lamp, and the radar a hit
 * breaks */
#ifndef SHARED_DECK_SIGNAL_H
#define SHARED_DECK_SIGNAL_H
#include <ninja.h>

struct task;
extern NJS_TEXLIST *deck_signal_texlists[];
extern NJS_TEXLIST *deck_radar_s_texlists[];

/* the inits of SIGNAL and RADERS. a hit stops SIGNAL's lamp blinking, and
 * sets the signal rocking and raising dust for a while. a hit on RADERS's
 * weak point, 30 units above pos, breaks the radar. the placement's state
 * keeps either hit */
void deck_signal_init(struct task *task);
void deck_radar_s_init(struct task *task);

#endif
