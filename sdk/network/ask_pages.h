/* the dictionary's file holds a header page, then the index from
 * ASK_PAGE_BYTES. the temporary area's pages follow, then the main
 * dictionary's. the file is ROM: d_write fails, whatever it is given */
#ifndef ASK_PAGES_H
#define ASK_PAGES_H
#include <sg_xpt.h>
#include "ask_session.h"
#define ASK_PAGE_BYTES 1024
#define ASK_INDEX_ROWS 1024
#define ASK_INDEX_KEY 4
#define ASK_TEMP_PAGES 8
/* where the temporary area's pages and the main dictionary's begin in the
 * file */
#define ASK_TEMP_OFFSET (ASK_PAGE_BYTES + (ASK_INDEX_ROWS * ASK_INDEX_KEY))
#define ASK_MAIN_OFFSET (ASK_TEMP_OFFSET + (ASK_TEMP_PAGES * ASK_PAGE_BYTES))
/* the offset in the header of the map chk_entry2 reads */
#define ASK_HEADER_PREFIX_MAP 78

extern Uint8 ask_dictionary_header[ASK_PAGE_BYTES];
/* mainindex holds a key for each main page in order. srch_page searches it
 * from row mainlower, which is always 0, up to row mainupper. a page holds
 * the readings from its key up to the next page's */
extern int mainupper, mainlower;
extern Uint8 mainindex[ASK_INDEX_ROWS][ASK_INDEX_KEY];
extern int dic_fd, dic_rw, gaku_mode, suji_mode, temp_page;
/* dic_fd while no dictionary is open */
#define ASK_NO_DICTIONARY (-1)
int d_read(int stream, Uint8 *buffer, int count);
int d_write(int stream, const void *buffer, int count);
void set_dicname(char *name);
/* open_dic, close_dic and seek_dic return 0, or -1 where they fail. so do
 * read_head, flush_head and read_index. seek_dic opens the file again after
 * a seek that fails, and seeks once more */
int open_dic(void);
int close_dic(void);
int seek_dic(int offset);
void init_page(void);
/* read the header page, which must begin with ACCESS's copyright, and take
 * mainupper, temp_page and suji_mode from it */
int read_head(void);
/* write temp_page, gaku_mode and suji_mode into the header, and the header
 * into the file. it writes nothing unless temp_page or suji_mode has changed
 * since read_head, or set_entry2 has marked the map */
int flush_head(void);
int read_index(void);
/* the header's map of the readings of one or two kana that the main
 * dictionary holds. set_entry2 marks the length bytes at reading. chk_entry2
 * returns 1 where they are unmarked, so that no page holds them. it returns 0
 * where they are marked, or aren't all kana */
int chk_entry2(const Uint8 *reading, Sint16 length);
void set_entry2(const Uint8 *reading, Sint16 length);
/* the row of mainindex whose key matches key's first ASK_INDEX_KEY bytes.
 * failing that, the last row whose key lies below them. -1 where every
 * row's key lies above them */
int srch_page(const Uint8 *key);
/* 0 where the key after row's in mainindex begins with all of key. a later
 * page may then hold readings that do too. 1 otherwise */
int page_fix(int row, const Uint8 *key);
/* the bytes key shares with row's key in mainindex, and one more where key
 * goes on past them. the count stops at ASK_INDEX_KEY */
int calc_pulen(int row, const Uint8 *key);
/* how many of their first length bytes a and b share, stopping at a NUL in
 * a */
int ask_prefix(const Uint8 *a, const Uint8 *b, int length);
/* main page number's bytes, from a cache of the pages loaded last.
 * update_nowpage marks the page loaded last as changed. flush_pages writes
 * back each changed page, as load_page does one it drops */
Uint8 *load_page(int number);
void update_nowpage(void);
void flush_pages(void);
/* an entry is a slot for a reading's records, and its id the slot's index.
 * a slot holds a main page's row and the record's offset in it, and the
 * temporary area's record. it counts its users, and holds a time.
 *
 * set_entid_tab returns the entry that holds those records already, with one
 * user more. failing that, it gives a free slot the records, one user and
 * time, and returns that entry. it returns -1 when every slot is taken.
 * get_entid_tab sets *offset, *temporary and *time, and returns the entry's
 * row. free_entid_tab takes one user away, and returns 0. get_entid_tab and
 * free_entid_tab return -1 for an id of no slot in use */
void init_entid_tab(void);
int set_entid_tab(int row, int offset, int temporary, int time);
int get_entid_tab(unsigned int id, int *offset, int *temporary, int *time);
/* the latest time of the count entries at ids, or 0. update_entid_rtime
 * gives each of them time. an id of no slot in use counts for nothing in
 * either */
int get_maxtime(const Sint32 *ids, int count);
int update_entid_rtime(const Sint32 *ids, int count, int time);
int free_entid_tab(unsigned int id);
/* clear_entid_tmpall drops each entry's temporary record that lies in the
 * temporary area's page. clear_entid_tmp drops each entry's temporary record
 * at offset temporary */
void clear_entid_tmpall(int page);
void clear_entid_tmp(int temporary);
#endif
