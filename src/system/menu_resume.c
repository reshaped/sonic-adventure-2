/* the menus' state that the main executable keeps, which lasts while
 * ADVERTISE.PRS isn't loaded */
#include "main.h"
#include "system/menu_snapshot.h"

int menu_resume_target = 0;
struct menu_snapshot menu_snapshot = {0};
