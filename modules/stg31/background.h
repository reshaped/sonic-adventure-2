#ifndef STG31_BACKGROUND_H
#define STG31_BACKGROUND_H
#include <ninja.h>
#include <sg_xpt.h>

struct texture_anim;

/* a terrain model that the background animates. where anim isn't null, it
 * scrolls the model's texture by speed_u and speed_v times a phase. a model
 * without anim takes the stage's pulsing tint instead */
struct terrain_scroll {
    NJS_CNK_OBJECT *object;
    struct texture_anim *anim;
    Sint32 speed_v;
    Sint32 speed_u;
};

#define GREEN_HILL_SCROLL_MODELS 16
extern struct terrain_scroll
    green_hill_terrain_scrolls[GREEN_HILL_SCROLL_MODELS];
extern Uint32 green_hill_tint_dark;
extern Uint32 green_hill_tint_light;
extern Uint32 green_hill_tint_rate;
extern Sint16 green_hill_scroll_rate;
extern Sint32 green_hill_horizon_u;
extern Sint32 green_hill_horizon_v;
extern Sint32 green_hill_lower_u;
extern Sint32 green_hill_lower_v;
extern Angle green_hill_sky_turn;

#endif
