/* what the enemies share. an enemy's task keeps an enemy_work in its motion,
 * and enemy_motion_step plays the enemy's actions */
#ifndef ENEMY_ENEMY_H
#define ENEMY_ENEMY_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/surface.h"

struct angles;
struct task;
struct task_work;

/* an entry of an enemy's table of actions */
struct enemy_action {
    NJS_MOTION *motion;
    Sint16 mode; /* an enemy_motion_mode */
    /* the index of the action that follows, where mode goes on to another */
    Sint16 next;
    /* how much of a blend into this action passes in a step. 1 changes to
     * the action at once, with no blend */
    Float blend;
    Float speed; /* how far the action's frame moves in a step */
};

/* an enemy_motion's modes. a zeroed enemy_motion is in ENEMY_MOTION_START.
 * the first three modes change the action, and the others play it */
enum enemy_motion_mode {
    /* make next the action at once, at frame 0. the following step goes on
     * as ENEMY_MOTION_CHANGE */
    ENEMY_MOTION_START,
    /* start a blend from the action into next. link holds the two motions,
     * and frame runs from 0 to 1 through the blend. the same step goes on as
     * ENEMY_MOTION_BLENDING */
    ENEMY_MOTION_CHANGE,
    /* move frame on by the new action's blend. at 1 the blend ends, and the
     * new action plays in its own mode */
    ENEMY_MOTION_BLENDING,
    /* forward by the action's speed, wrapped into the motion's frames */
    ENEMY_MOTION_LOOP,
    ENEMY_MOTION_ONCE, /* forward once, then into the action's next */
    ENEMY_MOTION_ONCE_HOLD, /* forward once, then holding the last frame */
    /* backward once from the last frame, then into the action's next */
    ENEMY_MOTION_BACK,
    /* backward once from the last frame, then holding the first */
    ENEMY_MOTION_BACK_HOLD,
    /* a loop by the action's speed times the enemy_motion's speed */
    ENEMY_MOTION_LOOP_SCALED,
    /* forward once by the enemy_motion's speed alone, then into the action's
     * next */
    ENEMY_MOTION_ONCE_SCALED,
    ENEMY_MOTION_STILL, /* frame stays, wrapped into the motion's frames */
    /* frame and the action stay, whatever next holds, and no blend runs */
    ENEMY_MOTION_FROZEN
};

/* in an enemy_motion's flags, which each step clears first: the step
 * reached the action's end. ENEMY_MOTION_ONCE_HOLD and ENEMY_MOTION_BACK_HOLD
 * set it on each step they hold the end. ENEMY_MOTION_ONCE, ENEMY_MOTION_BACK
 * and ENEMY_MOTION_ONCE_SCALED set it on the step the action's next takes
 * over */
#define ENEMY_MOTION_ENDED 0x1
/* the next step would take frame to the last frame or past it. only
 * ENEMY_MOTION_LOOP, ENEMY_MOTION_ONCE and ENEMY_MOTION_ONCE_HOLD set it, and
 * a loop tests before it wraps frame */
#define ENEMY_MOTION_ENDING 0x2

/* how an enemy plays its table of actions. a caller asks for another action
 * by setting next. enemy_motion_step changes to that action through a
 * blend, and moves frame on */
struct enemy_motion {
    /* the frame of the action that plays, or how far a blend has gone, 0 to
     * 1 */
    Float frame;
    Uint8 unk_04[4];
    /* the frame the next action starts at, where its mode is
     * ENEMY_MOTION_LOOP, ENEMY_MOTION_LOOP_SCALED or ENEMY_MOTION_STILL */
    Float next_frame;
    /* the speed of ENEMY_MOTION_LOOP_SCALED and ENEMY_MOTION_ONCE_SCALED */
    Float speed;
    Sint8 mode; /* an enemy_motion_mode */
    Sint8 flags; /* ENEMY_MOTION_ENDED and ENEMY_MOTION_ENDING */
    Sint8 action; /* the index in table of the action that plays */
    Sint8 next; /* the index of the action asked for */
    Sint8 last; /* the action that played at the step's start */
    /* the next of the action that took over when an action last ended */
    Sint8 after_next;
    Sint16 steps; /* the steps since the last change of action began */
    struct enemy_action *table;
    NJS_CNK_OBJECT *object; /* the model the actions move */
    /* while a blend runs, the motions and frames it blends from and to. both
     * motions are null outside one */
    NJS_MOTION_LINK link;
};

/* group's value for an enemy in no group */
#define ENEMY_NO_GROUP (-1)

