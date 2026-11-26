/* the menu of menu_textures_select's group 17, which nothing reaches. no
 * mode of menus_step runs its step, and no code calls its start or its
 * hide */
#include <sg_xpt.h>

#include "advertise/menu_textures.h"
#include "advertise/menus.h"

/* menu_textures_select has no case for group 17, so it loads only the share
 * data's work */
#define TEXTURE_GROUP 17

/* menu_control.step, while the menu runs */
enum group_17_menu_step { STEP_IDLE };

/* nothing calls these. none is static, since SHC leaves out a static
 * function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void group_17_menu_start(void);
/* hide screen_effect.h's effect */
void group_17_menu_hide(void);
Bool group_17_menu_step(void);
/* NOLINTEND(misc-use-internal-linkage) */

void group_17_menu_start(void)
{
    menu_textures_select(TEXTURE_GROUP);
    menu_control.step = STEP_IDLE;
}

void group_17_menu_hide(void)
{
    menu_works.work_17->shown = FALSE;
    menu_works.work_17->unk_04 = 0;
}

Bool group_17_menu_step(void)
{
    /* @hack redundant: a switch on the step whose one case does nothing */
    switch (menu_control.step) {
    case STEP_IDLE:
        break;
    }
    return FALSE;
}
