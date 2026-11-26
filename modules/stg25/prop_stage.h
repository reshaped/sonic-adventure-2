/* the fire props' and the treasures' sources build for Death Chamber, or
 * under EGG_QUARTERS for Egg Quarters. STAGE_PROP(name) names that stage's
 * own helper, chamber_name or quarters_name */
#ifndef STG25_PROP_STAGE_H
#define STG25_PROP_STAGE_H
#ifdef EGG_QUARTERS
#include "stg26/flame_props.h"
#include "stg26/quarters_util.h"
#define STAGE_PROP(name) quarters_##name
#else
#include "stg25/chamber_util.h"
#include "stg25/death_chamber.h"
#define STAGE_PROP(name) chamber_##name
#endif
#endif
