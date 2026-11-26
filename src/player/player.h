/* the players: their records, the steps every character shares, and the calls
 * other tasks make on them */
#ifndef PLAYER_PLAYER_H
#define PLAYER_PLAYER_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/surface.h"
#include "system/angle.h"

struct jiggle;
struct model_entry;
struct motion_entry;
struct path;
struct polygon_ball;
struct task;
struct task_work;
struct world_object;

/* the bits of a player's anim flags, which each step clears first. a step sets
 * PLAYER_ANIM_DONE where an animation that plays once reaches its end. it sets
 * it too on each step that PLAYER_ANIM_HOLD or PLAYER_ANIM_BACK_HOLD holds its
 * end. it sets PLAYER_ANIM_ENDING on the step before a PLAYER_ANIM_ONCE
 * animation ends. under PLAYER_ANIM_NO_BLEND, which module_motions_unregister
 * sets, the step changes to next without a blend */
#define PLAYER_ANIM_DONE 0x1
#define PLAYER_ANIM_ENDING 0x2
#define PLAYER_ANIM_NO_BLEND 0x4

/* the anim's modes. an anim starts in PLAYER_ANIM_NEW, which allocates the
 * blend's motion. a change of animation takes PLAYER_ANIM_CHANGE, then
 * PLAYER_ANIM_BLEND. in PLAYER_ANIM_BLEND, frame runs from 0 up to 1 as current
 * blends in. once frame reaches 1, the anim takes current's own mode, one of
 * those below. there frame counts current's frames. PLAYER_ANIM_END frees what
 * the anim allocated.
 *
 * in the modes below, current plays its action's rate of frames a step, unless
 * the mode says otherwise.
 * - PLAYER_ANIM_LOOP loops;
 * - PLAYER_ANIM_ONCE plays once, then its next, as PLAYER_ANIM_BACK does
 *   backward;
 * - PLAYER_ANIM_ONCE_BLEND plays once and blends into its next;
 * - PLAYER_ANIM_BLEND_AFTER plays once and blends into the anim's after;
 * - PLAYER_ANIM_HOLD and PLAYER_ANIM_BACK_HOLD play once and hold the end;
 * - PLAYER_ANIM_LOOP_SPEED loops at its rate times the record's spd.x;
 * - PLAYER_ANIM_LOOP_PACE loops at its rate times the record's anim_speed;
 * - PLAYER_ANIM_PACE_ONCE plays once at anim_speed alone, then its next;
 * - PLAYER_ANIM_DRIVEN plays at the frame its player sets */
#define PLAYER_ANIM_NEW 0
#define PLAYER_ANIM_CHANGE 1
#define PLAYER_ANIM_BLEND 2
#define PLAYER_ANIM_LOOP 3
#define PLAYER_ANIM_ONCE 4
#define PLAYER_ANIM_ONCE_BLEND 5
#define PLAYER_ANIM_HOLD 6
#define PLAYER_ANIM_BACK 7
#define PLAYER_ANIM_BACK_HOLD 8
#define PLAYER_ANIM_BLEND_AFTER 9
#define PLAYER_ANIM_LOOP_SPEED 10
#define PLAYER_ANIM_LOOP_PACE 11
#define PLAYER_ANIM_PACE_ONCE 12
#define PLAYER_ANIM_DRIVEN 13
#define PLAYER_ANIM_END 15

/* an entry of a character's table of animations */
struct player_action {
    Sint16 motion; /* its motion's slot of motion_table */
    Sint16 model; /* its model's slot of model_table */
    Sint16 mode; /* the PLAYER_ANIM_ mode it plays in */
    Sint16 next; /* the animation that follows it, below 256 */
    /* how much of a blend into it each step plays, of 1. at 1 the change
     * takes no blend */
    Float blend_step;
    Float rate; /* the frames it plays a step, or the factor of its pace */
};

/* the animation state inside a player's record */
struct player_anim {
    Sint16 mode; /* a PLAYER_ANIM_ mode */
    Sint16 next; /* the animation asked for, which the step changes to */
    /* the animation playing: its index in actions */
    Sint16 current;
    Sint16 last; /* what current was as the anim's last step began */
    /* the animation the setters give to follow next, or 0 */
    Sint16 after;
    /* the steps since the animation started or changed */
    Sint16 steps;
    /* PLAYER_ANIM_DONE, PLAYER_ANIM_ENDING and PLAYER_ANIM_NO_BLEND */
    Sint16 flags;
    Uint8 unk_0e[2];
    /* how far the animation has played, or in PLAYER_ANIM_BLEND how far the
     * blend has */
    Float frame;
    Float last_frame; /* what frame was as the anim's last step began */
    /* the frame a looping or driven animation starts on, and then 0 */
    Float start_frame;
    /* the record's spd and anim_speed, as player_register points them */
    NJS_VECTOR *speed;
    Float *pace;
    struct player_action *actions; /* the character's table */
    /* the blend's motion, which a step in PLAYER_ANIM_NEW allocates and one
     * in PLAYER_ANIM_END frees, or null */
    NJS_MOTION *blend;
};

/* the numbers of the animations the sources name. each character's table holds
 * the character's own animation at each number */
#define ANIM_STAND 0
/* a landing slower than ANIM_LAND's, and the end of a slide */
#define ANIM_LAND_STILL 1
/* two animations that player_stand_anim leaves playing on level ground, as it
 * does ANIM_STAND. nothing in the tree starts either */
#define ANIM_UNK_2 2
#define ANIM_UNK_3 3
/* a stand on a slope, by whether the player faces up it or down it */
#define ANIM_STAND_UPHILL 4
#define ANIM_STAND_DOWNHILL 5
/* PLAYER_MODE_RUN's five, which player_run_anim picks by speed. ANIM_WALK
 * plays up to trot_speed. each of the others plays above the next of
 * trot_speed, running_speed, sprint_speed and full_speed. a mech has ANIM_WALK,
 * ANIM_RUN and ANIM_FULL_SPEED. a run along a path starts in ANIM_RUN */
#define ANIM_WALK 6
#define ANIM_TROT 7
#define ANIM_RUN 8
#define ANIM_SPRINT 9
#define ANIM_FULL_SPEED 10
/* the rise of PLAYER_MODE_BOUNCE, PLAYER_MODE_LAUNCH and PLAYER_MODE_FLING */
#define ANIM_LAUNCH 13
#define ANIM_FALL_START 14 /* a fall's start, which goes on to ANIM_FALL */
#define ANIM_FALL 15
#define ANIM_LAND 16
#define ANIM_LAND_FAST 17
#define ANIM_BRAKE 18
/* the start of PLAYER_MODE_UNK_0E's push. the push's step brakes the player
 * while it plays, and pushes once it ends */
#define ANIM_PUSH_START 19
#define ANIM_HURT 21
#define ANIM_HURT_LAND 22
#define ANIM_UNK_23 23
/* the throw of PLAYER_MODE_CRASH, and its end, which ANIM_CRASH goes on to */
#define ANIM_CRASH 24
#define ANIM_CRASH_END 25
/* the rise out of a ball, as a roll slows below trot_speed or a spin ends */
#define ANIM_STAND_UP 26
#define ANIM_TURN 27 /* the turn about that ends PLAYER_MODE_BRAKE */
/* PLAYER_MODE_SLIDE's: ANIM_SLIDE_BACK where player_turn_along_slope is
 * below 0 as the slide starts */
#define ANIM_SLIDE 28
#define ANIM_SLIDE_BACK 29
/* PLAYER_MODE_PICK_UP's, which goes on to ANIM_HOLD_STAND */
#define ANIM_PICK_UP 31
/* the animations of a player holding a task, each in place of the one it is
 * named for. PLAYER_MODE_RUN takes ANIM_HOLD_RUN above running_speed, and
 * ANIM_HOLD_WALK otherwise */
#define ANIM_HOLD_RUN 32
#define ANIM_HOLD_BRAKE 33
#define ANIM_HOLD_TURN 34
#define ANIM_HOLD_STAND 35
#define ANIM_HOLD_STAND_UPHILL 36
#define ANIM_HOLD_STAND_DOWNHILL 37
#define ANIM_HOLD_WALK 38
/* PLAYER_MODE_UNK_20's, at a stick that player_stick_into_offer grades other
 * than PLAYER_WALL_HEAD_ON. the mode moves by player_accel_push, which pulls
 * the player back only at PLAYER_WALL_AWAY */
#define ANIM_DRAG 40
/* the shake of PLAYER_MODE_UNK_22, player_shake_touched's mode */
#define ANIM_SHAKE 41
#define ANIM_HOLD_SHAKE 42 /* PLAYER_MODE_SHAKE_HOLDING's */
/* a grab of a task to push or to shake, which player_hold and
 * player_grab_shakable start. a player that stops while it holds such a task
 * stands in ANIM_GRAB_STAND */
#define ANIM_GRAB 43
#define ANIM_GRAB_STAND 44
/* PLAYER_MODE_JUMP_HOLDING's, which the bounce and the launch of a player
 * holding a task play too */
#define ANIM_HOLD_JUMP 45
#define ANIM_HOLD_FALL_START 46
#define ANIM_HOLD_FALL 47
#define ANIM_HOLD_LAND_FAST 48
/* the throws of a held task, which player_throw_held starts on the ground
 * and player_air_throw_held in the air: at a target, or ahead.
 * player_set_down_held starts ANIM_SET_DOWN, which sets the task down */
#define ANIM_THROW_AT 49
#define ANIM_THROW 50
#define ANIM_AIR_THROW_AT 51
#define ANIM_AIR_THROW 52
#define ANIM_SET_DOWN 53
/* the win and loss animations that stage_pose_winner asks of the winner and
 * of the other player. a missed player answers them with
 * PLAYER_MODE_PLAY_ANIM */
#define ANIM_WIN 54
#define ANIM_LOSE 85
#define ANIM_CAUGHT 55
#define ANIM_BREATHE 56 /* what PLAYER_REQUEST_BREATHE starts */
#define ANIM_DROWN 57 /* what PLAYER_REQUEST_DROWN starts */
#define ANIM_MISS 59
#define ANIM_TEETER 61
#define ANIM_LEDGE_HANG 62
#define ANIM_LEDGE_CLIMB 63
#define ANIM_LEDGE_JUMP 64
/* a jump's rise, which goes on to ANIM_JUMP_RISE, and its fall */
#define ANIM_JUMP 65
#define ANIM_JUMP_RISE 66
#define ANIM_JUMP_FALL 67
/* a pull at a task: its start, its loop, the end of a pull that gives up,
 * and the lift of one that takes hold of the task */
#define ANIM_PULL_START 69
#define ANIM_PULL 70
#define ANIM_PULL_END 71
#define ANIM_PULL_LIFT 79
/* player_throw_at_touch's throw, at the task of a touched shape */
#define ANIM_THROW_AT_TOUCH 73
/* PLAYER_MODE_UNK_2A's, a call. a Chao near player 0 answers it where its
 * chao_player_liking is above 50, by chao_try_player_call */
#define ANIM_CALL 72
/* what PLAYER_REQUEST_FREE_FALL starts. while it plays, PLAYER_MODE_FALL's
 * step takes gravity alone, and no stick */
#define ANIM_FREE_FALL 74
#define ANIM_PET_CHAO 78 /* PLAYER_MODE_PET_CHAO's */
#define ANIM_FLUTE 82
#define ANIM_FLUTE_AGAIN 83
/* a sinking Sonic, Shadow or Metal Sonic plays ANIM_SINK. a sinking mech plays
 * its own, ANIM_SINK_EGGMAN_MECH or ANIM_SINK_TAILS_MECH */
#define ANIM_SINK 181
#define ANIM_SINK_EGGMAN_MECH 138
#define ANIM_SINK_TAILS_MECH 189
#define ANIM_CANT_PLAY_FLUTE 84
/* where motion_table's slot 86 holds no motion, the answer of a Sonic, a
 * hunter or a mech to a PLAYER_REQUEST_ANIM of it plays ANIM_STAND instead */
#define ANIM_UNK_86 86
/* what a player plays as it hangs from a vine's tip. the twining, the
 * revolving, the hanging and the side vine ask for it as a player catches
 * them */
#define ANIM_VINE_HANG 167
/* what a player plays on a bungee vine: the catch, the ride, and a second
 * ride, which each bungee vine plays at a time of its own */
#define ANIM_BUNGEE_RIDE 169
#define ANIM_BUNGEE_RIDE_A 170
#define ANIM_BUNGEE_CATCH 171
/* what a player plays on an iron bar: the swing, the throw, the flight and
 * the landing. a _BACK animation is the one for a swing that turns the other
 * way. a hunter's table holds entries of its own from 174 to 180 instead. its
 * 176 to 178 are player/knuckles.h's ANIM_ROUGE_SPECIAL to
 * ANIM_ROUGE_EARTHQUAKE */
#define ANIM_BAR_SWING 174
#define ANIM_BAR_SWING_BACK 175
#define ANIM_BAR_THROW_BACK 176
#define ANIM_BAR_THROW 177
#define ANIM_BAR_FLY_BACK 178
#define ANIM_BAR_FLY 179
#define ANIM_BAR_LAND 180
/* the animation of a Sonic or a Shadow that a current carries, which
 * PLAYER_REQUEST_CURRENT_PATH gives. PLAYER_REQUEST_MISS leaves it playing,
 * and the current carries the player on */
#define ANIM_IN_CURRENT 184
/* a standing animation of Rouge's, which player_stand_anim gives only while
 * motion_table's slot 216 holds a motion */
#define ANIM_ROUGE_STAND_216 216

/* the characters a player can be, as its record's character holds them. the
 * mechs are Tails's and Eggman's walkers */
#define PLAYER_SONIC 0
#define PLAYER_SHADOW 1
#define PLAYER_TAILS 2
#define PLAYER_EGGMAN 3
#define PLAYER_KNUCKLES 4
#define PLAYER_ROUGE 5
#define PLAYER_TAILS_MECH 6
#define PLAYER_EGGMAN_MECH 7
#define PLAYER_SUPER_SONIC 9
#define PLAYER_SUPER_SHADOW 10

/* two looks a mech can take besides its own, each named by its model's file:
 * CWALKMDL.PRS on PLAYER_TAILS_MECH, and BWALKMDL.PRS on PLAYER_EGGMAN_MECH */
#define PLAYER_KIND_CWALK 13
#define PLAYER_KIND_BWALK 14
/* four more looks, each named by its model's file, with the character that
 * takes it */
#define PLAYER_KIND_AMY 8 /* AMYMDL.PRS, on PLAYER_SONIC */
#define PLAYER_KIND_METALSONIC 12 /* METALSONICMDL.PRS, on PLAYER_SHADOW */
#define PLAYER_KIND_TICAL 15 /* TICALMDL.PRS, on PLAYER_KNUCKLES */
#define PLAYER_KIND_CHAOS0 16 /* CHAOS0MDL.PRS, on PLAYER_ROUGE */

/* the room a player's record has for actions */
#define PLAYER_ACTIONS 8

/* a player record's flags */
#define PLAYER_BARRIER 0x0001 /* the barrier of an NBarrier task */
/* the barrier of a TBarrier task. the rings near a player under it close
 * in on it */
#define PLAYER_MAGNETIC_BARRIER 0x0002
/* player_start_event made the player for an event, and player_end_event
 * kills it */
#define PLAYER_FOR_EVENT 0x2000
/* the player has missed. player_miss sets it as it misses the player, and a
 * miss's task sets it as the miss takes effect. a time limit that runs out
 * and a retry set player 0's. player_reset clears it. a player under it
 * takes a ring and gains nothing from it */
#define PLAYER_MISSED 0x4000
/* invincibility, as a Muteki task gives it */
#define PLAYER_INVINCIBLE 0x8000

/* the attack and the guard level of a player's body shapes under
 * PLAYER_INVINCIBLE */
#define INVINCIBLE_LEVEL 3

/* the upgrades, by number. each but UPGRADE_UNK_15 takes its name from the
 * upgrade stand's string for it, as UPGRADE_SONIC_SHOES does from
 * [SONIC]SHOES. a stand that gives upgrade n also sets game flag n */
enum upgrade_number {
    UPGRADE_SONIC_SHOES = 0,
    UPGRADE_SONIC_SPARKLE = 1,
    UPGRADE_SONIC_MAGICWRIST = 2,
    UPGRADE_SONIC_FLAMERING = 3,
    UPGRADE_SONIC_RUBBERUNIT = 4,
    UPGRADE_SONIC_MELODY = 5,
    UPGRADE_TWALKER_BOOSTER = 6,
    UPGRADE_TWALKER_HYPERCANNON = 7,
    UPGRADE_TWALKER_LAZERBALSTER = 8,
    UPGRADE_TWALKER_MELODY = 9,
    UPGRADE_KNUCKLES_CLAWS = 10,
    UPGRADE_KNUCKLES_SUNGLASS = 11,
    UPGRADE_KNUCKLES_GROVES = 12,
    UPGRADE_KNUCKLES_AIR = 13,
    UPGRADE_KNUCKLES_MELODY = 14,
    /* no stand holds it. a hit on a player with it halves its record's
     * spd.x. the player then takes no PLAYER_MODE_HURT, and the hit can't
     * make it miss. boss_last2 sets the upgrades of its players to this one
     * alone */
    UPGRADE_UNK_15 = 15,
    UPGRADE_SHADOW_SHOES = 16,
    UPGRADE_SHADOW_SPARKLE = 17,
    UPGRADE_SHADOW_FLAMERING = 18,
    UPGRADE_SHADOW_MELODY = 19,
    UPGRADE_EWALKER_JETENGINE = 20,
    UPGRADE_EWALKER_BAZOOKA = 21,
    UPGRADE_EWALKER_POWERGUN = 22,
    UPGRADE_EWALKER_EXTRASHIELD = 23,
    UPGRADE_EWALKER_MELODY = 24,
    UPGRADE_ROUGE_NAILS = 25,
    UPGRADE_ROUGE_SCOPE = 26,
    UPGRADE_ROUGE_BOOTS = 27,
    UPGRADE_ROUGE_MELODY = 28,
    /* no stand holds it. the title's test menu sets its game flag with
     * every stand's own, as full equipment */
    UPGRADE_UNK_29 = 29
};
/* upgrade n's bit in a player record's upgrades */
#define UPGRADE_BIT(n) (1 << (n))

