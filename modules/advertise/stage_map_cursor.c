/* the stage under the stage map's cursor, for the stage map's menu */
#include <sg_xpt.h>

#include "advertise/stage_map.h"
#include "advertise/stage_map_cursor.h"

Sint32 stage_map_cursor_stage(void)
{
    return stage_map_cells[stage_map_work.cursor].stage;
}
