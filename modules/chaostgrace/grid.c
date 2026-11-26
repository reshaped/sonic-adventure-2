#include "chao/race_record.h"
#include "chaostgrace/grid.h"

struct race_grid race_grids[RACE_RECORD_TRACKS] = {
#include "chaostgrace/grid/race_grids.inc"
};
