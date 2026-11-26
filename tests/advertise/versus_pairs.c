/* SELECTED_GAME_PAIR for the random game and the stages game, which have no
 * pair: the image reads the work's bytes past pairs, and the looks open to
 * the characters those bytes name */
#include <assert.h>

#include "modules/advertise/versus_select.c"

struct game_state game_state;
struct save_data save_data;
Uint8 game_flags[GAME_FLAGS];

int main(void)
{
    struct versus_select_work *work = &versus_select_work;
    const Sint8 *pair;

    work->games_help_shown = TRUE;
    work->wins[0] = PLAYER_SONIC;
    work->wins[1] = 2;
    work->unk_56[0] = 3;
    for (work->game = 0; work->game < VERSUS_SELECT_PAIRS; work->game++) {
        assert(SELECTED_GAME_PAIR(work) == work->pairs[work->game]);
    }
    work->game = VERSUS_SELECT_RANDOM_GAME;
    pair = SELECTED_GAME_PAIR(work);
    assert(pair[0] == work->games_help_shown && pair[1] == work->wins[0]);
    work->game = VERSUS_SELECT_STAGES_GAME;
    pair = SELECTED_GAME_PAIR(work);
    assert(pair[0] == work->wins[1] && pair[1] == work->unk_56[0]);

    /* the random game's first team takes Shadow's looks, by
     * games_help_shown, and its second Sonic's, by wins[0] */
    save_data.awards[SAVE_AWARD_SHADOW_TOP] = 1;
    work->game = VERSUS_SELECT_RANDOM_GAME;
    versus_select_find_looks(SELECTED_GAME_PAIR(work));
    assert(work->looks_open[0] == VERSUS_SELECT_LOOK_OPEN_ALT);
    assert(work->looks_open[1] == 0);
    return 0;
}