/* what an event draws for a player beside its own models: object, posed by
 * motion and shape where each isn't null */
struct player_event_part {
    NJS_CNK_OBJECT *object;
    NJS_MOTION *motion;
    NJS_MOTION *shape;
};

/* what an event's file holds for one of its characters: four parts. a part
 * whose object is null draws nothing */
struct player_event {
    /* the motion of the player's own model, which an Eggman's draw and a
     * mech's play at mini_event_frame in PLAYER_MODE_EVENT */
    NJS_MOTION *motion;
    /* @hack type: four members, where an array of four parts would do */
    struct player_event_part part0;
    struct player_event_part part1;
    struct player_event_part part2;
    struct player_event_part part3;
};

/* what a player's look_mode looks at */
#define PLAYER_LOOK_NONE 0
#define PLAYER_LOOK_TASK 1
#define PLAYER_LOOK_POINT 2
#define PLAYER_LOOK_ANGLES 3

/* a character's parameters, which player_register copies into its player's
 * record. the speeds are in the player's own axes, x ahead */
struct character_constants {
    Sint32 jump_hold_steps; /* how long a held jump keeps counting */
    /* how far below the player's middle, at most, its collision ball
     * reaches to keep it on the ground, before the drop's factors */
    Float snap;
    /* the fastest the player's motion runs along the world's x or z, and
     * along its y, as the speed steps hold it */
    Float speed_limit;
    Float fall_limit;
    /* a player on the ground faster than this meets resist.x against the
     * speed beyond it */
    Float top_speed;
    Float push_speed; /* the fastest it pushes or pulls what it holds */
    /* the speed up that a jump starts at. a hit throws the player up at it
     * too */
    Float jump_speed;
    /* a launched player rising at this speed or faster takes no stick, as
     * player_accel_launched has it. one rising slower steers as
     * player_accel_air lets it */
    Float launch_steer_speed;
    /* a standing Tails, Eggman or mech slides on steep ground where its
     * speed is past this, and stands again once its motion is slower */
    Float slide_speed;
    /* the speeds each faster run animation starts above. a hit throws the
     * player back at running_speed. speed_lines_create scales its lines'
     * length by a speed's ratio to sprint_speed */
    Float trot_speed;
    Float running_speed;
    Float sprint_speed;
    /* this fast or faster, a player that runs head on into a wall or a
     * shape crashes into it */
    Float crash_speed;
    Float full_speed;
    /* while player_count_jump_hold is TRUE, a Tails's, an Eggman's or a
     * mech's jump adds a share of it to spd.y each step */
    Float jump_lift;
    /* what a step adds to the speed ahead for a stick pushed all the way:
     * on the ground, and in the air */
    Float ground_accel;
    Float air_accel;
    /* what a step on the ground adds against the speed ahead, below 0.
     * ground_drag acts once the stick lets go, and brake_drag in a brake or in
     * deep water */
    Float ground_drag;
    Float brake_drag;
    /* the factor of the stick's push in the air, below 0, where it points
     * back against a speed above running_speed */
    Float air_brake;
    /* resistance, as factors below 0 of the speed it acts against, added
     * each step: air_resist of the speed ahead in the air, and resist of
     * each axis of the speed */
    Float air_resist;
    NJS_VECTOR resist;
    /* the ground's grip, as factors below 0 of the push into the ground.
     * moving_grip holds back a speed ahead, and side_grip a speed across.
     * still_grip holds a still axis still, and caps a push ahead */
    Float moving_grip;
    Float side_grip;
    Float still_grip;
    Float unk_6c;
    Float radius;
    /* from its pos up to its head. surface_probe_player's ahead probe starts
     * height + 2 above the player's pos. it lies along the up that the work's
     * ang.x and ang.z give the player. surface_probe_ground then lifts that
     * point 1 more, along the world's y */
    Float height;
    /* the speed it gains each step along world_gravity. player_ground_step
     * stands a player in the air on the ground below where it lies less than
     * this below the player's pos */
    Float gravity;
    /* the height of its eyes above its pos, where the camera's modes look.
     * player_finish_step counts its breath while they are under water */
    Float target_height;
    /* an Eggman's step puts the center of his second shape this far above
     * his pos, along his up, a mech's step its body's shape and its top
     * shape, and a Tails's his body's shape */
    Float center_height;
};

/* a target a falling player can steer onto, as player_add_target lists it */
struct player_target {
    NJS_POINT3 pos; /* its center */
    Float radius;
    Float distance; /* from the player to the target's surface */
    /* from the player to the target's middle, leaving out y, as the steer
     * last took it */
    Float across;
    Angle off; /* the turn to the target, from the player's heading */
    Angle ang; /* the heading from the player to the target */
};

/* how many targets a player's record has room for */
#define PLAYER_TARGETS 15

/* the steps of the stick's past that a hunter's record keeps */
#define HUNTER_STICK_STEPS 64

/* the points a mech's record gives its 2P missiles to leave from */
#define PLAYER_MISSILE_POINTS 6

/* the id of a mech's strike shape, walker.c's SHAPE_STRIKE, the fourth of the
 * five that the walkers' table gives a mech's collision. it is on only in a
 * strike's mode. the table makes it a sphere of radius 15, of attack and guard
 * level 3, that hurts players, enemies and objects */
#define MECH_STRIKE_SHAPE 7

/* the costumes of game_state.costume and of a record's costume. each
 * character's files for COSTUME_H and COSTUME_X take an H and an X before
 * its own name: HSONICMDL.PRS, XKNUCKMDL.PRS. COSTUME_PSO is named by
 * Sonic's and Shadow's files, PSOSONICMDL.PRS and PSOSHADOWMDL.PRS. the
 * other characters' files for 1 take other names, as BKNUCKMDL.PRS and
 * EWALK1MDL.PRS do */
#define COSTUME_NONE 0
#define COSTUME_PSO 1
#define COSTUME_H 2
#define COSTUME_X 3

/* a player's record, which is its task's extra. the bytes no source reads
 * yet are left as padding, and the record's size is unknown: never take
 * sizeof */
