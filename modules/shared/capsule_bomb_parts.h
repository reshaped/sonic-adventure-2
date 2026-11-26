/* the models, collision shapes and timings that both capsule bombs share:
 * capsule_bomb.c's and relay_bomb.c's */
#ifndef SHARED_CAPSULE_BOMB_PARTS_H
#define SHARED_CAPSULE_BOMB_PARTS_H
#include <ninja.h>

#include "collision/collision.h"

/* the capsule and the caps draw with capsule_bomb_texlist. the halves and
 * the two kinds of shards each have their own list */
extern NJS_TEXLIST capsule_bomb_texlist;
extern NJS_CNK_OBJECT capsule_bomb_top_cap;
extern NJS_CNK_OBJECT capsule_bomb_bottom_cap;
extern NJS_TEXLIST capsule_bomb_half_texlist;
/* @hack type: the capsule's node and the halves' declared as arrays, where
 * the nodes alone would do */
extern NJS_CNK_OBJECT capsule_bomb_object[];
extern NJS_CNK_OBJECT capsule_bomb_upper_half[];
extern NJS_CNK_OBJECT capsule_bomb_lower_half[];
extern NJS_TEXLIST capsule_bomb_shard_a_texlist;
/* @hack type: the shards' models declared as arrays, where the models alone
 * would do */
extern NJS_CNK_MODEL capsule_bomb_shard_a_model[];
extern NJS_CNK_OBJECT capsule_bomb_shard_a;
extern NJS_TEXLIST capsule_bomb_shard_b_texlist;
extern NJS_CNK_MODEL capsule_bomb_shard_b_model[];
extern NJS_CNK_OBJECT capsule_bomb_shard_b;

/* the work's mode. the bomb rides its track until a hit on its weak point
 * bursts the capsule. it then burns its fuse, and is gone until a new bomb
 * rides */
enum capsule_bomb_mode { MODE_RIDE, MODE_FUSE, MODE_GONE };

/* the bomb rides a vertical track. TRACK_MIDDLE holds the track's middle,
 * which the init copies from pos.y. HALF_TRACK, the absolute value of scl.y, is
 * half the track's length. the bomb rides RIDE_PAST beyond each end */
#define TRACK_MIDDLE(work) ((work)->scl.z)
#define HALF_TRACK(work) njAbs((work)->scl.y)
#define RIDE_PAST 20.0F
/* the bomb's rise along its track at each step, a fall where below 0 */
#define RIDE_SPEED(work) ((work)->scl.x)
/* the low byte of ang.x, one less than the steps the bomb waits at its ride's
 * end. the bomb then starts again from the other end */
#define WAIT_STEPS(work) ((Uint8)(work)->ang.x)
/* the bit of ang.x that draws the caps at the track's two ends */
#define SHOW_CAPS 0x100
/* a hurt to the weak point sets bit WEAK_POINT_INDEX of the work's submode.
 * a submode other than 0 bursts the capsule */
#define WEAK_POINT_INDEX 0

/* the bomb's collision shapes, in their order in a source's table: the capsule,
 * then the caps. the init centers the caps on the track's ends, in world
 * coordinates */
#define BOMB_SHAPES 3
enum capsule_bomb_shape { CAPSULE, TOP_CAP, BOTTOM_CAP };
/* BOMB_SHAPE spells a shape of a bomb's tables, with a center of 0 and no
 * size along z. collision_init gives each task its own copy of a table. the
 * init sets the caps' centers in that copy. damage_step grows the blast's
 * radius in its own */
#define BOMB_PUSH PUSH_SOLID(PUSH_ALL_BUT_PICKUPS)
#define BLAST_HIT (SHAPE_ATTACK_LEVEL(2) | SHAPE_HURTS_ALL)
#define BOMB_SHAPE(form, push, hit, flags, radius, half_length)                \
    {0,      form,        push, hit, flags,    {0.0F, 0.0F, 0.0F},             \
     radius, half_length, 0.0F, 0,   {0, 0, 0}}

/* the capsule's burst: a column of SHARD_ROWS shards at each of SHARD_TURNS
 * bearings, an eighth turn apart. kinds a and b alternate by row and by
 * bearing. a shard starts SHARD_OUT from the bomb's axis. the rows lie
 * SHARD_ROW_HEIGHT apart, the lowest SHARD_LOW below the bomb. a shard moves 1
 * a step outward. it rises SHARD_RISE a step for each row above
 * SHARD_MIDDLE_ROW, and falls as much for each row below. it rises up to
 * SHARD_LIFT more at random. each axis of its speed is then scaled by a factor
 * from SHARD_SPEED to SHARD_SPEED + SHARD_SPREAD */
#define SHARD_TURNS 8
#define SHARD_ROWS 5
#define SHARD_MIDDLE_ROW 2.0F
#define SHARD_OUT 10.0F
#define SHARD_ROW_HEIGHT 3.0F
#define SHARD_LOW 7.5F
#define SHARD_RISE 0.5F
#define SHARD_LIFT 0.3F
#define SHARD_SPEED 0.8F
#define SHARD_SPREAD 1.5F

/* the bomb blows up once MODE_FUSE's timer passes FUSE_STEPS. the blast's
 * damage grows to a radius of BLAST_REACH. its quake shakes the cameras within
 * SHAKE_RANGE. at step AFTERSHOCK_STEP of MODE_GONE, an aftershock shakes those
 * within AFTERSHOCK_RANGE */
#define FUSE_STEPS 20
#define BLAST_REACH 80.0F
#define SHAKE_RANGE 300.0F
#define AFTERSHOCK_RANGE 350.0F
#define AFTERSHOCK_STEP 10
/* the task level of the blast's damage, DmgColExec */
#define DAMAGE_TASK_LEVEL 3
/* the blast's damage takes DAMAGE_SHAPES shapes. DAMAGE_RADIUS is its radius,
 * in its task's work */
#define DAMAGE_SHAPES 1
#define DAMAGE_RADIUS(work) ((work)->scl.x)
/* a new bomb rides from the track's low end once MODE_GONE's timer passes
 * GONE_STEPS */
#define GONE_STEPS 180
/* the state a bomb's task leaves its placement in where it ends in MODE_GONE */
#define GONE_STATE 1

/* the capsule's turn about y for each of game_state.steps, in Angle units */
#define SPIN_PER_STEP 224
/* in MODE_FUSE each half draws HALF_OPEN * (1 - (timer * HALF_RATE)^2) above or
 * below the bomb's pos. the halves meet at the pos once the timer reaches 1 /
 * HALF_RATE */
#define HALF_OPEN 9.5F
#define HALF_RATE 0.05F
/* MODE_GONE fades the halves from WHITE to HALF_GONE_COLOR over HALF_FADE_STEPS
 * steps, and then draws them no more */
#define HALF_FADE_STEPS 30
#define WHITE 0xffffffff
#define HALF_GONE_COLOR 0x00ff1010
/* the poison in each cap is the cap's child. it cycles through POISON_FRAMES
 * textures of capsule_bomb_texlist from POISON_FIRST, a frame every
 * 1 << POISON_FRAME_SHIFT game steps. the draw writes the frame's texture id
 * into TID_BITS of word POISON_TID of the child's polygon list */
#define POISON_FRAMES 13
#define POISON_FRAME_SHIFT 1
#define POISON_FIRST 9
#define POISON_TID 7
#define TID_BITS 0x3fff

#endif
