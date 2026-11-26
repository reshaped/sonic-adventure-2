#include <sg_xpt.h>
#ifdef NONMATCHING
/* memset, with which this side's kinder_strings_release empties the strings */
#include <string.h>
#endif
#include "chaostgkinder/keyboard.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/text.h"

/* the words a 16-bit offset reaches */
#define STRING_WORDS 0x10000

/* the decoded kbdtext file. an index of the strings' offsets comes first,
 * in words from the file's start. the strings follow, each ending in a 0
 * code. the first offset, the index's length in words, is the number of
 * strings */
static Uint16 strings[STRING_WORDS];

#ifdef NONMATCHING
static Uint16 empty_string[1];
static Sint32 string_count;

/* the number of strings in a file of size bytes, or 0 where the file is
 * malformed */
static Sint32 count_strings(Sint32 size)
{
    Sint32 words, count, end, i;

    if (size <= 0 || (Uint32)size > sizeof(strings) || (size & 1)) {
        return 0;
    }
    words = size / sizeof(strings[0]);
    count = strings[0];
    if (count == 0 || count >= words) {
        return 0;
    }
    for (i = 0; i < count; i++) {
        end = i + 1 < count ? strings[i + 1] : words;
        if (end <= strings[i] || end > words || strings[end - 1] != 0) {
            return 0;
        }
    }
    return count;
}
#endif

void kinder_strings_load(void)
{
    const char *name = NULL;
    switch (game_state.text_language) {
    case TEXT_JAPANESE:
        name = "kbdtext_j.prs";
        break;
    case TEXT_ENGLISH:
        name = "kbdtext_e.prs";
        break;
    case TEXT_FRENCH:
        name = "kbdtext_f.prs";
        break;
    case TEXT_SPANISH:
        name = "kbdtext_s.prs";
        break;
    case TEXT_GERMAN:
        name = "kbdtext_g.prs";
        break;
    }
#ifdef NONMATCHING
    /* this side frees the strings it held, and reads the file bounded by
     * strings' size. it keeps no strings from a file it can't count */
    if (string_count != 0) {
        kinder_strings_release();
    }
    if (name) {
        string_count = count_strings(
            loader_read_prs_checked(name, strings, sizeof(strings)));
        if (string_count == 0) {
            kinder_strings_release();
        }
    }
#else
    if (name) {
        loader_read_prs(name, strings);
    }
#endif
}

void kinder_strings_release(void)
{
#ifdef NONMATCHING
    string_count = 0;
    /* widgets keep pointers into strings, so a release empties their text
     * rather than leave it stale */
    memset(strings, 0, sizeof(strings));
#endif
}

Uint16 *kinder_string(Sint32 id)
{
#ifdef NONMATCHING
    /* this side gives an empty string for an id outside the strings loaded */
    if (id < 0 || id >= string_count) {
        return empty_string;
    }
#endif
    return &strings[strings[id]];
}