struct player {
    Sint8 slot; /* its own index in the players' tables */
    Sint8 character; /* one of the PLAYER_ characters */
    Sint8 costume;
    /* its look: its character's number, or another look's number, such as
     * PLAYER_KIND_CWALK */
    Sint8 kind;
    /* the actions the player is offered: the first action_count of them are in
     * use. action holds the current action, one of those values, not an index.
     * the action window names it and steps it through the list. the window
     * sets it to 0 while the list is empty or the player's task has no step */
    Sint8 actions[PLAYER_ACTIONS];
    Sint8 action_count;
    Sint8 action;
    /* the action the action button took last from the list, as each
     * character's step keeps it. a search box or a hint box opens for the
     * player once it holds the action the box offers. the box then sets it
     * to 0 */
    Sint8 action_taken;
    Uint8 unk_00f;
    Sint16 flags; /* PLAYER_BARRIER and the rest */
    /* the steps the jump button has been held, as player_count_jump_hold
     * counts them. each jump of a Sonic's sets it to 0.
     * a current takes no player in PLAYER_MODE_JUMP while it is below 5 */
    Sint16 jump_steps;
    /* while it isn't 0, a player off a path takes no stick.
     * player_finish_step counts it down to 0. a mech's shot slows it only
     * while it is 0 or less. a Sonic's trick off a soap switch sets it to
     * the switch's timer */
    Sint16 stick_lock;
    /* the steps the player's eyes have been under water, as
     * player_finish_step counts them */
    Sint16 underwater_steps;
    /* a count of steps that the player's modes keep. standing counts its
     * steps in it, and other modes their own waits */
    Sint16 mode_steps;
    /* while it is above 0, the player is confused: player_read_stick mixes
     * up the heading the stick asks for, and a confu star lasts.
     * player_finish_step counts it down to 0 */
    Sint16 confused_steps;
    /* the work's flags as player_collide_world last began */
    Sint16 last_flags;
    /* how many of targets are listed. player_finish_step sets it to 0 */
    Sint16 target_count;
    /* the point of its path the player was taken on at. player_hang_path,
     * player_ledge_path and the rail requests give it */
    Sint16 path_point;
    Uint8 unk_022[2];
    Uint32 upgrades; /* the UPGRADE_BIT of each upgrade it has */
    Sint32 request_arg; /* what goes with a request of PLAYER_WORK_REQUEST */
    /* the player's lean into its turns. player_run_anim eases it toward four
     * times the turn of the last three steps, within 0x1d00 */
    Angle lean;
    Float path_distance; /* how far along its path it is */
    /* how level the ground it stands on is, against world_gravity: 1 where
     * level, 0 for a wall, and below 0 under a ceiling */
    Float level;
    /* the part of the stick's push that the ground's grip didn't hold, which
     * the running animations play the faster for. player_finish_step sets it
     * to 0 */
    Float slip;
    /* the stick's push this step along the player's own axes: ahead on the
     * ground, or across on a path. a Sonic's board leans by it, and its
     * sparks and sound follow it. a hunter's climb moves only while it
     * isn't 0 */
    Float drive;
    /* what scales the ground's grip. player_register sets it to 1 */
    Float grip_scale;
    /* the pace the animations that play by speed take */
    Float anim_speed;
    /* a mech's gauge, 0 to 10, or to 12 with UPGRADE_EWALKER_EXTRASHIELD.
     * player_miss makes no miss while it is above 0 */
    Float gauge;
    /* a push that other tasks give the player for a step. the collision
     * checks and a parked car set it, and a current adds to it. the
     * player's step moves its pos by it, and player_finish_step sets it to
     * 0 */
    NJS_VECTOR push;
    /* what the player's spd gains this step, in its own axes. the
     * acceleration steps set it, and a Sonic's rail step too.
     * player_apply_accel adds it to spd, and player_finish_step sets it to
     * 0 */
    NJS_VECTOR accel;
    /* the player's speed in its own axes, as turn_from_world gives them.
     * dust's smoke and sand tasks make their running ring only while spd.x
     * is 0.1 or more */
    NJS_VECTOR spd;
    /* the normal of the wall the player touches, or 0 where it touches
     * none */
    NJS_VECTOR wall_normal;
    NJS_VECTOR ground_normal;
    /* the world flags of the ground it stands on, and of the ground it stood
     * on as its collision step began */
    Sint32 ground_flags;
    Sint32 last_ground_flags;
    /* the ball the collision step sweeps through the world, which
     * player_register allocates */
    struct polygon_ball *ball;
    struct task *held; /* the task it holds under PLAYER_WORK_HOLDING */
    /* the task player_hand_task handed it to hold, or null */
    struct task *handed;
    /* the player's offer: the touched task whose action flags
     * player_action_flags gives, or the task it holds, or null. player_hold
     * holds it in place of the task the collision touched, and
     * player_ground_step empties it */
    struct task *hold_offer;
    /* the task whose collision surface the player stands on, or null */
    struct task *ground_task;
    /* the task whose collision shape the player stands on top of, as
     * collision_next_below found it, or null */
    struct task *stood_on;
    /* the task the player's turn toward a target or a touch last faced, as
     * player_turn_to_target and player_turn_to_touch set it.
     * player_finish_step empties it, and sets unk_0ac to 0 */
    struct task *faced;
    Sint32 unk_0ac;
    /* the world object player_collide_world last stood the player on, or null.
     * the next collision step meets it last */
    NJS_OBJECT *ground_object;
    /* room for PLAYER_TARGETS targets, the first target_count of them
     * listed, which player_register allocates */
    struct player_target *targets;
    struct path *path; /* the path it follows under PLAYER_WORK_ON_PATH */
    struct player_event *event; /* player_start_event's event, or null */
    /* a copy of its character's row, which the steps change for this player */
    struct character_constants consts;
    /* what the player's look task looks at while look_mode is a
     * PLAYER_LOOK_ mode other than PLAYER_LOOK_NONE: look_task, with
     * look_pos as an offset in its axes, look_pos in the world, or the turns
     * of look_ang. the look lasts look_steps steps, or for ever while
     * look_steps is below 0. the player_look_at_ calls set them */
    Sint8 look_mode;
    Uint8 unk_145[3];
    Sint32 look_steps;
    struct angles look_ang;
    NJS_POINT3 look_pos;
    Uint8 unk_164[0x0c];
    struct task *look_task;
    struct player_anim anim;
    /* the surfaces about two points, as surface_probe_player leaves them
     * each step. ground's point lies 1 above the player's pos, and ahead's
     * radius + 2 ahead of the player and above it, as collision/surface.h
     * says. while ground's last_above_flags has a bit of WORLD_FLAT, the
     * player is in water: a current takes and pushes only such a player,
     * and player_finish_step counts the player's breath only then */
    struct ground_probe ground;
    struct ground_probe ahead;
    /* a place in the world. an enemy ball starts there. a hunter's draw
     * keeps its hands at it and at unk_1f4 */
    NJS_POINT3 unk_1e8;
    /* places in the world, as unk_218 and unk_230 are. a Shadow's Chaos
     * Control draws at unk_1f4. a hunter's draw keeps its feet at unk_200
     * and unk_20c */
    NJS_POINT3 unk_1f4;
    NJS_POINT3 unk_200;
    NJS_POINT3 unk_20c;
    /* a place in the world. a confu star circles it. a hunter's draw keeps
     * its head there */
    NJS_POINT3 unk_218;
    /* a place in the world. a white breath puffs out from it, along
     * unk_2c0. a hunter's draw keeps its mouth there */
    NJS_POINT3 unk_224;
    /* a place in the world. a hunter's draw keeps there the node of model
     * 143, or of 172 for a Rouge */
    NJS_POINT3 unk_230;
    /* places in the world, as unk_230 is. where the record is a mech's, its
     * draws keep three places there. unk_23c is its gun's muzzle, which a
     * lock's shot and its first 2P special leave from along gun_dir. unk_248
     * is its cannon's muzzle, which its cannon fires from along cannon_dir.
     * unk_254 is the start of aim, the segment that its lock-on searches and
     * its laser sight follows. a black smoke can stand at any of the three.
     * a Tails's draw keeps the node of his tails at unk_23c, and its way down
     * at gun_dir */
    NJS_POINT3 unk_23c;
    NJS_POINT3 unk_248;
    NJS_POINT3 unk_254;
    /* where the record is a mech's: the point its 2P laser starts from. its
     * draws set it on the -x axis of a node of its model: 10 out from the
     * cockpit's for Eggman's walker and the dark Chao walker, 20 out for
     * Tails's, and 10 out from the gun turret's for the Chao walker */
    NJS_POINT3 laser_origin;
    /* ways in the world, as the draw of an Eggman, a hunter or a Tails sets
     * them from nodes of its model: unk_26c and unk_284 the z and -y axes
     * of the node placed at unk_1e8, unk_278 and unk_290 the -z and -y axes
     * of the one at unk_1f4, and unk_29c and unk_2a8 the -y axes of those
     * at unk_200 and unk_20c. a mech's draws set three of them, which
     * nothing of a mech's reads: unk_26c to the x axis of the node at
     * unk_1e8, and unk_29c and unk_2a8 to the way down at its feet, unk_200
     * and unk_20c. the two bursts of sparks of a Sonic's or a Shadow's
     * board lie apart along unk_29c. sonic_hold_place puts the task a
     * Sonic holds by unk_1e8 and unk_1f4, set off along unk_26c, unk_284
     * and unk_290 */
    NJS_VECTOR unk_26c;
    NJS_VECTOR unk_278;
    NJS_VECTOR unk_284;
    NJS_VECTOR unk_290;
    NJS_VECTOR unk_29c;
    NJS_VECTOR unk_2a8;
    /* the speed of the powder a hit gives off at unk_218. a mech's draws
     * set it to the x axis of its cockpit's node, at unk_218. the jets of
     * Tails's walker start out along it from unk_218, and blow against it.
     * a hunter's draw sets it to its head's x axis */
    NJS_VECTOR unk_2b4;
    /* the way a white breath puffs out from unk_224: a hunter's draw sets
     * it to its mouth's x axis */
    NJS_VECTOR unk_2c0;
    /* a way in the world: an Eggman's draw, a mech's and a hunter's set it
     * to the x axis of the node they place at unk_230. a Metal Sonic's jets
     * trail off along it */
    NJS_VECTOR unk_2cc;
    /* where the record is a mech's, its draws set these to the ways its
     * gun, its cannon and its aim point, as unk_23c says. aim is 800 long.
     * a Tails's draw sets gun_dir too, which nothing of his reads */
    NJS_VECTOR gun_dir;
    NJS_VECTOR cannon_dir;
    NJS_VECTOR aim;
    /* where the record is a mech's: the way its 2P laser points.
     * walker_laser_create's laser needs it to be a unit vector. its draws
     * set it to the -x axis of the node that laser_origin lies on */
    NJS_VECTOR laser_dir;
    /* each character's own fields, from here to the record's end: an arm for
     * each kind of record. a source reads the arm of the record's character,
     * which is its own or one it has tested */
    union {
        /* a Sonic's or a Shadow's, and Amy's and Metal Sonic's in their
         * place, whose code is player/sonic.c's */
        struct {
            /* the steps that made a dash, a bit each: SONIC_DASH_STEP. Sonic's
             * task sets bit 0 in a step of PLAYER_MODE_SONIC_HOMING, of
             * PLAYER_MODE_SONIC_LIGHT_ATTACK, and of PLAYER_MODE_ROLL at
             * running_speed or more, each beside its call of dash_create, and
             * shifts the bits up at each step's end */
            Sint8 dash_steps;
            /* the mode a combo's move goes on to once it ends */
            Sint8 combo_next;
            /* the homing attack's hits since it last ended, 0 to 2: each hit
             * plays the next of three animations, and the third starts it
             * over at 0, as sonic_test_jump's plain jump does */
            Sint8 homing_hits;
            /* the SONIC_TRICK_GRADE_ of its last trick. a landing from
             * PLAYER_MODE_SONIC_BOARD_TRICK earns a technique score by the
             * grade. so does one from PLAYER_MODE_SONIC_SOAP_JUMP or
             * PLAYER_MODE_SONIC_RAMP_JUMP. A earns 1000, B 500 and C 200, and
             * the others none */
            Sint8 trick_grade;
            /* the steps it spent under PLAYER_WORK_BALL. a spin, a light
             * attack and a totter on a rail count their steps in it too */
            Sint16 ball_steps;
            /* the steps its light dash has left: the dash ends once a step
             * finds it at 0 or below. a ring the dash finds leaves it 5 at
             * least */
            Sint16 light_dash_steps;
            /* the homing attack's state: -1 as it starts. homing_step makes
             * it 0 while it steers at a target, and 1 where it finds none,
             * or where on its first step the target lies against gravity.
             * from 1 on, it counts up each step. sonic_pick_mode makes a 0
             * that has stalled 1, and ends the attack at 15 */
            Sint16 homing;
            /* a count of steps that its modes keep, as mode_steps is. a
             * capture starts it at PLAYER_STRUGGLE */
            Sint16 steps;
            /* the steps the action button has stayed held, from 1 at its
             * press in a combo's wait or at a landing with it held, and 0
             * while it isn't counted. the body's shape attacks once it is 2
             * or more, and a hold long enough starts a spin */
            Sint16 hold_steps;
            /* Metal Sonic's jets: each step of PLAYER_MODE_RUN adds 5. each
             * step of the task gives metal_jet_create an eighth of it, held
             * to 8, and takes 1 from it, down to 0 */
            Sint16 jets;
            /* the rings its light dash has counted, from 0 at its start.
             * while it is 1 or less, a dash with no ring within 64 ends */
            Sint16 light_dash_rings;
            Uint8 unk_31a[2];
            /* two angles that turn by a fixed step in each step of the task
             * that picks a mode. the glow of UPGRADE_SONIC_FLAMERING's part
             * takes the first, and of UPGRADE_SONIC_RUBBERUNIT's the second */
            Angle flame_ring_glow;
            Angle bounce_glow;
            /* where unk_218 lay as the step before ended. the jiggle's param
             * grows with how far unk_218 moved since */
            NJS_POINT3 jiggle_pos;
            /* the pos of the last ring its light dash counted */
            NJS_POINT3 light_dash_ring_pos;
            /* the maker's jiggle, or null for Metal Sonic, who has none */
            struct jiggle *jiggle;
            /* its texture list, which a wind cutter, a Chaos Spear and a
             * Metal Sonic's jets draw with */
            NJS_TEXLIST *texlist;
            /* the model file and the motion file the maker read */
            struct model_entry *model_file;
            struct motion_entry *motion_file;
        } sonic;
        /* a hunter's: a Knuckles's or a Rouge's, or a Tikal's or a Chaos
         * 0's in their places. the GLIDE_ and CLIMB_ numbers are
         * player/knuckles.h's */
        struct {
            /* how much the glide has scraped. each entry into
             * PLAYER_MODE_HUNTER_GLIDE starts it at 0. each step of the
             * glide that reaches the scrape's test in the next mode takes 1
             * off, down to 0, and a scrape in the task's step then adds
             * GLIDE_SCRAPE. the test turns the glide to a fall once it is
             * GLIDE_SCRAPE_END or more */
            Sint8 glide_scrape;
            /* the mode an attack chain goes on to as its animation ends:
             * the chain's next attack, which a press of the action button
             * asks for, or PLAYER_MODE_RUN */
            Sint8 next_mode;
            /* the high byte of the stick's heading at each of the last
             * HUNTER_STICK_STEPS steps, or 0 for a step without the stick.
             * stick_history[stick_step & (HUNTER_STICK_STEPS - 1)] is the
             * latest, and hunter_offer_attack adds 1 to stick_step each
             * step */
            Sint8 stick_step;
            Sint8 stick_history[HUNTER_STICK_STEPS];
            Uint8 unk_34b;
            /* 60 once the stick's past turns a full circle one way, counted
             * down by 1 a step. the spiral is offered while it is above 0 */
            Sint16 spin_steps;
            /* the steps its task has run under PLAYER_WORK_BALL */
            Sint16 ball_steps;
            /* a count of steps that the hunter's modes keep, besides
             * mode_steps. a fall starts it at 0, and glides once it has
             * counted jump held for more than GLIDE_HOLD steps. an attack
             * counts its steps up from 0. a capture starts it at
             * PLAYER_STRUGGLE */
            Sint16 steps;
            /* each step of PLAYER_MODE_HUNTER_CLIMB at which
             * hunter_probe_steep returns TRUE sets it to CLIMB_GRACE. each at
             * which the probe returns FALSE takes 1 off, and the climb lets
             * go once it is 0 or less. no entry into the climb sets it: a
             * climb takes what the last one left. from the maker's 0, or after
             * a let-go, it lets go at its first step at which the probe
             * returns FALSE */
            Sint16 climb_grace;
            /* the glide's roll about the body's x. each draw in ANIM_GLIDE
             * eases it by up to GLIDE_ROLL_STEP toward minus the turn from
             * the work's ang.y to the motion's aim_y, and ANIM_GRAB_WALL
             * lessens it as it plays */
            Sint16 glide_roll;
            /* the steps of the digs. the debris flies and the dig sounds on
             * every eighth */
            Sint16 dig_steps;
            /* the steps PLAYER_MODE_HUNTER_TURN_BACK has left: 30 while
             * hunter_outside_kill_areas holds, and then counted down to the
             * glide at 0 */
            Sint16 turn_steps;
            /* Rouge's part: a second animation, of model_table's 190, that
             * her draw poses apart from her body. part_mode is
             * PLAYER_ANIM_LOOP while part_motion plays at part_frame, and
             * PLAYER_ANIM_BLEND while part_links blend from the last motion
             * and its shape into the next, as part_frame runs from 0 to 1.
             * Chaos 0's part takes PLAYER_ANIM_END, and stays */
            Sint16 part_mode;
            Sint32 part_motion; /* its slot of motion_table */
            /* how far the dig has sunk, from 0 down to -20, along the
             * ground's normal */
            Float dig_depth;
            Float part_frame;
            /* the turn of the sunglasses, or of Rouge's scope: 0 while they
             * are up, and EIGHTH_TURN once they are down over the eyes. the
             * draw turns them about z by it, less EIGHTH_TURN */
            Angle sunglasses;
            /* the work's pos where the last dig ended, or 0 from the
             * maker. a dig within 10 of it finds nothing */
            NJS_POINT3 dig_end;
            /* where unk_230 and unk_218 lay at the last step: the squash
             * and swing jiggles take how far each has moved since as their
             * kick */
            NJS_POINT3 squash_last;
            NJS_POINT3 swing_last;
            /* its jiggles, or null for one it lacks. Knuckles and Tikal have
             * a swing, Rouge a squash, and a swing but in costume 2. Chaos 0
             * has neither */
            struct jiggle *swing;
            struct jiggle *squash;
            /* its texture lists: its own, which an upgrade's model draws
             * with, and its effects', which the puffs of its upgraded blows
             * and a Rouge's black wave draw with. its maker loads both, the
             * effects' from K_EFFTEX for the looks of Knuckles and Tikal
             * and from R_EFFTEX for those of Rouge and Chaos 0, and its
             * task's destructor releases and frees them */
            NJS_TEXLIST *texlist;
            NJS_TEXLIST *effects_texlist;
            /* its files of models and motions, as model_file_read and
             * motion_file_read read them, and the links a Rouge's part
             * blends with: two motions that pose it, and two shapes that
             * move it */
            struct model_entry *models;
            struct motion_entry *motions;
            NJS_MOTION_LINK part_motion_link;
            NJS_MOTION_LINK part_shape_link;
        } hunter;
        /* a mech's, which runs to the end of its record */
        struct {
            /* 1 on a step whose held action button aims the lock-on, and 0
             * otherwise. a lock not yet released ends where it finds it 0.
             * at 1, the ground step turns the mech with
             * player_turn_at_speed where it would take player_turn_toward */
            Sint8 aiming;
            /* 1 on a step that lets one more of the released locks' shots
             * fly: the step sets it while shot_count is above 0, with 8 steps
             * between. the score task takes it back to 0 once it lets one
             * fly */
            Sint8 release_shot;
            /* 1 from a new lock until a step that aims finds lock_wait run
             * out. the lock-on's laser sight points at lock_pos while it is
             * 1, and along the record's aim otherwise */
            Sint8 laser_on_lock;
            /* 1 on a step on which the mech holds locks or has shots in
             * flight, in a mode that lets it fire, and 0 otherwise. in
             * PLAYER_MODE_HURT and the other modes that don't let it fire it
             * stays 0, whatever the mech holds. a released lock ends where
             * it finds it 0 */
            Sint8 has_locks;
            /* the steps before the cannon can fire again, 0 in a mode that
             * doesn't let it fire */
            Sint8 shot_wait;
            Uint8 unk_30d;
            /* what a mode counts. the laser's start sets it to 0, and each
             * step of PLAYER_MODE_MECH_LASER counts it up, until the mode
             * ends on the step that finds it past 160. in PLAYER_MODE_CAUGHT
             * each press of the stick or a button takes 20 from it, and the
             * mech stands once it is 0 or less */
            Sint16 mode_count;
            /* the held steps left in which the action button aims the
             * lock-on. a shot of the cannon sets it to 360, and each step
             * with the button held counts it down to 0. it keeps while the
             * button is up, and is 0 in a mode that doesn't let the mech
             * fire. walker_lockon_line_create dims its line as it falls */
            Sint16 lock_window;
            /* the steps that aim before the lock-on takes another lock: a
             * lock sets it to 10, and each step that aims counts it down */
            Sint16 lock_wait;
            /* the locks the mech holds, 32 at most, listed from locks. a
             * release hands them to a score task and adds them to
             * shot_count */
            Sint16 lock_count;
            /* the released locks whose shots have yet to end */
            Sint16 shot_count;
            Sint16 release_wait; /* the steps before release_shot is set */
            /* how hard the jets of Tails's walker blow. a step of its run
             * adds 6 and one of its hover 8, and each jet takes 1. the step
             * brings it back to 95 where it passes 96 */
            Sint16 jet_power;
            /* 1 once the run's footstep sound has played for a foot's
             * fall */
            Sint16 step_played;
            Uint8 unk_31e[2];
            /* the glow of the second 2P special, 0 for none. in a two-player
             * game or on STAGE_VERSUS_MECH_2, it fades in to 1 while the
             * second ring level is the player's, and out after while
             * game_state.solo_player is 0. the special sets it to 7 */
            Float glow;
            /* the frame the draws pose the laser cannon of Eggman's walker
             * at, held to its motion's last: 0 for shut */
            Float laser_frame;
            /* the work's pos.y as a hover began from a jump, or -1000000 for
             * one from a fall or a hit. on STAGE_COSMIC_WALL a hover lifts
             * the mech while it lies no more than 120 above it, and takes
             * -1000000 once it rises past */
            Float hover_floor;
            struct task *locks; /* the first of the mech's locks, or null */
            /* the name and step of the first lock's target, as the lock
             * took it */
            const char *first_target_name;
            void (*first_target_step)(struct task *task);
            /* the lock of another mech that holds this one, or null */
            struct task *locked_by;
            NJS_POINT3 lock_pos; /* where the latest lock's target lay */
            /* the place of a lock that waits on its target, which the turret
             * turns toward, or null. the step empties it on each step on
             * which released shots wait to fly */
            NJS_POINT3 *turret_aim;
            /* the points its 2P missiles leave from, in turn, and the way
             * they set off along. its draws set them in the world while its
             * glow is above 0 */
            NJS_POINT3 missile_points[PLAYER_MISSILE_POINTS];
            NJS_VECTOR missile_dir;
            /* its two jet nozzles, and the way each points, in the world.
             * a landing raises dust at each, and a black smoke can follow
             * either */
            NJS_POINT3 nozzle0_pos;
            NJS_POINT3 nozzle1_pos;
            NJS_VECTOR nozzle0_dir;
            NJS_VECTOR nozzle1_dir;
            Angle turret_ang; /* the gun turret's turn about y */
            /* the body's turn about y toward the stick as it aims */
            Angle body_turn;
            /* a turn that grows by 0x200 a step. the lamp beside the turret
             * draws with an alpha that follows its cosine */
            Angle pulse;
            /* the cannon's recoil and the gun's: a shot sets the cannon's to
             * 0x3000 and the gun's to 0x4000. each step then adds 0x1000 to
             * the cannon's and 0x2000 to the gun's, until the turn wraps to
             * 0. each muzzle draws drawn back by 1 less the turn's cosine */
            Angle cannon_recoil;
            Angle gun_recoil;
            /* the way down at the record's unk_23c, its gun's muzzle, which
             * its draws set. a lock's shot that heads straight away from
             * its target turns a tenth of the way along its shooter's, and
             * a homing shot along its victim's */
            NJS_VECTOR gun_down;
            /* the texture list of its gauge, of the explosions, flames and
             * ricochets made for its slot, and of the volley shots and
             * seekers fired at it. puffs next to each other in their level
             * draw with the list of the first one's slot. its laser draws
             * nothing while it is null. its maker loads WALKTEX into it, and
             * the lock-on's markers and shots draw with it */
            NJS_TEXLIST *effects_texlist;
            /* its own texture list, which its maker loads for its look and
             * its costume. its weapons and the upgrades it shows draw with
             * it too */
            NJS_TEXLIST *texlist;
            /* the model and motion files its maker read, which its
             * destructor frees */
            struct model_entry *models;
            struct motion_entry *motions;
        } mech;
        /* a Tails's, his task's extra of TAILS_RECORD_SIZE bytes */
        struct {
            /* his flight's flap. each step of the flight adds to it, and it
             * goes back to 0, with a flap's sound, once it reaches 64 */
            Sint8 flap;
            /* each unit of it adds half again to the flap's step. the
             * flight's step clears it, but under PLAYER_WORK_NO_CONTROL */
            Sint8 climb;
            Sint8 state; /* bits: TAILS_FLYING and TAILS_ATTACKING */
            /* bits: TAILS_30B_UNK_1, which the two calls at the end of his
             * object set and his draw reads */
            Sint8 unk_30b;
            /* the steps his task has run under PLAYER_WORK_BALL */
            Sint16 ball_steps;
            /* what's left of his struggle to break free of a capture, as
             * PLAYER_STRUGGLE says */
            Sint16 struggle;
            /* his draw sets it to 1 under TAILS_30B_UNK_1, and his choice
             * of mode to TAILS_UNK_310_LANDED as he lands from
             * PLAYER_MODE_TAILS_FLY. nothing in his object reads it */
            Float unk_310;
            /* how fast his tails spin about their x a step, and how far they
             * have spun */
            Angle spin_speed;
            Angle spin;
            Angle tail_tilt; /* the turn of his tails about their z */
            /* angles that PLAYER_MODE_TAILS_FLY eases toward his speed, and
             * that any other mode eases back to 0: tilt.x by spd.z and
             * tilt.z by accel.x. his draw turns his body by them in the
             * flight */
            struct angles tilt;
            NJS_POINT3 last_pos; /* his work's pos as his tails last moved */
            /* the tips of his tails, 6 to his +z and -z sides, as
             * tails_place_shapes last placed them in
             * PLAYER_MODE_TAILS_TAIL_ATTACK */
            /* @hack type: the tips as two fields, where an array of two
             * would do */
            NJS_POINT3 tip_plus_z;
            NJS_POINT3 tip_minus_z;
            Angle last_ang_y; /* his work's ang.y as his tails last moved */
            /* two jiggles of model TAILS_TAIL, his tails, mirrored about y,
             * each with a copy of its tail's model */
            /* @hack type: the tails as two fields, where an array of two
             * would do */
            struct jiggle *tail_a;
            struct jiggle *tail_b;
            NJS_TEXLIST *texlist; /* MILESTEX */
            struct model_entry *models; /* MILESMDL.PRS */
            struct motion_entry *motions; /* MILESMTN.PRS */
            /* the surfaces about a point TAILS_BEHIND_HEIGHT up his own y
             * from his pos, as his task probes them after the record's own
             * probes. the point's offset along his own x is tails_step's
             * local behind.x less his radius. nothing sets that local
             * first. his tails' jiggles take as param his pos.y's height
             * above its below_y, plus TAILS_BEHIND_HEIGHT */
            struct ground_probe behind;
        } tails;
        /* an Eggman's, his task's extra of EGGMAN_RECORD_SIZE bytes */
        struct {
            /* the steps his task has run under PLAYER_WORK_BALL */
            Sint16 ball_steps;
            Sint16 struggle; /* as PLAYER_STRUGGLE says */
            NJS_TEXLIST *texlist; /* EGGTEX */
            struct model_entry *models; /* EGGMDL.PRS */
            struct motion_entry *motions; /* EGGMTN.PRS */
        } eggman;
        /* Super Sonic and Super Shadow in Finalhazard */
        struct {
            /* the switch flash's frame, which cycles through six */
            Sint8 flash_frame;
            /* the steps the homing attack or the miss has run */
            Sint16 flight_steps;
            /* where his head was at the last step. the quills swing by its
             * travel since */
            NJS_POINT3 last_head;
            struct jiggle *quills;
            NJS_TEXLIST *textures;
            struct model_entry *models;
            struct motion_entry *motions;
        } super;
    } by_character;
};

/* the grades of a Sonic's tricks, best first, as its trick_grade keeps them.
 * a trick takes PASSED where placement_get_state of the task it jumped off
 * isn't 0. NONE is a board's jump too slow to be a trick */
#define SONIC_TRICK_GRADE_PASSED (-1)
#define SONIC_TRICK_GRADE_A 0
#define SONIC_TRICK_GRADE_B 1
#define SONIC_TRICK_GRADE_C 2
#define SONIC_TRICK_GRADE_NONE 3

/* in a Sonic's dash_steps: the bit of a dash made n shifts ago. a dash draws
 * its streak's first set of textures only while bits 1 and 2 are both set */
