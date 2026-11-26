#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#include "chaostgkinder/book.h"
#include "chaostgkinder/book_text.h"
#include "chaostgkinder/wide_text.h"

#define SAVE_VERSION 1
#define TITLE_BYTES (BOOK_TITLE_CODES * sizeof(Uint16))
#define MARK_LINE_BYTES (BOOK_MARK_LINE_CODES * sizeof(Uint16))
#define LINK_TEXT_BYTES (BOOK_LINK_CODES * sizeof(Uint16))
/* a page record's one-byte fields: its options, its line count and style,
 * and each link's kind and page */
#define PAGE_SAVE_FIELDS (BOOK_PAGE_OPTIONS + 2 + (BOOK_PAGE_LINKS * 2))
/* the save keeps every code of a page line but its last */
#define LINE_SAVE_BYTES ((BOOK_LINE_CODES - 1) * sizeof(Uint16))
/* an export's header opens with two blank words and a blank field. the
 * title follows, in BOOK_TITLE_CODES bytes. the offsets of the marks and
 * the pages come next, and last the first page and the setting */
#define EXPORT_HEADER_BYTES 46
#define BLANK_WORD_BYTES 4
#define BLANK_FIELD_BYTES 16
#define PAGE_HEADER_BYTES 10
/* the links' part of an export. two links take LINKS_HEADER_BYTES and their
 * texts. no link takes NO_LINK_BYTES: the style and the first kind. one
 * link takes ONE_LINK_BYTES: those and its page */
#define LINKS_HEADER_BYTES 9
#define NO_LINK_BYTES 2
#define ONE_LINK_BYTES 3

static const Uint16 empty_book_title[] = {0};
static const Uint16 empty_mark_title[] = {0};
static const Uint16 empty_page_title[] = {0};

/* @hack spelling: book_mark_line_create expanded inline, where calls would
 * do */
#ifdef NONMATCHING
#else
#pragma inline(book_mark_line_create)
#endif
struct book_mark_line *book_mark_line_create(void)
{
    struct book_mark_line *line;
    /* @hack redundant: an empty if on the allocation, where the assignment
     * alone would do */
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    line = syCalloc(1, sizeof(*line));
#else
    if ((line = syCalloc(1, sizeof(*line))) != NULL) {
    }
#endif
    return line;
}

void book_mark_line_destroy(struct book_mark_line *line)
{
    syFree(line);
}

struct book_mark *book_mark_create(void)
{
    struct book_mark *mark;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((mark = syCalloc(1, sizeof(*mark))) != NULL) {
        kinder_wide_copy(mark->title, empty_mark_title);
    }
    return mark;
}

void book_mark_destroy(struct book_mark *mark)
{
    int i;
    for (i = 0; i < mark->line_count; i++) {
        book_mark_line_destroy(mark->lines[i]);
    }
    syFree(mark);
}

void book_mark_add_line(struct book_mark *mark)
{
    if (mark->line_count < BOOK_MARK_LINES) {
#ifdef NONMATCHING
        /* this side adds no line where book_mark_line_create makes none */
        struct book_mark_line *line = book_mark_line_create();
        if (line) {
            mark->lines[mark->line_count++] = line;
        }
#else
        mark->lines[mark->line_count++] = book_mark_line_create();
#endif
    }
}

/* @hack spelling: book_line_create expanded inline, where calls would do */
#ifdef NONMATCHING
#else
#pragma inline(book_line_create)
#endif
struct book_line *book_line_create(void)
{
    struct book_line *line;
    /* @hack redundant: an empty if on the allocation, where the assignment
     * alone would do */
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    line = syCalloc(1, sizeof(*line));
#else
    if ((line = syCalloc(1, sizeof(*line))) != NULL) {
    }
#endif
    return line;
}

void book_line_destroy(struct book_line *line)
{
    syFree(line);
}

struct book_page *book_page_create(void)
{
    struct book_page *page;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((page = syCalloc(1, sizeof(*page))) != NULL) {
        kinder_wide_copy(page->title, empty_page_title);
    }
    /* @bug memory: where the allocation fails, book_page_create at
     * 0x0c9c1c04 writes the default options through NULL */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it returns null before the
     * options */
    if (!page) {
        return NULL;
    }
#endif
    page->options[0] = BOOK_DEFAULT_RATING;
    page->options[1] = BOOK_DEFAULT_RATING;
    page->options[2] = BOOK_DEFAULT_RATING;
    page->options[3] = BOOK_DEFAULT_RATING;
    return page;
}

