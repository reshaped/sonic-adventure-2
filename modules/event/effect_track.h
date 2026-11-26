#ifndef EVENT_EFFECT_TRACK_H
#define EVENT_EFFECT_TRACK_H
#include <ninja.h>
#include <sg_xpt.h>

/* an effect of the script, at the root of the current scene's
 * motions[motion]. it fires on the step that takes event_frame from below
 * frame to or past it. a kind that event_particles holds makes that particle,
 * which takes speed and scale */
struct event_effect {
    Sint32 frame;
    Sint8 kind, motion;
    Uint8 unk_06[2];
    Float scale;
    NJS_VECTOR speed;
    Uint8 unk_18[32];
};
/* the kinds that make no particle. EVENT_FLARE_ON turns the lens flare on at
 * the root, and EVENT_FLARE_OFF turns it off. the motion sprite kinds read
 * speed.y as a sprite's id.
 * EVENT_MOTION_SPRITE_ON starts a sprite at the root, with fade pattern
 * speed.x, size speed.z and texture scale. EVENT_MOTION_SPRITE_OFF hides the
 * sprites of that id */
#define EVENT_FLARE_ON 8
#define EVENT_FLARE_OFF 9
#define EVENT_MOTION_SPRITE_ON 17
#define EVENT_MOTION_SPRITE_OFF 18

#define EVENT_PARTICLES 19
typedef struct particle *event_particle_effect(NJS_POINT3 *position,
                                               NJS_VECTOR *speed, Float scale);
/* the particle each kind of effect makes. kind 0 and the kinds above hold
 * NULL */
extern event_particle_effect *event_particles[EVENT_PARTICLES];

void event_effects_reset(void);
void event_effect_fire(struct event_effect *effect);
void event_flare_place(NJS_POINT3 *position);
void event_flare_on(void);
void event_flare_off(void);
void event_flare_draw(void);
void event_effects_draw(void);

#endif
