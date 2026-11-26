/* the Chao Lobby: its area's step, and its exit from the Chao world, which
 * asks the player first */
#ifndef CHAOSTGLOBBY_LOBBY_H
#define CHAOSTGLOBBY_LOBBY_H
#include <ninja.h>

struct task;

/* the answers to the exit's question, in the order it lists them */
enum lobby_answer { LOBBY_ANSWER_YES, LOBBY_ANSWER_NO };

/* the step of the task CHAO.PRS creates for the lobby. the task must hold a
 * work. the step loads the lobby's land and places its exit and the Neutral
 * Garden's gate. it also places the Hero Garden's, the Dark Garden's and the
 * kindergarten's gate where each is open. where chao_take_bad_file returns
 * TRUE, it shows lobby_bad_file_message's warning too. the task's next step
 * greets the player by a messenger where the lobby is the first area.
 * destroying the task releases the lobby's textures and messages, and stops
 * its land */
void chaostglobby_step(struct task *task);

/* the exit's task, at pos and turned by yaw, or null when task_create fails.
 * the exit holds a player who steps onto its middle, and asks
 * lobby_exit_prompt's question. a player who answers LOBBY_ANSWER_YES leaves
 * the Chao world by chao_leave. one who answers LOBBY_ANSWER_NO is let go.
 * the exit asks again only once the player has left its reach, with the last
 * answer selected. it starts as if it had just asked. its beams face the
 * camera only where pos is the origin and yaw is 0 */
struct task *lobby_exit_create(const NJS_POINT3 *pos, Angle yaw);

/* show the warning that the garden file was damaged, and return its task, or
 * null when task_create fails. the warning fades in, stays and fades out,
 * and its task then destroys itself. where its text sprite can't be made,
 * the task draws and is destroyed through null */
struct task *lobby_bad_file_message(void);

/* open the exit's question with LOBBY_ANSWER_YES selected, and return its
 * task. it returns null while a question is open. it also returns null where
 * the task, its record or a text sprite can't be made */
struct task *lobby_exit_prompt(void);
/* close the open question. its task is destroyed at its next step. without
 * a question it does nothing */
void lobby_close_prompt(void);
/* select choice in the open question. without a question it does nothing */
void lobby_select_answer(enum lobby_answer choice);

#endif