#define SONIC_DASH_STEP(n) (1 << (n))

/* a player's motion record. no source reads past aim_z yet, so its size is
 * unknown */
struct player_motion {
    NJS_VECTOR spd; /* the player's speed in the world */
    NJS_VECTOR acc;
    /* the angles the player turns toward: the ang.y it heads for, and the
     * ang.x and ang.z it tilts to */
    Angle aim_x;
    Angle aim_y;
    Angle aim_z;
};

/* the bits of a player's work's flags that the sources name. the collision step
 * sets PLAYER_WORK_ON_GROUND where it stands the player on a surface.
 * player_ground_step sets PLAYER_WORK_ON_SHAPE where it stands the player on
 * another task's collision shape, or on a SHAPE_ID_LINE shape. the collision
 * checks set two more: WORK_UNDER and WORK_ON_TOP */
#define PLAYER_WORK_ON_GROUND 0x0001
#define PLAYER_WORK_ON_SHAPE 0x0002
/* the player stands on a surface or on a shape */
#define PLAYER_WORK_GROUNDED (PLAYER_WORK_ON_GROUND | PLAYER_WORK_ON_SHAPE)
#define PLAYER_WORK_UNK_8 0x0008
/* the player is curled in a ball under PLAYER_WORK_BALL, and its body attacks
 * under PLAYER_WORK_ATTACK. under PLAYER_WORK_BALL, player_test_fall gives a
 * falling player PLAYER_MODE_JUMP, and player_collide takes
 * player_collide_unmasked's collision with the world. under
 * PLAYER_WORK_ATTACK, a Sonic's body shape takes an attack and a guard of 1,
 * unless PLAYER_INVINCIBLE, UPGRADE_UNK_15 or its mode sets them first.
 * enemy_bounce_player throws only a player under PLAYER_WORK_ATTACK */
#define PLAYER_WORK_BALL 0x0100
/* the player holds a charge: a Sonic's light charge, or a walker's laser
 * while it waits to fire */
#define PLAYER_WORK_CHARGED 0x0200
#define PLAYER_WORK_ATTACK 0x0400
#define PLAYER_WORK_HOLDING 0x0800 /* the player holds its record's held */
/* submode holds a request, an action that other code asks of the player's
 * step. the record's request_arg holds what goes with it */
#define PLAYER_WORK_REQUEST 0x1000
#define PLAYER_WORK_ON_PATH 0x2000 /* the player follows its record's path */
/* the player takes no stick and no jump. the reads of the stick and of jump
 * give FALSE, and the turns toward the stick change nothing. the accels that
 * the comment above player_accel_air names change nothing, and the others take
 * no stick, so player_accel_air still takes gravity. nothing in the tree sets
 * it, and PLAYER_REQUEST_RESET's answer clears it */
#define PLAYER_WORK_NO_CONTROL 0x4000
/* the choice of mode of a Tails, an Eggman, a mech and the other characters
 * sets it while the record is PLAYER_INVINCIBLE, and as player_take_hit
 * takes a hit. it clears it at a step whose timer has run down and that
 * takes no hit. no source in the tree reads it */
#define PLAYER_WORK_UNK_8000 0x8000

/* in the work's flags of a task that a player holds */
#define WORK_HELD_BY_PLAYER 0x8000

/* the modes of a player's work that the sources name. a PLAYER_MODE_UNK_
 * mode can mean one thing for one character and another for the next. an
 * action of a record's actions is numbered as the mode its setter starts */
enum player_mode {
    PLAYER_MODE_STAND = 0x00,
    PLAYER_MODE_RUN = 0x01, /* the player moves on the ground */
    /* the player runs along its record's path under PLAYER_WORK_ON_PATH. an
     * Eggman takes it as he answers PLAYER_REQUEST_RUN_PATH, and a Tails or
     * an Eggman leaves it for PLAYER_MODE_RUN below trot_speed */
    PLAYER_MODE_PATH_RUN = 0x02,
    PLAYER_MODE_ROLL = 0x04,
    /* a roll along the record's path, which a Sonic takes as it answers
     * PLAYER_REQUEST_BOOST_PATH. a hunter's step runs it too */
    PLAYER_MODE_PATH_ROLL = 0x05,
    PLAYER_MODE_JUMP = 0x06,
    /* the modes a player takes in ANIM_LAUNCH as it answers
     * PLAYER_REQUEST_BOUNCE, PLAYER_REQUEST_LAUNCH and PLAYER_REQUEST_FLING.
     * the bounce and the launch fall in PLAYER_MODE_FALL once the player
     * falls. the fling flies under gravity alone until the player lands. no
     * hunter or mech answers the fling */
    PLAYER_MODE_BOUNCE = 0x07,
    PLAYER_MODE_LAUNCH = 0x08,
    PLAYER_MODE_FLING = 0x09,
    PLAYER_MODE_FALL = 0x0a,
    /* a run off a ledge with a drop of more than 60 below: the player stops
     * where it stood two steps before, or three for a mech */
    PLAYER_MODE_TEETER = 0x0b,
    /* a Tails or an Eggman brakes where player_stick_back is TRUE: from a run
     * at trot_speed or more, or as he lands at any speed ahead above 0. once
     * spd.x is down to 0 he turns about and runs, while the stick is still
     * back. otherwise he stands once his animation leaves ANIM_BRAKE */
    PLAYER_MODE_BRAKE = 0x0c,
    PLAYER_MODE_UNK_0E = 0x0e,
    PLAYER_MODE_HURT = 0x0f, /* a hit throws the player back */
    /* a run of a Tails or an Eggman that player_collide ends in
     * PLAYER_CRASHED throws him back at PLAYER_CRASH_REBOUND of his speed.
     * an Eggman also takes it as he answers PLAYER_REQUEST_KNOCK_BACK */
    PLAYER_MODE_CRASH = 0x10,
    /* a standing Tails or Eggman slides on steep ground, as the constants'
     * slide_speed says: where the y of his ball's up is below
     * PLAYER_SLIDE_STEEP. he stands again once slower than slide_speed, or
     * where that y rises above PLAYER_SLIDE_LEVEL */
    PLAYER_MODE_SLIDE = 0x11,
    /* the player plays the animation a PLAYER_REQUEST_ANIM gives it, still,
     * until another request */
    PLAYER_MODE_PLAY_ANIM = 0x12,
    /* the pick-up of a task that player_action_flags gives ACTION_FLAG_200,
     * in ANIM_PICK_UP. from a frame of it, the player takes hold of the task
     * in PLAYER_MODE_STAND_HOLDING, or stands where it can't */
    PLAYER_MODE_PICK_UP = 0x13,
    PLAYER_MODE_UNK_14 = 0x14,
    /* the player stands holding a task, in the animation
     * player_hold_stand_anim chooses */
    PLAYER_MODE_STAND_HOLDING = 0x15,
    /* the player holds a task that player_action_flags gives ACTION_FLAG_400,
     * without a lift, for the stick to push as player_test_push says.
     * player_hold starts it. a player that stops while it holds such a task
     * takes it too */
    PLAYER_MODE_GRAB_PUSHABLE = 0x16,
    PLAYER_MODE_UNK_17 = 0x17,
    /* the action that lets go of the held task by player_release_and_stand,
     * which stands the player. no step takes it as a mode */
    PLAYER_MODE_LET_GO = 0x18,
    PLAYER_MODE_UNK_19 = 0x19,
    /* the throw of the held task on the ground, which player_throw_held
     * starts */
    PLAYER_MODE_THROW = 0x1a,
    PLAYER_MODE_UNK_1B = 0x1b,
    /* the fall and the jump of a player holding a task that
     * player_action_flags gives ACTION_FLAG_200. PLAYER_REQUEST_UPDRAFT
     * starts the fall too */
    PLAYER_MODE_FALL_HOLDING = 0x1c,
    PLAYER_MODE_JUMP_HOLDING = 0x1d,
    /* PLAYER_MODE_BOUNCE and PLAYER_MODE_LAUNCH of a player holding a task,
     * in ANIM_HOLD_JUMP. each falls in PLAYER_MODE_FALL_HOLDING once the
     * player falls */
    PLAYER_MODE_BOUNCE_HOLDING = 0x1e,
    PLAYER_MODE_LAUNCH_HOLDING = 0x1f,
    PLAYER_MODE_UNK_20 = 0x20,
    /* the throw of the held task in the air, which player_air_throw_held
     * starts */
    PLAYER_MODE_AIR_THROW = 0x21,
    PLAYER_MODE_UNK_22 = 0x22,
    /* the shake of a held FALL_KIND_NORMAL task, in ANIM_HOLD_SHAKE. a player
     * in PLAYER_MODE_STAND_HOLDING takes it where player_test_shake_held is
     * TRUE. it shakes the task as PLAYER_HELD_ACC says */
    PLAYER_MODE_SHAKE_HOLDING = 0x23,
    /* a hunter hangs from its record's path and moves along it, by
     * hunter_climb_step. in PLAYER_MODE_PATH_HANG the stick's heading picks
     * the way along the path. in PLAYER_MODE_PATH_HANG_GO the hunter keeps
     * the way it has, until it reaches a hold without the stick and stops in
     * PLAYER_MODE_PATH_HANG. nothing in the tree starts either from any mode
     * but these two */
    PLAYER_MODE_PATH_HANG = 0x24,
    PLAYER_MODE_PATH_HANG_GO = 0x25,
    PLAYER_MODE_CAUGHT = 0x26,
    /* the player hangs from a ledge ahead, below its edge, and climbs it in
     * PLAYER_MODE_LEDGE_CLIMB */
    PLAYER_MODE_LEDGE_HANG = 0x27,
    PLAYER_MODE_LEDGE_CLIMB = 0x28,
    /* the pull that player_offer_target_actions offers in place of
     * PLAYER_MODE_PICK_UP where the task isn't FALL_KIND_NORMAL. the player
     * pulls at the task until it gives up, as player_test_pull_heavy says, and
     * lifts nothing */
    PLAYER_MODE_PULL_HEAVY = 0x29,
    PLAYER_MODE_UNK_2A = 0x2a,
    /* the player strokes a Chao: a task named ChaonadeExec holds a shape of
     * id 2 at the record's unk_1e8 for as long as the mode lasts */
    PLAYER_MODE_PET_CHAO = 0x2b,
    /* player_throw_at_touch's throw of the held task, at the task of a
     * touched shape of SHAPE_ID_FACED, in ANIM_THROW_AT_TOUCH */
    PLAYER_MODE_THROW_AT_TOUCH = 0x2c,
    /* the flute's modes. the action a flute stand offers gives
     * PLAYER_MODE_PLAY_FLUTE, or PLAYER_MODE_PLAY_FLUTE_AGAIN at a stand
     * played before. each starts a task named psExecuteFlute. the step turns
     * PLAYER_MODE_PLAY_FLUTE_AGAIN to PLAYER_MODE_PLAYING_FLUTE_AGAIN, which
     * lasts while the flute's animations play. a player that lacks its
     * character's MELODY upgrade takes PLAYER_MODE_CANT_PLAY_FLUTE in place
     * of PLAYER_MODE_PLAY_FLUTE. PLAYER_TAILS and PLAYER_EGGMAN have none */
    PLAYER_MODE_PLAY_FLUTE = 0x2d,
    PLAYER_MODE_PLAY_FLUTE_AGAIN = 0x2e,
    PLAYER_MODE_PLAYING_FLUTE_AGAIN = 0x2f,
    PLAYER_MODE_CANT_PLAY_FLUTE = 0x30,
    /* the pull out of the ground of a task that player_action_flags gives
     * ACTION_FLAG_1000, a FALL_KIND_BURIED task, which player_pull_buried
     * starts. the lift takes hold of it, as sonic_test_pull says */
    PLAYER_MODE_PULL_BURIED = 0x31,
    /* the action a hint box and a search box offer. a player asks the box
     * for a hint by taking it */
    PLAYER_MODE_ASK_HINT = 0x32,
    PLAYER_MODE_HANG = 0x33,
    /* player_collide takes player_collide_unmasked's collision with the world
     * in this mode, as it does under PLAYER_WORK_BALL. a Sonic leaves it
     * once its ground probe's last_above_flags has no bit of WORLD_FLAT, the
     * water's, or game_state.time_stopped isn't 0 */
    PLAYER_MODE_SWIM = 0x34,
    PLAYER_MODE_UNK_35 = 0x35,
    PLAYER_MODE_EVENT = 0x36,
    /* what an Eggman takes as he answers PLAYER_REQUEST_DROWN. a Tails or an
     * Eggman in it rises toward the surface of water above him */
    PLAYER_MODE_DROWN = 0x37,
    PLAYER_MODE_UNK_38 = 0x38,
    /* the miss, which a hit gives the player once its request of
     * PLAYER_REQUEST_MISS waits */
    PLAYER_MODE_MISS = 0x39,
    /* a free move, which the characters' steps force where
     * player_free_move_forced returns nonzero, which it never does. a Tails or
     * an Eggman leaves it for PLAYER_MODE_RUN at a press of A */
    PLAYER_MODE_FREE_MOVE = 0x3a,
    PLAYER_MODE_UNK_3B = 0x3b,
    PLAYER_MODE_UNK_3F = 0x3f,
    PLAYER_MODE_UNK_43 = 0x43,
    PLAYER_MODE_HOLD_HANDLE = 0x53,
    /* Rouge's rides on the turtles of stg18/turtle.h: the ferry between its
     * stops, and the swimmer along its path */
    PLAYER_MODE_ROUGE_RIDE_FERRY = 0x56,
    PLAYER_MODE_ROUGE_RIDE_SWIMMER = 0x57,

    /* the hunters' own names for modes whose numbers other characters use
     * for others. a dig sinks the hunter into the ground or a wall and takes
     * its find. it rises out of the ground into PLAYER_MODE_JUMP, and out of
     * the wall into PLAYER_MODE_HUNTER_CLIMB. a _FAIL mode is a dig that the
     * ground or the wall refuses. a hunter in deep water floats, swims by the
     * stick, and dives while the action button is held. each attack chain runs
     * three moves, standing or on the run. a glide outside the kill areas turns
     * back in PLAYER_MODE_HUNTER_TURN_BACK */
    PLAYER_MODE_HUNTER_GLIDE = 0x3b,
    PLAYER_MODE_HUNTER_TURN_BACK = 0x3c,
    PLAYER_MODE_HUNTER_DIG = 0x3d,
    PLAYER_MODE_HUNTER_DIG_OUT = 0x3e,
    PLAYER_MODE_HUNTER_DIG_FAIL = 0x40,
    PLAYER_MODE_HUNTER_WALL_DIG = 0x41,
    PLAYER_MODE_HUNTER_WALL_DIG_OUT = 0x42,
    PLAYER_MODE_HUNTER_WALL_DIG_FAIL = 0x44,
    PLAYER_MODE_HUNTER_CLIMB = 0x45,
    PLAYER_MODE_HUNTER_CLIMB_OVER = 0x46,
    PLAYER_MODE_HUNTER_ATTACK_1 = 0x47,
    PLAYER_MODE_HUNTER_ATTACK_2 = 0x48,
    PLAYER_MODE_HUNTER_ATTACK_3 = 0x49,
    PLAYER_MODE_HUNTER_RUN_ATTACK_1 = 0x4a,
    PLAYER_MODE_HUNTER_RUN_ATTACK_2 = 0x4b,
    PLAYER_MODE_HUNTER_RUN_ATTACK_3 = 0x4c,
    PLAYER_MODE_HUNTER_SPIRAL = 0x4d,
    PLAYER_MODE_HUNTER_DIVE = 0x4e,
    PLAYER_MODE_HUNTER_FLOAT = 0x4f,
    PLAYER_MODE_HUNTER_SWIM = 0x50,
    PLAYER_MODE_HUNTER_WATER_DIVE = 0x51,

    /* Sonic's and Shadow's own names for modes whose numbers other characters
     * use for others. the bounce, which UPGRADE_SONIC_RUBBERUNIT gives, drops
     * in PLAYER_MODE_SONIC_BOUNCE and springs up in
     * PLAYER_MODE_SONIC_BOUNCE_UP. the combo runs three moves standing and
     * three running. PLAYER_MODE_SONIC_COMBO_WAIT waits between moves, and
     * PLAYER_MODE_SONIC_COMBO_BACK steps back. PLAYER_MODE_SONIC_RAIL rides a
     * rail from above and PLAYER_MODE_SONIC_RAIL_BENEATH from beneath.
     * PLAYER_MODE_SONIC_BOARD rides a board by player_accel_board. the board's
     * jump, trick, fall and crash return to it. the action button held brakes
     * the board, in PLAYER_MODE_SONIC_BOARD_BRAKE. PLAYER_MODE_SONIC_BOARD_FAST
     * rides at a higher top speed while jump or the action button is held, and
     * jumps as jump is let go. nothing in the tree starts it */
    PLAYER_MODE_SONIC_SPIN = 0x03,
    PLAYER_MODE_SONIC_HOMING = 0x0d,
    PLAYER_MODE_SONIC_LIGHT_DASH = 0x3b,
    PLAYER_MODE_SONIC_LIGHT_ATTACK = 0x3c,
    PLAYER_MODE_SONIC_COMBO_1 = 0x3d,
    PLAYER_MODE_SONIC_COMBO_2 = 0x3e,
    PLAYER_MODE_SONIC_COMBO_3 = 0x3f,
    PLAYER_MODE_SONIC_COMBO_WAIT = 0x40,
    PLAYER_MODE_SONIC_COMBO_BACK = 0x41,
    PLAYER_MODE_SONIC_COMBO_RUNNING_1 = 0x42,
    PLAYER_MODE_SONIC_COMBO_RUNNING_2 = 0x43,
    PLAYER_MODE_SONIC_COMBO_RUNNING_3 = 0x44,
    PLAYER_MODE_SONIC_BOUNCE = 0x45,
    PLAYER_MODE_SONIC_BOUNCE_UP = 0x46,
    PLAYER_MODE_SONIC_RAIL = 0x47,
    PLAYER_MODE_SONIC_RAIL_BENEATH = 0x48,
    PLAYER_MODE_SONIC_SOAP_JUMP = 0x49,
    PLAYER_MODE_SONIC_MAGIC_HAND = 0x4a,
    PLAYER_MODE_SONIC_RAMP_JUMP = 0x4b,
    PLAYER_MODE_SONIC_BOARD = 0x4c,
    PLAYER_MODE_SONIC_BOARD_FAST = 0x4d,
    PLAYER_MODE_SONIC_BOARD_BRAKE = 0x4e,
    PLAYER_MODE_SONIC_BOARD_FALL = 0x4f,
    PLAYER_MODE_SONIC_BOARD_JUMP = 0x50,
    PLAYER_MODE_SONIC_BOARD_TRICK = 0x51,
    PLAYER_MODE_SONIC_BOARD_CRASH = 0x52,