void book_page_destroy(struct book_page *page)
{
    int i;
    for (i = 0; i < page->line_count; i++) {
        book_line_destroy(page->lines[i]);
    }
    syFree(page);
}

void book_page_add_line(struct book_page *page)
{
    if (page->line_count < BOOK_PAGE_LINES) {
#ifdef NONMATCHING
        /* this side adds no line where book_line_create makes none */
        struct book_line *line = book_line_create();
        if (line) {
            page->lines[page->line_count++] = line;
        }
#else
        page->lines[page->line_count++] = book_line_create();
#endif
    }
}

struct book *book_create(void)
{
    struct book *book;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((book = syCalloc(1, sizeof(*book))) != NULL) {
        kinder_wide_copy(book->title, empty_book_title);
    }
    return book;
}

void book_destroy(struct book *book)
{
    int i;
    for (i = 0; i < book->mark_count; i++) {
        book_mark_destroy(book->marks[i]);
    }
    for (i = 0; i < book->page_count; i++) {
        book_page_destroy(book->pages[i]);
    }
    syFree(book);
}

struct book_mark *book_add_mark(struct book *book)
{
    struct book_mark *mark = NULL;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (book->mark_count < BOOK_MARKS && (mark = book_mark_create()) != NULL) {
        book->marks[book->mark_count++] = mark;
    }
    return mark;
}

struct book_page *book_add_page(struct book *book)
{
    struct book_page *page = NULL;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (book->page_count < BOOK_PAGES && (page = book_page_create()) != NULL) {
        book->pages[book->page_count++] = page;
    }
    return page;
}

void book_remove_mark(struct book *book, int index)
{
    int i;
    book_mark_destroy(book->marks[index]);
    book->mark_count--;
    for (i = index; i < book->mark_count; i++) {
        book->marks[i] = book->marks[i + 1];
    }
}

void book_remove_page(struct book *book, int index)
{
    int i;
    book_page_destroy(book->pages[index]);
    book->page_count--;
    for (i = index; i < book->page_count; i++) {
        book->pages[i] = book->pages[i + 1];
    }
}

/* @hack spelling: a mark line's size from a function, where adding
 * MARK_LINE_BYTES in mark_record_size would do */
static Sint32 mark_line_size(const struct book_mark_line *line)
{
    Sint32 size = sizeof(line->text);
    return size;
}
static Sint32 mark_record_size(const struct book_mark *mark)
{
    int i;
    /* @hack spelling: the size summed a field at a time, where one sum would
     * do */
    Sint32 size = sizeof(mark->title);
    size++;
    for (i = 0; i < mark->line_count; i++) {
        size += mark_line_size(mark->lines[i]);
    }
    return size;
}

/* @hack spelling: a page line's size from a function, where adding
 * 1 + LINE_SAVE_BYTES in page_record_size would do */
static Sint32 page_line_size(const struct book_line *line)
{
    /* @hack spelling: the size summed a field at a time, where one sum would
     * do */
    Sint32 size = 0;
    size++;
    size += LINE_SAVE_BYTES;
    return size;
}
static Sint32 page_record_size(const struct book_page *page)
{
    int i;
    Sint32 size = sizeof(page->title);
    /* @hack spelling: the fields' bytes added as 1, then as
     * PAGE_SAVE_FIELDS - 1, where adding PAGE_SAVE_FIELDS would do */
    size++;
    size += PAGE_SAVE_FIELDS - 1;
    for (i = 0; i < page->line_count; i++) {
        size += page_line_size(page->lines[i]);
    }
    size += sizeof(page->links[0].text);
    size += sizeof(page->links[1].text);
    return size;
}

Sint32 book_save_size(const struct book *book)
{
    int i;
    /* @hack spelling: the size summed a field at a time, where one sum would
     * do */
    Sint32 size = 0;
    size++;
    size += sizeof(book->title);
    size += sizeof(book->author);
    size++;
    size++;
    size++;
    /* @bug memory: a save writes one byte past its buffer. book_save_size
     * at 0x0c9c1ec2 leaves out the setting's byte. the editor allocates a
     * buffer of this size at 0x0c9c72dc, and book_write_save at 0x0c9c20c2
     * writes the byte past it */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: the size counts the setting's
     * byte */
    size++;
#endif
    for (i = 0; i < book->mark_count; i++) {
        size += mark_record_size(book->marks[i]);
    }
    for (i = 0; i < book->page_count; i++) {
        size += page_record_size(book->pages[i]);
    }
    return size;
}

