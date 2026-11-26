#include "ask_temp.h"
#include "ask_pages.h"
#include "ask_dictionary.h"
#include "ask_jis.h"
#include <sg_xpt.h>
#include <string.h>

#define TEMP_NODE_COUNT 512
/* the bytes of a key, a reading as a search takes it */
#define KEY_BYTES 80
/* a bucket for each first byte of a reading from ASK_KANA_BASE. bucket 0
 * takes every other first byte too */
#define BUCKETS 80
#define RECORD_SIZE(record) ((Uint16)(((record)[1] << 8) + (record)[0]))
#define PAGE_BYTE_MASK ((1 << ASK_TEMP_PAGE_SHIFT) - 1)

struct temp_node {
    Uint8 *record;
    struct temp_node *next;
};
int temp_page;
static struct temp_node nodes[TEMP_NODE_COUNT];
static struct temp_node *free_nodes;
static struct temp_node *buckets[BUCKETS];
static Uint8 *next_record;
static Uint8 *page_end;
static Uint8 modified;
typedef Uint8 temp_page_rows[ASK_TEMP_PAGES][ASK_PAGE_BYTES];
#ifdef NONMATCHING
/* this side reads the pages through a union, the plain form the hack tag below
 * gives */
static union {
    temp_page_rows pages;
    Uint8 bytes[ASK_TEMP_PAGES * ASK_PAGE_BYTES];
} records;
#define TEMP_BYTES records.bytes
#define TEMP_ROWS records.pages
#else
static Uint8 temp_bytes[ASK_TEMP_PAGES * ASK_PAGE_BYTES];
#define TEMP_BYTES temp_bytes
/* @hack type: temp_bytes, flat, and its pages read through a cast, where a
 * union of the two would do */
#define TEMP_ROWS (*(temp_page_rows *)&temp_bytes)
#endif

static void init_nodes(void);
static void init_buckets(void);
static void reset_pages(void);
static void reclaim_page(void);
static int read_temp(void);
static int write_temp(void);
static int kanji_lead(Uint8 byte);

void init_temp(void)
{
    init_nodes();
    init_buckets();
    if (read_temp() == -1) {
#ifdef NONMATCHING
        /* read_temp can fail once it has indexed some records, so the index
         * starts over */
        init_nodes();
        init_buckets();
#endif
        reset_pages();
    }
    modified = 0;
}
void flush_temp(void)
{
    if (modified) {
        write_temp();
    }
}

static void init_nodes(void)
{
    struct temp_node *node = nodes;
    for (; node < &nodes[TEMP_NODE_COUNT - 1]; node++) {
        node->next = node + 1;
    }
#ifdef DREAMCAST
    /* @hack redundant: a test of nodes, in a block no path reaches */
    if (0) {
        if (nodes) {
        }
    }
    /* @hack redundant: an empty block no path reaches */
    if (0) {
    }
#endif
    node->next = NULL;
    free_nodes = nodes;
}
static void init_buckets(void)
{
    struct temp_node **bucket = buckets;
    do {
        *bucket++ = NULL;
    } while (bucket < buckets + BUCKETS);
}
static void reset_pages(void)
{
    int index;
    Uint8 *page;
    for (index = 0; index < ASK_TEMP_PAGES; index++) {
        page = TEMP_ROWS[index];
        page[0] = page[1] = 0;
    }
    temp_page = 0;
    next_record = TEMP_ROWS[0];
    page_end = TEMP_ROWS[0] + ASK_PAGE_BYTES;
}
static int reading_bucket(const Uint8 *reading)
{
    Uint8 initial = *reading;
    if (initial >= ASK_KANA_BASE && initial < ASK_KANA_BASE + BUCKETS) {
        return initial - ASK_KANA_BASE;
    }
    return 0;
}

static struct temp_node *alloc_node(void)
{
    struct temp_node *node;
#ifdef NONMATCHING
    /* this side reclaims pages until a node is free, where one reclaimed page
     * frees none */
    while (!free_nodes) {
        reclaim_page();
    }
#else
    if (!free_nodes) {
        reclaim_page();
    }
#endif
    node = free_nodes;
    free_nodes = free_nodes->next;
    return node;
}
static void free_node(struct temp_node *node)
{
    node->next = free_nodes;
    free_nodes = node;
}
static Uint8 *alloc_record(int size)
{
    Uint8 *record;
    if (next_record > page_end - size - ASK_END_MARK) {
        reclaim_page();
    }
    record = next_record;
    next_record += size;
    next_record[0] = 0;
    next_record[1] = 0;
    return record;
}