/* in an enemy_work's flags: enemy_meet_stage holds the enemy back from a
 * ledge, but not while ENEMY_LEDGE_HOLD_OFF is set. a hold returns the enemy to
 * last_pos, stepped back against spd in x and z, and zeroes spd */
#define ENEMY_HOLD_AT_LEDGE 0x1
/* a landing turns ang.x and ang.z toward the floor's tilt */
#define ENEMY_TILT 0x2
/* enemy_defeat makes a burst. it plays a sound too, where the heap has room
 * for a task named dmyEnemy that only waits */
#define ENEMY_BURST 0x4
/* enemy_defeat releases an animal in a bubble by animal_bubble_release, but
 * not in a two-player game */
#define ENEMY_RELEASES_ANIMAL 0x8
/* enemy_defeat leaves a chaos drive by chaos_drive_create, but not in a
 * two-player game */
#define ENEMY_LEAVES_CHAOS_DRIVE 0x10
#define ENEMY_COUNTED 0x20 /* the enemy's group counts it */
/* enemy_meet_stage's findings, which each call sets or clears */
#define ENEMY_UNDER_CEILING 0x40 /* a ceiling stopped the enemy */
#define ENEMY_ON_FLOOR 0x80 /* the enemy landed, or a ledge held it */
#define ENEMY_AT_WALL 0x100 /* a wall pushed the enemy */
/* while it is set, enemy_meet_stage holds the enemy at no ledge.
 * enemy_work_alloc and each hold at a ledge set it, and a landing clears it.
 * an enemy's own source may clear it too */
#define ENEMY_LEDGE_HOLD_OFF 0x200
/* Sonic's or Shadow's magic hand took the enemy, with the player's slot in
 * shot_by. on a hit, the enemy's source makes it a ball for that player by
 * enemy_ball_create */
#define ENEMY_IN_MAGIC_HAND 0x400
/* a walker's lock-on shot that reaches the enemy adds a hit to its lock */
#define ENEMY_UNK_800 0x800
/* enemy_take_hit found a hit by a player or an attack. once the flag is set,
 * no call declared here clears it */
#define ENEMY_HIT 0x1000
/* under ENEMY_PROBE_FLAT, enemy_meet_stage fills the probe's sides by
 * surface_probe_sides_flat. ENEMY_ON_FLAT then keeps whether that call
 * returned 1 */
#define ENEMY_PROBE_FLAT 0x2000
#define ENEMY_ON_FLAT 0x4000

/* an enemy's record, in its task's motion. a walker's lock-on reads any
 * target it locks as an enemy_work. a Uint8 array stands for bytes no source
 * reads. each kind of enemy keeps its own state in the other unk_ fields.
 * its source names them by macros */
struct enemy_work {
    NJS_VECTOR spd;
    NJS_VECTOR unk_00c;
    Angle unk_018;
    Angle unk_01c;
    Angle unk_020;
    Angle unk_024;
    Angle unk_028;
    Uint8 unk_02c[0x14];
    /* the walker's lock that holds the enemy, or null */
    struct task *lock_task;
    /* the slot of the player whose lock-on shot last reached the enemy, or
     * whose magic hand took it. enemy_work_alloc sets -1 */
    Sint8 shot_by;
    Sint8 group; /* the enemy's group, or ENEMY_NO_GROUP */
    Sint8 unk_046;
    Sint8 unk_047;
    Sint16 unk_048;
    Uint8 unk_04a[2];
    Sint16 flags;
    Sint16 unk_04e;
    Sint16 unk_050;
    Uint8 unk_052[2];
    /* what a touch of the record's enemy or item takes from a mech's gauge */
    Float damage;
    /* the work's pos, as enemy_work_alloc or enemy_set_home last kept it */
    NJS_POINT3 home;
    /* the point enemy_turn_to_target turns the enemy toward */
    NJS_POINT3 target;
    /* home plus offset is the point enemy_find_player_near_offset measures
     * from */
    NJS_VECTOR offset;
    NJS_POINT3 last_pos; /* the work's pos as enemy_save_pos last kept it */
    NJS_POINT3 unk_088;
    Float unk_094;
    Float unk_098;
    Float unk_09c;
    /* the normal of the floor enemy_meet_stage last landed the enemy on. it
     * is (0, 1, 0) where the last call landed none, and zero before the first
     * call */
    NJS_VECTOR floor_normal;
    NJS_ARGB unk_0ac;
    /* enemy_meet_stage probes the stage about the work's pos plus
     * probe_offset, turned with the work */
    NJS_VECTOR probe_offset;
    Float top; /* how far above the probe's point the enemy's top lies */
    /* how far across from the probe's point the enemy's sides lie */
    Float radius;
    /* where the enemy's bottom lies in y from the probe's point, negative
     * below it */
    Float bottom;
    /* a ledge is a floor more than step_down below the enemy's bottom, or no
     * floor at all */
    Float step_down;
    /* a wall the enemy moves into scales spd's part into the wall by 1 - 2 *
     * wall_bounce. a wall_bounce of 1 reflects that part whole, and 0.5 stops
     * it */
    Float wall_bounce;
    /* a landing scales spd.y by -floor_bounce. a floor_bounce of 1 reflects
     * spd.y whole, and 0 stops it */
    Float floor_bounce;
    Float friction; /* a landing scales spd.x and spd.z by friction */
    Float unk_0e4;
    /* how far a wall that reflects spd turns ang.y. it turns ang.y the way
     * that would bring spd's heading toward the wall's normal */
    Angle wall_turn;
    Float shadow_size; /* its shadow's scale along its heading */
    /* the ratio of its shadow's scale across its heading to shadow_size */
    Float shadow_ratio;
    /* the point where enemy_meet_stage last met the stage, and the surfaces
     * it found about that point */
    struct surface_probe probe;
    /* scl.y's speed as enemy_squash_step springs scl.y back toward 1 */
    Float squash_spd;
    Angle unk_1c4;
    Angle turn_speed; /* how far a turn moves ang.y toward aim_y in a step */
    Sint32 unk_1cc;
    /* the heading the enemy_turn_to calls turn ang.y toward */
    Angle aim_y;
    Angle sight_angle; /* how far either side of ang.y the enemy sees */
    Float sight_distance2; /* the square of how far the enemy sees */
    /* the square of how near a player must come for the enemy_find_player
     * calls to find it */
    Float near_distance2;
    struct enemy_motion motion;
};