#define WRITE_ARRAY(dst, array)                                                \
    do {                                                                       \
        memcpy(dst, array, sizeof(array));                                     \
        (dst) += sizeof(array);                                                \
    } while (0)

static Sint32 write_mark_line(const struct book_mark_line *line, Uint8 *dst)
{
    Uint8 *out = dst;
    WRITE_ARRAY(out, line->text);
    return out - dst;
}

static Sint32 write_mark(const struct book_mark *mark, Uint8 *dst)
{
    Uint8 *out = dst;
    int i;
    WRITE_ARRAY(out, mark->title);
    *out++ = mark->line_count;
    for (i = 0; i < mark->line_count; i++) {
        out += write_mark_line(mark->lines[i], out);
    }
    return out - dst;
}

static Sint32 write_page_line(const struct book_line *line, Uint8 *dst)
{
    Uint8 *out = dst;
    *out++ = line->code;
    memcpy(out, line->text, LINE_SAVE_BYTES);
    out += LINE_SAVE_BYTES;
    return out - dst;
}

static Sint32 write_page(const struct book_page *page, Uint8 *dst)
{
    Uint8 *out = dst;
    int i;
    WRITE_ARRAY(out, page->title);
    *out++ = page->options[0];
    *out++ = page->options[1];
    *out++ = page->options[2];
    *out++ = page->options[3];
    *out++ = page->options[4];
    *out++ = page->line_count;
    *out++ = page->style;
    *out++ = page->links[0].kind;
    *out++ = page->links[0].page;
    *out++ = page->links[1].kind;
    *out++ = page->links[1].page;
    for (i = 0; i < page->line_count; i++) {
        out += write_page_line(page->lines[i], out);
    }
    WRITE_ARRAY(out, page->links[0].text);
    WRITE_ARRAY(out, page->links[1].text);
    return out - dst;
}

Sint32 book_write_save(const struct book *book, Uint8 *dst)
{
    Uint8 *out = dst;
    int i;
    *out++ = SAVE_VERSION;
    WRITE_ARRAY(out, book->title);
    WRITE_ARRAY(out, book->author);
    *out++ = book->mark_count;
    *out++ = book->page_count;
    *out++ = book->first_page;
    *out++ = book->setting;
    for (i = 0; i < book->mark_count; i++) {
        out += write_mark(book->marks[i], out);
    }
    for (i = 0; i < book->page_count; i++) {
        out += write_page(book->pages[i], out);
    }
    return out - dst;
}

#define READ_ARRAY(array, src)                                                 \
    do {                                                                       \
        memcpy(array, src, sizeof(array));                                     \
        (src) += sizeof(array);                                                \
    } while (0)

static Sint32 read_mark_line(struct book_mark_line *line, const Uint8 *src)
{
    const Uint8 *in = src;
    READ_ARRAY(line->text, in);
    return in - src;
}

static Sint32 read_mark(struct book_mark *mark, const Uint8 *src)
{
    const Uint8 *in = src;
    int i;
    READ_ARRAY(mark->title, in);
    mark->line_count = *in++;
    for (i = 0; i < mark->line_count; i++) {
        mark->lines[i] = book_mark_line_create();
#ifdef NONMATCHING
        /* this side stops the read where book_mark_line_create makes no line.
         * it keeps the lines read so far, and returns 0 */
        if (!mark->lines[i]) {
            mark->line_count = i;
            return 0;
        }
#endif
        in += read_mark_line(mark->lines[i], in);
    }
    return in - src;
}

static Sint32 read_page_line(struct book_line *line, const Uint8 *src)
{
    const Uint8 *in = src;
    line->code = *in++;
    memcpy(line->text, in, LINE_SAVE_BYTES);
    in += LINE_SAVE_BYTES;
    return in - src;
}

