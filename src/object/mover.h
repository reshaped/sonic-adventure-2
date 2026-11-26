/* the mover, a record in a task's motion that moves the task's work. the
 * work falls by gravity and turns toward a point or a player. it lands on the
 * stage's floors, and the walls push it off */
#ifndef OBJECT_MOVER_H
#define OBJECT_MOVER_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/surface.h"

struct task;

/* a mover's flags */
/* the next landing takes no friction. on a steep floor, it also bounces spd
 * off the floor's normal, not in y alone. a floor is steep where its normal's
 * y is 0.78 or less. the landing clears the flag */
#define MOVER_SLOPE_BOUNCE 0x4
#define MOVER_CAN_REST 0x8 /* mover_step may bring the mover to rest */
/* the mover rests: mover_step doesn't move it, and mover_meet_stage passes it
 * over. mover_step sets and clears the flag */
#define MOVER_RESTING 0x10
/* mover_meet_stage's findings, which each call on a mover that isn't resting
 * sets anew */
#define MOVER_ON_FLOOR 0x400 /* a floor landed the mover */
#define MOVER_AT_WALL 0x800 /* a wall pushed the mover */
/* a flat object, one with a bit of WORLD_FLAT, had a surface below or above
 * the probe's point. flat_y holds the height the probe took for it */
#define MOVER_FLAT_FOUND 0x2000
/* the mover headed into a wall that pushed it. facing_wall holds that wall's
 * plane */
#define MOVER_FACING_WALL 0x4000

/* in the flags of the mover's task's work: the mover stands on a floor.
 * mover_meet_stage sets it anew, as it does MOVER_ON_FLOOR */
#define MOVER_WORK_ON_FLOOR 0x1

/* flat_y where the last probe found no flat object's surface */
#define NO_FLAT_FLOOR (-2000000.0F)

/* the point a mover last probed the stage from, and the nearest surface on
 * each side of it. pos and sides are laid out as a surface_probe's */
struct mover_probe {
    /* @hack type: pos and sides written out, where a surface_probe of the
     * two would do */
    NJS_POINT3 pos;
    struct surface_side sides[SURFACE_SIDES];
    /* each probe starts sides from these, with found cleared. no call
     * declared here writes them */
    struct surface_side initial[SURFACE_SIDES];
};

/* a mover, which mover_alloc puts in its task's motion. each Uint8 array
 * stands for bytes no source reads */
struct mover {
    NJS_VECTOR spd;
    /* what mover_step adds to spd before it moves the work, and then clears */
    NJS_VECTOR acc;
    /* the pitch a mech boss turns ang.x toward. no call declared here reads
     * it */
    Angle aim_x;
    Angle aim_y; /* the heading the mover_turn_to calls turn ang.y toward */
    Angle target_radial_heading; /* King Boom Boo's target measured from home */
    Uint8 unk_24[4];
    /* how far mover_turn_to_target, mover_turn_to_home and
     * mover_turn_to_player turn ang.y in a call */
    Angle turn_speed;
    Uint8 unk_2c[4];
    /* task_motion's hold_turn. a mover no player holds may keep its own
     * state there */
    Sint32 hold_turn;
    Uint8 unk_34[0xc];
    Uint16 flags;
    /* the task's own landing is one that the mover's task makes by its own
     * code, apart from mover_meet_stage. floor_calls counts that landing's
     * runs since it last probed floor_y. mover_meet_stage clears it */
    Uint16 floor_calls;
    Uint8 unk_44[4];
    Float gravity; /* what mover_step adds to acc.y */
    Angle sight_angle; /* how far either side of ang.y the mover sees */
    Float sight_distance; /* how far the mover sees */
    NJS_POINT3 target; /* the point mover_set_target gives */
    NJS_POINT3 last_pos; /* the work's pos as mover_save_pos last kept it */
    NJS_POINT3 home; /* the work's pos as mover_set_home last kept it */
    Uint8 unk_78[4];
    Angle unk_7c;
    Uint8 unk_80[4];
    /* the phases of a boss's sway about x, its bob and its sway about z. no
     * call declared here reads them */
    Angle sway_x;
    Angle sway_y;
    Angle sway_z;
    /* the plane of the last wall MOVER_FACING_WALL names: a point of the
     * wall, and its normal. a call that finds no such wall leaves it */
    NJS_PLANE facing_wall;
    Uint8 unk_a8[4];
    /* how far above the work's pos the probe's point lies */
    Float probe_height;
    Uint8 unk_b0[4];
    Float unk_b4;
    /* how far across from the probe's point the mover's sides lie */
    Float radius;
    /* where the mover's bottom lies in y from the probe's point, negative
     * below it */
    Float bottom;
    Float unk_c0;
    /* a wall the mover moves into scales spd's part into the wall by 1 - 2 *
     * wall_bounce. a wall_bounce of 1 reflects that part whole, and 0.5 stops
     * it */
    Float wall_bounce;
    /* a landing scales spd.y by 1 - 2 * floor_bounce. a floor_bounce of 1
     * reflects spd.y whole, and 0.5 stops it. under MOVER_SLOPE_BOUNCE, on a
     * steep floor, the landing scales spd's part into the floor instead */
    Float floor_bounce;
    Float unk_cc;
    /* a landing without MOVER_SLOPE_BOUNCE scales spd.x and spd.z by
     * friction */
    Float friction;
    Uint8 unk_d4[4];
    /* the floor's height under the mover, as the task's own landing last
     * probed it. the calls declared here neither read it nor write it */
    Float floor_y;
    /* the height of the last flat object's surface below the probe's point.
     * it is NO_FLAT_FLOOR where the probe found no flat object's surface. a
     * flat object with a surface only above the point gives a height 220
     * above it. that is the pos polygon_find_sides leaves on a side where it
     * finds none */
    Float flat_y;
    struct mover_probe probe;
};

