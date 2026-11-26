/* FIRE2 and FIRE3 in Sand Ocean's object list */
#ifndef STG21_FIRE_FIXTURE_H
#define STG21_FIRE_FIXTURE_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST chain_fire_texlist;
extern NJS_TEXLIST post_fire_texlist;
extern NJS_TEXLIST *chain_fire_texlists[];
extern NJS_TEXLIST *post_fire_texlists[];

/* the chain fire's flame model's own texture list. the draws bind
 * ocean_fire_texlist of shared/ocean_util.h instead */
extern NJS_TEXLIST chain_fire_flame_texlist;

/* the object list's steps for FIRE2 and FIRE3, as wall_fire_init is for WALL
 * FIRE. pos places the fire and ang.y turns it. 1 more than scl.x scales it.
 * its sound plays from 40 times that scale above pos. it plays while player 0
 * lies within 100 of that point. FIRE3's post is solid too */
void chain_fire_init(struct task *task);
void post_fire_init(struct task *task);
/* @hack type: the flame's model declared as an array of one, where the model
 * alone would do */
extern NJS_CNK_MODEL chain_fire_flame_model[1];
extern NJS_CNK_MODEL chain_fire_model;
extern NJS_CNK_MODEL post_fire_model;

#endif
