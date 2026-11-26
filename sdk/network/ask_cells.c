#include "ask_cells.h"
#include "ask_dictionary.h"
#include "sg_xpt.h"

#define ASK_CELLS 1000
union ask_cell {
    union ask_cell *free;
    struct ask_ending ending;
    struct ask_match match;
    struct ask_phrase phrase;
    union ask_candidate candidate;
    struct ask_candidate_list list;
};
static union ask_cell pool[ASK_CELLS];
static union ask_cell *free_head;
static Sint32 free_count;
/* ask_ime.h declares first_init, which the image holds after the statics.
 * SHC places an uninitialized variable where it first meets the name, so the
 * include comes after them */
#include "ask_ime.h"
Sint32 first_init;

void init_univmem(void)
{
    union ask_cell *cell;
    if (!first_init && free_count == ASK_CELLS) {
        return;
    }
    free_head = pool;
    for (cell = pool; cell < &pool[ASK_CELLS - 1]; cell++) {
        cell->free = cell + 1;
    }
    cell->free = NULL;
    free_count = ASK_CELLS;
    first_init = 0;
}

void *alloc_mem(void)
{
    union ask_cell *cell;
    if (!free_head) {
        return NULL;
    }
    cell = free_head;
    free_head = cell->free;
    --free_count;
    return cell;
}
void free_mem(void *block)
{
    union ask_cell *cell = block;
    cell->free = free_head;
    free_head = cell;
    ++free_count;
}

void free_pwmemlist(struct ask_ending *ending)
{
    struct ask_ending *next;
    while (ending) {
        next = ending->next;
        free_mem(ending);
        ending = next;
    }
}
void free_chmemlist(struct ask_match *match)
{
    struct ask_match *next;
    Sint32 last = -1;
    while (match) {
        next = match->next;
        /* @hack redundant: the test of last against -1, which the test of
         * entry against -1 covers */
        if ((last == -1 || match->entry != last) && match->entry != -1) {
            dic_freeentid(match->entry);
        }
        last = match->entry;
        free_mem(match);
        match = next;
    }
}
void free_bsmemlist(struct ask_phrase *phrase)
{
    struct ask_phrase *next;
    while (phrase) {
        next = phrase->next;
        free_mem(phrase);
        phrase = next;
    }
}
void free_khmemlist(union ask_candidate *candidate)
{
    union ask_candidate *next;
    while (candidate) {
        next = candidate->head.next;
        free_mem(candidate);
        candidate = next;
    }
}
void free_klmemlist(struct ask_candidate_list *list)
{
    struct ask_candidate_list *next;
    while (list) {
        next = list->next;
        free_mem(list);
        list = next;
    }
}
