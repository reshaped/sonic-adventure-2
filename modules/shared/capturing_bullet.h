/* the capturing bullet, a shot an enemy fires. it holds the player it
 * catches, then breaks into pieces */
#ifndef SHARED_CAPTURING_BULLET_H
#define SHARED_CAPTURING_BULLET_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* in the work flags of a bullet's parent. the bullet sets it as it catches a
 * player, and never clears it */
#define CAPTURING_BULLET_CAUGHT 0x400

/* load the bullets' textures from E_CAPTEX.PRS. a task at task level 0 then
 * holds them, and releases them when it is destroyed. while that task lives,
 * the call does nothing. the heap must have room for the task, unchecked */
void capturing_bullet_load_texture(void);

/* fire a bullet from pos at spd, and play its sound at pos. the bullet is a
 * child of parent, or a task at task level 3 where parent is null. the call
 * loads the textures as capturing_bullet_load_texture does. where the heap
 * can't give the bullet's task, the call does nothing. the heap must have room
 * for the bullet's collision record and enemy_work, unchecked. a parent must
 * have a work, unchecked.
 *
 * the bullet flies at spd, and ends once placement_try_leave finds it out of
 * range. a ceiling zeroes spd.y, and a floor or a wall bursts the bullet. spd
 * must not be zero, since the bullet divides by its length. a burst divides by
 * that length too, after a ceiling or a floor zeroes spd.y. so a bullet that
 * can meet one needs an x or a z in spd.
 *
 * the bullet is armed once it has taken six steps outside a time stop. an armed
 * bullet catches a player whose record is the latest it touched in a step. the
 * catch sends the player PLAYER_REQUEST_CAPTURE, and sets
 * CAPTURING_BULLET_CAUGHT in the work flags of parent, if any. the bullet holds
 * the player while the player's work stays in PLAYER_MODE_CAUGHT. it then
 * breaks into up to eight pieces. the caught player must not be PLAYER_EGGMAN,
 * unchecked: the hold's width is then undefined. an armed bullet that catches
 * no one bursts where its work's flags hold WORK_HURT, WORK_UNK_1 or
 * WORK_UNK_8. a floor or a wall can still burst the bullet in the step of a
 * catch. it then holds nothing.
 *
 * while game_state.time_stopped isn't 0, a flying or bursting bullet holds
 * still, and so do the pieces */
void capturing_bullet_fire(struct task *parent, NJS_VECTOR *spd,
                           NJS_POINT3 *pos);

#endif
