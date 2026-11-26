/* Prison Lane's doors. CAGE, CAGEB, CAGEC and CAGED open once a group of
 * enemies is beaten. WALL's gate drops shut once no elevator waits near it.
 * each keeps the record below, and bounces at its end alike */
#ifndef STG09_DOOR_H
#define STG09_DOOR_H
#include <sg_xpt.h>

/* a door's record, in task->motion. travel is how far the door stands open,
 * 0 when shut. speed is travel's change a step while the door bounces */
struct door {
    Float travel;
    Float speed;
};
#define DOOR(task) ((struct door *)(task)->motion.p)

/* a door bounces at the end of its travel. as it arrives, its speed gains
 * DOOR_KICK. at each step after, travel gains the speed. the speed then
 * becomes DOOR_DAMPING of itself plus DOOR_SPRING times the end less travel.
 * travel that reaches or passes the end stops there, and the speed points away
 * from it */
#define DOOR_KICK 0.5F
#define DOOR_DAMPING 0.96F
#define DOOR_SPRING 0.4F

/* each door but WALL's gate plays DOOR_MOVE_SOUND each step it moves, and
 * DOOR_STOP_SOUND as it reaches its end. each _UPDATES is sound_play_at_for's
 * duration, in sound updates */
#define DOOR_MOVE_SOUND 0x1020
#define DOOR_MOVE_UPDATES 3
#define DOOR_STOP_SOUND 0x1021
#define DOOR_STOP_UPDATES 60
#define DOOR_SOUND_PRIORITY 1
#define DOOR_SOUND_VOLUME 100

/* the dust_short_rect a door raises as it reaches its end. each door's
 * source names the half width, DUST_HALF_WIDTH */
#define DOOR_DUST_HALF_DEPTH 5.0F
#define DOOR_DUST_SCALE 6.0F
#define DOOR_DUST_DENSITY 1.0F
#define DOOR_DUST_SPEED 0.5F

/* the placement's state a door's destructor sets where the door is at its
 * end. that end is open, but shut for WALL's gate. a door whose placement's
 * state isn't 0 starts at its end */
#define DOOR_STATE_DONE 1

#endif
