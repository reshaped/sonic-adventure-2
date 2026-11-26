/* the motion of the shutters of core_shutter.c and stg22/shutter.c, which
 * task->motion holds. lift runs from 0 to 1 as the latch rises. opening then
 * runs from 0 when shut to 1 when open. CLOSING runs both back. a panel stands
 * opening times its _TRAVEL along its track, moved by its track's offset. once
 * the door stops, each offset springs toward 0 and bounces off it */
#ifndef SHARED_SHUTTER_MOTION_H
#define SHARED_SHUTTER_MOTION_H
#include <sg_xpt.h>

enum shutter_trigger { BY_SWITCH, BY_ENEMIES, BY_DYNAMITE, BY_PROXIMITY };
enum shutter_mode {
    CLOSED,
    UNLOCKING,
    LIFTING,
    OPENING,
    OPEN,
    CLOSING,
    SETTLING
};
/* UNLOCKING ends on the step that takes the work's timer past UNLOCK_STEPS,
 * and SETTLING on the step that takes it past SETTLE_STEPS */
#define UNLOCK_STEPS 15
#define SETTLE_STEPS 20
enum track { LEFT_TRACK, RIGHT_TRACK, TOP_TRACK, TRACKS };

struct shutter_motion {
    Float closed_light;
    Float open_light;
    Float opening;
    Float lift;
    Float offset[TRACKS];
    Float velocity[TRACKS];
};

#define MOTION(task) ((struct shutter_motion *)(task)->motion.p)
#define LEFT_TRAVEL 30.5F
#define RIGHT_TRAVEL 95.0F
#define TOP_TRAVEL 55.0F
#define LATCH_TRAVEL 20.0F
#define START_SOUND 6
#define STOP_SOUND 7
#define SHUTTER_DARK 0xff101010
#define SHUTTER_LIT 0xffffffff
#define TRANSLUCENT 0x70ffffff
/* the stop, go and main lamps blink by blink_level. each takes its own
 * _PERIOD, _REST and _FADE */
#define STOP_BLINK_PERIOD 38
#define STOP_BLINK_REST 20
#define STOP_BLINK_FADE 6
#define GO_BLINK_PERIOD 14
#define GO_BLINK_REST 10
#define GO_BLINK_FADE 3
#define MAIN_BLINK_PERIOD 30
#define MAIN_BLINK_REST 15
#define MAIN_BLINK_FADE 5

/* each track's spring is SPRING_INCREMENT stiffer than the one before. a track
 * keeps BOUNCE of its speed as its offset bounces off 0 */
#define SPRING_INCREMENT 0.02F
#define BOUNCE 0.6F

/* a track's next velocity. its spring pulls the offset back with a stiffness
 * of 0.2 plus increment times the track's index. the velocity keeps 0.983 of
 * itself a step */
#define SPRING_VELOCITY(motion, track, increment)                              \
    (-((motion)->offset[(track)] * (0.2F + ((track) * (increment)))) +         \
     ((motion)->velocity[(track)] * 0.983F))

/* move node by dx along x, where dx is its panel's place on its track.
 * MOVE_LATCH then adds lift times LATCH_TRAVEL, after dx. the latch and its
 * lamps move with the right panel */
#define MOVE_X(node, dx)                                                       \
    njTranslate(NULL, (node)->pos[0] + (dx), (node)->pos[1], (node)->pos[2])
#define MOVE_LATCH(node, dx, lift)                                             \
    njTranslate(NULL, ((node)->pos[0] + (dx)) + ((lift) * LATCH_TRAVEL),       \
                (node)->pos[1], (node)->pos[2])

#endif
