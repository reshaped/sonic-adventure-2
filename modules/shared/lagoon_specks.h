/* the specks, MizugomiExec: clouds of motes that float in a stage's water. Dry
 * Lagoon and Knuckles's route in Cannon's Core make them. each call returns the
 * cloud's task, or null where player has no work or an allocation fails. where
 * owner isn't null, *owner takes the task. the task's destructor sets *owner
 * back to null while *owner still holds it */
#ifndef SHARED_LAGOON_SPECKS_H
#define SHARED_LAGOON_SPECKS_H

#include <ninja.h>

struct task;
extern NJS_TEXLIST specks_texlist;

/* a cloud's modes, in its task's work. a cloud under MODE_LAGOON_SPECKS_HIDDEN
 * draws no speck, and the view cloud moves none either. the stage sets the
 * view cloud's mode. a column or side cloud starts shown */
enum lagoon_specks_mode { MODE_LAGOON_SPECKS_SHOWN, MODE_LAGOON_SPECKS_HIDDEN };

/* a cloud about the point halfway between player 0 and camera 0. while camera 0
 * has no view, the point is player 0's */
struct task *specks_create(int player, int count, struct task **owner);

struct task *specks_side_create(struct task *parent, int player, int count,
                                struct task **owner);
struct task *specks_column_create(struct task *parent, int player, int count,
                                  struct task **owner);

#endif
