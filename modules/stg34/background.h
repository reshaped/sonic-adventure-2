/* the background's task of Sonic's route, which draws stg34/scenery.h's
 * scenery */
#ifndef STG34_BACKGROUND_H
#define STG34_BACKGROUND_H

/* create the task, BgExec, whose draw is core_scenery_draw. while time runs,
 * its step turns the rotor and picks new values for core_flicker. it also
 * plays the machinery's sound where the camera can hear it */
void core_sonic_background_init(void);
/* rest the rotor at angle 0. create the colliders of the pillars, the fans
 * and the gate, and the entry's trigger. the trigger turns player 0's pad off
 * until a Sonic touches it */
void core_sonic_background_start(void);

#endif
