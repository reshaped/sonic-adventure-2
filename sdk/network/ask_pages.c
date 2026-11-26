#include "ask_session.h"
#include "ask_pages.h"
#include "ask_temp.h"
#include "ask_jis.h"
#include <iaskdic.h>
#include <sg_xpt.h>
#include <string.h>

#define ASK_INDEX_BYTES (ASK_INDEX_ROWS * ASK_INDEX_KEY)
#define CACHE_PAGES 10
#define ENTRY_SLOTS 128
/* the header's 16-bit little-endian fields after the copyright: the main
 * dictionary's page count, then temp_page, gaku_mode and suji_mode */
#define HEADER_PAGES 46
#define HEADER_TEMP_PAGE 48
#define HEADER_NUMBER_MODE 52
/* the map's bytes for each first kana. a bit stands for a second kana, or
 * bit 0 for none */
#define PREFIX_ROW_BYTES 11
#define FIRST_KANA (ASK_KANA_BASE + 1)
#define LE16(p) ((p)[0] + ((p)[1] * 256))

struct cache_page {
    int number;
    int dirty;
    struct cache_page *next;
#ifdef NONMATCHING
    /* ASK_END_MARK bytes past the page end a page its records fill */
    Uint8 data[ASK_PAGE_BYTES + ASK_END_MARK];
#else
    Uint8 data[ASK_PAGE_BYTES];
#endif
};
struct dictionary_reference {
    Uint8 users, time;
    Uint16 row;
    Sint16 offset, temporary;
};
Uint8 ask_dictionary_header[ASK_PAGE_BYTES];
int mainupper;
int mainlower;
Uint8 mainindex[ASK_INDEX_ROWS][ASK_INDEX_KEY];
static struct cache_page pages[CACHE_PAGES];
static struct cache_page *most_recent;
static struct dictionary_reference entries[ENTRY_SLOTS];
static Uint8 file_name[68];
static int saved_temp_page;
/* a place for gaku_mode beside the saved temp_page and suji_mode. read_head
 * never fills it */
static int saved_learning_mode;
static int saved_number_mode;

#pragma section ATA
static char copyright[] = "ASK Copyright (C) ACCESS CO.,LTD.";
#pragma section

static int d_open(char *name, int mode)
{
    return FAskRom_Open(name, mode);
}
int d_read(int stream, Uint8 *buffer, int count)
{
    return FAskRom_Read(stream, buffer, count);
}
int d_write(int stream, const void *buffer, int count)
{
    (void)stream;
    (void)buffer;
    (void)count;
    return -1;
}
static int d_close(int stream)
{
    return FAskRom_Close(stream);
}
static int d_seek(int stream, int offset, int origin)
{
    /* @hack spelling: the seek's result held in result, where returning the
     * call would do */
    int result = FAskRom_Seek(stream, offset, origin);
    return result;
}

void set_dicname(char *name)
{
#ifdef NONMATCHING
    /* this side copies at most file_name's room, and ends it with a NUL */
    strncpy((char *)file_name, name, sizeof(file_name) - 1);
    file_name[sizeof(file_name) - 1] = 0;
#else
    strcpy((char *)file_name, name);
#endif
}

int open_dic(void)
{
    int stream;
    if ((stream = d_open((char *)file_name, dic_rw)) == -1) {
        dic_fd = ASK_NO_DICTIONARY;
        return -1;
    }
    dic_fd = stream;
    return 0;
}

int close_dic(void)
{
    if (d_close(dic_fd) == -1) {
        dic_fd = ASK_NO_DICTIONARY;
        return -1;
    }
    dic_fd = ASK_NO_DICTIONARY;
    return 0;
}

int seek_dic(int offset)
{
    if (d_seek(dic_fd, offset, ASK_FROM_START) == -1) {
        if (open_dic() != 0) {
            return -1;
        }
        if (d_seek(dic_fd, offset, ASK_FROM_START) == -1) {
            return -1;
        }
    }
    return 0;
}

