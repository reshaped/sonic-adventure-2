/* table files: PRS files that open with a table of offsets into themselves,
 * which reading turns into pointers */
#include <sg_xpt.h>
#include <string.h>

#include "shared/table_file.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/text.h"

/* the bytes a path may take, its null included */
#define PATH_SIZE 256
/* the language whose letter names the second file tried */
#define FALLBACK_LANGUAGE TEXT_JAPANESE
/* LIST_END ends a table and a list, and GROUP_END ends a group of a list */
#define LIST_END (-1)
#define GROUP_END (-2)

/* the letter a file's name takes for each text language, indexed by
 * game_state.text_language */
static char text_letters[TEXT_LANGUAGES] = {'j', 'e', 'f', 's', 'g'};

static Sint32 *table_file_read_sparse(const char *path, Sint32 *count);
static Sint32 *table_file_read(const char *path, Sint32 *count);

void *table_file_read_language(const char *name, Sint32 *count)
{
    char path[PATH_SIZE];
    char letter[2];
    Sint32 *table;

    letter[0] = text_letters[game_state.text_language];
    letter[1] = '\0';
    strcpy(path, name);
    strcat(path, letter);
    strcat(path, ".prs");
    table = table_file_read(path, count);
    if (table != NULL) {
        return table;
    }
    letter[0] = text_letters[FALLBACK_LANGUAGE];
    /* @hack redundant: the end of letter written again, though nothing has
     * changed it */
    letter[1] = '\0';
    strcpy(path, name);
    strcat(path, letter);
    strcat(path, ".prs");
    table = table_file_read(path, count);
    if (table != NULL) {
        return table;
    }
    strcpy(path, name);
    strcat(path, ".prs");
    table = table_file_read(path, count);
    /* @hack redundant: the test of the last read's result, though null
     * returns either way, where return table after the read would do */
    if (table != NULL) {
        return table;
    }
    return NULL;
}

void *table_file_read_language_sparse(const char *name, Sint32 *count)
{
    char path[PATH_SIZE];
    char letter[2];
    Sint32 *table;

    letter[0] = text_letters[game_state.text_language];
    letter[1] = '\0';
    strcpy(path, name);
    strcat(path, letter);
    strcat(path, ".prs");
    table = table_file_read_sparse(path, count);
    if (table != NULL) {
        return table;
    }
    letter[0] = text_letters[FALLBACK_LANGUAGE];
    /* @hack redundant: the end of letter written again, though nothing has
     * changed it */
    letter[1] = '\0';
    strcpy(path, name);
    strcat(path, letter);
    strcat(path, ".prs");
    table = table_file_read_sparse(path, count);
    if (table != NULL) {
        return table;
    }
    strcpy(path, name);
    strcat(path, ".prs");
    table = table_file_read_sparse(path, count);
    /* @hack redundant: the test of the last read's result, though null
     * returns either way, where return table after the read would do */
    if (table != NULL) {
        return table;
    }
    return NULL;
}

static Sint32 *table_file_read_sparse(const char *path, Sint32 *count)
{
    Sint32 *table;
    Sint32 *entry;
    Sint32 entries;

    table = loader_read_prs_alloc(path);
    entry = table;
    if (table == NULL) {
        return NULL;
    }
    entries = 0;
    while (*entry != LIST_END) {
        entries++;
        if (*entry != 0) {
            *entry += (Sint32)table;
        }
        entry++;
    }
    if (count != NULL) {
        *count = entries;
    }
    return table;
}

static Sint32 *table_file_read(const char *path, Sint32 *count)
{
    Sint32 *table;
    Sint32 *entry;
    Sint32 entries;

    table = loader_read_prs_alloc(path);
    entry = table;
    if (table == NULL) {
        return NULL;
    }
    entries = 0;
    while (*entry != LIST_END) {
        *entry += (Sint32)table;
        entry++;
        entries++;
    }
    if (count != NULL) {
        *count = entries;
    }
    return table;
}

Sint32 *list_next_group(Sint32 *list)
{
    /* @hack spelling: value, a local for the long at list, where *list
     * tested in place would do */
    Sint32 value;

    value = *list;
    while (value != LIST_END && value != GROUP_END) {
        list++;
        value = *list;
    }
    if (*list == LIST_END) {
        return NULL;
    }
    return list + 1;
}
