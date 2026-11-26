/* a foothold's shatter where the heap can't give its pieces, and where it
 * can. without them, the call kills the pieces' task and makes no piece. with
 * them, the one piece's smoke takes no speed */
#include <assert.h>
#include <stdlib.h>

#include "boss_golem/footholds.c"

NJS_CNK_OBJECT golem_foothold_pieces[1];
NJS_TEXLIST golem_foothold_pieces_texlist;

static NJS_CNK_OBJECT piece;
static struct task_work foothold_work;
static struct task foothold;
static struct task pieces_task;
static struct foothold_piece pieces[PIECES];
static Bool refuse_pieces;
static int kills, smokes;

/* every piece that can smoke does */
int rand(void)
{
    return 0;
}

struct task *task_create(Uint16 flags, int level,
                         void (*step)(struct task *task), const char *name)
{
    return &pieces_task;
}

void task_kill(struct task *task)
{
    assert(task == &pieces_task);
    kills++;
}

void *syCalloc(Uint32 count, Uint32 size)
{
    assert(count == PIECES && size == sizeof(struct foothold_piece));
    return refuse_pieces ? NULL : pieces;
}

void golem_point_turn_y(Float x, Float z, Angle ang, NJS_POINT3 *point)
{
    point->x = x;
    point->y = 0.0F;
    point->z = z;
}

void golem_dust_smoke_behind(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                             Float scale)
{
    assert(spd->x == 0.0F && spd->y == 0.0F && spd->z == 0.0F);
    smokes++;
}

/* shatter the foothold where the heap refuses the pieces */
static void check_without_pieces(void)
{
    refuse_pieces = TRUE;
    foothold_shatter(&foothold);
    assert(kills == 1 && pieces_task.draw == NULL);
}

/* shatter the foothold into its one piece, which smokes */
static void check_drift(void)
{
    kills = 0;
    refuse_pieces = FALSE;
    foothold_shatter(&foothold);
    assert(kills == 0 && LIVE_PIECES(&pieces_task) == 1 && smokes == 1);
}

int main(void)
{
    foothold.work = &foothold_work;
    golem_foothold_pieces[0].child = &piece;
    check_without_pieces();
    check_drift();
    return 0;
}
