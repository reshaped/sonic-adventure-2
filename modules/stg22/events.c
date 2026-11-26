/* the stage's exit, and the event task's step, which makes the rails and
 * plays the stage's voices */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "effect/particle.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/item_icon.h"
#include "shared/messenger.h"
#include "shared/motions.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "sound/adx.h"
#include "stg22/crazy_gadget.h"
#include "stg22/rail.h"
#include "stg22/stage_data.h"
#include "system/flags.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/sequence.h"
#include "system/task.h"
#include "system/texture.h"

/* the event task plays the voices in the hero story alone. it plays
 * OPENING_VOICE at frame OPENING_FRAME of the stage clock's first second.
 * until the stage is cleared, it plays REMINDER_VOICE as the clock reaches
 * minute REMINDER_MINUTE. from REPEAT_MINUTE on it plays REPEAT_VOICE at each
 * even minute. REPEAT_VOICE stops once the stage is cleared or SECOND_RADIO
 * has played. a
 * player within RADIO_RANGE of a radio plays its voice, once. the work's tick
 * holds FIRST_RADIO and SECOND_RADIO, each set once its radio has played */
#define OPENING_VOICE 1735
#define OPENING_FRAME 5
#define REMINDER_VOICE 1736
#define REMINDER_MINUTE 2
#define REPEAT_VOICE 1737
#define REPEAT_MINUTE 4
#define FIRST_RADIO_VOICE 1738
#define SECOND_RADIO_VOICE 1739
#define VOICE_PRIORITY 0
#define RADIO_RANGE 90.0F
#define FIRST_RADIO 1
#define SECOND_RADIO 2
static NJS_POINT3 first_radio_position = {-10645.0F, -1007.0F, -2908.0F};
static NJS_POINT3 second_radio_position = {-9835.0F, -1027.0F, -2905.0F};

void crazy_gadget_exit(void)
{
    object_builds_exit();
    hint_box_hints_free();
    messenger_messages_free();
    object_light_destroy();
    fog_stop();
    njFogDisable();
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    particle_core_stop();
    texture_release_shared(gadget_object_pvms, gadget_object_groups);
    texture_release_shared(gadget_background_pvms, gadget_background_groups);
    set_end();
    if (gadget_set) {
        loader_free(gadget_set);
        gadget_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

void gadget_events_step(struct task *task)
{
    struct task_work *work = task->work;
    Sint8 minute;
    Sint8 second;
    Sint8 frame;
    /* @hack spelling: range, a local for RADIO_RANGE, where the constant in
     * place would do */
    Float range;

    if (!task->child) {
        gadget_rails_create(task);
    }
    if (game_flags[FLAG_SEQUENCE] == SEQUENCE_HERO) {
        stage_clock_get(&minute, &second, &frame);
        if (minute == 0 && second == 0 && frame == OPENING_FRAME) {
            voice_play(OPENING_VOICE, VOICE_PRIORITY);
        }
        if (!game_state.cleared) {
            if (minute == REMINDER_MINUTE && second == 0 && frame == 0) {
                voice_play(REMINDER_VOICE, VOICE_PRIORITY);
            }
            if (!(work->tick & SECOND_RADIO) && minute >= REPEAT_MINUTE &&
                minute % 2 == 0 && second == 0 && frame == 0) {
                voice_play(REPEAT_VOICE, VOICE_PRIORITY);
            }
        }
        range = RADIO_RANGE;
        if (!(work->tick & FIRST_RADIO) &&
            player_near(&first_radio_position, range)) {
            voice_play(FIRST_RADIO_VOICE, VOICE_PRIORITY);
            work->tick |= FIRST_RADIO;
        }
        if (!(work->tick & SECOND_RADIO) &&
            player_near(&second_radio_position, range)) {
            voice_play(SECOND_RADIO_VOICE, VOICE_PRIORITY);
            work->tick |= SECOND_RADIO;
        }
    }
}

/* gravity.c's code starts at the next 32-byte boundary */
/* clang-format off */
#ifdef DREAMCAST
#ifdef NONMATCHING
#else
/* @asm open: no inspect rule reads the NOPs its .ALIGN pads with */
#pragma asm
        .ALIGN      32
#pragma endasm
#endif
#endif
/* clang-format on */
