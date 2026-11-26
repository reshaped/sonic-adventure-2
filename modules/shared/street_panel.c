/* the street's panel, PANEL in Mission Street's object list: a model that
 * stands still. it draws as the stage's land does */
#include <ninja.h>

#include "shared/common_object.h"
#include "shared/street_panel.h"
#include "shared/street_panel/text.inc"

#include "shared/mission_street/street_panel_art.h"

NJS_TEXLIST *street_panel_texlists[2] = {
#include "shared/street_panel/street_panel_texlists.inc"
};

void street_panel_init(struct task *task)
{
    model_object_init(task, &street_panel_texlist, &street_panel_model, NULL, 0,
                      OBJECT_AS_LAND | OBJECT_MAY_LEAVE);
}
