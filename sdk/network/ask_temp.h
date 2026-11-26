/* the dictionary's temporary area: pages of records of the words ASK
 * learns, as ask_dictionary.h lays a record out. a record's offset in the
 * area is its page shifted by ASK_TEMP_PAGE_SHIFT, plus its byte in the
 * page. the area adds records to its current page. when it runs out of room,
 * the oldest page becomes the current one, and its records are dropped */
#ifndef ASK_TEMP_H
#define ASK_TEMP_H
#include "ask_dictionary.h"
#include <sg_xpt.h>

#define ASK_TEMP_PAGE_SHIFT 12

/* a record a lookup found, at offset in main page row. in the temporary
 * area, row is 0. offset is -1 for none. a search for the longest
 * reading sets length to the reading's. it sets prefix_length one past the
 * most bytes the key shares with a reading it compared */
struct ask_lookup {
    Sint16 row, offset, length, prefix_length;
    Uint8 reserved[4];
    Uint8 *record;
};
/* read the area from the dictionary's file, or empty it where the read
 * fails. flush_temp writes it back once tmp_touroku or update_nowtmp has
 * marked it changed */
void init_temp(void);
void flush_temp(void);
Uint8 *load_temp(int offset);
/* the bytes set_record writes for word, and that upd_record adds for it */
int newwdlen(struct ask_learn_word *word);
int updwdlen(struct ask_learn_word *word);
/* write a record of size bytes that holds word alone. upd_record writes word
 * at the record's end, where the caller has made extra bytes of room */
void set_record(Uint8 *record, int size, struct ask_learn_word *word,
                int frequency);
void upd_record(Uint8 *record, int extra, struct ask_learn_word *word,
                int frequency);
/* add a record of reading that holds word alone, and return 0. the record
 * takes the place of any record of the same reading */
int tmp_touroku(const Uint8 *reading, struct ask_learn_word *word,
                int frequency);
/* tmp_snssyn finds the area's longest reading that reading begins with. it
 * returns ASK_DIC_WHOLE where an area reading begins with all of reading.
 * tmp_getsyn finds the area reading that is all of reading. each returns
 * ASK_DIC_FOUND, or ASK_DIC_NONE where it finds none */
int tmp_snssyn(const char *reading, Sint16 length, struct ask_lookup *result);
int tmp_getsyn(const char *reading, Sint16 length, struct ask_lookup *result);
/* move the record at offset to the current page, so that it is dropped last.
 * making room for it can drop it first */
void shift_temp(int offset);
void update_nowtmp(void);
/* setkbuf writes NUL-terminated Shift-JIS text as a word's text. getkbuf
 * writes a word's text back, then a NUL, stopping at end or at the next
 * word's kind. setkbuflen and getkbuflen return the bytes each writes, but
 * for getkbuf's NUL */
int setkbuflen(const Uint8 *text);
void setkbuf(const Uint8 *text, Uint8 *output);
int getkbuflen(const Uint8 *record, const Uint8 *end);
void getkbuf(Uint8 *output, const Uint8 *record, const Uint8 *end);
#endif
