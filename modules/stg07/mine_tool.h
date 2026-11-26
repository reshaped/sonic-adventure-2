/* the mine tool, MITOOL in Aquatic Mine's object list. its wheel turns, and
 * carries the players who stand on it */
#ifndef STG07_MINE_TOOL_H
#define STG07_MINE_TOOL_H
#include <ninja.h>

struct task;

/* the texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *mine_tool_texlists[];

/* the object list's step for MITOOL. it does nothing where task leaves at its
 * placement's range. where the heap can't give its record, it does nothing,
 * and the next step tries again. otherwise it gives task the tool's step, draw
 * and destructor. it puts a copy of its collision object in the collision
 * world. it makes the wheel's surface a child task. where task_create_child
 * fails, the wheel turns all the same, and carries no one.
 *
 * task needs a work, whose pos places the tool. its ang turns the tool in the
 * order z, y, x. each later step tests the range as the first did. the
 * wheel's surface turns with the wheel. it moves players 0 and 1 by its force
 * records */
void mine_tool_init(struct task *task);

#endif
