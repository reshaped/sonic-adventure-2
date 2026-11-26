/* Exercise unify_khmem's frees of the candidates before the one selected, on
 * the non-matching side. */
#include <assert.h>
#include "ask_cells.c"
#include "ask_candidates.h"
#include "ask_phrases.h"
#include "ask_session.h"

struct ask_position hchar[ASK_POSITIONS];
int func_mode;

/* a candidate of cells cells, each linked by head.next, as kh_followed reads
 * a continuation cell too. its last cell links to next */
static union ask_candidate *candidate(int cells, union ask_candidate *next)
{
    union ask_candidate *head = NULL;
    union ask_candidate *cell;
    while (cells--) {
        cell = alloc_mem();
        assert(cell);
        cell->head.flags = head ? ASK_CANDIDATE_CONTINUES : 0;
        cell->head.next = head ? head : next;
        head = cell;
    }
    return head;
}

int main(void)
{
    union ask_candidate *selected;
    init_univmem();
    selected = candidate(1, candidate(2, NULL));
    hchar[0].candidates = candidate(2, candidate(3, selected));
    assert(free_count == ASK_CELLS - 8);
    top_kh = hchar[0].candidates;
    gun_nkh = 2;
    sel_job = 1;
    unify_khmem(0, 1);
    assert(hchar[0].candidates == selected && !selected->head.next);
    assert(free_count == ASK_CELLS - 1);
    return 0;
}
