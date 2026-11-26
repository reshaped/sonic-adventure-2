/* rings: the rings an object list places, and those that scatter from a
 * player. one task draws all but the SG rings from a chain of nodes */
#ifndef SHARED_RING_H
#define SHARED_RING_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"

struct task;

/* the bits of a ring_link's flags. a ring sets RING_LEFT as it leaves
 * range. a ring sets RING_SPENT once it is taken, or once a scattered ring
 * replaces it. a group makes no new ring for a link with RING_SPENT set */
#define RING_LEFT 0x1
#define RING_SPENT 0x2

/* what joins a ring to the group that made it */
struct ring_link {
    Sint32 flags;
    struct task *ring; /* the ring's task while it lives, or null */
};

/* the object list's steps for RING and SGRING. task needs a work, whose pos
 * places the ring. from its first step the ring keeps its state in
 * task->force, from the Shinobi heap. the heap must have room for it,
 * unchecked.
 *
 * a ring placed with a scl.x of -1 keeps to the floor below it while near
 * the camera. where that floor moves too far at once, a scattered ring falls
 * in the ring's place. a taken ring sparkles. a ring taken or replaced kills
 * its task on its next step. a ring with a placement kills it through
 * placement_kill. a player with PLAYER_MISSED set takes a ring and gains
 * nothing from it.
 *
 * while a player has PLAYER_MAGNETIC_BARRIER set, every ring stops turning.
 * each also loses its floor and its shadow for good. the nearer such player
 * pulls a ring that comes within 50 of it. a ring once pulled stays pulled. a
 * ring that two such players stand equally far from is spared all of this.
 *
 * ring_step gives the ring a collision record, and a player who touches the
 * ring takes it. a ring placed with 1 in ang.x's low 16 bits has no floor
 * and no shadow.
 *
 * sg_ring_step gives the ring no collision record. only a player whose bit
 * of game_state.sg_players is set can take the ring. the player takes it
 * once the player's first collision shape is within 14 of it. the ring draws
 * only on such a player's screen, unless game_state.unk_09 isn't 0 */
void ring_step(struct task *task);
void sg_ring_step(struct task *task);
/* the step of a scattered ring. task needs a work and an extra, as
 * task_create gives with TASK_WORK | TASK_EXTRA. the ring leaves the work's
 * pos along the heading of the work's ang.y. it falls toward world_gravity
 * and bounces. a player who touches the ring takes it, but only once it has
 * flown 47 steps. after 240 steps of flight the ring lies still and blinks.
 * 180 steps later it is gone */
void scattered_ring_step(struct task *task);

/* ring_attach sets link->ring to ring, and gives ring the link. ring must be
 * a task of ring_step or sg_ring_step. ring_attach does nothing to a ring that
 * has stepped, or that has been attached before.
 *
 * ring_detach takes the link from ring, and sets the link's ring to null. it
 * clears the link's RING_LEFT, and leaves its RING_SPENT. it does nothing to
 * a ring without a link */
void ring_attach(struct task *ring, struct ring_link *link);
void ring_detach(struct task *ring);

/* the player in slot loses every ring it holds. up to 20 of them scatter
 * from its pos, and a sound plays. ring_scatter calls player_miss for a player
 * who holds no ring, unless its character is PLAYER_EGGMAN_MECH or
 * PLAYER_TAILS_MECH. slot must hold a player */
void ring_scatter(int slot);
/* scatter count rings from (x, y, z). unlike ring_scatter, it takes no rings
 * from a player and plays no sound. no code in the game's images calls
 * ring_scatter_at */
void ring_scatter_at(Float x, Float y, Float z, int count);

extern const char ring_texture_filenames[33];

/* the ring's far, middle and near models, and its shadow's, which ring_build
 * builds */
extern NJS_TEXLIST ring_texlist;
/* @hack type: ring_model, ring_object, ring_middle_object and
 * ring_near_object declared as arrays of one, where single ones would do */
extern NJS_CNK_MODEL ring_model[1];
extern NJS_CNK_OBJECT ring_object[1];
extern NJS_TEXLIST ring_middle_texlist;
extern NJS_CNK_OBJECT ring_middle_object[1];
extern NJS_TEXLIST ring_near_texlist;
extern NJS_CNK_OBJECT ring_near_object[1];
extern NJS_TEXLIST ring_shadow_texlist;
extern NJS_CNK_MODEL ring_shadow_model;
extern NJS_CNK_OBJECT ring_shadow_object;
/* the shadow's scale along x and along z */
extern Float ring_shadow_scale_x;
extern Float ring_shadow_scale_z;
extern struct collision_shape ring_collision_shape;
/* the count the next ring to look for a floor starts from, 0 to 8. the
 * ring's count drops by 1 a step, and the ring looks on the step it reaches
 * -1. each ring that follows its floor moves the phase on by 1, and
 * so does each scattered ring. it wraps from 8 to 0, so that the rings look on
 * different steps */
extern Sint32 ring_floor_phase;
/* the node the chain hangs from while game_state.builds_drawn is 0 */
extern NJS_CNK_OBJECT ring_chain_root;
/* the node each entry of the chain starts as a copy of */
extern NJS_CNK_OBJECT ring_entry_template;
/* draw the ring's far model at the current matrix, with the fog off where
 * objects_fog_off isn't 0. it leaves ring_texlist set */
void ring_draw_model(void);
/* build the ring's three models and its shadow at buf, for the chain to draw
 * from. it returns buf past the builds. the ring's builds take ring_texlist,
 * and the shadow's ring_shadow_texlist. where objects_fog_off isn't 0, the
 * call builds with the fog off, then turns the fog on. once one call has
 * built all four, later calls build nothing */
Uint8 *ring_build(Uint8 *buf);
/* set ring_floor_phase to 0. the next ring that looks for a floor then looks
 * first on its second step. no code in the game's images calls
 * ring_reset_floor_phase */
void ring_reset_floor_phase(void);

#endif