static struct temp_node **find_node(const Uint8 *reading, Sint16 length,
                                    struct temp_node **found)
{
    struct temp_node *previous = NULL;
    struct temp_node *node;
    Sint16 record_length;
    int bucket;
    int order;
    node = buckets[bucket = reading_bucket(reading)];
    while (node) {
        record_length = node->record[ASK_READING_LENGTH_AT];
        if (!(order = strncmp((char *)node->record + ASK_READING_AT,
                              (const char *)reading, record_length))) {
            /* @hack type: both lengths cast to unsigned long, where comparing
             * them as they are would do */
            if ((unsigned long)length == (unsigned long)record_length) {
                break;
            }
        } else if (order > 0) {
            break;
        }
        previous = node;
        node = node->next;
    }
    *found = node;
    if (!previous) {
        return &buckets[bucket];
    }
    return &previous->next;
}

static void reclaim_page(void)
{
    Uint8 reading[KEY_BYTES];
    struct temp_node *node;
    struct temp_node **link;
    Uint8 *record;
    Sint16 length;
    *next_record++ = 0;
    *next_record++ = 0;
    temp_page = (temp_page + 1) % ASK_TEMP_PAGES;
    next_record = TEMP_ROWS[temp_page];
    page_end = next_record + ASK_PAGE_BYTES;
    for (record = next_record; RECORD_SIZE(record);
         record += RECORD_SIZE(record)) {
        if ((length = record[ASK_READING_LENGTH_AT]) != 0) {
            strncpy((char *)reading, (char *)record + ASK_READING_AT, length);
            /* @hack spelling: terminator, a copy of length, where length
             * itself would do */
            {
                int terminator = length;
                reading[terminator] = 0;
            }
            link = find_node(reading, length, &node);
#ifdef DREAMCAST
            /* @hack redundant: an empty if (0) */
            if (0) {
            }
#endif
#ifdef NONMATCHING
            /* this side tests node for NULL, which find_node leaves where no
             * record sorts at or after the reading */
            if (node && node->record == record) {
#else
            if (node->record == record) {
#endif
                *link = node->next;
                free_node(node);
            }
        }
    }
    clear_entid_tmpall(temp_page);
}

static int record_offset(const Uint8 *record)
{
    int offset = record - TEMP_BYTES;
    int page = offset / ASK_PAGE_BYTES;
    return (page << ASK_TEMP_PAGE_SHIFT) | (offset % ASK_PAGE_BYTES);
}
Uint8 *load_temp(int offset)
{
    Uint8 *record = &TEMP_ROWS[(Sint16)(offset >> ASK_TEMP_PAGE_SHIFT)]
                              [(Sint16)(offset & PAGE_BYTE_MASK)];
    return record;
}

static int read_temp(void)
{
    Uint8 reading[KEY_BYTES];
    struct temp_node *node;
    struct temp_node **link;
    Uint8 *record;
    Sint16 length;
    int page;
    int offset;
    if (seek_dic(ASK_TEMP_OFFSET) == -1) {
        return -1;
    }
    if (d_read(dic_fd, TEMP_BYTES, sizeof(TEMP_BYTES)) != sizeof(TEMP_BYTES)) {
        return -1;
    }
#ifdef NONMATCHING
    /* this side refuses a stored temp_page outside the ASK_TEMP_PAGES pages */
    if (temp_page < 0 || temp_page >= ASK_TEMP_PAGES) {
        return -1;
    }
#endif
    for (page = 0; page < ASK_TEMP_PAGES; page++) {
        offset = page * ASK_PAGE_BYTES;
        record = offset + TEMP_BYTES;
        while (RECORD_SIZE(record)) {
#ifdef NONMATCHING
            /* this side refuses a record shorter than 3 bytes or past its
             * page's end, and one whose reading runs past the record or past
             * reading's room */
            int size = RECORD_SIZE(record);
            if (size < 3 ||
                size >
                    TEMP_ROWS[page] + ASK_PAGE_BYTES - ASK_END_MARK - record ||
                record[ASK_READING_LENGTH_AT] > size - ASK_READING_AT ||
                record[ASK_READING_LENGTH_AT] >= sizeof(reading)) {
                return -1;
            }
#endif
            if ((length = record[ASK_READING_LENGTH_AT]) != 0) {
                strncpy((char *)reading, (char *)record + ASK_READING_AT,
                        length);
                /* @hack spelling: terminator, a copy of length, where length
                 * itself would do */
                {
                    int terminator = length;
                    reading[terminator] = 0;
#ifdef DREAMCAST
                    /* @hack redundant: an unreachable test of terminator */
                    if (0) {
                        if (terminator) {
                        }
                    }
#endif
                }
                link = find_node(reading, length, &node);
                {
                    struct temp_node *fresh;
#ifdef NONMATCHING
                    /* this side fails the load where no node is free */
                    if (!free_nodes) {
                        return -1;
                    }
#endif
                    fresh = alloc_node();
                    fresh->record = record;
                    fresh->next = node;
                    *link = fresh;
                }
            }
            record += RECORD_SIZE(record);
        }
        if (temp_page == page) {
            next_record = record;
            page_end = TEMP_BYTES + offset + ASK_PAGE_BYTES;
        }
    }
    return 0;
}

static int write_temp(void)
{
    if (seek_dic(ASK_TEMP_OFFSET) == -1) {
        return -1;
    }
    if (d_write(dic_fd, TEMP_BYTES, sizeof(TEMP_BYTES)) != sizeof(TEMP_BYTES)) {
        return -1;
    }
    return 0;
}

int newwdlen(struct ask_learn_word *word)
{
    int head_bytes;
    if (word->form || word->kind >= ASK_FORM_KINDS) {
        head_bytes = ASK_FORM_AT + 1;
    } else {
        head_bytes = ASK_FORM_AT;
    }
    return word->length + head_bytes + setkbuflen(word->writing) +
           ASK_READING_AT;
}
int updwdlen(struct ask_learn_word *word)
{
    int head_bytes;
    if (word->form || word->kind >= ASK_FORM_KINDS) {
        head_bytes = ASK_FORM_AT + 1;
    } else {
        head_bytes = ASK_FORM_AT;
    }
    return setkbuflen(word->writing) + head_bytes;
}

void set_record(Uint8 *record, int size, struct ask_learn_word *word,
                int frequency)
{
    *record++ = size % 256;
    *record++ = size / 256;
    *record++ = word->length;
    strncpy((char *)record, (char *)word->reading, word->length);
    record += word->length;
    *record++ = word->kind;
    *record++ = frequency;
    if (word->form || word->kind >= ASK_FORM_KINDS) {
        *record++ = word->form;
    }
    setkbuf(word->writing, record);
}
void upd_record(Uint8 *record, int extra, struct ask_learn_word *word,
                int frequency)
{
    int length = RECORD_SIZE(record);
    extra += length;
    record[0] = extra % 256;
    record[1] = extra / 256;
    record += length;
    *record++ = word->kind;
    *record++ = frequency;
    if (word->form || word->kind >= ASK_FORM_KINDS) {
        *record++ = word->form;
    }
    setkbuf(word->writing, record);
}

int tmp_touroku(const Uint8 *reading, struct ask_learn_word *word,
                int frequency)
{
    struct temp_node *node;
    struct temp_node *fresh;
    struct temp_node **link;
    Uint8 *record;
    Uint8 *stored_length;
    int reading_length;
    int length;
#ifdef NONMATCHING
    /* this side refuses a reading length outside 1 to KEY_BYTES - 1, and a word
     * too long for a page */
    if (word->length <= 0 || word->length >= KEY_BYTES ||
        newwdlen(word) > ASK_PAGE_BYTES - ASK_END_MARK) {
        return -1;
    }
#endif
    modified = 1;
    link = find_node(reading, word->length, &node);
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: it reads node's record only where
     * find_node found one */
    if (node) {
#else
    /* @bug memory: reads through node, which may be NULL, at 0x0c9f1514.
     * find_node leaves it NULL where no record of the bucket sorts at or
     * after reading, as in an empty bucket */
    {
#endif
        record = node->record;
        stored_length = record + ASK_READING_LENGTH_AT;
        if (*stored_length == (reading_length = word->length) &&
            !strncmp((const char *)reading, (char *)record + ASK_READING_AT,
                     reading_length)) {
            *stored_length = 0;
            *link = node->next;
            clear_entid_tmp(record_offset(node->record));
            free_node(node);
        }
    }
    fresh = alloc_node();
    record = alloc_record(length = newwdlen(word));
    set_record(record, length, word, frequency);
    link = find_node(reading, word->length, &node);
    fresh->record = record;
    fresh->next = node;
    *link = fresh;
    return 0;
}

int tmp_snssyn(const char *reading, Sint16 length, struct ask_lookup *result)
{
    struct temp_node *node;
    Uint8 *record;
    Uint8 *best;
    int search_length;
    Sint16 record_length;
    Sint16 best_length;
    Sint16 prefix_length;
    int common;
    int count;
    char *stored;
    int order;
    result->length = result->prefix_length = 0;
    best_length = 0;
    prefix_length = 0;
    node = buckets[reading_bucket((const Uint8 *)reading)];
    while (node) {
        record = node->record;
        record_length = record[ASK_READING_LENGTH_AT];
        /* @hack spelling: count, a copy of record_length, where record_length
         * itself would do */
        count = record_length;
        stored = (char *)record + ASK_READING_AT;
        /* @hack type: ask_prefix's result cast to Sint16, where common takes
         * it as it is */
        if ((common = (Sint16)ask_prefix((Uint8 *)stored,
                                         (const Uint8 *)reading, count)) >=
            prefix_length) {
            prefix_length = common + 1;
        }
        /* @hack spelling: search_length, a copy of length, where length itself
         * would do */
        search_length = length;
        if (!(order = strncmp(stored, reading, count))) {
            if (count == search_length) {
                return ASK_DIC_WHOLE;
            }
            best_length = record_length;
            best = record;
        } else if (order > 0) {
            if (!strncmp(stored, reading, search_length)) {
                return ASK_DIC_WHOLE;
            }
            break;
        }
        node = node->next;
    }
    if (!best_length) {
        result->offset = -1;
        return ASK_DIC_NONE;
    }
    result->row = 0;
    result->offset = record_offset(best);
    result->length = best_length;
    result->prefix_length = prefix_length;
    result->record = best;
    return ASK_DIC_FOUND;
}
int tmp_getsyn(const char *reading, Sint16 length, struct ask_lookup *result)
{
    struct temp_node *node;
    Uint8 *record;
    Sint16 record_length;
    int order;
    node = buckets[reading_bucket((const Uint8 *)reading)];
    while (node) {
        record = node->record;
        record_length = record[ASK_READING_LENGTH_AT];
        if (!(order = strncmp((char *)record + ASK_READING_AT, reading,
                              record_length))) {
            /* @hack type: both lengths cast to unsigned long, where comparing
             * them as they are would do */
            if ((unsigned long)record_length == (unsigned long)length) {
                break;
            }
        } else if (order > 0) {
            result->offset = -1;
            return ASK_DIC_NONE;
        }
        node = node->next;
    }
    if (!node) {
        result->offset = -1;
        return ASK_DIC_NONE;
    }
    result->row = 0;
    result->offset = record_offset(record);
    result->record = record;
    return ASK_DIC_FOUND;
}

void shift_temp(int offset)
{
    struct temp_node *node;
    Uint8 reading[KEY_BYTES];
    Uint8 *record;
    Uint8 *original;
    Uint8 *fresh;
    int length;
    Sint16 text_length;
    Sint16 raw_length;
    int page;
    if ((page = (Sint16)(offset >> ASK_TEMP_PAGE_SHIFT)) != temp_page) {
        original = record = load_temp(offset);
        /* @hack spelling: the size read low byte first, where RECORD_SIZE
         * would do */
        fresh = alloc_record(length = (Uint16)(record[0] + (record[1] * 256)));
        if (page == temp_page) {
            return;
        }
        /* @hack spelling: raw_length and count, copies on the way from
         * the reading's length to the strncpy, where text_length alone would
         * do */
        raw_length = record[ASK_READING_LENGTH_AT];
        text_length = raw_length;
        {
            Sint16 count = text_length;
            strncpy((char *)reading, (char *)record + ASK_READING_AT, count);
        }
        reading[text_length] = 0;
        find_node(reading, text_length, &node);
#ifdef NONMATCHING
        /* this side tests node for NULL, which find_node leaves where no record
         * sorts at or after the reading */
        if (node && node->record == record) {
#else
        if (node->record == record) {
#endif
            node->record = fresh;
            while (length--) {
                *fresh++ = *record++;
            }
            original[ASK_READING_LENGTH_AT] = 0;
            clear_entid_tmp(offset);
        }
    }
}
void update_nowtmp(void)
{
    modified = 1;
}

int setkbuflen(const Uint8 *text)
{
    int length = 0;
    while (*text) {
        if (kanji_lead(*text)) {
            text += 2;
        } else {
            text++;
        }
        length += 2;
    }
    return length;
}
void setkbuf(const Uint8 *text, Uint8 *output)
{
    while (*text) {
        if (kanji_lead(*text)) {
            *output = *text++;
        } else {
            *output = ASK_ONE_BYTE;
        }
        output++;
        *output++ = *text++;
    }
}
int getkbuflen(const Uint8 *record, const Uint8 *end)
{
    int length = 0;
    while (record < end && *record > ASK_LAST_KIND) {
        if (*record == ASK_ONE_BYTE) {
            length++;
        } else {
            length += 2;
        }
        record += 2;
    }
    return length;
}
void getkbuf(Uint8 *output, const Uint8 *record, const Uint8 *end)
{
    while (record < end && *record > ASK_LAST_KIND) {
        if (*record == ASK_ONE_BYTE) {
            record++;
        } else {
            *output++ = *record++;
        }
        *output++ = *record++;
    }
    *output = 0;
}
/* whether byte leads a two-byte Shift-JIS character. unlike is_kanji, it
 * takes 0x80 too */
static int kanji_lead(Uint8 byte)
{
    if ((byte >= 0x80 && byte <= 0x9f) || (byte >= 0xe0 && byte <= 0xfc)) {
        return 1;
    }
    return 0;
}
