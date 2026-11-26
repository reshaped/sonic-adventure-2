/* the places of the golem's sensors and points, as golem.h declares them */
#include <ninja.h>

#include "boss_golem_e/golem.h"
#include "shared/golem.h"

NJS_MATRIX golem_e_sensor_matrices[GOLEM_SENSORS];
NJS_MATRIX golem_e_sensor_bases[GOLEM_SENSORS];
NJS_POINT3 golem_e_hand_a_pos;
NJS_POINT3 golem_e_hand_b_pos;
NJS_POINT3 golem_e_head_pos;
