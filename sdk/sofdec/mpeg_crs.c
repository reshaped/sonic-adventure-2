/* adx_crs.c built again as the AHX player's own critical section, under
 * mpeg_crs.h's names */
#include <mpeg_crs.h>

/* NOLINTBEGIN(readability-identifier-naming) */
#define ADXCRS_Init mpeg_crs_init
#define ADXCRS_Lock mpeg_crs_lock
#define ADXCRS_Unlock mpeg_crs_unlock
#define adxcrs_lvl mpeg_crs_depth
#define adxcrs_msk mpeg_crs_mask
/* NOLINTEND(readability-identifier-naming) */

#include "../sofdec/adx_crs.c" /* NOLINT(bugprone-suspicious-include) */
