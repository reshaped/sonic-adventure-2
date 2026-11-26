/* the missile shutters, LSHELTH and LSHELTV in Iron Gate's object list. a
 * missile shutter's door shuts in two stages once a player comes near. a
 * missile's landing hurries the door shut, and then bursts it. LSHELTH's door
 * slides along x, and LSHELTV's along y */
#ifndef STG11_MISSILE_SHUTTER_H
#define STG11_MISSILE_SHUTTER_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* a missile shutter's modes, in its work's mode. the door shuts to half, and
 * then on to shut. the placement's state keeps the mode from one task to the
 * next */
enum missile_shutter_mode {
    MODE_OPEN,
    MODE_HALF,
    MODE_SHUTTING,
    MODE_SHUT,
    MODE_BURST
};

/* a missile shutter's record, in task->motion. open is how far the door stands
 * open, 0 when shut. speed is open's change a step while the door bounces shut.
 * hit is the shutter's target in missile_targets, which a missile of its id
 * points at its place */
struct missile_shutter {
    Float open;
    Float speed;
    NJS_POINT3 *hit;
};

/* TRUE once a missile shutter has begun to shut. while it is TRUE, every open
 * missile shutter begins to shut. each missile shutter's init sets it to
 * FALSE */
extern Bool missile_shutters_shutting;

/* LSHELTV's door, which the bursts of both shutters throw */
extern NJS_TEXLIST missile_shutter_v_door_texlist;
extern NJS_CNK_OBJECT missile_shutter_v_door_object[];

/* the texture lists for texture_load_shared of each shutter's door, frame, dust
 * and flash, ended by null */
extern NJS_TEXLIST *missile_shutter_h_texlists[];
extern NJS_TEXLIST *missile_shutter_v_texlists[];

/* the object list's steps for LSHELTH and LSHELTV. each does nothing where task
 * leaves at its placement's range. where the heap can't give its record, it
 * does nothing, and the next step tries again. otherwise it gives task the
 * shutter's step, draw and destructor, and sets missile_shutters_shutting to
 * FALSE. it gives task a collision record of kind COLLISION_OBJECT. the
 * record's five boxes are the sill, the door, the two posts and the lintel.
 * where the heap has no room for it, the shutter goes on without one.
 *
 * task needs a work, whose pos places the shutter and whose ang.y turns it. the
 * low byte of ang.x is the shutter's id. bit 8 of ang.x opens the door the
 * other way along its axis. a player within scl.z + 100 of the pos starts the
 * door shutting. so does missile_shutters_shutting while it is TRUE. the door
 * shuts to half by scl.x + 0.05 a step. it waits there ang.z steps past the
 * step it arrives in. it then shuts by scl.y + 0.05 a step.
 *
 * the shutter is a target in missile_targets for the missiles of its id. its
 * target's id has bit 31 set until the door shuts. LSHELTH's init adds the
 * target. LSHELTV's adds it only where task has a placement. as the door
 * shuts, each shutter's step adds the target anew, without bit 31. a missile's
 * landing takes effect after a delay that grows with the missile's distance. it
 * starts an open door shutting. it closes the door to half shut at once. once
 * the wait there is over, it shuts the door at once. the door then bursts,
 * which scores 20 points.
 *
 * LSHELTV's shutter starts camera_path_init for player 0 as its door starts to
 * shut. it starts none while the LSHELTV that started the last path still
 * lives */
void missile_shutter_h_init(struct task *task);
void missile_shutter_v_init(struct task *task);

#endif
