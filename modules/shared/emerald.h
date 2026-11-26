/* emerald pieces, the tasks named ObjectEmerald, and the sign named
 * EmeraldGetSign shown over the player who takes one. a piece is drawn and
 * can be taken only while stage/hunt.h's hunt holds it */
#ifndef SHARED_EMERALD_H
#define SHARED_EMERALD_H
#include <ninja.h>
#include <sg_xpt.h>

struct path;
struct task;

/* the pieces, by their index in a look's arrays */
#define EMERALD_PIECES 3
#define EMERALD_PIECE_B 0
#define EMERALD_PIECE_A 1
#define EMERALD_PIECE_C 2

/* a child piece's work tick. a shown piece with this tick is drawn with its
 * parent's ang.x and ang.z. the parent must have a work, unchecked. the draws
 * of EMERALD_KIND_A_2P_UNDGND, EMERALD_KIND_A_PATHMOVE and
 * EMERALD_KIND_INENEMY ignore the tick. a piece emerald_create_plain_child
 * makes reads it whatever its kind */
#define EMERALD_TICK_PARENT_ANGLES 0xff

/* a look flag that lights a piece by lights[7] of draw/light.h. each draw
 * sets lights[7] to lights[4] brightened by a pulse. without the flag,
 * lights[4] lights the piece */
#define EMERALD_LOOK_PULSE 0x1

/* how the pieces and signs are drawn. each array holds an entry for each
 * EMERALD_PIECE_ */
struct emerald_look {
    Uint32 flags;
    NJS_CNK_MODEL *models[EMERALD_PIECES];
    /* a model drawn over the piece's, added to what lies under it. its
     * brightness rises and falls every 30 steps. null draws none */
    NJS_CNK_MODEL *glow_models[EMERALD_PIECES];
    NJS_TEXLIST *texlists[EMERALD_PIECES];
    Angle spin; /* a piece's turn about y on each step while it is drawn */
    /* while bob_speed isn't 0, a piece or a sign bobs about its place. its
     * height there is bob_height * njSin(bob_speed * game_state.steps) */
    Float bob_height;
    Angle bob_speed;
};

/* the look the pieces and signs are drawn with, which a stage may change */
extern struct emerald_look emerald_look;
/* the pieces' three texture lists, then the ring sparkle's and the
 * sparkle's, ended by null, for texture_load_shared */
extern NJS_TEXLIST *emerald_texlists[];

/* set emerald_look to the look the pieces start with */
void emerald_reset_look(void);
/* give the pieces of EMERALD_KIND_A_PATHMOVE a table of count paths, or with
 * emerald_clear_paths take it away. there is no table before the first
 * emerald_set_paths. a piece set up without a table is never drawn or taken.
 * on each step, a piece set up with one takes the path its work's scl.x
 * picks, modulo count. the table stays the caller's, and must stay set while
 * such a piece lives. count must be above 0, unchecked.
 *
 * each path must have flags 0, so its points are NJS_POINT3s. a piece goes
 * round it as if it were PATH_CLOSED. a piece on a path with other flags
 * takes a place that nothing set. each path must have two neighbouring
 * points whose njDistanceP2P isn't 0. on a path without them, a moving
 * piece's set-up and step loop forever. a placed piece's path must hold 128
 * points or fewer. its placement keeps the piece's segment in a Sint8 */
void emerald_set_paths(struct path **paths, int count);
void emerald_clear_paths(void);
/* make a piece of kind and id at pos. the piece is a task at level 1 named
 * ObjectEmerald, whose first step is emerald_init. kind is an EMERALD_KIND_
 * value, taken modulo EMERALD_KINDS. returns the task, or null where
 * task_create fails */
struct task *emerald_create(NJS_POINT3 *pos, Uint8 kind, Uint8 id);
/* show or hide a piece. a hidden piece isn't drawn and can't be taken. an
 * EMERALD_KIND_INENEMY piece is drawn and can be taken either way. showing a
 * piece the hunt didn't hold makes it ask the hunt again */
void emerald_set_shown(struct task *task, int shown);
/* make a shown piece of kind and id, as emerald_create takes them, at pos as
 * parent's child. the piece takes EMERALD_KIND_B_NORMAL's step, draw and
 * destructor, whatever its kind, but is drawn as its kind's piece. it is set
 * up at once. its parent may move it. while the hunt holds the piece, it
 * tells the hunt its place on each step. returns the task, or null where
 * task_create_child fails. the heap must have room for the piece's collision
 * record, unchecked */
struct task *emerald_create_plain_child(struct task *parent, NJS_POINT3 *pos,
                                        Uint8 kind, Uint8 id);
/* a piece's first step, which an object list or emerald_create gives it.
 * bits 8 to 15 of the work's ang.x hold the kind, and bits 0 to 7 the id. the
 * kind is taken modulo EMERALD_KINDS. it gives task its kind's step, draw
 * and destructor, and sets the piece up. task needs a work, and the heap must
 * have room for the piece's collision record, unchecked */
void emerald_init(struct task *task);
/* show piece, an EMERALD_PIECE_ value, over player's first collision shape.
 * the sign is a task at level 3 named EmeraldGetSign. it grows and turns,
 * then spins and sheds sparkles, and ends in a burst of them. it ends early
 * on a step that finds player_work[player] null. returns the task, or null
 * where task_create fails. player must be 0 or 1: for any other player the
 * call never returns. the player's work must have a collision record,
 * unchecked */
struct task *emerald_sign_create(int player, int piece);

#endif