static void init_page_tab(void);
void init_page(void)
{
    init_page_tab();
    init_entid_tab();
}

int read_head(void)
{
    Uint8 *field;
    if (seek_dic(0) == -1) {
        return -1;
    }
    if (d_read(dic_fd, ask_dictionary_header, ASK_PAGE_BYTES) !=
        ASK_PAGE_BYTES) {
        return -1;
    }
    field = ask_dictionary_header;
    if (strncmp((char *)field, copyright, strlen(copyright)) != 0) {
        return -1;
    }
    field += HEADER_PAGES;
    mainlower = 0;
    mainupper = LE16(field);
#ifdef NONMATCHING
    /* this side refuses a page count of 0 or less, or one past the index's
     * ASK_INDEX_ROWS */
    if (mainupper <= 0 || mainupper > ASK_INDEX_ROWS) {
        return -1;
    }
#endif
    field += HEADER_TEMP_PAGE - HEADER_PAGES;
    saved_temp_page = temp_page = LE16(field);
    field += HEADER_NUMBER_MODE - HEADER_TEMP_PAGE;
    saved_number_mode = suji_mode = LE16(field);
    entry2upd = 0;
    (void)saved_learning_mode;
    return 0;
}

int flush_head(void)
{
    Uint8 *field = ask_dictionary_header + HEADER_TEMP_PAGE;
    if (temp_page != saved_temp_page || suji_mode != saved_number_mode ||
        entry2upd != 0) {
        *field++ = temp_page % 256;
        *field++ = temp_page / 256;
        *field++ = gaku_mode % 256;
        *field++ = gaku_mode / 256;
        *field++ = suji_mode % 256;
        *field = suji_mode / 256;
        if (seek_dic(0) == -1) {
            return -1;
        }
        if (d_write(dic_fd, ask_dictionary_header, ASK_PAGE_BYTES) !=
            ASK_PAGE_BYTES) {
            return -1;
        }
    }
    return 0;
}

int read_index(void)
{
    if (seek_dic(ASK_PAGE_BYTES) == -1) {
        return -1;
    }
    if (d_read(dic_fd, (Uint8 *)mainindex, ASK_INDEX_BYTES) !=
        ASK_INDEX_BYTES) {
        return -1;
    }
    return 0;
}

int chk_entry2(const Uint8 *reading, Sint16 length)
{
    int row;
    int column;
    int offset;
    if (*reading < FIRST_KANA) {
        return 0;
    }
    row = *reading++ - FIRST_KANA;
    if (length == 1) {
        column = 0;
    } else if (*reading < FIRST_KANA) {
        return 0;
    } else {
        column = *reading - ASK_KANA_BASE;
    }
    offset = (row * PREFIX_ROW_BYTES) + (column >> 3);
#ifdef NONMATCHING
    /* this side refuses a missing prefix map, and a reading whose bit lies past
     * it */
    if (!entry2code || offset >= ASK_PAGE_BYTES - ASK_HEADER_PREFIX_MAP) {
        return 0;
    }
#endif
    if (entry2code[offset] & (1 << (column & 7))) {
        return 0;
    }
    return 1;
}

void set_entry2(const Uint8 *reading, Sint16 length)
{
    int row;
    int column;
    int offset;
    if (*reading < FIRST_KANA) {
        return;
    }
    row = *reading++ - FIRST_KANA;
    if (length == 1) {
        column = 0;
    } else if (*reading < FIRST_KANA) {
        return;
    } else {
        column = *reading - ASK_KANA_BASE;
    }
    offset = (row * PREFIX_ROW_BYTES) + (column >> 3);
#ifdef NONMATCHING
    /* this side sets no bit for a missing prefix map, or one that lies past
     * it */
    if (!entry2code || offset >= ASK_PAGE_BYTES - ASK_HEADER_PREFIX_MAP) {
        return;
    }
#endif
    entry2code[offset] |= 1 << (column & 7);
    entry2upd = 1;
}

