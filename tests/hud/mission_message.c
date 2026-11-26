/* The title card prepares and measures a caption before the rasterizer uses
 * the disc workspace as its pixel buffer. */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "system/areas.h"

#define TABLE_ENTRIES (50 * 5)
static Sint32 work[4096];
static Sint32 catalog[4096];

#undef WORK_AREA
#undef WORK_AREA_END
#define WORK_AREA ((Uint8 *)work)
#define WORK_AREA_END (WORK_AREA + sizeof(work))
#include "hud/mission_message.c"

struct game_state game_state;
struct time_limit_table time_limits = {{{STAGE_NONE, {0, 0}}}, STAGE_NONE};
struct stage_time cart_time_limits[CART_LIMITS] = {{4, 5}, {3, 0}};

static struct text_sprite sprite;
static const char *expected;
static const char *filename;
static Sint32 file_size;
static int stage = STAGE_CART_1;
static int driver = PLAYER_TAILS;
static int reads;
static int allocations;
static Bool heap_full;
static Uint16 width = 320;

int stage_current_id(void)
{
    return stage;
}

int stage_character(int player)
{
    assert(player == 0);
    return driver;
}

Sint32 loader_read_prs_buffer(const char *name, void *dest, Uint32 capacity)
{
    assert(strcmp(name, filename) == 0);
    assert(dest == WORK_AREA && capacity == sizeof(work));
    reads++;
    if (file_size > 0) {
        assert((size_t)file_size <= capacity);
        memcpy(dest, catalog, (size_t)file_size);
    }
    return file_size;
}

void *syMalloc(Uint32 size)
{
    void *block = heap_full ? NULL : malloc(size);
    if (block != NULL) {
        allocations++;
    }
    return block;
}

void syFree(void *block)
{
    assert(block != NULL && allocations == 1);
    allocations--;
    free(block);
}

void text_sprite_measure(struct text_sprite *owner, const char *text,
                         int language, Uint16 *w, Uint16 *h)
{
    assert(owner->pitch_x == TEXT_PITCH_X && owner->pitch_y == TEXT_PITCH_Y);
    assert(language == game_state.text_language && allocations == 1);
    assert(strcmp(text, expected) == 0);
    *w = width;
    *h = 3 * (GLYPH_SIZE + TEXT_PITCH_Y);
}

struct text_sprite *text_sprite_create(const char *text, int language)
{
    memset(work, 0, sizeof(work));
    assert(language == game_state.text_language && strcmp(text, expected) == 0);
    return &sprite;
}

void text_sprite_destroy(struct text_sprite *owner)
{
    assert(owner == &sprite);
}

static void set_text(int row, const char *text)
{
    Sint32 start = (TABLE_ENTRIES + 1) * sizeof(Sint32);

    memset(catalog, 0, sizeof(catalog));
    catalog[TABLE_ENTRIES] = -1;
    catalog[row] = start;
    catalog[row + 1] = start;
    strcpy((char *)catalog + start, text);
    file_size = start + (Sint32)strlen(text) + 4;
    memset((char *)catalog + file_size - 3, ' ', 3);
}

static void expect_load(Bool present)
{
    mission_message_load();
    assert(message_sprite == (present ? &sprite : NULL));
    assert(allocations == 0);
}

int main(void)
{
    static const char translation[] =
        "A translated title-card caption can use more than sixty-four bytes.\n"
        "The next line can be shorter.\nThe last one fits as well.";

    game_state.text_language = TEXT_ENGLISH;
    game_state.mission = MISSION_TIMED;
    expect_load(FALSE);
    assert(reads == 0);
    mission_letters[TEXT_ENGLISH] = "E";
    filename = "MISSIONE.PRS";
    set_text(CART_TEXTS_TAILS * MISSIONS + MISSION_TIMED, "%1d min %2d sec");
    expected = "4 min  5 sec";
    expect_load(TRUE);
    file_size = -1;
    expect_load(FALSE);

    game_state.text_language = TEXT_JAPANESE;
    driver = PLAYER_ROUGE;
    mission_letters[TEXT_JAPANESE] = "J";
    filename = "MISSIONJ.PRS";
    set_text(CART_TEXTS_ROUGE * MISSIONS + MISSION_TIMED, "%1d:%02d");
    expected = "3:00";
    expect_load(TRUE);
    file_size = sizeof(Sint32);
    expect_load(FALSE);
    set_text(CART_TEXTS_ROUGE * MISSIONS + MISSION_TIMED, "Time: %1d");
    expect_load(FALSE);
    set_text(CART_TEXTS_ROUGE * MISSIONS + MISSION_TIMED, "\x83");
    expect_load(FALSE);

    stage = STAGE_CITY_ESCAPE;
    game_state.text_language = TEXT_FRENCH;
    mission_letters[TEXT_FRENCH] = "F";
    filename = "MISSIONF.PRS";
    game_state.mission = MISSION_FIRST;
    expected = translation;
    set_text(stage * MISSIONS, expected);
    heap_full = TRUE;
    expect_load(FALSE);
    heap_full = FALSE;
    expect_load(TRUE);
    width = 1025;
    expect_load(FALSE);
    width = 320;
    expect_load(TRUE);
    catalog[stage * MISSIONS + 1] = catalog[stage * MISSIONS] + 5;
    expect_load(FALSE);
    game_state.text_language = -1;
    game_state.mission = -1;
    reads = 0;
    expect_load(FALSE);
    assert(reads == 0);
    return 0;
}
