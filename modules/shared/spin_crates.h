/* IRON3, SOLID3 and MDCONTWOOD, the spinning crates of Meteor Herd and Mad
 * Space */
#ifndef SHARED_SPIN_CRATES_H
#define SHARED_SPIN_CRATES_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* build a crate's model at buffer, and return the first free byte past it.
 * the iron and wooden crates build their three kinds of piece too */
Uint8 *spin_iron_build(Uint8 *buffer);
Uint8 *spin_wood_build(Uint8 *buffer);
Uint8 *spin_solid_build(Uint8 *buffer);

/* the object list's steps for the crates. a crate turns by scl.x times
 * CRATE_TURN_X, _Y and _Z a step. the iron and wooden crates give 20 points
 * when they break. the iron crate breaks only for a character with its
 * upgrade. with a second player, spin_iron_init makes the task a wooden
 * crate */
void spin_iron_init(struct task *task);
void spin_wood_init(struct task *task);
void spin_solid_init(struct task *task);

extern NJS_CNK_MODEL spin_iron_model;
extern NJS_CNK_OBJECT spin_iron_object;
extern NJS_CNK_MODEL spin_solid_model;
extern NJS_CNK_OBJECT spin_solid_object;
extern NJS_CNK_MODEL spin_wood_model;
extern NJS_CNK_OBJECT spin_wood_object;

extern NJS_TEXLIST spin_iron_texlist;
extern NJS_TEXLIST spin_wood_texlist;
extern NJS_TEXLIST spin_solid_texlist;
extern NJS_TEXLIST *spin_iron_texlists[];
extern NJS_TEXLIST *spin_wood_texlists[];
extern NJS_TEXLIST *spin_solid_texlists[];

#endif
