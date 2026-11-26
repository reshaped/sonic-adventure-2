#include <sg_xpt.h>
#include "chao/garden.h"
#include "chao/garden_state.h"

Bool garden_bad_file = FALSE;
Bool garden_file_loaded = FALSE;
Bool garden_needs_first_save = FALSE;
Bool garden_save_on_exit = FALSE;
int garden_previous = CHAO_AREA_NONE;
int garden_current = CHAO_AREA_NONE;
int garden_next = CHAO_AREA_NONE;
