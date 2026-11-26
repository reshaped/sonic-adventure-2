/* the camera state that outlives each camera task and each stage. it holds
 * the views, the current state, each camera's mode stack and the editor's
 * record */
#include <sg_xpt.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "camera/editor.h"

struct camera_place *camera_views[CAMERAS];
Sint32 camera_active_count;
int camera_current_index;
struct camera_state camera_state;
struct camera_stack camera_stacks[CAMERAS];
struct camera_editor camera_editor;
