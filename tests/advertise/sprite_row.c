/* menu_sprite_row_draw for a row that ends on the last of its cells, as the
 * options' and the story select's titles do. the image reads the width of
 * the cell after the row's last, past the cells' end, and the non-matching
 * side reads no cell past the last. the cells here end where a page begins
 * that the host lets nothing read, so the matching side faults there */
#include <assert.h>
#include <sys/mman.h>
#include <unistd.h>

#include "modules/advertise/menu_draw.c"

#define CELLS 3
#define ROW_FIRST 1
#define ROW_COUNT 2

static Int drawn[ROW_COUNT];
static Float drawn_x[ROW_COUNT];
static int draws;

static void cell_draw(Int cell, Float scale, NJS_POINT3 *pos, Int attr)
{
    (void)scale;
    (void)attr;
    assert(draws < ROW_COUNT);
    drawn[draws] = cell;
    drawn_x[draws] = pos->x;
    draws++;
}

int main(void)
{
    static const NJS_POINT3 pos = {100.0F, 50.0F, -1.0F};
    size_t page = (size_t)sysconf(_SC_PAGESIZE);
    char *pages = mmap(NULL, 2 * page, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANON, -1, 0);
    NJS_TEXANIM *cells;
    struct menu_sprite_row row;

    assert(pages != MAP_FAILED);
    assert(mprotect(pages + page, page, PROT_NONE) == 0);
    cells = (NJS_TEXANIM *)(void *)(pages + page) - CELLS;
    cells[0].sx = 40;
    cells[1].sx = 31;
    cells[2].sx = 21;
    row.tanim = cells;
    row.first = ROW_FIRST;
    row.count = ROW_COUNT;
    row.pos = &pos;
    row.draw = cell_draw;
    menu_sprite_row_draw(&row);
    assert(draws == ROW_COUNT);
    assert(drawn[0] == ROW_FIRST && drawn_x[0] == pos.x);
    /* half of 31 and half of 21, each rounded down, and 1 */
    assert(drawn[1] == ROW_FIRST + 1 && drawn_x[1] == pos.x + 15 + 10 + 1);
    return 0;
}
