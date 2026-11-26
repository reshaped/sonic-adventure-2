/* draws of the land tables a stage holds beside its land. one draws
 * LandModChg's series of models, and one a table's entries */
#ifndef SHARED_LAND_DRAW_H
#define SHARED_LAND_DRAW_H

struct land_table;
struct task;

/* the draw land_model_change_create gives its task. each group draws
 * models[game_state.steps % model_count] at each of its entries. each draws
 * at its object's pos, turned by its object's ang. the draw first sets the
 * texture list in the task's extra, where that isn't null */
void land_model_change_draw(struct task *task);

/* draw land's entries with land's texlist, each at its object's pos and
 * turned by its object's ang. an entry draws where its blocks are 0, or share
 * a bit with the camera's game_state.land_blocks. its center, through the
 * current matrix, must also have a z above -radius and below land's clip.
 * LAND_FAR in its flags lifts the clip.
 *
 * the call pushes Ninja's matrix stack and pops none. so it returns one matrix
 * deeper, with the current matrix as it found it. the camera task's next draw
 * undoes that: its njSetCamera leaves the stack one matrix deep */
void land_table_draw(struct land_table *land);

#endif