/* release what an enemy holds, for its destructor to call. the call releases
 * the enemy's lock by lockon_release, and takes the enemy out of its group's
 * count. it stops the sounds keyed to task's work. the portable game also
 * stops those sounds if the enemy work allocation failed */
void enemy_destroy(struct task *task);

/* give task's motion a zeroed enemy_work, and return it. home takes the
 * work's pos, shot_by -1, group ENEMY_NO_GROUP and flags ENEMY_LEDGE_HOLD_OFF.
 * the fields enemy_meet_stage reads take defaults. so do turn_speed,
 * sight_angle, sight_distance2, near_distance2, damage and unk_0e4.
 *
 * where the heap has no room, the call kills task by task_kill, and returns
 * null. task's motion stays as it was. the task destroys itself at its next
 * step, unless the caller sets another step. a destructor the caller set
 * before the call runs at that destroy, while task's motion holds no
 * enemy_work.
 *
 * enemy_work_alloc_grouped and enemy_work_alloc_grouped_no_hunt fail as
 * enemy_work_alloc does. on success, each takes group from the low byte of
 * the work's ang.y, and clears that byte. each counts the enemy in group's
 * count. a byte of 0xff gives ENEMY_NO_GROUP, which has a count like any
 * group.
 *
 * the hunt is stage/hunt.h's hunt for a stage's pieces, which can hide a
 * piece in an enemy group. where group isn't ENEMY_NO_GROUP,
 * enemy_work_alloc_grouped tells the hunt by hunt_add_place that group can
 * hold a piece, at pos */
struct enemy_work *enemy_work_alloc(struct task *task);
struct enemy_work *enemy_work_alloc_grouped(struct task *task);
struct enemy_work *enemy_work_alloc_grouped_no_hunt(struct task *task);

void enemy_set_home(struct task_work *work, struct enemy_work *enemy);
/* keep the work's pos in last_pos. for an enemy in a group, the call passes
 * pos to hunt_find_piece. where the hunt hid a piece in the group, the piece
 * takes pos for its place */
void enemy_save_pos(struct task_work *work, struct enemy_work *enemy);

/* 1 where the enemy is hurt, else 0. the enemy is hurt where work's flags
 * hold WORK_HURT. without WORK_HURT, the enemy is hurt where enemy_hit_hook
 * is set and returns nonzero for the work's pos. the call then does nothing
 * more.
 *
 * under WORK_HURT, the call sets squash_spd, which starts enemy_squash_step's
 * spring. where the hits of work's collision record hold a player, or else
 * an attack, it sets ENEMY_HIT. it passes a player that hit to
 * enemy_bounce_player */
