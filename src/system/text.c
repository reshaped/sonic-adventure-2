/* the game's two text files in its text language: the text file and the
 * mcwarn file */
#include <sg_xpt.h>
#include <shinobi.h>

#include "system/game_state.h"
#include "system/loader.h"
#include "system/text.h"

/* each file's buffer from text_alloc, or null while it has none. each lies
 * before its file's pointer, as in the image */
static void *text_buffer = NULL;
Sint32 *text_file = NULL;
static void *mcwarn_buffer = NULL;
Sint32 *mcwarn_file = NULL;

static const char *text_names[TEXT_LANGUAGES] = {
    "text_j.prs", "text_e.prs", "text_f.prs", "text_s.prs", "text_g.prs",
};

static const char *mcwarn_names[TEXT_LANGUAGES] = {
    "mcwarn_j.prs", "mcwarn_e.prs", "mcwarn_f.prs",
    "mcwarn_s.prs", "mcwarn_g.prs",
};

#ifdef NONMATCHING
/* The stage-name and action tables use 144 text entries. mcwarn has eight
 * strings followed by a -1 sentinel. Offsets can repeat or be unordered. */
#define TEXT_ENTRIES 144
#define MCWARN_ENTRIES 8

static Bool read_strings(const char *name, Sint32 *offsets, Uint32 capacity,
                         Sint32 count)
{
    Sint32 size;
    Sint32 i;
    Sint32 first = count * (Sint32)sizeof(*offsets);
    const char *bytes = (const char *)offsets;

    size = loader_read_prs_checked(name, offsets, capacity);
    /* Every published string needs a terminator within the decoded file. */
    while (size > first && bytes[size - 1] != '\0') {
        size--;
    }
    if (size <= first) {
        return FALSE;
    }
    for (i = 0; i < count; i++) {
        if (offsets[i] < first || offsets[i] >= size) {
            return FALSE;
        }
    }
    return TRUE;
}

void text_alloc(void)
{
    if (text_buffer == NULL) {
        text_buffer = syCalloc(1, TEXT_SIZE);
    }
    if (mcwarn_buffer == NULL) {
        mcwarn_buffer = syCalloc(1, MCWARN_SIZE);
    }
}

Bool text_read(void)
{
    int language = game_state.text_language;

    text_file = NULL;
    mcwarn_file = NULL;
    if (language < 0 || language >= TEXT_LANGUAGES) {
        return FALSE;
    }
    text_alloc();
    if (text_buffer == NULL || mcwarn_buffer == NULL ||
        !read_strings(text_names[language], text_buffer, TEXT_SIZE,
                      TEXT_ENTRIES) ||
        !read_strings(mcwarn_names[language], mcwarn_buffer, MCWARN_SIZE,
                      MCWARN_ENTRIES)) {
        return FALSE;
    }
    text_file = text_buffer;
    mcwarn_file = mcwarn_buffer;
    return TRUE;
}
#else
void text_alloc(void)
{
    text_buffer = syCalloc(1, TEXT_SIZE);
    text_file = NULL;
    mcwarn_buffer = syCalloc(1, MCWARN_SIZE);
    mcwarn_file = NULL;
}

void text_read(void)
{
    loader_read_prs(text_names[game_state.text_language], text_buffer);
    text_file = text_buffer;
    loader_read_prs(mcwarn_names[game_state.text_language], mcwarn_buffer);
    mcwarn_file = mcwarn_buffer;
}

#endif

void text_free(void)
{
#ifdef NONMATCHING
    /* this side clears the pointers into the buffers it frees below */
    text_file = NULL;
    mcwarn_file = NULL;
#endif
    if (text_buffer != NULL) {
        syFree(text_buffer);
        text_buffer = NULL;
    }
    if (mcwarn_buffer != NULL) {
        syFree(mcwarn_buffer);
        mcwarn_buffer = NULL;
    }
}
