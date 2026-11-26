/* Final Chase's meteors: FIREBALL, METEO_MOVE, METEO_MOVE_F and METEO_SET in
 * its object list, and those the background throws */
#ifndef STG40_METEOR_H
#define STG40_METEOR_H
#include <ninja.h>

struct task;

/* the objects' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *meteor_texlists[];

/* the object list's step for FIREBALL. task needs a work, whose pos places
 * the fireball and whose ang turns it. the fireball spins, sheds sparks, and
 * hurts a player it touches. the touch breaks it. so does the 61st step after
 * a player comes within scl.x + 30 of pos. the break plays sound 12 of the
 * stage's sound list, throws twenty pieces and ends the task. where the heap
 * can't give the fireball's collision record, the break reads through null */
void fireball_init(struct task *task);
/* the first step of a meteor the background throws. task needs a work,
 * whose pos is where the meteor starts. its scl is the meteor's speed, added
 * to pos each step. its timer counts the meteor's steps from 0, and the task
 * ends once the timer passes ang.z */
void meteor_fall_init(struct task *task);
/* the object list's steps for METEO_MOVE and METEO_MOVE_F, and for METEO_SET.
 * task needs a work, whose pos places the meteor. scl.x picks one of four
 * rocks: its whole part, taken unsigned, modulo 4. meteor_move_init's meteor
 * starts at random angles and spins. scl.y and scl.z add degrees a step to its
 * spin about x and about y. the moving meteor pushes players, enemies and
 * objects. meteor_set_init's meteor keeps its ang, and hurts a player it
 * touches */
void meteor_move_init(struct task *task);
void meteor_set_init(struct task *task);

#endif