int enemy_take_hit(struct task_work *work, struct enemy_work *enemy);
/* settle a beaten enemy. work's collision must not be null, unchecked. the
 * call passes the enemy to lockon_count_blast, and takes it out of its
 * group's count. it sets WORLD_ADDED in work's flags.
 *
 * at the center of the collision's first shape, the call makes what
 * ENEMY_BURST, ENEMY_RELEASES_ANIMAL and ENEMY_LEAVES_CHAOS_DRIVE ask for. for
 * an enemy in a group, hunt_take_piece tells the hunt that a player took the
 * group's piece there. that player is the first player among the collision's
 * hits, or -1 where no player hit */
void enemy_defeat(struct task_work *work, struct enemy_work *enemy);

/* step motion. outside ENEMY_MOTION_START, ENEMY_MOTION_BLENDING and
 * ENEMY_MOTION_FROZEN, a next other than the action puts motion in
 * ENEMY_MOTION_CHANGE. the step then moves frame as the mode says.
 *
 * a change between two ENEMY_MOTION_LOOP_SCALED actions starts the new
 * action partway into its frames. let f be the fraction of its frames the
 * old action had left to play. the new action starts f plus ENEMY_LOOP_LEAD
 * of the way into its frames. a start of the new action's frame count or
 * more wraps back by that count */
void enemy_motion_step(struct enemy_motion *motion);
#define ENEMY_LOOP_LEAD 0.1F

/* the enemy_turn_to calls set aim_y to the heading across the ground toward
 * player, home or target. each turns ang.y toward aim_y by turn_speed. each
 * returns how far ang.y was from aim_y before the turn, 0 to 0x8000.
 * enemy_aim_at_player only aims, and leaves ang.y. player's slot of
 * player_work must not be empty, unchecked */
Angle enemy_turn_to_player(struct task_work *work, struct enemy_work *enemy,
                           int player);
Angle enemy_turn_to_home(struct task_work *work, struct enemy_work *enemy);
Angle enemy_turn_to_target(struct task_work *work, struct enemy_work *enemy);
Angle enemy_aim_at_player(struct task_work *work, struct enemy_work *enemy,
                          int player);

/* move the enemy part of the way out of each surface its probe overlaps. set
 * the findings in flags. a ceiling zeroes spd.y. a floor lands the enemy,
 * or a ledge holds it. a wall that spd moves into reflects spd by
 * wall_bounce, and turns ang.y by wall_turn */
void enemy_meet_stage(struct task_work *work, struct enemy_work *enemy);

/* break the enemy into a piece for each model of the null-ended models, each
 * drawn with texlist. throw 16 scraps besides, by scraps_throw. each piece is
 * a task at the work's pos and angles. a piece flies up, bouncing on floors,
 * then fades.
 *
 * while game_state.time_stopped is set, pieces and scraps hold still. the
 * exception is the flight of one made while it was set */
void enemy_break(struct task_work *work, NJS_CNK_MODEL **models,
                 NJS_TEXLIST *texlist);
/* throw count scraps from pos, turned by ang. each is a task that flies, then
 * ends */
void scraps_throw(int count, NJS_POINT3 *pos, struct angles *ang);

/* draw the enemy's shadow at the work's pos, turned by its heading */
void enemy_draw_shadow(struct task_work *work, struct enemy_work *enemy);

/* the enemy_find_player calls look at players 0 and 1. each returns 1 for
 * player 0, 2 for player 1 or 0 for neither. each returns 0 while
 * game_state.mini_event_running or game_state.cleared is 1.
 *
 * enemy_find_player returns the first player, in slot order, that is near or
 * seen. a near player's squared distance from the work's pos lies below
 * near_distance2. a seen player's lies below sight_distance2, and its heading
 * lies within sight_angle of ang.y.
 *
 * the others return the last player whose squared distance across the
 * ground lies below near_distance2. enemy_find_player_near measures from the
 * work's pos, enemy_find_player_near_home from home, and
 * enemy_find_player_near_offset from home plus offset */
int enemy_find_player_near_home(struct task_work *work,
                                struct enemy_work *enemy);
int enemy_find_player_near(struct task_work *work, struct enemy_work *enemy);
int enemy_find_player_near_offset(struct task_work *work,
                                  struct enemy_work *enemy);
int enemy_find_player(struct task_work *work, struct enemy_work *enemy);

/* where the enemy lies range or more from home, the call pulls it in. pos.x
 * and pos.z become home's plus range times the unit vector from home to pos,
 * and pos.y stays. enemy_keep_near_home measures and takes the unit vector in
 * all three axes, and enemy_keep_near_home_flat in x and z. range must be
 * above 0, unchecked: an enemy at home otherwise takes NaNs in pos.x and
 * pos.z */
