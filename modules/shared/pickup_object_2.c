/* the pickup object of shared/pickup_object_2.h: pickup_object.c compiled a
 * second time. it takes the names below, and another hold angle */
#include "shared/pickup_object_2.h"

/* NOLINTBEGIN(readability-identifier-naming) */
#define pickup_object_init pickup_object_2_init
#define pickup_object_texlists pickup_object_2_texlists
#define pickup_object_texlist pickup_object_2_texlist
#define pickup_object_model pickup_object_2_model
/* NOLINTEND(readability-identifier-naming) */
#define PICKUP_OBJECT_HOLD_ANGLE 0x200

#include "shared/pickup_object.c" /* NOLINT(bugprone-suspicious-include) */
