/* the game's two text files in its text language: the text file and the
 * mcwarn file */
#ifndef SYSTEM_TEXT_H
#define SYSTEM_TEXT_H
#include <sg_xpt.h>

/* the languages text_read has files for, as game_state.text_language counts
 * them. their files' names carry the letters j, e, f, s and g in that
 * order */
enum text_language {
    TEXT_JAPANESE,
    TEXT_ENGLISH,
    TEXT_FRENCH,
    TEXT_SPANISH,
    TEXT_GERMAN
};
#define TEXT_LANGUAGES 5

/* the room text_alloc gives each file, decoded, in bytes */
#define TEXT_SIZE 0x6000
#define MCWARN_SIZE 0x1000

/* The two published files. On the port both become available together, and
 * both are null after a failed load or text_free. */
extern Sint32 *text_file;
extern Sint32 *mcwarn_file;

/* the text at index in text_file, which opens with each text's offset from
 * the file's start. MCWARN gives a text of mcwarn_file, which opens the same
 * way */
#define TEXT(index) ((char *)text_file + text_file[index])
#define MCWARN(index) ((char *)mcwarn_file + mcwarn_file[index])

/* the mcwarn file's texts, by index. MCWARN_BLOCKS and MCWARN_SAVE_BLOCKS are
 * sprintf formats of the blocks a save needs. MCWARN_SOCKET is a format of a
 * socket's number and its port's letter, and TEXT_JAPANESE's takes the letter
 * first */
#define MCWARN_NO_CARD 0
#define MCWARN_BLOCKS 1
#define MCWARN_NO_PAD 2
#define MCWARN_SOCKET 3
#define MCWARN_NOT_OURS 4
#define MCWARN_SAVE_BLOCKS 5
#define MCWARN_SAVE_FAILED 6
#define MCWARN_LOAD_FAILED 7

/* Allocate the cleared buffers for both files. The port retains existing
 * allocations and retries missing ones on the next read. */
void text_alloc(void);
#ifdef NONMATCHING
/* Load the selected language pair within its buffers and check the string
 * offsets and terminators. On failure neither file is published. */
Bool text_read(void);
#else
/* The original publishes each buffer after its unchecked read. */
void text_read(void);
#endif
/* free the buffers that text_alloc gave, if any */
void text_free(void);

#endif
