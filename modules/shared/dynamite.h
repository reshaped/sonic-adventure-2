/* the dynamite: DYNAMITE in a stage's object list, a charge that blows up when
 * a player hits it. other objects count the charges of an id that are left */
#ifndef SHARED_DYNAMITE_H
#define SHARED_DYNAMITE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the steps over which a blown charge's body fades to red */
#define DYNAMITE_FADE_STEPS 20

/* the charge's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *dynamite_texlists[];
/* how many charges have blown up since a stage last set it to 0. each blast
 * adds 1, and no charge sets it back */
extern Sint32 dynamite_blasts;

/* build the charge's model at buf, unless it is built already or its texture
 * list isn't loaded. returns buf past what it built. a whole charge draws the
 * build once there is one */
Uint8 *dynamite_build(Uint8 *buf);

/* the object list's step for DYNAMITE. task needs a work. the low byte of its
 * ang.x is the charge's id. bits 8 to 15 of ang.x turn the charge about x.
 * each run of dynamite_init calls blast_load_texture first.
 *
 * a charge with a set file entry joins the table that dynamite_count reads. the
 * table holds an entry once, and takes none while its 80 slots are full. a
 * charge stays in the table while it is out of range.
 *
 * without room for the charge's 8-byte force record, dynamite_init runs again
 * on task's next step. the heap must then have room for a collision record and
 * an enemy_work, unchecked.
 *
 * a hit by player 0 or 1, or a touch by a lock-on blast, blows the charge up.
 * the blast adds 1 to dynamite_blasts, and the charge scores 100 through
 * score_add. it leaves the table and stops its placement. it ends 21 steps
 * later. a whole charge whose placement has been stopped leaves the table as it
 * ends. it scores 100 then too */
void dynamite_init(struct task *task);

/* how many charges of id the table holds. each start and restart of the stage
 * empties the table */
int dynamite_count(int id);

#endif
