/* the editor's task and its pages' settings. a setting keeps its value from
 * one opening of a page to the next */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/editor.h"

struct task *garden_editor = NULL;
Sint32 editor_page_count = 0;
Angle editor_growth_phase = 0;
Sint32 editor_camera_index = 0;
Sint32 editor_growth_graph = 0;
Sint32 editor_motion_index = 0;
Float editor_motion_speed = 1.0F;
Sint32 editor_sound_id = 0;
Sint32 editor_volume = 0;
Angle editor_marker_phase = 0;