    /* the mechs' own names for modes whose numbers other characters use for
     * others. a walker with its booster upgrade hovers in
     * PLAYER_MODE_MECH_HOVER while jump stays held after a jump or a fall.
     * each walker strikes what lies close ahead, in its own mode. the 2P
     * specials take the walker's ring levels: PLAYER_MODE_MECH_SPECIAL_1
     * the first, PLAYER_MODE_MECH_SPECIAL_2 the second, and
     * PLAYER_MODE_MECH_LASER the third, whose 2P laser fires for as long as
     * the mode lasts */
    PLAYER_MODE_MECH_HOVER = 0x3c,
    PLAYER_MODE_EGGMAN_MECH_STRIKE = 0x3d,
    PLAYER_MODE_TAILS_MECH_STRIKE = 0x3e,
    PLAYER_MODE_MECH_SPECIAL_1 = 0x3f,
    PLAYER_MODE_MECH_SPECIAL_2 = 0x40,
    PLAYER_MODE_MECH_LASER = 0x41,

    /* a Tails's own names for modes whose numbers other characters use for
     * others: his flight, which tails_fly_start starts, and his tail
     * attack, which spins the tips of his tails about him. his hover answers
     * PLAYER_REQUEST_HOVER */
    PLAYER_MODE_TAILS_FLY = 0x3b,
    PLAYER_MODE_TAILS_TAIL_ATTACK = 0x3c,
    PLAYER_MODE_TAILS_HOVER = 0x3d,

    /* an Eggman's own name for a mode whose number other characters use for
     * others: his attack, an action he takes from his list.
     * eggman_set_shapes turns one of the attack's collision shapes on while
     * it lasts */
    PLAYER_MODE_EGGMAN_ATTACK = 0x3c
};

/* the requests the sources send. those the moves below send are named for
 * their moves. PLAYER_REQUEST_ANIM's arg is the animation the player then
 * plays. PLAYER_REQUEST_RELEASE takes an arg of 0. a source sends it once it
 * is done with a player it sent PLAYER_REQUEST_ANIM. player_end_event sends
 * it too, as do the cries of some characters as they fall or sink to a
 * miss, without PLAYER_REQUEST_ANIM */
#define PLAYER_REQUEST_RUN_PATH 1
#define PLAYER_REQUEST_BOOST_PATH 2
#define PLAYER_REQUEST_LAUNCH_SPEED 3
#define PLAYER_REQUEST_BOUNCE 4
/* the player falls in PLAYER_MODE_FALL, in ANIM_FREE_FALL. fun_8c0492d0
 * sends it */
#define PLAYER_REQUEST_FREE_FALL 5
#define PLAYER_REQUEST_LAUNCH 6
#define PLAYER_REQUEST_FLING 7
#define PLAYER_REQUEST_UPDRAFT 8
#define PLAYER_REQUEST_ANIM 9
/* a Sonic's or a hunter's step answers it: the player stops and spins in
 * PLAYER_MODE_PLAY_ANIM, as a ball that attacks. neither step reads its arg */
#define PLAYER_REQUEST_SPIN 10
#define PLAYER_REQUEST_RELEASE 15
/* a Sonic answers it rolling in PLAYER_MODE_ROLL, and a hunter jumping in
 * PLAYER_MODE_JUMP, each as a ball that attacks. a mech bounces, in
 * PLAYER_MODE_BOUNCE. a Tails and an Eggman have no answer. fun_8c049304
 * sends it */
#define PLAYER_REQUEST_LAUNCH_BALL 16
#define PLAYER_REQUEST_JOIN_PATH 17
#define PLAYER_REQUEST_HANG_PATH 18
#define PLAYER_REQUEST_LEDGE_PATH 19
/* what a reel sends the player who grabs its handle. the player's step
 * answers it with PLAYER_MODE_HANG in its work's mode. PLAYER_REQUEST_HANG's
 * arg is the animation the player then plays */
#define PLAYER_REQUEST_HANG 20
/* what a capturing bullet sends the player it catches, with an arg of 0. the
 * player's step answers it with PLAYER_MODE_CAUGHT */
#define PLAYER_REQUEST_CAPTURE 22
/* a Sonic's step and a hunter's answer it as they answer
 * PLAYER_REQUEST_CAPTURE, but with a voice of its own. an Amy plays her
 * capture voice for both. no other step answers it, and nothing in the tree
 * sends it */
#define PLAYER_REQUEST_UNK_23 23
#define PLAYER_REQUEST_KNOCK_BACK 24
/* an Eggman's step and a mech's answer it in PLAYER_MODE_UNK_35, and set the
 * record's underwater_steps to 0 */
#define PLAYER_REQUEST_BREATHE 25
#define PLAYER_REQUEST_HAND 26
/* what a BreathCounterP task requests as its player drowns, and a player
 * that meets a KILL_AREA_SINK area as it starts to sink */
#define PLAYER_REQUEST_DROWN 27
#define PLAYER_REQUEST_SINK 28
/* a Tails answers PLAYER_REQUEST_FLY with tails_fly_start. he answers
 * PLAYER_REQUEST_HOVER in PLAYER_MODE_TAILS_HOVER, which holds him still in
 * tails.c's ANIM_FLY_LOW, under TAILS_FLYING, as a body that attacks, until a
 * request or a hit. no other step answers either, and nothing in the tree
 * sends either */
#define PLAYER_REQUEST_FLY 29
#define PLAYER_REQUEST_HOVER 30
#define PLAYER_REQUEST_RAIL_PATH 31
#define PLAYER_REQUEST_RAIL_PATH_BENEATH 32
/* what a ghost sends a player it scares, with the animation the player then
 * plays as its arg. an Eggman's step and a mech's answer it in
 * PLAYER_MODE_UNK_35 */
#define PLAYER_REQUEST_SCARE 33
/* a request that puts a player on its board, with an arg of 0. a Sonic's step
 * answers it in PLAYER_MODE_SONIC_BOARD_FALL, with animation 129 */
#define PLAYER_REQUEST_BOARD 34
#define PLAYER_REQUEST_CURRENT_PATH 35
/* a request that resets the player. while it waits, the moves' requests
 * leave it in place. player_ground_step answers it: the player stands at
 * once, with no blend. player_reset makes it */
#define PLAYER_REQUEST_RESET 36
/* what player_miss requests */
#define PLAYER_REQUEST_MISS 37
/* the record's flags and the work's that player_reset clears. each answer to
 * the PLAYER_REQUEST_RESET it makes clears them too, beside flags of its own */
#define PLAYER_RESET_FLAGS                                                     \
    (PLAYER_BARRIER | PLAYER_MAGNETIC_BARRIER | PLAYER_MISSED |                \
     PLAYER_INVINCIBLE)
#define PLAYER_RESET_WORK_FLAGS                                                \
    (WORK_HURT | PLAYER_WORK_UNK_8 | WORK_UNDER | WORK_ON_TOP)

/* the steps a hit's timer runs. player_take_hit starts the work's timer at
 * PLAYER_HIT_STEPS. each character's choice of mode counts it down by one a
 * step, or sets it to 0 under PLAYER_INVINCIBLE. a Sonic's, a hunter's and a
 * mech's count only while game_state.solo_player is 0. the stick of a Sonic,
 * a Tails or an Eggman ends PLAYER_MODE_HURT on the ground once
 * PLAYER_HURT_STEER_STEPS or fewer are left */
#define PLAYER_HIT_STEPS 120
#define PLAYER_HURT_STEER_STEPS 60
/* a player's draw leaves out its body while its work's timer has this bit, so
 * the player blinks as a hit's timer counts down. the draw of a Sonic, a
 * hunter or a mech shows the body anyway while game_state.solo_player isn't
 * 0 */
#define PLAYER_TIMER_HIDDEN 0x2

/* a caught player's struggle, which each character keeps in its own arm of
 * the record. the answer of every character but a mech to
 * PLAYER_REQUEST_CAPTURE starts it at PLAYER_STRUGGLE. each step of
 * PLAYER_MODE_CAUGHT that the stick is pushed, or a button of
 * PLAYER_STRUGGLE_BUTTONS pressed, takes PLAYER_STRUGGLE_PUSH from it, and the
 * player stands once it is 0 or less. a mech counts its mode_count down the
 * same way, which no answer of its own starts */
#define PLAYER_STRUGGLE 180
#define PLAYER_STRUGGLE_PUSH 20
#define PLAYER_STRUGGLE_BUTTONS                                                \
    (PDD_DGT_TL | PDD_DGT_TR | PDD_DGT_TX | PDD_DGT_TY | PDD_DGT_KR |          \
     PDD_DGT_KL | PDD_DGT_KD | PDD_DGT_KU | PDD_DGT_ST | PDD_DGT_TA |          \
     PDD_DGT_TB)

/* the numbers that the characters' modes share. a mech's take only the
 * slide's, PLAYER_JUMP_LIFT, PLAYER_RISE and PLAYER_TEETER_STEPS */
/* the share of its speed at which the crashed run of a Sonic, a Tails or an
 * Eggman turns back */
#define PLAYER_CRASH_REBOUND 0.3F
/* a standing player slides where the y of its ball's up is below
 * PLAYER_SLIDE_STEEP. the slide ends where that y rises above
 * PLAYER_SLIDE_LEVEL */
#define PLAYER_SLIDE_STEEP 0.77F
#define PLAYER_SLIDE_LEVEL 0.98F
/* while player_count_jump_hold is TRUE, a jump adds PLAYER_JUMP_LIFT times the
 * constants' jump_lift to spd.y each step. it adds PLAYER_JUMP_HOLDING_LIFT
 * times it in PLAYER_MODE_JUMP_HOLDING */
#define PLAYER_JUMP_LIFT 0.8F
#define PLAYER_JUMP_HOLDING_LIFT 0.4F
/* a landing whose motion is slower than PLAYER_LAND_SLOW takes the slower of
 * the landing animations. that of a Sonic, an Eggman or a hunter slower than
 * PLAYER_LAND_STILL plays ANIM_LAND_STILL */
#define PLAYER_LAND_SLOW 2.5F
#define PLAYER_LAND_STILL 1.0F
/* the most PLAYER_MODE_DROWN rises a step */
#define PLAYER_RISE 0.4F
/* the PLAYER_MODE_TEETER of a Sonic, a hunter or a mech ends on the step that
 * finds mode_steps past PLAYER_TEETER_STEPS. it ends too at a push of the
 * stick, or a press of jump or the action button */
#define PLAYER_TEETER_STEPS 180
/* a roll along a path in PLAYER_MODE_PATH_ROLL, a Sonic's or a hunter's, curls
 * into a spin below PLAYER_PATH_ROLL_MIN */
#define PLAYER_PATH_ROLL_MIN 1.85F
/* the PLAYER_MODE_SHAKE_HOLDING of a Sonic, a Tails or a hunter sets each axis
 * of the held task's motion's acc to the animation's frame times
 * PLAYER_HELD_ACC */
#define PLAYER_HELD_ACC 0.01F
/* a Sonic's or a hunter's swing jiggle takes a phase of QUARTER_TURN at a
 * step that kicks it past PLAYER_SWING_KICK */
#define PLAYER_SWING_KICK 0.4F

/* the sound ids the characters share. PLAYER_SOUND_LAND and
 * PLAYER_SOUND_PICK_UP are entries of the character's own list, so each
 * character plays its own. PLAYER_MODE_UNK_0E stops PLAYER_SOUND_PUSH as it
 * ends, and so does the PLAYER_MODE_UNK_20 of a Sonic, a Tails or an Eggman,
 * though no character plays it */
#define PLAYER_SOUND_LAND 0x2003
#define PLAYER_SOUND_PICK_UP 0x2004
#define PLAYER_SOUND_THROW 0x8007
#define PLAYER_SOUND_PUSH ((int)0xffff000f)

/* the slots of the model table and of the motion table */
#define MODEL_SLOTS 395
#define MOTION_SLOTS 256

/* a model and the slot of the model table it takes. a list of them ends at
 * one whose index is -1 */
struct model_entry {
    Sint32 index;
    NJS_CNK_OBJECT *object;
};

/* a motion and the slot of the motion table it takes. a list of them ends at
 * one whose index is MOTION_ENTRY_END */
struct motion_entry {
    Sint16 index;
    Sint16 nodes; /* relocate_motion's nodes for motion */
    NJS_MOTION *motion;
};
#define MOTION_ENTRY_END (-1)

/* the slots of the players' tables */
#define PLAYER_SLOTS 8
/* the task_create level of a player's task */
#define PLAYER_LEVEL 1
/* the vibration_start power and freq of a player's pad at a crash, and at a
 * fast landing or any landing on a board or a rail */
#define CRASH_VIBRATION_POWER 3
#define CRASH_VIBRATION_FREQ 7
#define LANDING_VIBRATION_POWER 6
#define LANDING_VIBRATION_FREQ 15

/* the model table and the motion table, which the characters' models and
 * motions fill. each slot holds its own index, and a model or motion, or
 * null where the slot is free. players_init frees every slot */
extern struct model_entry model_table[MODEL_SLOTS];
extern struct motion_entry motion_table[MOTION_SLOTS];
/* each slot's task, and its task work, motion and record, null while the
 * slot is empty */
extern struct task *player_tasks[PLAYER_SLOTS];
extern struct task_work *player_work[PLAYER_SLOTS];
extern struct player_motion *player_motion[PLAYER_SLOTS];
extern struct player *player_records[PLAYER_SLOTS];
/* the record player_records[slot] holds, read through the slot's task */
#define PLAYER_RECORD(slot) ((struct player *)player_tasks[slot]->extra.p)

/* the character of the player in slot, one of the PLAYER_ characters, or -1
 * while the slot is empty */
int player_character(int slot);

/* an area of a stage that kills players, in kill_areas_start's list */
struct kill_area {
    /* the characters it kills, bit n for character n as stage_character
     * gives it, and the KILL_AREA_ flags */
    Uint32 flags;
    NJS_OBJECT *object; /* null ends the list */
};

/* a kill area's character bits that kill every character */
#define KILL_AREA_ALL_CHARACTERS 0xff

/* a kill area's flags. a player meets an area where a surface of the area's
 * object lies straight above it, within 30 of its pos. it then misses as
 * player_fall_out makes it miss. a player that meets a KILL_AREA_SINK area
 * requests PLAYER_REQUEST_SINK and sinks through it. it misses at the sink's
 * 122nd step, unless its work's mode leaves PLAYER_MODE_UNK_38 and
 * PLAYER_MODE_MISS first. a player that meets a KILL_AREA_WATER area splashes
 * into the water as it misses */
#define KILL_AREA_SINK 0x100
#define KILL_AREA_WATER 0x200
/* a surface below the player meets it too, and one above at any height */
#define KILL_AREA_BELOW 0x400
#define KILL_AREA_ABOVE 0x800 /* a surface above meets it at any height */
/* the task draws the object with njCnkEasyDrawObject, and the area kills
 * nobody */
#define KILL_AREA_DRAWN 0x80000000

/* the parameters of each character, PLAYER_SONIC to PLAYER_EGGMAN_MECH, and
 * of three more */
#define CHARACTER_KINDS 11
extern struct character_constants character_constants[CHARACTER_KINDS];

/* players_init empties the players' tables, and frees every slot of the model
 * table and the motion table. it reads PLCOMMTN.PRS, the motions the characters
 * share, and allocates the players' past, each where it hasn't yet. a read or
 * allocation that fails leaves its pointer null, and no call that uses it
 * checks. players_exit frees the three, each where it holds one */
void players_init(void);
void players_exit(void);

/* read the PRS file name, a list of models or motions, and relocate each
 * entry's object or motion. each entry goes to its slot of the model table or
 * the motion table. it goes there only where its index is in the table and the
 * slot is free. a motion takes its nodes with it. each returns the list, or
 * null where loader_read_prs_alloc fails.
 *
 * model_file_free and motion_file_free empty each slot that still holds an
 * entry of list's, then free list */
struct model_entry *model_file_read(const char *name);
void model_file_free(struct model_entry *list);
struct motion_entry *motion_file_read(const char *name);
void motion_file_free(struct motion_entry *list);

/* 0, whatever slot. where it isn't 0, the characters' steps force
 * PLAYER_MODE_FREE_MOVE */
int player_free_move_forced(int slot);
/* PLAYER_MODE_FREE_MOVE moves a player FREE_MOVE_STEP a step, once either axis
 * of the stick lies past PAD_DEAD_ZONE from its center */
#define FREE_MOVE_STEP 5.0F
/* a zeroed block of 0x308 bytes from syCalloc, or null where it fails */
void *fun_8c038e08(void);

/* make task the player in slot: put task, its work, motion and extra in the
 * players' tables. the extra must be a record whose character is set. the
 * record takes its character's constants, with less gravity and air resistance
 * on a stage in space. it takes a ball from syCalloc, unchecked, and room for
 * targets as player_targets_alloc gives it. the upgrades that are its
 * character's, and whose byte of game_flags isn't 0, set their bits in
 * upgrades. the work's id becomes WORK_ID_PLAYER, and its scale 1.
 *
 * player_unregister empties slot in the four tables. it frees the ball, the
 * blend the record's anim allocated, and the targets, each where it isn't null.
 * the task is left as it was. slot must hold a player, unchecked */
