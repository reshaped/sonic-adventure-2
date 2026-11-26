/* the records of ASK's conversion lists: endings, words, phrases,
 * candidates and lists of candidates. each record takes one 20-byte cell of a
 * pool that every list shares */
#ifndef ASK_CELLS_H
#define ASK_CELLS_H
#include <sg_xpt.h>

struct ask_match {
    Sint16 length;
    Uint8 kind, form;
    Sint32 entry; /* the dictionary entry, or -1 for none */
    Sint16 frequency;
    struct ask_match *next;
};
struct ask_phrase {
    Sint16 length;
    Uint8 kind, form;
    struct ask_match *word;
    Uint16 score, preference;
    struct ask_phrase *next;
};
/* a chain of endings that can follow a word, length bytes of the reading.
 * kind and form are its first ending's. word_kind and word_form are its
 * last's, which a phrase that ends with the chain takes */
struct ask_ending {
    Sint16 length;
    Uint8 kind, form, word_kind, word_form;
    struct ask_ending *next;
};
/* a candidate's text lies in its head cell and the continuation cells after
 * it. the head holds up to 4 bytes of it, and each continuation up to 14.
 * each piece ends with a NUL. next links every cell: a candidate's own, then
 * the next candidate's head. a head's length is the reading bytes the
 * candidate stands for, and form its word's form */
union ask_candidate {
    struct {
        Uint8 flags, text[5], length, form;
        struct ask_match *match;
        Uint16 id;
        Sint16 priority;
        union ask_candidate *next;
    } head;
    struct {
        Uint8 flags, text[15];
        union ask_candidate *next;
    } continuation;
};
/* a candidate cell's flags */
enum {
    ASK_CANDIDATE_CONTINUES = 1, /* another cell of the candidate follows */
    ASK_CANDIDATE_TAIL = 2, /* a continuation cell */
    ASK_CANDIDATE_RAW = 0x80 /* the head of the reading kept as it is */
};
/* a candidate's id where no dictionary word stands behind it */
#define ASK_NO_ID 0xffff
struct ask_candidate_list {
    struct ask_phrase *phrase;
    Uint16 priority;
    union ask_candidate *candidates;
    struct ask_candidate_list *next;
};
/* free every cell, whatever list holds it */
void init_univmem(void);
/* a free cell, or NULL when every cell is taken */
void *alloc_mem(void);
void free_mem(void *block);
/* each frees the cells of a list, by its next links, and not what its
 * records point to. free_chmemlist also releases each match's dictionary
 * entry, once for a run of matches that hold the same entry */
void free_pwmemlist(struct ask_ending *ending);
void free_chmemlist(struct ask_match *match);
void free_bsmemlist(struct ask_phrase *phrase);
void free_khmemlist(union ask_candidate *candidate);
void free_klmemlist(struct ask_candidate_list *list);
#endif
