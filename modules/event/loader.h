#ifndef EVENT_LOADER_H
#define EVENT_LOADER_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/texture.h"

/* the PVMs and lists that texture_load_shared takes for the event's
 * effects */
extern struct texture_pvm event_pvms[];
extern NJS_TEXLIST **event_texture_groups[];
/* the textures of eveff.pvm, which the model bursts draw with */
extern NJS_TEXLIST event_effect_texlist;

/* the events below EVENT_FIRST_MOVIE are scenes, e%04d.prs. those from it
 * on are movies, e%04d.sfd */
#define EVENT_FIRST_MOVIE 400

#ifdef NONMATCHING
/* event_load_step's result where a file the event needs fails to load. it
 * then counts no view of the event, and leaves the sequence where it was */
#define EVENT_LOAD_FAILED (-2)
#endif

/* load an event, a step at a time. in a sequence, it loads
 * sequence_events[event_sequence_index] and moves the index on. otherwise it
 * loads the event the debug menu selects. it returns the event's number once
 * its files have loaded or its movie has played. until then it returns
 * NO_EVENT. a load counts a view of the event in game_bits. a movie's load sets
 * event_movie_draw_plane. a story's ending next in the sequence becomes the
 * second ending once the other story's has been seen. each call first makes
 * that change */
int event_load_step(void);
#endif
