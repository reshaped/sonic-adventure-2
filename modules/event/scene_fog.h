/* the fog of each of an event's scenes, as its fog files set it */
#ifndef EVENT_SCENE_FOG_H
#define EVENT_SCENE_FOG_H
#include <sg_xpt.h>

#include "shared/fog.h"

#define EVENT_SCENE_FOGS 30

/* while event_editing_fog is TRUE, event_fog_select applies
 * event_editor_fog in place of a scene's setting */
extern struct fog_setting event_editor_fog;
extern Bool event_editing_fog;

/* clear the editor's setting and every scene's, start the fog with scene 0's,
 * and stop editing. event_fog_stop stops the fog */
void event_fog_start(void);
void event_fog_stop(void);
/* read e<event>_<scene>.fog into the settings of scenes 0 to scenes - 1. a
 * scene whose file is missing keeps the setting it had. that is the one an
 * earlier event read, or else the clear one event_fog_start gave it */
void event_fog_load(int event, int scenes);
/* apply scene's setting from the fog task's next step. scene must lie below
 * EVENT_SCENE_FOGS, which the port checks */
void event_fog_select(int scene);

#endif
