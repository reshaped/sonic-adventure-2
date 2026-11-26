/* Cosmic Wall's set objects: the object list's steps for them, and their
 * texture lists */
#ifndef STG43_OBJECTS_H
#define STG43_OBJECTS_H
#include <ninja.h>

struct task;

/* SIGNEND, stg40/sign_end.h's sign with one model and no sound. it bobs up to 5
 * above and below pos. its child turns about its z by the whole part of scl.x
 * in quarter turns */
void sign_end_init(struct task *task);
extern NJS_TEXLIST *sign_end_texlists[];

/* GATE, a gate facing ang.y, whose panel scrolls 8 a step. scl.x of 1 or more
 * ties it to the second drill ride and reverses the scroll. its panel lights
 * while its ride is under way */
void cosmic_gate_init(struct task *task);
extern NJS_TEXLIST *cosmic_gate_texlists[];

/* INDCTR, a sign facing ang.y, whose panel scrolls 16 along v a step. scl.x of
 * 1 or more picks the second of its two signs */
void cosmic_indicator_init(struct task *task);
extern NJS_TEXLIST *cosmic_indicator_texlists[];

/* VULCAN, a gun that breaks into nine pieces at its fourth attack. breaking it
 * scores 20 */
void cosmic_vulcan_init(struct task *task);
extern NJS_TEXLIST *cosmic_vulcan_texlists[];

/* LOCKON, a target that bobs scl.x above and below pos. scl.y of 1 or more
 * gives it two weak points, one either side of pos along x. otherwise it has
 * one, at pos. it breaks when it has none left */
void cosmic_lockon_init(struct task *task);
extern NJS_TEXLIST *cosmic_lockon_texlists[];

/* LAZER, a gun that raises its barrel and fires a beam. it waits the whole
 * part of scl.y steps first. losing its weak point breaks it. its placement
 * keeps it broken when it comes back in range */
void cosmic_laser_init(struct task *task);
extern NJS_TEXLIST *cosmic_laser_texlists[];

/* METEOBULLET, a flaming meteor, and METEOBULLET2, a rock. each breaks when a
 * player touches it, or at its fourth separate attack. a rock whose scl.x is 1
 * or more is the small one, and breaks at its second */
void cosmic_flaming_meteor_init(struct task *task);
void cosmic_rock_init(struct task *task);
extern NJS_TEXLIST *cosmic_meteor_texlists[];

#endif
