#ifndef EVENT_MOVIE_H
#define EVENT_MOVIE_H
#include <sfdfx_mw.h>
#include <sg_mwply.h>
#include <sg_xpt.h>

#include "event/script.h"
#include "system/game_state.h"

#define EVENT_MOVIE_PAL                                                        \
    (NJD_DSP_PAL | NJD_DSP_PSINTER | NJD_DSP_640x480_EXT |                     \
     NJD_PALEXT_HEIGHT_RATIO_1_133)
/* set mode to game_state.display's resolution: VGA's, pal under PAL, or
 * NTSC's. another display leaves mode as it was */
#define EVENT_SET_DISPLAY_MODE(mode, pal)                                      \
    switch (game_state.display) {                                              \
    case DISPLAY_VGA:                                                          \
        (mode) = NJD_RESOLUTION_VGA;                                           \
        break;                                                                 \
    case DISPLAY_PAL:                                                          \
        (mode) = (pal);                                                        \
        break;                                                                 \
    case DISPLAY_NTSC:                                                         \
        (mode) = NJD_RESOLUTION_640x480_NTSCNI;                                \
        break;                                                                 \
    }

enum event_movie_kind {
    MOVIE_OVERLAY = 1,
    MOVIE_TEXTURE,
    MOVIE_OPAQUE,
    MOVIE_MASKED
};

struct event_movie_request {
    const char *name;
    /* fade_in is how far a movie's brightness rises each frame, from 0 to
     * 255. fade_out is how far it falls each frame as the movie ends. a fade
     * of 0 starts the movie at 255, or ends it after 60 frames */
    Sint32 fade_in, fade_out;
};
/* the fade_out the event gives the movies event_movie_play plays */
#define EVENT_MOVIE_FADE_OUT 17
struct event_movie_texture {
    const char *name;
    Sint32 texture;
};
struct event_movie_scene {
    const char *name;
    struct event_screen_point position;
    union {
        struct {
            Float depth;
            Sint32 kind;
        } screen;
        struct event_movie_texture texture;
    } draw;
};

extern Bool event_movie_draw_plane;
extern Bool event_debug_print;
extern MWPLY event_movie_player;
extern Bool event_movie_scene_active;
extern MWS_PLY_CPRM_SFD event_movie_parameters;
extern Bool event_movie_texture_active;
void event_movie_start_scenes(void);
void event_movie_stop_scenes(void);
void event_movie_step(void);
void event_movie_draw(void);
void event_movie_pause(Bool paused);
void event_movie_start_scene(struct event_movie_scene *request);
void event_movie_start_texture(struct event_movie_texture *request);
void event_movie_transfer_texture(void);
void event_movie_step_texture(void);
void event_movie_record_error(void *obj, char *message);
#ifdef NONMATCHING
/* this side returns whether the movie played */
Bool event_movie_play(struct event_movie_request *request);
#else
void event_movie_play(struct event_movie_request *request);
#endif
#endif
