/* the side shutter, SHELTH in Iron Gate's object list. its door slides shut
 * as a player comes near. four panels then light on it, and the door breaks
 * once attacks have hit each */
#ifndef STG11_SIDE_SHUTTER_H
#define STG11_SIDE_SHUTTER_H
#include <ninja.h>

struct task;

/* the side shutter's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *side_shutter_texlists[];

/* the object list's step for SHELTH. it makes the shutters' manager first, as
 * shutter_manager_create does. where task's placement is used, it then sets
 * SHUTTER_FLAG_BROKEN(n), as the break below does. it does nothing more where
 * task leaves at its placement's range. where the heap can't give its record,
 * it does nothing, and the next step tries again. otherwise it gives task the
 * side shutter's step, draw and destructor, and a collision record of kind
 * COLLISION_OBJECT. the record's five boxes are the sill, the door, the two
 * posts and the lintel. where the heap has no room for it, the side shutter
 * goes on without one.
 *
 * task needs a work, whose pos places the shutter and whose ang.y turns it. the
 * low byte of ang.x, modulo 5, is the shutter's number n. an open door stands
 * to the shutter's -x side. where bit 8 of ang.x is set, it stands to the +x
 * side. the door starts to shut once a player comes within scl.z + 100 of the
 * pos. it slides to half shut by scl.x + 0.05 a step. it waits there ang.z
 * steps past the step it arrives in. it then slides by scl.y + 0.05 a step.
 *
 * a placed side shutter numbered 0 plays the stage's opening voice as it
 * starts to shut. it does so only where SHUTTER_FLAG_OPENING_VOICE is clear,
 * and then sets the flag. a child of task, the stop, keeps a walker's lock-on
 * from passing the door. it stands from the end of the wait until the break. a
 * side shutter that its placement's state starts shut gets no stop. once shut,
 * the door shows four panels in turn. an attack can hit a panel only while the
 * nearest player stands on the door's +z side. the door breaks once attacks
 * have hit all four, which scores 20 points. a placed side shutter plays
 * shutter n's voice as it breaks, where SHUTTER_FLAG_BROKEN(n) is clear. it
 * then sets the flag. the side shutter leaves the fog as it is.
 *
 * the placement's state keeps the work's mode from one task to the next. the
 * destructor marks a broken side shutter's placement used. a side shutter
 * whose placement is used starts broken */
void side_shutter_init(struct task *task);

#endif
