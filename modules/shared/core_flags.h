/* Cannon's Core's flags, which each of its routes clears as it starts. a set
 * flag marks a message the messenger has given, which then plays no more.
 * index is from 0 to CORE_FLAGS - 1, unchecked */
#ifndef SHARED_CORE_FLAGS_H
#define SHARED_CORE_FLAGS_H
#include <sg_xpt.h>

#define CORE_FLAGS 2

void core_flags_clear(void);
void core_flag_set(int index);
Bool core_flag_is_set(int index);

#endif