static Sint32 read_page(struct book_page *page, const Uint8 *src)
{
    const Uint8 *in = src;
    int i;
    READ_ARRAY(page->title, in);
    page->options[0] = *in++;
    page->options[1] = *in++;
    page->options[2] = *in++;
    page->options[3] = *in++;
    page->options[4] = *in++;
    page->line_count = *in++;
    page->style = *in++;
    page->links[0].kind = *in++;
    page->links[0].page = *in++;
    page->links[1].kind = *in++;
    page->links[1].page = *in++;
    for (i = 0; i < page->line_count; i++) {
        page->lines[i] = book_line_create();
#ifdef NONMATCHING
        /* this side stops the read where book_line_create makes no line. it
         * keeps the lines read so far, and returns 0 */
        if (!page->lines[i]) {
            page->line_count = i;
            return 0;
        }
#endif
        in += read_page_line(page->lines[i], in);
    }
    READ_ARRAY(page->links[0].text, in);
    READ_ARRAY(page->links[1].text, in);
    return in - src;
}

#ifdef NONMATCHING
/* whether the save's counts fit the book's arrays and its records end
 * within BOOK_SAVE_BUFFER_SIZE bytes. the editor reads every save into a
 * zeroed buffer of that size. past a shorter save's end, the check reads
 * zeros */
static int book_records_fit(const Uint8 *src, int version)
{
    const size_t counts_offset = 1 + (2 * TITLE_BYTES);
    unsigned marks = src[counts_offset];
    unsigned pages = src[counts_offset + 1];
    size_t offset = counts_offset + 3 + version;
    unsigned i;
    if (marks > BOOK_MARKS || pages > BOOK_PAGES) {
        return FALSE;
    }
    for (i = 0; i < marks; i++) {
        unsigned lines;
        if (BOOK_SAVE_BUFFER_SIZE - offset < TITLE_BYTES + 1) {
            return FALSE;
        }
        lines = src[offset + TITLE_BYTES];
        offset += TITLE_BYTES + 1;
        if (lines > BOOK_MARK_LINES ||
            lines * MARK_LINE_BYTES > BOOK_SAVE_BUFFER_SIZE - offset) {
            return FALSE;
        }
        offset += lines * MARK_LINE_BYTES;
    }
    for (i = 0; i < pages; i++) {
        unsigned lines;
        size_t bytes;
        if (BOOK_SAVE_BUFFER_SIZE - offset < TITLE_BYTES + PAGE_SAVE_FIELDS) {
            return FALSE;
        }
        lines = src[offset + TITLE_BYTES + BOOK_PAGE_OPTIONS];
        bytes = TITLE_BYTES + PAGE_SAVE_FIELDS +
                (lines * (1 + LINE_SAVE_BYTES)) + 2 * LINK_TEXT_BYTES;
        if (lines > BOOK_PAGE_LINES || bytes > BOOK_SAVE_BUFFER_SIZE - offset) {
            return FALSE;
        }
        offset += bytes;
    }
    return TRUE;
}
#endif

Sint32 book_read_save(struct book *book, const Uint8 *src)
{
    const Uint8 *in = src;
    int version = *in++;
    int i;
    if (version > SAVE_VERSION) {
        return 0;
    }
#ifdef NONMATCHING
    /* this side reads nothing from a save whose records book_records_fit
     * refuses */
    if (!book_records_fit(src, version)) {
        return 0;
    }
#endif
    READ_ARRAY(book->title, in);
    READ_ARRAY(book->author, in);
    book->mark_count = *in++;
    book->page_count = *in++;
    book->first_page = *in++;
    if (version >= 1) {
        book->setting = *in++;
    } else {
        book->setting = 0;
    }
    for (i = 0; i < book->mark_count; i++) {
        book->marks[i] = book_mark_create();
#ifdef NONMATCHING
        /* this side frees what it read and returns 0 where a mark can't be made
         * or read */
        {
            Sint32 size = book->marks[i] ? read_mark(book->marks[i], in) : 0;
            if (!size) {
                book->mark_count = i + (book->marks[i] != NULL);
                book->page_count = 0;
                goto release_children;
            }
            in += size;
        }
#else
        in += read_mark(book->marks[i], in);
#endif
    }
    for (i = 0; i < book->page_count; i++) {
        book->pages[i] = book_page_create();
#ifdef NONMATCHING
        /* this side frees what it read and returns 0 where a page can't be made
         * or read */
        {
            Sint32 size = book->pages[i] ? read_page(book->pages[i], in) : 0;
            if (!size) {
                book->page_count = i + (book->pages[i] != NULL);
                goto release_children;
            }
            in += size;
        }
#else
        in += read_page(book->pages[i], in);
#endif
    }
    return in - src;
#ifdef NONMATCHING
/* where this side's failed read frees the book's marks and pages */
release_children:
    while (book->page_count) {
        book_page_destroy(book->pages[--book->page_count]);
    }
    while (book->mark_count) {
        book_mark_destroy(book->marks[--book->mark_count]);
    }
    return 0;
#endif
}

