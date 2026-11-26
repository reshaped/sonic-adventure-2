/* Green Hill's Batabata, beeton.c under BATABATA with its own models. where
 * Beeton stops to fire, it swoops along a parabola through its home */
#define BATABATA
#define FLIER_DEBRIS_SLOTS 5
#define FLIER_ACTION_COUNT 2
#define FLIER(name) green_hill_batabata_##name
#include "stg31/beeton.c" /* NOLINT(bugprone-suspicious-include) */
