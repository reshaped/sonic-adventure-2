#include <assert.h>
#include <string.h>
#include <ninja.h>
#include "draw/color.h"
#include "player/walker_lock_assets.h"
#include "player/player.h"
#include "system/game_state.h"
#include "blast-draw.inc"

static int depth;
static int draws;
static Float factor;
static NJS_CNK_MODEL *selected;

Bool njPushMatrixEx(void)
{
    assert(depth == 0);
    depth = 1;
    return TRUE;
}

Bool njPopMatrixEx(void)
{
    assert(depth == 1);
    depth = 0;
    return TRUE;
}

void njScale(NJS_MATRIX *matrix, Float x, Float y, Float z)
{
    assert(matrix == NULL && depth == 1);
    assert(x == factor && y == x && z == x);
}

void njCnkEasyDrawModel(NJS_CNK_MODEL *model)
{
    assert(depth == 1 && model == selected && model->r == 30.0F);
    draws++;
}

int main(int argc, char **argv)
{
    int i, j;
    int available;

    assert(argc == 2);
    available = strcmp(argv[1], "optical") == 0;
    for (i = 0; i < 8; i++) {
        Float radius;
        Uint32 before[27];
        Uint32 *words;
        selected = walker_blast_parts[i];
        radius = selected->r;
        words = (Uint32 *)selected->vlist;
        assert(words);
        if (available) {
            assert(radius > 0.0F && (words[1] >> 16) == 27);
            factor = 20.0F / radius;
            for (j = 0; j < 27; j++) {
                before[j] = words[5 + j * 4];
            }
        } else {
            assert(radius == 0.0F);
        }
        lock_blast_draw_part(20.0F, 0.0F, i,
                             i < 4 ? PLAYER_EGGMAN_MECH : PLAYER_TAILS_MECH);
        assert(!depth && selected->r == radius);
        assert(draws == (available ? i + 1 : 0));
        if (available) {
            for (j = 0; j < 27; j++) {
                Uint32 alpha = (Uint32)BLAST_ALPHA_FULL << ALPHA_SHIFT;
                Uint32 expected = before[j];

                if (expected & COLOR_ALPHA) {
                    alpha =
                        expected & BLAST_FULL_ALPHA
                            ? alpha | BLAST_FULL_ALPHA
                            : (alpha >> 1) & (COLOR_ALPHA & ~BLAST_FULL_ALPHA);
                    expected = (expected & COLOR_RGB) | alpha;
                }
                assert(words[5 + j * 4] == expected);
            }
        }
    }
    return 0;
}
