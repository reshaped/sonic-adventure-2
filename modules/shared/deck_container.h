/* the containers that ride rails on the carrier's deck: CONTA, CONTB and CONTC
 */
#ifndef SHARED_DECK_CONTAINER_H
#define SHARED_DECK_CONTAINER_H
#include <ninja.h>

struct task;

/* deck_container_empty is CONTB's collision node, an empty platform. CONTA
 * takes its model for its own collision once the cargo breaks. CONTA's and
 * CONTC's cargo breaks into the three cargo_ models, which each places as its
 * cargo stands */
extern NJS_OBJECT deck_container_empty[];
extern NJS_CNK_MODEL cargo_plate;
extern NJS_CNK_MODEL cargo_panel;
extern NJS_CNK_MODEL cargo_side;
extern NJS_TEXLIST cargo_plate_texlist;
extern NJS_TEXLIST cargo_panel_texlist;
extern NJS_TEXLIST cargo_side_texlist;

extern NJS_TEXLIST *deck_container_a_texlists[];
extern NJS_TEXLIST *deck_container_b_texlists[];
extern NJS_TEXLIST *deck_container_c_texlists[];

/* the inits of CONTA, CONTB and CONTC. a container's scl.x is half its rail's
 * length, and scl.y + 0.1 its top speed a step. CONTA and CONTB ride along
 * their local z, and CONTC along its local x. CONTA's and CONTC's cargo breaks,
 * and the placement's state keeps it broken */
void deck_container_a_init(struct task *task);
void deck_container_b_init(struct task *task);
void deck_container_c_init(struct task *task);

#endif
