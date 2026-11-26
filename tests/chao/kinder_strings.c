#include <assert.h>
#include <string.h>

#include "chaostgkinder/keyboard.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/text.h"

struct game_state game_state;
static const Uint16 english[] = {3, 4, 6, 0, 'A', 0, 'B', 0};
static const char *filename;
static const Uint16 *file_data;
static Sint32 file_size;
static int reads;

Sint32 loader_read_prs_checked(const char *name, void *dest, Uint32 capacity)
{
    assert(strcmp(name, filename) == 0);
    assert(kinder_string(1)[0] == 0);
    assert(capacity == 65536 * sizeof(Uint16));
    if (file_size < 0) {
        /* The decoder may already have written part of its output. */
        memset(dest, 'X', sizeof(english));
    } else if ((Uint32)file_size <= capacity) {
        memcpy(dest, file_data, file_size);
    }
    reads++;
    return file_size;
}

static void load_english(void)
{
    game_state.text_language = TEXT_ENGLISH;
    filename = "kbdtext_e.prs";
    file_data = english;
    file_size = sizeof(english);
    kinder_strings_load();
    assert(kinder_string(0)[0] == 0);
    assert(kinder_string(1)[0] == 'A' && kinder_string(1)[1] == 0);
    assert(kinder_string(2)[0] == 'B' && kinder_string(2)[1] == 0);
    assert(kinder_string(-1)[0] == 0 && kinder_string(3)[0] == 0);
}

int main(void)
{
    static const Uint16 invalid[][8] = {{0},
                                        {8},
                                        {3, 2, 6},
                                        {3, 4, 8},
                                        {3, 4, 9},
                                        {3, 3, 6},
                                        {3, 4, 6, 0, 'A', 'X', 'B', 0},
                                        {3, 4, 6, 0, 'A', 0, 'B', 'X'}};
    static const Sint32 bad_sizes[] = {-1, 0, 1, sizeof(english) - 1, 131074};
    Uint16 *cached;
    unsigned int i;
    int before;

    assert(kinder_string(0)[0] == 0);
    for (i = 0; i < sizeof(invalid) / sizeof(invalid[0]); i++) {
        load_english();
        cached = kinder_string(1);
        file_data = invalid[i];
        kinder_strings_load();
        assert(kinder_string(1)[0] == 0 && cached[0] == 0);
    }
    for (i = 0; i < sizeof(bad_sizes) / sizeof(bad_sizes[0]); i++) {
        load_english();
        cached = kinder_string(1);
        file_size = bad_sizes[i];
        kinder_strings_load();
        assert(kinder_string(1)[0] == 0 && cached[0] == 0);
    }
    load_english();
    cached = kinder_string(1);
    before = reads;
    game_state.text_language = TEXT_LANGUAGES;
    kinder_strings_load();
    assert(reads == before && cached[0] == 0 && kinder_string(1)[0] == 0);
    load_english();
    cached = kinder_string(2);
    kinder_strings_release();
    assert(cached[0] == 0 && kinder_string(2)[0] == 0);
    load_english();
    return 0;
}