/* an export list starts with its count, a byte. each entry's offset from
 * the list's start follows, as a little-endian 16-bit word, then the
 * entries */
#define LIST_HEADER(count) (1 + ((count) * 2))
#define LIST_DATA(dst, count) ((dst) + ((count) * 2) + 1)
/* @hack spelling: the offset's two stores bare, where a do-while around
 * them would do */
#define WRITE_OFFSET(head, offset)                                             \
    *(head)++ = (Uint8)(offset);                                               \
    *(head)++ = (Uint8)((offset) >> 8)

static Sint32 export_mark_line(const struct book_mark_line *line, Uint8 *dst)
{
    return book_encode_text(dst, line->text, BOOK_MARK_LINE_CODES - 1) + 1;
}

static Sint32 export_mark(const struct book_mark *mark, Uint8 *dst)
{
    Uint8 *head = dst;
    Uint8 *data = LIST_DATA(dst, mark->line_count);
    int i;
    *head++ = mark->line_count;
    for (i = 0; i < mark->line_count; i++) {
        WRITE_OFFSET(head, data - dst);
        data += export_mark_line(mark->lines[i], data);
    }
    return data - dst;
}

static Sint32 export_marks(const struct book *book, Uint8 *dst)
{
    Uint8 *head = dst;
    Uint8 *data = LIST_DATA(dst, book->mark_count);
    int i;
    *head++ = book->mark_count;
    for (i = 0; i < book->mark_count; i++) {
        WRITE_OFFSET(head, data - dst);
        data += export_mark(book->marks[i], data);
    }
    return data - dst;
}

static Sint32 export_line(const struct book_line *line, Uint8 *dst)
{
    Uint8 *out = dst;
    *out++ = line->code;
    out += book_encode_text(out, line->text, BOOK_LINE_CODES - 1) + 1;
    return out - dst;
}

static Sint32 export_lines(const struct book_page *page, Uint8 *dst)
{
    Uint8 *head = dst;
    Uint8 *data = LIST_DATA(dst, page->line_count);
    int i;
    *head++ = page->line_count;
    for (i = 0; i < page->line_count; i++) {
        WRITE_OFFSET(head, data - dst);
        data += export_line(page->lines[i], data);
    }
    return data - dst;
}

static Sint32 export_links(const struct book_page *page, Uint8 *dst)
{
    Uint8 *head = dst;
    Uint8 *data = dst + LINKS_HEADER_BYTES;
    *head++ = page->style;
    *head++ = page->links[0].kind;
    switch (page->style) {
    case 2:
    case 3:
        data = head;
        break;
    default:
        *head++ = page->links[0].page;
        switch (page->style) {
        case 0:
        case 4:
        case 5:
            data = head;
            break;
        default:
            WRITE_OFFSET(head, data - dst);
            data +=
                book_encode_text(data, page->links[0].text, BOOK_LINK_CODES) +
                1;
            *head++ = page->links[1].kind;
            *head++ = page->links[1].page;
            WRITE_OFFSET(head, data - dst);
            data +=
                book_encode_text(data, page->links[1].text, BOOK_LINK_CODES) +
                1;
            break;
        }
        break;
    }
    return data - dst;
}

static Sint32 export_page(const struct book_page *page, Uint8 *dst)
{
    Uint8 *head = dst;
    Uint8 *data = dst + PAGE_HEADER_BYTES;
    Sint32 offset;
    *head++ = page->line_count && page->lines[0]->code == 0;
    *head++ = page->options[0];
    *head++ = page->options[1];
    *head++ = page->options[2];
    *head++ = page->options[3];
    *head++ = page->options[4];
    offset = data - dst;
    WRITE_OFFSET(head, offset);
    data += export_lines(page, data);
    offset = data - dst;
    WRITE_OFFSET(head, offset);
    data += export_links(page, data);
    return data - dst;
}