void player_register(int slot, struct task *task);
void player_unregister(int slot);

/* the bookkeeping that ends a player's step. it drops a request of
 * PLAYER_WORK_REQUEST, and clears PLAYER_WORK_UNK_8, WORK_UNDER and
 * WORK_ON_TOP. it zeroes the motion's acc and the record's slip, push and
 * accel. it counts stick_lock and confused_steps down to 0. it records the
 * work's pos and ang as the latest of the player's past. it empties faced, and
 * drops an action of PLAYER_MODE_ASK_HINT.
 *
 * it counts the steps the player's eyes spend under water. the water is where
 * the ground probe's last_above_flags has a bit of WORLD_FLAT. the count starts
 * over where they aren't. at the 1140th step it starts the player's breath
 * task, BreathCounterP. no player counts while game_state.cleared is 1, nor one
 * that has missed, nor a Knuckles with UPGRADE_KNUCKLES_AIR */
void player_finish_step(struct task_work *work, struct player_motion *motion,
                        struct player *record);

/* the action flags, which say what a task the player holds or is offered lets
 * it do. player_action_flags gives the flags of the task the player holds, or
 * else of its offer, its record's hold_offer. without an offer, the nearest
 * task the record touched within 0x3000 of the player's heading becomes the
 * offer. its work must have FALL_OFFERABLE, lack WORK_HELD_BY_PLAYER, and give
 * flags. it reads an offer only while the player's collision record has
 * COLLISION_TOUCHED and COLLISION_SIDE, and its work lacks WORK_HURT. a held
 * task becomes the offer where there is none. it gives 0 where task_kill has
 * ended the held task, or where it lacks a motion, a work or a collision
 * record.
 *
 * the flags come by the task's work's id, for PLAYER_SONIC to PLAYER_ROUGE
 * alone. FALL_KIND_NORMAL and FALL_KIND_HEAVY give ACTION_FLAG_200,
 * FALL_KIND_SHAKE ACTION_FLAG_800 and FALL_KIND_BURIED ACTION_FLAG_1000.
 * FALL_KIND_11 gives ACTION_FLAG_400, as SHAPE_ID_NO_ACTION_400 says */
#define ACTION_FLAG_200 0x200
#define ACTION_FLAG_400 0x400
#define ACTION_FLAG_800 0x800
#define ACTION_FLAG_1000 0x1000
int player_action_flags(struct task_work *work, struct player *record);
/* ACTION_FLAG_200 where the task the player holds is FALL_KIND_NORMAL, and 0
 * otherwise. without a held task it tests its record's hold_offer, as
 * player_action_flags reads it, and finds no offer of its own. a held task
 * without a motion, a work or a collision record gives 0 */
int player_normal_kind_flag(struct task_work *work, struct player *record);
/* TRUE where the player's offer, found as player_action_flags finds one, is
 * FALL_KIND_SHAKE */
Bool player_touches_shakable(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record);
/* set *dir to the unit vector from the player's offer, found as
 * player_action_flags finds one, to the player, and return TRUE. it returns
 * FALSE without an offer, leaving *dir, or where the two share a pos, with
 * *dir the zero vector */
Bool player_dir_from_offer(struct task_work *work, struct player *record,
                           NJS_VECTOR *dir);
/* push the player's FALL_KIND_SHAKE offer away from the player. the offer's
 * motion's acc takes the level way from the player to it. its length runs from
 * 0 up to 1 and back as the anim's current animation plays, which must have a
 * motion, unchecked */
void player_shake_touched(struct task_work *work, struct player_motion *motion,
                          struct player *record);
/* turn the player toward its offer, found as player_action_flags finds one.
 * where the offer lies at its reach or more, step the player 1 nearer. the
 * reach is a length for the offer's kind, plus the radius of the player's first
 * shape. it is measured across x and z. it returns TRUE. it returns FALSE
 * without an offer. after the turn, it returns FALSE too where the offer lacks
 * a collision record or a FALL_KIND_ id */
Bool player_approach_touched(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record);
/* the last task the player's record touched at a side, of those whose
 * touched shape lacks SHAPE_UNK_2000 and whose own record has
 * COLLISION_SIDE, or null. it is null unless the player's record has
 * COLLISION_SIDE and COLLISION_PUSHED, and its work lacks WORK_HURT */
struct task *player_last_side_touch(struct task_work *work,
                                    struct player *record);
/* TRUE where the work's angles turn its up axis upward. the up is (0, 1, 0),
 * turned by them and then by Ninja's current matrix, and its y must be above
 * 0. it is FALSE on STAGE_METEOR_HERD */
Bool player_upright(struct task_work *work);

/* the ids of collision shapes that the players' steps read. a touch of
 * SHAPE_ID_FACED turns player_turn_to_touch's player toward its center. a
 * FALL_KIND_11 task gives ACTION_FLAG_400 unless the player touches a shape of
 * SHAPE_ID_NO_ACTION_400. a touch of SHAPE_ID_ACTION_400, or a held task,
 * gives the flag even then. no shape of SHAPE_ID_NO_CRASH crashes a player. a
 * touched task whose first shape has SHAPE_ID_NO_OFFER is never offered */
#define SHAPE_ID_FACED 1
#define SHAPE_ID_NO_ACTION_400 12
#define SHAPE_ID_ACTION_400 13
#define SHAPE_ID_NO_CRASH 14
#define SHAPE_ID_NO_OFFER 16
/* the id of a collision shape that stands for a line along its task's y
 * axis, turned by the task's ang.y and ang.z. player_ground_step stands a
 * player on it, and player_read_stick steers along it */
#define SHAPE_ID_LINE 15
/* more ids of collision shapes that the players' steps read. a hit that
 * counts against a player touching a shape of SHAPE_ID_THROWS throws it away
 * from that shape's task. a touch of SHAPE_ID_NO_STAND keeps a player off
 * every task's shape. a held task whose first shape has SHAPE_ID_NO_SET_DOWN is
 * never offered a setting down. a walker's strike offer passes over a task
 * whose first shape has SHAPE_ID_NO_STRIKE. a task with a shape of
 * SHAPE_ID_PET is one a player can pet */
#define SHAPE_ID_NO_SET_DOWN 6
#define SHAPE_ID_THROWS 7
#define SHAPE_ID_NO_STAND 9
#define SHAPE_ID_NO_STRIKE 10
#define SHAPE_ID_PET 146

/* TRUE where the player takes its stick this step. it is FALSE under
 * PLAYER_WORK_NO_CONTROL, while the stick's stroke is 0, and while stick_lock
 * isn't 0 off a path. on TRUE, *strength takes the stroke, and *angle the
 * heading the stick asks for, where each isn't null. while confused_steps is
 * above 0, the heading is one of four others, by the step.
 *
 * on STAGE_MAD_SPACE *angle keeps that heading. on any other stage, a touched
 * shape of SHAPE_ID_LINE points the heading along its line, one way or the
 * other. where the line stands within 0.99 of upright, slot 0's pad picks
 * the way by the sign of its y1. otherwise the way is the one on the
 * stick's side of the line. without such a shape, the heading is mirrored
 * while world_gravity.y is above 0.1 */
Bool player_read_stick(struct task_work *work, struct player *record,
                       Angle *angle, Float *strength);
/* TRUE where the player takes its stick, as player_read_stick says, and
 * the stick asks for a heading more than 0x6000 from the work's ang.y. it
 * is FALSE under PLAYER_WORK_ON_PATH or PLAYER_WORK_NO_CONTROL, and while the
 * player touches a shape of SHAPE_ID_LINE */
Bool player_stick_back(struct task_work *work, struct player *record);

/* TRUE at a press of jump. FALSE under PLAYER_WORK_NO_CONTROL */
Bool player_jump_pressed(struct task_work *work, struct player *record);
/* TRUE while jump is held and the record's jump_steps, which each such
 * call counts up, was below its constants' jump_hold_steps. FALSE under
 * PLAYER_WORK_NO_CONTROL */
Bool player_count_jump_hold(struct task_work *work, struct player *record);

/* the grades of how far a heading lies from a way. a heading grades
 * PLAYER_WALL_AWAY past 0x6000, and PLAYER_WALL_ACROSS past 0x3400. it grades
 * PLAYER_WALL_ASKEW past 0x1c00 for player_face_offer, player_stick_into_wall
 * and player_face_wall, and past 0x1400 for the others. nearer, it grades
 * PLAYER_WALL_HEAD_ON. PLAYER_WALL_NONE is for nothing to grade */
enum player_wall {
    PLAYER_WALL_NONE,
    PLAYER_WALL_AWAY,
    PLAYER_WALL_ACROSS,
    PLAYER_WALL_ASKEW,
    PLAYER_WALL_HEAD_ON
};
/* the graders. the way toward the offer runs from the player to its offer, as
 * player_dir_from_offer finds it. without an offer it runs against the normal
 * that collision_get_normal gives for the player's collision record. where
 * that gives none, it runs against the vector's last value.
 *
 * player_face_offer grades the player's heading against the way toward its
 * offer, and player_stick_into_offer the stick's. player_stick_into_wall and
 * player_face_wall grade the stick and the heading against wall_normal,
 * reversed, and give PLAYER_WALL_NONE where wall_normal is zero.
 * player_stick_ahead grades the stick against the heading. a grader of the
 * stick gives PLAYER_WALL_NONE where player_read_stick is FALSE.
 * player_move_into_offer grades the motion's spd, reversed, against a vector
 * it never sets */
int player_face_offer(struct task_work *work, struct player_motion *motion,
                      struct player *record);
int player_stick_into_wall(struct task_work *work, struct player *record);
int player_face_wall(struct task_work *work, struct player *record);
int player_stick_into_offer(struct task_work *work, struct player *record);
int player_move_into_offer(struct task_work *work, struct player_motion *motion,
                           struct player *record);
int player_stick_ahead(struct task_work *work, struct player *record);

/* TRUE where a target lies near the player in slot, in near_targets for
 * slot 0 and near_targets_player1 for any other. FALSE on STAGE_CHAO */
Bool player_has_target(int slot);
/* turn the player toward the nearest of its near targets that is an item or an
 * enemy. a target whose work is FALL_KIND_NORMAL is passed over. the motion's
 * aim_y takes the heading toward the target, mirrored while world_gravity.y is
 * above 0. ang.y turns toward it, and the record's faced takes the target's
 * task. a target whose task's motion is null leaves the turn to
 * player_turn_to_stick_at_speed. without a target the player keeps its heading,
 * but on STAGE_CHAO, where it turns by its stick.
 *
 * player_turn_to_target turns by up to 0x1000 a step, and player_face_target at
 * once. player_turn_to_landed_target and player_turn_to_target_ahead turn by up
 * to 0x400, and turn by the stick with player_turn_to_stick_at_speed.
 * player_turn_to_landed_target leaves to the stick a target whose enemy_work
 * lacks ENEMY_ON_FLOOR. player_turn_to_target_ahead turns toward no target more
 * than 0x2c00 off its heading */
void player_turn_to_target(struct task_work *work, struct player_motion *motion,
                           struct player *record);
void player_turn_to_landed_target(struct task_work *work,
                                  struct player_motion *motion,
                                  struct player *record);
void player_turn_to_target_ahead(struct task_work *work,
                                 struct player_motion *motion,
                                 struct player *record);
void player_face_target(struct task_work *work, struct player_motion *motion,
                        struct player *record);
/* turn the player by up to 0x1000 toward the center of a touched shape of
 * SHAPE_ID_FACED. collision_next_with_id_hit finds the touch. the motion's
 * aim_y takes the heading, and the record's faced the touched task. without
 * such a touch it turns by its stick, with player_turn_to_stick */
void player_turn_to_touch(struct task_work *work, struct player_motion *motion,
                          struct player *record);

/* the acceleration of a player on the ground, in the player's own axes. the
 * record's accel takes the motion's acc with gravity, the slope's pull,
 * resistance, the stick's push and the ground's grip. drive takes the push,
 * and slip what of it the grip doesn't hold. the stick turns the player toward
 * the heading it asks for, and steers it along a wall it touches. without the
 * stick the player slows by ground_drag, or rolls downhill on steep ground.
 *
 * on GROUND_CLING ground the player falls into the ground in place of along
 * world_gravity. the pad then turns it across the ground's face, as the
 * player's camera sees it. on WORLD_UPRIGHT ground the slope pulls nothing */
void player_accel_ground(struct task_work *work, struct player_motion *motion,
                         struct player *record);

/* the other acceleration steps. each sets the record's accel from the
 * motion's acc, in the player's own axes, as its comment says. under
 * PLAYER_WORK_NO_CONTROL, player_accel_push, player_accel_drive,
 * player_accel_glide, player_accel_climb, player_accel_board,
 * player_accel_board_brake and the four from player_accel_brake to
 * player_accel_gravity change nothing, and the others take no stick.
 *
 * player_accel_air takes gravity, resistance and the stick's push ahead. a
 * stick more than 0x6000 from the heading of a player faster than
 * running_speed brakes by air_brake. one within 0x1000 pushes by air_accel,
 * twice that while the motion falls, and one between only aims the player.
 * the stick turns it by player_turn_toward. player_accel_air_weightless is
 * the same without gravity, and pushes no more while falling */
void player_accel_air(struct task_work *work, struct player_motion *motion,
                      struct player *record);
void player_accel_air_weightless(struct task_work *work,
                                 struct player_motion *motion,
                                 struct player *record);
/* along the player's path. gravity pulls into the ground, and resist.x
 * holds back a speed ahead past 5, or below 0. the record's path_distance
 * takes the place along the path nearest the player, and the player turns
 * by player_turn_on_course toward the way the path runs there. drive takes
 * the stick's push across, by its stroke and the pad's way, and accel.z
 * gains it */
void player_accel_path(struct task_work *work, struct player_motion *motion,
                       struct player *record);
/* pushing or pulling: the player faces its offer, or the wall where it has
 * none. accel.x alone takes the push. a stick that grades PLAYER_WALL_HEAD_ON
 * against the offer pushes ahead by ground_accel times its stroke, and one
 * that grades PLAYER_WALL_AWAY pulls back by as much. any other slows the
 * player by ground_drag. a still player held by the ground's still_grip
 * doesn't move */
void player_accel_push(struct task_work *work, struct player_motion *motion,
                       struct player *record);
/* gravity, half the resistance, and the pad's axes. x1 turns the player by
 * 4 a step for each unit. y1, moved 12 toward 0, pushes ahead by air_accel
 * times its square over 115 squared, at most 1. a y1 above 12 pushes none */
void player_accel_drive(struct task_work *work, struct player_motion *motion,
                        struct player *record);
/* a twentieth of gravity and half the resistance. the stick pushes ahead by
 * air_accel times its stroke, and turns the player by player_turn_toward */
void player_accel_glide(struct task_work *work, struct player_motion *motion,
                        struct player *record);
/* the pad alone: accel.x takes 0.8 of the pad's y1, reversed, and accel.z
 * 0.8 of its x1, each over 127 and at most 1. a WORK_ID_RIVAL player takes
 * its stick's heading and stroke in place of the pad. without the stick,
 * accel is zero. on GROUND_CLIMB_REVERSED ground, accel.z reverses where the
 * player's camera looks along the player's up */
void player_accel_climb(struct task_work *work, struct player_motion *motion,
                        struct player *record);
/* on a board. gravity pulls, and resist.x holds back the board's speed while it
 * runs ahead past top, or back. the stick aims the board off its heading, by a
 * step that shrinks with the speed. it pushes ahead while the board is no
 * faster than trot_speed. the speed then eases toward the way the board
 * points, and the push takes the ground's grip */
void player_accel_board(struct task_work *work, struct player_motion *motion,
                        struct player *record, Float top);
/* a board in the air: player_accel_air's push and brake, by
 * player_turn_at_speed. without the stick, a board no faster than
 * trot_speed turns by up to 0x200 a step toward the way down the ground
 * below */
void player_accel_board_air(struct task_work *work,
                            struct player_motion *motion,
                            struct player *record);
/* gravity and 0.05 more, and resist.y and resist.z. the aim keeps the
 * heading */
void player_accel_board_trick(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record);
/* a board braking, which takes nothing of the stick. it turns toward the way
 * its speed runs, then toward the way down. its speed eases toward the way the
 * board then points, and brakes by a share of itself. the push takes the
 * ground's grip */
void player_accel_board_brake(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record);
/* a player braking. gravity and resistance, and on the ground brake_drag
 * against the speeds ahead and across, scaled by grip_scale, with the
 * ground's grip. there a still axis that still_grip holds stays still, and
 * a moving one loses besides moving_grip ahead or side_grip across, of the
 * push into the ground. no brake reverses a speed. the player then turns
 * toward the motion's aim_y by player_turn_toward */
void player_accel_brake(struct task_work *work, struct player_motion *motion,
                        struct player *record);
/* gravity and resistance, and on the ground the grip of player_accel_brake
 * without its brake. on the ground it also takes the push of the surface
 * the collision ball last met: the hit's normal_speed, divided by top_speed
 * over five sevenths of gravity, joins accel.y and accel.z in the player's
 * axes, and the hit's normal_speed becomes 0 */
void player_accel_launched(struct task_work *work, struct player_motion *motion,
                           struct player *record);
/* gravity and resistance, with no grip, and on the ground the surface's push
 * as player_accel_launched takes it. faster than running_speed with a level
 * below 0, the player's accel.y reverses and grows eightfold. under
 * PLAYER_WORK_BALL, at a level of 0.98 or less, the speed ahead loses
 * 0.0002 of itself in place of resist.x */
void player_accel_roll(struct task_work *work, struct player_motion *motion,
                       struct player *record);
/* gravity alone */
void player_accel_gravity(struct task_work *work, struct player_motion *motion,
                          struct player *record);

