/* the places of the golem's sensors and points, as golem.h declares them */
#include <ninja.h>

#include "boss_golem/golem.h"
#include "boss_golem/head_parts.h"
#include "shared/golem.h"

NJS_MATRIX golem_sensor_matrices[GOLEM_SENSORS];
NJS_MATRIX golem_sensor_bases[GOLEM_SENSORS];
NJS_MATRIX golem_head_matrices[GOLEM_HEAD_PARTS];
NJS_POINT3 golem_hand_a_pos;
NJS_POINT3 golem_hand_b_pos;
NJS_POINT3 golem_flash_pos;
