/* blasts: three shells of fire about a point, two spheres and a flat ring.
 * the shells grow and fade. every blast but a tiny one throws out black smoke
 * as it grows */
#ifndef SHARED_BLAST_H
#define SHARED_BLAST_H
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"

/* a blast's size, and whether it is raised. a raised blast lifts its two
 * spheres above its point, and leaves the ring there. they rise 10 when large
 * or huge, 5 when medium and 3 when small or tiny */
enum blast_kind {
    BLAST_LARGE_RAISED,
    BLAST_LARGE,
    BLAST_HUGE_RAISED,
    BLAST_HUGE,
    BLAST_SMALL_RAISED,
    BLAST_SMALL,
    BLAST_MEDIUM_RAISED,
    BLAST_MEDIUM,
    BLAST_TINY_RAISED,
    BLAST_TINY
};

/* load the blasts' textures from E_EXPTEX.PRS. a task named ManTex at task
 * level 0 holds them until it is destroyed. while that task lives, the call
 * does nothing. the heap must have room for the task, unchecked */
void blast_load_texture(void);
/* start a blast of kind at a copy of *pos. its task, named exec, runs at task
 * level 3. the call then loads the textures as blast_load_texture does. the
 * blast pauses while game_state.time_stopped isn't 0. its task ends when the
 * blast does. where the heap can't give the task or its record, nothing starts
 * and nothing loads. a kind outside blast_kind makes a task that draws nothing
 * and never ends */
void blast_create(enum blast_kind kind, NJS_POINT3 *pos);

/* the blasts' texture list, which blast_load_texture fills */
extern NJS_TEXLIST blast_texlist;
/* the second sphere's model, of radius 5, and its texture's scroll. a blast's
 * draw scrolls v by 5 for each of the blast's steps */
extern NJS_CNK_MODEL blast_model_1;
extern struct texture_anim blast_texture_anim;

#endif
