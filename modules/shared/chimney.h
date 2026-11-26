/* the chimney, CHIMNEY in a stage's object list. now and then it shows ten
 * frames of its model in turn, and an explosion above it */
#ifndef SHARED_CHIMNEY_H
#define SHARED_CHIMNEY_H
#include <ninja.h>

struct task;

/* the chimney's texture lists and the explosion's, for texture_load_shared,
 * each ended by null */
extern NJS_TEXLIST *chimney_texlists[];
extern NJS_TEXLIST *chimney_blast_texlists[];

/* the object list's step for CHIMNEY. task needs a work, whose pos places
 * the chimney and whose ang.y turns it. the models draw at a scale of 1 more
 * than scl.x.
 *
 * each step between blasts starts one with a chance of 1 in 300. a blast
 * moves to its next frame on each step whose game_state.steps is even, for
 * ten frames. its explosion faces the screen, 300 times the scale above
 * pos */
void chimney_init(struct task *task);

/* the explosion has a model and a texture list for each frame of a blast.
 * the chimney has one of each at rest, then one for each frame */
extern NJS_CNK_MODEL chimney_blast_model_0;
extern NJS_CNK_MODEL chimney_blast_model_1;
extern NJS_CNK_MODEL chimney_blast_model_2;
extern NJS_CNK_MODEL chimney_blast_model_3;
extern NJS_CNK_MODEL chimney_blast_model_4;
extern NJS_CNK_MODEL chimney_blast_model_5;
extern NJS_CNK_MODEL chimney_blast_model_6;
extern NJS_CNK_MODEL chimney_blast_model_7;
extern NJS_CNK_MODEL chimney_blast_model_8;
extern NJS_CNK_MODEL chimney_blast_model_9;
extern NJS_TEXLIST chimney_blast_texlist_0;
extern NJS_TEXLIST chimney_blast_texlist_1;
extern NJS_TEXLIST chimney_blast_texlist_2;
extern NJS_TEXLIST chimney_blast_texlist_3;
extern NJS_TEXLIST chimney_blast_texlist_4;
extern NJS_TEXLIST chimney_blast_texlist_5;
extern NJS_TEXLIST chimney_blast_texlist_6;
extern NJS_TEXLIST chimney_blast_texlist_7;
extern NJS_TEXLIST chimney_blast_texlist_8;
extern NJS_TEXLIST chimney_blast_texlist_9;
extern NJS_CNK_MODEL chimney_model_0;
extern NJS_CNK_MODEL chimney_model_1;
extern NJS_CNK_MODEL chimney_model_2;
extern NJS_CNK_MODEL chimney_model_3;
extern NJS_CNK_MODEL chimney_model_4;
extern NJS_CNK_MODEL chimney_model_5;
extern NJS_CNK_MODEL chimney_model_6;
extern NJS_CNK_MODEL chimney_model_7;
extern NJS_CNK_MODEL chimney_model_8;
extern NJS_CNK_MODEL chimney_model_9;
extern NJS_CNK_MODEL chimney_model_10;
extern NJS_TEXLIST chimney_texlist_0;
extern NJS_TEXLIST chimney_texlist_1;
extern NJS_TEXLIST chimney_texlist_2;
extern NJS_TEXLIST chimney_texlist_3;
extern NJS_TEXLIST chimney_texlist_4;
extern NJS_TEXLIST chimney_texlist_5;
extern NJS_TEXLIST chimney_texlist_6;
extern NJS_TEXLIST chimney_texlist_7;
extern NJS_TEXLIST chimney_texlist_8;
extern NJS_TEXLIST chimney_texlist_9;
extern NJS_TEXLIST chimney_texlist_10;

#endif
