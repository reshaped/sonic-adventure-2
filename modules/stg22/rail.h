/* Crazy Gadget's three transport rails, each of which carries a player from
 * its intake to its exit */
#ifndef STG22_RAIL_H
#define STG22_RAIL_H
#include <sg_xpt.h>

struct task;

/* start a task for each of gadget_rails' rails, as a child of parent. a
 * rail's first child is its exit, which holds the rail's intake and markers
 * as its children. a player within 15 of an idle rail's first point boards
 * it. a rail waits after each ride before it takes another. the heap must
 * have room for each intake's collision record and for its cover's. the
 * intake makes its cover on a later step, and writes through both records
 * untested */
void gadget_rails_create(struct task *parent);
/* start a ride for player on rail, and return TRUE, where rail is idle and
 * waits no more. otherwise change nothing, and return FALSE */
Bool gadget_rail_board(struct task *rail, int player);
#endif
