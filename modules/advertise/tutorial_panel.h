/* the tutorial's panel: a model over a backdrop, the list of lessons, and a
 * board of pictures. its task draws the panel as tutorial.h's menu sets the
 * work */
#ifndef ADVERTISE_TUTORIAL_PANEL_H
#define ADVERTISE_TUTORIAL_PANEL_H
#include "advertise/menus.h"
#include "system/menu_snapshot.h" /* IWYU pragma: export */

/* how the model moves while the work's shown is 1, as its model_mode says */
enum tutorial_model_mode {
    TUTORIAL_MODEL_HOME, /* to its first stop, to bob facing the camera */
    TUTORIAL_MODEL_SWIM /* from stop to stop, facing the way it swims */
};

/* the work, menu_works.work_16 */
extern struct tutorial_work tutorial_work;

/* create the task, tutorialExec, with the work cleared, the panel hidden and
 * the model at its first stop. the task's destructor releases the textures */
void tutorial_panel_create(void);
/* load the panel's textures and each lesson's pictures, or release them,
 * and set loaded and pictures_loaded to match. neither does anything where
 * loaded matches already. the load skips a lesson whose pictures_loaded is
 * 1 */
void tutorial_panel_load(void);
void tutorial_panel_release(void);

#endif