void enemy_keep_near_home(struct task_work *work, struct enemy_work *enemy,
                          Float range);
void enemy_keep_near_home_flat(struct task_work *work, struct enemy_work *enemy,
                               Float range);

/* set offset along the work's heading. enemy_set_offset_behind sets it
 * against the heading. its length is the fraction of scl.z, taken without
 * its sign, times ENEMY_OFFSET_SCALE. both calls then round scl.z toward 0 */
void enemy_set_offset(struct task_work *work, struct enemy_work *enemy);
void enemy_set_offset_behind(struct task_work *work, struct enemy_work *enemy);
#define ENEMY_OFFSET_SCALE 1000.0F

/* the square of the enemy's distance from home. enemy_home_distance2
 * measures in all three axes, and enemy_home_distance2_flat in x and z */
Float enemy_home_distance2(struct task_work *work, struct enemy_work *enemy);
Float enemy_home_distance2_flat(struct task_work *work,
                                struct enemy_work *enemy);
/* the square of the distance from the work's pos to player's. player's slot
 * of player_work must not be empty, unchecked */
Float enemy_player_distance2(struct task_work *work, int player);

/* bounce player off the enemy: set its speed to world_gravity times -2, and
 * shake its pad. only a player whose work's flags hold PLAYER_WORK_ATTACK,
 * and neither PLAYER_WORK_ON_GROUND nor PLAYER_WORK_ON_SHAPE, bounces. Sonic
 * and Shadow bounce in any mode. Knuckles and Rouge bounce only in
 * PLAYER_MODE_JUMP or PLAYER_MODE_HUNTER_RUN_ATTACK_3. no other character
 * bounces, and an empty slot is passed over */
void enemy_bounce_player(int player);

/* empty every group's count, and clear enemy_switch_bits */
void enemy_groups_clear(void);
/* 1 where group counts no enemy, and 0 otherwise. only group's low byte
 * counts */
int enemy_group_empty(int group);

/* project pos through the current matrix by njProjectScreen. return 1 where
 * it lands from 0 to 640 across and 0 to 400 down, else 0. a point in the
 * screen's last 80 lines gives 0 */
int point_in_upper_screen(NJS_POINT3 *pos);
/* set target to player's pos plus one of four offsets, the one at index, 0
 * to 3. the camera's heading turns the offset first. player's slot of
 * player_work must not be empty, and camera_task_work must not be null,
 * unchecked */
void enemy_set_target_near_player(struct task_work *work,
                                  struct enemy_work *enemy, int player,
                                  int index);

/* spring scl.y back toward 1, as a damped spring whose speed is squash_spd.
 * scl.y stays from ENEMY_SQUASH_MIN to ENEMY_SQUASH_MAX, and scl.x becomes 2
 * less scl.y */
void enemy_squash_step(struct task_work *work, struct enemy_work *enemy);
#define ENEMY_SQUASH_MIN 0.3F
#define ENEMY_SQUASH_MAX 1.7F

/* make a ball of an enemy for player to hold, and return its task. it returns
 * null where the heap has no room. player must have a record, and scl must not
 * be null, unchecked.
 *
 * the ball draws object with texlist in a sphere, at offset from the sphere's
 * center. a null offset draws object at the center. the ball starts at the
 * point unk_1e8 of player's record holds, and player_hand_task hands it to
 * player. it falls by fall_step_small as a FALL_KIND_NORMAL, turns, and grows
 * toward scl. it bursts and kills its task once it lands, or once its work's
 * flags hold FALL_UNK_8. it destroys itself once placement_try_leave finds it
 * out of range */
struct task *enemy_ball_create(int player, NJS_CNK_OBJECT *object,
                               NJS_TEXLIST *texlist, NJS_VECTOR *scl,
                               NJS_VECTOR *offset);
/* draw model, an index in debug_models, as debug_draw_model does, but unlit
 * in a constant translucent purple. the call puts back the constant
 * attributes and the 3D control flags it found. it leaves the constant
 * material 0 */
void debug_draw_model_unlit(NJS_POINT3 *pos, struct angles *ang,
                            NJS_VECTOR *scl, int model);

/* null, or a function enemy_take_hit calls with the enemy's pos where
 * WORK_HURT is clear. a result other than 0 makes the enemy hurt */
extern int (*enemy_hit_hook)(NJS_POINT3 *pos);
/* a bit for each switch number, 0 to 15. a shouko's switch sets its number's
 * bit, which wakes the shoukos of that number. enemy_groups_clear clears
 * every bit */
extern Sint16 enemy_switch_bits;

#endif
