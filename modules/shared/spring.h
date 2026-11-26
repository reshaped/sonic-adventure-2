/* the springs, SPRA and SPRB in a stage's object list: a base, a coil and a
 * top. a touch launches a player along the spring's y axis, and the spring
 * bobs after */
#ifndef SHARED_SPRING_H
#define SHARED_SPRING_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

extern const char spring_texture_filenames[14];

/* the springs' texture list and their nodes. each base's child is its coil,
 * and the coil's child its top */
extern NJS_TEXLIST spring_texlist;
extern NJS_TEXLIST spring_b_texlist;
extern NJS_CNK_MODEL spring_a_base_model;
extern NJS_CNK_OBJECT spring_a_base_object;
extern NJS_CNK_OBJECT spring_a_coil_object;
extern NJS_CNK_OBJECT spring_a_top_object;
extern NJS_CNK_MODEL spring_b_base_model;
extern NJS_CNK_OBJECT spring_b_base_object;
extern NJS_CNK_OBJECT spring_b_coil_object;
extern NJS_CNK_OBJECT spring_b_top_object;

/* build at buf each of the six nodes that isn't built yet, if its texture list
 * is loaded. it returns buf past the builds. a spring set up after a call that
 * built all six draws from the builds. where objects_fog_off isn't 0, the call
 * builds with the fog off, then turns the fog on */
Uint8 *spring_build(Uint8 *buf);

/* the object list's steps for SPRA and SPRB. task needs a work, and the heap
 * must have room for a collision record, unchecked. a spring keeps Floats in
 * task->motion and task->extra, and spring b in task->force too. the three
 * must hold no record, and the destructor clears them.
 *
 * the work's pos places the spring. its ang turns the spring by njRotateZ,
 * njRotateX and njRotateY, in that order. scl.y adds to the launch's speed of
 * 5. spring b's scl.x, cut to an integer, is the stick_lock it gives the
 * player it launches. a scl.x below 0 gives 0. the first step overwrites
 * scl.z, and spring a's overwrites scl.x too. while the timer isn't 0, the
 * spring draws at 1 / timer of its size. each step that waits for a touch
 * takes 1 from a timer that isn't 0.
 *
 * a touch by player 0 or 1 launches the player along the spring's y axis.
 * spring a launches by player_bounce, and spring b by player_launch. the
 * launch shakes the player's pad and gives the player MESSENGER_EVENT_SPRING */
void spring_a_step(struct task *task);
void spring_b_step(struct task *task);

#endif
