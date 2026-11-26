/* the depth of the editor's camera stack when it opened, and the pages
 * registered with it. closing the editor pops the stack back to that depth */
#include <sg_xpt.h>

#include "chao/editor.h"

Sint32 editor_entry_camera_depth;
struct chao_editor_page editor_pages[EDITOR_PAGE_CAPACITY];
