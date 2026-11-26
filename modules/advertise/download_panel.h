/* the download panel, the task downloadExec, which shows nothing. the
 * download menu shows kart_race_panel.h's panel instead */
#ifndef ADVERTISE_DOWNLOAD_PANEL_H
#define ADVERTISE_DOWNLOAD_PANEL_H
#include <sg_xpt.h>

#include "advertise/menus.h"

/* menu_works.work_14 */
extern struct download_panel_work download_panel_work;

/* create the task, downloadExec, and clear the whole work, its loaded flag
 * too */
void download_panel_create(void);
/* clear the work but its loaded flag */
void download_panel_reset(void);
/* the panel has no textures to load or release. download_panel_work.loaded
 * stays 0 */
void download_panel_load(void);
void download_panel_release(void);

#endif
