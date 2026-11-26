/* the three colors of many props in Death Chamber and Egg Quarters */
#ifndef STG25_CHAMBER_COLOR_H
#define STG25_CHAMBER_COLOR_H

/* the colors are blue, green and red, in the order each prop's tables and color
 * names take them */
#define CHAMBER_COLORS 3
#define CHAMBER_COLOR(work)                                                    \
    ((int)_builtin_fabsf((work)->scl.z) % CHAMBER_COLORS)

#endif