/* give task's motion a new mover from the Shinobi heap, and return it. the
 * non-matching build leaves motion unchanged and returns null on allocation
 * failure. the shipped build requires room on the heap. the mover is zero
 * but for the defaults of
 * turn_speed, wall_bounce, floor_bounce, friction, probe_height, radius,
 * bottom, unk_b4, unk_c0 and unk_cc */
struct mover *mover_alloc(struct task *task);

/* add gravity to acc.y and acc to spd, clear acc, and move the work's pos by
 * spd.
 *
 * MOVER_CAN_REST lets a mover with MOVER_ON_FLOOR and without MOVER_AT_WALL
 * come to rest. it rests where its spd across the ground falls below 0.01,
 * and spd becomes 0. a resting mover doesn't move, though its spd still takes
 * gravity and acc. it wakes where collision_pushed is TRUE for task, or spd's
 * size passes 0.01, and moves in that step */
void mover_step(struct task *task);

void mover_set_spd(struct task *task, NJS_VECTOR *spd);
void mover_clear_spd(struct task *task);
void mover_set_acc(struct task *task, NJS_VECTOR *acc);
void mover_clear_acc(struct task *task);
void mover_set_gravity(struct task *task, Float gravity);
void mover_set_hold_turn(struct task *task, Sint32 value);

void mover_set_home(struct task *task);
void mover_save_pos(struct task *task);
void mover_return_home(struct task *task);
void mover_restore_pos(struct task *task);

void mover_set_turn_speed(struct task *task, Angle speed);
void mover_set_target(struct task *task, NJS_POINT3 *target);

/* the calls that take a player read player's slot of player_work, which must
 * not be empty, unchecked. only mover_find_player tests the slots */

/* the distance from the work's pos to target, home or player's pos, in all
 * three axes. the distance2 calls return its square */
Float mover_target_distance(struct task *task);
Float mover_target_distance2(struct task *task);
Float mover_home_distance(struct task *task);
Float mover_home_distance2(struct task *task);
Float mover_player_distance2(struct task *task, int player);

/* the heading across the ground from the work's pos to target or player's
 * pos: 0 toward +z, and QUARTER_TURN toward +x, as the mover's ang.y
 * faces */
Angle mover_target_heading(struct task *task);
Angle mover_player_heading(struct task *task, int player);

/* the mover_turn_to calls set aim_y to the heading toward target, home or
 * player, and turn ang.y toward aim_y. the plain calls turn by turn_speed,
 * and mover_turn_to_aim and the _by calls by step. mover_turn_to_aim leaves
 * aim_y as it stands. each returns how far ang.y is from aim_y after the turn,
 * 0 to 0x8000 */
Angle mover_turn_to_target(struct task *task);
Angle mover_turn_to_aim(struct task *task, Angle step);
Angle mover_turn_to_target_by(struct task *task, Angle step);
Angle mover_turn_to_home(struct task *task);
Angle mover_turn_to_home_by(struct task *task, Angle step);
Angle mover_turn_to_player(struct task *task, int player);
Angle mover_turn_to_player_by(struct task *task, Angle step, int player);

/* the slot of the first of players 0 and 1 that the mover sees. a seen
 * player lies within sight_distance of the work's pos, at a heading within
 * sight_angle of ang.y. it returns 0 where it sees neither, as it does for a
 * seen player
 * 0 */
int mover_find_player(struct task *task);

/* meet the stage, unless the mover rests. the call probes the surfaces on
 * each side of the probe's point, and keeps them in probe. where the floor
 * below the point lies above the mover's bottom, the work rises onto it: a
 * landing. the walls within radius of the point push the work out. each call
 * clears floor_calls, and sets MOVER_WORK_ON_FLOOR and the mover's findings
 * anew.
 *
 * the probe takes the world's objects that world_list_near lists near the
 * point. a task's object counts only where its position lies within its
 * reach of the point. its reach is its model's radius times its largest
 * scale. a flat object gives flat_y in place of a floor or a ceiling. on
 * the +x side, the probe takes no surface from a flat object.
 *
 * a landing bounces spd by floor_bounce, and scales spd.x and spd.z by
 * friction unless MOVER_SLOPE_BOUNCE is set. a wall pushes the work along its
 * normal across the ground. it reflects spd by wall_bounce where spd moves
 * into it. a near wall whose back faces the point pushes the work through to
 * its front */
void mover_meet_stage(struct task *task);

#endif
