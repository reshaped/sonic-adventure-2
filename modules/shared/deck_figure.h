/* FIG_AI, FIG_AI2 and FIG_ENE, the carrier stages' figures that a hit breaks */
#ifndef SHARED_DECK_FIGURE_H
#define SHARED_DECK_FIGURE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;
extern NJS_TEXLIST *deck_figure_texlists[];

/* build each figure's model and far model at buffer, and return the next
 * free byte. from 300 units of the camera on, FIG_ENE draws its far model */
Uint8 *deck_figures_build(Uint8 *buffer);

/* the inits of FIG_AI, FIG_AI2 and FIG_ENE. a hit breaks a figure into the
 * pieces of its model. FIG_ENE alone is also land, which a player stands on */
void deck_figure_ally_init(struct task *task);
void deck_figure_ally_2_init(struct task *task);
void deck_figure_enemy_init(struct task *task);

#endif
