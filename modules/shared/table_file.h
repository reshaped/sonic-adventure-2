/* table files: PRS files that open with a table of offsets into themselves,
 * which reading turns into pointers */
#ifndef SHARED_TABLE_FILE_H
#define SHARED_TABLE_FILE_H
#include <sg_xpt.h>

/* read the table file for name in the text language, and return it. each
 * returns NULL where no file reads. the file is the first of
 * <name><letter>.prs, <name>j.prs and <name>.prs that loader_read_prs_alloc
 * reads. letter is the text language's letter. the caller frees the file
 * with loader_free.
 *
 * a -1 ends the file's table of longs. each long is an offset from the
 * file's start, and the read adds the file's address to it. the caller gives
 * the table's entries their type.
 * table_file_read_language_sparse leaves a long of 0 as 0. where count isn't
 * null, *count gets the number of longs before the -1. where no file reads,
 * *count keeps its value.
 *
 * name must be no longer than 250 characters, and game_state.text_language
 * must be below TEXT_LANGUAGES, both unchecked */
void *table_file_read_language(const char *name, Sint32 *count);
void *table_file_read_language_sparse(const char *name, Sint32 *count);

/* list holds longs in groups. a -2 ends each group, and a -1 ends the list.
 * it returns a pointer to the long after the first -2 at or past list. it
 * returns NULL where a -1 comes first */
Sint32 *list_next_group(Sint32 *list);

#endif