int srch_page(const Uint8 *key)
{
    int upper = mainupper;
    int lower = mainlower;
    int middle;
    int order;
    while (lower + 1 < upper) {
        middle = (upper + lower) / 2;
        if ((order = strncmp((const char *)key, (char *)mainindex[middle],
                             ASK_INDEX_KEY)) == 0) {
            return middle;
        } else if (order > 0) {
            lower = middle;
        } else {
            upper = middle;
        }
    }
    if (strncmp((const char *)key, (char *)mainindex[lower], ASK_INDEX_KEY) <
        0) {
        return lower - 1;
    }
    return lower;
}

int page_fix(int row, const Uint8 *key)
{
    int length;
#ifdef NONMATCHING
    /* this side refuses a row outside the index, which mainindex[row + 1]
     * reads */
    if (row < -1 || row >= ASK_INDEX_ROWS - 1) {
        return 0;
    }
#endif
    length = ask_prefix(mainindex[row + 1], key, ASK_INDEX_KEY);
    if (key[length] == 0) {
        return 0;
    }
    return 1;
}

int calc_pulen(int row, const Uint8 *key)
{
    int length;
    if ((length = ask_prefix(mainindex[row], key, ASK_INDEX_KEY)) >=
        ASK_INDEX_KEY) {
        return length;
    }
    if (key[length] == 0) {
        return length;
    }
    return length + 1;
}

int ask_prefix(const Uint8 *a, const Uint8 *b, int length)
{
    int same;
    for (same = 0; same < length; same++, a++, b++) {
        if (*a != *b || *a == 0) {
            break;
        }
    }
    return same;
}

static void init_page_tab(void)
{
    struct cache_page *page;
    most_recent = pages;
    for (page = pages; page < &pages[CACHE_PAGES - 1]; page++) {
        page->number = -1;
        page->dirty = 0;
        page->next = page + 1;
    }
    page->number = -1;
    page->dirty = 0;
    page->next = NULL;
}

static int write_page(struct cache_page *page)
{
    if (seek_dic((page->number * ASK_PAGE_BYTES) + ASK_MAIN_OFFSET) == -1) {
        return -1;
    }
    if (d_write(dic_fd, page->data, ASK_PAGE_BYTES) != ASK_PAGE_BYTES) {
        return -1;
    }
    return 0;
}
static int read_page(struct cache_page *page)
{
    if (seek_dic((page->number * ASK_PAGE_BYTES) + ASK_MAIN_OFFSET) == -1) {
        return -1;
    }
#ifdef NONMATCHING
    /* this side fails a short read, and puts two zero bytes past the page's
     * ASK_PAGE_BYTES, where a scan of its records stops */
    if (d_read(dic_fd, page->data, ASK_PAGE_BYTES) != ASK_PAGE_BYTES) {
        return -1;
    }
    page->data[ASK_PAGE_BYTES] = 0;
    page->data[ASK_PAGE_BYTES + 1] = 0;
#else
    d_read(dic_fd, page->data, ASK_PAGE_BYTES);
#endif
    return 0;
}

Uint8 *load_page(int number)
{
    /* @hack redundant: previous declared register, where a plain local would
     * do */
    register struct cache_page *previous = NULL;
    struct cache_page *page = most_recent;
    for (;;) {
        if (page->number == number) {
            if (previous != NULL) {
                previous->next = page->next;
                page->next = most_recent;
                most_recent = page;
            }
            return page->data;
        } else if (page->next == NULL) {
            break;
        }
        previous = page;
        page = page->next;
    }
    previous->next = NULL;
    page->next = most_recent;
    most_recent = page;
    if (page->dirty == 1) {
        write_page(page);
    }
    page->number = number;
    page->dirty = 0;
#ifdef NONMATCHING
    if (read_page(page) != 0) {
        /* a page that can't be read stands as an empty one, whose first
         * size is 0. number -1 makes the next load read it again */
        memset(page->data, 0, sizeof(page->data));
        page->number = -1;
    }
#else
    read_page(page);
#endif
    return page->data;
}

