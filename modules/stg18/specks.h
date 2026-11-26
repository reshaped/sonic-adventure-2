/* the specks, MizugomiExec: motes that float in Dry Lagoon's water, in
 * clouds of sprites, each cloud one task's */
#ifndef STG18_SPECKS_H
#define STG18_SPECKS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the specks' texture list, which names miu32_mi001 alone. the stage's
 * texture groups load it */
extern NJS_TEXLIST specks_texlist;

/* a cloud's modes, in its task's work. a cloud under MODE_LAGOON_SPECKS_HIDDEN
 * moves no speck and draws none. specks_create's caller sets that cloud's
 * mode. a column's or a side's cloud sets its own mode each step */
enum lagoon_specks_mode { MODE_LAGOON_SPECKS_SHOWN, MODE_LAGOON_SPECKS_HIDDEN };

/* make a cloud of count specks about the view, a task named MizugomiExec at
 * task level 1. it returns the task, or null where player's slot holds no
 * work or the heap runs out. where owner isn't null, *owner takes the task.
 * the task's destructor clears *owner where it still holds the task.
 *
 * the cloud is a ball of radius 100. its center lies halfway between player
 * 0's pos and camera 0's, or at player 0's without camera 0. a speck keeps its
 * place in the world as the ball moves, and a slow wave drifts it. a speck
 * that leaves the ball starts again near its edge. a step ends the task once
 * player's slot holds no work */
struct task *specks_create(int player, int count, struct task **owner);

/* make a cloud of count specks that rise in a column, as a child of parent.
 * the column stands where parent's pos is at the call. its specks lie within
 * 0.8 times parent's scl.x of its axis. they rise from there, and start over
 * at random once past parent's scl.y. it returns the task, or null where
 * player's slot holds no work or the heap runs out. *owner takes the task
 * where owner isn't null, as specks_create says.
 *
 * the cloud moves and shows only while camera 0's land_block_mask holds land
 * block 21, LOW_BLOCKS of stg18/lagoon_util.h. the current camera must also
 * lie below lagoon_water_level's height. the cloud hides while player 0
 * stands higher than 100 in either of two boxes. one takes x in [-80, 270)
 * and z in [860, 1210). the other takes x in [140, 270) and z in
 * [1210, 1450). player 0's work must not be null, unchecked */
struct task *specks_column_create(struct task *parent, int player, int count,
                                  struct task **owner);

/* make a cloud of count specks that drift, as a child of parent. they drift
 * from where parent's pos is at the call, along -z turned by parent's ang.y.
 * they start over at that pos at random once past parent's scl.y from it.
 * they lie within 0.8 times parent's scl.x of their line. it returns, and
 * shows its cloud, as specks_column_create does */
struct task *specks_side_create(struct task *parent, int player, int count,
                                struct task **owner);

#endif
