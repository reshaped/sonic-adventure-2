/* the kinds of object that the set files of the carrier stages, STG12, STG15
 * and STG20, name */
#ifndef SHARED_CARRIER_OBJECTS_H
#define SHARED_CARRIER_OBJECTS_H
#include "object/set.h"

#define WEAPONS_BED_KIND_COUNT 86
#define WEAPONS_BED_2P_KIND_COUNT 83
#define CARRIER_DUEL_KIND_COUNT 56

#ifdef CARRIER_DUEL
#define CARRIER_KIND_COUNT CARRIER_DUEL_KIND_COUNT
#elif defined(CARRIER_2P)
#define CARRIER_KIND_COUNT WEAPONS_BED_2P_KIND_COUNT
#else
#define CARRIER_KIND_COUNT WEAPONS_BED_KIND_COUNT
#endif
extern struct object_kind carrier_kinds[CARRIER_KIND_COUNT];
#endif
