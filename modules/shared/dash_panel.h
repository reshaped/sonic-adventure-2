/* the dash panel, KASOKU in a stage's object list. a player who touches it
 * on the ground is sent off along its angles */
#ifndef SHARED_DASH_PANEL_H
#define SHARED_DASH_PANEL_H
#include <sg_xpt.h>

#include <ninja.h>

#include "draw/texture_anim.h"

struct task;

extern const char dash_panel_texture_filenames[29];

/* the textures of the panel's base and of its animated model */
extern NJS_TEXLIST dash_panel_texlist;
extern NJS_TEXLIST dash_panel_anim_texlist;

/* the panel's base, and the model whose textures dash_panel_texture_anim
 * animates */
extern NJS_CNK_MODEL dash_panel_model, dash_panel_anim_model;
extern NJS_CNK_OBJECT dash_panel_object, dash_panel_anim_object;
extern struct texture_anim dash_panel_texture_anim;

/* compile the panel's base model at buf for njDirectDrawObject, unless it is
 * compiled already or dash_panel_texlist isn't loaded. it returns buf past
 * what it compiled. a panel set up after the build draws the base from it */
Uint8 *dash_panel_build(Uint8 *buf);
/* the object list's step for KASOKU. unless placement_try_leave(task)
 * returns TRUE, the first step sets the panel up. that step needs the heap's
 * room for a collision record, unchecked.
 *
 * task needs a work. scl.x is the speed the panel sends a player off at.
 * scl.y rounded up is n, the steps that player's stick is then locked for. at
 * 0 or below, scl.x takes 14 and scl.y 60.
 *
 * the panel sends a player who touches it on the ground, by
 * player_launch_speed of player/player.h. the player takes the panel's
 * angles and a speed of scl.x along its own x. a touch by any task but player
 * 1's counts as player 0's. the panel then refuses that player for n - 1
 * steps, unless it sends the other first.
 *
 * the panel keeps its state in task->motion and task->extra. its destructor
 * clears motion, force and extra. task must hold no record in them, since
 * none would be freed */
void dash_panel_step(struct task *task);

#endif