/* a miss. each way to it below starts a miss's task, whose miss takes effect at
 * its 122nd step. the task ends first where game_state.cleared turns 1 or the
 * work's mode turns PLAYER_MODE_EVENT. in a one-player game it ends too where
 * the other player has missed already. the other player is slot 1's for slot 0,
 * and slot 0's for any other slot.
 *
 * the miss sets PLAYER_MISSED. in a one-player game it ends the stage in
 * STAGE_END_MISS, or for a WORK_ID_RIVAL work clears the stage for the other
 * player. in a two-player game whose slot 0 holds a mech, it clears the stage
 * for the other player. in any other two-player game, it puts the player back,
 * as player_reset and stage_place_player do. it then takes the player's rings,
 * but for a sink's miss, and awards it a ring level.
 *
 * player_miss makes the player in slot miss, unless its gauge is above 0. it
 * requests PLAYER_REQUEST_MISS at once, past any waiting request, and sets
 * PLAYER_MISSED. in a one-player game, or one whose slot 0 holds a mech,
 * pause_disable closes the pause menu and the stage's clock stops. in a
 * one-player game whose other player has missed already, it misses nothing, and
 * sets the gauge to 0.01 instead. slot must hold a player, unchecked */
void player_miss(int slot);
/* the player in slot falls out of the stage. its camera stops following it, and
 * it cries out and misses. a Knuckles, a Rouge, a mech, a Tical or a Chaos 0
 * also requests PLAYER_REQUEST_RELEASE. the miss's task is kill_areas_task's
 * child where that task lives. in a one-player game, pause_disable closes the
 * pause menu and the stage's clock stops. a player that has missed already
 * falls too, and an empty slot's null record is read */
void player_fall_out(int slot);
/* reset the player in slot for a restart, where its slot holds a task. it
 * requests PLAYER_REQUEST_RESET, and clears its collision record's touches, its
 * work's timer, WORK_HURT and PLAYER_WORK_UNK_8 to WORK_ON_TOP. its
 * ground_flags become 0. it loses its barriers, its invincibility,
 * PLAYER_MISSED and what it holds. a mech takes its gauge anew. a player that
 * had missed in a one-player game costs player 0 a life */
void player_reset(int slot);

/* start the task KillPlayerFallingDownStageP, which makes the players in slots
 * 0 and 1 that meet its areas miss, as kill_area says. it draws its
 * KILL_AREA_DRAWN areas. it passes over a player whose miss has begun, and one
 * that has missed. on STAGE_SAND_OCEAN, STAGE_HIDDEN_BASE, STAGE_SAND_OCEAN_2P
 * and the golem bosses, a player that has missed stays sunk in a
 * KILL_AREA_SINK area. nothing happens while game_state.cleared is 1.
 *
 * kill_areas_start returns TRUE, or FALSE where task_create or syCalloc fails,
 * which leaves no task. kill_areas_task holds the latest task, and kill_areas
 * its areas, which must last as long as the task. both are null once a task is
 * destroyed */
Bool kill_areas_start(struct kill_area *areas);
extern struct task *kill_areas_task;
extern struct kill_area *kill_areas;

/* ask the step of the player in slot to take request: set
 * PLAYER_WORK_REQUEST in its work's flags, submode to request, and the
 * record's request_arg to arg. a waiting request is replaced. the step's
 * character decides what a request does. nothing happens for an empty
 * slot */
void player_request(int slot, int request, int arg);

/* the moves, from here to player_set_acc, need a player in slot, unchecked.
 * a move that requests sets PLAYER_WORK_REQUEST and its request in the
 * work's submode, past any waiting request, and leaves request_arg as it
 * was. it makes no request, and changes nothing, while PLAYER_REQUEST_RESET
 * waits. player_knock_back and fun_8c049304 request even then, and
 * player_knock_back sets request_arg to 0 */

/* request PLAYER_REQUEST_KNOCK_BACK, and turn the player to face against
 * (x, 0, z) as player_face_along turns it. the player takes the speed
 * (x, y, z) as player_set_speed gives it. player_knock_apart knocks the
 * players in slot and other back from each other at speed, level, along the
 * way between their pos */
void player_knock_back(int slot, Float x, Float y, Float z);
void player_knock_apart(int slot, int other, Float speed);

/* set the motion's spd to (x, y, z), in the world's axes. the record's spd
 * takes the same speed in the player's own axes, where an x or z within
 * 0.001 of 0 becomes 0. player_set_local_speed takes (x, y, z) in the
 * player's own axes */
void player_set_speed(int slot, Float x, Float y, Float z);
void player_set_local_speed(int slot, Float x, Float y, Float z);
/* PLAYER_REQUEST_BOUNCE with player_set_speed */
void player_bounce(int slot, Float x, Float y, Float z);
/* player_launch_speed with spd (0, speed, 0), but requesting
 * PLAYER_REQUEST_LAUNCH in place of PLAYER_REQUEST_LAUNCH_SPEED */
void player_launch(int slot, Float speed, struct angles *ang, Sint16 lock);
/* PLAYER_REQUEST_FLING, and level the player to head along dir's x and z.
 * the motion's spd becomes dir times speed, and the record's spd the same in
 * the player's axes */
void player_fling(int slot, Float speed, NJS_VECTOR *dir);
/* PLAYER_REQUEST_LAUNCH_SPEED. stick_lock takes lock, and the record's spd
 * takes spd, in the player's own axes. the work's angles come from ang as
 * player_set_angles sets them. the motion's spd becomes spd turned into the
 * world's axes */
void player_launch_speed(int slot, NJS_VECTOR *spd, struct angles *ang,
                         Sint16 lock);
/* player_launch_speed, but requesting PLAYER_REQUEST_FREE_FALL in place of
 * PLAYER_REQUEST_LAUNCH_SPEED */
void fun_8c0492d0(int slot, NJS_VECTOR *spd, struct angles *ang, Sint16 lock);
/* player_launch_speed with PLAYER_REQUEST_LAUNCH_BALL, which it requests while
 * PLAYER_REQUEST_RESET waits too */
void fun_8c049304(int slot, NJS_VECTOR *spd, struct angles *ang, Sint16 lock);
/* set the work's ang to (x, QUARTER_TURN - y, z), and the motion's aim_y to
 * its ang.y */
void player_set_angles(int slot, Angle x, Angle y, Angle z);
/* turn the player toward (x, y, z). ang.y and the motion's aim_y take the
 * heading atan2f(z, x), and ang.x and ang.z tilt the player toward y. z must
 * not be 0, nor x and y both: the angles divide by values that are 0 then */
void player_face_along(int slot, Float x, Float y, Float z);
/* PLAYER_REQUEST_UPDRAFT with player_set_acc. nothing happens while
 * game_state.cleared is 1 */
void player_updraft(int slot, Float x, Float y, Float z);
/* player_set_acc of (x, y, z) divided by its constants' gravity */
void player_push(int slot, Float x, Float y, Float z);
/* set the motion's acc to (x, y, z) */
void player_set_acc(int slot, Float x, Float y, Float z);

/* put the player in slot at (x, y, z), and make that the latest place of its
 * past, as player_get_past gives it. nothing happens for an empty slot */
void player_set_pos(int slot, Float x, Float y, Float z);

/* push the player in slot 0 out of work's first shape, to a gap of 0.2
 * between that shape and the player's first. the push runs along the way
 * between their centers, or along +x where those are one point. it moves the
 * player only where the two shapes, as spheres, overlap. both must have a
 * collision record, unchecked */
void player_push_out_of(struct task_work *work);
/* which of the players in slots 0 and 1 is near pos: 1 for slot 0, or else
 * 2 for slot 1, or 0 for neither. a player is near where its collision
 * record's first shape has its center nearer than range to pos. a slot
 * without a work or a collision record is passed over */
int player_near(NJS_POINT3 *pos, Float range);
/* 1 where the player in slot 0 lies in an upright cylinder about pos, and 0
 * otherwise. the cylinder has the radius radius, and reaches height above
 * and below pos. slot 0 must hold a player, unchecked */
int player_in_cylinder(NJS_POINT3 *pos, Float radius, Float height);

/* put the player in slot on path with a request. its work takes
 * PLAYER_WORK_REQUEST and the request in its submode, and its record path.
 * player_run_path, player_join_path and player_current_path do nothing for
 * an empty slot. the others need a player in slot, unchecked.
 *
 * player_run_path takes the player on at the place along path nearest it,
 * and raises its spd.x to 5 where it is below. player_boost_path adds 5 to
 * its spd.x. player_join_path takes it on at the nearest place, with no
 * speed. player_hang_path and player_ledge_path take it on at point, and
 * player_ledge_path turns it to ang, as player_set_angles turns it.
 * player_rail_path and player_rail_path_beneath take it on at distance along
 * path, with path_point the point path_segment_at finds there.
 * player_current_path gives it the path alone */
void player_run_path(int slot, struct path *path);
void player_boost_path(int slot, struct path *path);
void player_join_path(int slot, struct path *path);
void player_hang_path(int slot, struct path *path, int point);
void player_ledge_path(int slot, struct path *path, int point,
                       struct angles *ang);
void player_rail_path(int slot, struct path *path, Float distance);
void player_rail_path_beneath(int slot, struct path *path, Float distance);
void player_current_path(int slot, struct path *path);
/* which of the players in slots 0 and 1 follow path under
 * PLAYER_WORK_ON_PATH: 1 for slot 0, 2 for slot 1, 3 for both, or 0 for
 * neither */
int players_on_path(struct path *path);

/* the steps of a player along its path, which its record's path and
 * path_distance hold */

/* set the record's accel to gravity in the player's own axes. it adds
 * resist.x of the speed ahead, and the stick's push ahead by ground_accel.
 * without the stick it takes ground_drag, and a still player stays still where
 * still_grip holds the push. it changes nothing under PLAYER_WORK_NO_CONTROL */
void player_accel_on_path(struct task_work *work, struct player_motion *motion,
                          struct player *record);
/* add accel.x to the speed ahead. the speed stops where the step reverses it,
 * and where the step brings it from 0.1 or more to below it. the record's spd
 * keeps no other part. the motion's spd takes it turned into the world's axes,
 * within speed_limit and fall_limit */
void player_apply_path_speed(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record);
/* the movers. each clears PLAYER_WORK_ON_PATH and PLAYER_WORK_GROUNDED, and
 * steps path_distance by spd.x, ahead or back as the player faces along the
 * path. each turns the player's heading by up to 0x800 toward the way the path
 * runs there, in the plane of the player's up. the motion's aim_y takes the
 * heading. each then sets PLAYER_WORK_ON_PATH and PLAYER_WORK_ON_GROUND. off
 * the path's ends it sets neither, and the player moves by its motion's spd
 * instead.
 *
 * player_move_on_path puts the player at the path's place, and tilts it to the
 * path's ang_x and ang_z after its turn. player_move_on_path_tilted tilts it
 * before its turn. player_move_toward_path tilts the player by the path's
 * slope, and moves it toward the place by at most 1.3 times its speed */
void player_move_on_path(struct task_work *work, struct player_motion *motion,
                         struct player *record);
void player_move_on_path_tilted(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record);
void player_move_toward_path(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record);
/* keep path_distance 0.01 within the path's ends, or at 0 on a one-point
 * path. each returns TRUE while the player is on the path. the record's drive
 * is 1 while they move the player along it, and 0 at an end or off it.
 * player_slide_on_path also turns the player, and tilts it to the path's ang_x
 * and ang_z. player_shift_on_path moves the player ahead by spd.x alone, and
 * leaves its angles */
Bool player_slide_on_path(struct task_work *work, struct player_motion *motion,
                          struct player *record);
Bool player_shift_on_path(struct task_work *work, struct player_motion *motion,
                          struct player *record);

/* the look of the player in slot. player_look_at_task looks at target, with
 * offset in its axes, or none where offset is null. player_look_at_point
 * looks at pos in the world, and player_look_at_angles turns the look by
 * ang. each sets look_steps to steps, and does nothing for an empty slot or
 * a null target, pos or ang. player_clear_look sets look_mode to
 * PLAYER_LOOK_NONE and look_steps to 0, where the slot isn't empty */
void player_look_at_task(int slot, struct task *target, NJS_POINT3 *offset,
                         int steps);
void player_look_at_point(int slot, NJS_POINT3 *pos, int steps);
void player_look_at_angles(int slot, struct angles *ang, int steps);
void player_clear_look(int slot);

/* the past of the player in slot: its pos and ang as player_finish_step
 * recorded them, once a step. back counts the steps back from the latest, which
 * is 0, and wraps at 256. player_get_past copies them to pos and ang, and
 * player_set_past copies them from there, where each isn't null. each
 * returns TRUE, or FALSE for an empty slot, which copies nothing */
Bool player_get_past(int slot, int back, NJS_POINT3 *pos, struct angles *ang);
Bool player_set_past(int slot, int back, NJS_POINT3 *pos, struct angles *ang);

/* the slot of the player that task belongs to, or -1. a task with a parent
 * is taken for its parent. where the work's id is 1 to 4, the result is the
 * slot its extra's record holds. WORK_ID_PLAYER and WORK_ID_RIVAL lie in
 * that range. a parentless task whose work's id is WORK_ID_PLAYER_PROXY gives
 * its work's submode. other ids give -1, and so does a null task. the task
 * taken must have a work, unchecked */
int player_slot_of(struct task *task);
/* the character of the player whose task is task, or task's parent where it
 * has one. a parentless task whose work's id is WORK_ID_PLAYER_PROXY gives its
 * work's submode. other tasks give -1, and so does null */
int player_character_of(struct task *task);
/* the slot of the player whose work's pos is nearest to pos. it returns 0
 * where none is nearer than 10000000 */
int player_nearest(NJS_POINT3 *pos);
/* the first slot other than slot whose work's id is WORK_ID_PLAYER or
 * WORK_ID_RIVAL, or -1 */
int player_find_rival(int slot);
/* a rival's voice stops each voice that gives way to RIVAL_VOICE_PRIORITY,
 * then plays at that priority */
#define RIVAL_VOICE_PRIORITY 3
/* TRUE where the player in slot is under PLAYER_WORK_ON_GROUND with task as
 * its ground_task. FALSE otherwise, and for an empty slot */
Bool player_on_task(int slot, struct task *task);
/* where the player in slot has a bit of PLAYER_WORK_GROUNDED, store the
 * record's spd.x through speed and return TRUE. otherwise return FALSE and
 * store nothing, as for an empty slot */
Bool player_ground_speed(int slot, Float *speed);

/* the attack tests. each returns TRUE where a player attacks, as its character
 * and its work's mode say, and FALSE otherwise. other is the task a hit names,
 * and task the task hit.
 *
 * player_attacks_task is TRUE where other's work's id is WORK_ID_ATTACK. it is
 * FALSE where other is anything else but a player's task with a collision
 * record. task must have a work and a collision record, unchecked. the test
 * restarts the collision scans, then moves their place by
 * collision_next_player(task). other rests on task where task's record has
 * COLLISION_UNDER, and the player's stood_on is task or
 * collision_next_above(task, 0) gives other.
 *
 * Sonic or Shadow attacks in its combo, while task's record lacks
 * COLLISION_UNDER, and Sonic in PLAYER_MODE_SONIC_BOUNCE too. Knuckles or Rouge
 * attacks in its attack chains, where collision_next_player gave other and
 * other doesn't rest on task. it attacks too in its refused digs from frame 3
 * on, and in its dive. those need ground_task to be task, or else other to rest
 * on task where collision_next_player gave other */
Bool player_attacks_task(struct task *task, struct task *other);
/* TRUE where player_attacks_task is, and other's player has its character's
 * upgrade for it. Sonic needs UPGRADE_SONIC_FLAMERING, and Shadow
 * UPGRADE_SHADOW_FLAMERING. Knuckles needs UPGRADE_KNUCKLES_GROVES, and Rouge
 * UPGRADE_ROUGE_BOOTS. PLAYER_TAILS_MECH needs UPGRADE_TWALKER_HYPERCANNON,
 * and PLAYER_EGGMAN_MECH UPGRADE_EWALKER_BAZOOKA. other characters give
 * FALSE */
Bool player_breaks_task(struct task *task, struct task *other);
/* TRUE where the player whose task owns other's collision record attacks.
 * Sonic or Shadow attacks in a mode from PLAYER_MODE_SONIC_LIGHT_ATTACK to
 * PLAYER_MODE_SONIC_COMBO_RUNNING_3. Knuckles or Rouge attacks from
 * PLAYER_MODE_HUNTER_ATTACK_1 to PLAYER_MODE_HUNTER_RUN_ATTACK_3. any player
 * under PLAYER_INVINCIBLE attacks too. FALSE where other, its work or its
 * record is null. task is unused */
/* @hack type: task volatile, where a plain parameter would do */
Bool player_hit_attacks(struct task *volatile task, struct task *other);
/* TRUE where collision_next_player(task) gives Sonic's or Shadow's task, in
 * a mode from PLAYER_MODE_SONIC_COMBO_1 to
 * PLAYER_MODE_SONIC_COMBO_RUNNING_3 */
Bool player_touch_attacks(struct task *task);

/* the power-ups. each gives the player in slot its power-up with a new
 * task, unless it has it already or the task can't be had. then it plays
 * the voice for the record's kind. player_give_invincibility plays the
 * jingle invncibl.adx too. a barrier replaces the other barrier. nothing
 * happens for an empty slot. slot must be from 0 to 7, unchecked: a slot
 * above 7 is read past the players' tables */
void player_give_barrier(int slot);
void player_give_magnetic_barrier(int slot);
void player_give_invincibility(int slot);
/* start the task PlayerSpeedUp for the player in slot. a slot past 7, or one
 * from 1 to 7 without a work, is taken for slot 0. the task's first step
 * doubles the top_speed, ground_accel and still_grip of the record's constants,
 * and plays the jingle speedup.adx and the voice for the record's kind. after
 * 600 steps the task ends, and its destructor puts the three back. a second
 * task that starts while one runs leaves them doubled when both are done. a
 * task whose slot is empty ends on its next step */
void player_speed_up(int slot);
/* start the task MomentMuteki, which keeps PLAYER_INVINCIBLE set for the
 * player in slot on each of its next 120 steps. it then clears it and ends.
 * slot must be from 0 to 7, unchecked, as for the power-ups */
