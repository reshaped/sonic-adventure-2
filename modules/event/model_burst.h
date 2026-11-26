#ifndef EVENT_MODEL_BURST_H
#define EVENT_MODEL_BURST_H
#include <ninja.h>
#include <sg_xpt.h>
#include "system/angle.h"

/* a burst of count pieces. model is 0 for the crystal, or 1 or 2 for its long
 * or short shard. a piece starts at position, and flies along y turned by
 * direction. each piece turns each of direction's angles by up to spread / 2
 * either way, in Angle units. its speed is 0.5 to 1.5 times speed, and its size
 * 0.5 to 1.5 times scale. acceleration adds to its velocity each frame. every
 * trail_interval frames, a piece makes the particle event_particles[trail].
 * trail_interval must not be 0, and event_particles[trail] must not be NULL.
 * the caller fills the fields through trail_interval, and each piece's task
 * sets the rest */
struct event_model_burst {
    struct angles direction;
    NJS_POINT3 position;
    NJS_VECTOR acceleration;
    Sint32 spread;
    Float speed, scale;
    Sint32 count, model, trail, trail_interval;
    NJS_VECTOR velocity;
    struct angles rotation, spin;
    Sint32 age;
};
/* start a task for each of burst's pieces, with a copy of burst */
void event_model_burst_start(struct event_model_burst *burst);
#endif
