/* the shutter, SHELTV in Iron Gate's object list. its door drops shut as a
 * player comes near. four panels then light on it, and the door breaks once
 * attacks have hit each. the shutters' manager task keeps the stage's flags */
#ifndef STG11_SHUTTER_H
#define STG11_SHUTTER_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the shutter, its frame, its dust
 * and its flash, ended by null */
extern NJS_TEXLIST *shutter_texlists[];

/* the shutter's door and frame, and their texture lists. the side shutter and
 * the missile shutters draw this frame, and the side shutter's break throws
 * this door */
extern NJS_TEXLIST shutter_texlist;
extern NJS_TEXLIST shutter_frame_texlist;
extern NJS_CNK_OBJECT shutter_frame_object;
/* @hack type: the door's node declared as an array, where the node alone
 * would do */
extern NJS_CNK_OBJECT shutter_door_object[];

/* the object list's step for SHELTV. it makes the shutters' manager first, as
 * shutter_manager_create does. where task's placement is used, it then sets
 * SHUTTER_FLAG_BROKEN(n) and the fog's flag the break below would set. it sets
 * them whether or not iron_gate_background exists. it does nothing more where
 * task leaves at its placement's range. where the heap can't give its record,
 * it does nothing, and the next step tries again. otherwise it gives task the
 * shutter's step, draw and destructor, and a collision record of kind
 * COLLISION_OBJECT. the record's five boxes are the sill, the door, the two
 * posts and the lintel. where the heap has no room for it, the shutter goes on
 * without one.
 *
 * task needs a work, whose pos places the shutter and whose ang.y turns it. the
 * low byte of ang.x, modulo 5, is the shutter's number n. bit 8 of ang.x makes
 * the door shut upward. bits 12 and up of ang.x, modulo 3, are the fog's kind.
 * kind 0 leaves the fog alone, 1 turns it red and 2 clears it. the door starts
 * to shut once a player comes within scl.z + 100 of the pos. it drops to half
 * shut by scl.x + 0.05 a step. it waits there ang.z steps past the step it
 * arrives in. it then drops by scl.y + 0.05 a step.
 *
 * from that drop until the break, a child of task stops a walker's lock-on
 * through the door. a shutter that its placement's state starts shut gets no
 * such child. once shut, the door shows four panels in turn. an attack
 * can hit a panel only while the nearest player stands on the door's +z side.
 * the door breaks once attacks have hit all four, which scores 20 points. a
 * placed shutter plays shutter n's voice as it breaks, where
 * SHUTTER_FLAG_BROKEN(n) is clear. it then sets the flag. as it breaks, a
 * placed shutter of fog kind 1 sets its pair's _RED_ flag. one of kind 2 sets
 * its pair's _CLEAR_ flag. each does so only where iron_gate_background exists.
 *
 * the placement's state keeps the work's mode from one task to the next. the
 * destructor marks a broken shutter's placement used. a shutter whose placement
 * is used starts broken */
void shutter_init(struct task *task);

/* the stage's flags, bits 0 to 31 of a word the shutters' manager keeps. bit
 * must not be negative. for a bit below 32, each call first makes the manager
 * where there is none, as shutter_manager_create does. shutter_flag_test
 * returns whether bit is set. it returns TRUE for a bit of 32 or more, and
 * where the manager can't be made. shutter_flag_set sets bit. it does nothing
 * for a bit of 32 or more, or where the manager can't be made */
Bool shutter_flag_test(int bit);
void shutter_flag_set(int bit);
/* make the manager, a task at level 3 named ShutManTask, where there is none.
 * its step does nothing. where the heap can't give the task, there stays
 * none. the manager's flags start clear, and go with its task */
void shutter_manager_create(void);

/* the flags. SHUTTER_FLAG_OPENING_VOICE records that the stage's opening voice,
 * SHUTTER_OPENING_VOICE, has played or is no longer due. the shutter numbered n
 * sets SHUTTER_FLAG_BROKEN(n) as it breaks. the background turns the fog red
 * while a pair's _RED_ flag is set and its _CLEAR_ flag isn't. shutters
 * numbered 0 and 1 set the _A pair's flags, and the others the _B pair's */
#define SHUTTER_FLAG_OPENING_VOICE 0
#define SHUTTER_FLAG_BROKEN(n) (1 + (n))
#define SHUTTER_FLAG_RED_B 28
#define SHUTTER_FLAG_CLEAR_B 29
#define SHUTTER_FLAG_RED_A 30
#define SHUTTER_FLAG_CLEAR_A 31
/* the stage's opening voice. shutter n's voice follows it, at
 * SHUTTER_OPENING_VOICE + 1 + n */
#define SHUTTER_OPENING_VOICE 1909

/* the door, as the shutter, the side shutter and the missile shutters each
 * move it. under SHUTTER_REVERSED, bit SHUTTER_REVERSED_SHIFT of the work's
 * ang.x, the door shuts the other way. it waits SHUTTER_WAIT_STEPS at half
 * shut */
#define SHUTTER_REVERSED_SHIFT 8
#define SHUTTER_REVERSED(work) (((work)->ang.x >> SHUTTER_REVERSED_SHIFT) & 1)
#define SHUTTER_WAIT_STEPS(work) ((Uint16)(work)->ang.z)
/* how far the door stands open as the shutter starts, and at half shut */
#define SHUTTER_OPEN_FULL 50.0F
#define SHUTTER_OPEN_HALF 25.0F
#define SHUTTER_NEAR_MARGIN 100.0F
#define SHUTTER_MIN_SPEED 0.05F
/* the bounce: a kick as the door shuts, the spring and the damping */
#define SHUTTER_SHUT_KICK 0.5F
#define SHUTTER_BOUNCE_STIFFNESS 0.4F
#define SHUTTER_BOUNCE_DAMPING 0.96F
/* the door's sounds, three of the stage's sound list. the shutting sound
 * plays at SHUTTER_SHUTTING_PRIORITY, and the others at
 * SHUTTER_SOUND_PRIORITY */
#define SHUTTER_START_SOUND 8
#define SHUTTER_HALF_SOUND 9
#define SHUTTER_SHUTTING_SOUND 11
#define SHUTTER_SHUTTING_UPDATES 20
#define SHUTTER_SOUND_PRIORITY 0
#define SHUTTER_SHUTTING_PRIORITY 1
#define SHUTTER_SOUND_VOLUME 0
/* the dust as the door shuts */
#define SHUTTER_DUST_HALF_X 30.0F
#define SHUTTER_DUST_HALF_Z 5.0F
#define SHUTTER_DUST_SCALE 6.0F
#define SHUTTER_DUST_DENSITY 1.0F
#define SHUTTER_DUST_SPEED 0.5F

#endif
