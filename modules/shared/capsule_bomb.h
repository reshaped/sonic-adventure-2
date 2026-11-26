/* the capsule bomb, CAPSULEBOMB in a stage's object list. the bomb rides
 * in a capsule along a track. a hit bursts the capsule, and the bomb then
 * blows up */
#ifndef SHARED_CAPSULE_BOMB_H
#define SHARED_CAPSULE_BOMB_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the bomb's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *capsule_bomb_texlists[];

/* compile the bomb's seven models at buf, and return buf past them. each
 * compiles once, and only while its texture list is loaded. a bomb draws the
 * compiled models only where one call compiled all seven before its init. its
 * shards take each model that is compiled */
Uint8 *capsule_bomb_build(Uint8 *buf);
/* the object list's step for CAPSULEBOMB. task needs a work. the bomb ends
 * where placement_try_leave returns TRUE. it gives task a weak point at the
 * work's pos, and a collision shape about the capsule. outside Lost Colony,
 * it also gives task a shape at each end of the track.
 *
 * the track stands upright. its middle is the work's pos.y, and half its
 * length is the size of scl.y. the bomb rides at scl.x a step, up where scl.x
 * is above 0 and down otherwise. it rides from 20 past one end to 20 past the
 * other. it waits there one step more than the low byte of ang.x. it then
 * starts again from the first end. it has no weak point from the far end of
 * the track until it starts again. in Lost Colony, it has none while it waits.
 * ang.y turns the bomb about y, and the bit 0x100 of ang.x draws a cap at each
 * end.
 *
 * a hit on the weak point bursts the capsule, and the bomb blows up 22 steps
 * later. the blast's damage grows to a radius of 80. 182 steps after the
 * blast, a new bomb starts 20 below the track's lower end. a task that ends in
 * that wait sets its placement's state to 1, which the bomb never clears. each
 * later task of the placement then starts in the wait. set_reset,
 * set_reset_but_unk_20 and set_reactivate clear the state */
void capsule_bomb_init(struct task *task);

/* the object list's step for Cannon's Core's capsule bomb, which all five of
 * its sections link. it differs from capsule_bomb_init in two ways. its weak
 * point stays until the bomb takes a hit or waits at the track's far end. the
 * bomb draws its models uncompiled */
void capsule_bomb_relay_init(struct task *task);

#endif