void player_moment_invincibility(int slot);
/* set or get the kind of what the player in slot holds. a byte for each
 * character keeps it, by the player's character. slot must hold a player,
 * unchecked */
void player_set_held_kind(int slot, int kind);
int player_get_held_kind(int slot);

/* player_pause clears the step and draw of slot's task, so that it neither
 * steps nor draws. player_resume puts back what player_pause took. each
 * does nothing for an empty slot */
void player_pause(int slot);
void player_resume(int slot);

/* hand the player in slot task to hold: request PLAYER_REQUEST_HAND, past
 * any waiting request, with task in the record's handed, unless handed
 * holds a task already. slot must hold a player, unchecked */
void player_hand_task(int slot, struct task *task);
/* where the player in slot holds a task, let it go: clear
 * PLAYER_WORK_HOLDING, clear WORK_HELD_BY_PLAYER in the held task's work
 * where it has one, and empty held. nothing happens for an empty slot */
void player_let_go(int slot);
/* move held, which the player whose work and record these are sets down,
 * clear of the player, level, along the way from the player's pos to its
 * own. it moves only where the center of held's second shape lies nearer
 * the player's pos than the reach: that shape's radius and the constants'
 * radius, or that radius and 5 for an Eggman. the center then lies the
 * reach from the player's pos, in x and z. held needs two shapes where it
 * has a collision record, unchecked, and nothing moves without one */
void player_set_down(struct task_work *work, struct task_work *held,
                     struct player *record);

/* give record room for PLAYER_TARGETS targets from syCalloc, and none listed
 * yet. a failed allocation leaves targets null. it is external only because SHC
 * folds a static of one call into player_register */
void player_targets_alloc(struct player *record);
/* offer players 0 and 1 a target: a sphere with its center at x, y, z.
 * the first that qualifies lists it until its player_finish_step. a player
 * qualifies with room for it, no lower than its center, within 120 of its
 * surface */
void player_add_target(Float x, Float y, Float z, Float radius);
/* steer a falling player onto a target, and return TRUE where it steers. of the
 * targets whose center isn't above the player, it takes the nearest, by
 * player_add_target's distance from its surface. a target more than 0x1800 off
 * the player's heading competes only while the player lies within its radius in
 * x and z. where such a target is the nearest, nothing steers. nothing steers
 * either for a nearest target the player was inside as it was listed. nor does
 * anything steer where the stick's heading lies more than 0x1800 off the
 * target.
 *
 * it follows the player's fall by its constants, for up to 120 steps, and
 * steers only a fall that ends within them. it adds a speed in x and z to the
 * motion's acc, spread over the fall's steps. each axis of it is held within
 * ground_accel. that speed moves the landing toward a point short of the
 * center. a player heading within 0x1000 of the target turns toward it by up to
 * 0x100. then player_accel_roll runs */
Bool player_steer_to_target(struct task_work *work,
                            struct player_motion *motion,
                            struct player *record);

/* add amount to the gauge of a mech in slot. the gauge stays from 0 to 10,
 * or to 12 with UPGRADE_EWALKER_EXTRASHIELD. other characters, and an empty
 * slot, are left as they are */
void player_add_gauge(int slot, Float amount);

/* turn vector in place, from the world's axes into those of work's angles,
 * or back for turn_to_world. turn_to_world turns about y by -ang.y, then
 * about x by ang.x, then about z by ang.z. each pushes a matrix on Ninja's
 * stack and pops it */
void turn_from_world(struct task_work *work, NJS_VECTOR *vector);
void turn_to_world(struct task_work *work, NJS_VECTOR *vector);

/* give event to the player of character. the player takes PLAYER_MODE_EVENT,
 * and player_stop stops it. its work loses PLAYER_WORK_BALL,
 * PLAYER_WORK_ATTACK and PLAYER_WORK_ON_PATH.
 *
 * where no record holds character, the character's maker makes its player
 * in slot n, where n is the count of slots that hold a task. the new player
 * is marked PLAYER_FOR_EVENT. character must then be one of the PLAYER_
 * characters, unchecked. the players there are must fill slots 0 to n - 1,
 * with n below 8, unchecked */
void player_start_event(int character, struct player_event *event);
/* end the event of the player of character, where a record holds
 * character. it kills the player where it is PLAYER_FOR_EVENT, or else
 * requests PLAYER_REQUEST_RELEASE. the record's event becomes null */
void player_end_event(int character);
/* set NJD_EVAL_HIDE in the first child of the objects that model_table
 * holds for slot's character: in its slots 21 to 24 for Sonic, 86 to 89
 * for Shadow, 156 to 159 for Knuckles, 186 and 187 for Rouge, or 224 to 227
 * for Tails. other characters are left as they are. slot must hold a
 * player, and each of those slots an object with a child, unchecked */
void player_hide_event_models(int slot);
/* draw the parts of the event of the player in slot at mini_event_frame,
 * then show again what player_hide_event_models hid. a Rouge draws only the
 * first two. a part draws with njCnkSimpleDrawShapeMotion where it has a shape,
 * with njCnkSimpleDrawMotion where it has only a motion, and else with
 * njCnkSimpleDrawObject. slot must hold a player whose event isn't null,
 * unchecked */
void player_draw_event_parts(int slot);

/* the speed step of a player on the ground. the record's spd gains its accel. a
 * speed is slow below 0.87 of trot_speed. a speed ahead that the step would
 * reverse stops at 0. without the stick, so does a slow one on ground whose
 * level is above 0.7. a slow speed across stops where the step left it alone,
 * or where the step brings it from faster to slow. deep water brakes the speed
 * ahead by brake_drag. the motion's spd then takes the record's, turned into
 * the world's axes. ground_normal tilts those axes on WORLD_UPRIGHT ground, but
 * in PLAYER_MODE_JUMP while the motion's spd.y isn't above 0. the motion's spd
 * is held within the constants' speed_limit and fall_limit. without the stick,
 * each part of it less than 0.02 from 0 becomes 0 */
void player_apply_accel(struct task_work *work, struct player_motion *motion,
                        struct player *record);
/* player_apply_accel for a player along its path. a speed is slow below
 * 0.01, at any level. the turn into the world takes the work's angles
 * alone */
void player_apply_path_accel(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record);
/* the speed step of a push or a pull. the pushed task is the record's held.
 * where that is null, it is the task of the hit the work's collision record
 * last kept. that hit must be there, unchecked. without ACTION_FLAG_400 from
 * player_action_flags, the player stops, as player_stop stops it. otherwise the
 * record's spd gains its accel. a speed ahead or across that the step would
 * reverse stops at 0. so does one that slows from 0.1 or more to below it. the
 * speed ahead stays within push_speed, or within 0.1 on STAGE_WILD_CANYON and
 * STAGE_WILD_CANYON_2P. the motion's spd takes the record's, turned into the
 * world's axes, and then within speed_limit and fall_limit. the pushed task's
 * motion takes that speed's x and z as its acc, with an acc.y of 0. it takes
 * them before the limits. a task without a motion takes nothing */
void player_apply_push(struct task_work *work, struct player_motion *motion,
                       struct player *record);
/* the record's spd takes its accel whole. the motion's spd takes the same,
 * turned into the world's axes, within speed_limit and fall_limit */
void player_apply_climb_accel(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record);
/* the record's spd gains its accel. a speed ahead below 1.5 gains 0.15
 * more, from 0 where it was below 0, and it never passes 3.5. the motion's
 * spd then takes it as player_apply_climb_accel says */
void player_apply_glide_accel(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record);
/* player_collide's results besides 0. a slide along a wall can leave another
 * value, as player.c's ball_slide_along_walls says */
#define PLAYER_BUMPED 1
#define PLAYER_CRASHED 2
#define PLAYER_RUN_STOPPED 3
/* the collision step of a player, which returns one of player_collide's
 * results, or 0. it clears wall_normal, and moves ground_flags to
 * last_ground_flags.
 *
 * it first meets the task whose shape last pushed the player at a side, and
 * wall_normal takes that touch's normal. a player whose spd.x is 15 or more
 * crashes and stops, at any shape but one of SHAPE_ID_NO_CRASH. one at
 * crash_speed or more that meets such a shape head on crashes too, and loses
 * its motion's x and z. any other touch is PLAYER_BUMPED. at running_speed or
 * more, a player runs round an object's sphere or cylinder that it runs into.
 * the shape must be larger than the player's first shape, and the player's
 * guard must hold off the shape's attack. at a box the player stops or slides
 * along. moving away from the shape, it is pushed back under ACTION_FLAG_400,
 * or steers off the shape, or slides along it.
 *
 * without such a task, a WORK_ID_STOP_RUN task that its record touched may
 * stop the player: PLAYER_RUN_STOPPED. the player must be on the ground and
 * not rising, at running_speed or more or under PLAYER_WORK_BALL. its heading
 * must lie within 0x3000 of the task's ang.y.
 *
 * last, it moves the player against the world: by player_collide_unmasked in
 * PLAYER_MODE_SWIM or under PLAYER_WORK_BALL, and else by
 * player_collide_world. it returns that step's result where it set none. a
 * player under PLAYER_WORK_ON_SHAPE that finds no ground in the world takes
 * its ground_normal from its angles, and drops by 0.5. where it isn't rising,
 * it loses the speed along its up, and otherwise PLAYER_WORK_ON_SHAPE */
int player_collide(struct task_work *work, struct player_motion *motion,
                   struct player *record);
#ifdef NONMATCHING
/* the y at which player_collide's slide along a wall starts its normal. the
 * image reads that y from a word of the stack that no path sets, and this
 * stands for the word where the frames of the image's code say what it holds:
 * each site that sets this names the value the image leaves there. it is 0
 * anywhere else, a normal that lies level. walker_step clears it as it ends,
 * so only a mech's step holds anything in it */
extern Float player_slide_y;
#endif
/* the collision step of a climbing player, which returns PLAYER_CRASHED,
 * PLAYER_BUMPED or 0. it clears wall_normal, and moves ground_flags to
 * last_ground_flags. a player riding its ground_task moves with the task's
 * force record for its slot. a shape may push the player at a side, as in
 * player_collide. it takes away the motion's x and z where the motion runs into
 * the touch's normal. that gives PLAYER_CRASHED at running_speed or more, or
 * else PLAYER_BUMPED. the work then loses PLAYER_WORK_GROUNDED.
 *
 * the ball, of half the player's height, starts at the player's middle moved
 * ahead by the motion, and sweeps along the motion and the push. a touched
 * object that moves, deflects players or can't be climbed ends the step, as
 * does a surface facing down. a player that takes no stick moves off such a
 * surface by half its normal. it moves off a wall the same way where the wall's
 * normal meets its up by 0.259 or less. the first such wall turns the motion
 * along it, at its own length, and a second ends the step. a falling player
 * then probes below its feet. a floor there ends the step, and the ball's
 * hit.normal takes the up's x and z and the floor's y.
 *
 * otherwise the player climbs a wall it touches just ahead along its up, whose
 * normal meets the up by more than 0.259. it takes the one whose point lies
 * furthest along the up, or of two at one point, the one whose normal lies
 * nearer the up. it takes the angles of the normal halfway between its up and
 * the wall's. it moves to the wall with the ride of the wall's task, or stops
 * within 0.3 of it. the work takes PLAYER_WORK_ON_GROUND, and the record the
 * wall's task, object and flags. ground_normal takes the ball's hit.normal.
 * where it finds no wall, a rising player probes above its head, and a floor
 * there ends the step. the step moves the player by nothing but the ride, the
 * pushes and the climb */
int player_collide_climb(struct task_work *work, struct player_motion *motion,
                         struct player *record);
/* set the record's spd from the motion's, turned into the player's own axes. a
 * player with a bit of PLAYER_WORK_GROUNDED is on the ground. there its spd.y
 * becomes 0. on WORLD_UPRIGHT ground the turn takes the tilt of ground_normal
 * in place of the work's ang.x and ang.z. a spd.x less than 0.01 from 0 becomes
 * 0, and so does a spd.z within 0.9 times the constants' ground_accel of 0.
 * neither does after GROUND_GRIP_LEAST or GROUND_GRIP_LOW ground. in the air,
 * spd.y is kept, and a spd.x or spd.z within 0.001 of 0 becomes 0.
 *
 * it masks the work's ang, and the motion's aim_x, aim_y and aim_z, to 16 bits.
 * level takes minus the inner product of world_gravity and ground_normal on the
 * ground, and 1 in the air or on WORLD_UPRIGHT ground. on STAGE_CHAO, spd.x
 * also becomes 0 below a margin that gravity and level set */
void player_take_speed(struct task_work *work, struct player_motion *motion,
                       struct player *record);
/* set the record's spd from speed, turned into the player's own axes. a spd.x
 * or spd.z within 0.001 of 0 becomes 0, and on the ground spd.y becomes 0.
 * the task the player's collision record last hit first moves by speed in the
 * world */
void player_take_push_speed(struct task_work *work, NJS_VECTOR *speed,
                            struct player *record);
/* zero record's spd and motion's spd, where each isn't null */
void player_stop(struct player_motion *motion, struct player *record);

/* turn the player with player_turn_toward toward the heading its stick asks
 * for, as player_read_stick gives it. the motion's aim_y takes the heading.
 * under GROUND_CLING in ground_flags, the heading comes instead from the x1 and
 * y1 of the player's pad. they are turned by the view of the player's camera,
 * and about ground_normal. there player_turn_at_speed turns the player toward
 * it ahead of player_turn_toward.
 *
 * nothing changes under PLAYER_WORK_NO_CONTROL, and a player under
 * PLAYER_WORK_ON_PATH follows its path instead. without the stick, a player
 * with a bit of PLAYER_WORK_GROUNDED turns down the slope it stands on. one in
 * the air keeps its heading */
void player_turn_to_stick(struct task_work *work, struct player_motion *motion,
                          struct player *record);
/* turn the player toward the heading its stick asks for, or toward its own
 * heading without the stick, by player_turn_at_speed, or by
 * player_turn_slowly's step of 0x30, or 0x38 for a WORK_ID_RIVAL work. the
 * motion's aim_y takes the heading. nothing changes under
 * PLAYER_WORK_NO_CONTROL */
void player_turn_to_stick_at_speed(struct task_work *work,
                                   struct player_motion *motion,
                                   struct player *record);
void player_turn_to_stick_slowly(struct task_work *work,
                                 struct player_motion *motion,
                                 struct player *record);
/* turn the motion's aim_y toward the way down the slope the player stands on.
 * where the way down lies behind the player, aim_y turns toward the way up. the
 * player turns toward aim_y by player_turn_toward. the way down is
 * world_gravity in the player's own axes. the player stands on a slope where
 * its y lies between -0.87 and 0. off a slope, aim_y takes the heading.
 *
 * it returns the cosine of the way down's heading from the player's, below 0
 * where the player turned up the slope. it returns 0, and changes nothing, for
 * a player faster than trot_speed whose unit motion meets world_gravity below
 * -0.86, as a steep rise does. off a slope it returns no value it sets, and
 * under stick_lock or on WORLD_UPRIGHT ground none at all, changing nothing */
Float player_turn_along_slope(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record);
/* slide the player down the ground it stands on under PLAYER_WORK_ON_GROUND,
 * or along world_gravity off it. the motion's spd gains its acc and 0.4 of
 * the constants' gravity along the slide, then loses resist.x of itself. the
 * record's spd takes the result in the player's own axes. the motion's spd is
 * held as player_apply_accel holds it. the player then heads along
 * its motion, which aim_y takes too. nothing changes under stick_lock */
void player_slide_downhill(struct task_work *work, struct player_motion *motion,
                           struct player *record);
/* tilt a player in the air back toward world_gravity_ang_x and
 * world_gravity_ang_z, by up to 0x800 a step. the tilt turns it about the
 * middle of its height. its speed in the world stays as it was. it leaves a
 * player with a bit of PLAYER_WORK_GROUNDED. it leaves too a player without
 * UPGRADE_UNK_15 whose speed passes its constants' full_speed */
void player_turn_upright(struct task_work *work, struct player_motion *motion,
                         struct player *record);

/* player_turn_toward, player_turn_on_course and player_turn_at_speed turn the
 * player toward heading, moving the work's ang.y the short way by at most a
 * step. each then mixes two speeds into the record's spd. one is spd as it was,
 * which turns with the player. the other is the speed the player had in the
 * world, in its new axes.
 *
 * player_turn_toward's step falls as heading nears. its spd keeps the least of
 * itself in the air or on the ground without the stick. it keeps the most with
 * the stick on ground nearer level. a grip_scale below 1 scales each share on
 * the ground. player_turn_on_course steps by 0x2000, and keeps the speed in the
 * world whole. player_turn_at_speed's step falls as spd.x passes full_speed,
 * and its spd keeps what player_turn_toward's keeps with the stick, unscaled by
 * grip_scale */
void player_turn_toward(struct task_work *work, struct player_motion *motion,
                        struct player *record, Angle heading);
void player_turn_on_course(struct task_work *work, struct player_motion *motion,
                           struct player *record, Angle heading);
void player_turn_at_speed(struct task_work *work, struct player_motion *motion,
                          struct player *record, Angle heading);

/* a step of a player's anim. it plays current at its mode's pace, and steps
 * counts its steps. where next isn't current, it changes to next. it blends
 * only where both use one model, next's blend_step isn't 1, and
 * PLAYER_ANIM_NO_BLEND is clear. a change between two PLAYER_ANIM_LOOP_PACE
 * animations starts the new one partway. it starts at the share of its frames
 * the old one had left, and a tenth more. the step in PLAYER_ANIM_NEW allocates
 * the blend's motion, unchecked, and PLAYER_ANIM_END frees it. a step in
 * PLAYER_ANIM_END needs that motion, unchecked. it must follow a step in
 * PLAYER_ANIM_NEW, and no other step in PLAYER_ANIM_END */
void player_anim_step(struct player_anim *anim);
/* end a blend at once: an anim in PLAYER_ANIM_BLEND takes current's own
 * mode, frees the blend's keys, and starts current's frame over. an anim in
 * any other mode is left as it is */
void player_anim_end_blend(struct player_anim *anim);

#endif