static Sint32 export_pages(const struct book *book, Uint8 *dst)
{
    Uint8 *head = dst;
    Uint8 *data = LIST_DATA(dst, book->page_count);
    int i;
    *head++ = book->page_count;
    for (i = 0; i < book->page_count; i++) {
        WRITE_OFFSET(head, data - dst);
        data += export_page(book->pages[i], data);
    }
    return data - dst;
}

Sint32 book_export(const struct book *book, Uint8 *dst)
{
    Uint8 *head = dst;
    Uint8 *data = dst + EXPORT_HEADER_BYTES;
    Sint32 offset;

    memset(head, 0, BLANK_WORD_BYTES);
    head += BLANK_WORD_BYTES;
    memset(head, 0, BLANK_WORD_BYTES);
    head += BLANK_WORD_BYTES;
    memset(head, 0, BLANK_FIELD_BYTES);
    head += BLANK_FIELD_BYTES;
    book_encode_text(head, book->title, BOOK_TITLE_CODES);
    head += BOOK_TITLE_CODES;
    offset = data - dst;
    WRITE_OFFSET(head, offset);
    data += export_marks(book, data);
    offset = data - dst;
    WRITE_OFFSET(head, offset);
    data += export_pages(book, data);
    *head++ = book->first_page;
    *head++ = book->setting;
    return data - dst;
}

static Sint32 mark_text_size(const struct book_mark_line *line)
{
    Uint8 text[BOOK_MARK_LINE_CODES];
    return book_encode_text(text, line->text, BOOK_MARK_LINE_CODES - 1) + 1;
}

static Sint32 mark_export_size(const struct book_mark *mark)
{
    Sint32 size = LIST_HEADER(mark->line_count);
    int i;
    for (i = 0; i < mark->line_count; i++) {
        size += mark_text_size(mark->lines[i]);
    }
    return size;
}

static Sint32 marks_export_size(const struct book *book)
{
    Sint32 size = LIST_HEADER(book->mark_count);
    int i;
    for (i = 0; i < book->mark_count; i++) {
        size += mark_export_size(book->marks[i]);
    }
    return size;
}

static Sint32 line_text_size(const struct book_line *line)
{
    Uint8 text[BOOK_LINE_CODES];
    /* @bug logic: an export's size falls a byte short for each page line.
     * lines_export_size at 0x0c9c28ec leaves out each line's code byte,
     * which export_line at 0x0c9c2524 writes before its text */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: the size counts the line's code
     * byte */
    return book_encode_text(text, line->text, BOOK_LINE_CODES - 1) + 2;
#else
    return book_encode_text(text, line->text, BOOK_LINE_CODES - 1) + 1;
#endif
}

static Sint32 lines_export_size(const struct book_page *page)
{
    Sint32 size = LIST_HEADER(page->line_count);
    int i;
    for (i = 0; i < page->line_count; i++) {
        size += line_text_size(page->lines[i]);
    }
    return size;
}

static Sint32 links_export_size(const struct book_page *page)
{
    Uint8 text[16];
    Sint32 size = LINKS_HEADER_BYTES;
    switch (page->style) {
    case 2:
    case 3:
        size = NO_LINK_BYTES;
        break;
    case 0:
    case 4:
    case 5:
        size = ONE_LINK_BYTES;
        break;
    default:
        size +=
            book_encode_text(text, page->links[0].text, BOOK_LINK_CODES) + 1;
        size +=
            book_encode_text(text, page->links[1].text, BOOK_LINK_CODES) + 1;
        break;
    }
    return size;
}

static Sint32 page_export_size(const struct book_page *page)
{
    Sint32 size = PAGE_HEADER_BYTES;
    size += lines_export_size(page);
    size += links_export_size(page);
    return size;
}

static Sint32 pages_export_size(const struct book *book)
{
    Sint32 size = LIST_HEADER(book->page_count);
    int i;
    for (i = 0; i < book->page_count; i++) {
        size += page_export_size(book->pages[i]);
    }
    return size;
}

Sint32 book_export_size(const struct book *book)
{
    Sint32 size = EXPORT_HEADER_BYTES;
    size += marks_export_size(book);
    size += pages_export_size(book);
    return size;
}
