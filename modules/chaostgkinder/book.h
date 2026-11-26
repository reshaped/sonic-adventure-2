/* the book the editor makes. book_write_save stores it as a VMU save.
 * book_export lays it out for the VMU's executable file, its text in
 * book_encode_text's codes */
#ifndef CHAOSTGKINDER_BOOK_H
#define CHAOSTGKINDER_BOOK_H
#include <sg_xpt.h>

#define BOOK_MARKS 4
#define BOOK_MARK_LINES 15
#define BOOK_PAGES 63
#define BOOK_PAGE_LINES 16
/* the codes of a title, a mark's line, a page's line and a link's text */
#define BOOK_TITLE_CODES 16
#define BOOK_MARK_LINE_CODES 32
#define BOOK_LINE_CODES 64
#define BOOK_LINK_CODES 14
#define BOOK_PAGE_OPTIONS 5
#define BOOK_PAGE_LINKS 2
/* a page's first four options are ratings, from 0 to 4 */
#define BOOK_DEFAULT_RATING 2
/* the size of the buffer the editor reads a save into */
#define BOOK_SAVE_BUFFER_SIZE 0x4000
/* the bytes an export has room for, which the editor's meter counts down
 * from */
#define BOOK_EXPORT_BYTES 0x1000

struct book_mark_line {
    Uint16 text[BOOK_MARK_LINE_CODES];
};

struct book_mark {
    Uint16 title[BOOK_TITLE_CODES];
    Sint32 line_count;
    struct book_mark_line *lines[BOOK_MARK_LINES];
};

struct book_line {
    Sint32 code;
    Uint16 text[BOOK_LINE_CODES];
};

/* page is the index of the page the link leads to */
struct book_link {
    Sint32 kind, page;
    Uint16 text[BOOK_LINK_CODES];
};

struct book_page {
    Uint16 title[BOOK_TITLE_CODES];
    Sint32 options[BOOK_PAGE_OPTIONS];
    Sint32 line_count;
    struct book_line *lines[BOOK_PAGE_LINES];
    Sint32 style;
    struct book_link links[BOOK_PAGE_LINKS];
};

struct book {
    Uint16 title[BOOK_TITLE_CODES], author[BOOK_TITLE_CODES];
    Sint32 mark_count;
    struct book_mark *marks[BOOK_MARKS];
    Sint32 page_count;
    struct book_page *pages[BOOK_PAGES];
    Sint32 first_page, setting;
};

/* each create returns a zeroed record, or NULL. each destroy frees its
 * record and the records it holds. book_page_create starts a page's ratings
 * at BOOK_DEFAULT_RATING. in SHC's code, a failed book_page_create writes
 * them through NULL */
struct book_mark_line *book_mark_line_create(void);
void book_mark_line_destroy(struct book_mark_line *line);
struct book_mark *book_mark_create(void);
void book_mark_destroy(struct book_mark *mark);
/* add a line to mark while it has fewer than BOOK_MARK_LINES. in SHC's
 * code, a line whose allocation fails is added as NULL */
void book_mark_add_line(struct book_mark *mark);
struct book_line *book_line_create(void);
void book_line_destroy(struct book_line *line);
struct book_page *book_page_create(void);
void book_page_destroy(struct book_page *page);
/* add a line to page as book_mark_add_line does, below BOOK_PAGE_LINES */
void book_page_add_line(struct book_page *page);
struct book *book_create(void);
void book_destroy(struct book *book);
/* add a new mark or page to book, and return it. return NULL where book
 * holds BOOK_MARKS or BOOK_PAGES already, or the allocation fails */
struct book_mark *book_add_mark(struct book *book);
struct book_page *book_add_page(struct book *book);
/* destroy the mark or page at index, and move those after it down one. a
 * link's page stays as it was, so it may then name another page */
void book_remove_mark(struct book *book, int index);
void book_remove_page(struct book *book, int index);
/* the size book_write_save writes. SHC's code leaves out the setting's
 * byte */
Sint32 book_save_size(const struct book *book);
/* write book to dst as a save of version 1, and return its size. dst needs
 * book_save_size(book) bytes, and in SHC's code one more */
Sint32 book_write_save(const struct book *book, Uint8 *dst);
/* read the save at src, BOOK_SAVE_BUFFER_SIZE bytes, into book, a book from
 * book_create. return the save's size, or 0 for a save past version 1,
 * which reads nothing. in SHC's code the save's counts must fit the book's
 * arrays. a count past BOOK_MARKS, BOOK_PAGES, BOOK_MARK_LINES or
 * BOOK_PAGE_LINES writes past them. the GCC side returns 0 for such a save,
 * and for one whose records pass src's end. it also returns 0 where an
 * allocation fails, and then keeps no marks or pages */
Sint32 book_read_save(struct book *book, const Uint8 *src);
/* the size book_export writes. SHC's code leaves out each page line's code
 * byte */
Sint32 book_export_size(const struct book *book);
/* write book to dst in the export's layout, and return its size. dst needs
 * book_export_size(book) bytes, and in SHC's code one more for each page
 * line */
Sint32 book_export(const struct book *book, Uint8 *dst);
#endif
