/* the mech bosses' voice script: the lines a boss's pilot speaks as the
 * boss's power falls */
#ifndef SHARED_MECH_BOSS_VOICE_H
#define SHARED_MECH_BOSS_VOICE_H
#include <sg_xpt.h>

struct task;

/* create a task in task level 3 that speaks the three lines of boss's
 * script, and return it. it returns null where task_create fails. boss must be
 * one of enum mech_boss of shared/mech_boss.h, unchecked. power must stay
 * valid while the task lives. boss_task isn't read.
 *
 * the first line starts on the task's sixth step. the second line starts on a
 * step where *power, cut to a Sint8, is 1. the third starts on one where it is
 * 0. either starts even while the line before it plays. a line that plays to
 * its end leaves 16 steps of silence. the first line's silence ends in a
 * closing voice. the task ends itself 91 steps after the third line's
 * silence. it holds still while game_state.time_stopped or
 * game_state.mini_event_running isn't 0 */
struct task *mech_boss_voice_create(struct task *boss_task, Sint8 boss,
                                    Float *power);

#endif
