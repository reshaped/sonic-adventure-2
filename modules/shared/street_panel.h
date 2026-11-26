/* the street's panel, PANEL in Mission Street's object list: a model that
 * stands still */
#ifndef SHARED_STREET_PANEL_H
#define SHARED_STREET_PANEL_H
#include <ninja.h>

struct task;

/* the panel's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *street_panel_texlists[];

/* the object list's step for PANEL. it makes task a model object of
 * shared/common_object.h under OBJECT_MAY_LEAVE, drawn as the stage's land
 * draws. task needs a work, whose pos places the panel and whose ang.y turns
 * it */
void street_panel_init(struct task *task);

#endif
