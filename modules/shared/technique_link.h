/* the technique link, LINKLINK in a stage's object list: an unseen volume,
 * one numbered link of a row. a player who touches a row's links in order
 * earns the bonus of the last link touched */
#ifndef SHARED_TECHNIQUE_LINK_H
#define SHARED_TECHNIQUE_LINK_H

struct task;

/* the object list's step for LINKLINK. unless placement_try_leave(task)
 * returns TRUE, it gives task a COLLISION_OBJECT record of one shape, and the
 * link's own step, draw and destructor. task needs a work and a placement.
 * the heap must have room for the record, unchecked.
 *
 * the low 16 bits of ang.x are the link's time, in steps. bits 4 to 7 of
 * ang.y are its number in its row, and bits 0 to 3 its bonus. bonuses 0 to 10
 * give 0, 100, 200, 300, 400, 500, 600, 800, 1000, 1500 and 2000 points, and
 * 11 to 15 give none. the low bit of ang.z picks the shape: a box of half
 * sizes scl at 0, and a sphere of radius scl.x at 1. the call then clears
 * ang.x and the low bytes of ang.y and ang.z. the box, a SHAPE_BOX2, turns by
 * the ang.y and ang.z the call leaves.
 *
 * a stage runs one row at a time. a player's touch of link 0 while no row
 * runs starts a row, with link 0 as its latest link. a touch of the link
 * numbered one past the latest makes that link the latest. the link it
 * replaces ends at its next step. a latest link that nothing replaces ends
 * time + 1 steps after the touch that made it latest. the row ends with it,
 * and technique_score_start shows the link's points, unless 0. a latest link
 * destroyed any other way ends the row with no points.
 *
 * the latest link never leaves range. a link that ends at its own step sets
 * its placement's state to 1. each later task of that placement never
 * registers its collision record, so no touch reaches it, until
 * set_reactivate or a reset clears the state */
void technique_link_init(struct task *task);

#endif
