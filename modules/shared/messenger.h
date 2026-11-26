/* the messenger, MSGER in a stage's object list: a flying helper that shows
 * the stage's messages. a placed messenger waits until a player touches it.
 * it then rises and shows its message. a player can carry it. it says the
 * system's messages as it is picked up, carried, let go and hit. a hit stuns
 * it, and it then flies home. messenger_create summons one that shows a
 * message and then flies off for good */
#ifndef SHARED_MESSENGER_H
#define SHARED_MESSENGER_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* what messenger_create makes. a window of kind MESSAGE_WINDOW_HIDDEN plays
 * its pages' voices but never draws */
enum messenger_create_mode {
    MESSENGER_SAY_WINDOW, /* a window of kind MESSAGE_WINDOW_HIDDEN */
    MESSENGER_TALK_WINDOW, /* a window of kind MESSAGE_WINDOW_FADE */
    MESSENGER_SUMMON /* a messenger that shows a message and flies off */
};

/* build at buf each of the messenger's nine parts not yet built, if its
 * texture list is loaded. it returns buf past the builds. a call that
 * builds all nine also builds the face lights in white. messengers draw
 * from the builds once such a call returns, and from the models until then.
 * where objects_fog_off isn't 0, the call builds with the fog off, then
 * turns the fog on */
Uint8 *messenger_build_models(Uint8 *buf);
/* messenger_knudai_texts returns the system messages' texts from number 28 on,
 * which shared/knudai.h's flute stands show. messenger_upgrade_texts returns
 * those from number 50 on, which an upgrade stand indexes by its upgrade. each
 * returns null while the system's file holds fewer than 114 messages */
char **messenger_knudai_texts(void);
char **messenger_upgrade_texts(void);
/* free the stage's and the system's messages, where loaded */
void messenger_messages_free(void);
/* load the stage's messages from the table file name, and the system's from
 * mhsys. table_file_read_language_sparse reads each. it returns how many
 * messages the two files hold. where either file is loaded already, it reads
 * nothing and returns -1. where either read fails, it frees both and returns
 * -1.
 *
 * a message past its file's count, or one whose file isn't loaded, shows as
 * a stand-in text. a messenger reads through null for a message its file
 * holds without a text. so each system message a messenger says needs a
 * text. so does each stage message a placed or summoned messenger shows.
 * messenger_create's windows show the stand-in for those */
Sint32 messenger_messages_load(const char *name);
/* what a player has just met. a messenger the player carries answers the
 * event at its next step, and then sets it to MESSENGER_EVENT_NONE. a
 * messenger's first step carried clears its holder's event unanswered.
 * MESSENGER_EVENTS counts the events */
enum messenger_event {
    MESSENGER_EVENT_NONE,
    MESSENGER_EVENT_SPRING, /* a spring, SPRA or SPRB, launched it */
    MESSENGER_EVENT_GOAL_RING, /* a goal ring it touched cleared the stage */
    MESSENGER_EVENT_SAVE_POINT, /* it passed a save point */
    MESSENGER_EVENTS
};

/* set player's event. the call ignores a player of 2 or more, and an event
 * of MESSENGER_EVENTS or more. player must not be negative, unchecked */
void messenger_set_event(int player, enum messenger_event event);
/* the step an object list gives a messenger, which messenger_create calls
 * too. it gives task a record at task->extra, a body at task->force and a
 * collision record. it also gives task the messenger's own step, draw and
 * destructor, and a child task, its collider. a placed messenger does none
 * of this where placement_try_leave(task) returns TRUE. each of its steps
 * makes the same test, and a summoned one makes none.
 *
 * a placed messenger's collider attacks items, enemies and objects. it does
 * so only while the messenger moves fast and nobody carries it.
 *
 * task needs a work and a motion record. the work's pos places the
 * messenger, and its ang.y turns it. the messenger shows the stage message
 * numbered by scl.x, made positive and cut to an integer. a messenger whose
 * scl.y is 1 or more draws no trails and sheds no sparkles. without a motion
 * record, or room in the heap for the record, the call does nothing. it then
 * stays task's step.
 *
 * where the heap can't give the body, the messenger's step goes through a
 * null task->force. the step reads the collider's collision record
 * untested, so the heap must have room for it. where scl.y is below 1, the
 * step and the draw read the trails untested. the heap must then have room
 * for those too */
void messenger_init(struct task *task);
/* make a task of mode. it is a child of parent, or a root task of level 2
 * where parent is null. it returns the task, or null for any other mode or
 * where no task can be made.
 *
 * a window task shows the stage message whose number is message. where the
 * stage's messages hold no text of that number, it shows a stand-in. the task
 * destroys itself at its first step after its window closes, or where none
 * opened. destroying the task closes the window.
 *
 * a summoned messenger starts at pos, turned to ang_y, and follows player.
 * it rises, shows the stage message whose number is message, and then flies
 * off for good. only a summon reads pos, ang_y and player. where the heap
 * can't give the messenger its record, the call writes through null */
struct task *messenger_create(NJS_POINT3 *pos, Angle ang_y, int player,
                              int message, enum messenger_create_mode mode,
                              struct task *parent);

#endif
