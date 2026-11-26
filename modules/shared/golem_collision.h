/* the collision models of the Egg Golem's nine sensors.
 * golem_head_model_collision belongs to the model that rides the golem's head
 * in the hero story's fight */
#ifndef SHARED_GOLEM_COLLISION_H
#define SHARED_GOLEM_COLLISION_H

#include <ninja.h>

extern NJS_OBJECT golem_sensor_low;
extern NJS_OBJECT golem_sensor_head;
extern NJS_OBJECT golem_sensor_upper_arm_a;
extern NJS_OBJECT golem_sensor_forearm_a;
extern NJS_OBJECT golem_sensor_hand_a;
extern NJS_OBJECT golem_sensor_upper_arm_b;
extern NJS_OBJECT golem_sensor_forearm_b;
extern NJS_OBJECT golem_sensor_hand_b;
extern NJS_OBJECT golem_sensor_full;
extern NJS_OBJECT golem_head_model_collision;

#endif
