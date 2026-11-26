/* the dynamite charges that stg23/tower.c fixes to each tower support */
#ifndef STG23_CHARGE_H
#define STG23_CHARGE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* build the charge's model at buffer, unless it is built already or its
 * texture list isn't loaded. returns buffer past what it built. until a charge
 * blows up, it draws the build, once there is one */
Uint8 *base_charge_build(Uint8 *buffer);
/* attach a charge to parent as its child. each step, the charge sits at offset
 * from the pos of parent's work, in world axes. yaw turns it about y. parent
 * needs a work.
 *
 * a hit by player 0 or 1, or a touch by a lock-on blast, blows the charge up.
 * the blast sets bit index of the submode in parent's work. the submode is a
 * Sint8, so index must be below 8. the charge is killed 51 steps after its
 * blast.
 *
 * without room for the charge's task, the call does nothing. with room, the
 * heap must also hold a collision record and an enemy_work, unchecked */
void base_charge_attach(struct task *parent, NJS_POINT3 *offset, Angle yaw,
                        int index);

#endif
