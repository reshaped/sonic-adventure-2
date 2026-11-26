/* the guidance, GUIDANCE in City Escape's object list: a box that steers
 * players on boards toward a point. it also speeds them up */
#ifndef STG13_GUIDANCE_H
#define STG13_GUIDANCE_H

struct task;

/* the object list's step for GUIDANCE. it gives task the guidance's own step,
 * and a draw that draws nothing.
 *
 * task needs a work. the box stands at pos, turned by ang.y about y. its half
 * sizes along x, y and z are twice the low bytes of ang.x, ang.y and ang.z.
 * the guidance steers toward pos + scl.
 *
 * each step acts on each player in slot 0 or 1 inside the box. the player's
 * mode must lie from PLAYER_MODE_SONIC_BOARD to PLAYER_MODE_SONIC_BOARD_CRASH.
 * the step reads the mode alone, not the character. it turns a player in
 * PLAYER_MODE_SONIC_BOARD_CRASH toward the point by up to 0xc00 a step. it
 * turns a player in another of those modes so only where player_read_stick
 * returns FALSE. the record's spd.x must then lie within 2 of 0 too. a player
 * then heading within 0x1800 of the point gains 0.1 of spd.x a step, up to 1.2.
 * the step kills task once placement_try_leave(task) returns TRUE */
void guidance_init(struct task *task);

#endif
