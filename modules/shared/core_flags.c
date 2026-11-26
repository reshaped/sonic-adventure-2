#include <sg_xpt.h>

#include "shared/core_flags.h"

static Bool core_flags[CORE_FLAGS];

void core_flags_clear(void)
{
    int i;
    for (i = 0; i < CORE_FLAGS; ++i) {
        core_flags[i] = FALSE;
    }
}

void core_flag_set(int index)
{
    core_flags[index] = TRUE;
}

Bool core_flag_is_set(int index)
{
    return core_flags[index];
}