void update_nowpage(void)
{
    most_recent->dirty = 1;
}

void flush_pages(void)
{
    struct cache_page *page;
    for (page = most_recent; page != NULL; page = page->next) {
        if (page->dirty == 1) {
            write_page(page);
        }
    }
}

void init_entid_tab(void)
{
    int i;
    for (i = 0; i < ENTRY_SLOTS; i++) {
        entries[i].users = 0;
    }
}

int set_entid_tab(int row, int offset, int temporary, int time)
{
    /* @hack redundant: entry declared register, where a plain local would do */
    register struct dictionary_reference *entry = entries;
    int i;
    int available = -1;
    for (i = 0; i < ENTRY_SLOTS; i++, entry++) {
        if (entry->users == 0) {
            if (available == -1) {
                available = i;
            }
        } else if (entry->row == row && entry->offset == offset &&
                   entry->temporary == temporary) {
#ifdef NONMATCHING
            /* this side refuses a 256th user, which entry->users, a byte, can't
             * count */
            if (entry->users == 255) {
                return -1;
            }
#endif
            entry->users++;
            return i;
        }
    }
    if (available == -1) {
        return -1;
    }
    entry = &entries[available];
    entry->users = 1;
    entry->row = row;
    entry->offset = offset;
    entry->temporary = temporary;
    entry->time = time;
    return available;
}

int get_entid_tab(unsigned int id, int *offset, int *temporary, int *time)
{
    struct dictionary_reference *entry;
    if (id >= ENTRY_SLOTS) {
        return -1;
    }
    entry = &entries[id];
    if (entry->users == 0) {
        return -1;
    }
    *offset = entry->offset;
    *temporary = entry->temporary;
    *time = entry->time;
    return entry->row;
}

int get_maxtime(const Sint32 *ids, int count)
{
    /* @hack redundant: i and id declared register, where plain locals would
     * do */
    register int i;
    register int id;
    int latest = 0;
    struct dictionary_reference *entry;
    for (i = 0; i < count; i++) {
        id = ids[i];
        if (id < 0 || id >= ENTRY_SLOTS) {
            continue;
        }
        if ((entry = &entries[id])->users > 0 && latest < entry->time) {
            latest = entry->time;
        }
    }
    return latest;
}

int update_entid_rtime(const Sint32 *ids, int count, int time)
{
    /* @hack redundant: i and id declared register, where plain locals would
     * do */
    register int i;
    register int id;
    struct dictionary_reference *entry;
    for (i = 0; i < count; i++) {
        id = ids[i];
        if (id < 0 || id >= ENTRY_SLOTS) {
            continue;
        }
        if ((entry = &entries[id])->users > 0) {
            entry->time = time;
        }
    }
    return 0;
}

int free_entid_tab(unsigned int id)
{
    struct dictionary_reference *entry;
    if (id >= ENTRY_SLOTS) {
        return -1;
    }
    entry = &entries[id];
    if (entry->users == 0) {
        return -1;
    }
    entry->users--;
    return 0;
}

void clear_entid_tmpall(int page)
{
    int i;
    for (i = 0; i < ENTRY_SLOTS; i++) {
        if ((Sint16)(entries[i].temporary >> ASK_TEMP_PAGE_SHIFT) == page) {
            entries[i].temporary = -1;
        }
    }
}

void clear_entid_tmp(int temporary)
{
    int i;
    for (i = 0; i < ENTRY_SLOTS; i++) {
        if (entries[i].temporary == temporary) {
            entries[i].temporary = -1;
        }
    }
}
