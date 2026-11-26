/* the lamp strips of Cannon's Core's mech routes, which light as
 * shared/core_machine_lights.h says. draw_strip_lights draws core_strip_lamps.
 * with CORE_SECOND_LAMP_GROUP defined, draw_entrance_lights draws
 * tails_entrance_lamps too */
#ifndef SHARED_CORE_STRIP_LIGHTS_H
#define SHARED_CORE_STRIP_LIGHTS_H
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/stage_util.h"
#include "shared/core_machine_lights.h"
#include "shared/core_lamp_art.h"
#include "system/game_state.h"
#include "system/task.h"

static void draw_lamp_at(NJS_CNK_OBJECT *lamp, Uint32 color)
{
    njPushMatrixEx();
    njTranslate(NULL, lamp->pos[0], lamp->pos[1], lamp->pos[2]);
    njRotateXYZ(NULL, lamp->ang[0], lamp->ang[1], lamp->ang[2]);
    model_draw_tinted_additive(&core_strip_lamp_model, color);
    njPopMatrixEx();
}

#define STRIP_PULSE_DRAW draw_strip_lights
#define STRIP_PULSE_LAMPS core_strip_lamps
#include "shared/core_strip_pulse_impl.h"
#undef STRIP_PULSE_DRAW
#undef STRIP_PULSE_LAMPS

#ifdef CORE_SECOND_LAMP_GROUP
#define STRIP_PULSE_DRAW draw_entrance_lights
#define STRIP_PULSE_LAMPS tails_entrance_lamps
#include "shared/core_strip_pulse_impl.h"
#undef STRIP_PULSE_DRAW
#undef STRIP_PULSE_LAMPS
#endif

#endif
