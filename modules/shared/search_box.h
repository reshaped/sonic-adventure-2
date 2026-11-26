/* the search boxes, SCHBOX and SGSCHBOX in a stage's object list. a player
 * near a box can ask it for a hint to the hunt's current piece. the box
 * opens, shows the hint, then rises and goes. after each hint the boxes rest
 * a while: none opens, and each fades. the pause screen shows the current
 * piece's hints given so far */
#ifndef SHARED_SEARCH_BOX_H
#define SHARED_SEARCH_BOX_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* compile the box's three models at buf, once to draw opaque and once to draw
 * faded. the faded builds draw at the opacity of a resting box. a build
 * already compiled, or whose texture list isn't loaded, is passed over. it
 * returns buf past the builds. where the last call compiled all six, a box
 * set up after it draws from them. it draws its body and top from them while
 * it is opaque or at rest.
 *
 * the call puts back _nj_control_3d_flag_ and the constant attributes, but
 * overwrites what draw_control_save and draw_attr_save last saved. it leaves
 * the constant material at 0 */
Uint8 *search_box_build(Uint8 *buf);

/* the object list's steps for SCHBOX and SGSCHBOX. under MISSION_RINGS or
 * MISSION_CHAO, each kills the box. else, where placement_try_leave(task)
 * returns TRUE, each sets up nothing. otherwise each gives task the box's
 * record in task->motion, and the box's step, draw and destructor. without room
 * for the record, task keeps this step and tries again at its next step. a box
 * set up starts the pause screen's hints, where they don't run yet.
 *
 * task needs a work, and no record in motion. pos places the box, and ang.y
 * turns it. a SCHBOX takes a collision record, whose shape every SCHBOX
 * shares. without room for the record, it goes on without one. an SGSCHBOX
 * takes none.
 *
 * a player within 50 of pos, as player_near measures, is near. where both
 * players are near, slot 0's player counts. while the box waits and no
 * message window is open, it offers the near player its action. the box must
 * lie ahead of that player. the player asks by taking the action. the
 * box takes no ask while a message window is open or the boxes rest. once
 * its hint's window ends, the box rises over 60 steps, then kills itself.
 *
 * an SGSCHBOX serves only the players whose bits are set in
 * game_state.sg_players, and draws only on their screens. it puts each of
 * them within 12 of pos back out to 12, but leaves one within 0.01 */
void search_box_init(struct task *task);
void search_box_sg_init(struct task *task);

/* the box's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *search_box_texlists[];

#endif
