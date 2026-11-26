/* Exercise published story steps and mini-event triggers through the resident
 * controller. The fixture selects a stage without starting its renderer. */
#include <assert.h>
#include <stdlib.h>
#include <sg_xpt.h>
#include "event/mini_event.h"
#include "main.h"
#include "player/player.h"
#include "sound/adx.h"
#include "stage/stage.h"
#include "system/flags.h"
#include "system/game_state.h"
#include "system/sequence.h"

Uint8 game_flags[GAME_FLAGS];
Uint8 game_bits[GAME_BITS];
struct game_state game_state;
static int stage, character, loaded_event, starts;

void stage_set_character(int slot, int id)
{
    assert(slot == 0);
    character = id;
}

void stage_set_current(int id)
{
    stage = id;
}

int stage_current_id(void)
{
    return stage;
}

int player_character(int slot)
{
    assert(slot == 0);
    return character;
}

void mini_event_load(int event)
{
    loaded_event = event;
}

int mini_event_start(void)
{
    ++starts;
    return 0;
}

void music_play_deferred(void)
{
    abort();
}

static void expect_menu(int step)
{
    game_flags[FLAG_SEQUENCE_STEP] = step;
    game_state.mode = GAME_MODE_SEQUENCE;
    sequence_apply_step();
    assert(game_state.mode == GAME_MODE_TO_MENU);
    assert(game_flags[FLAG_SEQUENCE_STEP_COPY] == step);
}

int main(void)
{
    /* the menu's event step, then its table's end, then steps past it */
    sequence_start(SEQUENCE_MENU_EVENT);
    sequence_apply_step();
    assert(game_state.mode == GAME_MODE_EVENT);
    assert(game_state.next_mode == GAME_MODE_SEQUENCE);
    assert(sequence_events[0] == 350 && sequence_events[1] == NO_EVENT);
    assert(!sequence_mini_event_due());
    sequence_play_mini_event();
    assert(starts == 0 && game_flags[FLAG_MINI_EVENT_UNPLAYED]);
    sequence_advance();
    expect_menu(1);
    expect_menu(2);
    expect_menu(255);

    /* Sand Ocean as Eggman, whose mini event starts once */
    sequence_start(SEQUENCE_SAND_OCEAN);
    sequence_apply_step();
    assert(game_state.mode == GAME_MODE_STAGE);
    assert(stage == STAGE_SAND_OCEAN && character == PLAYER_EGGMAN_MECH);
    assert(sequence_mini_event_due());
    sequence_play_mini_event();
    assert(starts == 1 && loaded_event == 104 && !sequence_mini_event_due());
    sequence_play_mini_event();
    assert(starts == 1);
    sequence_advance();
    expect_menu(1);
    expect_menu(2);
    expect_menu(255);
    return 0;
}
