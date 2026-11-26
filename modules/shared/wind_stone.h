/* the stones a wind throws up. a stone spins as it rises, and can break into
 * pieces. it fades out once its y passes its wind's scl.y */
#ifndef SHARED_WIND_STONE_H
#define SHARED_WIND_STONE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

struct task;

/* a stone's owner is a task with a work. WIND_STONES is the first of its
 * stones, which its force holds. WIND_STONE_COUNT counts the stones, and
 * WIND_STONE_PIECE_COUNT the pieces among them. the owner starts force at null
 * and both counts at 0 */
#define WIND_STONES(owner) ((struct wind_stone *)(owner)->force.p)
#define WIND_STONE_COUNT(owner) ((owner)->work->timer)
#define WIND_STONE_PIECE_COUNT(owner) ((owner)->unk_3c.l)
/* the most whole stones an owner keeps, and the most pieces */
#define WIND_STONE_WHOLE_MAX 50
#define WIND_STONE_PIECE_MAX 50

/* a whole stone, or a piece that broke off one. a whole stone's kind runs from
 * 1, the largest, to 5. kind sets a stone's lift, its speed limit and its spin,
 * and a piece's model too */
struct wind_stone {
    Sint32 kind;
    Float alpha; /* 1, falling to 0 as the stone fades out */
    NJS_POINT3 pos;
    NJS_VECTOR spd;
    NJS_VECTOR acc;
    struct angles ang;
    struct angles spin; /* what ang gains each step */
    NJS_VECTOR scl;
    Bool is_piece;
    /* TRUE once last_matrix holds a whole stone's matrix of the step before. a
     * piece's stays FALSE, and its last_matrix is never written */
    Bool has_last;
    NJS_MATRIX *matrix; /* the matrix of pos and ang */
    NJS_MATRIX *last_matrix;
    struct wind_stone *prev;
    struct wind_stone *next;
    struct task *owner;
};

/* make a whole stone of a random kind and size at pos, at the head of owner's
 * list. return the stone, or null where owner has WIND_STONE_WHOLE_MAX whole
 * stones, or 100 stones of both kinds, already. it returns null too where the
 * heap has no room for the stone and its two matrices */
struct wind_stone *wind_stone_create(struct task *owner, NJS_POINT3 *pos);
/* take stone out of its owner's list and counts, and free it with its
 * matrices. a null stone does nothing */
void wind_stone_free(struct wind_stone *stone);
/* move stone a step, which may free it. a stone frees itself once it has faded
 * out. a whole stone of kind 1 can break into five pieces, or three in a
 * two-player game. it then frees itself. a whole stone of kind 1 or 2 can shed
 * a piece of kind 5 and stay whole. it sheds none in a two-player game. a stone
 * breaks only above a y of 35, where in_view_box of shared/canyon_util.h passes
 * its pos */
void wind_stone_step(struct wind_stone *stone);
/* draw stone, faded by its alpha */
void wind_stone_draw(struct wind_stone *stone);

#endif
