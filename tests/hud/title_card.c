/* Select the archive through the same entry used by the stage controller.
 * The texture reader records the request and reports an unavailable archive. */
#include <assert.h>
#include <string.h>
#include "hud/title_card.c"

static int current_stage, character, requests;
static const char *expected_archive;

int stage_current_id(void)
{
    return current_stage;
}

int stage_character(int slot)
{
    assert(slot == 0);
    return character;
}

NJS_TEXLIST *texture_load_pvm_alloc(const char *name)
{
    assert(expected_archive != NULL && strcmp(name, expected_archive) == 0);
    ++requests;
    return NULL;
}

static void check_archive(int stage, int player, const char *name)
{
    current_stage = stage;
    character = player;
    expected_archive = name;
    requests = 0;
    assert(!title_card_start());
    assert(requests == (name != NULL));
}

int main(void)
{
    check_archive(STAGE_CITY_ESCAPE, PLAYER_SONIC, "stg_title13");
    check_archive(STAGE_RELAY_FIRST, PLAYER_TAILS_MECH, "stg_title36");
    check_archive(STAGE_CART_1, PLAYER_ROUGE, "stg_title71");
    check_archive(STAGE_CART_1, PLAYER_TAILS, "stg_title70");
    check_archive(STAGE_VERSUS_SONIC_1, PLAYER_SHADOW, NULL);
    check_archive(STAGE_GREEN_FOREST, PLAYER_SONIC, "stg_title03");
    return 0;
}
